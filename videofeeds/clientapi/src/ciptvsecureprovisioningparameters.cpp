/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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





// INCLUDE FILES
#include "IptvDebug.h"
#include <sysutil.h>

#include <3des.h>               // For C3DES
#include <centralrepository.h>  // link against centralrepository.lib
#include <e32math.h>
#include <mmtsy_names.h>

#include "ciptvsecureprovisioningparameters.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TUid    KCRUidPnpUtil   = {0x1020433A};
const TInt KMaxKeyInfoLength  = 20;
const TInt KNonceLength       = 8;
const TUint32 KPnPUtilNonce   = 0x00000002;
const TUint32 KPnPUtilHomeMcc = 0x00000004;
const TUint32 KPnPUtilHomeMnc = 0x00000005;
const TUint32 KPnPUtilKeyInfo = 0x00000008;
_LIT8( K3DesKey, "111111111111111111111111" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvSecureProvisioningParameters::ConstructL()
    {
    User::LeaveIfError( iServer.Connect() );
    User::LeaveIfError( iServer.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( iPhone.Open( iServer, KMmTsyPhoneName ) );
    User::LeaveIfError( iPhone.Initialise() );

    iRepository = CRepository::NewL( KCRUidPnpUtil );

#ifndef __WINS__ // calling C3DESEncryptor::NewL crashes emulator
    iEncryptor = C3DESEncryptor::NewL( K3DesKey );
#endif 
    }

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvSecureProvisioningParameters* CIptvSecureProvisioningParameters::NewL()
    {
    CIptvSecureProvisioningParameters* self = new( ELeave ) CIptvSecureProvisioningParameters;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
CIptvSecureProvisioningParameters::~CIptvSecureProvisioningParameters()
    {
    delete iRepository;
#ifndef __WINS__
    delete iEncryptor;
#endif
    iPhone.Close();
    iServer.Close();
    }

// -----------------------------------------------------------------------------    
// CIptvSecureProvisioningParameters::SecureProvisioningUrlParametersL()
// -----------------------------------------------------------------------------
//
HBufC* CIptvSecureProvisioningParameters::SecureProvisioningUrlParametersL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvService::SecureProvisioningUrlParametersL() start");
    
    // home country code and home network id

    FetchHomeNetworkInfoL();

    RMobilePhone::TMobilePhoneNetworkCountryCode homeMcc = HomeMccL();
    RMobilePhone::TMobilePhoneNetworkIdentity homeMnc    = HomeMncL();
    
    const TInt HomeCountryCodeLength = 4;
    const TInt HomeNetworkCodeLength = 8;
    TBuf<HomeCountryCodeLength> hmcc16;
    TBuf<HomeNetworkCodeLength> hmnc16;
    hmcc16.Copy( homeMcc );
    hmnc16.Copy( homeMnc );

    // keyinfo, nonce
    TBuf8<KMaxKeyInfoLength> keyInfo;
    TBuf<KMaxKeyInfoLength> keyInfo16;
    GetKeyInfoL( keyInfo );
    keyInfo16.Copy( keyInfo );
    TBuf8<KNonceLength> nonce;
    TBuf<KNonceLength> nonce16;
    TInt timeout( 0 );
    CreateNewNonceL( timeout, nonce );
    nonce16.Copy( nonce );
    
    // product model
    TBuf8<KSysUtilVersionTextLength> deviceinfo;
    TBuf<KSysUtilVersionTextLength> deviceinfo16;
    FetchProductModelL( deviceinfo );
    deviceinfo16.Copy( deviceinfo );

    // 1. Keyinfo
    // 2. Nonce
    // 3. Home MCC
    // 4. Home MNC
    // 5. Network (current) MCC
    // 6. Network MNC
    // 7. Version
    // 8. deviceinfo

    _LIT( KVersionDes, "NPnP-MSS60-1.01" );
    const TInt KVersionDesLength = 16; 

    TBuf<KVersionDesLength> version16;
    version16    = KVersionDes;

    // These are for the test service, comments kept here until we get the real thing up.
    //version16    = _L("email-0.1");
    //deviceinfo16 = _L("n80");
    
    _LIT(KParameterFormatDes,
        "?sType=4&Key=%S&Nonce=%S&MCC=%S&MNC=%S&cVersion=%S&deviceinfo=%S&VCSW=1&action=activate");
    const TInt KParameterFormatDesLength = 150;
    
    HBufC* parameters = HBufC::NewL(
        KParameterFormatDesLength +
        keyInfo16.Length() +
        nonce16.Length() +
        hmcc16.Length() +
        hmnc16.Length() +
        KVersionDesLength +
        deviceinfo16.Length() );
    
    TPtr parametersPtr( parameters->Des() );

    parametersPtr.Format(
        KParameterFormatDes,
        &keyInfo16,
        &nonce16,
        &hmcc16,
        &hmnc16,
        &version16,
        &deviceinfo16);
    
    IPTVLOGSTRING2_HIGH_LEVEL( "%S", &parametersPtr );
            
    IPTVLOGSTRING_LOW_LEVEL("CIptvService::SecureProvisioningUrlParametersL() exit");
    return parameters;
    }

// -----------------------------------------------------------------------------    
// CIptvSecureProvisioningParameters::FetchProductModelL()
// -----------------------------------------------------------------------------
//
void CIptvSecureProvisioningParameters::FetchProductModelL( TDes8& aModel )
    {
    HBufC* tmpVersion = HBufC::NewLC( KSysUtilVersionTextLength );
    TPtr ptr( tmpVersion->Des() );
    User::LeaveIfError( SysUtil::GetSWVersion( ptr ) );

    _LIT(KVerStrStart,"V ");
    _LIT(KVerStrEnd,"\n");

    TInt pos1 = tmpVersion->Find(KVerStrStart);
    TInt pos2 = tmpVersion->Find(KVerStrEnd);
    TInt verlen = ((TDesC)(KVerStrStart)).Length();
    
    // Example version string:
    // V 10.0.053
    // 21-09-07
    // RM-179
    // (c) Nokia N81 (92.03)
    
    // RM-179 is extracted from it
    
    if( pos1==KErrNotFound) // Version does not start with "V "
        {
        pos1=0;
        verlen=0;
        }

    if(    (pos1!=KErrNotFound) 
        && (pos2!=KErrNotFound) 
        && (pos2 > (pos1 + verlen) ) 
      )
        {
        TPtrC ptrSeek(ptr);
        pos1 = ptrSeek.Find(KVerStrEnd);
        if(pos1>=0)
            {
            ptrSeek.Set(ptrSeek.Mid(pos1+1)); //removes "V 10.0.053"
            pos1 = ptrSeek.Find(KVerStrEnd);
            if( pos1 >= 0 )
                {
                ptrSeek.Set(ptrSeek.Mid(pos1+1)); // removes "21-09-07"
                pos1 = ptrSeek.Find(KVerStrEnd);
                if( pos1 < 0 )
                    {
                    ptrSeek.Set(ptrSeek.Mid(1));
                    aModel.Copy(ptrSeek);
                    }
                else if( pos1 > 0 )
                    {
                    ptrSeek.Set(ptrSeek.Mid(1,pos1-1));
                    aModel.Copy(ptrSeek);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::FetchHomeNetworkInfoL
// -----------------------------------------------------------------------------
//
void CIptvSecureProvisioningParameters::FetchHomeNetworkInfoL()
    {
    // Get home (sim) MCC&MNC info
    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );
#ifndef __WINS__
    TRequestStatus status( KRequestPending );
    iPhone.GetHomeNetwork( status, infoPckg );
    User::WaitForRequest( status );
    User::LeaveIfError( status.Int() );
#else
    info.iCountryCode.Copy( _L("244") );
    info.iNetworkId.Copy( _L("05") );
#endif

    RMobilePhone::TMobilePhoneNetworkIdentity formattedMnc;
    FormatMncCodeL( info.iCountryCode, info.iNetworkId, formattedMnc );
    SetHomeMncL( formattedMnc );
    SetHomeMccL( info.iCountryCode );
    }

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::FormatMncCodeL
// -----------------------------------------------------------------------------
//
void CIptvSecureProvisioningParameters::FormatMncCodeL(
    const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc,
    const RMobilePhone::TMobilePhoneNetworkIdentity aUnformattedMnc,
    RMobilePhone::TMobilePhoneNetworkIdentity& aFormattedMnc )
    {
    RMobilePhone::TMobilePhoneNetworkIdentity formattedMnc;

    // Check that codes only contain valid characters
    TInt mncValue(0);
    TInt mccValue(0);

    TLex mncParser( aUnformattedMnc );
    TInt err = mncParser.Val( mncValue );
    if( err != KErrNone )
        {
        User::Leave( KErrCorrupt );
        }

    TLex mccParser( aMcc );
    err = mccParser.Val( mccValue );
    if( err != KErrNone )
        {
        User::Leave( KErrCorrupt );
        }

    // In N7610 the MNC given by RMobilePhone::GetHomeNetwork is
    // always three digits
    // But for example in N6630 the MNC seems to be correctly formatted

    // The initial value is the given MNC:
    formattedMnc.Copy( aUnformattedMnc );

    // Reformat only if needed
    if( aUnformattedMnc.Length() == 3 )
        {
        // Assume two digits, exceptions follow
        formattedMnc.Copy( aUnformattedMnc.Left(2) );

        if( aMcc.Compare( _L("302") ) == 0 ||
            aMcc.Compare( _L("346") ) == 0 ||
            aMcc.Compare( _L("348") ) == 0 ||
            aMcc.Compare( _L("356") ) == 0 ||
            aMcc.Compare( _L("365") ) == 0 ||
            aMcc.Compare( _L("376") ) == 0 ||
            aMcc.Compare( _L("467") ) == 0 ||
            aMcc.Compare( _L("732") ) == 0 )
            {
            // MNC should be three digits
            formattedMnc.Copy( aUnformattedMnc );
            }
        else if( aMcc.Compare( _L("310") ) == 0 )
            {
            // MNC is always three digits in this case
            // For example: 000, 011, 110, 020, 200, ...
            formattedMnc.Copy( aUnformattedMnc );
            }
        else if( aMcc.Compare( _L("311") ) == 0 )
            {
            // 3 digit (USA) some exceptions

            // See previous case for comments
            formattedMnc.Copy( aUnformattedMnc );
            }

        else if( aMcc.Compare( _L("338") ) == 0 )
            {
            // 2 digit, but 3 digit if 180 (JAM)
            if( aUnformattedMnc.Compare( _L("180") ) == 0 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("342") ) == 0 )
            {
            // 2 digit, but 3 digit if larger than 510
            if( mncValue >= 510 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("344") ) == 0 )
            {
            // 2 digit, but 3 digit if larger than 310
            if( mncValue >= 310 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("352") ) == 0 )
            {
            // 2 digit, but 3 digit if smaller than 300
            if( mncValue < 300 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("358") ) == 0 )
            {
            // 2 digit, but 3 digit if smaller than 300
            if( mncValue < 300 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("360") ) == 0 )
            {
            // 2 digit, but 3 digit if 110
            if( aUnformattedMnc.Compare( _L("110") ) == 0 )
                {
                formattedMnc.Copy( aUnformattedMnc  );
                }
            }
        else if( aMcc.Compare( _L("362") ) == 0 )
            {
            // 2 digit, but 3 digit if larger than 920
            if( mncValue >= 920 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("366") ) == 0 )
            {
            // 2 digit, but 3 digit if 110
            if( aUnformattedMnc.Compare( _L("110") ) == 0 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        else if( aMcc.Compare( _L("722") ) == 0 )
            {
            // 2 digit, but 3 digit if 310
            if( aUnformattedMnc.Compare( _L("310") ) == 0 )
                {
                formattedMnc.Copy( aUnformattedMnc );
                }
            }
        }
    aFormattedMnc.Copy( formattedMnc );
    }

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::HomeMccL
// -----------------------------------------------------------------------------
//
const RMobilePhone::TMobilePhoneNetworkCountryCode CIptvSecureProvisioningParameters::HomeMccL()
    {
    RMobilePhone::TMobilePhoneNetworkIdentity mcc;
    User::LeaveIfError( iRepository->Get( KPnPUtilHomeMcc, mcc ) );

    return mcc;
    }

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::SetHomeMncL
// -----------------------------------------------------------------------------
//
void CIptvSecureProvisioningParameters::SetHomeMncL( const RMobilePhone::TMobilePhoneNetworkIdentity aMnc )
    {
    User::LeaveIfError( iRepository->Set( KPnPUtilHomeMnc, aMnc ) );
    }

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::HomeMncL
// -----------------------------------------------------------------------------
//
const RMobilePhone::TMobilePhoneNetworkIdentity CIptvSecureProvisioningParameters::HomeMncL()
    {
    RMobilePhone::TMobilePhoneNetworkIdentity mnc;
    User::LeaveIfError( iRepository->Get( KPnPUtilHomeMnc, mnc ) );
    return mnc;    
    }
    
// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::SetHomeMccL
// -----------------------------------------------------------------------------
//
void CIptvSecureProvisioningParameters::SetHomeMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc )
    {
    User::LeaveIfError( iRepository->Set( KPnPUtilHomeMcc, aMcc ) );
    }

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::GetKeyInfoL
// -----------------------------------------------------------------------------
//
void CIptvSecureProvisioningParameters::GetKeyInfoL( TDes8& aKeyInfo )
    {
    if( aKeyInfo.MaxLength() < KMaxKeyInfoLength )
        {
        User::Leave( KErrArgument );
        }
    // read keyinfo from cenrep
    TBuf<KMaxKeyInfoLength> buffer;
    User::LeaveIfError( iRepository->Get( KPnPUtilKeyInfo, buffer) );
    aKeyInfo.Copy( buffer );
    }

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::CreateNewNonceL
// -----------------------------------------------------------------------------
//
void CIptvSecureProvisioningParameters::CreateNewNonceL( const TUint /*aTimeOut*/, TDes8& aNonce )
    {
    if( aNonce.MaxLength() < KNonceLength )
        {
        User::Leave( KErrArgument );
        }

    TBuf8<KNonceLength> buffer;

    TTime time;
    time.HomeTime();
    TInt64 seed = time.Int64();
    for( TInt i(0); i < KNonceLength; i++ )
        {
        TChar character( RandomCharacter( seed ) );
        buffer.Append( character );
        }

    aNonce.Copy( buffer );

    // Encrypt
#ifndef __WINS__
    iEncryptor->Transform( buffer );
#endif

    // Save the nonce
    User::LeaveIfError( iRepository->Set( KPnPUtilNonce, buffer ) );
    }

// -----------------------------------------------------------------------------
// CIptvSecureProvisioningParameters::RandomCharacter
// -----------------------------------------------------------------------------
//
TUint CIptvSecureProvisioningParameters::RandomCharacter( TInt64& aSeed )
    {
    // Create random number value for token
    TInt value = Math::Rand( aSeed );
    if( value < 0 )
        {
        value = -value;
        }
    value %= 61; // [0,61]
    value += 48; // [48,109]
    // split the range to ranges [48,57],[65,90] and [97,122]
    // that is: the ascii codes for numbers from 0 to 9,
    // capital alphabets from A to Z and alphabets from a to z
    if( value > 57 )
        {
        value += 8; // [65,90]
        if( value > 90 )
            {
            value += 6; // [97,122]
            }
        }
    return value;
    }
