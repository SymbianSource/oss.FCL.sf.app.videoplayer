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
* Description:      Class for sharing thumbnailmanager session
*/


// INCLUDE FILES
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include "vcxhgmyvideosthumbnailmanager.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosThumbnailManager* CVcxHgMyVideosThumbnailManager::NewL()
    {
    CVcxHgMyVideosThumbnailManager* self = 
        CVcxHgMyVideosThumbnailManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosThumbnailManager* CVcxHgMyVideosThumbnailManager::NewLC()
    {
    CVcxHgMyVideosThumbnailManager* self = 
        new (ELeave) CVcxHgMyVideosThumbnailManager();
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::~CVcxHgMyVideosThumbnailManager()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosThumbnailManager::~CVcxHgMyVideosThumbnailManager()
    {
    iTnObservers.Reset(); // The pointers are not own
    delete iTnManager;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::PeekL()
// -----------------------------------------------------------------------------
//
TThumbnailRequestId CVcxHgMyVideosThumbnailManager::PeekL(
    CThumbnailObjectSource& aSource )
    {
    CThumbnailManager& tnm = SessionL();

    // Set 'do not create' if needed
    if ( !( iCurrentFlags & CThumbnailManager::EDoNotCreate ) )
        {
        CThumbnailManager::TThumbnailFlags flags =
            static_cast< CThumbnailManager::TThumbnailFlags >(
                iCurrentFlags | CThumbnailManager::EDoNotCreate );
        tnm.SetFlagsL( flags );
        iCurrentFlags = flags;
        }

    return tnm.GetThumbnailL( aSource );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::GetL()
// -----------------------------------------------------------------------------
//
TThumbnailRequestId CVcxHgMyVideosThumbnailManager::GetL(
    CThumbnailObjectSource& aSource )
    {
    CThumbnailManager& tnm = SessionL();

    // Remove 'do not create' if needed
    if ( iCurrentFlags & CThumbnailManager::EDoNotCreate )
        {
        CThumbnailManager::TThumbnailFlags flags =
            static_cast< CThumbnailManager::TThumbnailFlags >(
                iCurrentFlags & (~CThumbnailManager::EDoNotCreate) );
        tnm.SetFlagsL( flags );
        iCurrentFlags = flags;
        }

    return tnm.GetThumbnailL( aSource );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::Cancel()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosThumbnailManager::Cancel( TThumbnailRequestId aRequestId )
    {
    if ( iTnManager )
        {
        iTnManager->CancelRequest( aRequestId );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::Reset()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosThumbnailManager::Reset( CThumbnailObjectSource& aSource )
    {
    if ( iTnManager )
        {
        iTnManager->DeleteThumbnails( aSource );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::AddObserverL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosThumbnailManager::AddObserverL(
        MThumbnailManagerObserver& aObserver )
    {
    iTnObservers.InsertInAddressOrderL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::RemoveObserver()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosThumbnailManager::RemoveObserver(
        MThumbnailManagerObserver& aObserver )
    {
    TInt i = iTnObservers.FindInAddressOrder( &aObserver );
    if ( i >= 0 && i < iTnObservers.Count() )
        {
        iTnObservers.Remove( i ); // Pointer is not own
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::ThumbnailPreviewReady()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosThumbnailManager::ThumbnailPreviewReady(
        MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/ )
    {
    // No implementation required.
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::ThumbnailReady()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosThumbnailManager::ThumbnailReady( TInt aError,
        MThumbnailData& aThumbnail, TThumbnailRequestId aId )
    {
    TInt count = iTnObservers.Count();
    for( TInt i = 0; i < count; ++i )
        {
        iTnObservers[i]->ThumbnailReady( aError, aThumbnail, aId );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::CVcxHgMyVideosThumbnailManager()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosThumbnailManager::CVcxHgMyVideosThumbnailManager()
    {
    // No implementation required.
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosThumbnailManager::SessionL()
// -----------------------------------------------------------------------------
//
CThumbnailManager& CVcxHgMyVideosThumbnailManager::SessionL()
    {
    if ( !iTnManager )
        {
        CThumbnailManager* tnm = CThumbnailManager::NewLC( *this );
        tnm->SetThumbnailSizeL( EVideoListThumbnailSize );
        tnm->SetDisplayModeL( EColor16M );
        iCurrentFlags = tnm->Flags();
        CleanupStack::Pop( tnm );
        iTnManager = tnm;
        }
    return *iTnManager;
    }
