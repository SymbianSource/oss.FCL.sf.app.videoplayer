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
* Description:  Implementation of QMPXVideoPlaybackStatusPaneControl
*
*/

// Version : %version: 10 %



#ifndef MPXVIDEOPLAYBACKSTATUSPANECONTROL_H_
#define MPXVIDEOPLAYBACKSTATUSPANECONTROL_H_


#include "mpxvideoplaybackfullscreencontrol.h"


class HbLabel;
class HbAction;
class HbGroupBox;
class QGraphicsWidget;
class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackStatusPaneControl : public QMPXVideoPlaybackFullScreenControl
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackStatusPaneControl( QMPXVideoPlaybackControlsController* controller, 
                                            TMPXVideoPlaybackControls index,
                                            HbWidget* widget, 
                                            TUint controlproperties );

        virtual ~QMPXVideoPlaybackStatusPaneControl();

    public:

        /**
        * Set visibility of each control
        */
        void setVisibility( TMPXPlaybackState aState );

        /*
         *  Update the controls with the file details
         */
        void updateControlsWithFileDetails( QMPXVideoPlaybackViewFileDetails *details );

        void setVisible( bool visible );

        bool isVisible();

        void setMenu( QMPXVideoPlaybackViewFileDetails* details );

        void controlListUpdated( QMPXVideoPlaybackViewFileDetails* details );

    private slots:
        void handleAboutToShow();
        void handleAboutToHide();
        void openFullScreenView();

    private:
        bool                     mVisible;
        HbAction                *mActionBack;
        HbLabel                 *mTitleLabel;
        HbGroupBox              *mTitleGroupBox;
        QGraphicsWidget         *mTitleLayout;
};

#endif /*MPXVIDEOPLAYBACKSTATUSPANECONTROL_H_*/

