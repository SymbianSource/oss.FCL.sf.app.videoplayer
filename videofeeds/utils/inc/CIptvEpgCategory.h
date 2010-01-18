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
* Description:    Category information*
*/




#ifndef CIPTVEPGCATEGORY_H
#define CIPTVEPGCATEGORY_H

#include <e32std.h>
#include "CIptvUtil.h"

/**
 *  Category info describtor.
 *
 *  @lib IptvClientApi.lib
 *  @since Engine 1.0
 */
class CIptvEpgCategory : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CIptvEpgCategory* NewL();

    /**
     * Destructor.
     */
    virtual ~CIptvEpgCategory();
   
private: // New functions

    /**
     * C++ default constructor.
     */
    CIptvEpgCategory();

public: // Data

    /**
     * Unique key for database indexing.
     */
    TUint32 iKey;

    /**
     * Parent category key, unique key for database indexing.
     */
    TUint32 iParentKey;

    /**
     * Content Id from service provider.
     */
    TBuf<KIptvIdMaxLength> iId;

    /**
     * Category name.
     */
    TBuf<KIptvEpgCategoryNameMaxLength> iName;

    /**
     * Icon file id.
     */
    TUint32 iIconFileId;

    /**
     * ETrue: iIconFilePath is URL where icon can be downloaded.
     * EFalse: iIconFilePath is icons path to local filesystem.
     */
    TBool iIsIconFilePathUrl;

    /**
     * Icon file path, see iUrl.
     */
    TBuf<KIptvEpgCategoryIconFilePathMaxLength> iIconFilePath;

    /**
     * Category description.
     */
    TBuf<KIptvEpgCategoryDescriptionMaxLength> iDescription;

    /**
     * Indicates when the item was published.
     */
    TTime iPubDate;

    /**
     * Number of items in this category.
     */
    TUint32 iContentCount;
    };

#endif  // CIPTVEPGCATEGORY.H
