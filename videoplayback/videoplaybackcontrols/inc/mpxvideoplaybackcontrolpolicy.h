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
* Description:  Implementation of MPXVideoPlaybackControlPolicy
*
*/

// Version : %version: 4 %



#ifndef MPXVIDEOPLAYBACKCONTROLPOLICY_H_
#define MPXVIDEOPLAYBACKCONTROLPOLICY_H_

// INCLUDES
#include "mpxvideoplaybackcontrol.hrh"

// FORWARD DECLARATIONS
class CMPXVideoPlaybackViewFileDetails;

// DATA TYPES

// CLASS DECLARATION
class CMPXVideoPlaybackControlPolicy : public CBase
{

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackControlPolicy* NewL();

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackControlPolicy();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackControlPolicy();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    public:
        /**
        * Sets the control properties per policy
        */
        void SetControlProperties( TMPXVideoPlaybackControls aControlIndex,
                                   TUint& aProperties,
                                   CMPXVideoPlaybackViewFileDetails* aDetails );
};

#endif /*MPXVIDEOPLAYBACKCONTROLPOLICY_H_*/

// End of File
