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


#ifndef __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__
#define __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__

// INCLUDES

#include <mpxmessage2.h>
#include <MMFScalingCustomCommandConstants.h>
#include <mpxvideoplaybackdefs.h>
#include <mediaclientvideodisplay.h>


// 
//  CLASS DECLARATION
//
class CMPXVideoContainer;
class MMPXPlaybackUtility;
class CMPXVideoViewWrapper;
class QMPXVideoPlaybackViewFileDetails;

/*
 *  CMPXVideoPlaybackDisplayHandler
 *
 */
class CMPXVideoPlaybackDisplayHandler : public CBase
{
    //
    //  To save user's preference for scaling type in video ratio + screen ratio
    //
    typedef struct
    {
        TReal32         videoRatio;
        TReal32         screenRatio;
        TMMFScalingType scalingType;
    } TMPXAspectRatio ;
    
    public:

        ~CMPXVideoPlaybackDisplayHandler();

        static CMPXVideoPlaybackDisplayHandler* NewL( MMPXPlaybackUtility* aPlayUtil,
                                                      CMPXVideoViewWrapper* aViewWrapper );

        void CreateDisplayWindowL( RWsSession& aWs,
                                   CWsScreenDevice& aScreenDevice,
                                   RWindow& aWin,
                                   TRect aDisplayRect );

        void RemoveDisplayWindow();

        void HandleVideoDisplayMessageL( CMPXMessage* aMessage );

        TInt SetAspectRatioL( TMPXVideoPlaybackCommand aCmd );
        
        TInt SetDefaultAspectRatioL( QMPXVideoPlaybackViewFileDetails* aFileDetails, 
                                     TReal aDisplayAspectRatio );

        void UpdateVideoRectL( TRect aRect, TBool transitionEffect );

    private:

        CMPXVideoPlaybackDisplayHandler( MMPXPlaybackUtility* aPlayUtil,
                                         CMPXVideoViewWrapper* aViewWrapper );

        void ConstructL();
        
        void LoadAspectRatioL();
        
        void SaveAspectRatioL();

        void SetVideoRectL( TRect aClipRect );

        void CalculateVideoRectL();

        static TInt UpdateVideoRectTimeOutL( TAny* aPtr );

    private:
        void AddDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                RWindowBase& aWindowBase,
                                RWindow* aWin );

        void SurfaceCreatedL( CMPXMessage* aMessage );
        void SurfaceChangedL( CMPXMessage* aMessage );
        void SurfaceRemoved();
        TInt SetNgaAspectRatioL( TMPXVideoPlaybackCommand aCmd );

    private:
        MMPXPlaybackUtility*                iPlaybackUtility;

        RArray<TMPXAspectRatio>             iAspectRatioArray;
        TInt                                iCurrentIndexForAspectRatio;
        TReal                               iDisplayAspectRatio;

        TRect                               iWindowRect;

        TReal32                             iTlXDiff;
        TReal32                             iTlYDiff;
        TReal32                             iBrXDiff;
        TReal32                             iBrYDiff;

        TInt                                iTransitionEffectCnt;

        CPeriodic*                          iResizingTimer;
        CMPXVideoViewWrapper*               iViewWrapper;

        CMediaClientVideoDisplay*           iVideoDisplay;

        RWindowBase*                        iWindowBase;
        TBool                               iSurfaceCached;
        TSurfaceId                          iSurfaceId;
        TRect                               iCropRect;
        TVideoAspectRatio                   iAspectRatio;
        TReal32                             iScaleWidth;
        TReal32                             iScaleHeight;
        TInt                                iHorizontalPosition;
        TInt                                iVerticalPosition;
        TVideoRotation                      iRotation;
        TAutoScaleType                      iAutoScale;
        CMPXVideoContainer*                 iVideoContainer;
};

#endif // __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__
