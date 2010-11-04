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
#include <RemConCoreApiTargetObserver.h> // Side volume key

#include "mpxvideoplaybackcontrol.hrh"
#include "mpxvideoplaybackcontrol.h"
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
        static CMPXVideoPlaybackUserInputHandler* NewL(CMPXVideoPlaybackContainer* iContainer);

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackUserInputHandler();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackUserInputHandler( CMPXVideoPlaybackContainer* iContainer );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    public:

        void ProcessPointerEventL( CCoeControl* aControl,
                                   const TPointerEvent& aPointerEvent,
                                   TMPXVideoControlType aMPXControl );

        void ProcessKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    private:

        TMPXVideoUserInputType                  iProcessingInputType;     // the type of input being processed
        TInt                                    iLastPressedKeyScanCode; // scancode of last key that sent EEventKeyDown

        CMPXVideoPlaybackContainer*             iContainer;      // not owned
};


#endif /*MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_*/

// End of File
