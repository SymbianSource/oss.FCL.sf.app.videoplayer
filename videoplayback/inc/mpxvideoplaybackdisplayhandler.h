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


// Version : %version: 12 %


#ifndef __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__
#define __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__

// INCLUDES

#include <mpxmessage2.h>
#include <MMFScalingCustomCommandConstants.h>
#include <mediaclientvideodisplay.h>
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

        void CreateDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                   RWindow& aWin,
                                   CMPXVideoPlaybackViewFileDetails* aFileDetails );

        void RemoveDisplayWindow();

        void HandleVideoDisplayMessageL( CMPXMessage* aMessage );

        void SetAspectRatioL( TMPXVideoPlaybackCommand aCmd );

        void DoHandleRealOneBitmapTimeoutL();

        TBool ShowAspectRatioIcon();

    private:

        CMPXVideoPlaybackDisplayHandler( MMPXPlaybackUtility* aPlayUtil,
                                         CMPXVideoPlaybackContainer* aContainer );

        void ConstructL();

        void LoadAspectRatioL();

        void SaveAspectRatioL();

        void AddDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                RWindowBase& aWindowBase,
                                RWindow* aWin );

        void SurfaceCreatedL( CMPXMessage* aMessage );
        void SurfaceChangedL( CMPXMessage* aMessage );
        void SurfaceRemoved();

        void SignalSurfaceRemovedL();

        void AttachNewSurfaceToWindowL();

        void CalculateAspectRatioL();

        TBool IsAspectRatioEqual( TReal aRatio1, TReal aRatio2 );

        TReal CalculateVideoAspectRatio();

        void RemoveSurfaceFromPlaybackPluginL();

    private:
        MMPXPlaybackUtility*                iPlaybackUtility;
        CMPXVideoPlaybackContainer*         iContainer;

        RArray<TMPXAspectRatio>             iAspectRatioArray;
        TInt                                iCurrentIndexForAspectRatio;

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

        TReal                               iDisplayAspectRatio;
        TInt                                iVideoHeight;
        TInt                                iVideoWidth;
};

#endif // __CMPXVIDEOPLAYBACKDISPLAYHANDLER_H__
