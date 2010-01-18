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
* Description:  Implementation of MPXVideoPlaybackControlsController
*
*/

// Version : %version: 7 %


#ifndef MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <mpxplaybackframeworkdefs.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <f32file.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrol.hrh"

// FORWARD DECLARATIONS
class CEikLabel;
class CEikImage;
class MTouchFeedback;
class CMPXVideoPlaybackControl;
class CMPXVideoPlaybackContainer;
class CMPXVideoPlaybackControlPolicy;
class CAknsBasicBackgroundControlContext;
class CMPXVideoPlaybackControlConfiguration;

// DATA TYPES

enum TMPXTimerAction
{
    EMPXTimerCancel,
    EMPXTimerReset
};


// CLASS DECLARATION

class CMPXVideoPlaybackControlsController : public CBase
{

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMPXVideoPlaybackControlsController* NewL(
                CMPXVideoPlaybackContainer* aContainer,
                TRect aRect,
                CMPXVideoPlaybackViewFileDetails* aDetails );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMPXVideoPlaybackControlsController();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackControlsController( CMPXVideoPlaybackContainer* aContainer, TRect aRect );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( CMPXVideoPlaybackViewFileDetails* aDetails );

    public:

        /**
        * Handle event from container
        */
        IMPORT_C void HandleEventL( TMPXVideoPlaybackControlCommandIds aEvent, TInt aValue = 0 );

        IMPORT_C CEikImage* GetBitmap( TMPXVideoPlaybackControls aBitmap );

        //
        //  Add the file details to the controls controller when available
        //
        IMPORT_C void AddFileDetailsL( CMPXVideoPlaybackViewFileDetails* aDetails );

        /**
        * Command handling function.
        * Call HandleCommandL() of container
        */
        void HandleCommandL( TInt aCommand, TInt aValue = 0 );

        /**
        * Return ETrue if TV-out cable gets connected
        */
        inline TBool IsTvOutConnected();

        /*
         * Return ETrue if TV-out cable is connected and content can be played
         */
        inline TBool IsTvOutPlaybackAllowed();

        /**
        * Reset or cancel timers for the controls
        */
        void ResetDisappearingTimers( TMPXTimerAction aTimerAction );

        /**
        * Return state
        */
        inline TMPXPlaybackState State();

        /**
        * Return file details
        */
        inline CMPXVideoPlaybackViewFileDetails* FileDetails();

        //
        //  Locates the bitmap file searching from y: down and then z: last
        //
        void LocateBitmapFileL( TFileName& aFileName );

        void SetRealOneBitmapVisibility( TBool aVisible );

    private:
        /**
        * Create controls
        */
        void CreateControlsL();

        /**
        * Update controls
        */
        void HandleStateChange( TMPXPlaybackState aNewState );

        /**
        * Update Control's visibility
        */
        void UpdateControlsVisibility();

        /**
        * Toggle visibility
        */
        void ToggleVisibility();

        CEikImage* CreateImageL( TFileName& aIconsPath, TInt aBitmapIndex, TInt aBitmapMaskIndex );

        CEikImage* CreateBitmapL( TFileName& aIconsPath,
                                  TInt aBitmapIndex,
                                  const TAknWindowLineLayout& aLayout );

        /**
        * Create/delete controls based on updated control list
        */
        void ControlsListUpdatedL();

        /**
        * Create fake softkeyL
        */
        void CreateFakeSoftKeyL( TInt aResource,
                                 TBool aTop,
                                 TMPXVideoPlaybackControls aControlIndex,
                                 TUint aProperties );

        /**
        * Crate text label with speccific layout
        */
        void CreateTextLabelWithSpecificLayoutL(
                TInt aResource,
                TMPXVideoPlaybackControls aControlIndex,
                TUint aProperties,
                TAknTextComponentLayout aLayout );

        //
        //  Create the bitmaps for Audio Only Clips
        //
        void CreateAudioOnlyBitmapL( TFileName& aIconsPath,
                                     TInt aBitmapIndex,
                                     TInt aBitmapMaskIndex,
                                     TMPXVideoPlaybackControls aControlIndex,
                                     const TAknWindowLineLayout& aLayout,
                                     TUint aProperties );

        //
        //  Create the title control for Audio Only Clips
        //
        void CreateTitleControlL( TMPXVideoPlaybackControls aControlIndex,
                                  TUint aProperties );

        /**
        * Create CEikLabelL
        */
        CEikLabel* CreateTextLabelL( const TDesC& aText );

        /*
        *  Handles the Controls Timer Timout
        */
        static TInt HandleControlsTimeout( TAny* aPtr );

        /**
        * Cancels all disappearing timers and hides all controls
        */
        void HideAllControls();

        /**
        * Show Controls and reset the timers
        */
        void ShowControls();

        /**
        * Return ETrue if any control is visible
        */
        TBool IsVisible();

        /**
        * Append a control based on control index
        */
        void AppendControlL( TMPXVideoPlaybackControls aControlIndex );

        /**
        * Set changed volume
        */
        void VolumeChanged( TInt aVolume );

        /**
        * Set changed duration
        */
        void DurationChangedL( TInt aDuration);

        /**
        * Set changed position
        */
        void PositionChangedL( TInt aPosition );

        /**
        * Set changed position
        */
        void AspectRatioChanged( TInt aAspectRatio );
        /*
         *  Sets the download size on the progress bar
         */
        void SetDownloadSize( TInt aSize );

        /*
         *  Updates the download ratio on the progress bar
         */
        void UpdateDownloadPosition( TInt aNewSize );

        /**
        * Set changed state on button bar
        */
        void UpdateStateOnButtonBar();

        /**
        * Stop branding animation
        */
        void StopBrandingAnimation();

        /**
        * Check whether this clip is real format or not
        */
        TBool IsRealFormatL( const TDesC& aDes );

        /**
        * Check whether this clip is real format or not for streaming/live streaming
        */
        TBool RealFormatForStreamingL( const TDesC& aDes );

        /**
        * Check whether this clip is real format or not for local/progressive donwload
        */
        TBool RealFormatForLocalL();

        /*
         *  Create the Real One bitmap
         */
        void CreateRealOneBitmapL();

        /**
        * Handle errors
        */
        void HandleErrors();

        /**
        * Return ETrue if control is visible
        */
        TBool IsSoftKeyVisible( TInt aValue );

        /**
        * Handle tvout connected/disconnected event
        */
        void HandleTvOutEventL( TBool aConnected,
                                TMPXVideoPlaybackControlCommandIds aEvent,
                                TInt aValue );
        /**
        * Handle softkey pressed event
        */
        void HandleSoftKeyPressedL( TInt aValue );

        /*
         *  Locate the path of the dll
         */
        TInt AddDllPath( TDes& aFileName );

        /*
         *  Shows the Meida File Details Viewer control
         */
        void ShowMediaDetailsViewerL();

        /*
         *  Checks if Media Details Viewer control is open/visible
         */
        TBool IsMediaDetailsViewerVisible();

        /*
         *  Closes Media Details Viewer
         */
        void CloseMediaDetailsViewer();

    private:

        CMPXVideoPlaybackControlPolicy*         iControlsPolicy;
        CArrayPtrFlat<CMPXVideoPlaybackControl> iControls;
        CMPXVideoPlaybackControl*               iMediaDetailsViewerControl;

        CMPXVideoPlaybackControlConfiguration*  iControlsConfig;

        CEikImage*                              iRealOneBitmap;

        CPeriodic*                              iControlsTimer;

        TRect                                   iRect;
        TInt                                    iVideoResourceOffset;

        TMPXPlaybackState                       iState;

        CMPXVideoPlaybackViewFileDetails*       iFileDetails;     //  not owned

        RFs                                     iFs;
        TFileName                               iBitmapFileName;

#ifdef RD_TACTILE_FEEDBACK
        MTouchFeedback*                         iFeedback;
#endif //RD_TACTILE_FEEDBACK

    public:
        CMPXVideoPlaybackContainer*             iContainer;
};

// INLINE METHODS

inline
TMPXPlaybackState CMPXVideoPlaybackControlsController::State()
{
    return iState;
}

inline
CMPXVideoPlaybackViewFileDetails* CMPXVideoPlaybackControlsController::FileDetails()
{
    return iFileDetails;
}

inline
TBool CMPXVideoPlaybackControlsController::IsTvOutConnected()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::IsTvOutConnected(%d)"),
        iFileDetails->iTvOutConnected);

    return iFileDetails->iTvOutConnected;
}

inline
TBool CMPXVideoPlaybackControlsController::IsTvOutPlaybackAllowed()
{
    TBool playable = ( ! iFileDetails->iTvOutConnected || iFileDetails->iTvOutPlayAllowed );
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::IsTvOutPlaybackAllowed() [%d]"), playable);
    return playable;
}

#endif /*MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_*/

// End of File
