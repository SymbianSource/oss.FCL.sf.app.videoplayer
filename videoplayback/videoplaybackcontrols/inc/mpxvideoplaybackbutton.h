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

// Version : %version: 7 %


#ifndef MPXVIDEOPLAYBACKBUTTON_H
#define MPXVIDEOPLAYBACKBUTTON_H

//  INCLUDES
#include <coecntrl.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolscontroller.h"

// FORWARD DECLARATIONS

#ifdef RD_TACTILE_FEEDBACK
class MTouchFeedback;
#endif //RD_TACTILE_FEEDBACK

// DATA TYPES
enum TMPXButtonState
{
   EMPXButtonNormal,
   EMPXButtonDimmed,
   EMPXButtonPressed
};

class CMPXVideoPlaybackButton : public CCoeControl
{
    public:
        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackButton* NewL( CMPXVideoPlaybackControlsController* aController,
                                              TRect aRect, const TDesC &aIconPath );

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackButton();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackButton();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( CMPXVideoPlaybackControlsController* aController,
                         TRect aRect, const TDesC &aIconPath );

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

    private:

        /**
        * Creates button per a state
        */
        CGulIcon* CreateButtonL( const TAknsItemID &aID,
                                 const TInt aFileIndex,
                                 const TInt aFileMaskIndex );

    public:
        /**
        * Creates button for defautl cases
        * @return void
        */
        void CreateDefaultButtonL( const TAknsItemID &aID,
                                   const TInt aFileIndex,
                                   const TInt aFileMaskIndex );

        /**
        * Creates button for dimmed state
        * @return void
        */
        void CreateDimmedButtonL( const TAknsItemID &aID,
                                  const TInt aFileIndex,
                                  const TInt aFileMaskIndex );

        /**
        * Creates button for pressed button
        * @return void
        */
        void CreatePressedButtonL( const TAknsItemID &aID,
                                   const TInt aFileIndex,
                                   const TInt aFileMaskIndex );

        /**
        * Set Dimmed
        * @return void
        */
        void SetDimmed( TBool aDimmed );

        /**
        * Set pressed
        * @return void
        */
        void SetPressed( TBool aPressed );

        /**
        * Check whether this button is dimmed or not
        * @return void
        */
        inline TBool IsDimmed();

    private:    // Data
        CGulIcon*               iDefaultButton;
        CGulIcon*               iDimmedButton;
        CGulIcon*               iPressedButton;

        TMPXButtonState         iState;
        HBufC*                  iIConPath;

        CMPXVideoPlaybackControlsController* iController;
};

// INLINE METHODS

inline
TBool CMPXVideoPlaybackButton::IsDimmed()
{
    TBool isDimmed = ( iState == EMPXButtonDimmed )? ETrue : EFalse;

    MPX_DEBUG(_L("CMPXVideoPlaybackButton::IsDimmed() (%d)"), isDimmed);

    return isDimmed;
}

#endif //MPXVIDEOPLAYBACKBUTTON_H

// End of File
