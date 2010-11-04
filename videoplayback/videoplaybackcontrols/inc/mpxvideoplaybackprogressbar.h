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
* Description: Progress bar control
*
*/

// Version : %version: 6 %



#ifndef MPXVIDEOPLAYBACKPROGRESSBAR_H
#define MPXVIDEOPLAYBACKPROGRESSBAR_H

//  INCLUDES
#include <coecntrl.h>

#include "mpxhelixplaybackplugindefs.h"


// FORWARD DECLARATIONS
class CEikLabel;
class CMPXVideoPlaybackViewFileDetails;
class CMPXVideoPlaybackControlsController;

#ifdef RD_TACTILE_FEEDBACK
class MTouchFeedback;
#endif //RD_TACTILE_FEEDBACK


class CMPXVideoPlaybackProgressBar : public CCoeControl
{
    public:
        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackProgressBar* NewL( CMPXVideoPlaybackControlsController* aController,
                                                   TRect aRect );

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackProgressBar();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackProgressBar( CMPXVideoPlaybackControlsController* aController );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( TRect aRect );

    private: // from CoeControl

        /**
        * From CoeControl,CountComponentControls.
        * @return Number of contained component controls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        * @param aIndex index of a contained component control.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl,Draw.
        * @param aRect drawable area.
        */
        void Draw( const TRect& aRect ) const;

    public: // from CoeControl

        /**
        * From CoeControl,OfferKeyEventL
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

        /**
        * From CCoeControl.
       */
        void HandleResourceChange( TInt aType );

    public:

        /**
        * Set changed duration
        * @param aDur the duration text to be displayed
        */
        void DurationChangedL( TInt aDuration );

        /**
        * Set changed position
        * @param aPos the position text to be displayed
        * @since 5.0
        */
        void PositionChangedL( TInt aPosition, TBool aUserChanged = EFalse );

        /*
        *  Sets the size of download
        *  @param aSize   Download size
        *  @since 5.0
        */
        void SetDownloadSize( TInt aSize );
        
        /*
        *  Updates the download size and calculates the download progress ratio
        *  @param aSize   Updated Download Size
        *  @since 5.0
        */
        void UpdateDownloadPosition( TInt aSize );
        
        /*
         *  Creates the needed resource if the playback mode changes
         *  @since 5.0
         */
        void UpdateProgressBarStateL( CMPXVideoPlaybackViewFileDetails* aDetails );
        
        /*
         *  Abandons any ongoing pointer input 
         *  @since 5.0
         */        
        void Reset();

    private: //new fuctions
        /**
        * Set layout for each icon
        */
        void SetLayoutL();

        /**
        * Creates new skins for the control.
        * @since 5.0
        * @return void
        */
        void SkinChangeL();

        /*
        *  change times to readable format for MM:SS
        *  @param aTime need to change aTime to readable format
        *  @param aLabel label to assing readable time
        */
        void SetTextLWithReadableTimeL( TInt aTime, CEikLabel* aLabel );
        
        //
        //  Performs logic for the button down events
        //
        void HandleButtonDownEventL( TReal aRatio );

        //
        //  Performs logic for the dragging events
        //
        void HandleDraggingEventL( TReal aRatio );
        
        //
        //  Performs logic for the button up events
        //
        void HandleButtonUpEventL( TReal aRatio );
        
    private:    // Data
        CMPXVideoPlaybackControlsController* iController;

        CGulIcon*               iFrameIcon;
        CGulIcon*               iPlaybackIcon;
        CGulIcon*               iDownloadIcon;
        CGulIcon*               iSliderIcon;
        CGulIcon*               iSelectedSliderIcon;

        TRect                   iFrameIconRect;
        TRect                   iPlaybackRect;
        TRect                   iDownloadRect;
        TRect                   iSliderRect;

        TReal                   iDuration;
        TReal                   iPlaybackRatio;
        TReal                   iDownloadRatio;

        CEikLabel*              iPositionLabel;
        CEikLabel*              iDurationLabel;

        HBufC*                  iMinSecFormatString;
        HBufC*                  iHourMinSecFormatString;
        HBufC*                  iLiveStreamingString;

        TBool                   iDragging;
        TBool                   iPointerEventStarted;
        TBool                   iWasPlaying;
        
        TInt                    iDownloadSize;
        TInt                    iOldDLSize;

#ifdef RD_TACTILE_FEEDBACK
        MTouchFeedback*         iFeedback;
#endif //RD_TACTILE_FEEDBACK

};

#endif //MPXVIDEOPLAYBACKPROGRESSBAR_H

// End of File
