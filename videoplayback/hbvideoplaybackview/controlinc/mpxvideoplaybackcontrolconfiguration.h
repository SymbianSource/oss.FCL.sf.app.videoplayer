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
* Description:  Implementation of CMPXVideoPlaybackControlConfiguration
*
*/

// Version : %version: da1mmcf#6 %



#ifndef MPXVIDEOPLAYBACKCONTROLCONFIGURATION_H_
#define MPXVIDEOPLAYBACKCONTROLCONFIGURATION_H_

// INCLUDES
#include <QObject>

#include "mpxvideoplaybackcontrol.hrh"
#include "mpxhelixplaybackplugindefs.h"

// FORWARD DECLARATIONS
class QMPXVideoPlaybackControlsController;

// DATA TYPES

// CLASS DECLARATION
class QMPXVideoPlaybackControlConfiguration : public QObject
{
    Q_OBJECT

    public:

        /**
        * constructor.
        */
        QMPXVideoPlaybackControlConfiguration( QMPXVideoPlaybackControlsController* controller );

        /**
        * Destructor.
        */
        virtual ~QMPXVideoPlaybackControlConfiguration();

    public:
        /**
        * Update control list
        */
        void updateControlList( TMPXVideoPlaybackControlCommandIds event );

        /**
        * Return control list
        */
        QList<TMPXVideoPlaybackControls>& controlList();

        void updateControlsWithFileDetails();

    private:
        /**
        * Create control list
        */
        void createControlList();

        /**
        * Delete controls from list
        */
        void deleteControlFromList( TMPXVideoPlaybackControls control );

        /**
        * Add controls to list
        */
        void addControlToList( TMPXVideoPlaybackControls control );

    signals:
        void controlListUpdated();

    private:
        QMPXVideoPlaybackControlsController *mControlsController;
        QList<TMPXVideoPlaybackControls>     mControlsList;
};

#endif /*MPXVIDEOPLAYBACKCONTROLCONFIGURATION_H_*/

// End of File
