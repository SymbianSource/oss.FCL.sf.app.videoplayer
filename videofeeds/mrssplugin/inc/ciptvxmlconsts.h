/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef CIPTV3XMLCONSTS_H_
#define CIPTV3XMLCONSTS_H_

_LIT( KIptvJan, "jan" );
_LIT( KIptvFeb, "feb" );
_LIT( KIptvMar, "mar" );
_LIT( KIptvApr, "apr" );
_LIT( KIptvMay, "may" );
_LIT( KIptvJun, "jun" );
_LIT( KIptvJul, "jul" );
_LIT( KIptvAug, "aug" );
_LIT( KIptvSep, "sep" );
_LIT( KIptvOct, "oct" );
_LIT( KIptvNov, "nov" );
_LIT( KIptvDec, "dec" );

const TInt KIptvMonthShortNameLen( 3 );
const TInt KIptvShiftNumberMaxCount( 4 );
const TInt KIptvHourNumberCount( 2 );
const TInt KIptvMinuteNumberCount( 2 );

const TInt KIptvShiftNumberCountH( 1 );
const TInt KIptvShiftNumberCountHH( 2 );
const TInt KIptvShiftNumberCountHHM( 3 );
const TInt KIptvShiftNumberCountHHMM( KIptvShiftNumberMaxCount );

const TInt KIptvShiftHourHighIndex( 0 );
const TInt KIptvShiftHourLowIndex( 1 );
const TInt KIptvShiftMinuteHighIndex( 2 );
const TInt KIptvShiftMinuteLowIndex( 3 );


const TInt KIptvTimeZoneShift_None( 0 );
_LIT( KIptvTimeZone_GMT, "GMT" );
_LIT( KIptvTimeZone_UT, "UT" );
_LIT( KIptvTimeZone_Z, "Z" );
const TInt KIptvTimeZoneShift_Z( KIptvTimeZoneShift_None );

_LIT( KIptvTimeZone_A, "A" );
const TInt KIptvTimeZoneShift_A( -1 );

_LIT( KIptvTimeZone_B, "B" );
const TInt KIptvTimeZoneShift_B( -2 );

_LIT( KIptvTimeZone_C, "C" );
const TInt KIptvTimeZoneShift_C( -3 );

_LIT( KIptvTimeZone_EDT, "EDT" );
_LIT( KIptvTimeZone_D, "D" );
const TInt KIptvTimeZoneShift_D( -4 );

_LIT( KIptvTimeZone_EST, "EST" );
_LIT( KIptvTimeZone_CDT, "CDT" );
_LIT( KIptvTimeZone_E, "E" );
const TInt KIptvTimeZoneShift_E( -5 );

_LIT( KIptvTimeZone_CST, "CST" );
_LIT( KIptvTimeZone_MDT, "MDT" );
_LIT( KIptvTimeZone_F, "F" );
const TInt KIptvTimeZoneShift_F( -6 );

_LIT( KIptvTimeZone_MST, "MST" );
_LIT( KIptvTimeZone_PDT, "PDT" );
_LIT( KIptvTimeZone_G, "G" );
const TInt KIptvTimeZoneShift_G( -7 );

_LIT( KIptvTimeZone_PST, "PST" );
_LIT( KIptvTimeZone_H, "H" );
const TInt KIptvTimeZoneShift_H( -8 );

_LIT( KIptvTimeZone_I, "I" );
const TInt KIptvTimeZoneShift_I( -9 );

_LIT( KIptvTimeZone_K, "K" );
const TInt KIptvTimeZoneShift_K( -10 );

_LIT( KIptvTimeZone_L, "L" );
const TInt KIptvTimeZoneShiftL( -11 );

_LIT( KIptvTimeZone_M, "M" );
const TInt KIptvTimeZoneShift_M( -12 );

_LIT( KIptvTimeZone_N, "N" );
const TInt KIptvTimeZoneShift_N( 1 );

_LIT( KIptvTimeZone_O, "O" );
const TInt KIptvTimeZoneShift_O( 2 );

_LIT( KIptvTimeZone_P, "P" );
const TInt KIptvTimeZoneShift_P( 3 );

_LIT( KIptvTimeZone_Q, "Q" );
const TInt KIptvTimeZoneShift_Q( 4 );

_LIT( KIptvTimeZone_R, "R" );
const TInt KIptvTimeZoneShift_R( 5 );

_LIT( KIptvTimeZone_S, "S" );
const TInt KIptvTimeZoneShift_S( 6 );

_LIT( KIptvTimeZone_T, "T" );
const TInt KIptvTimeZoneShift_T( 7 );

_LIT( KIptvTimeZone_U, "U" );
const TInt KIptvTimeZoneShift_U( 8 );

_LIT( KIptvTimeZone_V, "V" );
const TInt KIptvTimeZoneShift_V( 9 );

_LIT( KIptvTimeZone_W, "W" );
const TInt KIptvTimeZoneShift_W( 10 );

_LIT( KIptvTimeZone_X, "X" );
const TInt KIptvTimeZoneShift_X( 11 );

_LIT( KIptvTimeZone_Y, "Y" );
const TInt KIptvTimeZoneShift_Y( 12 );

_LIT( KIptvYahooRss, "http://search.yahoo.com/mrss" );

// RSS 2.0
_LIT( KIptvRssRss, "rss" );
_LIT( KIptvRssChannel, "channel" );
_LIT( KIptvRssTitle, "title" );
_LIT( KIptvRssLink, "link" );
_LIT( KIptvRssDescription, "description" );
_LIT( KIptvRssItem, "item" );
_LIT( KIptvRssEnclosure, "enclosure" );
_LIT( KIptvRssGuid, "guid" );
_LIT( KIptvRssUrl, "url" );
_LIT( KIptvRssType, "type" );
_LIT( KIptvRssImage, "image" );
_LIT( KIptvRssPubDate, "pubDate" );
_LIT( KIptvRssLength, "length" );
_LIT( KIptvRssThumbnail, "thumbnail" );
_LIT( KIptvRssHref, "href" );
_LIT( KIptvRssAuthor, "author" );

// Yahoo media extension
_LIT( KIptvRssMediaGroup, "group" );
_LIT( KIptvRssMediaContent, "content" );
_LIT( KIptvRssMedia, "media" );
_LIT( KIptvRssMediaFileSize, "fileSize" );
_LIT( KIptvRssMediaMedium, "medium" );
_LIT( KIptvRssMediaIsDefault, "isDefault" );
_LIT( KIptvRssMediaExpression, "expression" );
_LIT( KIptvRssMediaBitrate, "bitrate" );
_LIT( KIptvRssMediaFramerate, "framerate" );
_LIT( KIptvRssMediaSamplingrate, "samplingrate" );
_LIT( KIptvRssMediaDuration, "duration" );
_LIT( KIptvRssMediaHeight, "height" );
_LIT( KIptvRssMediaWidth, "width" );
_LIT( KIptvRssMediaLanguage, "lang" );
_LIT( KIptvRssMediaSample, "sample" );
_LIT( KIptvRssMediaFull, "full" );
_LIT( KIptvRssMediaNonStop, "nonstop" );
_LIT( KIptvRssMediaCopyright, "copyright" );
_LIT( KIptvRssMediaRating, "rating" );
_LIT( KIptvRssMediaCategory, "category" );
_LIT( KIptvRssMediaRatingScheme, "scheme" );
_LIT( KIptvRssMediaRatingSchemeSimple, "urn:simple" );
_LIT( KIptvRssMediaRatingSchemeNum, "urn:num" );
_LIT( KIptvRssMediaAdult, "adult" );
_LIT( KIptvRssMediaLabel, "label" );

_LIT( KIptvRssRtsp, "rtsp://" );
_LIT( KIptvRssHttp, "http://" );

_LIT( KIptvTrue, "true" );

const TInt KIptvProtocolNameLength( 7 );
const TInt KIptvRssItemNameMaxLength( 255 );
const TInt KIptvRssEnclosureParameterMaxValueLength( 2048 );

const TInt KIptvMediaRatingAgeLength( 3 );

const TInt KRatingAgeAdult( 18 );
const TInt KRatingAgeUnlimited( 0 );

const TInt KIptvPopValueAndName( 2 );

// MIME types that need special handling.
//
_LIT( KIptvMimeApplicationSdp, "application/sdp" );


#endif /* CIPTV3XMLCONSTS_H_ */
