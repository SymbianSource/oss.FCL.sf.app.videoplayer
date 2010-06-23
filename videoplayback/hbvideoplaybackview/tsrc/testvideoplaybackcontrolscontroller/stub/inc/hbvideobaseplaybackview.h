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

// Version : %version: 3 %



// This file defines the API for .dll

#ifndef __HBVIDEOBASEPLAYBACKVIEW_H__
#define __HBVIDEOBASEPLAYBACKVIEW_H__

//  Include Files
#include <hbview.h>

class CMPXVideoViewWrapper;
class RWindow;

//  Constants

//  Forward Declarations

//  Class Definitions

class HbVideoBasePlaybackView : public HbView
{
    Q_OBJECT

    public:
        HbVideoBasePlaybackView();
        virtual ~HbVideoBasePlaybackView();

    signals:
        void tappedOnScreen();

    public slots:
        void closePlaybackView();

    public:
        bool mViewClosed;

};

#endif  // __HBVIDEOBASEPLAYBACKVIEW_H__

// EOF
