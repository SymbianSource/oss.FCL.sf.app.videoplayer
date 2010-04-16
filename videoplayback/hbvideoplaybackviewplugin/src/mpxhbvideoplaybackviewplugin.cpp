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



#include <QtGui>
#include <hbview.h>
#include <xqplugin.h>
#include <hbinstance.h>

#include "mpxvideo_debug.h"
#include "hbvideoplaybackview.h"
#include "mpxhbvideocommondefs.h"
#include "mpxhbvideoplaybackviewplugin.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
QMpxHbVideoPlaybackViewPlugin::QMpxHbVideoPlaybackViewPlugin()
    : mView( NULL )
	, mViewActivated ( false )
{
    MPX_ENTER_EXIT(_L("QMpxHbVideoPlaybackViewPlugin::QMpxHbVideoPlaybackViewPlugin()"));
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
QMpxHbVideoPlaybackViewPlugin::~QMpxHbVideoPlaybackViewPlugin()
{
    MPX_ENTER_EXIT(_L("QMpxHbVideoPlaybackViewPlugin::~QMpxHbVideoPlaybackViewPlugin()"));
    destroyView();
}

// ---------------------------------------------------------------------------
// Create view
// ---------------------------------------------------------------------------
//
void QMpxHbVideoPlaybackViewPlugin::createView()
{
    MPX_ENTER_EXIT(_L("QMpxHbVideoPlaybackViewPlugin::createView()"));

    mView = new HbVideoPlaybackView();
    connect( mView, SIGNAL( activatePreviousView() ), this, SLOT( back() ) );
}

// ---------------------------------------------------------------------------
// Destroy view
// ---------------------------------------------------------------------------
//
void QMpxHbVideoPlaybackViewPlugin::destroyView()
{
    MPX_ENTER_EXIT(_L("QMpxHbVideoPlaybackViewPlugin::destroyView()"));

    if ( mView )
    {
        delete mView;
        mView = NULL;
    }
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
void QMpxHbVideoPlaybackViewPlugin::activateView()
{
    MPX_ENTER_EXIT(_L("QMpxHbVideoPlaybackViewPlugin::activateView()"));

    if ( mView && !mViewActivated )
    {
        mView->handleActivateView();
        mViewActivated = true;
    }
}

// ---------------------------------------------------------------------------
// Deactivate view
// ---------------------------------------------------------------------------
//
void QMpxHbVideoPlaybackViewPlugin::deactivateView()
{
    MPX_ENTER_EXIT(_L("QMpxHbVideoPlaybackViewPlugin::deactivateView()"));

    mView->handleDeactivateView();
    mViewActivated = false;
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
QGraphicsWidget* QMpxHbVideoPlaybackViewPlugin::getView()
{
    return mView;
}

// ---------------------------------------------------------------------------
// QMpxHbVideoPlaybackViewPlugin::orientationChange
// ---------------------------------------------------------------------------
//
void QMpxHbVideoPlaybackViewPlugin::orientationChange( Qt::Orientation orientation )
{
    Q_UNUSED( orientation );
}

// ---------------------------------------------------------------------------
// QMpxHbVideoPlaybackViewPlugin::back
// ---------------------------------------------------------------------------
//
void QMpxHbVideoPlaybackViewPlugin::back()
{
    MPX_ENTER_EXIT(_L("QMpxHbVideoPlaybackViewPlugin::back()"));

    emit command( MpxHbVideoCommon::CollectionView );
}

XQ_EXPORT_PLUGIN2( hbvideoplaybackviewplugin, QMpxHbVideoPlaybackViewPlugin );

// End of File
