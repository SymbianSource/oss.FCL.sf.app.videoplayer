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

// Version : %version:  13 %



#include <qfileinfo>
#include <qgraphicslinearlayout>
#include <qdatetime>
#include <qdir>

#include <hblistwidgetitem.h>
#include <hbratingslider.h>
#include <hblistwidget.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackfiledetailswidget.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackcontrolscontroller.h"

const float KILOBYTE = 1024 ;
const QString KDATETIMEFORMAT = "d/M/yyyy hh:mm:ss ap";

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::QMPXVideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackFileDetailsWidget::QMPXVideoPlaybackFileDetailsWidget( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
    , mListWidget( 0 )
    , mFileDetailsUpdated( false )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::QMPXVideoPlaybackFileDetailsWidget()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::~QMPXVideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackFileDetailsWidget::~QMPXVideoPlaybackFileDetailsWidget()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::~QMPXVideoPlaybackFileDetailsWidget()"));

    HbStyleLoader::unregisterFilePath( ":/hbvideoplaybackview/hblistwidget.css" );
    
    if ( mListWidget ) 
    {
        delete mListWidget;
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::initialize
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::initialize()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::initialize"));

    updateWithFileDetails( mController->fileDetails() );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::updateControlsWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    QMPXVideoPlaybackDocumentLoader *loader = mController->layoutLoader();

    if ( loader )
    {
        //
        // load fileDetails widget
        //
        QGraphicsWidget *widget = loader->findWidget( QString( "fileDetails" ) );
        
        //
        // cast fileDetails widget to HbListWidget
        //
        mListWidget = qobject_cast<HbListWidget*>( widget );

        if ( ! mFileDetailsUpdated )
        {        
            //
            // load file details style sheet
            //
            bool ret = HbStyleLoader::registerFilePath( ":/hbvideoplaybackview/hblistwidget.css" );
            
            mFileDetailsUpdated = true;
            
            QFileInfo fileInfo( details->mClipName );     

            //
            // Title
            //            
            makeTitleItem( details );
            
            //
            // Description
            //
            addItemToListWidget( "Description", details->mDescription ); 
            
            //
            // Duration
            //
            if ( details->mPlaybackMode != EMPXVideoLiveStreaming && details->mDuration > 0 )
            {
                QString duration = QString("%1").arg( 
                        valueToReadableFormat( (qreal)details->mDuration / (qreal)KPbMilliMultiplier ) );                    
                addItemToListWidget( "Duration", duration );                                                                        
            }    
            
            //
            // Date 
            //
            if ( details->mPlaybackMode == EMPXVideoLocal ||
                 details->mPlaybackMode == EMPXVideoProgressiveDownload )
            {       
                //
                // Date created
                //
                QString date = QString("%1").arg( fileInfo.created().toString( KDATETIMEFORMAT ) );                
                addItemToListWidget( "Date", date );      
                
                //
                // Date modified
                //
                QString modified = QString("%1").arg( fileInfo.lastModified().toString( KDATETIMEFORMAT ) );                
                addItemToListWidget( "Modified", modified );                                                                        
            }

            //
            // Location
            //
            addItemToListWidget( "Location", details->mLocation );                                                
            
            //
            // Author
            //
            addItemToListWidget( "Author", details->mArtist );                                            
            
            //
            // Copyright
            //
            addItemToListWidget( "Copyright", details->mCopyright );                                        
            
            //
            // Language
            //
            addItemToListWidget( "Language", details->mLanguage );                                    
            
            //
            // Keywords
            //
            addItemToListWidget( "Keywords", details->mKeywords );                                
            
            //
            // Size
            //
            makeSizeItem( details );

            //
            // Resolution
            //
            if ( details->mVideoEnabled )
            {
                QString resolution = QString("%1 x %2")
                       .arg( details->mVideoWidth ).arg( details->mVideoHeight );            
                addItemToListWidget( "Resolution", resolution );                    
            }
            
            //
            // Format
            //
            addItemToListWidget( "Format", details->mMimeType );                    
            
            //
            // Bitrate
            //
            makeBitRateItem( details );
            
            //
            // Folder
            //
            if ( details->mPlaybackMode == EMPXVideoLocal ||
                 details->mPlaybackMode == EMPXVideoProgressiveDownload)
            {
                QString folder = fileInfo.dir().dirName();                    
                addItemToListWidget( "Collection", folder );    
            }
            
        }

        //
        // Set the rect size dynamically based on the view mode
        //
        QString sectionName;

        if ( mController->viewMode() == EDetailsView )
        {
            sectionName = "detailsView";
        }
        else if ( mController->viewMode() == EAudioOnlyView )
        {
            if ( details->mPlaybackMode == EMPXVideoLocal )
            {
                sectionName = "audioOnlyViewWithLocalPlayback";
            }
            else
            {
                sectionName = "audioOnlyViewWithNonLocalPlayback";
            }
        }

        bool ok = false;
        loader->load( KMPXPLAYBACKVIEW_XML, sectionName, &ok );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::valueToReadableFormat
// -------------------------------------------------------------------------------------------------
//
QString QMPXVideoPlaybackFileDetailsWidget::valueToReadableFormat( int value )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFileDetailsWidget::valueToReadableFormat value = %d"), value);

    int hour = value / 3600;
    value = value % 3600;
    int minutes = value / 60;
    value = value % 60;
    int second = value;

    QTime time( hour ,minutes ,second );
    QString str;
    
    if ( hour > 0 )
    {
        str = time.toString( QString::number( hour ) ).append(" hr "); 
    }
    
    if ( minutes > 0 )
    {
        str.append( time.toString( QString::number( minutes ) ).append(" min ")); 
    }
    
    if ( second > 0 )
    {
        str.append( time.toString( QString::number( second ) ).append(" sec ")); 
    }
    
    return str;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::makeTitleItem
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::makeTitleItem( QMPXVideoPlaybackViewFileDetails* details )
{    
    MPX_DEBUG(_L("QMPXVideoPlaybackFileDetailsWidget::makeTitleItem()"));

    //
    // Title should be the first item in file details list
    //
    if ( mController->viewMode() == EAudioOnlyView )
    {    
        //
        // show the title for audio-only view 
        //
        if ( mListWidget->count() == 0 || ! mListWidget->item( 0 )->text().contains( "Title", Qt::CaseInsensitive ) )
        {
            QString title = details->mTitle;

            if ( title.count() == 0 )
            {
                //
                // If title is not available, show clip name
                //            
                QFileInfo fileInfo( details->mClipName );
                title = fileInfo.baseName ();
            }

            //
            // populate Title and its associated text
            //
            HbListWidgetItem* listWidgetItem = new HbListWidgetItem();
            listWidgetItem->setText( "Title" );
            listWidgetItem->setSecondaryText( title );
            mListWidget->insertItem( 0, listWidgetItem );
        }
    }
    else if ( mListWidget->count() &&
              mListWidget->item( 0 )->text().contains( "Title", Qt::CaseInsensitive ) )
    {
        // 
        // title is displayed under the status pane for video clips,
        // so hide the title for this details view
        //
        delete mListWidget->takeItem ( 0 );
    }
    
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::makeSizeItem
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::makeSizeItem( QMPXVideoPlaybackViewFileDetails* details )
{        
    if ( details->mPlaybackMode == EMPXVideoLocal ||
         details->mPlaybackMode == EMPXVideoProgressiveDownload )
    {
        QFileInfo fileInfo( details->mClipName );     
        float size = fileInfo.size();
          
        if ( size > 0.0 )
        {        
            QString scale(" KB");
        
            if ( size > KILOBYTE )
            {
                size /= KILOBYTE;
            }
            
            if ( size > KILOBYTE )
            {
                size /= KILOBYTE;
                scale = QString(" MB");
            }
     
            if ( size > KILOBYTE )
            {
                size /= KILOBYTE;
                scale = QString(" GB");
            }
             
            int temp = size * 10;
            size = (float)temp / 10;
    
            QString text = QString("%1").arg( size );
            text.append( scale );
            
            addItemToListWidget( "Size", text );                            
        }    
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::makeBitRateItem
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::makeBitRateItem( QMPXVideoPlaybackViewFileDetails* details  )
{        
    if ( details->mBitRate > 0 )
    {
        float bitrate = details->mBitRate;
    
        QString scale(" Kbps");
        
        if ( bitrate > KILOBYTE )
        {
             bitrate /= KILOBYTE;
        }
        
        if ( bitrate > KILOBYTE )
        {
             bitrate /= KILOBYTE;
             scale = QString(" Mbps");
        }
    
        int temp = bitrate * 10;
        bitrate = (float)temp / 10;
    
        QString text = QString("%1").arg( bitrate );
        text.append( scale );
        
        addItemToListWidget( "Bitrate", text );            
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::addItemToListWidget
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::addItemToListWidget( QString item, QString text  )
{    
    if ( text.count() > 0 )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackFileDetailsWidget::addItemToListWidget( %s , %s )"), item.data(), text.data() );
		
        HbListWidgetItem* listWidgetItem = new HbListWidgetItem();
        listWidgetItem->setText( item );
        listWidgetItem->setSecondaryText( text );
        
        mListWidget->addItem( listWidgetItem );
    }
}

//End of file
