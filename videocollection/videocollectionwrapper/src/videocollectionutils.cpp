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
* Description: VideoCollectionUtils class implementation
* 
*/

// Version : %version: %

// INCLUDE FILES
#include <qobject.h>
#include <QTime>
#include <hbglobal.h>
#include <hbextendedlocale.h>

#include "videocollectionutils.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// instance
// -----------------------------------------------------------------------------
//
VideoCollectionUtils& VideoCollectionUtils::instance()
{
    static VideoCollectionUtils _collectionUtilsInstance;
    return _collectionUtilsInstance;
}
	
// -----------------------------------------------------------------------------
// VideoCollectionUtils
// -----------------------------------------------------------------------------
//
VideoCollectionUtils::VideoCollectionUtils()
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// ~CVideoCollectionUtils
// -----------------------------------------------------------------------------
//
VideoCollectionUtils::~VideoCollectionUtils()
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// prepareLengthString
// -----------------------------------------------------------------------------
//
QString VideoCollectionUtils::prepareLengthString(quint32 length)
{
	FUNC_LOG;
    const int secondsInMinute( 60 );
    const int secondsInHour( 3600 );
    QString hour;
    QString min;
    QString sec;
    
    HbExtendedLocale locale = HbExtendedLocale::system();
    
    if ( length > 0 )
    {
        quint32 hours = length / secondsInHour;
        quint32 minutes = length / secondsInMinute % secondsInMinute;
        quint32 seconds = length % secondsInMinute;
        
        if ( hours > 0 )
        {
            if(hours == 1)
            {
                hour = QObject::tr("%1 hour ").arg(locale.toString(hours)); //localisation
            }
            else
            {
                hour = QObject::tr("%1 hours ").arg(locale.toString(hours)); //localisation
            }
        }

        if ( minutes > 0 )
        {
            if(minutes == 1)
            {
                min = QObject::tr("%1 minute ").arg(locale.toString(minutes)); //localisation
            }
            else
            {
                min = QObject::tr("%1 minutes ").arg(locale.toString(minutes)); //localisation
            }
        }
        if (seconds > 0)
        {
            if(seconds == 1)
            {
                sec = QObject::tr("%1 second").arg(locale.toString(seconds)); //localisation
            }
            else
            {
                sec = QObject::tr("%1 seconds").arg(locale.toString(seconds)); //localisation
            }
        }
    } 
    else 
    {
        sec = QObject::tr("0 seconds"); //localisation
    }
    
    QString lengthStr(hbTrId( "txt_videos_list_l1_l2_l3" ).arg( hour ).arg( min ).arg( sec ));
    
    return lengthStr.trimmed();
}
// -----------------------------------------------------------------------------
// VideoCollectionUtils::prepareShortLengthString()
// -----------------------------------------------------------------------------
//
const QString VideoCollectionUtils::prepareShortLengthString(quint32 total)
{
	const int secondsInMinute( 60 );
    const int secondsInHour( 3600 );

    int hour = total / secondsInHour;
    total = total % secondsInHour;
    int minutes = total / secondsInMinute;
    total = total % secondsInMinute;
    int second = total;

    QTime time( hour ,minutes ,second );
    QString str;

    HbExtendedLocale locale = HbExtendedLocale::system();

    str = locale.format( time, r_qtn_time_durat_long_with_zero );

    return str;
}

// -----------------------------------------------------------------------------
// prepareSizeString
// -----------------------------------------------------------------------------
//
QString VideoCollectionUtils::prepareSizeString(quint32 size)
{
	FUNC_LOG;
    const int videoSizeGB( 0x40000000 );
    const int videoSizeHalfGB( 0x20000000 );
    const int videoSizeMB( 0x100000 );
    const int videoSizeHalfMB( 0x80000 );
    const int videoSizeKB( 0x400 );

    QString sizeStr("");
    
    if ( size > 0 )
    {
        quint32 dispSize = 0;
     
        HbExtendedLocale locale = HbExtendedLocale::system();
        
        if ( size >= videoSizeGB )
        {
            dispSize  = size + videoSizeHalfGB;
            dispSize /= videoSizeGB;
            sizeStr = QString(hbTrId("txt_videos_list_l1_gb").arg(locale.toString(dispSize)));
        }
        else if ( size >= videoSizeMB )
        {
            dispSize  = size + videoSizeHalfMB;
            dispSize /= videoSizeMB;
            sizeStr = QString(hbTrId("txt_videos_list_l1_mb").arg(locale.toString(dispSize)));
        }
        else
        {
            dispSize  = size + videoSizeKB;
            dispSize /= videoSizeKB;
            sizeStr = QString(hbTrId("txt_videos_list_l1_kb").arg(locale.toString(dispSize)));
        }
    }

    return sizeStr;
}

// End of file
