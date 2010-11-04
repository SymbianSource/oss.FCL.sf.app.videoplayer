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
* Description:  Branding Animation control for full screen container
*
*/

// Version : %version: 3 %



#ifndef MPXVIDEOPLAYBACKBRANDINGANIMATION_H
#define MPXVIDEOPLAYBACKBRANDINGANIMATION_H

//  INCLUDES
#include <coecntrl.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CMPXVideoPlaybackControlsController;


class CMPXVideoPlaybackBrandingAnimation : public CCoeControl
{
    public:
        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackBrandingAnimation* NewL( 
                CMPXVideoPlaybackControlsController* aController,
                                                     TRect aRect, 
                                                     TBool aRealFormat );

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackBrandingAnimation();
               
        /*
         *  Start the Branding Animation timer
         *  @since 9.2
         */
        void StartBrandingTimer();
        
        /*
         *  Cancel the Branding Animation timer
         *  @since 9.2
         */
        void CancelBrandingTimer();


    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackBrandingAnimation( CMPXVideoPlaybackControlsController* aController,
                                            TBool aRealFormat );

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


    private:
        
        /**
        * Creates new bitmaps for the control
        * @since 9.2 
        * @return void
        */
        void CreateBitmapsL();
        
        /** 
        * Timer callback for Branding Animation. 
        * @since 9.2 
        * @param aPtr Pointer to timers callback 
        * @return KErrNone 
        */ 
        static TInt BrandingTimer( TAny* aPtr );

        /** 
        * Handle Branding Animation Timer
        * @since 9.2 
        * @return void 
        */ 
        void HandleBrandingTimer();

    private:    // Data        
        CArrayPtrFlat< CGulIcon >   iIconArray;

        TRect                       iBrandingRect;
        TInt                        iCount;
        TInt                        iCurrentIndex;
        CPeriodic*                  iBrandingTimer;  
        TBool                       iRealFormat;
        
        CMPXVideoPlaybackControlsController* iController;
};

#endif //MPXVIDEOPLAYBACKBRANDINGANIMATION_H

// End of File
