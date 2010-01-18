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
* Description:  Implementation of CMPXVideoPlaybackControl
*
*/

// Version : %version: 3 %



#ifndef MPXPLAYBACKCONTROLPDL_H_
#define MPXPLAYBACKCONTROLPDL_H_

// INCLUDES

#include "mpxvideoplaybackcontrol.h"

// DATA TYPES

// CLASS DECLARATION

class CMPXVideoPlaybackControlPdl : public CMPXVideoPlaybackControl
{
    public: // Constructors and destructor

        /*
         * Two-phased constructor.
         */
        static CMPXVideoPlaybackControlPdl* NewL( CMPXVideoPlaybackControlsController* aController,
                                                  CCoeControl* aControl,
                                                  TRect aRect,
                                                  TMPXVideoPlaybackControls aControlIndex,
                                                  TUint aProperties );

        /*
         * Destructor.
         */
        virtual ~CMPXVideoPlaybackControlPdl();

    private:

        /*
         * C++ default constructor.
         */
        CMPXVideoPlaybackControlPdl( CMPXVideoPlaybackControlsController* aController,
                                     CCoeControl* aControl,
                                     TMPXVideoPlaybackControls aControlIndex,
                                     TUint aProperties );

    public:

        /*
         *  Sets visibility of each control
         */
        void MakeVisible( TBool aVisible );
};

#endif /*MPXPLAYBACKCONTROLPDL_H_*/

// End of File
