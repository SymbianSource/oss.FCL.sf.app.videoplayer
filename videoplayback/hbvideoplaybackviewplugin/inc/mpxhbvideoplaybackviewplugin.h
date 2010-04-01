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
* Description:  QMpxHbVideoPlaybackViewPlugin
*
*/

// Version : %version:  6 %


#ifndef QMPXHBVIDEOPLAYBACKVIEWPLUGIN_H
#define QMPXHBVIDEOPLAYBACKVIEWPLUGIN_H

#include <QObject>
#include <mpxviewpluginqt.h>


// FORWARD DECLARATION
class HbVideoPlaybackView;

/**
 *  MPX playback view plugin definition.
 *
 */
class QMpxHbVideoPlaybackViewPlugin : public MpxViewPlugin
{
    Q_OBJECT

public:

    /**
     * C++ default constructor.
     */
    QMpxHbVideoPlaybackViewPlugin();

    /**
     * Destructor.
     *
     */
    virtual ~QMpxHbVideoPlaybackViewPlugin();

    // from QViewPlugin

    void createView();

    void destroyView();

    void activateView();

    void deactivateView();

    QGraphicsWidget* getView();
	
signals:
    void command( int aCommand );    

public slots:
    void orientationChange(Qt::Orientation orientation);
    
    void back();

public:
	HbVideoPlaybackView* mView;
	bool                 mViewActivated;
	
};


#endif //QMPXHBVIDEOPLAYBACKVIEWPLUGIN_H
