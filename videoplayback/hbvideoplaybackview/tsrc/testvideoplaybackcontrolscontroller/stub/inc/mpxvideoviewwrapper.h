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
* Description:  Stub MPX Video Wrapper
*
*/

// Version : %version:  1 %



// This file defines the API for .dll

#ifndef __MPXVIDEOVIEWWRAPPER_H__
#define __MPXVIDEOVIEWWRAPPER_H__

//  Include Files

#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf
#include <mpxplaybackframeworkdefs.h>

#include "mpxvideoplaybackcontrol.hrh"

//  Constants

//  Forward Declarations
class HbVideoBasePlaybackView;
class QMPXVideoPlaybackViewFileDetails;


//  Class Definitions

class CMPXVideoViewWrapper : public CBase
{
    public:
        static CMPXVideoViewWrapper* NewL( HbVideoBasePlaybackView* aView );
        virtual ~CMPXVideoViewWrapper();

        CMPXVideoViewWrapper( HbVideoBasePlaybackView* aView );        
        void ConstructL();

    public:
        void HandleCommandL( TInt aCommand );
                             
        void SetPropertyL( TMPXPlaybackProperty aProperty, TInt aValue );

        void UpdateVideoRect( TInt aX, TInt aY, TInt aWidth, TInt aHeight, TBool transitionEffect );

        
    public: // data

        HbVideoBasePlaybackView*             mView;
        TBool                                mMediaRequested;
        QMPXVideoPlaybackViewFileDetails*    mFileDetails;
        TBool                                mClosePlayerAO;
        TBool                                mForeground;
        int                                  mProperty;
        int                                  mCommandId;
};

#endif  // __MPXVIDEOVIEWWRAPPER_H__

// EOF
