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
* Description:  HB Video playback view
*
*/

// Version : %version:  2 %



// This file defines the API for .dll

#ifndef __HBVIDEOPLAYBACKVIEW_H__
#define __HBVIDEOPLAYBACKVIEW_H__

//  Include Files
#include <hbview.h>
//#include <qgraphicswidget.h>

//  Constants

//  Forward Declarations

//  Class Definitions

class HbVideoPlaybackView : public HbView
{
    Q_OBJECT

    public:
        IMPORT_C HbVideoPlaybackView();
        IMPORT_C virtual ~HbVideoPlaybackView();

        IMPORT_C void handleActivateView(); 
        
        IMPORT_C void handleDeactivateView();
                
        IMPORT_C void handleBack();
        
        void handleSoftkeyBack();
        
        void handlePluginError( int aError );
        
        void handlePdlStateChange( int aState );
        
        void handleStoppedState();
				
};

#endif  // __HBVIDEOPLAYBACKVIEW_H__

// EOF
