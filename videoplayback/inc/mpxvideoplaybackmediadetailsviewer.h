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


// Version : %version:  7 %


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
        * Timer callback for scroll timer
        * @since 9.2
        * @param aPtr Pointer to timers callback
        * @return KErrNone
        */
        static TInt ScrollTimer( TAny* aPtr );

        /**
        * Handle Scroll Timer
        * @since 9.2
        * @return void
        */
        void HandleScrollTimerL();

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

        /**
         * Update the text of filename label
         */
        void UpdateFilenameL();

        /**
         * Update the text of title label
         */
        void UpdateTitleL();

    private:

        /**
         * Scroll the too long text for some label
         */
        class TTextScroller
        {
            public:

                /**
                 * Constructor
                 */
                TTextScroller();

                /**
                 * Check if the source text needs scrolling.
                 */
                TBool IsScrollNeeded();

                /**
                 * Check if the text of a label needs to be updated
                 */
                TBool IsUpdateNeeded();

                /**
                 * Scroll the source text, and append it to the destination text
                 *
                 * @param aSrcText the source text to be scrolled.
                 * @param aDesText to which the scrolled text to be appended
                 */
                void ScrollText( const TDesC& aSrcText, TDes& aDesText );

            private:

                TUint32        iDelayBeginningTick;
                TInt           iTextScrollPos;
                TBool          iDelay;
                TBool          iScroll;
                TInt           iSrcTextLen;
        };

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
        TRect                                iViewerRect;
        HBufC*                               iAdditionalString;
        TTextScroller                        iFilenameScroller;
        TTextScroller                        iTitleScroller;
        // after every scrolling label has updated, draw them
        TBool                                iScrolledTextUpdated;
        TBool                                iDrmDetailsLaunched;
};



#endif /* MPXVIDEOPLAYBACKMEDIADETAILSVIEWER_H_ */
