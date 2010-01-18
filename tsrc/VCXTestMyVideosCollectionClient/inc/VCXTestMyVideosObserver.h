/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef VCXTESTMYVIDEOSOBSERVER_H_
#define VCXTESTMYVIDEOSOBSERVER_H_

// INCLUDES

#include <e32base.h>

// CLASS DECLARATION

/**
*  MVCXTestMyVideosObserver 
*
*  @lib ?library
*  @since ?Series60_version
*/
class MVCXTestMyVideosObserver
    {
    public:
        enum TVCXTestVideoEvent 
            {
            EVCXTestVideoGeneral, // aError = error code
            EVCXTestVideoListRefreshed,
            EVCXTestVideoVideoAdded, // aParam1 = video id
            EVCXTestVideoVideoDeleted, // aParam1 = video id
            EVCXTestVideoDownloadStarted, // aParam1 = video id
            EVCXTestVideoDownloadPaused, // aParam1 = video id
            EVCXTestVideoDownloadFailed, // aParam1 = video id, aError = download error.
            EVCXTestVideoDownloadCompleted, // aParam1 = video id
            EVCXTestVideoDownloadProgressed, // aParam1 = video id, aParam2 = download progress
            EVCXTestVideoAllDownloadsCompleted,
            };
        
        /**
         *   
         */
        virtual void VideoEventL( TVCXTestVideoEvent aEvent, TUint32 aParam1, TUint32 aParam2, TInt aError ) = 0;
    };

#endif /* VCXTESTMYVIDEOSOBSERVER_H_ */
