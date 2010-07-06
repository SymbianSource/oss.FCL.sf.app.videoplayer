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

// Version : %version:  26 %


#include <QDir>
#include <QDateTime>
#include <QFileInfo>

#include <hbglobal.h>
#include <hblistwidgetitem.h>
#include <hbratingslider.h>
#include <hblistwidget.h>
#include <hblistviewitem.h>
#include <hbextendedlocale.h>
#include <hbanchorlayout.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackfiledetailswidget.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackcontrolscontroller.h"

const float KILOBYTE = 1024 ;

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
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::updateWithFileDetails
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

        //
        // set the min/max number of lines in the second row
        //
        HbListViewItem *prototype = mListWidget->listItemPrototype();
        prototype->setSecondaryTextRowCount( 1, 30 );

        //
        // Title
        //
        makeTitleItem( details );

        if ( ! mFileDetailsUpdated )
        {
            HbExtendedLocale locale = HbExtendedLocale::system();

            mFileDetailsUpdated = true;

            //
            // Description
            //
            addItemToListWidget(
                    hbTrId( "txt_videos_list_description" ), details->mDescription );

            //
            // Duration
            //
            if ( details->mPlaybackMode != EMPXVideoLiveStreaming && details->mDuration > 0 )
            {
                int value = (qreal)details->mDuration / (qreal)KPbMilliMultiplier;
                QString hour = locale.toString( value / 3600 );
                value = value % 3600;
                QString min = locale.toString( value / 60 );
                value = value % 60;
                QString sec = locale.toString( value );

                addItemToListWidget(
                        hbTrId( "txt_videos_list_duration" ),
                        hbTrId( "txt_videos_list_l1l2l3" ).arg( hour ).arg( min ).arg( sec ) );
            }

            //
            // Date/Time
            //
            makeDateTimeItem( details );

            //
            // Location
            //
            addItemToListWidget( hbTrId( "txt_videos_list_location" ), details->mLocation );

            //
            // Author
            //
            addItemToListWidget( hbTrId( "txt_videos_list_author" ), details->mArtist );

            //
            // Copyright
            //
            addItemToListWidget( hbTrId( "txt_videos_list_copyright" ), details->mCopyright );

            //
            // Language
            //
            addItemToListWidget( hbTrId( "txt_videos_list_language" ), details->mLanguage );

            //
            // Keywords
            //
            addItemToListWidget( hbTrId( "txt_videos_list_keywords" ), details->mKeywords );

            //
            // Size
            //
            makeSizeItem( details );

            //
            // Resolution
            //
            if ( details->mVideoEnabled )
            {
                QString resolution = hbTrId( "txt_videos_list_l1l2" )
                       .arg( locale.toString( details->mVideoWidth ) )
                       .arg( locale.toString( details->mVideoHeight ) );
                addItemToListWidget( hbTrId( "txt_videos_list_resolution" ), resolution );
            }

            //
            // Format
            //
            addItemToListWidget( hbTrId( "txt_videos_list_format" ), details->mMimeType );

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
                QFileInfo fileInfo( details->mClipName );
                QString folder = fileInfo.dir().dirName();
                addItemToListWidget( hbTrId( "txt_videos_list_collection_name" ), folder );
            }
        }

        //
        // Set the rect size dynamically based on the view mode
        //
        QGraphicsWidget *content = loader->findWidget( QString( "content" ) );
        HbAnchorLayout *layout = static_cast<HbAnchorLayout*>( content->layout() );

        if ( layout )
        {
            QGraphicsWidget *titleLayout = loader->findWidget( QString( "titleLayout" ) );
            QRectF titleRect = titleLayout->geometry();

            switch ( mController->viewMode() )
            {
                case EDetailsView:
                {
                    layout->setAnchor( titleLayout, Hb::BottomEdge, this, Hb::TopEdge, 0 );
                    layout->setAnchor( layout, Hb::BottomEdge, this, Hb::BottomEdge, 0 );

                    break;
                }
                case EAudioOnlyView:
                {
                    QGraphicsWidget *controlLayout = loader->findWidget( QString( "controlBarLayout" ) );
                    QRectF controlRect = controlLayout->geometry();

                    layout->setAnchor(
                            this, Hb::TopEdge, titleLayout,
                            Hb::BottomEdge, titleRect.bottom() - titleRect.top() );

                    layout->setAnchor(
                            this, Hb::BottomEdge,
                            layout, Hb::BottomEdge, layout->geometry().height() - controlRect.top() );

                    break;
                }
            }
        }
    }
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
                title = fileInfo.completeBaseName();
            }

            //
            // populate Title and its associated text
            //
            HbListWidgetItem* listWidgetItem = new HbListWidgetItem();
            listWidgetItem->setText( hbTrId( "txt_videos_list_title" ) );
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

        //
        // to prevent overflow, get an unsigned value of file size
        //
        ulong fileSize = fileInfo.size();

        //
        // convert file size to KB, MB, GB accordingly
        //
        if ( fileSize > 0 )
        {
            HbExtendedLocale locale = HbExtendedLocale::system();

            QString scale;

            //
            // cast for later conversion with floating point
            //
            float size = (float) fileSize;

            if ( size > KILOBYTE )
            {
                size /= KILOBYTE;
                scale = hbTrId( "txt_videos_list_l1_kb" );
            }

            if ( size > KILOBYTE )
            {
                size /= KILOBYTE;
                scale = hbTrId( "txt_videos_list_l1_mb" );
            }

            if ( size > KILOBYTE )
            {
                size /= KILOBYTE;
                scale = hbTrId( "txt_videos_list_l1_gb" );
            }

            int temp = size * 10;
            size = (float)temp / 10;

            addItemToListWidget(
                    hbTrId( "txt_videos_list_file_size" ), scale.arg( locale.toString( size ) ) );
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
        HbExtendedLocale locale = HbExtendedLocale::system();

        float bitrate = details->mBitRate;

        QString scale = hbTrId( "txt_videos_list_l1_kbps" );

        if ( bitrate > KILOBYTE )
        {
             bitrate /= KILOBYTE;
        }

        if ( bitrate > KILOBYTE )
        {
             bitrate /= KILOBYTE;
             scale = hbTrId( "txt_videos_list_l1_mbps" );
        }

        int temp = bitrate * 10;
        bitrate = (float)temp / 10;

        addItemToListWidget(
                hbTrId( "txt_videos_list_bitrate" ), scale.arg( locale.toString( bitrate ) ) );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::addItemToListWidget
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::addItemToListWidget( QString item, QString text  )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::addItemToListWidget"));

    if ( text.count() > 0 )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackFileDetailsWidget::addItemToListWidget( %s %s )"),
		    item.data(), text.data() );

        HbListWidgetItem* listWidgetItem = new HbListWidgetItem();
        listWidgetItem->setText( item );
        listWidgetItem->setSecondaryText( text );

        mListWidget->addItem( listWidgetItem );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::makeDateTimeItem
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::makeDateTimeItem( QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::makeDateTimeItem"));

    //
    // attempt to get 'created' and 'last modified' date/time from metadata first,
    // if it's not available, then get it from the file system
    //

    if ( details->mPlaybackMode == EMPXVideoLocal ||
         details->mPlaybackMode == EMPXVideoProgressiveDownload )
    {
        QDateTime dateTime;
        QFileInfo fileInfo( details->mClipName );
        HbExtendedLocale locale = HbExtendedLocale::system();

        //
        // Date created
        //
        if ( details->mCreationTime > 0 )
        {
            dateTime.setTime_t( details->mCreationTime );
        }
        else
        {
            dateTime = fileInfo.created();
        }

        //
        // convert 'created' date/time to proper string format
        // according to its current locale
        //
        QString date = locale.format( dateTime.date(), r_qtn_date_usual );
        QString time = locale.format( dateTime.time(), r_qtn_time_long_with_zero );
        addItemToListWidget( hbTrId( "txt_videos_list_date" ), date + "  " + time );

        //
        // Date modified
        //
        if ( details->mModificationTime > 0 )
        {
            dateTime.setTime_t( details->mModificationTime );
        }
        else
        {
            dateTime = fileInfo.lastModified();
        }

        //
        // convert 'last modified' date/time to proper string format
        // according to its current locale
        //
        date = locale.format( dateTime.date(), r_qtn_date_usual );
        time = locale.format( dateTime.time(), r_qtn_time_long_with_zero );
        addItemToListWidget( hbTrId( "txt_videos_list_modified" ), date + "  " + time );
    }
}

//End of file
