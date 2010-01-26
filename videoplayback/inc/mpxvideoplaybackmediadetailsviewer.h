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
* Description:  Media Details Viewer
*
*/

// Version : %version:  e003sa33#4 %

#ifndef MPXVIDEOPLAYBACKMEDIADETAILSVIEWER_H_
#define MPXVIDEOPLAYBACKMEDIADETAILSVIEWER_H_

//  INCLUDES
#include <coecntrl.h>

#include "mpxvideo_debug.h"

// FORWARD DECLARATIONS
class CEikLabel;
class CMPXVideoPlaybackViewFileDetails;
class CMPXVideoPlaybackControlsController;

#ifdef RD_TACTILE_FEEDBACK
class MTouchFeedback;
#endif //RD_TACTILE_FEEDBACK

class CMPXVideoPlaybackMediaDetailsViewer : public CCoeControl
{
    public:
        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackMediaDetailsViewer* NewL( CMPXVideoPlaybackControlsController* aController);

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackMediaDetailsViewer();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackMediaDetailsViewer( CMPXVideoPlaybackControlsController* aController );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

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
        
        /**
        * Creates new skins for the control
        * @return void
        */
        void SkinChangeL();        


    public: // from CoeControl

        /**
        * From CoeControl,OfferKeyEventL
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    public:

        /*
         *  Bring the button bar to its original state
         *  @since 5.0
         */
        void Reset();
        
        /** 
        * Accessor for the dynamically calculated viewer rect
        * @since 9.2 
        * @return the viewer rect
        */                   
        TRect ViewerRect();         
              

    private:
        
        void CreateLabelsL();
        
        void LaunchDRMDetailsL();
        
        /**
        * Sets label rects and text
        */
        void FormatLabelsL() const;   
        
        void UpdateBackgroundBitmapL() const;
        
        /** 
        * Timer callback for scroll filename timer
        * @since 9.2 
        * @param aPtr Pointer to timers callback 
        * @return KErrNone 
        */ 
        static TInt ScrollFilenameTimer( TAny* aPtr );

        /** 
        * Handle Scroll Filename Timer
        * @since 9.2 
        * @return void 
        */ 
        void HandleScrollFilenameTimerL();   
        
        /** 
        * Determine the number of items to be shown in the viewer
        * @since 9.2 
        * @return the number of rows to be created
        */         
        TInt NumOfItemsShownInViewerL();
        
        /** 
        * Dynamically calculate determine the viewer rectangle dyanmically
        * @since 9.2 
        * @return the viewer rect
        */                   
        TRect CalculateViewerRectL();          
                

    private:    // Data
        CMPXVideoPlaybackControlsController* iController;
        
        CEikLabel*                           iClipnameLabel;
        CEikLabel*                           iTitleLabel;
        CEikLabel*                           iArtistLabel;        
        CEikLabel*                           iFormatLabel;
        CEikLabel*                           iResolutionLabel;
        CEikLabel*                           iDurationLabel;
        CEikLabel*                           iBitrateLabel;
        CEikLabel*                           iLicenseLabel;
        CEikLabel*                           iAdditionalLabel;
        

        CFbsBitmap*                          iBackgroundBitmap;   
        CPeriodic*                           iScrollingTextTimer;
        TInt                                 iScrollPosition;
        TBool                                iShouldPauseScrolling;
        TRect                                iViewerRect;
        HBufC*                               iAdditionalString;
};



#endif /* MPXVIDEOPLAYBACKMEDIADETAILSVIEWER_H_ */
