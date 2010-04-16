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

// INCLUDE FILES


#include <qobject.h>
#include "videocollectionutils.h"


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
    
}

// -----------------------------------------------------------------------------
// ~CVideoCollectionUtils
// -----------------------------------------------------------------------------
//
VideoCollectionUtils::~VideoCollectionUtils()
{

}

// -----------------------------------------------------------------------------
// prepareLengthString
// -----------------------------------------------------------------------------
//
QString VideoCollectionUtils::prepareLengthString(quint32 length)
{
    const int secondsInMinute( 60 );
    const int secondsInHour( 3600 );
    QString lengthStr("");
    
    if ( length > 0 )
    {
        quint32 hours = length / secondsInHour;
        quint32 minutes = length / secondsInMinute % secondsInMinute;
        quint32 seconds = length % secondsInMinute;
        
        if ( hours > 0 )
        {
            if(hours == 1)
            {
                lengthStr = QObject::tr("%1 hour ").arg(QString::number(hours)); //localisation
            }
            else
            {
                lengthStr += QObject::tr("%1 hours ").arg(QString::number(hours)); //localisation
            }
        }

        if ( minutes > 0 )
        {
            if(minutes == 1)
            {
                lengthStr += QObject::tr("%1 minute ").arg(QString::number(minutes)); //localisation
            }
            else
            {
                lengthStr += QObject::tr("%1 minutes ").arg(QString::number(minutes)); //localisation
            }
        }
        if (seconds > 0 && hours == 0)
        {
            if(seconds == 1)
            {
                lengthStr += QObject::tr("%1 second").arg(QString::number(seconds)); //localisation
            }
            else
            {
                lengthStr += QObject::tr("%1 seconds").arg(QString::number(seconds)); //localisation
            }
        }
    } else {
        lengthStr += QObject::tr("0 seconds"); //TODO: Localisation
    }
    
    return lengthStr;
}
// -----------------------------------------------------------------------------
// VideoCollectionUtils::prepareLengthStrings()
// -----------------------------------------------------------------------------
//
const QStringList VideoCollectionUtils::prepareLengthStrings(quint32 total)
{
    const int secondsInMinute( 60 );
    const int secondsInHour( 3600 );

    quint32 hours(0);
    quint32 minutes(0);
    quint32 seconds(0);
    
    QString hrs("");
    QString mins("");
    QString secs("");

    if ( total > 0 )
    {
    	hours =   (total / secondsInHour);
    	total = total - (hours * secondsInHour);
    	minutes = (total / secondsInMinute);
        seconds = (total % secondsInMinute);
    }
    
    hrs = QString::number(hours); 
    
    if (minutes < 10)
    {
        mins = "0" + QString::number(minutes); 
        
    }
    else
    {
        mins = QString::number(minutes);
    }

    if (seconds < 10)
    {
        secs = "0" + QString::number(seconds); 
        
    }
    else
    {
        secs = QString::number(seconds);
    }
    
    QStringList retVal;
    
    retVal.append(hrs);
    retVal.append(mins);
    retVal.append(secs);
    
    return retVal;
}


// -----------------------------------------------------------------------------
// prepareSizeString
// -----------------------------------------------------------------------------
//
QString VideoCollectionUtils::prepareSizeString(quint32 size)
{
    const int videoSizeGB( 0x40000000 );
    const int videoSizeHalfGB( 0x20000000 );
    const int videoSizeMB( 0x100000 );
    const int videoSizeHalfMB( 0x80000 );
    const int videoSizeKB( 0x400 );
    const int videoSizeHalfKB( 0x200 );
    
    QString sizeStr("");
    
    if ( size > 0 )
    {
        quint32 dispSize = 0;
        
        if ( size >= videoSizeGB )
        {
            dispSize  = size + videoSizeHalfGB;
            dispSize /= videoSizeGB;
            sizeStr = QString(QObject::tr("%1 GB").arg(QString::number(dispSize))); //localisation
        }
        else if ( size >= videoSizeMB )
        {
            dispSize  = size + videoSizeHalfMB;
            dispSize /= videoSizeMB;
            sizeStr = QString(QObject::tr("%1 MB").arg(QString::number(dispSize))); //localisation
        }
        else if (size >= videoSizeKB) 
        {
            dispSize  = size + videoSizeHalfKB;
            dispSize /= videoSizeKB;
            sizeStr = QString(QObject::tr("%1 kB").arg(QString::number(dispSize))); //localisation
        }
        else
        {
            sizeStr = QString(QObject::tr("%1B").arg(QString::number(size))); //localisation
        }
    }
    
    return sizeStr;
}

// End of file
