/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  hb videoplaybackview
*
*/

// Version : %version:  da1mmcf#4 %



#ifndef __HBVIDEOPLAYBACKVIEW_H__
#define __HBVIDEOPLAYBACKVIEW_H__

//  Include Files
#include "hbvideobaseplaybackview.h"
#include "videoplaybackexport.h"


/**
 *  HbVideoPlaybackView
 * 
 */
class VIDEOPLAYBACK_DLL_EXPORT HbVideoPlaybackView : public HbVideoBasePlaybackView
{
    Q_OBJECT

    public:
        HbVideoPlaybackView();
        virtual ~HbVideoPlaybackView();

        void handleActivateView(); 
        
        void handleDeactivateView();
                
        void handleBack();

    private:
        
        virtual void handleSoftkeyBack();

        virtual void handlePluginError( int aError );

        /*
         *  Handle Download State Change
         *  @param  aState  new state of the download
         */
        virtual void handlePdlStateChange( int aState );

        /*
         *  Handle transition to the stopped state
         */
        virtual void handleStoppedState();
};

#endif  // __HBVIDEOPLAYBACKVIEW_H__

// EOF
