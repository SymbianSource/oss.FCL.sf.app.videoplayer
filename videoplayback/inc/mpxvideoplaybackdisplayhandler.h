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
* Description:   Implementation of video playback display handler
*
*/


// Version : %version: 10 %


#ifndef __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__
#define __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__

// INCLUDES

#include <mpxmessage2.h>
#include <MMFScalingCustomCommandConstants.h>

#ifdef SYMBIAN_BUILD_GCE
#include <mediaclientvideodisplay.h>
#endif

#include <mpxvideoplaybackdefs.h>

//
//  CLASS DECLARATION
//
class MMPXPlaybackUtility;
class CMPXVideoPlaybackContainer;
class CMPXVideoPlaybackViewFileDetails;

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
                                                      CMPXVideoPlaybackContainer* aContainer );

        void CreateDisplayWindowL( CWsScreenDevice& aScreenDevice, RWindow& aWin );

        void RemoveDisplayWindow();

        void HandleVideoDisplayMessageL( CMPXMessage* aMessage );

        TInt SetAspectRatioL( TMPXVideoPlaybackCommand aCmd );

        TInt SetDefaultAspectRatioL( CMPXVideoPlaybackViewFileDetails* aFileDetails );

        void DoHandleRealOneBitmapTimeoutL();

    private:

        CMPXVideoPlaybackDisplayHandler( MMPXPlaybackUtility* aPlayUtil,
                                         CMPXVideoPlaybackContainer* aContainer );

        void ConstructL();

        void LoadAspectRatioL();

        void SaveAspectRatioL();


#ifdef SYMBIAN_BUILD_GCE

    private:
        void AddDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                RWindowBase& aWindowBase,
                                RWindow* aWin );

        void SurfaceCreatedL( CMPXMessage* aMessage );
        void SurfaceChangedL( CMPXMessage* aMessage );
        void SurfaceRemoved();
        TInt SetNgaAspectRatioL( TMPXVideoPlaybackCommand aCmd );

        void SignalSurfaceRemovedL();

#endif

    private:
        MMPXPlaybackUtility*                iPlaybackUtility;
        CMPXVideoPlaybackContainer*         iContainer;

        RArray<TMPXAspectRatio>             iAspectRatioArray;
        TInt                                iCurrentIndexForAspectRatio;
        TReal                               iDisplayAspectRatio;

#ifdef SYMBIAN_BUILD_GCE
        CMediaClientVideoDisplay*           iVideoDisplay;

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
#endif

};

#endif // __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__
