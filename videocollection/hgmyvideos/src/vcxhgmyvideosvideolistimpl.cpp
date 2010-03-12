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
* Description:      Implementation of UI functionality for video list.*
*/




// INCLUDE FILES
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <centralrepository.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <bautils.h>  
#include <gulicon.h>
#include <fbs.h>

#include <sendui.h>
#include <SendUiConsts.h>
#include <CMessageData.h>
#include <MessagingDomainCRKeys.h>

#include <ganes/HgScroller.h>
#include <ganes/HgDoubleGraphicList.h>
#include <AknWaitDialog.h> 

#include <vcxhgmyvideos.rsg>
#include <vcxhgmyvideosicons.mbg>
#include <vcxmyvideosdefs.h>
#include <myvideosindicator.h>
#include "IptvDebug.h"

#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <mpxcommandgeneraldefs.h>

#include "vcxhgmyvideosvideolistimpl.h"
#include "vcxhgmyvideosvideomodelhandler.h"
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideosmainview.h"
#include "vcxhgmyvideoscollectionclient.h"
#include "vcxhgmyvideos.hrh"
#include "vcxhgmyvideosvideocopier.h"
#include "vcxhgmyvideospanics.h"
#include "vcxhgmyvideosupnpinterface.h"
#ifdef RD_VIDEO_AS_RINGING_TONE
#include "vcxhgmyvideosaiwmenuhandler.h"
#endif

_LIT( KVcxHgMyVideosMifFile, "\\resource\\apps\\vcxhgmyvideosicons.mif" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListImpl* CVcxHgMyVideosVideoListImpl::NewL(
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    {
    CVcxHgMyVideosVideoListImpl* self = 
        CVcxHgMyVideosVideoListImpl::NewLC( aModel, aView, aScroller );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListImpl* CVcxHgMyVideosVideoListImpl::NewLC(
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    {
    CVcxHgMyVideosVideoListImpl* self = 
        new (ELeave) CVcxHgMyVideosVideoListImpl( aModel, aView, aScroller );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::CVcxHgMyVideosVideoListImpl()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListImpl::CVcxHgMyVideosVideoListImpl( 
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
  : CVcxHgMyVideosListBase( aModel, aView, aScroller )
#ifdef RD_VIDEO_AS_RINGING_TONE
    , iAiwMenuHandler( NULL )
#endif
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ConstructL()
    {
    CVcxHgMyVideosListBase::ConstructL();

    iVideoModel = CVcxHgMyVideosVideoModelHandler::NewL( 
                      *this, iModel, iView, *iScroller );

    iVideoCopier = CVcxHgMyVideosVideoCopier::NewL( *this, *iVideoModel, iModel );
    
    iUPnP = CVcxHgMyVideosUPnPInterface::NewL( this );
    
    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* mask( NULL );
    CGulIcon* defaultIcon( NULL );
    
    // Create default icon for Hg list.
    AknsUtils::CreateIconLC( AknsUtils::SkinInstance(), 
                             KAknsIIDQgnPropMtvProgClip,
                             bitmap,
                             mask,
                             KVcxHgMyVideosMifFile,
                             EMbmVcxhgmyvideosiconsQgn_prop_mtv_prog_clip,
                             EMbmVcxhgmyvideosiconsQgn_prop_mtv_prog_clip_mask );
    
    TInt error( KErrNone );
    error = AknIconUtils::SetSize( bitmap, 
                                   CHgDoubleGraphicList::PreferredImageSize(), 
                                   EAspectRatioPreservedAndUnusedSpaceRemoved );
    User::LeaveIfError( error );
    
    error = AknIconUtils::SetSize( mask, 
                                   CHgDoubleGraphicList::PreferredImageSize(), 
                                   EAspectRatioPreservedAndUnusedSpaceRemoved );
    User::LeaveIfError( error );
    
    // Ownership of bitmap and mask is transferred to icon.
    defaultIcon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 ); // bitmap and mask
    
    CleanupStack::PushL( defaultIcon );
             
    // Ownership of icon is transferred.
    iScroller->SetDefaultIconL( defaultIcon );
    CleanupStack::Pop( defaultIcon );
    
    CGulIcon* ind = iVideoModel->VideoIndicator().VideoOverlayIndicatorL();
    if ( ind )
        {
        CleanupStack::PushL( ind );
        // Ownership of the icon is transferred
        static_cast<CHgDoubleGraphicList*>( iScroller )->SetIconOverlayIndicator( ind );
        CleanupStack::Pop( ind );
        }
    
    iScroller->SetMarkingObserver( *this );
    iScroller->SetSelectionObserver( *this );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::~CVcxHgMyVideosVideoListImpl()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListImpl::~CVcxHgMyVideosVideoListImpl()
    {
    CloseDeleteWaitNote();
    delete iUPnP;
    delete iVideoCopier;
    delete iVideoModel;
    delete iSendUi;
#ifdef RD_VIDEO_AS_RINGING_TONE
    delete iAiwMenuHandler;
#endif
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::DoListActivateL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DoListActivateL( TInt aIndex )
    {
    TInt index = aIndex;
    index = ( ( index != KErrNotFound ) ? index : iVideoModel->CurrentCategoryIndex() );
    index = ( ( index != KErrNotFound ) ? index : 0 );

    iVideoModel->UpdateVideoListL( index );
    
    iVideoModel->DoModelActivateL();
    CVcxHgMyVideosListBase::DoListActivateL( index );
    iScroller->SetFocus( ETrue );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::DoListDeactivate()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DoListDeactivate()
    {
    CVcxHgMyVideosListBase::DoListDeactivate();
    iVideoModel->DoModelDeactivate();
    iScroller->SetFocus( EFalse );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::VideoModel()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler& CVcxHgMyVideosVideoListImpl::VideoModel()
    {
    return *iVideoModel;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::UPnPInterface()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosUPnPInterface& CVcxHgMyVideosVideoListImpl::UPnPInterface()
    {
    return *iUPnP;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::HandleSelectL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleSelectL( TInt /*aIndex*/ )
    {
	iView.DynInitMskL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::HandleOpenL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleOpenL( TInt /*aIndex*/ )
    {
    if ( iModel.TouchSupport() )
        {    
        PlayCurrentVideoL();
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::HandleMarkingL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleMarkingL( TInt /*aIndex*/, TBool /*aMarked*/ )
    {    
    iView.DynInitMskL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::Highlight()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoListImpl::Highlight()
    {
    return iVideoModel->Highlight();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::GetOperationTargetIndexesL()
// Returns the index(es) of videos that should be the target for user's 
// operation.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::GetOperationTargetIndexesL( 
        RArray<TInt>& aOperationTargets )
    {
    iVideoModel->MarkedVideosL( aOperationTargets );

    if ( aOperationTargets.Count() == 0 )
        {
        TInt highlight = Highlight();
        if ( highlight == KErrNotFound )
            {
            User::Leave( KErrNotFound );
            }

        aOperationTargets.AppendL( highlight );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::PlayCurrentVideoL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::PlayCurrentVideoL()
    {
    PlayVideoL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::DeleteVideosL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DeleteVideosL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoListImpl::DeleteVideosL()" );
    RArray<TInt> operationTargets;
    CleanupClosePushL( operationTargets );
    
    GetOperationTargetIndexesL( operationTargets );

    if ( ConfirmDeleteVideosL( operationTargets.Count(),
                               iVideoModel->GetVideoName( operationTargets[0] ) ) )
        {
        HandleMarkCommandL( EVcxHgMyVideosCmdUnmarkAll );

        // Wait note is closed in destructor (CloseDeleteWaitNote()), 
        // in DialogDismissedL() or in VideoDeletionCompletedL().
        OpenDeleteWaitNoteL();

        iVideoModel->DeleteVideosL( operationTargets );
        }
    CleanupStack::PopAndDestroy( &operationTargets );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::ConfirmDeleteVideosL()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoListImpl::ConfirmDeleteVideosL( TInt aTargetCount,
                                                         const TDesC& aTargetName )
    {
    HBufC* text = NULL;

    if ( aTargetCount == 1 )
        {
        text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_QUERY_COMMON_CONF_DELETE, aTargetName );
        }
    else if ( aTargetCount > 1 )
        {
        text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_FLDR_DEL_ITEMS_QUERY, aTargetCount );
        }

    if ( text )
        {
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();

        TInt result = dlg->ExecuteLD( R_VCXHGMYVIDEOS_DELETE_CONFIRMATION_QUERY, *text );
        CleanupStack::PopAndDestroy( text );

        if ( result == EAknSoftkeyYes || result == EAknSoftkeyOk )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::OpenDeleteWaitNoteL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::OpenDeleteWaitNoteL()
    {
    CloseDeleteWaitNote();

    iDeleteWaitDialog = new (ELeave) 
            CAknWaitDialog( ( REINTERPRET_CAST( CEikDialog**, &iDeleteWaitDialog ) ), ETrue );
    iDeleteWaitDialog->SetCallback( this );
    iDeleteWaitDialog->ExecuteLD( R_VCXHGMYVIDEOS_DELETING_WAIT_NOTE );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::CloseDeleteWaitNote()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::CloseDeleteWaitNote()
    {
    if ( iDeleteWaitDialog )
        {
        TRAPD( error, iDeleteWaitDialog->ProcessFinishedL() );
        if ( error != KErrNone )
            {
            delete iDeleteWaitDialog;
            }
        iDeleteWaitDialog = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::CommandComplete()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::CommandComplete( TInt /*aStatusCode*/ )
    {
    // status larger than KErrNone does not require any attention.
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::DialogDismissedL()
// Callback about (delete) wait note dismissal.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DialogDismissedL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyCancel )
        {
        iModel.CollectionClient().CancelDeleteVideosL();
        }
    
    iDeleteWaitDialog = NULL;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::VideoDeletionCompletedL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoListImpl::VideoDeletionCompletedL( TInt aFailedCount,
                                                           const TDesC& aFailedName )
    {
    CloseDeleteWaitNote();

    if ( aFailedCount > 0 )
        {
        HBufC* text = NULL;

        if ( aFailedCount == 1 )
            {
            text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_FLDR_CANT_DELETE_FILE_OPEN, aFailedName );
            }
        else
            {
            text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_CANT_DELETE_ITEMS );
            }

        CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );  
        }    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::PlayVideoL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::PlayVideoL()
    {
    RArray<TInt> operationTargets;
    CleanupClosePushL( operationTargets );
    
    GetOperationTargetIndexesL( operationTargets );
    HandleMarkCommandL( EVcxHgMyVideosCmdUnmarkAll );
    
    // Currently only play one video
    // Need to handle multiply selected videos
    for ( TInt i = operationTargets.Count() - 1; i >= 0; i-- )
        {
        if( !iUPnP->IsStarted() )
            {
            iVideoModel->PlayVideoL( operationTargets[i] );

            iModel.SetAppState( CVcxHgMyVideosModel::EVcxMyVideosAppStatePlayer );
            }
        else // UPnP started
            {
            TInt err(KErrNone); 
            
            err = iUPnP->ShowVideo( iVideoModel->GetVideoUri( operationTargets[0] ) );

            if( err != KErrNone )
                {
                // some general upnp framework error, 
                // call stop to deallocate
                iUPnP->Stop();
                }
            }        
        break;
        }
    
    CleanupStack::PopAndDestroy( &operationTargets );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::ResumeDownloadL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ResumeDownloadL()
    {
    iVideoModel->ResumeDownloadL( Highlight() );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::CancelDownloadL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::CancelDownloadL()
    {
    iVideoModel->CancelDownloadL( Highlight() );   
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::HandleSortCommandL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleSortCommandL( TInt aSortCommand )
    {
    TVcxMyVideosSortingOrder currentOrder = iModel.VideolistSortOrderL();
    TVcxMyVideosSortingOrder wantedOrder( EVcxMyVideosSortingNone );
        
    switch ( aSortCommand )
        {
        case EVcxHgMyVideosCmdSortByDate:
            {
            wantedOrder = EVcxMyVideosSortingCreationDate;
            break;
            }
        case EVcxHgMyVideosCmdSortByName:
            {
            wantedOrder = EVcxMyVideosSortingName;
            break;
            }            
        case EVcxHgMyVideosCmdSortBySize:
            {
            wantedOrder = EVcxMyVideosSortingSize;
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    if ( currentOrder != wantedOrder )
        {
        iModel.SetVideolistSortOrderL( wantedOrder );
        iVideoModel->ResortVideoListL();
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::ShowMarkMenuItemsL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ShowMarkMenuItemsL( 
        TBool& aShowMarkSubmenu,
        TBool& showStartMarking,
        TBool& showEndMarking, 
        TBool& aShowMarkAll,
        TBool& aShowUnmarkAll )
    {
    RArray<TInt> markedVideos;
    CleanupClosePushL( markedVideos );
    
    iVideoModel->MarkedVideosL( markedVideos );
    
    TInt count = iVideoModel->VideoCount();
    TInt highlight = Highlight();

    // Set menu visibility information.
    TBool markingMode( IsMarking() );
    showStartMarking = !markingMode;
    showEndMarking = markingMode;
    aShowMarkAll = ( ( count > 0 ) && ( markedVideos.Count() < count ) );
    aShowUnmarkAll = ( ( count > 0 ) && ( markedVideos.Count() > 0 ) );
    aShowMarkSubmenu = ( showStartMarking || showEndMarking || aShowMarkAll || aShowUnmarkAll );
    
    CleanupStack::PopAndDestroy( &markedVideos );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::HandleMarkCommandL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleMarkCommandL( TInt aMarkCommand )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoListImpl::HandleMarkCommandL()" );
    
    switch ( aMarkCommand )
        {
        case EVcxHgMyVideosCmdToggleMultipleMarking:
            {
            StartMarkingMode();
            break;
            }
        case EVcxHgMyVideosCmdStopMarking:
            {
            iVideoModel->HandleMarkCommandL( EVcxHgMyVideosCmdUnmarkAll );
            EndMarkingMode();
            break;
            }
        case EVcxHgMyVideosCmdMark:
        case EVcxHgMyVideosCmdMarkAll:
            {
            iVideoModel->HandleMarkCommandL( aMarkCommand );
            StartMarkingMode();
            break;
            }
        case EVcxHgMyVideosCmdUnmark:
        case EVcxHgMyVideosCmdUnmarkAll:
            {
            iVideoModel->HandleMarkCommandL( aMarkCommand );
            EndMarkingMode();
            break;
            }
        }
    
    iView.DynInitMskL();
    iView.DynInitRskL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::ShowMoveAndCopyMenuItemsL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ShowMoveAndCopyMenuItemsL( 
        TBool& aShowMoveAndCopySubmenu, TBool& aShowCopy, TBool& aShowMove )
    {
    RArray<TInt> operationTargets;
    CleanupClosePushL( operationTargets );
    
    TRAPD( error, GetOperationTargetIndexesL( operationTargets ) );

    if ( error != KErrNone )
        {
        aShowMoveAndCopySubmenu = aShowCopy = aShowMove = EFalse;
        }
	else
        {
        iVideoCopier->ShowMenuItemsL( operationTargets, 
                                      aShowMoveAndCopySubmenu,
                                      aShowCopy,
                                      aShowMove );
        }
    
    CleanupStack::PopAndDestroy( &operationTargets );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::HandleMoveOrCopyCommandL() 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleMoveOrCopyCommandL( TBool aCopy )
    {
    RArray<TInt> operationTargets;
    CleanupClosePushL( operationTargets );
    
    GetOperationTargetIndexesL( operationTargets );
    HandleMarkCommandL( EVcxHgMyVideosCmdUnmarkAll );
    iVideoCopier->MoveOrCopyL( operationTargets, aCopy );
    
    CleanupStack::PopAndDestroy( &operationTargets );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::VideoMoveOrCopyCompletedL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::VideoMoveOrCopyCompletedL( TInt aFailedCount,
                                                             const TDesC& aFailedName )
    {
    iVideoCopier->VideoMoveOrCopyCompletedL( aFailedCount, aFailedName );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::ShowResumeStartsFromBeginningNoteL() 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ShowResumeStartsFromBeginningNoteL()
    {
    HBufC* text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_RESUME_NOT_POSSIBLE );
    CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );     
    }

// ------------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::DynInitMenuPaneL()
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DynInitMenuPaneL( TInt aResourceId,
                                                    CEikMenuPane* aMenuPane )
    {
#ifdef RD_VIDEO_AS_RINGING_TONE
    if ( AiwMenuHandlerL()->TryHandleSubmenuL( aMenuPane ) )
        {
        // Submenu was initialized by Aiw
        return;
        }
#endif

    RArray<TInt> markedVideos;
    CleanupClosePushL( markedVideos );
    
    iVideoModel->MarkedVideosL( markedVideos );
    TInt highlight = Highlight();        
    TInt count = iVideoModel->VideoCount();

    TVcxMyVideosDownloadState dlState( EVcxMyVideosDlStateNone );
    TBool progressivePlay( EFalse );
    
    if ( count > 0 && highlight >= 0 )
        {
        dlState = iVideoModel->VideoDownloadState( highlight );
        progressivePlay = iVideoModel->IsProgressivePlayPossible( highlight );
        }

    // Check if there are marked items on the list that can be deleted
    TBool itemsToDelete( EFalse );
    
    for ( TInt i = 0; i < markedVideos.Count(); i++ )
        {
        if ( iVideoModel->VideoDownloadState( markedVideos[i] ) == EVcxMyVideosDlStateNone )
            {    
            itemsToDelete = ETrue;
            break;
            }
        }

    TBool showDelete = ( ( highlight >= 0 ) && ( count > 0 ) 
                         && ( ( dlState == EVcxMyVideosDlStateNone && markedVideos.Count() == 0 ) 
                         || itemsToDelete ) );
    
    if ( aResourceId == R_VCXHGMYVIDEOS_MAINVIEW_MENU )
        {      
        if ( ! iModel.TouchSupport() )
            {
            TBool showPlay = ( ( markedVideos.Count() == 0 ) && ( highlight >= 0 ) && ( count > 0 )
                           && ( ( dlState == EVcxMyVideosDlStateNone ) || ( progressivePlay ) ) );
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdPlay, ! showPlay );
            }

        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdDelete, ! showDelete );

        TBool showCancel = ( ( markedVideos.Count() == 0 ) && ( highlight >= 0 ) && ( count > 0 )
                             && ( dlState != EVcxMyVideosDlStateNone ) );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdCancelDownload, ! showCancel );

        TBool showResume = showCancel && ( ( dlState == EVcxMyVideosDlStatePaused ) ||
                                           ( dlState == EVcxMyVideosDlStateFailed ) );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdResumeDownload, ! showResume );

        TBool showVideoDetails = ( ( markedVideos.Count() == 0 ) &&
                                   ( highlight >= 0 ) && 
                                   ( count > 0 ) && 
                                   ( ( dlState == EVcxMyVideosDlStateNone ) ||
                                     ( progressivePlay ) ) );        
        
        // Dont show file details if file isn't downloaded yet.
        if( ( VideoModel().VideoDownloadState( Highlight() ) == EVcxMyVideosDlStateDownloading ) ||
            ( VideoModel().VideoDownloadState( Highlight() ) == EVcxMyVideosDlStateFailed ) ||
            ( VideoModel().VideoDownloadState( Highlight() ) == EVcxMyVideosDlStatePaused ) )
            {
            showVideoDetails = EFalse;
            }
        
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdVideoDetails, ! showVideoDetails );        

        // UPnP menu item. Item's behaviour follows video details item.     
        if ( showVideoDetails && count > 0 && highlight >= 0 && iUPnP->IsApplicable() )
            {
            if ( iUPnP->IsStarted() )
                {
                aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdStopShowViaHomenet, EFalse);
                }
            else
                {
                aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdShowViaHomenet, EFalse );
                }
            }
        
        // Send menu item.
        if ( ( highlight >= 0 ) && ( count > 0 ) &&
                ( dlState == EVcxMyVideosDlStateNone ) )
            {
            // Get total size for marked videos
            RArray<TInt> operationTargets;
            CleanupClosePushL( operationTargets );
            
            GetOperationTargetIndexesL( operationTargets );
            TInt64 attachmentsTotalSize( 0 );
            for ( TInt i = 0; i < operationTargets.Count(); i++ )
                {
                attachmentsTotalSize += iVideoModel->GetVideoSize( operationTargets[i] );
                }

            IPTVLOGSTRING3_LOW_LEVEL( "CVcxHgMyVideosVideoListImpl::DynInitMenuPaneL() %d files with total size %ld", operationTargets.Count(), attachmentsTotalSize );
            
            // SendUi uses TInt for size, hide Send menu item for over 2GB attachments
            if ( attachmentsTotalSize > 0 && attachmentsTotalSize < KMaxTInt )
                {
                // Place Send menu item on top of "Use as" (Assign) submenu
                TInt sendItemIndex = 0;
#ifdef RD_VIDEO_AS_RINGING_TONE                
                aMenuPane->ItemAndPos( EVcxHgMyVideosCmdAiwAssign, sendItemIndex );
#else
                aMenuPane->ItemAndPos( EVcxHgMyVideosCmdSortSubMenu, sendItemIndex );
#endif
                // Add Send item to menu
                TSendingCapabilities capabilities(
                    0,
                    attachmentsTotalSize,
                    TSendingCapabilities::ESupportsAttachments );
                SendUiL()->AddSendMenuItemL(
                        *aMenuPane,
                        sendItemIndex,
                        EVcxHgMyVideosCmdSend,
                        capabilities );
                aMenuPane->SetItemSpecific( EVcxHgMyVideosCmdSend, ETrue );

                // The Send item also needs to be flagged as a list query.
                CEikMenuPaneItem::SData& itemData = aMenuPane->ItemData( EVcxHgMyVideosCmdSend );
                itemData.iFlags |= EEikMenuItemSpecificListQuery;
                }
            CleanupStack::PopAndDestroy( &operationTargets );
            }

#ifdef RD_VIDEO_AS_RINGING_TONE
        // Assign (use as) menu item
        if ( ( markedVideos.Count() == 0 ) &&
                ( highlight >= 0 ) && ( count > 0 ) &&
                ( ( dlState == EVcxMyVideosDlStateNone ) ) )
            {
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdAiwAssign, EFalse );
            }
        else
            {
            // Hide Assign (use as) menu item
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdAiwAssign, ETrue );
            }
#endif

        // Marking submenu
        if( count > 0 )
            {
            // Show "Mark/Unmark" only if there are videos in the list
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdMarkUnmarkSubMenu, EFalse);
            }

        // "Sort by" submenu
        if( count == 0 )
            {
            // Dont show "Sort by" if list is empty.
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdSortSubMenu, ETrue );
            }
        
        // Hide "Settings" menu item
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdOpenSettings, ETrue );
        }
#ifdef RD_VIDEO_AS_RINGING_TONE
    else if ( aResourceId == R_VCXHGMYVIDEOS_USE_AS_SUBMENU )
        {
        RArray<TInt> operationTargets;
        CleanupClosePushL( operationTargets );

        GetOperationTargetIndexesL( operationTargets );
        if ( operationTargets.Count() > 0 )
            {
            HBufC* videoUri = iVideoModel->GetVideoUri( operationTargets[0] ).AllocLC();
            if ( videoUri->Length() 
                    && BaflUtils::FileExists( iModel.FileServerSessionL(), *videoUri ) )
                {
                AiwMenuHandlerL()->InitializeMenuPaneL(
                        aResourceId,
                        aMenuPane,
                        *videoUri,
                        KNullDesC );    //mime type
                }
            CleanupStack::PopAndDestroy( videoUri );
            }
        CleanupStack::PopAndDestroy( &operationTargets );
        }
#endif
    CleanupStack::PopAndDestroy( &markedVideos );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::SetEmptyListTextL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::SetEmptyListTextL()
    {
    CVcxHgMyVideosListBase::SetEmptyListTextByResourceL( R_VCXHGMYVIDEOS_OPENING_VIDEOS );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::SetTitleTextL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::SetTitleTextL( TInt aIndex )
    {
    switch ( aIndex )
        {
        case KVcxMvcCategoryIdAll:
            CVcxHgMyVideosListBase::SetTitleTextByResourceL( R_VCXHGMYVIDEOS_ALL_VIDEOS_TITLE );
            break;
        case KVcxMvcCategoryIdDownloads:
            CVcxHgMyVideosListBase::SetTitleTextByResourceL( R_VCXHGMYVIDEOS_DOWNLOADS_TITLE );
            break;
        case KVcxMvcCategoryIdCaptured:
            CVcxHgMyVideosListBase::SetTitleTextByResourceL( R_VCXHGMYVIDEOS_CAPTURED_TITLE );
            break;
        case KVcxMvcCategoryIdOther:
            CVcxHgMyVideosListBase::SetTitleTextByResourceL( R_VCXHGMYVIDEOS_OTHER_TITLE );
            break;
        default:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "MPX My Videos UI # ...VideoListImpl::SetTitleTextL() Unexpected category!" );
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::GetMskResourceL()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoListImpl::GetMskResourceL()
    {
    RArray<TInt> markedVideos;
    CleanupClosePushL( markedVideos );
    
    iVideoModel->MarkedVideosL( markedVideos );
    TInt highlight = Highlight();
    TInt count = iVideoModel->VideoCount();
    TVcxMyVideosDownloadState dlState( EVcxMyVideosDlStateNone );
    TBool progressivePlay( EFalse );
    if ( count > 0 && highlight >= 0 )
        {
        dlState = iVideoModel->VideoDownloadState( highlight );
        progressivePlay = iVideoModel->IsProgressivePlayPossible( highlight );
        }
    TBool showPlay = ( ( markedVideos.Count() == 0 ) && ( highlight >= 0 ) && ( count > 0 )
                       && ( ( dlState == EVcxMyVideosDlStateNone ) || ( progressivePlay ) ) );
    
    CleanupStack::PopAndDestroy( &markedVideos );
    
    if ( showPlay )
        {
        return R_VCXHGMYVIDEOS_MSK_PLAY;
        }
    else if ( count == 0 )
        {
        return R_VCXHGMYVIDEOS_MSK_NOTHING;
        }
    else
        {
        return R_VCXHGMYVIDEOS_MSK_CONTEXTMENU;
        }
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::StartMarkingMode()
// ---------------------------------------------------------------------------
//  
void CVcxHgMyVideosVideoListImpl::StartMarkingMode()
    {
    IPTVLOGSTRING2_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoListImpl::StartMarkingMode() IsMarking = %d", IsMarking() );
    
    // Start multiple marking mode
    if ( iModel.TouchSupport() )
        {
        iScroller->SetFlags( CHgScroller::EHgScrollerSelectionMode );
        }
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::EndMarkingMode()
// ---------------------------------------------------------------------------
//  
void CVcxHgMyVideosVideoListImpl::EndMarkingMode()
    {
    IPTVLOGSTRING2_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoListImpl::EndMarkingMode() IsMarking = %d", IsMarking() );

    // End multiple marking mode
    iScroller->ClearFlags( CHgScroller::EHgScrollerSelectionMode );
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::HandleSendL()
// ---------------------------------------------------------------------------
//  
void CVcxHgMyVideosVideoListImpl::HandleSendL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxHgMyVideosVideoListImpl::HandleSendL() ENTER" );

    // Create message and disabled transports filter
    CMessageData* message = CMessageData::NewLC();
    CArrayFixFlat< TUid >* mtmFilter = new( ELeave ) CArrayFixFlat< TUid >( 1 );
    CleanupStack::PushL( mtmFilter );

    if ( MessagingConfL() & KMuiuEmailDisabledInMultimedia )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CVcxHgMyVideosVideoListImpl::HandleSendL() send via email disabled" );
        // Send via email disabled in CenRep, add to filter to disable
        mtmFilter->AppendL( KSenduiMtmSmtpUid );
        mtmFilter->AppendL( KSenduiMtmSyncMLEmailUid );
        }

    // Get the selected video(s) and attach
    RArray<TInt> operationTargets;
    CleanupClosePushL( operationTargets );
    
    GetOperationTargetIndexesL( operationTargets );
    TInt64 attachmentsTotalSize( 0 );

    IPTVLOGSTRING2_LOW_LEVEL( "CVcxHgMyVideosVideoListImpl::HandleSendL() targets count: %d", operationTargets.Count() );
    for ( TInt i = 0; i < operationTargets.Count(); i++ )
        {
        HBufC* videoUri = iVideoModel->GetVideoUri( operationTargets[i] ).AllocLC();
        TInt64 videoSize = iVideoModel->GetVideoSize( operationTargets[i] );
        TVcxMyVideosDownloadState dlState = iVideoModel->VideoDownloadState( operationTargets[i] );

        IPTVLOGSTRING3_LOW_LEVEL( "CVcxHgMyVideosVideoListImpl::HandleSendL() file: %S (%ld bytes)", videoUri, videoSize );

        // Attach file to message
        if ( videoUri->Length() > 0 &&
                videoSize > 0 &&
                EVcxMyVideosDlStateNone == dlState )
            {
            message->AppendAttachmentL( *videoUri );
            attachmentsTotalSize += videoSize;
            }

        CleanupStack::PopAndDestroy( videoUri );
        }

    // Send message if it has attachment(s)
    TSendingCapabilities capabilities( 0, attachmentsTotalSize, TSendingCapabilities::ESupportsAttachments );
    if ( message->AttachmentArray().MdcaCount() > 0 )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CVcxHgMyVideosVideoListImpl::HandleSendL() send message" );
        SendUiL()->ShowQueryAndSendL( message, capabilities, mtmFilter, KNullUid, EFalse );
        HandleMarkCommandL( EVcxHgMyVideosCmdUnmarkAll );
        }

    CleanupStack::PopAndDestroy( &operationTargets );
    CleanupStack::PopAndDestroy( mtmFilter );
    CleanupStack::PopAndDestroy( message );

    IPTVLOGSTRING_LOW_LEVEL( "CVcxHgMyVideosVideoListImpl::HandleSendL() EXIT" );
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::MessagingConfL()
// ---------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoListImpl::MessagingConfL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxHgMyVideosVideoListImpl::MessagingConfL()" );
    TInt messagingConf;
    CRepository* cenrep = CRepository::NewL( KCRUidMuiuMessagingConfiguration );
    TInt cenrepErr( cenrep->Get( KMuiuSendUiConfiguration, messagingConf ) );
    delete cenrep;
    User::LeaveIfError( cenrepErr );
    return messagingConf;
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::SendUi()
// ---------------------------------------------------------------------------
//
CSendUi* CVcxHgMyVideosVideoListImpl::SendUiL()
    {
    if ( !iSendUi )
        {
        iSendUi = CSendUi::NewL();
        }
    return iSendUi;
    }

#ifdef RD_VIDEO_AS_RINGING_TONE
// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::TryHandleAiwCommandL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::TryHandleAiwCommandL( TInt aCommand )
    {
    if ( AiwMenuHandlerL()->IsAiwCommand( aCommand ) )
        {
        // Call AIW service handler
        RArray<TInt> operationTargets;
        CleanupClosePushL( operationTargets );
        
        GetOperationTargetIndexesL( operationTargets );
        if ( operationTargets.Count() > 0 )
            {
            HBufC* videoUri = iVideoModel->GetVideoUri( operationTargets[0] ).AllocLC();
            if ( videoUri->Length() )
                {
                AiwMenuHandlerL()->HandleAiwCommandL(
                        aCommand,
                        *videoUri,
                        KNullDesC );    //mime type
                }
            CleanupStack::PopAndDestroy( videoUri );
            }
        CleanupStack::PopAndDestroy( &operationTargets );
        }
    }
#endif

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::IsMarking()
// ---------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoListImpl::IsMarking()
    {
    TBool ret( EFalse );
    
    if( iScroller->Flags() & CHgScroller::EHgScrollerSelectionMode )
        {
        ret = ETrue;
        }
    return ret;
    }

#ifdef RD_VIDEO_AS_RINGING_TONE
// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoListImpl::AiwMenuHandlerL()
// ---------------------------------------------------------------------------
//
CHgMyVideosAiwMenuHandler* CVcxHgMyVideosVideoListImpl::AiwMenuHandlerL()
    {
    if ( !iAiwMenuHandler )
        {
        iAiwMenuHandler = new (ELeave) CHgMyVideosAiwMenuHandler();
        iAiwMenuHandler->AttachMenuL( R_VCXHGMYVIDEOS_USE_AS_SUBMENU );
        }
    return iAiwMenuHandler;
    }
#endif
