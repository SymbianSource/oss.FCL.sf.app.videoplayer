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

// Version : %version:  1 %



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
class QTimer;
class QString;
class HbAction;
class QActionGroup;
class QMPXVideoPlaybackControlPolicy;
class QMPXVideoPlaybackControlsLayout;
class QMPXVideoPlaybackDocumentLoader;
class QMPXVideoPlaybackFullScreenControl;
class QMPXVideoPlaybackNonTouchVolumeBar;
class QMPXVideoPlaybackControlsController;
class QMPXVideoPlaybackControlConfiguration;


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

const int KMPXControlsTimeOut = 4000;


// CLASS DECLARATION

class QMPXVideoPlaybackControlsController : public QObject
{
    Q_OBJECT

    public:

        /**
        * constructor.
        */
        QMPXVideoPlaybackControlsController( QMPXVideoPlaybackViewFileDetails *details );

        /**
        * Destructor.
        */
        virtual ~QMPXVideoPlaybackControlsController();

    public:

        /**
        * Handle event from container
        */
        void handleEvent( TMPXVideoPlaybackControlCommandIds event, int value = 0 );

        /**
        * Command handling function.
        * Call HandleCommandL() of container
        */
        void handleCommand( TMPXVideoPlaybackViewCommandIds command, int value = 0 );

        /*
        * Return ETrue if TV-out cable gets connected
        */
        inline bool isTvOutConnected();

        /*
         * Return ETrue if TV-out cable is connected and content can be played
         */
        inline bool isTvOutPlaybackAllowed();

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

        //
        //  Add the file details to the controls controller when available
        //
        void addFileDetails( QMPXVideoPlaybackViewFileDetails* details );

        QMPXVideoPlaybackDocumentLoader* layoutLoader();

        inline bool isFlipView();

        void updateVideoRectDone();
        
        void changeViewMode( TPlaybackViewMode viewMode, bool transitionEffect = true );
        
        TPlaybackViewMode viewMode();

    public:
        /**
        * Initialize controller
        */
        void initializeController();

        /**
        * Create controls
        */
        void createControls();

        /**
        * Update controls
        */
        void handleStateChange( TMPXPlaybackState newState );

        /**
        * Update Control's visibility
        */
        void updateControlsVisibility();

        /**
        * Toggle visibility
        */
        void toggleVisibility();

        /**
        * Create/delete controls based on updated control list
        */
        void controlsListUpdated();

        /**
        * Show Controls and reset the timers
        */
        void showControls();

        /**
        * Return ETrue if any control is visible
        */
        bool isVisible();

        /**
        * Append a control based on control index
        */
        void appendControl( TMPXVideoPlaybackControls controlIndex );

        /**
        * Set changed volume
        */
        void volumeChanged( int volume );

        /**
        * Set changed duration
        */
        void durationChanged( int duration);

        /**
        * Set changed position
        */
        void positionChanged( int position );

        /**
        * Set changed position
        */
        void aspectRatioChanged( int aspectRatio );

        /*
         *  Sets the download size on the progress bar
         */
        void setDownloadSize( int size );

        /*
         *  Updates the download ratio on the progress bar
         */
        void updateDownloadPosition( int newSize );

        /**
        * Set changed state on button bar
        */
        void updateStateOnButtonBar();

        /**
        * Check whether this clip is real format or not
        */
        bool realFormat( QString filename );

        /**
        * Check whether this clip is real format or not for streaming/live streaming
        */
        bool realFormatForStreaming( const TDesC& des );

        /**
        * Check whether this clip is real format or not for local/progressive donwload
        */
        bool realFormatForLocal();
        
        /**
        * Handle errors
        */
        void handleErrors(); 
        
        /**
		* Return ETrue if control is visible
		*/
		bool isSoftKeyVisible( int value );

        /**
        * Handle tvout connected/disconnected event
        */
    void handleTvOutEvent( bool connected, 
		                        TMPXVideoPlaybackControlCommandIds event,
		                        int value );

		HbAction* createAction( QActionGroup *actionsGroup,
		                        int index,
		                        const char *slot,
		                        const QString& toolTip );

		void updateVideoRect(  bool transitionEffect = true ); 

		void showVolumeControls();

    private slots:
        void hideAllControls();
        void skipToNextVideoItem();
        void skipToPreviousVideoItem();
        void handleTappedOnScreen();

    public:
        QMPXVideoPlaybackViewFileDetails          *mFileDetails;

        QList<QMPXVideoPlaybackFullScreenControl*> mControls;

        QMPXVideoPlaybackControlPolicy            *mControlsPolicy;
        QMPXVideoPlaybackControlConfiguration     *mControlsConfig;

        QTimer                                    *mControlsTimer;

        QMPXVideoPlaybackDocumentLoader           *mLoader;
        QMPXVideoPlaybackNonTouchVolumeBar        *mVolumeControl;

        bool                                       mFlipViewOpened;
        bool                                       mViewTransitionIsGoingOn;
        TMPXPlaybackState                          mState;
        TPlaybackViewMode                          mViewMode;
        
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

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::isTvOutConnected
// -------------------------------------------------------------------------------------------------
//
inline
bool QMPXVideoPlaybackControlsController::isTvOutConnected()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::isTvOutConnected(%d)"),
            mFileDetails->mTvOutConnected);

    return mFileDetails->mTvOutConnected;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::isTvOutPlaybackAllowed
// -------------------------------------------------------------------------------------------------
//
inline
bool QMPXVideoPlaybackControlsController::isTvOutPlaybackAllowed()
{
    bool playable = ( ! mFileDetails->mTvOutConnected || mFileDetails->mTvOutPlayAllowed );

    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::isTvOutPlaybackAllowed() [%d]"),
            playable);

    return playable;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::isFlipView
// -------------------------------------------------------------------------------------------------
//
inline
bool QMPXVideoPlaybackControlsController::isFlipView()
{
    return mFlipViewOpened;
}


#endif /*MPXVIDEOPLAYBACKCONTROLSCONTROLLER_P_H_*/

// End of File
