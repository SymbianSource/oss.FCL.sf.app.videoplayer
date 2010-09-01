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
* Description:    Observer for IPTV thumbnail generation*
*/




#ifndef CIPTVTHUMBNAILOBSERVER_H
#define CIPTVTHUMBNAILOBSERVER_H

#include <e32base.h>

/**
*  Observer for thumbnail generation on IPTV
*
*  @lib    IptvUtil
*  @since  S60 v3.1
*/
class MIptvThumbnailObserver
    {
public:
    /**
    *  Notification for thumbnail generation completed
    *  @param aError KErrNone on thumbnail generation success, system error code otherwise
    */
    virtual void ThumbnailCompleted( TInt aError ) = 0;
    };

#endif // CIPTVTHUMBNAILOBSERVER_H
