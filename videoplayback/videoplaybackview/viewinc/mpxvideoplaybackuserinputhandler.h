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

// Version : %version:  6 %



#ifndef MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_
#define MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_

// INCLUDES
#include <remconcoreapitargetobserver.h> // Side volume key


// FORWARD DECLARATIONS
class CRemConInterfaceSelector; // Side volume key
class CRemConCoreApiTarget;
class CMPXVideoViewWrapper;

// CLASS DECLARATION

class CMPXVideoPlaybackUserInputHandler : public CBase,
                                          public MRemConCoreApiTargetObserver
{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackUserInputHandler* NewL(
                CMPXVideoViewWrapper* aWrapper, TBool aTvOutConnected );

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackUserInputHandler();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackUserInputHandler( CMPXVideoViewWrapper* aWrapper );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( TBool aTvOutConnected );

    public:
        void ProcessKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

	    void ProcessMediaKey( TRemConCoreApiOperationId aOperationId,
                              TRemConCoreApiButtonAction aButtonAct );

	    void DoHandleMediaKey( TRemConCoreApiOperationId aOperationId,
                               TRemConCoreApiButtonAction aButtonAct );

        // From MRemConCoreApiTargetObserver
        /**
        * Side volume key API from MRemConCoreApiTargetObserver
        * @since 3.2
        * @see MRemConCoreApiTargetObserver
        */
        virtual void MrccatoPlay(
                TRemConCoreApiPlaybackSpeed aSpeed,
                TRemConCoreApiButtonAction aButtonAct );

        virtual void MrccatoCommand(
                TRemConCoreApiOperationId aOperationId,
                TRemConCoreApiButtonAction aButtonAct );

        /**
        * Setter method for iForeground
        * @param aForeground - the value to be set
        * @return void
        */
        void SetForeground( TBool aForeground );
 
        void HandleTVOutEventL( TBool aTVOutConnected );
                
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

        void HandleFastForward( TRemConCoreApiButtonAction aButtonAct );

        void HandleRewind( TRemConCoreApiButtonAction aButtonAct );

        void HandleVolumeUp( TRemConCoreApiButtonAction aButtonAct );

        void HandleVolumeDown( TRemConCoreApiButtonAction aButtonAct );

        // Handles the Display light timer timeout
        static TInt  HandleDisplayTimeout( TAny* aPtr );
        
        // Disable the display backlight
        void DisableBacklight();
        
        // Enable the display backlight
        void EnableBacklight();
        
        // Restarts the timer for display light time-out
        void RestartDisplayTimer();

    private:

        TVideoUserInputType                     iProcessingInputType; 	// the type of input being processed
        TUint                                   iLastPressedKeyCode;     // keycode of last key that sent EEventKeyDown
        TInt                                    iLastPressedKeyScanCode; // scancode of last key that sent EEventKeyDown
        TRemConCoreApiOperationId               iLastMediaKeyPressed;    // Id of last mediakey thats sent a button-press

        CRemConInterfaceSelector*               iInterfaceSelector; 		// Side volume key, owned
        CRemConCoreApiTarget*                   iCoreTarget;            // Owned by CRemConInterfaceSelector

        CPeriodic*                              iVolumeRepeatTimer;     // owned
        TBool                                   iVolumeRepeatUp;
        TBool                                   iTVOutConnected;        // Flag to indicate if TV is connected
        CPeriodic*                              iDisplayTimer;          // Timer to timeout the lights time-out
        TInt                                    iDisplayTimeOut;        // Value of the lights time-out

        TBool                                   iForeground;
        CMPXVideoViewWrapper*                   iViewWrapper;
};


#endif /*MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_*/

// End of File
