/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

// Version : %version: da1mmcf#5 %



#ifndef MPXVIDEOPLAYBACKCONTROLPOLICY_H_
#define MPXVIDEOPLAYBACKCONTROLPOLICY_H_

// INCLUDES
#include <QObject>

#include "mpxvideoplaybackcontrol.hrh"
#include "mpxvideoplaybackcontrolscontroller.h"

// FORWARD DECLARATIONS
class QMPXVideoPlaybackViewFileDetails;

// DATA TYPES

// CLASS DECLARATION
class QMPXVideoPlaybackControlPolicy : public QObject
{
    Q_OBJECT

    public:

        /**
        * Two-phased constructor.
        */
        QMPXVideoPlaybackControlPolicy();

        /**
        * Destructor.
        */
        virtual ~QMPXVideoPlaybackControlPolicy();

    public:
        /**
        * Sets the control properties per policy
        */
        void setControlProperties( TMPXVideoPlaybackControls controlIndex,
                                   TUint& properties,
                                   QMPXVideoPlaybackViewFileDetails *details,
                                   TPlaybackViewMode viewMode );
};

#endif /*MPXVIDEOPLAYBACKCONTROLPOLICY_H_*/

// End of File
