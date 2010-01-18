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
* Description:  Popup control for full screen container
*
*/

// Version : %version: 6 %



#ifndef MPXVIDEOPLAYBACKASPECTRATIOICON_H
#define MPXVIDEOPLAYBACKASPECTRATIOICON_H

//  INCLUDES
#include <coecntrl.h>
#include <MMFScalingCustomCommandConstants.h>

// FORWARD DECLARATIONS
class CMPXVideoPlaybackControlsController;

#ifdef RD_TACTILE_FEEDBACK
class MTouchFeedback;
#endif //RD_TACTILE_FEEDBACK

class CMPXVideoPlaybackAspectRatioIcon : public CCoeControl
{
    public:
        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackAspectRatioIcon* NewL( 
                CMPXVideoPlaybackControlsController* aController, TRect aRect );

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackAspectRatioIcon();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackAspectRatioIcon( CMPXVideoPlaybackControlsController* aController );

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
        * From CCoeControl,Draw.
        * @param aRect drawable area.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CoeControl,OfferKeyEventL
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    private:
        /**
        * Creates new skins for the control
        * @return void
        */
        void SkinChangeL();

    public:

        /**
        * change the icon per aspect ratio
        * @return void
        */
        void AspectRatioChanged( TInt aAspectRatio );

    private:    // Data
        CGulIcon*        iNaturalIcon;
        CGulIcon*        iStretchIcon;
        CGulIcon*        iZoomIcon;

        TMMFScalingType  iAspectRatio;
        TRect            iAspectRatioRect;

#ifdef RD_TACTILE_FEEDBACK
        MTouchFeedback*  iFeedback;
#endif //RD_TACTILE_FEEDBACK

        CMPXVideoPlaybackControlsController* iController;
};

#endif //MPXVIDEOPLAYBACKASPECTRATIOICON_H

// End of File
