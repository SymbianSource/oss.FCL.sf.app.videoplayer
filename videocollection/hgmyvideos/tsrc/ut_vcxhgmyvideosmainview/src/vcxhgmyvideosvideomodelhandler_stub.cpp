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
* Description:     Stub class for unit testing.*
*/




// INCLUDE FILES


#include "VcxHgMyVideosVideoModelHandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler* CVcxHgMyVideosVideoModelHandler::NewL( 
        CVcxHgMyVideosVideoListImpl& aVideoListImpl,
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )        
    {
    CVcxHgMyVideosVideoModelHandler* self = 
        CVcxHgMyVideosVideoModelHandler::NewLC( 
            aVideoListImpl, aModel, aView, aScroller );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler* CVcxHgMyVideosVideoModelHandler::NewLC(
        CVcxHgMyVideosVideoListImpl& aVideoListImpl,
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )        
    {
    CVcxHgMyVideosVideoModelHandler* self = 
        new (ELeave) CVcxHgMyVideosVideoModelHandler( 
            aVideoListImpl, aModel, aView, aScroller );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler::CVcxHgMyVideosVideoModelHandler(
        CVcxHgMyVideosVideoListImpl& aVideoListImpl,
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    : iVideoListImpl( aVideoListImpl ),
      iModel( aModel ),
      iView( aView ),
      iScroller( aScroller ),
      iCurrentCategoryIndex( KErrNotFound ),
      iRestoreHighlightPosition( EFalse )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler::~CVcxHgMyVideosVideoModelHandler()
    {

    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::DoModelActivateL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::DoModelDeactivate()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::SaveHighlightedItemIdL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::UpdateVideoListL( TInt aCategoryIndex )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ResortVideoListL()
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::Highlight()
    {
    return 0;
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::VideoCount()
    {
    return 1;
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::CurrentCategoryIndex()
    {
    return 2;
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::MarkedVideosL( 
        RArray<TInt>& aMarkedVideos )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoModelHandler::IsProgressivePlayPossible( TInt aIndex )
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::HandleMarkCommandL( TInt aMarkCommand )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::GetViaPlayerCustomMessage(
        TInt aIndex, TViaPlayerCustomMessage& aVideoInfo, TInt& aMpxId1 )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::GetViaPlayerCustomMessage(
        TMPXItemId& aMpxItemId, TViaPlayerCustomMessage& aVideoInfo )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::GetVideoPlayerCustomMessage(
        TInt aIndex, TVideoPlayerCustomMessage& aVideoInfo, TInt& aMpxId1 )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::GetVideoPlayerCustomMessage(
        TMPXItemId& aMpxItemId, TVideoPlayerCustomMessage& aVideoInfo )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt32 CVcxHgMyVideosVideoModelHandler::VideoAgeProfileL( TInt aIndex )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt32 CVcxHgMyVideosVideoModelHandler::VideoAgeProfileL( TMPXItemId aId )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt32 CVcxHgMyVideosVideoModelHandler::VideoAgeProfileL( CMPXMedia* aMedia )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// Tries to find MPX Item Id based on URI. Does not modify MPX Item Id's initial
// value if video not found.
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoModelHandler::FindVideoMpxIdL( const TDesC& aVideoPath,
                                                        TMPXItemId& aMpxItemId )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Informative call about video being played. Method removes possible new indicator
// from video.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ClearNewVideoIndicatorL( TInt aIndex )
    {
    }

// -----------------------------------------------------------------------------
// Informative call about video being played. Method removes possible new indicator
// from video.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ClearNewVideoIndicatorL( TMPXItemId& aMpxItemId )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ShowVideoDetailsDialogL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::LastPlaybackPosition( const TDesC& aContentUri )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::SetLastPlaybackPositionL( const TDesC& aContentUri,
                                                                TInt aPosition )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC& CVcxHgMyVideosVideoModelHandler::GetVideoName( TInt aIndex )
    {
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC& CVcxHgMyVideosVideoModelHandler::GetVideoUri( TInt aIndex )
    {
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::GetVideoSize( TInt aIndex )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::DeleteVideosL( 
        const RArray<TInt>& aOperationTargets )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::MoveOrCopyVideosL( 
        const RArray<TInt>& aOperationTargets,
        TInt aTargetDrive,
        TBool aCopy )
    { 
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::PlayVideoL( TInt aIndex )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::Release( TInt aReleaseStart, 
                                               TInt aReleaseEnd )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::Request( TInt aRequestStart, 
                                               TInt aRequestEnd,
                                               THgScrollDirection aDirection )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::HandleRequestL( TInt aRequestStart, 
                                                      TInt aRequestEnd,
                                                      THgScrollDirection aDirection )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
HBufC* CVcxHgMyVideosVideoModelHandler::FormatVideoSecondRowLC( 
           CMPXMedia& aMedia )
        
    {
    return ( HBufC* ) NULL ;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::ReplaceVideoArrayL( CMPXMediaArray& aVideoList )
    {

    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::UpdateVideoListItemL( TInt aListIndex )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::DeleteItemL( TMPXItemId aMpxItemId )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::InsertVideoL( CMPXMedia* aVideo )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
TBool CVcxHgMyVideosVideoModelHandler::ValidOrigin( CMPXMedia& aMedia )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// MPX Collection calls this callback when video list is available.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::NewVideoListL( CMPXMediaArray& aVideoList )
    {
    }

// -----------------------------------------------------------------------------
// MPX Collection calls this callback when the video list has been modified.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoListModifiedL()
    {}

// -----------------------------------------------------------------------------
// MPX Collection calls this callback when single video item has been modified.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoModifiedL( TMPXChangeEventType aEventType,
                                                      TMPXItemId aMpxItemId,
                                                      TInt32 aExtraInfo,
                                                      TBool aSimulated )
    {
    }

// -----------------------------------------------------------------------------
// MPX Collection calls this callback when a single video has fetched.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoFetchingCompletedL( CMPXMedia* aVideo )
    {
    }

// -----------------------------------------------------------------------------
// MPX Collection calls this callback when deletion of multiple videos completes.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoDeletionCompletedL( TInt aFailedCount,
                                                               TMPXItemId aFailedMpxId )
    {
    }

// -----------------------------------------------------------------------------
// MPX Collection calls this callback when move or copy command is completed.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoMoveOrCopyCompletedL( TInt aFailedCount,
                                                                 TMPXItemId aFailedMpxId )
    {
    }

// -----------------------------------------------------------------------------
// MPX Collection calls this callback when details fetching is completed.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoDetailsCompletedL( const CMPXMedia& aMedia )
    {
    }
