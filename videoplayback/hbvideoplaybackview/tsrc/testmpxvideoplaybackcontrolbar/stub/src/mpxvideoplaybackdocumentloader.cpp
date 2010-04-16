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
* Description:  Implementation of QMPXVideoPlaybackDocumentLoader
*
*/

// Version : %version:   1 %




#include "mpxvideo_debug.h"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackcontrolscontroller.h"

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

    for ( int i = 0 ; i < mWidgets.count() ; i++ )
    {
        mWidgets.removeAt( 0 );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::findWidget()
// -------------------------------------------------------------------------------------------------
//
QGraphicsWidget *QMPXVideoPlaybackDocumentLoader::findWidget( const QString &name )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::findWidget") );

    QGraphicsWidget *object = NULL;

    int index = exist( name );

    if ( index == -1 )
    {
        object = createWidget( name );
    }
    else
    {
        object = mWidgets[ index ];
    }

    return object;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::createWidget()
// -------------------------------------------------------------------------------------------------
//
QGraphicsWidget *QMPXVideoPlaybackDocumentLoader::createWidget( const QString &name )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::createWidget") );

    QGraphicsWidget *object = NULL;
        
    if ( name == "buttonBarLayout" )
    {
        object = new QMPXVideoPlaybackButtonBar( mController );
        object->setObjectName( name );
        mWidgets.append( object );
    }
    else if ( name == "progressBarLayout" )
    {
        object = new QMPXVideoPlaybackProgressBar( mController );
        object->setObjectName( name );
        mWidgets.append( object );
    }

    return object;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::exist()
// -------------------------------------------------------------------------------------------------
//
int QMPXVideoPlaybackDocumentLoader::exist( const QString &name )
{
    int i = 0;

    for ( ; i < mWidgets.count() ; i++ )
    {
        if( mWidgets[i]->objectName() == name )
        {
            break;
        }
    }

    if ( i == mWidgets.count() )
    {
        i = -1;
    }

    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::exist %d"), i );

    return i;
}

// End of file
