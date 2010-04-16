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
* Description:  Implementation of QMPXVideoPlaybackFileDetailsWidget
*
*/

// Version : %version: 8 %



#ifndef MPXVIDEOPLAYBACKFILEDETAILSWIDGET_H_
#define MPXVIDEOPLAYBACKFILEDETAILSWIDGET_H_

#include <QObject>

#include <hbwidget.h>

class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;
class HbListWidget;

class QMPXVideoPlaybackFileDetailsWidget : public HbWidget
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackFileDetailsWidget( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackFileDetailsWidget();
        void initialize();

    public:
        void updateWithFileDetails( QMPXVideoPlaybackViewFileDetails* details );

    private:
        QString valueToReadableFormat( int value );
        
        void makeTitleItem( QMPXVideoPlaybackViewFileDetails* details );
        void makeSizeItem( QMPXVideoPlaybackViewFileDetails* details );
        void makeBitRateItem( QMPXVideoPlaybackViewFileDetails* details );
        void addItemToListWidget( QString item, QString text );
        
    private:
        QMPXVideoPlaybackControlsController *mController;
        HbListWidget                        *mListWidget;
        bool                                 mFileDetailsUpdated;
};

#endif /*MPXVIDEOPLAYBACKFILEDETAILSWIDGET_H_*/

