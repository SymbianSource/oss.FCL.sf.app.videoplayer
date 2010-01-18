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
* Description:  Implementation of MPXVideoPlaybackUserInputHandler
*
*/

// Version : %version:  5 %



#ifndef MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_
#define MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_

// INCLUDES
#include <f32file.h>
#include <coecntrl.h>
#include <remconcoreapitargetobserver.h> // Side volume key

#include "mpxvideoplaybackcontrol.hrh"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideo_debug.h"

// FORWARD DECLARATIONS
class CMPXVideoPlaybackControl;
class CMPXVideoPlaybackControlsController;
class CRemConInterfaceSelector; // Side volume key
class CRemConCoreApiTarget;
class CMPXVideoPlaybackContainer;

// CLASS DECLARATION

class CMPXVideoPlaybackUserInputHandler : public CBase,
                                          public MRemConCoreApiTargetObserver
{

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMPXVideoPlaybackUserInputHandler* NewL(CMPXVideoPlaybackContainer* iContainer, TBool aTvOutConnected);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMPXVideoPlaybackUserInputHandler();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackUserInputHandler(CMPXVideoPlaybackContainer* iContainer);

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( TBool aTvOutConnected );

    public:

        IMPORT_C void ProcessPointerEventL( CCoeControl* aControl,
                                            const TPointerEvent& aPointerEvent,
                                            TMPXVideoControlType aMPXControl );

        void ProcessKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

	    void ProcessMediaKey(TRemConCoreApiOperationId aOperationId,
                             TRemConCoreApiButtonAction aButtonAct);

	    void DoHandleMediaKey(TRemConCoreApiOperationId aOperationId,
                               TRemConCoreApiButtonAction aButtonAct);

        // From MRemConCoreApiTargetObserver
        /**
        * Side volume key API from MRemConCoreApiTargetObserver
        * @since 3.2
        * @see MRemConCoreApiTargetObserver
        */
        virtual void MrccatoPlay(
                TRemConCoreApiPlaybackSpeed aSpeed,
                TRemConCoreApiButtonAction aButtonAct);

        virtual void MrccatoCommand(
                TRemConCoreApiOperationId aOperationId,
                TRemConCoreApiButtonAction aButtonAct);

        /**
        * Setter method for iForeground
        * @param aForeground - the value to be set
        * @return void
        */
        void SetForeground(TBool aForeground);
 
        void HandleTVOutEvent(TBool aTVOutConnected);
                
	private:
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

        void HandleFastForward(TRemConCoreApiButtonAction aButtonAct);

        void HandleRewind(TRemConCoreApiButtonAction aButtonAct);

        void HandleVolumeUp(TRemConCoreApiButtonAction aButtonAct);

        void HandleVolumeDown(TRemConCoreApiButtonAction aButtonAct);

        void ReRoutePointerEventL(CCoeControl* aControl,
                                  const TPointerEvent& aPointerEvent,
                                  TMPXVideoControlType aMPXControl);
        
        // Handles the Display light timer timeout
        static TInt  HandleDisplayTimeout( TAny* aPtr );
        
        // Disable the display backlight
        void DisableBacklight();
        
        // Enable the display backlight
        void EnableBacklight();
        
        // Restarts the timer for display light time-out
        void RestartDisplayTimer();

    private:

        TMPXVideoUserInputType                  iProcessingInputType; 	// the type of input being processed
        TUint                                   iLastPressedKeyCode;     // keycode of last key that sent EEventKeyDown
        TInt                                    iLastPressedKeyScanCode; // scancode of last key that sent EEventKeyDown
        TRemConCoreApiOperationId               iLastMediaKeyPressed;    // Id of last mediakey thats sent a button-press

        CRemConInterfaceSelector*               iInterfaceSelector; 		// Side volume key, owned
        CRemConCoreApiTarget*                   iCoreTarget;            // Owned by CRemConInterfaceSelector

        CPeriodic*                              iVolumeRepeatTimer;     // owned
        TBool                                   iVolumeRepeatUp;
        TBool                                   iForeground;
        TBool                                   iTVOutConnected;        // Flag to indicate if TV is connected
        CPeriodic*                              iDisplayTimer;          // Timer to timeout the lights time-out
        TInt                                    iDisplayTimeOut;        // Value of the lights time-out


        CMPXVideoPlaybackContainer*             iContainer;	  // not owned


};


#endif /*MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_*/

// End of File
