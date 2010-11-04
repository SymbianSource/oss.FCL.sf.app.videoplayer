/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Application class
*
*/

// Version : %version: da1mmcf#4 %


#ifndef CMPXVIDEOPLAYERAPPLICATION_H
#define CMPXVIDEOPLAYERAPPLICATION_H

#include <aknapp.h>                    // CAknApplication

/**
*  Application class.
*  Creates concrete document object.
*
*  @lib mpxvideoplayer.exe 
*  @since MpxVideoPlayer 0.1
*/
class CMpxVideoPlayerApplication : public CAknApplication
    {
    private:

        /**
        * From CApaApplication, creates document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID.
        * @return The value of KUidMacFwApplication.
        */
        TUid AppDllUid() const;
    };

#endif       // CMPXVIDEOPLAYERAPPLICATION_H
