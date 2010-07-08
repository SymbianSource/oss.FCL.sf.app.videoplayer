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
* Description:   Implementation of video playback display handler
*
*/

// Version : %version:  2 %


#ifndef __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__
#define __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__

// INCLUDES

#include <mpxmessage2.h>
#include <mmfscalingcustomcommandconstants.h>
#include <mpxvideoplaybackdefs.h>
#ifdef SYMBIAN_BUILD_GCE
#include <mediaclientvideodisplay.h>
#else
#include <w32std.h>
#endif


// 
//  CLASS DECLARATION
//
class MMPXPlaybackUtility;
class CMPXVideoViewWrapper;
class VideoPlaybackViewFileDetails;

/*
 *  CMPXVideoPlaybackDisplayHandler
 *
 */
class CMPXVideoPlaybackDisplayHandler : public CBase
#ifndef SYMBIAN_BUILD_GCE
                                      , public MDirectScreenAccess
#endif
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
        
        TInt SetDefaultAspectRatioL( VideoPlaybackViewFileDetails* aFileDetails, 
                                     TReal aDisplayAspectRatio );

        void UpdateVideoRectL(  TRect aRect, TBool transitionEffect  );

    private:

        CMPXVideoPlaybackDisplayHandler( MMPXPlaybackUtility* aPlayUtil,
                                         CMPXVideoViewWrapper* aViewWrapper );

        void ConstructL();
        
        void LoadAspectRatioL();
        
        void SaveAspectRatioL();

        void SetVideoRectL( TRect aClipRect );

        void CalculateVideoRectL();

        static TInt UpdateVideoRectTimeOutL( TAny* aPtr );

#ifdef SYMBIAN_BUILD_GCE

    private:
        void AddDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                RWindowBase& aWindowBase,
                                RWindow* aWin );

        void SurfaceCreatedL( CMPXMessage* aMessage );
        void SurfaceChangedL( CMPXMessage* aMessage );
        void SurfaceRemoved();
        TInt SetNgaAspectRatioL( TMPXVideoPlaybackCommand aCmd );

#else

    private:
        //
        //  MDirectScreenAccess Implementation
        //
        void AbortNow( RDirectScreenAccess::TTerminationReasons aReason );
        void Restart( RDirectScreenAccess::TTerminationReasons aReason );

        TInt CreateAspectRatioCommandL( TMPXVideoPlaybackCommand aCmd );
        
        void SetDisplayWindowL( RWsSession& aWs,
                                CWsScreenDevice& aScreenDevice,
                                RWindowBase& aWin,
                                TRect aClipRect );
        void RestartDsaL();
        void CreateAbortDsaCmdL();

#endif

    public:
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

#ifdef SYMBIAN_BUILD_GCE
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
#else
        CDirectScreenAccess*                iDirectScreenAccess;
#endif

};

#endif // __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__
