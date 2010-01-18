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
* Description:  MPX Video playback view's container implementation.
*
*/

// Version : %version: 6 %




#ifndef __CMPXVIDEOPLAYBACKCONTAINER_H__
#define __CMPXVIDEOPLAYBACKCONTAINER_H__


// INCLUDES
#include <coecntrl.h>
#include <eikimage.h> // CEikImage
#include <remconcoreapitargetobserver.h> // Side volume key

#include <mpxplaybackframeworkdefs.h>
#include "mpxvideoplaybackcontrol.hrh"

// FORWARD DECLARATIONS
class CMPXVideoBasePlaybackView;
class CMPXVideoPlaybackControlsController;
class CMPXVideoPlaybackViewFileDetails;
class CMPXVideoPlaybackUserInputHandler;

// CLASS DECLARATION

/**
 *  Container class for video view.
 *
 */
class CMPXVideoPlaybackContainer : public CCoeControl
{
    public:

        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackContainer* NewL( CMPXVideoBasePlaybackView* aView );

        /**
        * Destructor.
        */
        ~CMPXVideoPlaybackContainer();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackContainer( CMPXVideoBasePlaybackView* aView );

        /**
        * Symbian 2nd phase constructor.
        *
        * @param aRect Frame rectangle for container.
        */
        void ConstructL();

    protected:
        // from base class CoeControl
        /**
        * From CoeControl.
        * Handles key events.
        *
        * @param aKeyEvent The key event.
        * @param aType The type of key event.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CoeControl.
        * Handles a change to the control's resources.
        *
        * @param aType A message UID value.
        */
        void HandleResourceChange( TInt aType );

        /**
        * From CoeControl.
        * Gets the control's help context.
        *
        * @param aContext The control's help context.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

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
        * From CCoeControl.
        * Draws the control.
        *
        * @param aRect The rectangular region of the control to be drawn.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl.
        * Handle events from the touch screen
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent);

    public:
        /**
        * Return a handle to the window
        */
        RWindow& GetWindow() const;

        /**
        * Handle commands from control controller
        * @return void
        */
        IMPORT_C void HandleCommandL( TInt aCommand, TInt aValue = 0 );

        IMPORT_C void DoHandlePointerEventL( const TPointerEvent& aPointerEvent );
        IMPORT_C void DoHandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Getter method for User INput Handler
        *
        */
        IMPORT_C CMPXVideoPlaybackUserInputHandler* UserInputHandler();

        /**
        * Handle event from container
        */
        void HandleEventL( TMPXVideoPlaybackControlCommandIds aEvent, TInt aValue = 0 );

        void AddFileDetailsL( CMPXVideoPlaybackViewFileDetails* aDetails );

    private:

        /**
        * Creates new skins for the view.
        * @return void
        */
        void SkinChangeL();

        /**
        * Create CEikImage
        * @param aIconsPath icon path
        * @param aBitmapIndex bitmap index
        * @param aBitmapMaskIndex bitmap index
        * @return CEikImage
        */
        CEikImage* CreateImageL( TFileName& aIconsPath,
                                 TInt aBitmapIndex,
                                 TInt aBitmapMaskIndex );

        /**
        * Handles volume repeat timer timout
        * @return TInt
        */
        static TInt HandleVolumeRepeatTimeoutL( TAny* aPtr );

        /**
        * Adjust volume(+1/-1) for media key
        * @return void
        */
        void HandleVolumeRepeatL();

        void CreateControlsL();

        /**
        * Handles rocker's middle key
        *
        * @param aKeyEvent The key event.
        * @param aType The type of key event.
        */
        void HandleRockerMiddleKeyL(const TKeyEvent& aKeyEvent,
                                   TEventCode aType);

        /**
        * Handles rocker's up key
        *
        * @param aType The type of key event.
        */
        void HandleSeekFwdL(TEventCode aType);

        /**
        * Handles rocker's down key
        *
        * @param aType The type of key event.
        */
        void HandleSeekBackL(TEventCode aType);


    protected:   // data
        CMPXVideoBasePlaybackView*              iView;
        CMPXVideoPlaybackControlsController*    iControlsController;

        //
        //  Bitmaps are owned by the Controls Controller
        //
        CEikImage*                              iRealOneBitmap;

        TBool                                   iVolumeRepeatUp;
        TBool                                   iSurfaceCreated;
        TBool                                   iTvOutConnected;

        CMPXVideoPlaybackViewFileDetails*       iFileDetails;
        TKeyResponse                            iKeyResponse;

        CMPXVideoPlaybackUserInputHandler*      iUserInputHandler;  // owned
};

#endif  // __CMPXVIDEOPLAYBACKCONTAINER_H__

// End of File
