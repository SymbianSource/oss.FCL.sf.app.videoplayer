/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Utility class and constants used by Iptv application.*
*/




// INCLUDE FILES
#include <e32base.h>
#include <e32debug.h>
#include <f32file.h>    // RFs
#include <apgcli.h>     // RApaLsSession
#include <s32mem.h>
#include <centralrepository.h>
#include <pathinfo.h> 
//#include <oem/mplayerconstants.h> // Some Mime types used by player.
#include <bautils.h>
#include <bldvariant.hrh>

#include "CIptvUtil.h"
#include "IptvDebug.h"
#include <WlanCdbCols.h>
#include "CIptvDriveMonitor.h"
#include "IptvExternalFolderInfo.h"

// CONSTANTS

_LIT(KIptvRootPathC, ":\\data\\videocenter");  // System drive
_LIT(KIptvRootPathE, ":\\data\\videocenter");  // E drive and other drives

// Permanent folders. Beginning of path is generated dynamically.
_LIT( KMyVideosExternalFolderOwn, "My Videos" );

_LIT(KSlash,"\\");
_LIT(KColonAndSlash, ":\\");

static const TInt KNumberOfLanguages = 96;

typedef struct
	{
	TLanguage iLanguage;
	char* iLanguageCode;
	} SLanguageItem;

static const SLanguageItem languageArray[ KNumberOfLanguages ] =
	{
		{ ELangEnglish, "en" },
		{ ELangFrench, "fr" },
		{ ELangGerman, "de" },
		{ ELangSpanish, "es" },
		{ ELangItalian, "it" },
		{ ELangSwedish, "sv" },
		{ ELangDanish, "da" },
		{ ELangNorwegian, "nn" },
		{ ELangFinnish, "fi" },
		{ ELangAmerican, "am" },
		{ ELangSwissFrench, "sf" },
		{ ELangSwissGerman, "sg" },
		{ ELangPortuguese, "pt" },
		{ ELangTurkish, "tr" },
		{ ELangIcelandic, "is" },
		{ ELangRussian, "ru" },
		{ ELangHungarian, "hu" },
		{ ELangDutch, "nl" },
		{ ELangBelgianFlemish, "bl" },
		{ ELangAustralian, "au" },
		{ ELangBelgianFrench, "bf" },
		{ ELangAustrian, "as" },
		{ ELangNewZealand, "nz" },
		{ ELangInternationalFrench, "if" },
		{ ELangCzech, "cs" },
		{ ELangSlovak, "sk" },
		{ ELangPolish, "pl" },
		{ ELangSlovenian, "sl" },
		{ ELangTaiwanChinese, "tc" },
		{ ELangHongKongChinese, "hk" },
		{ ELangPrcChinese, "zh" },
		{ ELangJapanese, "ja" },
		{ ELangThai, "th" },
		{ ELangAfrikaans, "af" },
		{ ELangAlbanian, "sq" },
		{ ELangAmharic, "ah" },
		{ ELangArabic, "ar" },
		{ ELangArmenian, "hy" },
		{ ELangTagalog, "tl" },
		{ ELangBelarussian, "be" },
		{ ELangBengali, "bn" },
		{ ELangBulgarian, "bg" },
		{ ELangBurmese, "my" },
		{ ELangCatalan, "ca" },
		{ ELangCroatian, "hr" },
		{ ELangCanadianEnglish, "ce" },
		{ ELangInternationalEnglish, "ie" },
		{ ELangSouthAfricanEnglish, "sa" },
		{ ELangEstonian, "et" },
		{ ELangFarsi, "fa" },
		{ ELangCanadianFrench, "cf" },
		{ ELangScotsGaelic, "gd" },
		{ ELangGeorgian, "ka" },
		{ ELangGreek, "el" },
		{ ELangCyprusGreek, "gr" },
		{ ELangGujarati, "gu" },
		{ ELangHebrew, "he" },
		{ ELangHindi, "hi" },
		{ ELangIndonesian, "id" },
		{ ELangIrish, "ga" },
		{ ELangSwissItalian, "sz" },
		{ ELangKannada, "kn" },
		{ ELangKazakh, "kk" },
		{ ELangKhmer, "km" },
		{ ELangKorean, "ko" },
		{ ELangLao, "lo" },
		{ ELangLatvian, "lv" },
		{ ELangLithuanian, "lt" },
		{ ELangMacedonian, "mk" },
		{ ELangMalay, "ms" },
		{ ELangMalayalam, "ml" },
		{ ELangMarathi, "mr" },
		{ ELangMoldavian, "mo" },
		{ ELangMongolian, "mn" },
		{ ELangNorwegianNynorsk, "nn" },
		{ ELangBrazilianPortuguese, "bp" },
		{ ELangPunjabi, "pa" },
		{ ELangRomanian, "ro" },
		{ ELangSerbian, "sr" },
		{ ELangSinhalese, "si" },
		{ ELangSomali, "so" },
		{ ELangInternationalSpanish, "os" },
		{ ELangLatinAmericanSpanish, "ls" },
		{ ELangSwahili, "sw" },
		{ ELangFinlandSwedish, "fs" },
		{ ELangTamil, "ta" },
		{ ELangTelugu, "te" },
		{ ELangTibetan, "bo" },
		{ ELangTigrinya, "ti" },
		{ ELangCyprusTurkish, "ct" },
		{ ELangTurkmen, "tk" },
		{ ELangUkrainian, "uk" },
		{ ELangUrdu, "ur" },
		{ ELangVietnamese, "vi" },
		{ ELangWelsh, "cy" },
		{ ELangZulu, "zu" }
	};    

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvUtil::LongPrintL()
// -----------------------------------------------------------------------------
// 		        
#if IPTV_LOGGING_METHOD != 0
EXPORT_C void CIptvUtil::LongPrintL( TDesC& aData )
    {
    const TInt KMaxLength = 80;
    TInt length = aData.Length();
                
    if ( length > KMaxLength )
        {                
        for ( TInt i = 0; i <= length; i += KMaxLength )
            {
            HBufC* temp = HBufC::NewLC( KMaxLength );
            TInt counter = length - i;
            if ( counter < KMaxLength )
                {
                temp->Des().Copy( aData.Mid( i, counter ) );
                }
            else
                {                                                    
                temp->Des().Copy( aData.Mid( i, KMaxLength ) );
                }
                    
            IPTVLOGSTRING2_HIGH_LEVEL("Value = %S", temp);
            CleanupStack::PopAndDestroy( temp );
            }
        }
    else
        {            
        IPTVLOGSTRING2_HIGH_LEVEL("Value = %S", &aData);
        }
    }    
#else
EXPORT_C void CIptvUtil::LongPrintL( TDesC& /*aData*/ )
    {
    }
#endif

// -----------------------------------------------------------------------------
// CIptvUtil::StringToInt()
// -----------------------------------------------------------------------------
// 
EXPORT_C TUint32 CIptvUtil::StringToInt(TDesC& value)
    {
    TUint32 id = 0;
    if (value.Compare(_L("")) != 0)
        {
        TLex l(value);
        l.Val(id, EDecimal);
        }                
    return id;
    }

// -----------------------------------------------------------------------------
// CIptvUtil::GetPathL()
// -----------------------------------------------------------------------------
//   
EXPORT_C void CIptvUtil::GetPathL( RFs& aFs,
                                   TIptvPathSelection aSelection,
                                   TDes& aPath,
                                   TDriveNumber aDrive,
                                   TBool aCreatePath )
    {
	IPTVLOGSTRING_HIGH_LEVEL( "CIptvUtil::GetPathL(1) - Enter" );

	TChar driveChar;
    TUint driveDoubleChar;

	aFs.DriveToChar( aDrive, driveChar );
    driveDoubleChar = driveChar;

    // Most of the data is stored to "private" folder that is hidden so that 
    // harvesters don't find possible thumbnail pictures, etc. in them.
    if ( aSelection != EIptvPathMyVideosVideo )
        {
    	if ( aDrive == EDriveC )
    	   {
    	   aPath.Format( _L("%c%S"), driveDoubleChar, &KIptvRootPathC );
    	   }
    	else
    	   {
    	   aPath.Format( _L("%c%S"), driveDoubleChar, &KIptvRootPathE );
    	   }

        if ( aCreatePath )
            {
        	EnsureAndHidePathL( aFs, aPath, ETrue );
            }
        }
    // Videos and their thumbnails are stored under \My Videos.
    else
        {
        if ( aDrive == EDriveC )
            {
            TFileName cRoot = PathInfo::PhoneMemoryRootPath();
            aPath.Format( _L("%S%S"), &cRoot, &KMyVideosExternalFolderOwn );
            }
    	else
            {
            aPath.Format( _L("%c%S%S"), driveDoubleChar,
                                        &KColonAndSlash,
                                        &KMyVideosExternalFolderOwn );
            }

        if ( aCreatePath )
            {        
            EnsureAndHidePathL( aFs, aPath, EFalse );
            }
        }

	// Add subfolder based on the exact path requested.
    switch ( aSelection )
        {
        case EIptvPathWritableRoot:
            {
            }
        	break;

        case EIptvPathTmp:
            {
        	_LIT( KTempSuffix, "\\tmp" );
            aPath.Append( KTempSuffix );
            }
            break;

        case EIptvPathMyVideosDb:
            {
        	_LIT( KMyVideosDbSuffix, "\\mv" );
            aPath.Append( KMyVideosDbSuffix );
            }
            break;

        case EIptvPathMyVideosVideo:
            {
        	_LIT( KMyVideosVideoSuffix, "\\Downloads" );
            aPath.Append( KMyVideosVideoSuffix );
            }
            break;

        case EIptvPathEcg:
            {
        	_LIT( KContentGuideSuffix, "\\ecg" );
            aPath.Append( KContentGuideSuffix );
            }
            break;

        case EIptvPathService:
            {
        	_LIT( KServicePathSuffix, "\\srv" );
            aPath.Append( KServicePathSuffix );
            }
            break;

        case EIptvPathEcgLive:
            {
        	_LIT( KEcgLiveSuffix, "\\ecg\\live" );
        	aPath.Append( KEcgLiveSuffix );
            }
        	break;

        case EIptvPathTmpLive:
            {
        	_LIT( KTmpLiveSuffix, "\\tmp\\live" );
        	aPath.Append( KTmpLiveSuffix );
            }
        	break;

        default:
            {
		    IPTVLOGSTRING_HIGH_LEVEL("CIptvUtil::GetPathL(1) - Invalid argument!" );            
            User::Leave( KErrPathNotFound );
            }
            break;
        }

    // Add trailing slash.
    aPath.Append( KSlash );
    
    // Ensure / hide the path, depending on exact path.
    if ( aCreatePath )
        {
        if ( aSelection == EIptvPathMyVideosVideo )
            {
            EnsureAndHidePathL( aFs, aPath, EFalse );
            }
        else
            {
            EnsureAndHidePathL( aFs, aPath, ETrue );
            }
        }

    IPTVLOGSTRING2_HIGH_LEVEL( "CIptvUtil::GetPathL(1) - Exit (%S)", &aPath );
    }

// -----------------------------------------------------------------------------
// CIptvUtil::GetPathL()
// -----------------------------------------------------------------------------
//  
EXPORT_C void CIptvUtil::GetPathL( RFs& aFs,
                                   TIptvPathSelection aSelection,
								   TUint32 aServiceId,
								   TDes& aPath,
								   TBool aCreatePath )
	{
	IPTVLOGSTRING_HIGH_LEVEL("CIptvUtil::GetPathL(2) - Enter" );

	switch ( aSelection )
		{
		case EIptvPathEcgLive:
		case EIptvPathTmpLive:
			{
			// First get normal path based on TIptvPathSelection
			GetPathL( aFs, aSelection, aPath, EDriveC, aCreatePath );
			// Append serviceId directory at the end of the path as a directory
			aPath.AppendNum( aServiceId );
			// Finally add '\' at the end of the path
			aPath.Append( KSlash );
			}
			break;

		case EIptvPathRssSearchThumbnail:
		case EIptvPathRssThumbnail:
			{
			// First of all get the ecg path for thumbnails.
			// Thumbnail path is c:\data\videocenter\ecg\<serviceId>\(stbn/tbn)
			GetPathL( aFs, EIptvPathEcg, aPath, EDriveC, aCreatePath );

			// Append serviceid and slash
			aPath.AppendNum( aServiceId );
			aPath.Append( KSlash );

            if ( aCreatePath )
                {
                EnsureAndHidePathL( aFs, aPath, ETrue );
                }

			// Select thumbnail dir based on what client requested
			if ( aSelection == EIptvPathRssThumbnail )
				{
				_LIT( KIptvVodThumbnailDir, "tbn\\" );
				aPath.Append( KIptvVodThumbnailDir );
				}
			else
				{
				_LIT( KSearchThumbnailDir, "stbn\\");
				aPath.Append( KSearchThumbnailDir );
				}

			// Now in aPath we have either
			// \\data\\videocenter\\ecg\\<serviceId>\\tbn\\
			// or
			// \\data\\videocenter\\ecg\\<serviceId>\\stbn\\
			// First one is for vod, second one is for search
			}
			break;

		default:
		    {
		    IPTVLOGSTRING_HIGH_LEVEL("CIptvUtil::GetPathL(2) - Invalid argument!" );
		    User::Leave( KErrPathNotFound );
		    }
			break;
		}

    if ( aCreatePath )
        {
        EnsureAndHidePathL( aFs, aPath, ETrue );
        }

    IPTVLOGSTRING2_HIGH_LEVEL("CIptvUtil::GetPathL(2) - Exit (%S)", &aPath );
	}

// -----------------------------------------------------------------------------
// CIptvUtil::ReadDes
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvUtil::ReadDesFromStreamL(TDes& aDes, RReadStream& aStream)
    {
    TInt i;
    TInt length = aStream.ReadUint16L();
    
    aDes.Zero();
    
    if ( aDes.MaxLength() < length )
        {
        User::Leave( KErrArgument );
        }
    
    for(i = 0; i < length; i++)
        {
        aDes.Append(aStream.ReadUint16L());
        }
    }

// -----------------------------------------------------------------------------
// CIptvUtil::WriteDes
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvUtil::WriteDesToStreamL(const TDesC& aDes, RWriteStream& aStream)
    {
    TInt i;
    TInt length = aDes.Length();
    
    aStream.WriteUint16L(length);
    for(i = 0; i < length; i++)
        {
        aStream.WriteUint16L(aDes[i]);
        }
    }
// -----------------------------------------------------------------------------
// CIptvUtil::ReadDes8
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvUtil::ReadDes8FromStreamL(TDes8& aDes, RReadStream& aStream)
    {
    TInt i;
    TInt length = aStream.ReadUint16L();
    
    aDes.Zero();
    
    if ( aDes.MaxLength() < length )
        {
        User::Leave( KErrArgument );
        }    
    
    for(i = 0; i < length; i++)
        {
        aDes.Append(aStream.ReadUint8L());
        }
    }
    
// -----------------------------------------------------------------------------
// CIptvUtil::WriteDes8
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvUtil::WriteDes8ToStreamL(const TDesC8& aDes, RWriteStream& aStream)
    {
    TInt i;
    TInt length = aDes.Length();
    
    aStream.WriteUint16L(length);
    for(i = 0; i < length; i++)
        {
        aStream.WriteUint8L(aDes[i]);
        }
    }

// -----------------------------------------------------------------------------
// CIptvUtil::ReadUint64FromStream
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvUtil::ReadUint64FromStreamL(TUint64& aData, RReadStream& aStream)
    {
    TUint low  = static_cast<TUint>(aStream.ReadUint32L());
    TUint high = static_cast<TUint>(aStream.ReadUint32L());
    aData  = high;
    aData  = aData << 32;
    aData |= low;
    }

// -----------------------------------------------------------------------------
// CIptvUtil::WriteTUint64ToStreamL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvUtil::WriteUint64ToStreamL(const TUint64 aData, RWriteStream& aStream)
    {
    aStream.WriteUint32L( aData & 0x00000000FFFFFFFFULL);
    aStream.WriteUint32L( (aData & 0xFFFFFFFF00000000ULL) >> 32);
    }

// -----------------------------------------------------------------------------
// CIptvUtil::ReadHBufCFromStreamL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CIptvUtil::ReadHBufCFromStreamL( RReadStream& aStream )
    {
    TUint32 length  = aStream.ReadUint32L();

    HBufC* data = HBufC::NewL( length );
    CleanupStack::PushL( data ); // 1->
    
    TPtr dataPtr( data->Des() );
    
    for ( TInt i = 0; i < length; i++ )
        {
        dataPtr.Append( aStream.ReadUint16L() );
        }

    CleanupStack::Pop( data ); // <-1
    return data;
    }

// -----------------------------------------------------------------------------
// CIptvUtil::WriteHBufCToStreamL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvUtil::WriteHBufCToStreamL( HBufC* aData, RWriteStream& aStream )
    {
    aStream.WriteUint32L( aData->Length() );
    
    TPtrC dataPtr( aData->Des() );
    
    for ( TInt i = 0; i < aData->Length(); i++ )
        {
        aStream.WriteUint16L( dataPtr[i] );
        }
    }

// -----------------------------------------------------------------------------
// CIptvUtil::GetLanguageByCountryCode
// -----------------------------------------------------------------------------
//
EXPORT_C TLanguage CIptvUtil::GetLanguageByCountryCode( const TDesC& aLanguageCode )
	{
    IPTVLOGSTRING_LOW_LEVEL("CIptvUtil::GetLanguageByCountryCode");

	TLanguage language = ELangNone;
	for( TInt i = 0; i < KNumberOfLanguages; i++ )
		{
		SLanguageItem item = languageArray[i];
		TBuf8<10> buf;
		buf.Copy( (unsigned char*) item.iLanguageCode );
		TBuf16<10> buf2;
		buf2.Copy( buf );
		if( buf2.Compare( aLanguageCode ) == 0 )
			{
			language = item.iLanguage;
			i = KNumberOfLanguages;
			}
		}

	IPTVLOGSTRING2_LOW_LEVEL("CIptvUtil::GetLanguageByCountryCode Language to be used = %d (65535 = ELangNone)", language);
	return language;
	}

// -----------------------------------------------------------------------------
// Runs recognizers on the given file to determine its mime type
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvUtil::RecognizeFile(
    RFs& aFsSession,
    const TDesC& aFilePath, 
    TDataType& aDataType )
    {
    TBuf8<200> buffer; // Enough file content to recognize the type
    RFile file;
    TInt err = file.Open( aFsSession, aFilePath, EFileRead | EFileShareAny );
    if ( err == KErrNone )
        {
        err = file.Read( buffer );
        file.Close();

        if ( err == KErrNone )
            {
            RApaLsSession apaSession;
            err = apaSession.Connect();
            if ( err == KErrNone )
                {
                TDataRecognitionResult result;
                err = apaSession.RecognizeData( aFilePath, buffer, result );
                if ( result.iConfidence >= CApaDataRecognizerType::EProbable )
                    {
                    aDataType = result.iDataType;
                    }

                apaSession.Close();
                }
            }
        }

    return err;
    }

// -----------------------------------------------------------------------------
// Runs recognizers on the given file to determine its mime type
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvUtil::RecognizeFile(
    const RFile& aFileHandle,
    TDataType& aDataType )
    {
#ifdef __S60_30__

    IPTVLOGSTRING_HIGH_LEVEL( "CIptvUtil::RecognizeFile() returns KErrNotSupported" );
    ( void )aFileHandle;
    return KErrNotSupported;
    
#else // __S60_30__

    RApaLsSession apaSession;
    TInt err( apaSession.Connect() );
    if ( err == KErrNone )
        {
        TDataRecognitionResult result;
        err = apaSession.RecognizeData( const_cast<RFile&>( aFileHandle ), result );         
        if ( result.iConfidence >= CApaDataRecognizerType::EProbable )
            {
            aDataType = result.iDataType;
            }

        apaSession.Close();
        }

    return err;

#endif // __S60_30__
    }
    
// -----------------------------------------------------------------------------
// CIptvUtil::RemoveTrailingBackSlashesL()
// -----------------------------------------------------------------------------
//
void CIptvUtil::RemoveTrailingBackSlashesL( HBufC* & aString )
    {
    while ( aString->LocateReverse( '\\' ) == ( aString->Length() - 1 ) )
        {
        aString->Des().SetLength( aString->Length() - 1 );
        }
    }

// -----------------------------------------------------------------------------
// CIptvUtil::ConnectionTypeL
// -----------------------------------------------------------------------------
//    
EXPORT_C CIptvUtil::TConnectionType CIptvUtil::ConnectionTypeL(TUint32 aIapId)
    {
    TBool isHiddenWlanIap;
    return ConnectionTypeL(aIapId, isHiddenWlanIap);
    }

// -----------------------------------------------------------------------------
// CIptvUtil::ConnectionTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvUtil::TConnectionType CIptvUtil::ConnectionTypeL( TUint32 aIapId,
                                                     TBool& aIsHiddenWlanIap )
    {
    TConnectionType connectionType = EUnknown;

    CCommsDatabase* commsDbIap = CCommsDatabase::NewL();
    CleanupStack::PushL(commsDbIap);                    // 1->
    CCommsDbTableView* iapTableView;
    iapTableView = commsDbIap->OpenTableLC(TPtrC(IAP)); // 2->
    
    //Search correct iap and pick connection type
    TInt err = iapTableView->GotoFirstRecord();
    while(err == KErrNone)
        {
        TUint32 iapId;
        iapTableView->ReadUintL(TPtrC(COMMDB_ID), iapId);

        if ( aIapId == iapId )
            {
            TBuf<KIptvUtilMaxTextFieldLengthInDb> colText;
            TUint32 serviceId;
                    
            iapTableView->ReadUintL(TPtrC(IAP_SERVICE), serviceId);
            iapTableView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), colText); 

            if ( colText == KIptvUtilLanService )
                {
                //was lan service
                if ( IsWlanIapL( serviceId, aIsHiddenWlanIap ) )
                    {
                    //was wlan iap
                    connectionType = EWlan;
                    }
                }           
            else if ( colText == KIptvUtilOutGoingGprs )
                {
                connectionType = EGprs;
                }
            else if( colText == KIptvUtilDialOutISP )
                {
                connectionType = ECsd;
                }
            break;
            }
        err = iapTableView->GotoNextRecord();
        }

    CleanupStack::PopAndDestroy(iapTableView); //<-2
    CleanupStack::PopAndDestroy(commsDbIap);   //<-1
   
    return connectionType;
    }

// -----------------------------------------------------------------------------
// CIptvUtil::IsWlanIapL
// -----------------------------------------------------------------------------
//    
#ifndef __WINSCW__

EXPORT_C TBool CIptvUtil::IsWlanIapL(TUint32 aIapService)
    {
    TBool isHiddenWlanIap;
    return IsWlanIapL( aIapService, isHiddenWlanIap );
    }
    
#else

EXPORT_C TBool CIptvUtil::IsWlanIapL(TUint32 /*aIapService*/)
    {
    return EFalse;
    }

#endif

// -----------------------------------------------------------------------------
// CIptvUtil::IsWlanIapL
// -----------------------------------------------------------------------------
//
#ifdef __WINSCW__

EXPORT_C TBool CIptvUtil::IsWlanIapL( TUint32 /*aIapService*/, TBool& /*aIsHiddenWlanIap*/)
    {
    return EFalse;
    }

#else
    
EXPORT_C TBool CIptvUtil::IsWlanIapL( TUint32 aIapService, TBool& aIsHiddenWlanIap )
    {
    TBool retval;
    
    CCommsDbTableView* wlantable = NULL;
    CCommsDatabase* commsDbWlan = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    CleanupStack::PushL( commsDbWlan ); // 1->
    wlantable = commsDbWlan->OpenViewMatchingUintLC( TPtrC(WLAN_SERVICE),
                                                     TPtrC(WLAN_SERVICE_ID),
                                                     aIapService ); // 2->
                                             
    TInt err = wlantable->GotoFirstRecord();
    if ( err == KErrNotFound )
        {
        //is not a wlan iap
        retval = EFalse;
        }
    else
        {
        User::LeaveIfError( err );
        //is a wlan iap
        TUint32 scanSsid = 0;
        wlantable->ReadUintL( TPtrC(WLAN_SCAN_SSID), scanSsid );
        aIsHiddenWlanIap = static_cast<TBool>( scanSsid );
        retval = ETrue;
        }

    CleanupStack::PopAndDestroy( wlantable );   // <-2
    CleanupStack::PopAndDestroy( commsDbWlan ); // <-1
    
    return retval;
    }
#endif

// -----------------------------------------------------------------------------
// CIptvUtil::GetAllowedIapsListL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvUtil::GetAllowedIapsListL( RArray<TUint32>& aAllowedIaps )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvUtil::GetAllowedIapsListL() - Enter" );

    CCommsDatabase* database = NULL;
    CCommsDbTableView* view = NULL;
    TInt result = KErrNotFound;
    TUint32 iapId = 0;

    aAllowedIaps.Reset();

    database = CCommsDatabase::NewL();
    CleanupStack::PushL( database );

// For ARMV5 we do the check as we should, by taking IAPs from WAP_ACCESS_POINT table.
#if !defined(__WINS__) && !defined(__WINSCW__)

    TUint32 wapId = 0;    
    CCommsDbTableView* wapbearer = NULL;

    view = database->OpenTableLC( TPtrC( WAP_ACCESS_POINT ) );
    result = view->GotoFirstRecord();

    while ( result == KErrNone )
        {
        view->ReadUintL( TPtrC( COMMDB_ID ), wapId );

        wapbearer = database->OpenViewMatchingUintLC( TPtrC( WAP_IP_BEARER ),
                                                      TPtrC( WAP_ACCESS_POINT_ID ),
                                                      wapId );
        wapbearer->GotoFirstRecord();
        wapbearer->ReadUintL( TPtrC( WAP_IAP ), iapId );

#ifdef _DEBUG

        TBuf<40> name;
        wapbearer->ReadTextL( TPtrC( COMMDB_NAME ), name );
        IPTVLOGSTRING3_LOW_LEVEL(
            "CIptvUtil::GetAllowedIapsListL(ARMV5) adding IAP ID %d (%S).",
            iapId,
            &name );

#endif // _DEBUG        

        CleanupStack::PopAndDestroy( wapbearer );
        aAllowedIaps.AppendL( iapId );
        result = view->GotoNextRecord();
        }

// For WINSCW we read IAP table as WAP_ACCESS_POINT table is not normally configured.
#else

    view = database->OpenTableLC( TPtrC( IAP ) );
    result = view->GotoFirstRecord();

    while ( result == KErrNone )
        {
        view->ReadUintL( TPtrC( COMMDB_ID ), iapId );

#ifdef _DEBUG

        TBuf<40> name;
        view->ReadTextL( TPtrC( COMMDB_NAME ), name );
        IPTVLOGSTRING3_LOW_LEVEL(
            "CIptvUtil::GetAllowedIapsListL(WINSCW) adding IAP ID %d (%S).",
            iapId,
            &name );

#endif // _DEBUG     

        aAllowedIaps.AppendL( iapId );
        result = view->GotoNextRecord();
        }

#endif // if !defined(__WINS__) && !defined(__WINSCW__)

    CleanupStack::PopAndDestroy( view );
    CleanupStack::PopAndDestroy( database );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvUtil::GetAllowedIapsListL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CIptvUtil::IsAllowedIapL
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool CIptvUtil::IsAllowedIapL( TUint32 aIapId, RArray<TUint32>& aAllowedIaps )
    {
    TBool allowed = EFalse;
    
    for ( TInt i = 0; i < aAllowedIaps.Count(); i++ )
        {
        if ( aIapId == aAllowedIaps[i] )
            {
            allowed = ETrue;
            break;
            }
        }

    IPTVLOGSTRING3_LOW_LEVEL( "CIptvUtil::IsAllowedIapL() returning %d for IAP %d",
        static_cast<TInt>( allowed ),
        aIapId );

    return allowed;
    }

// -----------------------------------------------------------------------------
// CIptvUtil::EnsureAndHidePathL
// -----------------------------------------------------------------------------
//   
EXPORT_C void CIptvUtil::EnsureAndHidePathL( RFs& aFsSession,
                                             const TDesC& aPath,
                                             TBool aHide )
    {
    if ( aPath.Length() == 0 )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CIptvUtil::EnsureAndHidePathL() empty path supplied!" );
        User::Leave( KErrArgument );
        }

    // Ensure that the path exists.
    TRAPD( baflError, BaflUtils::EnsurePathExistsL( aFsSession, aPath ) );

    if ( baflError != KErrNone && 
         ( aPath[0] == 'c' || aPath[0] == 'C' ) )
         {
         IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvUtil::EnsureAndHidePathL() problem %d with C-drive!", baflError );
         User::Leave( baflError );
         }

    // Hide the path if requested.
    if ( aHide && baflError == KErrNone )
        {
        TUint attValue = 0;
        if ( aFsSession.Att( aPath, attValue ) == KErrNone )
            {
            if ( ! ( attValue & KEntryAttHidden ) )
                {
                User::LeaveIfError( aFsSession.SetAtt( aPath, KEntryAttHidden, 0 ) );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CIptvUtil::LastPlaybackPositionFeatureSupported
// ----------------------------------------------------------------------------
//    
EXPORT_C TBool CIptvUtil::LastPlaybackPositionFeatureSupported()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvUtil::LastPlaybackPositionFeatureSupported() - Enter" );

	TBool isSupported( ETrue );
    
    CRepository* cenRep( NULL );
    TRAP_IGNORE( cenRep = CRepository::NewL( KIptvCenRepUid ) );
    
    if ( cenRep )
        {
        cenRep->Get( KIptvCenRepLastPlaybackPositionFeatureKey, isSupported );    
        delete cenRep;
        }
	IPTVLOGSTRING2_LOW_LEVEL( 
       "CIptvUtil::LastPlaybackPositionFeatureSupported() - Exit, return value = %b", isSupported );	
    
	return isSupported;
    }
