/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Implementation for MPX My Videos Collection Move, Copy and Delete operations.*
*/




// INCLUDE FILES
#include <e32cmn.h>
#include <s32mem.h>
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmessagecontainerdefs.h>
#include <vcxmyvideosuids.h>
#include <drmutility.h>
#include <bautils.h>
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideoscollection.hrh"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosdownloadutil.h"
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideoscategories.h"
#include "vcxmyvideosmessagelist.h"
#include "vcxmyvideosasyncfileoperations.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations* CVcxMyVideosAsyncFileOperations::NewL(
    CVcxMyVideosCollectionPlugin& aCollection )
    {
    MPX_FUNC("CVcxMyVideosAsyncFileOperations::NewL");

    CVcxMyVideosAsyncFileOperations* self = new (ELeave) CVcxMyVideosAsyncFileOperations(
            aCollection );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations::~CVcxMyVideosAsyncFileOperations()
    {
    MPX_FUNC("CVcxMyVideosAsyncFileOperations::~CVcxMyVideosAsyncFileOperations");
        
    iOperationIdArray.Close();
    iOperationResult.Close();
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations::CVcxMyVideosAsyncFileOperations( CVcxMyVideosCollectionPlugin& aCollection )
: iCollection( aCollection )
    {
    MPX_FUNC("CVcxMyVideosAsyncFileOperations::CVcxMyVideosAsyncFileOperations");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::ConstructL ()
    {
    MPX_FUNC("CVcxMyVideosAsyncFileOperations::ConstructL");
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DeleteVideoL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::DeleteVideoL( TUint32 aMdsId, TBool aForce )
    {
    MPX_FUNC("CVcxMyVideosAsyncFileOperations::DeleteVideoL");
            
    TInt pos;
    CMPXMedia* videoInCache = iCollection.iCache->FindVideoByMdsIdL( aMdsId, pos );
                    
    if ( !videoInCache )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: delete failed, MPX item not found from cache.");
        User::Leave( KErrNotFound );
        }
    
    TUint32 downloadId = TVcxMyVideosCollectionUtil::DownloadIdL( *videoInCache );            
    if ( downloadId )
        {
        TVcxMyVideosDownloadState dlState =
                TVcxMyVideosCollectionUtil::DownloadStateL( *videoInCache );
                        
        if ( aForce )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: aForce is ETrue, the video will be deleted even if download is ongoing");
            }
        else if ( dlState != EVcxMyVideosDlStateNone )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: delete failed since there is download going on.");
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: use download cancel instead.");
            User::Leave( KErrArgument );
            }
        else
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: download id was != 0 but download state was EVcxMyVideosDlStateNone");
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: -> we delete the video, it is prob gone from Download Manager already.");
            }        
        }
            
    if ( !videoInCache->IsSupported( KMPXMediaGeneralUri ) )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations::DeleteVideoL() KMPXMediaGeneralUri attribute missing -> leaving");
        User::Leave( KErrArgument );
        }
    
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: trying to delete: %S",
            &videoInCache->ValueText( KMPXMediaGeneralUri ));
    TInt err = iCollection.iFs.Delete( videoInCache->ValueText( KMPXMediaGeneralUri ) );
        
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: file delete failed: %d", err );

        if ( err == KErrNotFound || err == KErrPathNotFound )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: file was not found, trying to remove mds object anyways");
            TInt result = iCollection.iMyVideosMdsDb->RemoveVideo( aMdsId );
            if ( result == KErrNone )
                {
                MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d removed from mds", aMdsId); 
                //mds events will clean cache
                }
            else
                {
                if ( result == KErrNotFound )
                    {
                    // file is gone, and mds item is gone -> try to delete from cache
                    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d not found from MDS, trying to delete from cache", aMdsId );
                    TInt result = iCollection.iCache->RemoveL( aMdsId );
                    if ( result == KErrNone )
                        {
                        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d deleted from cache, generating event", aMdsId );
                        iCollection.iMessageList->AddEventL( TMPXItemId( aMdsId, 0),
                                EMPXItemDeleted, EVcxMyVideosListNoInfo );
                        iCollection.iMessageList->SendL();
                        } 
                    }
                else
                    {
                    //file is gone, mds item and cache are still present, couldnt remove them
                    MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: failed to remove %d from mds (%d)", aMdsId, result);
                    User::Leave( result );
                    }
                }
            }            
        else
            {
            // file delete failed and file still exists
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: leaving mds object intact");
            User::Leave( err );
            }
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: file delete successful, deleting item from MDS also to speed things up");
        TInt result = iCollection.iMyVideosMdsDb->RemoveVideo( aMdsId );
        if ( result == KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d removed from mds", aMdsId);
            //mds events will clean cache
            }
        else
            {
            if ( result == KErrNotFound )
                {
                // file is gone, and mds item is gone -> try to delete from cache
                MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d not found from MDS, trying to delete from cache", aMdsId );
                TInt result = iCollection.iCache->RemoveL( aMdsId );
                if ( result == KErrNone )
                    {
                    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: %d deleted from cache, generating event", aMdsId );
                    iCollection.iMessageList->AddEventL( TMPXItemId( aMdsId, 0),
                            EMPXItemDeleted, EVcxMyVideosListNoInfo );
                    iCollection.iMessageList->SendL();
                    }
                }
            else
                {
                //mds item and cache are still present, couldnt remove them
                MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: failed to remove %d from mds (%d)", aMdsId, result);
                User::Leave( result );
                }
            }
        }   
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::HandleMoveOrCopyStepL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAsyncFileOperations::HandleMoveOrCopyStepL()
    {
    CMPXMedia& cmd = iCollection.iActiveTask->GetCommand();
    
    TBool done;
    
    TBool isMoveOperation = EFalse;    
    TUint32 cmdId = cmd.ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId );    
    if ( cmdId == KVcxCommandMyVideosMove )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: move operation");
        isMoveOperation = ETrue;
        }
        
    // Start operations
    if ( iCurrentOperationIndex == 0 )
        {
        if ( !cmd.IsSupported( KMPXMediaArrayContents ) )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: error, no array defined");
            User::Leave( KErrArgument );
            }
    
        CMPXMediaArray* idMediaArray = cmd.Value<CMPXMediaArray>(
                KMPXMediaArrayContents );

        if ( idMediaArray->Count() == 0 )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: error, no items in array ");
            User::Leave( KErrArgument );
            }
        
        iTargetDrive = cmd.ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
        
        TMPXItemId mpxId;    
        iOperationIdArray.Reset();
        TInt count = idMediaArray->Count();
        for ( TInt i = 0; i < count; i++ )
            {
            mpxId = idMediaArray->AtL( i )->
                            ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: MPX ID: (%d, %d) will be moved ",
                    mpxId.iId1,
                    mpxId.iId2);
            iOperationIdArray.AppendL( idMediaArray->AtL( i )->
                    ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId ).iId1 );
            }
        
        iCollection.SendMyVideosMessageL( KVcxMessageMyVideosMoveOrCopyStarted, &cmd );

        iOperationResult.Reset();
        }

    TRAPD( err, MoveOrCopyVideoL( iOperationIdArray[iCurrentOperationIndex],
            iTargetDrive, isMoveOperation ));

    iOperationResult.AppendL( err );
    
    iCurrentOperationIndex++;
    
    // End operations
    if ( iCurrentOperationIndex > (iOperationIdArray.Count() - 1) )
        {
        iCurrentOperationIndex = 0;
        done                   = ETrue;
        if ( isMoveOperation )
            {
            SendOperationRespL( KVcxMessageMyVideosMoveResp );
            }
        else
            {
            SendOperationRespL( KVcxMessageMyVideosCopyResp );
            }
        }
    else
        {
        done = EFalse;
        }
        
    return done;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::CancelOperationL
// Called when leave or cancel occurs for the operation, generates resp msg.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::CancelOperationL( TInt aErr )
    {
    if ( iCollection.iActiveTask->IsActive() )
        {
        TInt mvCmdId = -1;
        CMPXMedia& cmd = iCollection.iActiveTask->GetCommand();
        TMPXCommandId commandId = *cmd.Value<TMPXCommandId>( KMPXCommandGeneralId );
        
        if ( commandId == KVcxCommandIdMyVideos )
            {
            mvCmdId = cmd.ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId );
            }
                    
        TInt messageId; 
        
        switch ( mvCmdId )
            {
            case KVcxCommandMyVideosMove:
                MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: generating KVcxMessageMyVideosMoveResp");
                messageId = KVcxMessageMyVideosMoveResp;
                break;
                
            case KVcxCommandMyVideosCopy:
                MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: generating KVcxMessageMyVideosCopyResp");
                messageId = KVcxMessageMyVideosCopyResp;
                break;
                
            case KVcxCommandMyVideosDelete:
                MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: generating KVcxMessageMyVideosDeleteResp");
                messageId = KVcxMessageMyVideosDeleteResp;
                break;
            
            default:
                MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: no resp msg for this operation");
                return;
            }
                        
        // generates response array and resp message
        for ( TInt i = iCurrentOperationIndex; i < iOperationIdArray.Count(); i++ )
            {
            iOperationResult.Append( aErr );
            }
        iCurrentOperationIndex = 0;

        SendOperationRespL( messageId );
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: no move,copy or delete operations were going on (nor anything else)");
        User::Leave( KErrNotFound );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::SendOperationRespL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::SendOperationRespL( TInt aCmdId )
    {
    CMPXMessage* message = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CleanupStack::PushL( message ); // 1->

    CMPXMediaArray* messageArray = message->Value<CMPXMediaArray>(
            KMPXMediaArrayContents );
    message->SetTObjectValueL<TUid>(KMPXMessageCollectionId,
            TUid::Uid(KVcxUidMyVideosMpxCollection));        
            
    // Add results
    CMPXMedia* media;
    TInt count = iOperationIdArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: item added to array");
        
        media = CMPXMedia::NewL();
        CleanupStack::PushL( media ); // 2->
        media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId,
                TMPXItemId( iOperationIdArray[i], 0) );
        media->SetTObjectValueL<TInt32>( KVcxMediaMyVideosInt32Value,
                iOperationResult[i] );
        
        messageArray->AppendL( media );
        CleanupStack::Pop( media ); // <-2                            
        }
        
    // Set message attributes
    //
    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId,
            TMPXItemId( KVcxCommandIdMyVideos, 0 ));

    message->SetTObjectValueL<TInt>( KVcxMediaMyVideosCommandId, aCmdId );
    
    iCollection.iMessageList->AddL( message );
    CleanupStack::Pop( message ); // <-1
    iCollection.iMessageList->SendL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::MoveOrCopyVideoL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::MoveOrCopyVideoL( TUint32 aMdsId, TInt aTargetDrive,
        TBool aMove )
    {
    MPX_FUNC("CVcxMyVideosAsyncFileOperations::MoveOrCopyVideoL");

    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: mds id = %d", aMdsId);
    
    //get media from cache or mds
    TInt pos;
    CMPXMedia* videoInCache = iCollection.iCache->FindVideoByMdsIdL( aMdsId, pos );

    CMPXMedia* video = NULL;
    if ( videoInCache )
        {
        video = CMPXMedia::NewL( *videoInCache );
        }
        
    if ( !video )
        {
        video = iCollection.iMyVideosMdsDb->CreateVideoL( aMdsId, EFalse /* brief details */ );
        }
        
    if ( !video )
        {
        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: mds id %d not found from mds or cache", aMdsId);
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( video ); // 1->
    
    // sanity checks
    if ( video->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId ) != 0 )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: file is being downloaded, fail, leaving with KErrInUse code.");
        User::Leave( KErrInUse );
        }

    const TInt KMaxPathLength = 255;    
    TBuf<KMaxPathLength> sourcePath( video->ValueText( KMPXMediaGeneralUri ) );
            
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: source path = %S", &sourcePath);

    if ( !DriveHasEnoughFreeSpaceL( sourcePath, aTargetDrive ) )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: target drive full -> skipping");
        User::Leave( KErrDiskFull );
        }

    TInt sourceDrive;
    User::LeaveIfError( iCollection.iFs.CharToDrive( sourcePath[0], sourceDrive ) );

    if ( sourceDrive == aTargetDrive )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: source and target drives are the same, doing nothing.");
        CleanupStack::PopAndDestroy( video ); // <-1
        return;
        }

    TBuf<KMaxPathLength> targetPath;
    
    GenerateTargetPathForMoveOrCopyL( sourcePath, targetPath, aTargetDrive );

    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: target path = %S", &targetPath );
    
    // update mds and cache
    CMPXMedia* mediaForMoveOp = NULL;
    CMPXMedia* mediaForCopyOp = NULL;
    if ( aMove )
        {
        // Update existing media.
        // Create new media object with only KMPXMediaGeneralId, and KMPXMediaGeneralUri
        // attributes set, that way update is lighter operation.
        mediaForMoveOp = CMPXMedia::NewL();
        CleanupStack::PushL( mediaForMoveOp ); // 2->
        mediaForMoveOp->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId,
               video->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
        mediaForMoveOp->SetTextValueL( KMPXMediaGeneralUri, targetPath );
        
        iCollection.SetVideoL( *mediaForMoveOp );
        }
    else
        {
        // Create new media.
        mediaForCopyOp = CMPXMedia::CopyL( *video );
        CleanupStack::PushL( mediaForCopyOp ); // 2->
        mediaForCopyOp->SetTextValueL( KMPXMediaGeneralUri, targetPath );
        iCollection.AddVideoToMdsAndCacheL( *mediaForCopyOp );
        }
        
    //copy file, delete original if move case
    TRAPD( err, BaflUtils::EnsurePathExistsL( iCollection.iFs, targetPath ) );
    
    TUint att = 0;
    iCollection.iFs.Att( sourcePath, att);    
    if ( aMove && (att & KEntryAttReadOnly) )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: move operation and source file is read only -> skipping");
        err = KErrAccessDenied;
        }
         
    if ( err == KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: copying: %S", &sourcePath);
        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: to     : %S", &targetPath);
        err = BaflUtils::CopyFile( iCollection.iFs, sourcePath, targetPath );
        if ( err == KErrNone )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: Copy succeeded");
            if ( aMove )
                {
                MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: move case");
                err = BaflUtils::DeleteFile( iCollection.iFs, sourcePath );
                if ( err != KErrNone )
                    {
                    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: delete for source file failed: %d", err );
                    MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: deleting target file");
                    TInt delErr = BaflUtils::DeleteFile( iCollection.iFs, targetPath );
                    if ( delErr != KErrNone )
                        {
                        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: delete for target file failed: %d", delErr );
                        }
                    }
                }
            }
        else
            {
            MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: CopyFile failed: %d", err);
            }
        }
    
    // roll mds and cache back if file operations failed
    if ( err != KErrNone )
        {
        if ( aMove )
            {
            MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: move failed %d", err );
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: setting media path back and leaving." );
            mediaForMoveOp->SetTextValueL( KMPXMediaGeneralUri, sourcePath );
            iCollection.SetVideoL( *mediaForMoveOp );
            }
        else
            {
            MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: copy failed %d", err );
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: deleting the added media object and leaving");
            iCollection.iMyVideosMdsDb->RemoveVideo( mediaForCopyOp->ValueTObjectL<TMPXItemId>(
                    KMPXMediaGeneralId ).iId1 );
            }
        User::Leave( err );
        }
    
    if ( aMove )
        {
        CleanupStack::PopAndDestroy( mediaForMoveOp ); // <-2
        }
    else
        {
        CleanupStack::PopAndDestroy( mediaForCopyOp ); // <-2
        }
    CleanupStack::PopAndDestroy( video ); // <-1
    }    

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::DriveHasEnoughFreeSpaceL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAsyncFileOperations::DriveHasEnoughFreeSpaceL( const TDesC& aPath, TInt aDrive )
    {
    TEntry entry;
    User::LeaveIfError( iCollection.iFs.Entry( aPath, entry ) );
    TUint32 size = static_cast<TUint32>( entry.iSize );
    
    TVolumeInfo volInfo;
    User::LeaveIfError( iCollection.iFs.Volume( volInfo, aDrive ) );
    
    TInt64 freeSpace = volInfo.iFree;

    const TInt K4MBSlack = 0x400000;

    MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: space needed: %d, space available: %d", size + K4MBSlack,
            freeSpace );
    
    if ( freeSpace < ( size + K4MBSlack ) )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: target drive does not have enough free space");
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::GenerateTargetPathForMoveOrCopyL
// ----------------------------------------------------------------------------
//    
void CVcxMyVideosAsyncFileOperations::GenerateTargetPathForMoveOrCopyL(
        const TDesC& aSourcePath, TDes& aTargetPath, TInt aTargetDrive )
    {
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: source path = %S", &aSourcePath );
    
    TChar targetDriveChar;
    User::LeaveIfError( iCollection.iFs.DriveToChar( aTargetDrive, targetDriveChar ) );    
    aTargetPath.Append( targetDriveChar );
    aTargetPath.Append( ':' );
    
    TInt sourceDrive;
    User::LeaveIfError( iCollection.iFs.CharToDrive( aSourcePath[0], sourceDrive ) );

    TInt systemDrive = iCollection.iFs.GetSystemDrive();
    
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: source drive: %d", sourceDrive);
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: target drive: %d", aTargetDrive);
    MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: system drive: %d", systemDrive);

    _LIT(KDataDes, "data");
        
    if ( sourceDrive == systemDrive )
        {
        //remove *:\data\* from the path
        TPtrC pathData( aSourcePath.Mid(3,4) );
        MPX_DEBUG2("CVcxMyVideosAsyncFileOperations:: sourcePath.Mid(3,4)= %S", &pathData);

        if ( aSourcePath.Mid(3,4) == KDataDes )
            {
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: source drive is system drive and 'data' exists in sourcePath");
            MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: not copying 'data' to the target path");
            aTargetPath.Append( aSourcePath.Mid( 7 ) );
            }
        else
            {
            aTargetPath.Append( aSourcePath.Mid( 2 ) );
            }
        }    
    else if ( aTargetDrive == systemDrive )
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: target drive is system drive -> adding 'data' to path");
        aTargetPath.Append( '\\' );
        aTargetPath.Append( KDataDes );
        aTargetPath.Append( aSourcePath.Mid( 2 ) );
        }
    else 
        {
        MPX_DEBUG1("CVcxMyVideosAsyncFileOperations:: source and target drives are not system drive, not adding or removing 'data'");        
        aTargetPath.Append( aSourcePath.Mid( 2 ) );        
        }
    const TInt KMaxPathLength = 255;
    TBuf<KMaxPathLength> uniquePath;
    TVcxMyVideosCollectionUtil::MakeUniqueFileNameL( iCollection.iFs, aTargetPath, uniquePath );
    aTargetPath = uniquePath;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::HandleDeleteStepL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAsyncFileOperations::HandleDeleteStepL()
    {
    CMPXMedia& cmd = iCollection.iActiveTask->GetCommand();
    
    //no sanity checks for array items, since we want to generate all events, even if there is nothing to delete
    
    TBool done;
    
    TUint32 cmdId = cmd.ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId );    
        
    // Start operations
    if ( iCurrentOperationIndex == 0 )
        {
        iOperationIdArray.Reset();

        if ( cmd.IsSupported( KMPXMediaArrayContents ) )
            {    
            CMPXMediaArray* idMediaArray = cmd.Value<CMPXMediaArray>(
                KMPXMediaArrayContents );
                
            TMPXItemId mpxId;
            TInt count = idMediaArray->Count();
            for ( TInt i = 0; i < count; i++ )
                {
                mpxId = idMediaArray->AtL( i )->
                                ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                MPX_DEBUG3("CVcxMyVideosAsyncFileOperations:: MPX ID: (%d, %d) will be deleted ",
                        mpxId.iId1,
                        mpxId.iId2);
                iOperationIdArray.AppendL( idMediaArray->AtL( i )->
                        ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId ).iId1 );
                }

            }
                    
        iCollection.SendMyVideosMessageL( KVcxMessageMyVideosDeleteStarted, &cmd );

        iOperationResult.Reset();
        }

    if ( iOperationIdArray.Count() > 0 )
        {
        TRAPD( err, DeleteVideoL( iOperationIdArray[iCurrentOperationIndex] ) );
        iOperationResult.AppendL( err );
        }
        
    iCurrentOperationIndex++;
    
    // End operations
    if ( iCurrentOperationIndex > (iOperationIdArray.Count() - 1) )
        {
        iCurrentOperationIndex = 0;
        done                   = ETrue;

        SendOperationRespL( KVcxMessageMyVideosDeleteResp );
        }
    else
        {
        done = EFalse;
        }
        
    return done;
    }


