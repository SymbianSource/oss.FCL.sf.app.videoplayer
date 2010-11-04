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
* Description:  MPX Video playback view plugin definition.
*
*/

// Version : %version: 3 %



// This file defines the API for mpxvideoplaybackviewplugin.dll

#ifndef __VIDEOPLAYBACKVIEWPLUGIN_H__
#define __VIDEOPLAYBACKVIEWPLUGIN_H__

//  Include Files

#include <e32base.h>
#include <e32std.h>

#include "mpxaknviewplugin.h"

//  Constants


//  Class Definitions

NONSHARABLE_CLASS( CMPXVideoPlaybackViewPlugin ) : public CMPXAknViewPlugin
{
    public:
        static CMPXVideoPlaybackViewPlugin* NewL();
        static CMPXVideoPlaybackViewPlugin* NewLC();
        ~CMPXVideoPlaybackViewPlugin();

    public:
        /*
         * From CMPXAknViewPlugin
         * Construct Avkon view.
         *
         * @since S60 v3.0
         * @return Pointer to a newly created Avkon view.
         */
        CAknView* ConstructViewLC();

    private: 
        CMPXVideoPlaybackViewPlugin();
        void ConstructL();
};

#endif  // __VIDEOPLAYBACKVIEWPLUGIN_H__

// End of File
