/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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

// Version : %version:   5 %



// This file defines the API for .dll

#ifndef __HBVIDEOBASEPLAYBACKVIEW_H__
#define __HBVIDEOBASEPLAYBACKVIEW_H__

//  Include Files
#include <hbview.h>


//  Constants

//  Forward Declarations

//  Class Definitions

class HbVideoBasePlaybackView : public HbView
{
    Q_OBJECT

    public:
        HbVideoBasePlaybackView();
        virtual ~HbVideoBasePlaybackView();

        void handleActivateView(); 
        
        void handleDeactivateView();
        
        virtual void handleClosePlaybackView();   
        
        void startClosingPlaybackView();
        
        virtual void doClosePlayer();

        void setViewFlags( HbView::HbViewFlags flags );
        HbView::HbViewFlags viewFlags();

    protected slots:

        virtual void closePlaybackView();

    public:
        bool mViewActive;
        HbView::HbViewFlags mFlag;
        				
};

#endif  // __HBVIDEOBASEPLAYBACKVIEW_H__

// EOF
