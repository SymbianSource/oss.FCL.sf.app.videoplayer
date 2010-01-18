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
* Description:  Popup control for full screen container
*
*/

// Version : %version: 9 %


#ifndef MPXVIDEOPLAYBACKBUTTONBAR_H
#define MPXVIDEOPLAYBACKBUTTONBAR_H

//  INCLUDES
#include <coecntrl.h>
#include <mpxplaybackframeworkdefs.h>

#include "mpxhelixplaybackplugindefs.h"
#include "mpxvideo_debug.h"

// FORWARD DECLARATIONS
class CMPXVideoPlaybackButton;
class CMPXVideoPlaybackViewFileDetails;
class CMPXVideoPlaybackControlsController;

#ifdef RD_TACTILE_FEEDBACK
class MTouchFeedback;
#endif //RD_TACTILE_FEEDBACK

// DATA TYPES
enum TMPXButton
{
    EMPXButtonFastForward,
    EMPXButtonPlay,
    EMPXButtonRewind,
    EMPXButtonPause,
    EMPXButtonCount    // Should always be the last value
};

enum TMPXButtonPressed
{
    EMPXNotPressed,
    EMPXTopButtonPressed,
    EMPXMiddleButtonPressed,
    EMPXBottomButtonPressed
};


class CMPXVideoPlaybackButtonBar : public CCoeControl
{
    public:
        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackButtonBar* NewL( CMPXVideoPlaybackControlsController* aController,
                                                 TRect aRect );

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackButtonBar();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackButtonBar( CMPXVideoPlaybackControlsController* aController );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( TRect aRect );

    private:

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
        * From CoeControl,HandleResourceChange
        */
        void HandleResourceChange( TInt aType );

        /**
        * From CoeControl,SetContainerWindowL
        */
        void SetContainerWindowL( const CCoeControl& aContainer );

        /**
        * From CoeControl,OfferKeyEventL
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    public:

        /**
        * Set changed state
        * @return void
        */
        void UpdateStateOnButtonBar( TMPXPlaybackState aState );

        /*
         *  Update button's state based on file details
         *  @since 5.0
         */
        void UpdateButtonBarState( CMPXVideoPlaybackViewFileDetails* aDetails  );

        /*
         *  Bring the button bar to its original state
         *  @since 5.0
         */
        void Reset();

    private:
        /**
        * Creates buttons
        * @return void
        */
        void CreateButtonsL();

        /**
        * Creates new skins for the control.
        * @return void
        */
        void SkinChangeL();

        //
        //  Performs logic for the button down events
        //
        void HandleButtonDownEventL( const TPointerEvent& aPointerEvent );

        //
        //  Performs logic for the button up events for top button
        //
        void HandleTopButtonUpEventL( const TPointerEvent& aPointerEvent );

        //
        //  Performs logic for the button up events for middle button
        //
        void HandleMiddleButtonUpEventL();

        //
        //  Performs logic for the button up events for bottom button
        //
        void HandleBottomButtonUpEventL( const TPointerEvent& aPointerEvent );

        //
        //  Callback fuction of iSeekingTimer
        //
        static TInt StartSeekingL( TAny* aPtr );

        //
        //  Start seeking
        //
        void DoStartSeekingL();

    private:    // Data
        CArrayPtrFlat< CMPXVideoPlaybackButton >  iButtons;
        CMPXVideoPlaybackControlsController* iController;

        TBool                                iSeeking;

        TRect                                iTopButtonRect;
        TRect                                iMiddleButtonRect;
        TRect                                iBottomButtonRect;

        TMPXButtonPressed                    iButtonPressed;
        CPeriodic*                           iSeekingTimer;

#ifdef RD_TACTILE_FEEDBACK
        MTouchFeedback*                      iFeedback;
#endif //RD_TACTILE_FEEDBACK
};

#endif //MPXVIDEOPLAYBACKBUTTONBAR_H

// End of File
