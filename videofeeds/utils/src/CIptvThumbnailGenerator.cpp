/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Thumbnail generation wrapper*
*/




#include "IptvDebug.h"
#include "CIptvThumbnailImpl.h"
#include "CIptvThumbnailObserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
EXPORT_C CIptvThumbnailGenerator* CIptvThumbnailGenerator::NewL( MIptvThumbnailObserver& aObserver, TInt aPriority )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailGenerator::NewL IN" );
    CIptvThumbnailGenerator* self = CIptvThumbnailGenerator::NewLC( aObserver, aPriority );
    CleanupStack::Pop( self );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailGenerator::NewL OUT" );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
EXPORT_C CIptvThumbnailGenerator* CIptvThumbnailGenerator::NewLC( MIptvThumbnailObserver& aObserver, TInt aPriority )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailGenerator::NewLC IN" );
    CIptvThumbnailGenerator* self = CIptvThumbnailImpl::NewLC( aObserver, aPriority );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailGenerator::NewLC OUT" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIptvThumbnailGenerator
// ---------------------------------------------------------------------------
CIptvThumbnailGenerator::CIptvThumbnailGenerator( TInt aPriority )
    : CActive(aPriority)
    {
    }
