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
* Description:  Implementation of CMPXVideoPlaybackControlConfiguration
*
*/

// Version : %version: 4 %



#ifndef MPXVIDEOPLAYBACKCONTROLCONFIGURATION_H_
#define MPXVIDEOPLAYBACKCONTROLCONFIGURATION_H_

// INCLUDES
#include "mpxvideoplaybackcontrol.hrh"
#include "mpxhelixplaybackplugindefs.h"

// FORWARD DECLARATIONS
class CMPXVideoPlaybackControlsController;
class CMPXVideoPlaybackViewFileDetails;

// DATA TYPES

// CLASS DECLARATION
class CMPXVideoPlaybackControlConfiguration : public CBase
{

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackControlConfiguration* NewL(
                        CMPXVideoPlaybackControlsController* aController );

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackControlConfiguration();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackControlConfiguration( CMPXVideoPlaybackControlsController* aController );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    public:
        /**
        * Update control list
        */
        void UpdateControlListL( TMPXVideoPlaybackControlCommandIds aEvent );

        /**
        * Return control list
        */
        RArray<TMPXVideoPlaybackControls>& ControlList();

        void UpdateControlsWithFileDetailsL();

    private:
        /**
        * Create control list
        */
        void CreateControlListL();

        /**
        * Delete controls from list
        */
        void DeleteControlFromList( TMPXVideoPlaybackControls aControl );

    private:
        CMPXVideoPlaybackControlsController*      iControlsController;
        RArray<TMPXVideoPlaybackControls>         iControlsList;

        TBool      iAspectRatioIconAdded;
        TBool      iAudioOnlyIndicatorsAdded;
        TBool      iTitleArtistIndicatorsAdded;
        TBool      iDownloadPausedAdded;
};

#endif /*MPXVIDEOPLAYBACKCONTROLCONFIGURATION_H_*/

// End of File
