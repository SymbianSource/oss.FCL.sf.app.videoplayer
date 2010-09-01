/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Live TV image handler observer.*
*/





#ifndef __MIPTV_LIVE_UI_IMAGE_OBSERVER_H__
#define __MIPTV_LIVE_UI_IMAGE_OBSERVER_H__

// INCLUDES
#include <fbs.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Live TV Image handler observer.
*  @since
*/
class MIptvLiveUIImageObserver
    {
    public: // New functions
        
        /**
		* Image conversion is complete.
		* @param aError KErrNone if ok, otherwise system-wide error codes.
		* @param aBitmap Image bitmap, If aError != KErrNone aBitmap is empty.
		*/
        virtual void ImageReadyL( TInt aError, CFbsBitmap& aBitmap ) = 0;
    };

#endif      // __MIPTV_LIVE_UI_IMAGE_OBSERVER_H__

// End of File
