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
* Description:   CVcxHgMyVideosVideoData class implementation*
*/




// INCLUDE FILES
#include "vcxhgmyvideosvideodata.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoData* CVcxHgMyVideosVideoData::NewL()
    {
    CVcxHgMyVideosVideoData* self = 
        CVcxHgMyVideosVideoData::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoData* CVcxHgMyVideosVideoData::NewLC()
    {
    CVcxHgMyVideosVideoData* self = 
        new (ELeave) CVcxHgMyVideosVideoData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::CVcxHgMyVideosVideoData()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoData::CVcxHgMyVideosVideoData()
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoData::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::~CVcxHgMyVideosVideoData()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoData::~CVcxHgMyVideosVideoData()
    {
    delete iThumbnail;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::MPXItemId()
// -----------------------------------------------------------------------------
//
TMPXItemId CVcxHgMyVideosVideoData::MPXItemId()
    {
    return iMPXItemId;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::State()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoData::TVideoDataState CVcxHgMyVideosVideoData::State()
    {
    return iState;
    }   

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::ThumbnailConversionId()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoData::ThumbnailConversionId()
    {
    return iConversionId;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::Thumbnail()
// -----------------------------------------------------------------------------
//
CFbsBitmap* CVcxHgMyVideosVideoData::Thumbnail( TBool aGiveOwnership )
    {
    CFbsBitmap* thumbnail = iThumbnail;
    if ( aGiveOwnership )
        {
        iThumbnail = NULL;
        }
    return thumbnail;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::DrmProtected()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoData::DrmProtected()
    {
    return iDrmProtected;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::ValidDrmRights()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoData::ValidDrmRights()
    {
    return iValidDrmRights;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::SetMPXItemId()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoData::SetMPXItemId( TMPXItemId aMPXItemId )
    {
    iMPXItemId = aMPXItemId;  
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::SetState()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoData::SetState( CVcxHgMyVideosVideoData::TVideoDataState aState )
    {
    iState = aState;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::SetThumbnailConversionId()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoData::SetThumbnailConversionId( TInt aConversionId )
    {
    iConversionId = aConversionId;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::SetThumbnail()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoData::SetThumbnail( CFbsBitmap* aThumbnail )
    {
    delete iThumbnail;
    iThumbnail = aThumbnail;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::SetDrmProtected()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoData::SetDrmProtected( TBool aDrmProtected )
    {
    iDrmProtected = aDrmProtected;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoData::SetValidDrmRights()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoData::SetValidDrmRights( TBool aValidDrmRights )
    {
    iValidDrmRights = aValidDrmRights;
    }
