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
* Description:    Descriptor of a download item.*
*/




#ifndef CIPTVDOWNLOADITEM_H
#define CIPTVDOWNLOADITEM_H

#include <e32std.h>
#include "CIptvUtil.h"

//lint -e1925 Use of public variables are architechtural decision.

/**
 *  Descriptor of a download item.
 *
 *  Holds information associated to a download item
 *
 *  @lib IptvRssPlugin.dll
 *  @since S60 v3.1
 */
class CIptvDownloadItem : public CBase
    {

public:

    static CIptvDownloadItem* NewL();

    virtual ~CIptvDownloadItem();

private:

    CIptvDownloadItem();

public: // Data members

    /**
     * Thumbnail URL.
     */
    TBuf<KIptvCAContentUrlLength> iThumbnailUrl;

    /**
     * If ETrue thumbnail is service thumbnail, else thumbnail is
     * content thumbnail.
     */
    TBool iServiceThumbnail;

    /**
     * Category / content id.
     */
    TUint32 iId;

    };

#endif  // CIPTVDOWNLOADITEM_H
