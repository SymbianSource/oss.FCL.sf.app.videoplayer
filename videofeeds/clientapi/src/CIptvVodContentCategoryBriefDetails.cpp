/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Defines category data class structure for the API*
*/




#include <e32base.h>
#include <s32strm.h>
#include "IptvDebug.h"
#include "CIptvVodContentCategoryBriefDetails.h"

const TInt KTUint32SizeInBytes( 4 );

#ifdef _DEBUG

    _LIT( KIptvDatePrint, "%D%M%Y%/0%1%/1%2%/2%3%/3" );
    _LIT( KIptvTimePrint, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B" );

#endif

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CIptvVodContentCategoryBriefDetails*
    CIptvVodContentCategoryBriefDetails::NewL()
    {
    CIptvVodContentCategoryBriefDetails* self =
        new ( ELeave ) CIptvVodContentCategoryBriefDetails();

    return self;
    }

// --------------------------------------------------------------------------
// C++ destructor
// --------------------------------------------------------------------------
//
CIptvVodContentCategoryBriefDetails::~CIptvVodContentCategoryBriefDetails()
    {
    }

// --------------------------------------------------------------------------
// C++ default constructor
// --------------------------------------------------------------------------
//
CIptvVodContentCategoryBriefDetails::CIptvVodContentCategoryBriefDetails()
    {
    }

// --------------------------------------------------------------------------
// Serialize data content to stream.
// --------------------------------------------------------------------------
//
EXPORT_C void CIptvVodContentCategoryBriefDetails::ExternalizeL(
    RWriteStream& aStream ) const
    {
    // Category Id
    aStream.WriteUint32L( iCategoryId );

    // Is search category
    aStream.WriteUint32L( iIsSearchCategory );

    // Category name
    CIptvUtil::WriteDesToStreamL( iName, aStream );

#ifdef _DEBUG

    // PubDate
    TBuf<100> datePrint;
    iPubDate.FormatL( datePrint, KIptvDatePrint );
    IPTVLOGSTRING2_LOW_LEVEL(
        "CIptvVodContentCategoryBriefDetails::ExternalizeL -- Category publish date : %S",
        &datePrint );

    TBuf<100> timePrint;
    iPubDate.FormatL( timePrint, KIptvTimePrint );
    IPTVLOGSTRING2_LOW_LEVEL(
        "CIptvVodContentCategoryBriefDetails::ExternalizeL -- Category publish time : %S",
        &timePrint );

#endif

    TInt64 time = iPubDate.Int64();    // Obsolete field
    TUint32 lower = static_cast<TInt32>( ( 0x00000000FFFFFFFFULL ) & time );
    TUint32 upper = static_cast<TInt32>( ( 0x00000000FFFFFFFFULL ) & ( time >> 32 ) );
    aStream.WriteUint32L( lower );
    aStream.WriteUint32L( upper );

    // Thumbnail path - Obsolete field
    CIptvUtil::WriteDesToStreamL( iThumbnailPath, aStream );

    // Content count
    aStream.WriteUint32L( iContentCount );
    }

// --------------------------------------------------------------------------
// Read serialized data content from stream.
// --------------------------------------------------------------------------
//
EXPORT_C void CIptvVodContentCategoryBriefDetails::InternalizeL(
    RReadStream& aStream )
    {
    // Category Id
    iCategoryId = aStream.ReadUint32L();

    iIsSearchCategory = aStream.ReadUint32L();

    // Category name
    CIptvUtil::ReadDesFromStreamL( iName, aStream );

    // PubDate - Obsolete field
    TUint32 lower = aStream.ReadUint32L();
    TUint32 upper = aStream.ReadUint32L();
    TInt64 time = 0;
    time = static_cast<TInt64>( lower );
    TInt64 longUpper = static_cast<TInt64>( upper );
    longUpper = longUpper << 32;
    longUpper = longUpper & ( 0xFFFFFFFF00000000ULL );
    time |= longUpper;
    iPubDate = time;

#ifdef _DEBUG

    TBuf<100> datePrint;
    iPubDate.FormatL( datePrint, KIptvDatePrint );
    IPTVLOGSTRING2_LOW_LEVEL(
        "CIptvVodContentCategoryBriefDetails::InternalizeL -- Category publish date : %S",
        &datePrint );

    TBuf<100> timePrint;
    iPubDate.FormatL( timePrint, KIptvTimePrint );
    IPTVLOGSTRING2_LOW_LEVEL(
        "CIptvVodContentCategoryBriefDetails::InternalizeL -- Category publish time : %S",
        &timePrint );

#endif

    // Thumbnail path - Obsolete field
    CIptvUtil::ReadDesFromStreamL( iThumbnailPath, aStream );

    // Content count
    iContentCount = aStream.ReadUint32L();
    }

// --------------------------------------------------------------------------
// Count externalized size.
// --------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvVodContentCategoryBriefDetails::CountExternalizeSize()
    {
    TUint32 size = 0;

    size =
        // Category Id
        KTUint32SizeInBytes +

        // iIsSearchCategory
        KTUint32SizeInBytes +

        // Name
        2 + ( iName.Length() * 2 ) +

        // PubDate - Obsolete field
        8 +

        // Thumbnail path - Obsolete field
        2 + ( iThumbnailPath.Length() * 2 ) +

        // Content count
        KTUint32SizeInBytes;

    return size;
    }
