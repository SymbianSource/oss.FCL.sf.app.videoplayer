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
* Description:  definition of common data for video collection components
*
*/

#ifndef __VIDEOCOLLECTIONCOMMON_H__
#define __VIDEOCOLLECTIONCOMMON_H__


#include <QObject>

namespace VideoCollectionCommon
{
    /**
    * enums indicating keys for the detail data 
    * fetched from video model
    */
    enum VideoDetailKey
    {
        KeyUndefined = Qt::UserRole,
        KeyDateTime,
		KeySizeValue,
		KeyStatus,
		KeyMetaData,
		KeyFilePath,
        KeyLast
    };
    
    /**
    * Keys for the metadata map fetched from video model.
    * These are strings, because QVariant only supports QMap<QString, QVariant>
    */
    const char* const MetaKeyDate = "MetaKeyDate";
    const char* const MetaKeyDurationString = "MetaKeyDurationString";
    const char* const MetaKeySizeString = "MetaKeySizeString";
    const char* const MetaKeyStarRating = "MetaKeyStarRating";
    const char* const MetaKeyDRMInfo = "MetaKeyDRMInfo";
    const char* const MetaKeyServiceURL = "MetaKeyServiceURL";
    const char* const MetaKeyDescription = "MetaKeyDescription";
    const char* const MetaKeyModifiedDate = "MetaKeyModifiedDate";
    const char* const MetaKeyShotLocation = "MetaKeyShotLocation";
    const char* const MetaKeyAuthor = "MetaKeyAuthor";
    const char* const MetaKeyCopyright = "MetaKeyCopyright";
    const char* const MetaKeyAudioType = "MetaKeyAudioType";
    const char* const MetaKeyLanguageString = "MetaKeyLanguageString";
    const char* const MetaKeyKeywords = "MetaKeyKeywords";
    const char* const MetaKeyVideoResolutionString = "MetaKeyVideoResolutionString";
    const char* const MetaKeyBitRate = "MetaKeyBitRate";
    const char* const MetaKeyFormat = "MetaKeyFormat";
    
    enum VideoItemStatus
    {
        StatusNone = 0,
        StatusDeleted
    };
    
    /**
     * Labels for the different details. Needs to be in the same
     * order than the VideoDetailLabelKeys.
     */
    const char * const VideoDetailLabels[] = {
            QT_TR_NOOP("DRM: %1"),
            QT_TR_NOOP("Service: %1"),
            QT_TR_NOOP("Description: %1"),
            QT_TR_NOOP("Length: %1"),
            QT_TR_NOOP("Date: %1"),
            QT_TR_NOOP("Modified: %1"),
            QT_TR_NOOP("Location: %1"),
            QT_TR_NOOP("Author: %1"),
            QT_TR_NOOP("Copyright: %1"),
            QT_TR_NOOP("Audio Type: %1"),
            QT_TR_NOOP("Language: %1"),
            QT_TR_NOOP("Keywords: %1"),
            QT_TR_NOOP("Size: %1"),
            QT_TR_NOOP("Resolution: %1"),
            QT_TR_NOOP("Format: %1"),
            QT_TR_NOOP("Bitrate: %1")
    };

    /**
     * Order of keys in the labels const array.
     */
    const char * const VideoDetailLabelKeys[] = {
            MetaKeyDRMInfo,
            MetaKeyServiceURL,
            MetaKeyDescription,
            MetaKeyDurationString,
            MetaKeyDate,
            MetaKeyModifiedDate,
            MetaKeyShotLocation,
            MetaKeyAuthor,
            MetaKeyCopyright,
            MetaKeyAudioType,
            MetaKeyLanguageString,
            MetaKeyKeywords,
            MetaKeySizeString,
            MetaKeyVideoResolutionString,
            MetaKeyFormat,
            MetaKeyBitRate
    };
    
    
    /**
     * Collection related asyncronous 
     * operations status codes
     */
    enum VideoCollectionAsyncStatus
    {
        statusSingleDeleteFail = 1,
        statusMultipleDeleteFail,
        statusMultipleDeleteSucceed        
    };
    
   
}
#endif	// __VIDEOCOLLECTIONCOMMON_H__
