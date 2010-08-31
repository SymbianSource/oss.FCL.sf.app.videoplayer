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
* Description:  Implementation of VideoPlaybackDocumentLoader
*
*/

// Version : %version:  5 %



#include "hbpushbutton.h"

#include "mpxvideo_debug.h"
#include "videoplaybackdocumentloader.h"


// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::~VideoPlaybackDocumentLoader
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDocumentLoader::~VideoPlaybackDocumentLoader()
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader") );

    for ( int i = 0 ; i < mWidgets.count() ; i++ )
    {
        mWidgets.removeAt( 0 );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::findWidget()
// -------------------------------------------------------------------------------------------------
//
QGraphicsWidget *VideoPlaybackDocumentLoader::findWidget( const QString &name )
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::findWidget( %s )"), name.data() );

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
// VideoPlaybackDocumentLoader::createWidget()
// -------------------------------------------------------------------------------------------------
//
QGraphicsWidget *VideoPlaybackDocumentLoader::createWidget( const QString &name )
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::createWidget( %s )"), name.data() );

    QGraphicsWidget *object = NULL;
        
    if ( name == "detailsViewPlayButton" || 
         name == "detailsAttachButton" ||
         name == "detailsShareButton" )
    {
        object = new HbPushButton();
        object->setObjectName( name );
        mWidgets.append( object );
    }

    return object;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::exist()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackDocumentLoader::exist( const QString &name )
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

    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::exist( %s ) ret %d"), name.data(), i );

    return i;
}

// End of file
