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


// Version : %version:  8 %


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
        IMPORT_C static CMPXVideoPlaybackUserInputHandler* NewL(CMPXVideoPlaybackContainer* iContainer );

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
        void ConstructL( );

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

        CMPXVideoPlaybackContainer*             iContainer;	        // not owned
        CCoeControl*                            iActiveControlPtr;  // not owned
        TMPXVideoControlType                    iActiveControlType;
};


#endif /*MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_*/

// End of File
