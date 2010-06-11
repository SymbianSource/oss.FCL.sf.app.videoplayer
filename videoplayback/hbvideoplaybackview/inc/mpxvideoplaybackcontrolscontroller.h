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

// Version : %version: da1mmcf#19 %



#ifndef MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_
#define MPXVIDEOPLAYBACKCONTROLSCONTROLLER_H_

// INCLUDES
#include <QObject>
#include <QPixmap>

#include <mpxplaybackframeworkdefs.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrol.hrh"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackviewfiledetails.h"

// FORWARD DECLARATIONS
class QTimer;
class QString;
class HbAction;
class ThumbnailManager;
class HbVolumeSliderPopup ;
class CMPXVideoViewWrapper;
class HbVideoBasePlaybackView;
class QMPXVideoPlaybackControlPolicy;
class QMPXVideoPlaybackDocumentLoader;
class QMPXVideoPlaybackFullScreenControl;
class QMPXVideoPlaybackControlsController;
class QMPXVideoPlaybackControlConfiguration;
class VideoServices;


// DATA TYPES

enum TMPXTimerAction
{
    EMPXTimerCancel,
    EMPXTimerReset
};

enum TThumbNailState
{
    EThumbNailEmpty,
    EThumbNailNotAvailable,
    EThumbNailRequsted,
    EThumbNailSet
};

enum TPlaybackViewMode
{
    EFullScreenView,
    EDetailsView,
    EAudioOnlyView
};

const int KMPXControlsTimeOut = 4000;
const int KMPXRNLogoTimeOut = 600;

const QString KMPXPLAYBACKVIEW_XML = ":/hbvideoplaybackview/hbvideoplaybackview.docml";


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

        inline QMPXVideoPlaybackDocumentLoader* layoutLoader();

        inline HbVideoBasePlaybackView* view();

        void updateVideoRectDone();

        TPlaybackViewMode viewMode();

        void changeViewMode( TPlaybackViewMode viewMode, bool transitionEffect = true );

        inline bool isAttachOperation();

        bool isRNLogoBitmapInControlList();

    private:
        /**
        * Initialize controller
        */
        void initializeController();

        /**
        * Update controls
        */
        void handleStateChange( TMPXPlaybackState newState );

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
        * Set changed state on the controls
        */
        void updateState();

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
		* Return true if control is visible
		*/
		bool isSoftKeyVisible();

        /**
        * Handle tvout connected/disconnected event
        */
		void handleTvOutEvent( bool connected, 
		                       TMPXVideoPlaybackControlCommandIds event );

		void updateVideoRect(  bool transitionEffect = true );

		void showVolumeControls();

		void setDefaultBitmap();

		void generateThumbNail();

    signals:

        /**
        * Signals the file path of the current video to VideoServices
        *
        * @param filePath of the video
        */
        void attachVideoPath( const QString& );

    private slots:
        void hideAllControls();
        void skipToNextVideoItem();
        void skipToPreviousVideoItem();
        void handleTappedOnScreen();
        void handleThumbnailReady( QPixmap tnData, void *internal , int id, int error );
        void controlsListUpdated();
        void attachVideo();
        void sendVideo();
        void handleRNLogoVisibleChanged();
        void handleRNLogoTimeout();
        void handleOrientationChanged( Qt::Orientation orientation );

    private:
        HbVideoBasePlaybackView                   *mView;
        CMPXVideoViewWrapper                      *mViewWrapper;
        QMPXVideoPlaybackViewFileDetails          *mFileDetails;

        QList<QMPXVideoPlaybackFullScreenControl*> mControls;

        QMPXVideoPlaybackControlPolicy            *mControlsPolicy;
        QMPXVideoPlaybackControlConfiguration     *mControlsConfig;

        QTimer                                    *mControlsTimer;
        QTimer                                    *mRNLogoTimer;

        QMPXVideoPlaybackDocumentLoader           *mLoader;
        HbVolumeSliderPopup                       *mVolumeControl;

        ThumbnailManager                          *mThumbnailManager;
        VideoServices                             *mVideoServices;

        bool                                       mViewTransitionIsGoingOn;
        bool                                       mIsAttachOperation;
        
        TThumbNailState                            mThumbNailState;

        TMPXPlaybackState                          mState;
        TPlaybackViewMode                          mViewMode;
        Qt::Orientation                            mOrientation;
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
//   QMPXVideoPlaybackControlsController::layoutLoader
// -------------------------------------------------------------------------------------------------
//
inline
QMPXVideoPlaybackDocumentLoader* QMPXVideoPlaybackControlsController::layoutLoader()
{
    return mLoader;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::view
// -------------------------------------------------------------------------------------------------
//
inline
HbVideoBasePlaybackView* QMPXVideoPlaybackControlsController::view()
{
    return mView;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
inline
TPlaybackViewMode QMPXVideoPlaybackControlsController::viewMode()
{
    return mViewMode;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::isAttachOperation
// -------------------------------------------------------------------------------------------------
//
inline
bool QMPXVideoPlaybackControlsController::isAttachOperation()
{        
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::isAttachOperation() ret %d"), 
        mIsAttachOperation );
    
    return mIsAttachOperation;
}

#endif /*MPXVIDEOPLAYBACKCONTROLSCONTROLLER_P_H_*/

// End of File
