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
* Description:    Download list cache. Circular buffer made for speeding up*
*/




// INCLUDE FILES
#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <vcxmyvideosdefs.h>
#include <centralrepository.h>
#include <collate.h>
#include "vcxmyvideosdownloadcache.h"
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideosdownloadutil.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideoscategories.h"

const TInt KVcxMvDownloadFindCacheSize = 20;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosDownloadCache* CVcxMyVideosDownloadCache::NewL()
    {
    CVcxMyVideosDownloadCache* self = new (ELeave) CVcxMyVideosDownloadCache();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosDownloadCache::~CVcxMyVideosDownloadCache()
    {
    MPX_FUNC("CVcxMyVideosDownloadCache::~CVcxMyVideosDownloadCache");
    
    iDownloadId.Close();
    iMedia.Close(); // pointers are not own
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosDownloadCache::CVcxMyVideosDownloadCache()
    {
    MPX_FUNC("CVcxMyVideosDownloadCache::CVcxMyVideosDownloadCache");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadCache::ConstructL ()
    {
    iDownloadId.Reset();
    iMedia.Reset();
    for ( TInt i = 0; i < KVcxMvDownloadFindCacheSize; i++ )
        {
        iDownloadId.Append( 0 );
        iMedia.Append( NULL );
        }
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadCache::Get
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosDownloadCache::Get( TUint32 aDownloadId )
    {
    for ( TInt i = 0; i < KVcxMvDownloadFindCacheSize; i++ )
        {
        if ( iDownloadId[i] == aDownloadId )
            {
            return iMedia[i];
            }
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadCache::Add
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadCache::Add( TUint32 aDownloadId, CMPXMedia* aVideo )
    {
    iPos++;
    iPos %= KVcxMvDownloadFindCacheSize;
    iDownloadId[iPos] = aDownloadId;
    iMedia[iPos]      = aVideo;    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadCache::Delete
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadCache::Delete( CMPXMedia* aVideo )
    {
    for ( TInt i = 0; i < KVcxMvDownloadFindCacheSize; i++ )
        {
        if ( iMedia[i] == aVideo )
            {
            iDownloadId[i] = 0;
            iMedia[i]      = NULL;
            }
        }
    }

// End of file

