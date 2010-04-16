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
* Description:  Implementation of QMPXVideoPlaybackFullScreenControl
*
*/

// Version : %version:  1 %



#ifndef MPXVIDEOPLAYBACKFULLSCREENCONTROL_H_
#define MPXVIDEOPLAYBACKFULLSCREENCONTROL_H_

#include <qobject>

#include <mpxvideoplaybackcontrol.hrh>
#include <mpxplaybackframeworkdefs.h>

class HbWidget;
class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackFullScreenControl : public QObject
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackFullScreenControl( QMPXVideoPlaybackControlsController* controller, 
                                            TMPXVideoPlaybackControls index,
                                            HbWidget* widget, 
                                            TUint controlproperties );

        virtual ~QMPXVideoPlaybackFullScreenControl();

    public:

        /**
        * Set visibility of each control
        */
        virtual void setVisibility( TMPXPlaybackState aState );
        
        /**
        * return control index
        */
        TMPXVideoPlaybackControls controlIndex();

        /**
        * set changed volume
        */
        bool volumeChanged( int aVolume );

        /**
        * set changed duration
        */
        bool durationChanged( int duration );

        /**
        * set changed volume
        */
        bool positionChanged( int position );

        /**
        * Set changed position
        */
        bool aspectRatioChanged( int aspectRatio );

        /**
        * set changed volume
        */
        bool setDownloadSize( int size );

        /**
        * set changed state
        */
        void updateState( TMPXPlaybackState state );
        
        /*
         *  UpdateDownloadPosition
         *  updates the download ratio on the progress bar
         */
        bool updateDownloadPosition( int size );

        /*
         *  Update the controls with the file details
         */
        virtual void updateControlsWithFileDetails( QMPXVideoPlaybackViewFileDetails *details );

        virtual void setVisible( bool visible );

        virtual bool isVisible();

        virtual void updateControlProperties( TUint properties );

    public:
        int                       mPosition;
        int                       mDuration;
        int                       mAspectRatio;
        int                       mDownloadSize;
        int                       mDownloadPosition;
        int                       mVolume;
        
        bool                      mVisible;
        TMPXPlaybackState         mState;
        TMPXPlaybackState         mVisibilityState;
        
    protected:
        QMPXVideoPlaybackControlsController* mController;
        HbWidget                 *mControl;
        TMPXVideoPlaybackControls mControlIndex;
        TUint                     mProperties;
};

#endif /*MPXVIDEOPLAYBACKFULLSCREENCONTROL_H_*/

