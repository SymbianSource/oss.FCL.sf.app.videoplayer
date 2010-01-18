/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   HG VOD utility class*
*/


#include <AknUtils.h>
#include <avkon.rsg>
#include <gulicon.h>
#include <cemailaccounts.h>
#include <CMessageData.h>
#include <eikenv.h>
#include <sendui.h>
#include <SendUiConsts.h>
#include <StringLoader.h>
#include <txtrich.h>
#include <tz.h>
#include <utf.h>
#include "vcxnscategory.h"
#include <vcxhgvodui.rsg>

#include "vcxhgvodutils.h"

//CONSTANTS
const TInt KVcxNsVideoSizeGB     = 0x40000000;
const TInt KVcxNsVideoSizeHalfGB = 0x20000000;
const TInt KVcxNsVideoSizeMB     = 0x100000;
const TInt KVcxNsVideoSizeHalfMB = 0x80000;
const TInt KVcxNsVideoSizeKB     = 0x400;
const TInt KVcxNsVideoSizeHalfKB = 0x200; 

const TInt KVcxNsHourInSeconds   = 3600;
const TInt KVcxNsMinuteInSeconds = 60;

const TInt KVcxNsLengthNumbersArraySize = 2;

_LIT(KVcxNsComma, ", ");

// -----------------------------------------------------------------------------
// CVcxHgVodUtils::~CVcxHgVodUtils()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodUtils::~CVcxHgVodUtils()
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgVodUtils::CVcxHgVodUtils()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodUtils::CVcxHgVodUtils( )
    {
    }

// -----------------------------------------------------------------------------
//       Landscape    Portrait
// QVGA  320x240  or  240x320
// QHD   640x360  or  360x640
// VGA   640x480  or  480x640
// -----------------------------------------------------------------------------
//
CVcxHgVodUtils::TVcxScreenResolution CVcxHgVodUtils::GetScreenResolution( 
        CCoeEnv* aCoeEnv )
    {
    TVcxScreenResolution resolution( EVcxScreenResolutionUnknown );
    TSize screenSize = aCoeEnv->ScreenDevice()->SizeInPixels(); 
    TBool landscape = screenSize.iWidth > screenSize.iHeight;

    if ( ( landscape && screenSize.iHeight <= 240 ) ||
         ( ! landscape && screenSize.iWidth <= 240 ) )
        {
        resolution = EVcxScreenResolutionQVGA;
        }
    else if ( ( landscape && screenSize.iHeight <= 360 ) ||
              ( ! landscape && screenSize.iWidth <= 360 ) )
        {
        resolution = EVcxScreenResolutionQHD;
        }
    else if ( ( landscape && screenSize.iHeight <= 480 ) ||
              ( ! landscape && screenSize.iWidth <= 480 ) )
        {
        resolution = EVcxScreenResolutionVGA;
        }

    return resolution;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodUtils::LoadStringFromResourceLC
// -----------------------------------------------------------------------------
//
HBufC* CVcxHgVodUtils::LoadStringFromResourceLC( TInt aResourceId )
    {
    return StringLoader::LoadLC( aResourceId );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodUtils::LoadStringFromResourceLC
// -----------------------------------------------------------------------------
//
HBufC* CVcxHgVodUtils::LoadStringFromResourceLC( 
    TInt aResourceId, const TDesC& aDesc )
    {
    return StringLoader::LoadLC( aResourceId, aDesc );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodUtils::LoadStringFromResourceLC
// -----------------------------------------------------------------------------
//
HBufC* CVcxHgVodUtils::LoadStringFromResourceLC( TInt aResourceId, const TInt aNum )
    {
    return StringLoader::LoadLC( aResourceId, aNum );
    }

// ---------------------------------------------------------
// CVcxHgVodUtils::FormatDateAndTimeL()
// ---------------------------------------------------------
//
void CVcxHgVodUtils::FormatDateAndTimeL( 
    TTime& aTime, 
    TDes& aFormattedString )
    {    
    TTime pubDate(aTime);
    aFormattedString.Zero();
    
    //Convert GMT time to local time
    RTz rtz;
    User::LeaveIfError( rtz.Connect() );
    CleanupClosePushL( rtz );
    User::LeaveIfError( rtz.ConvertToLocalTime( pubDate ) );
    CleanupStack::PopAndDestroy( &rtz );
    
    HBufC* dateFormatString = StringLoader::LoadLC( R_QTN_DATE_USUAL ); 
    HBufC* timeFormatString = StringLoader::LoadLC( R_QTN_TIME_USUAL ); 
    
    TBuf<KVcxNsMaxDateBufSize> dateString;
    // Format the date to user readable format. 
    // The format is locale dependent
    pubDate.FormatL( dateString, *dateFormatString );  
    aFormattedString.Append( dateString ); 
    
    aFormattedString.Append( KVcxNsSpaceString ); 

    TBuf<KVcxNsMaxTimeBufSize> timeString;
    // Format the time to user readable format.
    // The format is locale dependent
    pubDate.FormatL( timeString, *timeFormatString );  
    aFormattedString.Append( timeString ); 

    AknTextUtils::LanguageSpecificNumberConversion( aFormattedString );

    CleanupStack::PopAndDestroy( timeFormatString );
    CleanupStack::PopAndDestroy( dateFormatString );
    }

// ---------------------------------------------------------
// CVcxHgVodUtils::FormatLengthAndSizeL()
// ---------------------------------------------------------
//
void CVcxHgVodUtils::FormatLengthAndSizeL( 
    TUint32 aLength, 
    TUint32 aSize, 
    TDes& aFormattedString )
    {
    aFormattedString.Zero();

    if ( aLength > 0 )
        {
        HBufC* videoLength;
        
        TInt hours  = aLength / KVcxNsHourInSeconds;
        TInt minutes = ( aLength - ( hours * KVcxNsHourInSeconds ) ) / KVcxNsMinuteInSeconds;

        if ( hours > 0 )
            {
            CArrayFix<TInt>* numbers = 
                new (ELeave) CArrayFixFlat<TInt>(KVcxNsLengthNumbersArraySize); 
            CleanupStack::PushL( numbers );
            
            numbers->AppendL( hours );
            numbers->AppendL( minutes );
            
            videoLength = StringLoader::LoadLC( R_VCXHG_VIDEOLENGTH_FULL, *numbers ); 
            
            aFormattedString.Append( *videoLength );
                
            CleanupStack::PopAndDestroy( videoLength );
            CleanupStack::PopAndDestroy( numbers ); 
            }
        else 
            {
            if ( hours == 0 && minutes == 0 )
                {
                minutes = 1;
                }
            
            videoLength = StringLoader::LoadLC( R_VCXHG_VIDEOLENGTH_MIN, minutes ); 
            aFormattedString.Append( *videoLength );
            CleanupStack::PopAndDestroy( videoLength );
            }                        
        }    

    if ( aSize > 0 )
        {
        if ( aFormattedString.Length() > 0  )
            {
            aFormattedString.Append( KVcxNsComma ); //add comma separator 
            }

        HBufC* videosize;
        TUint dispSize;
    
        if ( aSize >= KVcxNsVideoSizeGB )                    // 1 GB 
            {
            dispSize  = aSize + KVcxNsVideoSizeHalfGB;       // ½ GB 
            dispSize /= KVcxNsVideoSizeGB;
            videosize   = StringLoader::LoadLC( R_VCXHG_VIDEOSIZE_GB, dispSize );
            }
        else if ( aSize >= KVcxNsVideoSizeMB )               // 1 MB 
            {
            dispSize  = aSize + KVcxNsVideoSizeHalfMB;       // ½ MB 
            dispSize /= KVcxNsVideoSizeMB;
            videosize   = StringLoader::LoadLC( R_VCXHG_VIDEOSIZE_MB, dispSize );
            }
        else
            {
            dispSize  = aSize + KVcxNsVideoSizeHalfKB;       // ½ kB 
            dispSize /= KVcxNsVideoSizeKB;                           
            if ( dispSize == 0 ) 
                {
                dispSize = 1;
                }
            videosize = StringLoader::LoadLC( R_VCXHG_VIDEOSIZE_KB, dispSize );
            }
    
        aFormattedString.Append( *videosize );
        CleanupStack::PopAndDestroy( videosize );
        }

    }

// ---------------------------------------------------------
// CVcxHgVodUtils::FormatDownloadStateL()
// ---------------------------------------------------------
//
void CVcxHgVodUtils::FormatDownloadStateL( 
    CVcxNsContent::TVcxContentDlStatus aStatus,
    TInt aProgress,
    TDes& aFormattedString )
    {
    aFormattedString.Zero();
   
    HBufC* desString = NULL;
    
    switch( aStatus )
        {
        case CVcxNsContent::EVcxContentDlStateDownloading:
            {
            desString = StringLoader::LoadLC( R_IPTV_VIDEO_DOWNLOADING, aProgress );
            }
            break;
        case CVcxNsContent::EVcxContentDlStateFailed:
            {
            desString = StringLoader::LoadLC( R_IPTV_DOWNLOAD_FAILED, aProgress );
            }
            break;
        case CVcxNsContent::EVcxContentDlStatePaused:
            {
            desString = StringLoader::LoadLC( R_IPTV_DOWNLOAD_PAUSED, aProgress );
            }
            break;
        case CVcxNsContent::EVcxContentDlStateDownloaded:
            {
            desString = StringLoader::LoadLC( R_IPTV_VIDEO_DOWNLOADED );
            }
            break; 
        default:
            {
            desString = KNullDesC().AllocLC();
            }
            break;
        }

    aFormattedString.Append( *desString );
    CleanupStack::PopAndDestroy( desString );
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CVcxHgVodUtils::IsSearchCategory( CVcxNsCategory& aCategory )
    {
    return aCategory.GetCategoryId() == KVcxNsSearchCategoryId ? ETrue : EFalse;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
HBufC* CVcxHgVodUtils::ResolveCategoryNameLC( CVcxNsCategory& aCategory )
    {
    if ( IsSearchCategory( aCategory ) )
        {
        return LoadStringFromResourceLC( R_VCXHG_VIDEO_SEARCH );
        }
    else if ( aCategory.GetName().Length() == 0 )
        {
        return LoadStringFromResourceLC( R_VCXHG_CATEGORIES_VIDEO_LIST );
        }
    else
        {
        return aCategory.GetName().AllocLC();
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
HBufC* CVcxHgVodUtils::ResolveCategoryInfoLC( CVcxNsCategory& aCategory )
    {
    if( IsSearchCategory( aCategory ) )
        {
        if( aCategory.GetVideoCount() == 0 )
            {
            return HBufC::NewLC(0);
            }
        else if( aCategory.GetVideoCount() == 1 )
            {
            return LoadStringFromResourceLC( R_VCXHG_SEARCH_VIDEOS_FOUND_ONE );
            }
        else
            {
            return LoadStringFromResourceLC( R_VCXHG_SEARCH_VIDEOS_FOUND, 
                                             aCategory.GetVideoCount() );            
            }
        }
    else
        {
        if( aCategory.GetVideoCount() == 1 )
            {
            return LoadStringFromResourceLC( R_VCXHG_ONE_VIDEO_IN_CATEGORY );
            }
        else
            {
            return LoadStringFromResourceLC( R_VCXHG_VIDEOS_IN_CATEGORY,
                                             aCategory.GetVideoCount() );            
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodUtils::IsDefaultSmtpAccountDefinedL()
// -----------------------------------------------------------------------------
//
TBool CVcxHgVodUtils::IsDefaultSmtpAccountDefinedL()
    {
    CEmailAccounts* mailAccounts = CEmailAccounts::NewLC();

    TSmtpAccount account;
    TInt error = mailAccounts->DefaultSmtpAccountL( account );

    CleanupStack::PopAndDestroy( mailAccounts );
    return ( KErrNone == error );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodUtils::ShareLinkL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodUtils::ShareLinkL(
    const TDesC& aLink,
    const TUid aMessageTypeModule,
    CEikonEnv& aEikonEnv )
    {
    CSendUi* sendUi = CSendUi::NewLC();
    TInt linkLength = aLink.Length();

    // Must be able to send at least linkLength long body text,
    // make sure it's supported
    TSendingCapabilities sendingCapabilities(
        linkLength, linkLength, TSendingCapabilities::ESupportsBodyText );

    if ( sendUi->ValidateServiceL( aMessageTypeModule, sendingCapabilities ) )
        {
        CMessageData* message = CMessageData::NewLC();
        CRichText* richText = CRichText::NewL(
                aEikonEnv.SystemParaFormatLayerL(),
                aEikonEnv.SystemCharFormatLayerL() );
        CleanupStack::PushL( richText );

        richText->InsertL( 0, aLink );
        message->SetBodyTextL( richText );

        sendUi->CreateAndSendMessageL(
                aMessageTypeModule,
                message,
                KNullUid,
                EFalse ); // ETrue:  Open in embedded mode
                          // EFalse: Do not embed message editor

        CleanupStack::PopAndDestroy( richText );
        CleanupStack::PopAndDestroy( message );
        }
    else
        {
        // Sending link with aServiceUid is not supported
        User::Leave( KErrNotSupported );
        }

    CleanupStack::PopAndDestroy( sendUi );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodUtils::DuplicateGulIconL()
// -----------------------------------------------------------------------------
//
CGulIcon* CVcxHgVodUtils::DuplicateGulIconL( CGulIcon* aIcon )
    {
    CFbsBitmap* icon = new (ELeave) CFbsBitmap;
    CleanupStack::PushL( icon );
    
    CFbsBitmap* mask = NULL;

    icon->Duplicate( aIcon->Bitmap()->Handle() );
    
    if ( aIcon->Mask() )
        {
        mask = new (ELeave) CFbsBitmap();
        CleanupStack::PushL( mask );
        mask->Duplicate( aIcon->Mask()->Handle() );
        }
    
    CGulIcon* newIcon = CGulIcon::NewL( icon, mask );
    
    if ( mask )
        {
        CleanupStack::Pop( mask );
        }
    
    CleanupStack::Pop( icon );
    
    return newIcon;
    }
