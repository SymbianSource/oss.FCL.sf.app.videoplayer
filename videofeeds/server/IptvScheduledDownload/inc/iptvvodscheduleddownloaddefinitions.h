/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Scheduled download definitions.*
*/




#ifndef CIPTVVODSCHEDULEDDOWNLOADDEFINITONS_H
#define CIPTVVODSCHEDULEDDOWNLOADDEFINITONS_H


//  DEFINITIONS

/**
* The network to use.
*/
enum TIptvVodScheduleConnectionCondition
    {
    EManual = 0,
    EWLAN,
    EHomeCellular,
    EAlways
    };

/**
* The scheduled times.
*/
enum TIptvVodScheduleDownloadtime
    {
    ENoSchedule = 0,
    ENight      = (1 << 0), //  00:00 - 05:59
    EMorning    = (1 << 1), //  06:00 - 10:59
    ENoon       = (1 << 2), //  11:00 - 12:59
    EAfternoon  = (1 << 3), //  13:00 - 17:59
    EEvening    = (1 << 4)  //  18:00 - 23:59
    };

/**
* Combination of the scheduled times.
*/
typedef TUint32 TIptvVodScheduleDownloadtimeCombination;

/**
* The scheduled download type
*/
enum TIptvVodScheduleDownloadType
    {
    ENoDownload = 0,
    EDownloadNumberMask = 0x00FFFFFF,   // bits 0 - 23 (n) are used as number
    EDownloadLatest = (1 << 31),        // only n latest
    EDownloadOldest = (1 << 30),        // only n oldest
    EDownloadAddedAfterLastScheduledDownload = (1 << 30),
                                        // added after last scheduled download
    EDownloadAll = (1 << 28)            // all
    };

/**
* Combination of the scheduled type and limits.
*/
typedef TUint32 TIptvVodScheduleDownloadTypeCombination;


#endif      // CIPTVVODSCHEDULEDDOWNLOADDEFINITONS_H
            
// End of File
