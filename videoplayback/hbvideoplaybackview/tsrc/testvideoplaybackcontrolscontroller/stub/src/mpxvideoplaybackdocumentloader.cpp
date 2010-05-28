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

// Version : %version:  4 %



#include <hblistwidget.h>
#include <hblabel.h>
#include <hbtransparentwindow.h>
#include <hbratingslider.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackcontrolscontroller.h"

#include "mpxvideoplaybackcontrolbar.h"
#include "mpxvideoplaybackfiledetailswidget.h"
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
    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::QMPXVideoPlaybackDocumentLoader : mWidgets.count(%d)"), 
            mWidgets.count() );

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
    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::findWidget(%s)"), name.data() );

    QGraphicsWidget *object = NULL;

    //int index = exist( name );
    mIndex = exist( name );

    if ( mIndex == -1 )
    {
        object = createWidget( name );
    }
    else
    {
        object = mWidgets[ mIndex ];
    }
    
    return object;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::createWidget()
// -------------------------------------------------------------------------------------------------
//
QGraphicsWidget *QMPXVideoPlaybackDocumentLoader::createWidget( const QString &name )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::createWidget(%s)"), name.data() );

    QGraphicsWidget *object = NULL;
    
    if ( name == "transparentWindow" ||
         name == "small_transparentWindow" )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::creating transparentWindow") );
        
        object = new HbTransparentWindow();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::appending[transparentWindow] to object list") );
        mWidgets.append( object );                
    }
    else if ( name == "content" || name == "bitmapLayout" )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::creating HbWidget") );
        
        object = new HbWidget();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::appending[HbWidget] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "title" || name == "bufferingIcon" || 
              name == "tvOutBitmap" || name == "realAudioOnlyBitmap" ||
              name == "partialAudioOnlyBitmap" || name == "audioOnlyBitmap" )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::creating HbLabel") );
        
        object = new HbLabel();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::appending[HbLabel] to object list") );                
        mWidgets.append( object );
    }
    else if ( name == "controlBarLayout" )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::creating controlBarLayout") );
    
        object = new QMPXVideoPlaybackControlBar( mController );
        object->setObjectName( name );
        
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::appending[controlBarLayout] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "fileDetailsLayout" )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::creating fileDetailsLayout") );
    
        object = new QMPXVideoPlaybackFileDetailsWidget( mController );
        object->setObjectName( name );
        
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::appending[fileDetailsLayout] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "detailsPlaybackWindow" )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::creating detailsPlaybackWindow") );
    
        object = new QMPXVideoPlaybackDetailsPlaybackWindow( mController );
        object->setObjectName( name );
        
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::appending[detailsPlaybackWindow] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "fileDetails" )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::creating HbListWidget") );
        
        object = new HbListWidget();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::appending[HbListWidget] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "nonLocalPlayback" )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::creating HbRatingSlider") );
        
        object = new HbRatingSlider();
        object->setObjectName( name );
        
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::appending[HbRatingSlider] to object list") );                        
        mWidgets.append( object );
    }
    else if ( name == "rnLogoBitmap" )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::creating rnLogoBitmap") );
        
        object = new HbWidget();
        object->setObjectName( name );

        MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::appending[rnLogoBitmap] to object list") );                        
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

    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::exist(%s) returns %d"), 
            name.data(), i );

    return i;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::load()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackDocumentLoader::load( 
        const QString &fileName, const QString &section , bool *ok )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::load(fileName=%s, section=%s)"), 
            fileName.data(), section.data() );
    
    Q_UNUSED( fileName );
    Q_UNUSED( section );
    *ok = true;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::load()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackDocumentLoader::load( 
        const QString &fileName, bool *ok )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::load(fileName=%s)"), fileName.data() );
    Q_UNUSED( fileName );
    *ok = true;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDocumentLoader::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackDocumentLoader::isVisible( int index )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDocumentLoader::isVisible(%d)"), index );
    
    return mWidgets[ index ]->isVisible();
}

// End of file
