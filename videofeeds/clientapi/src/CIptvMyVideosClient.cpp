/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Implements My Videos Client API.*
*/




// INCLUDE FILES

#include <e32svr.h>
#include <s32mem.h> 

#include "IptvClientServerCommon.h"
#include "IptvDebug.h"
#include "CIptvMyVideosClient.h"
#include "MIptvMyVideosClientObserver.h"
#include "IptvServer.pan"
#include "CIptvMyVideosFolder.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvMyVideosVideoBriefDetails.h"
#include "CIptvMyVideosVideoFullDetails.h"
#include "CIptvMyVideosContentUpdateObserver.h"
#include "CIptvMyVideosTotalVideoLengthObserver.h"
#include "CIptvMyVideosCopyOrMoveObserver.h"

// CONSTANTS
const TInt KIptvMaxFileSizeInKiloBytes = 4194303; // (TUint32) ((KMaxTUint32 - 1) / 1024)
const TInt KTUint32SizeInBytes         = 4;
const TInt KTUint16SizeInBytes         = 2;

_LIT(KIptvInvalidFileNameChars, "\"<>/|'?*");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CIptvMyVideosClient
// -----------------------------------------------------------------------------
//
CIptvMyVideosClient::CIptvMyVideosClient( 
    MIptvMyVideosClientObserver& aClientObserver ) 
  : CActive( EPriorityStandard ),
    iClientObserver( aClientObserver ),
    iMsgPtr( NULL, 0 )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::ConstructL
// -----------------------------------------------------------------------------
//
void CIptvMyVideosClient::ConstructL( TBool aSkipMaintenance )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::ConstructL -- Enter");

    User::LeaveIfError(iSession.Connect());

    // Create observers for "content updated", "get total video length" and "copy/move complete"
    // messages from server. These messages stay pending until server has completed operation.
    //
    iContentUpdateObserver    = CIptvMyVideosContentUpdateObserver::NewL(iClientObserver, iSession);
    iTotalVideoLengthObserver = CIptvMyVideosTotalVideoLengthObserver::NewL(iClientObserver, iSession);
    iCopyOrMoveObserver       = CIptvMyVideosCopyOrMoveObserver::NewL(iClientObserver, iSession);

    // If client does not need maintenance scan, send the information about
    // need for maintenance to server. If this message is not sent, server
    // assumes that maintenance is needed and performs it after a short while.
    //
    if ( aSkipMaintenance )
        {
        TRequestStatus status;
        delete iMsg;
        iMsg = NULL;
        iMsg = HBufC8::NewL(KTUint32SizeInBytes);
        iMsgPtr.Set(iMsg->Des());
        RDesWriteStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL(stream);
        stream.WriteUint32L(static_cast<TUint32>(aSkipMaintenance));
        CleanupStack::PopAndDestroy(&stream);
        iSession.SendRequest(EIptvEngineMyVideosPassMaintenanceInfoReq, iMsgPtr, status);
        User::WaitForRequest(status);
        }

    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::ConstructL -- Exit");
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvMyVideosClient* CIptvMyVideosClient::NewL(
        MIptvMyVideosClientObserver& aClientObserver,
        TBool aMaintenance /*= EFalse*/ )
    {
	CIptvMyVideosClient* self = CIptvMyVideosClient::NewLC(
	        aClientObserver,
	        aMaintenance );
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvMyVideosClient* CIptvMyVideosClient::NewLC(
        MIptvMyVideosClientObserver& aClientObserver,
        TBool aMaintenance /*= EFalse*/ )
    {
	CIptvMyVideosClient* self = new (ELeave) CIptvMyVideosClient(aClientObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aMaintenance);
	return self;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::~CIptvMyVideosClient
// -----------------------------------------------------------------------------
//
CIptvMyVideosClient::~CIptvMyVideosClient()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::~CIptvMyVideosClient -- Enter");

    // Cancel own requests, there shouldn't be any though.
    Cancel(); 

    // Cancel the content update and total video length requests.
    // The cancellation is done with the help of server side.
    if (!IsActive())
        {
        if (!iSession.IsNull())
            {
            TRequestStatus status;
            iSession.SendRequest(EIptvEngineMyVideosCancelReq, status);
            User::WaitForRequest(status);
            }
        }

    delete iContentUpdateObserver;
    delete iTotalVideoLengthObserver;
    delete iCopyOrMoveObserver;

    delete iMsg;

    iSession.Close();
    
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::~CIptvMyVideosClient -- Exit");    
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetFolderListL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetFolderListL( 
        CIptvMyVideosGlobalFolderId& aParentFolderId,
        TUint32 aFrom,
        TUint32 aAmount,
        TUint32& aTotalAmount,
        RPointerArray<CIptvMyVideosFolder>& aFolderList )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetFolderListL");

    TInt error = KErrServerBusy;

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosGetFolderListSizeReq,
                                     aParentFolderId,
                                     aFrom,
                                     aAmount ) ) )
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( EIptvEngineMyVideosGetFolderListDataReq ) ) )
                {
                error = HandleMyVideosGetFolderListDataReqL( aTotalAmount, aFolderList );
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetParentFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetParentFolderL(
        CIptvMyVideosGlobalFolderId& aFolderId,
        CIptvMyVideosGlobalFolderId& aParentFolder )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetParentFolderL");

    TInt error = KErrServerBusy;

    if ( aFolderId.iDrive == 0 && aFolderId.iFolderId == 0 )
        {
        return KErrArgument;
        }

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosGetParentFolderSizeReq, aFolderId ) ) )
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( EIptvEngineMyVideosGetParentFolderDataReq ) ) )
                {
                error = HandleMyVideosGetParentFolderDataReqL( aParentFolder );
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetFolderNameL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetFolderNameL(
        CIptvMyVideosGlobalFolderId& aFolderId,
        TDes& aName )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetParentFolderL");

    TInt error = KErrServerBusy;

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosGetFolderNameSizeReq, aFolderId ) ) )
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( EIptvEngineMyVideosGetFolderNameDataReq ) ) )
                {
                error = HandleMyVideosGetFolderNameDataReqL( aName );
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DeleteFolderReqL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteFolderReqL(CIptvMyVideosGlobalFolderId& aFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DeleteFolderReqL");
    
    TInt error = KErrServerBusy;
    
    if (aFolderId.iDrive == 0 && aFolderId.iFolderId == 0)
        {
        return KErrArgument;
        }  

    if (!IsActive())
        {
        if (iMsg)
            {
            delete iMsg;
            iMsg = NULL;
            }

        TUint32 dataSize = aFolderId.CountExternalizeSize();

        iMsg = HBufC8::NewL(dataSize);
        iMsgPtr.Set(iMsg->Des());

        RDesWriteStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aFolderId.ExternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );

        iSession.SendRequest(EIptvEngineMyVideosDeleteFolderRequestReq, iMsgPtr, iStatus);
        iActiveRequest = EIptvEngineMyVideosDeleteFolderRequestReq;
        SetActive();

        error = KErrNone;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DeleteFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteFolderL( CIptvMyVideosGlobalFolderId& aFolderId )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DeleteFolderL");
    
    TInt error = KErrServerBusy;
    
    if ( aFolderId.iDrive == 0 && aFolderId.iFolderId == 0 )
        {
        return KErrArgument;
        }  

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosDeleteFolderReq, aFolderId ) ) )
            {
            error = HandleMyVideosReqL(); // No return data.
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetVideoListL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetVideoListL(
        CIptvMyVideosGlobalFolderId& aParentFolderId,
        TUint32 aFrom,
        TUint32 aAmount,
        TUint32& aTotalAmount,
        RPointerArray<CIptvMyVideosVideoBriefDetails>& aVideoList )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetVideoListL");
    
    TInt error = KErrServerBusy;
    
    if ( ! IsActive() )
        {
        if ( KErrNone == 
             ( error = SendRequestL( EIptvEngineMyVideosGetVideoListSizeReq,
                                     aParentFolderId,
                                     aFrom,
                                     aAmount ) ) )
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( EIptvEngineMyVideosGetVideoListDataReq ) ) )
                {
                error = HandleMyVideosGetVideoListDataReqL( aTotalAmount, aVideoList );
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetVideoDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetVideoDetailsL(
        CIptvMyVideosGlobalFileId& aFileId,
        CIptvMyVideosVideoFullDetails& aFullDetails )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetVideoDetailsL");
    
    TInt error = KErrServerBusy;
    
    if ( ! IsActive() )
        {        
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosGetVideoDetailsSizeReq, aFileId ) ) )
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( EIptvEngineMyVideosGetVideoDetailsDataReq ) ) )
                {
                error = HandleMyVideosGetVideoDetailsDataReqL( aFullDetails );
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetVideoDetailsForPathL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetVideoDetailsForPathL(
        const TDesC& aLocalPath,
        CIptvMyVideosVideoFullDetails& aFullDetails )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetVideoDetailsForPathL");

    TInt error = KErrServerBusy;

    if ( aLocalPath.Length() == 0 || aLocalPath.Length() > KIptvMaxPath )
        {
        return KErrArgument;
        }

    if ( ! IsActive() )
        {
        RDesWriteStream writeStream;
        CleanupClosePushL( writeStream );
        TRequestStatus status;
        TUint32 length = aLocalPath.Length();

        // Request for the video details.
        delete iMsg;
        iMsg = NULL;
        iMsg = HBufC8::NewL( KTUint16SizeInBytes + ( length * 2 ) );
        iMsgPtr.Set( iMsg->Des() );
        writeStream.Open( iMsgPtr );
        CIptvUtil::WriteDesToStreamL( aLocalPath, writeStream );
        CleanupStack::PopAndDestroy( &writeStream );

        iSession.SendRequest( EIptvEngineMyVideosGetVideoDetailsForPathSizeReq, iMsgPtr, status );
        User::WaitForRequest( status );
        error = status.Int();

        // If operation went ok, get the reply data.
        if ( error == KErrNone )    
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( 
                               EIptvEngineMyVideosGetVideoDetailsForPathDataReq ) ) )
                {
                error = HandleMyVideosGetVideoDetailsDataReqL( aFullDetails );
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetTotalVideoLengthRequestL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetTotalVideoLengthRequestL()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetTotalVideoLengthRequestL");

    if (iTotalVideoLengthObserver && (! iTotalVideoLengthObserver->IsActive()))
        {
        iTotalVideoLengthObserver->SetActiveL();
        return KErrNone;
        }
    else
        {
        return KErrServerBusy;
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SetVideoDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::SetVideoDetailsL( 
        CIptvMyVideosVideoFullDetails& aVideoFullDetails )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SetVideoDetailsL()");
    
    TInt error = KErrServerBusy;
    
    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosSetVideoDetailsReq,
                                     aVideoFullDetails ) ) )
            {
            error = HandleMyVideosReqL(); // No return data.
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DeleteVideoReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteVideoReqL(CIptvMyVideosGlobalFileId& aFileId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DeleteVideoReqL");

    TInt error = KErrServerBusy;

    if (!IsActive())
        {
        if (iMsg)
            {
            delete iMsg;
            iMsg = NULL;
            }

        TUint32 dataSize = aFileId.CountExternalizeSize();

        iMsg = HBufC8::NewL(dataSize);
        iMsgPtr.Set(iMsg->Des());

        RDesWriteStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aFileId.ExternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );

        iSession.SendRequest(EIptvEngineMyVideosDeleteVideoRequestReq, iMsgPtr, iStatus);
        iActiveRequest = EIptvEngineMyVideosDeleteVideoRequestReq;
        SetActive();

        error = KErrNone;
        }

    return error;    
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DeleteVideoL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteVideoL( CIptvMyVideosGlobalFileId& aFileId )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DeleteVideoL");

    TInt error = KErrServerBusy;

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosDeleteVideoReq, aFileId ) ) )
            {
            error = HandleMyVideosReqL(); // No return data.
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CopyReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CopyReqL(CIptvMyVideosGlobalFileId& aSourceFileId, CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CopyReqL(file)");

    if (iCopyOrMoveObserver && (! iCopyOrMoveObserver->IsActive()))
        {
        iCopyOrMoveObserver->SetActiveL(EIptvEngineMyVideosCopyFileRequestReq, aSourceFileId, aTargetFolderId);
        return KErrNone;
        }
    else
        {
        return KErrServerBusy;
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CopyL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CopyL(
        CIptvMyVideosGlobalFileId& aSourceFileId,
        CIptvMyVideosGlobalFolderId& aTargetFolderId )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CopyL(file)");

    TInt error = KErrServerBusy;
    
    if ( ! IsActive() )
        {
        if ( KErrNone == 
             ( error = SendRequestL( EIptvEngineMyVideosCopyFileReq,
                                     aSourceFileId,
                                     aTargetFolderId ) ) )
            {
            error = HandleMyVideosReqL(); // No return data.
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CopyReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CopyReqL(CIptvMyVideosGlobalFolderId& aSourceFolderId, CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CopyReqL(folder)");

    if (iCopyOrMoveObserver && (! iCopyOrMoveObserver->IsActive()))
        {
        iCopyOrMoveObserver->SetActiveL(EIptvEngineMyVideosCopyFolderRequestReq, aSourceFolderId, aTargetFolderId);
        return KErrNone;
        }
    else
        {
        return KErrServerBusy;
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CopyL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CopyL(
        CIptvMyVideosGlobalFolderId& aSourceFolderId,
        CIptvMyVideosGlobalFolderId& aTargetFolderId,
        TUint32& aFailed )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CopyL(folder)");

    TInt error = KErrServerBusy;

    if ( aSourceFolderId.iDrive == 0 && aSourceFolderId.iFolderId == 0 )
        {
        return KErrArgument;
        }    

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosCopyFolderSizeReq,
                                     aSourceFolderId,
                                     aTargetFolderId ) ) )
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( EIptvEngineMyVideosCopyFolderDataReq ) ) )
                {
                error = HandleMyVideosCopyFolderDataReqL( aFailed );
                }
            }
        }        

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::MoveReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::MoveReqL(CIptvMyVideosGlobalFileId& aSourceFileId, CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::MoveReqL(file)");

    if (iCopyOrMoveObserver && (! iCopyOrMoveObserver->IsActive()))
        {
        iCopyOrMoveObserver->SetActiveL(EIptvEngineMyVideosMoveFileRequestReq, aSourceFileId, aTargetFolderId);
        return KErrNone;
        }
    else
        {
        return KErrServerBusy;
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::MoveL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::MoveL(
        CIptvMyVideosGlobalFileId& aSourceFileId,
        CIptvMyVideosGlobalFolderId& aTargetFolderId )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::MoveL(file)");

    TInt error = KErrServerBusy;

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosMoveFileReq,
                                     aSourceFileId,
                                     aTargetFolderId ) ) )
            {
            error = HandleMyVideosReqL(); // No return data.
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::MoveReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::MoveReqL(CIptvMyVideosGlobalFolderId& aSourceFolderId, CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::MoveReqL(folder)");

    if (iCopyOrMoveObserver && (! iCopyOrMoveObserver->IsActive()))
        {
        iCopyOrMoveObserver->SetActiveL(EIptvEngineMyVideosMoveFolderRequestReq, aSourceFolderId, aTargetFolderId);
        return KErrNone;
        }
    else
        {
        return KErrServerBusy;
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::MoveL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::MoveL(
        CIptvMyVideosGlobalFolderId& aSourceFolderId,
        CIptvMyVideosGlobalFolderId& aTargetFolderId,
        TUint32& aFailed )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::MoveL(folder)");
    
    TInt error = KErrServerBusy;
    
    if ( aSourceFolderId.iDrive == 0 && aSourceFolderId.iFolderId == 0 )
        {
        return KErrArgument;
        }    

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosMoveFolderSizeReq,
                                     aSourceFolderId,
                                     aTargetFolderId ) ) )
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( EIptvEngineMyVideosMoveFolderDataReq ) ) )
                {
                error = HandleMyVideosMoveFolderDataReqL( aFailed );
                }
            }        
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CancelCopyOrMoveL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CancelCopyOrMoveL()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CancelCopyOrMoveL()");

    TInt error = KErrServerBusy;

    if (!IsActive())
        {
        TRequestStatus status;
        iSession.SendRequest(EIptvEngineMyVideosCancelCopyOrMoveReq, status);
        User::WaitForRequest(status);

        error = status.Int();
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::RenameDatabaseFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::RenameDatabaseFolderL(
        CIptvMyVideosGlobalFolderId& aFolderId,
        TDesC& aName )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::RenameDatabaseFolderL");

    TInt error = KErrServerBusy;

    if ( ( aFolderId.iDrive == 0 && aFolderId.iFolderId == 0 ) || 
         ( aName.Length() == 0 ) ||
         ( aName.Length() > KIptvMyVideosFolderNameMaxLength ) )
        {
        return KErrArgument;
        }

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosRenameDatabaseFolderReq,
                                     aFolderId,
                                     aName ) ) )
            {
            error = HandleMyVideosReqL(); // No return data.
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::NewDatabaseFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::NewDatabaseFolderL(
        CIptvMyVideosGlobalFolderId& aParentFolderId,
        TDesC& aName,
        CIptvMyVideosGlobalFolderId& aNewFolderId )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::NewDatabaseFolderL");

    TInt error = KErrServerBusy;

    if ( ( aName.Length() == 0 ) || ( aName.Length() > KIptvMyVideosFolderNameMaxLength ) )
        {
        return KErrArgument;
        }

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosNewDatabaseFolderSizeReq,
                                     aParentFolderId,
                                     aName ) ) )
            {
            if ( KErrNone == 
                 ( error = HandleMyVideosSizeReqL( 
                               EIptvEngineMyVideosNewDatabaseFolderDataReq ) ) )
                {
                error = HandleMyVideosNewDatabaseFolderDataReqL( aNewFolderId );
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CreateVideoL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CreateVideoL(
        CIptvMyVideosVideoFullDetails& aFullDetails,
        TVideoType aVideoType,
        TUint32 aSizeEstimateInKiloBytes )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CreateVideoL");

    TInt error = KErrServerBusy;

    if ( aSizeEstimateInKiloBytes > KIptvMaxFileSizeInKiloBytes ||
         HasInvalidFileNameChars( aFullDetails.VideoPath() ) )
        {
        return KErrArgument;
        }

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosCreateVideoSizeReq,
                                     aFullDetails,
                                     (TUint32) aVideoType,
                                     aSizeEstimateInKiloBytes ) ) )
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( EIptvEngineMyVideosCreateVideoDataReq ) ) )
                {
                error = HandleMyVideosGetVideoDetailsDataReqL( aFullDetails ); 
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::EnsureFreeSpaceL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::EnsureFreeSpaceL(
        CIptvMyVideosGlobalFileId& aFileId,
        TUint32 aRequiredSpaceInKiloBytes )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::EnsureFreeSpaceL (single drive)");

    TInt error = KErrServerBusy;

    if ( ( aFileId.iDrive == 0 && aFileId.iFileId == 0 ) ||
         ( aRequiredSpaceInKiloBytes > KIptvMaxFileSizeInKiloBytes ) )
        {
        return KErrArgument;
        }

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosEnsureFreeSpaceReq,
                                     aFileId,
                                     aRequiredSpaceInKiloBytes ) ) )
            {
            error = HandleMyVideosReqL(); // No return data.
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::EnsureFreeSpaceL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::EnsureFreeSpaceL(
        CIptvMyVideosGlobalFileId& aCurrentFileId,
        TUint32 aRequiredSpaceInKiloBytes,
        CIptvMyVideosGlobalFileId& aNewFileId )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::EnsureFreeSpaceL (multi drive)");

    TInt error = KErrServerBusy;

    if ( ( aCurrentFileId.iDrive == 0 && aCurrentFileId.iFileId == 0 ) ||
         ( aRequiredSpaceInKiloBytes > KIptvMaxFileSizeInKiloBytes ) )
        {
        return KErrArgument;
        }

    if ( ! IsActive() )
        {
        if ( KErrNone ==
             ( error = SendRequestL( EIptvEngineMyVideosEnsureFreeSpaceSizeReq,
                                     aCurrentFileId,
                                     aRequiredSpaceInKiloBytes ) ) )
            {
            if ( KErrNone ==
                 ( error = HandleMyVideosSizeReqL( EIptvEngineMyVideosEnsureFreeSpaceDataReq ) ) )
                {
                error = HandleMyVideosEnsureFreeSpaceDataReqL( aNewFileId );
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// Count total video length and total file size (sum of all videos).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetTotalVideoLengthL(
    TIptvPlayTime& /*aTotalPlayTime*/,
    TIptvFileSize& /*aTotalFileSize*/ )
    {
    // Not implemented here.
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// Deletes all videos which are under downloading.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteAllDownloadsL()
    {
    // Not implemented here.
    return KErrNotSupported;
    }
    
// -----------------------------------------------------------------------------
// CIptvMyVideosClient::RunL
// -----------------------------------------------------------------------------
//
void CIptvMyVideosClient::RunL()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::RunL");

    // Note: Always delete the message first. If user makes another 
    // request from DeleteResponse below, we don't want to delete that.
    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    if ( iActiveRequest == EIptvEngineMyVideosDeleteFolderRequestReq ||
         iActiveRequest == EIptvEngineMyVideosDeleteVideoRequestReq )
        {
        iClientObserver.DeleteResponse(iStatus.Int());
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DoCancel
// -----------------------------------------------------------------------------
//
void CIptvMyVideosClient::DoCancel()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DoCancel");

    iActiveRequest = KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::RunError
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::RunError(TInt /*aError*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::RunError");

    iActiveRequest = KErrNone;
    
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::SendRequestL(TInt aMessage, CIptvMyVideosGlobalFileId& aFileIdParam)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SendRequestL");

    TRequestStatus status;
    TUint32        dataSize = 0;

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    dataSize = aFileIdParam.CountExternalizeSize();

    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    aFileIdParam.ExternalizeL(stream);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, status);
    User::WaitForRequest(status);  
    
    return status.Int();  
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::SendRequestL(TInt aMessage, CIptvMyVideosGlobalFileId& aSourceFileId, CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SendRequestL");
    
    TRequestStatus status;
    TUint32        dataSize = 0;

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    dataSize  = aSourceFileId.CountExternalizeSize();
    dataSize += aTargetFolderId.CountExternalizeSize();

    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    aSourceFileId.ExternalizeL(stream);
    aTargetFolderId.ExternalizeL(stream);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, status);
    User::WaitForRequest(status); 
    
    return status.Int();      
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::SendRequestL(TInt aMessage, CIptvMyVideosGlobalFolderId& aFolderIdParam)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SendRequestL");

    TRequestStatus status;
    TUint32        dataSize = 0;

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    dataSize = aFolderIdParam.CountExternalizeSize();

    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    aFolderIdParam.ExternalizeL(stream);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, status);
    User::WaitForRequest(status);  
    
    return status.Int(); 
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::SendRequestL(TInt aMessage, CIptvMyVideosGlobalFolderId& aFolderId, const TDesC& aName)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SendRequestL");   
    
    TRequestStatus status;
    TUint32        dataSize = 0;
    TUint32        length = aName.Length();

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    dataSize  = (aFolderId.CountExternalizeSize() + KTUint16SizeInBytes + (length * 2));

    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    aFolderId.ExternalizeL(stream);
    CIptvUtil::WriteDesToStreamL(aName, stream);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, status);
    User::WaitForRequest(status); 
    
    return status.Int();       
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::SendRequestL(TInt aMessage, CIptvMyVideosGlobalFolderId& aSourceFolderId, CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SendRequestL");
    
    TRequestStatus status;
    TUint32        dataSize = 0;

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    dataSize  = aSourceFolderId.CountExternalizeSize();
    dataSize += aTargetFolderId.CountExternalizeSize();

    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    aSourceFolderId.ExternalizeL(stream);
    aTargetFolderId.ExternalizeL(stream);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, status);
    User::WaitForRequest(status);  
    
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::SendRequestL(TInt aMessage, CIptvMyVideosGlobalFolderId& aFolderIdParam, TUint32 aUintParam1, TUint32 aUintParam2)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SendRequestL");

    TRequestStatus status;
    TUint32        dataSize = 0;

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    dataSize = (aFolderIdParam.CountExternalizeSize() + KTUint32SizeInBytes + KTUint32SizeInBytes);

    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    aFolderIdParam.ExternalizeL(stream);
    stream.WriteUint32L(aUintParam1);
    stream.WriteUint32L(aUintParam2);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, status);
    User::WaitForRequest(status);
    
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::SendRequestL(TInt aMessage, CIptvMyVideosVideoFullDetails& aVideoFullDetails)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SendRequestL");

    TRequestStatus status;
    TUint32        dataSize = 0;

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    dataSize = aVideoFullDetails.CountExternalizeSize();

    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    aVideoFullDetails.ExternalizeL(stream);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, status);
    User::WaitForRequest(status); 
    
    return status.Int();  
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::SendRequestL(TInt aMessage, CIptvMyVideosVideoFullDetails& aVideoFullDetails, TUint32 aUintParam1, TUint32 aUintParam2)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SendRequestL");

    TRequestStatus status;
    TUint32        dataSize = 0;

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    dataSize = (aVideoFullDetails.CountExternalizeSize() + KTUint32SizeInBytes + KTUint32SizeInBytes);

    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    aVideoFullDetails.ExternalizeL(stream);
    stream.WriteUint32L(aUintParam1);
    stream.WriteUint32L(aUintParam2);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, status);
    User::WaitForRequest(status); 
    
    return status.Int();  
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::SendRequestL(TInt aMessage, CIptvMyVideosGlobalFileId& aFileIdParam, TUint32 aUintParam)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SendRequestL");

    TRequestStatus status;
    TUint32        dataSize = 0;

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }

    dataSize = (aFileIdParam.CountExternalizeSize() + KTUint32SizeInBytes);

    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    aFileIdParam.ExternalizeL(stream);
    stream.WriteUint32L(aUintParam);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, status);
    User::WaitForRequest(status);  
    
    return status.Int();  
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosSizeReqL
// Generic method for handling received size reply and starting data request.
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosSizeReqL(TInt aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosSizeReqL");         

    if (iMsg)
        {
        RDesReadStream stream;
        CleanupClosePushL( stream );
        stream.Open(iMsgPtr);
        TUint32 dataSize = stream.ReadUint32L();
        CleanupStack::PopAndDestroy( &stream );

        delete iMsg;
        iMsg = NULL;

        iMsg = HBufC8::NewL(dataSize);
        iMsgPtr.Set(iMsg->Des());

        TRequestStatus status;
        iSession.SendRequest(aMessage, iMsgPtr, status);
        User::WaitForRequest(status);

        return status.Int();
        }
        
    return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosReqL
// Generic method for finishing request with no response data.
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosReqL()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosReqL");

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        
        return KErrNone;        
        }
        
    return KErrNone;        
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosGetFolderListDataReqL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosGetFolderListDataReqL(TUint32& aTotalAmount, RPointerArray<CIptvMyVideosFolder>& aFolderList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvMyVideosClient::HandleMyVideosGetFolderListDataReqL");         

    TUint32              count = 0;
    CIptvMyVideosFolder* folder = NULL;

    if (iMsg)
        {
        RDesReadStream  stream;
        CleanupClosePushL( stream );
        stream.Open(iMsgPtr);
        aTotalAmount = stream.ReadUint32L();
        count        = stream.ReadUint32L();
        for (TUint32 i = 0; i < count; i++)
            {
            folder = CIptvMyVideosFolder::NewLC();
            folder->InternalizeL(stream);
            aFolderList.AppendL(folder);
            CleanupStack::Pop(folder);
            }
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;
        
        return KErrNone;        
        }
        
    return KErrGeneral;        
    }
    
// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosGetParentFolderDataReqL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosGetParentFolderDataReqL(CIptvMyVideosGlobalFolderId& aParentFolder)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosGetParentFolderDataReqL");         

    if (iMsg)
        {
        RDesReadStream stream;
        CleanupClosePushL( stream );
        stream.Open(iMsgPtr);
        aParentFolder.InternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;
        
        return KErrNone;        
        }
        
    return KErrGeneral;        
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosGetFolderNameDataReqL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosGetFolderNameDataReqL(TDes& aName)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosGetFolderNameDataReqL");         

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        CIptvUtil::ReadDesFromStreamL(aName, stream);
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;
        
        return KErrNone;        
        }
        
    return KErrGeneral;        
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosGetVideoListDataReqL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosGetVideoListDataReqL(TUint32& aTotalAmount, RPointerArray<CIptvMyVideosVideoBriefDetails>& aVideoList)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosGetVideoListDataReqL");         

    if (iMsg)
        {
        TUint32                         count = 0;
        CIptvMyVideosVideoBriefDetails* details = NULL;
        RDesReadStream                  stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aTotalAmount = stream.ReadUint32L();
        count        = stream.ReadUint32L();
        for (TUint32 i = 0; i < count; i++)
            {
            details = CIptvMyVideosVideoBriefDetails::NewLC();
            details->InternalizeL(stream);
            aVideoList.AppendL(details);
            CleanupStack::Pop(details);
            }
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;
        
        return KErrNone;        
        }
        
    return KErrGeneral;        
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosGetVideoDetailsDataReqL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosGetVideoDetailsDataReqL(CIptvMyVideosVideoFullDetails& aFullDetails)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosGetVideoDetailsDataReqL");         

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aFullDetails.InternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;
        
        return KErrNone;        
        }
        
    return KErrGeneral;        
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosCopyFolderDataReqL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosCopyFolderDataReqL(TUint32& aFailed)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosCopyFolderDataReqL");         

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aFailed = stream.ReadUint32L();
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;
        
        return KErrNone;        
        }

    return KErrGeneral;        
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosMoveFolderDataReqL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosMoveFolderDataReqL(TUint32& aFailed)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosMoveFolderDataReqL");         

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aFailed = stream.ReadUint32L();
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;

        return KErrNone;        
        }

    return KErrGeneral;        
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosNewDatabaseFolderDataReqL
// -----------------------------------------------------------------------------    
//
TInt CIptvMyVideosClient::HandleMyVideosNewDatabaseFolderDataReqL(CIptvMyVideosGlobalFolderId& aNewFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosNewDatabaseFolderDataReqL");         

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aNewFolderId.InternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;
        
        return KErrNone;
        }
        
    return KErrGeneral;        
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HandleMyVideosEnsureFreeSpaceDataReqL
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::HandleMyVideosEnsureFreeSpaceDataReqL(CIptvMyVideosGlobalFileId& aNewFileId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::HandleMyVideosEnsureFreeSpaceDataReqL");         

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aNewFileId.InternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;
        
        return KErrNone;        
        }

    return KErrGeneral;        
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::HasInvalidFileNameChars
// -----------------------------------------------------------------------------
//
TBool CIptvMyVideosClient::HasInvalidFileNameChars(const TDesC& aString)
    {
    const TDesC& charString = KIptvInvalidFileNameChars;
    
    for (TInt strIndex = 0; strIndex < aString.Length(); strIndex++)
        {
        for (TInt charIndex = 0; charIndex < charString.Length(); charIndex++)
            {
            if (aString[strIndex] == charString[charIndex])
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }
