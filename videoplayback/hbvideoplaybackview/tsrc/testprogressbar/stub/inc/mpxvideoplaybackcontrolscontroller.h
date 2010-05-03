/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

// Version : %version: 2 %



#ifndef MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <qobject>

#include <mpxplaybackframeworkdefs.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrol.hrh"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackviewfiledetails.h"

// FORWARD DECLARATIONS
class QMPXVideoPlaybackDocumentLoader;
class QMPXVideoPlaybackControlsController;


// DATA TYPES

enum TMPXTimerAction
{
    EMPXTimerCancel,
    EMPXTimerReset
};

enum TPlaybackViewMode
{
    EFullScreenView,
    EDetailsView,
    EAudioOnlyView
};


// CLASS DECLARATION

class QMPXVideoPlaybackControlsController : public QObject
{
    Q_OBJECT

    public:

        /**
        * constructor.
        */
        QMPXVideoPlaybackControlsController();

        /**
        * Destructor.
        */
        virtual ~QMPXVideoPlaybackControlsController();

    public:
        /**
        * Initialize controller
        */
        void initializeController();

        /**
        * Command handling function.
        * Call HandleCommandL() of container
        */
        void handleCommand( TMPXVideoPlaybackViewCommandIds command, int value = 0 );

        /**
        * Reset or cancel timers for the controls
        */
        void resetDisappearingTimers( TMPXTimerAction timerAction );

        /**
        * Return state
        */
        inline TMPXPlaybackState state();

        /**
        * Return file details
        */
        inline QMPXVideoPlaybackViewFileDetails* fileDetails();

        QMPXVideoPlaybackDocumentLoader* layoutLoader();

        TPlaybackViewMode viewMode();

    public:
        QMPXVideoPlaybackViewFileDetails          *mFileDetails;
        QMPXVideoPlaybackDocumentLoader           *mLoader;
        TMPXPlaybackState                          mState;
        TPlaybackViewMode                          mViewMode;
        TMPXTimerAction                            mTimerAction;
        TMPXVideoPlaybackViewCommandIds            mCommand;
        int                                        mValue;
};

// INLINE METHODS
// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::state
// -------------------------------------------------------------------------------------------------
//
inline
TMPXPlaybackState QMPXVideoPlaybackControlsController::state()
{
    return mState;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::fileDetails
// -------------------------------------------------------------------------------------------------
//
inline
QMPXVideoPlaybackViewFileDetails* QMPXVideoPlaybackControlsController::fileDetails()
{
    return mFileDetails;
}

#endif /*MPXVIDEOPLAYBACKCONTROLSCONTROLLER_P_H_*/

// End of File
