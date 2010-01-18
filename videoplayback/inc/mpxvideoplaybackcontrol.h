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
* Description:  Implementation of CMPXVideoPlaybackControl
*
*/

// Version : %version: 3 %


#ifndef MPXPLAYBACKCONTROL_H_
#define MPXPLAYBACKCONTROL_H_

// INCLUDES
#include <coecntrl.h>
#include <mpxplaybackframeworkdefs.h>

#include "mpxhelixplaybackplugindefs.h"
#include "mpxvideoplaybackcontrol.hrh"

// FORWARD DECLARATIONS
class CMPXVideoPlaybackControlsController;
class CMPXVideoPlaybackViewFileDetails;

#ifdef RD_TACTILE_FEEDBACK
class MTouchFeedback;
#endif //RD_TACTILE_FEEDBACK

// DATA TYPES

// CLASS DECLARATION

class CMPXVideoPlaybackControl : public CCoeControl
{

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackControl* NewL( CMPXVideoPlaybackControlsController* aController,
                                               CCoeControl* aControl,
                                               TRect aRect,
                                               TMPXVideoPlaybackControls aControlIndex,
                                               TUint aProperties );

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackControl();

    protected:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackControl( CMPXVideoPlaybackControlsController* aController,
                                  CCoeControl* aControl,
                                  TMPXVideoPlaybackControls aControlIndex,
                                  TUint aProperties );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( TRect aRect );

    protected:

        /**
        * From CCoeControl.
        * Gets the number of controls contained in a compound control.
        *
        * @return The number of component controls contained by this control.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl.
        * Gets an indexed component of a compound control.
        *
        * @param aIndex The index of the control.
        * @return The component control with an index of aIndex.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl,Draw.
        * @param aRect drawable area.
        */
        void Draw(const TRect& aRect) const;

        /**
        * From CCoeControl.
        * Handle events from the touch screen
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent);

    public:

        /**
        * Set visibility of each control
        */
        void SetVisibility( TMPXPlaybackState aState );

        /**
        * return control index
        */
        TMPXVideoPlaybackControls ControlIndex();

        /**
        * set changed volume
        */
        TBool VolumeChanged( TInt aVolume );

        /**
        * set changed duration
        */
        TBool DurationChangedL( TInt aDuration );

        /**
        * set changed volume
        */
        TBool PositionChangedL( TInt aPosition );

        /**
        * Set changed position
        */
        TBool AspectRatioChanged( TInt aAspectRatio );

        /**
        * set changed volume
        */
        TBool SetDownloadSize( TInt aSize );

        /**
        * set changed state
        */
        TBool UpdateStateOnButtonBar( TMPXPlaybackState aState );

        /*
         *  UpdateDownloadPosition
         *  updates the download ratio on the progress bar
         */
        TBool UpdateDownloadPosition( TInt aSize );

        /*
         *  Update the controls with the file details
         */
        void UpdateControlsWithFileDetailsL( CMPXVideoPlaybackViewFileDetails* aDetails );

        /**
        *  Set the visibility of the control to false unless it is a PDL control
        */
        void HideControl();

        /*
         *  Stop Animation Branding Timer
         */
        TBool StopBrandingAnimationTimer();

        IMPORT_C void DoHandlePointerEventL( const TPointerEvent& aPointerEvent );

        void ResetControl();

    protected:

        CMPXVideoPlaybackControlsController* iController;
        CCoeControl*                         iControl;
        TMPXVideoPlaybackControls            iControlIndex;
        TUint                                iProperties;
        TMPXVideoMode                        iPlaybackMode;

#ifdef RD_TACTILE_FEEDBACK
        MTouchFeedback*                      iFeedback;
#endif //RD_TACTILE_FEEDBACK

};

#endif /*MPXPLAYBACKCONTROL_H_*/

// End of File
