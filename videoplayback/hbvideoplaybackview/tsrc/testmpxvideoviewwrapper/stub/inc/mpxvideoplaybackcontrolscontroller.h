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
* Description:  Implementation of MPXVideoPlaybackControlsController
*
*/

// Version : %version: 6 %



#ifndef MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <qobject>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrol.hrh"

// FORWARD DECLARATIONS
class CMPXVideoViewWrapper;
class HbVideoBasePlaybackView;
class QMPXVideoPlaybackControlsController;
class QMPXVideoPlaybackViewFileDetails;

// CLASS DECLARATION

class QMPXVideoPlaybackControlsController : public QObject
{
    Q_OBJECT

    public:

        /**
        * constructor.
        */
        QMPXVideoPlaybackControlsController(
                HbVideoBasePlaybackView *view,
                CMPXVideoViewWrapper *viewWrapper,
                QMPXVideoPlaybackViewFileDetails *details );

        /**
        * Destructor.
        */
        virtual ~QMPXVideoPlaybackControlsController();

    public:

        //
        //  Handle event from container
        //
        void handleEvent( TMPXVideoPlaybackControlCommandIds event, int value = 0 );

        //
        //  Add the file details to the controls controller when available
        //
        void addFileDetails( QMPXVideoPlaybackViewFileDetails* details );

        //
        //  updateVideoRectDone
        //
        void updateVideoRectDone();

    public:
    	
        /**
        * Initialize controller
        */
        void initializeController();

        /**
        * Handle tvout connected/disconnected event
        */
        void handleTvOutEvent( bool connected, 
		                           TMPXVideoPlaybackControlCommandIds event,
		                           int value );

    private slots:

        void handleTappedOnScreen();

    public:
        HbVideoBasePlaybackView                   *mView;
        CMPXVideoViewWrapper                      *mViewWrapper;
        QMPXVideoPlaybackViewFileDetails          *mFileDetails;

        bool                                       mViewTransitionIsGoingOn;
};

#endif /*MPXVIDEOPLAYBACKCONTROLSCONTROLLER_P_H_*/

// End of File
