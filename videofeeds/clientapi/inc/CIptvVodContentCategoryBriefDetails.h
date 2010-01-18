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




#ifndef CIPTVVODCONTENTCATEGORYBRIEFDETAILS_H
#define CIPTVVODCONTENTCATEGORYBRIEFDETAILS_H

#include <e32std.h>
#include "CIptvUtil.h"

class RWriteStream;
class RReadStream;

/**
 *  VoD ECG content API category info holder.
 *
 *  @lib IptvClientApi.lib
 *  @since Engine 1.0
 */
class CIptvVodContentCategoryBriefDetails : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CIptvVodContentCategoryBriefDetails* NewL();

    /**
     * Serialize data content to stream.
     *
     * @param aStream Write stream object to write data.
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Read serialized data content from stream.
     *
     * @param aStream Read stream object to read data.
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

    /**
     * Count externalized size.
     */
    IMPORT_C TUint32 CountExternalizeSize();

    /**
     * Destructor.
     */
    virtual ~CIptvVodContentCategoryBriefDetails();

protected:

    /**
     * C++ default constructor.
     */
    CIptvVodContentCategoryBriefDetails();

public: // Data

    /**
     * Category ID identifier.
     */
    TIptvCategoryId iCategoryId;

    /**
     * Category name string.
     */
    TBuf<KIptvEpgCategoryNameMaxLength> iName;

    /**
     * Indicates when the item was published. In RSS 2.0 specification
     * <pubDate>
     */
    TTime iPubDate; // Obsolete field

    /**
     * Complete thumbnail load path, field is currently obsolete.
     */
    TBuf<KIptvEpgCategoryIconFilePathMaxLength> iThumbnailPath;

    /**
     * Clas describes a search category.
     */
    TBool iIsSearchCategory;

    /**
     * Content count
     */
    TUint32 iContentCount;
    };

#endif  // CIPTVVODCONTENTCATEGORYBRIEFDETAILS_H
