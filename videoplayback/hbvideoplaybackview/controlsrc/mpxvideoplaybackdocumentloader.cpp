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
* Description:  Implementation of QMPXVideoPlaybackDocumentLoader
*
*/

// Version : %version: 7 %



#include "mpxvideo_debug.h"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackfiledetailswidget.h"
#include "mpxvideoplaybacknontouchvolumebar.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackcontrolconfiguration.h"
#include "mpxvideoplaybackdetailsplaybackwindow.h"

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::QMPXVideoPlaybackDocumentLoader
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackDocumentLoader::QMPXVideoPlaybackDocumentLoader( 
        QMPXVideoPlaybackControlsController *controller )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackDocumentLoader::QMPXVideoPlaybackDocumentLoader()"));

    mController = controller;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::~QMPXVideoPlaybackDocumentLoader
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackDocumentLoader::~QMPXVideoPlaybackDocumentLoader()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::QMPXVideoPlaybackDocumentLoader") );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::createObject()
// -------------------------------------------------------------------------------------------------
//
QObject *QMPXVideoPlaybackDocumentLoader::createObject( const QString& type, const QString &name )
{
    QObject *object = NULL;

    if ( name == "buttonBarLayout" )
    {
        object = new QMPXVideoPlaybackButtonBar( mController );
        object->setObjectName( name );
    }
    else if ( name == "progressBarLayout" )
    {
        object = new QMPXVideoPlaybackProgressBar( mController );
        object->setObjectName( name );
    }
    else if ( name == "fileDetailsLayout" )
    {
        object = new QMPXVideoPlaybackFileDetailsWidget( mController );
        object->setObjectName( name );
    }
    else if ( name == "volumeSlider" )
    {
        object = new QMPXVideoPlaybackNonTouchVolumeBar();
        object->setObjectName( name );
    }
    else if ( name == "detailsPlaybackWindow" )
    {
        object = new QMPXVideoPlaybackDetailsPlaybackWindow( mController );
        object->setObjectName( name );
    }
    else
    {
        object = HbDocumentLoader::createObject( type, name );
    }

    return object;
}

// End of file
