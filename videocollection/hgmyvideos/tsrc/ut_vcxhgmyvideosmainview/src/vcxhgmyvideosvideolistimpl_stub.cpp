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
* Description:     Stub class for unit testing*
*/




#include <centralrepository.h>
#include "VcxHgMyVideosVideoListImpl.h"

// INCLUDE FILES

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
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
//
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
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListImpl::CVcxHgMyVideosVideoListImpl( 
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
  : CVcxHgMyVideosListBase( aModel, aView, aScroller ),
    iCurrentlyPlayedVideo( KErrNotFound ),
    iMultipleMarkingActive( EFalse ),
    iEnv( CCoeEnv::Static() )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListImpl::~CVcxHgMyVideosVideoListImpl()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DoListActivateL( TInt aIndex )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DoListDeactivate( )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler& CVcxHgMyVideosVideoListImpl::VideoModel()
    {
    return *( CVcxHgMyVideosVideoModelHandler* ) NULL;
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosUPnPInterface& CVcxHgMyVideosVideoListImpl::UPnPInterface()
    {
    return *( CVcxHgMyVideosUPnPInterface* ) NULL;
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::UpdateLayout( TRect& aRect )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleSelectL( TInt /*aIndex*/ )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleOpenL( TInt /*aIndex*/ )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleMarkingL( TInt /*aIndex*/, TBool /*aMarked*/ )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoListImpl::Highlight()
    {
    }

// ---------------------------------------------------------
// Returns the index(es) of videos that should be the target
// for user's operation.
// ---------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::GetOperationTargetIndexesL( 
        RArray<TInt>& aOperationTargets )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::PlayCurrentVideoL()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::PlayExternalVideoL( TBool /*aLastWatched*/,
                                                      const TDesC& aVideoPath )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::SelectNextVideoL( 
        TBool aForward,
        TViaPlayerCustomMessage& aVideoInfo )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DeleteVideosL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoListImpl::ConfirmDeleteVideosL( TInt aTargetCount,
                                                         const TDesC& aTargetName )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::OpenDeleteWaitNoteL()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::CloseDeleteWaitNote()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::CommandComplete( TInt aStatusCode )
    {
    }

// -----------------------------------------------------------------------------
// Callback about (delete) wait note dismissal.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DialogDismissedL( TInt aButtonId )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoListImpl::VideoDeletionCompletedL( TInt aFailedCount,
                                                           const TDesC& aFailedName )
    {
    }


// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
//void CVcxHgMyVideosVideoListImpl::PlayVideoL()
//    {
//    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ResumeDownloadL()
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::CancelDownloadL()
    { 
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleSortCommandL( TInt aSortCommand )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ShowMarkMenuItemsL( 
        TBool& aShowMarkSubmenu, TBool& aShowMark, TBool& aShowUnmark, 
        TBool& aShowMarkAll, TBool& aShowUnmarkAll )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleMarkCommandL( TInt aMarkCommand )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ShowMoveAndCopyMenuItemsL( 
        TBool& aShowMoveAndCopySubmenu, TBool& aShowCopy, TBool& aShowMove )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::HandleMoveOrCopyCommandL( TBool aCopy )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::VideoMoveOrCopyCompletedL( TInt aFailedCount,
                                                             const TDesC& aFailedName )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::ShowResumeStartsFromBeginningNoteL()
    {

    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DynInitToolbarL()
    {    
    }

// ------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::OfferToolbarEventL( TInt aCommand )
    {
    }

// ------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::MakeToolbarVisible( TBool aVisible )
    {
    }

// ------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::DynInitMenuPaneL( TInt aResourceId,
                                                    CEikMenuPane* aMenuPane )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::SetEmptyListTextL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::SetTitleTextL( TInt aIndex )
    {
    
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoListImpl::GetMskResourceL()
    {
    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::CheckParentalControlL( TUint32 aAgeProfile )
    {
    }

// ---------------------------------------------------------------------------
// StartMarkingMode()
// ---------------------------------------------------------------------------
//  
void CVcxHgMyVideosVideoListImpl::StartMarkingMode()
    {
    }

// ---------------------------------------------------------------------------
// EndMarkingMode()
// ---------------------------------------------------------------------------
//  
void CVcxHgMyVideosVideoListImpl::EndMarkingMode()
    {
    }

// ---------------------------------------------------------------------------
// HandleSendL()
// ---------------------------------------------------------------------------
//  
void CVcxHgMyVideosVideoListImpl::HandleSendL()
    {
    }

// ---------------------------------------------------------------------------
// MessagingConfL()
// ---------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoListImpl::MessagingConfL()
    {
    }

// ---------------------------------------------------------------------------
// IsDrmProtectedFileL()
// ---------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoListImpl::IsDrmProtectedFileL( TDesC& aFileName )
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// IsSendingFileAllowedL()
// ---------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoListImpl::IsSendingFileAllowedL( TDesC& aFileName )
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// SendUi()
// ---------------------------------------------------------------------------
//
CSendUi* CVcxHgMyVideosVideoListImpl::SendUiL()
    {
    return ( CSendUi* ) 0;
    }

// ---------------------------------------------------------------------------
// DrmUtilityL()
// ---------------------------------------------------------------------------
//
DRM::CDrmUtility* CVcxHgMyVideosVideoListImpl::DrmUtilityL()
    {
    return ( DRM::CDrmUtility* ) 0;
    }

// ---------------------------------------------------------------------------
// DrmHelperL()
// ---------------------------------------------------------------------------
//
CDRMHelper* CVcxHgMyVideosVideoListImpl::DrmHelperL()
    {
    return ( CDRMHelper* ) 0;
    }

// ---------------------------------------------------------------------------
// TryHandleAiwCommandL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::TryHandleAiwCommandL( TInt aCommand )
    {
    }

// ---------------------------------------------------------------------------
// NotifyForbiddenFilesL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListImpl::NotifyForbiddenFilesL( TInt aCount )
    {
    }
