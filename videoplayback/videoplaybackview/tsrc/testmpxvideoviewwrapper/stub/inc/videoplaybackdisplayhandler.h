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
* Description:   Implementation of video playback display handler
*
*/

// Version : %version:  8 %


#ifndef __CVIDEOPLAYBACKDISPLAYHANDLER_H__
#define __CVIDEOPLAYBACKDISPLAYHANDLER_H__

// INCLUDES

#include <mpxmessage2.h>
#include <mmfscalingcustomcommandconstants.h>
#include <mpxvideoplaybackdefs.h>
#include <mediaclientvideodisplay.h>


// 
//  CLASS DECLARATION
//
class MMPXPlaybackUtility;
class CMPXVideoViewWrapper;
class VideoPlaybackViewFileDetails;

/*
 *  CVideoPlaybackDisplayHandler
 *
 */
class CVideoPlaybackDisplayHandler : public CBase
{
    public:

        ~CVideoPlaybackDisplayHandler();

        static CVideoPlaybackDisplayHandler* NewL( MMPXPlaybackUtility* aPlayUtil,
                                                   CMPXVideoViewWrapper* aViewWrapper );

        void CreateDisplayWindowL( RWsSession& aWs,
                                   CWsScreenDevice& aScreenDevice,
                                   RWindow& aWin,
                                   TRect aDisplayRect );

        void RemoveDisplayWindow();

        void HandleVideoDisplayMessageL( CMPXMessage* aMessage );

        TInt SetAspectRatioL( TMPXVideoPlaybackCommand aCmd );
        
        TInt SetDefaultAspectRatioL( VideoPlaybackViewFileDetails* aFileDetails, 
                                     TReal32 aDisplayAspectRatio );

        void UpdateVideoRectL( TRect aRect, TBool transitionEffect );

    private:

        CVideoPlaybackDisplayHandler( MMPXPlaybackUtility* aPlayUtil,
                                      CMPXVideoViewWrapper* aViewWrapper );

        void ConstructL();

    public:
        MMPXPlaybackUtility*                iPlaybackUtility;
        CMPXVideoViewWrapper*               iViewWrapper;
        CMediaClientVideoDisplay*           iVideoDisplay;

        TRect                               iRect;
        TInt                                iAspectRatio;
        TInt                                iCommand;
};

#endif // __CVIDEOPLAYBACKDISPLAYHANDLER_H__
