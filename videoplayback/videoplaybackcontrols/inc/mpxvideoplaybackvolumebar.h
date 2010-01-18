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
* Description:  Implementation of CMPXVideoPlaybackVolumeBar
*
*/

// Version : %version: 7 %



#ifndef MPXVIDEOPLAYBACKVOLUMEBAR_H
#define MPXVIDEOPLAYBACKVOLUMEBAR_H

//  INCLUDES
#include <coecntrl.h>


// FORWARD DECLARATIONS
class CMPXVideoPlaybackControlsController;

#ifdef RD_TACTILE_FEEDBACK
class MTouchFeedback;
#endif //RD_TACTILE_FEEDBACK

// DATA TYPES
enum TMPXVolumeDragging
{
   EVolumeNotDragging,
   EVolumeIncreaseDragging,
   EVolumeDecreaseDragging,
   EVolumeSpeakerDragging,
   EVolumeBarDragging
};


class CMPXVideoPlaybackVolumeBar : public CCoeControl
{
    public:
        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackVolumeBar* NewL( CMPXVideoPlaybackControlsController* aController,
                                                 TRect aRect );

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackVolumeBar();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackVolumeBar( CMPXVideoPlaybackControlsController* aController, TRect aRect );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private: // from CoeControl

        /**
        * From CoeControl,CountComponentControls.
        * @return Number of contained component controls.
        */
        TInt CountComponentControls() const;

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
        * Set changed volume
        * @since 5.0
        * @return void
        */
        void VolumeChanged( TInt aVolume );
        
        /*
         *  Abandons any ongoing pointer input 
         *  @since 5.0
         */        
        void Reset();

    private: //new fuctions
        /**
        * Set layout for each icon
        */
        void SetLayout();

        /**
        * Creates new skins for the control.
        * @since 5.0
        * @return void
        */
        void SkinChangeL();
        
        void HandleVolumeIncreaseL( const TPointerEvent& aPointerEvent );
        void HandleVolumeDecreaseL( const TPointerEvent& aPointerEvent );
        void HandleSpeakerControlEventL( const TPointerEvent& aPointerEvent );
        void HandleVolumeBarEventL( const TPointerEvent& aPointerEvent );

        static TInt HandleVolumeDragEventTimeOut( TAny* aPtr );
        void DoHandleVolumeDragEventTimeOut();
        void SetVolumeL( TInt aVolume ); 

    private:    // Data
        CGulIcon*               iSpeakerIcon;
        CGulIcon*               iSpeakerMuteIcon;
        CGulIcon*               iSliderIcon;
        CGulIcon*               iSliderSelectedIcon;
        CGulIcon*               iVolumeUpIcon;
        CGulIcon*               iVolumeDownIcon;
        CGulIcon*               iVolumeFrameIconTop;
        CGulIcon*               iVolumeFrameIconMiddle;
        CGulIcon*               iVolumeFrameIconBottom;
        CGulIcon*               iNoAudioIcon; 

        TReal                   iOneVolumeIncrementHeight;

        TBool                   iMuted;
        TInt                    iVolumeWhileDraggingEvent;

        TRect                   iRect;
        TRect                   iSliderRect;        
        TRect                   iSpeakerRect;
        TRect                   iSpeakerControlRect;
        TRect                   iVolumeUpRect;
        TRect                   iVolumeDownRect;
        TRect                   iVolumeUpControlRect;
        TRect                   iVolumeDownControlRect;
        TRect                   iVolumeBarRect;
        TRect                   iVolumeBarRectTop;
        TRect                   iVolumeBarRectMiddle;
        TRect                   iVolumeBarRectBottom;
        TRect                   iVolumeControlRect;

        TMPXVolumeDragging      iDragging;
        
        CPeriodic*              iDraggingHandlerTimer;

        CMPXVideoPlaybackControlsController* iController;

#ifdef RD_TACTILE_FEEDBACK
        MTouchFeedback*         iFeedback;
#endif //RD_TACTILE_FEEDBACK
};

#endif //MPXVIDEOPLAYBACKVOLUMEBAR_H

// End of File
