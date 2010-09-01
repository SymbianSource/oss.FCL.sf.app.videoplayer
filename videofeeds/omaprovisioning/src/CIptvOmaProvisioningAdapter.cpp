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
* Description:    Receives and stores the IPTV settings.*
*/




#include <e32base.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include "IptvDebug.h"
#include "IptvEngineUids.h"
#include "CIptvUtil.h"
#include <StringLoader.h>
#include <IptvOmaProvisioningAdapterRes.rsg>
#include <bautils.h>
#include <f32file.h>
#include "iptvlocalisationliterals.h"
#include <bautils.h>
#include "CIptvResourceLoader.h"

#include "CIptvService.h"
#include "CIptvServices.h"
#include "CIptvServiceManagementClient.h"
#include "CIptvOmaProvisioningAdapter.h"
#include "IptvOmaProvisioningAdapterUIDs.h"

// Type
_LIT(KIptvVodCast, "VC");
_LIT(KIptvLiveTv, "LiveTV");
_LIT(KIptvVod, "VOD");
_LIT(KIptvBrowser, "Browser");
_LIT(KIptvApplication, "Application");
_LIT(KIptvMobileTv, "MobileTv");
_LIT(KIptvSecureProvisioningServer, "SecureProvisioningServer");
_LIT(KIptvVideoStore, "VideoStore");

// Protocol
_LIT(KIptvProtocolRss, "RSS2.0");
_LIT(KIptvProtocolDvbIpi, "DVB-IPI");
_LIT(KIptvProtocolHTTPProp, "HTTP-PROP");   // Application and plugin specific communication mechanism
_LIT(KIptvProtocolHTTP, "HTTP");            // Browser
_LIT(KIptvProtocolXMLTV, "XMLTV" );

_LIT(KIptvAppId, "w9031");

// Parameter names
_LIT(KIptvParamNameApplicationMaxBandWidth, "APPLICATION/MAX-BANDWIDTH");
_LIT(KIptvParamNameApplicationNetInfo, "APPLICATION/NETINFO");
_LIT(KIptvParamNameApplicationMinUdpPort, "APPLICATION/MIN-UDP-PORT");
_LIT(KIptvParamNameApplicationInfraId, "APPLICATION/INFRA-ID");
_LIT(KIptvParamNameApplicationTypeId, "APPLICATION/TYPE-ID");
_LIT(KIptvParamNameApplicationIconUrl, "APPLICATION/ICON-URL");
_LIT(KIptvParamNameApplicationAccountUrl, "APPLICATION/ACCOUNT-URL");

const TInt KIptvDriveLetterSpace( 2 );

_LIT( KIptvOmaProvisioningAdapterResFile, "\\Resource\\Plugins\\IptvOmaProvisioningAdapterRes.");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CIptvOmaProvisioningAdapter::CIptvOmaProvisioningAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CIptvOmaProvisioningAdapter::CIptvOmaProvisioningAdapter() : CWPAdapter()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::CIptvOmaProvisioningAdapter() start" );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::CIptvOmaProvisioningAdapter() exit" );
    }

// ---------------------------------------------------------
// CIptvOmaProvisioningAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::ConstructL() start" );

    iServices = CIptvServices::NewL();

    CCoeEnv* env = CCoeEnv::Static();
    if( env )
        {
        CIptvResourceLoader* resourceLoader = CIptvResourceLoader::NewL( *env );   
        if ( resourceLoader )
            {
            CleanupStack::PushL( resourceLoader );
            resourceLoader->AddResourceL( KIptvOmaProvisioningAdapterResFile );
            HBufC* resourceText = StringLoader::LoadLC( R_IPTV_OMA_CLIENT_PROV_SUMMARY_TITLE );
            if ( resourceText->Des().Length() > KIptvOcpMaxSummaryTitleLength )
                {
                iSummaryTitle = resourceText->Des().Left( KIptvOcpMaxSummaryTitleLength - 1 );
                }
            else
                {
                iSummaryTitle = resourceText->Des();
                }
            CleanupStack::PopAndDestroy( resourceText );
			CleanupStack::PopAndDestroy( resourceLoader );
            }
        else
            {
            User::Leave( KErrGeneral );
            }
        } 
    
    User::LeaveIfError( iFs.Connect() );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::ConstructL() exit" );
    }

// ---------------------------------------------------------
// CIptvOmaProvisioningAdapter::NewL
// Static constructor.
// ---------------------------------------------------------
//
CIptvOmaProvisioningAdapter* CIptvOmaProvisioningAdapter::NewL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::NewL() start" );
    CIptvOmaProvisioningAdapter* self = new ( ELeave ) CIptvOmaProvisioningAdapter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::NewL() exit" );
    return self;
    }

// Destructor
CIptvOmaProvisioningAdapter::~CIptvOmaProvisioningAdapter()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::~CIptvOmaProvisioningAdapter() start" );

    delete iCurrentService;
    delete iServices;

    iFs.Close();

    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::~CIptvOmaProvisioningAdapter() exit" );
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::ContextExtension
// -----------------------------------------------------------------------------
//
TInt CIptvOmaProvisioningAdapter::ContextExtension( MWPContextExtension*& aExtension )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::ContextExtension() start" );
    aExtension = this;
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::ContextExtension() exit" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::SaveDataL
// -----------------------------------------------------------------------------
//
const TDesC8& CIptvOmaProvisioningAdapter::SaveDataL( TInt /*aIndex*/ ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::ContextExtension() start" );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::ContextExtension() exit" );
    return KIptvEmptyDes8;
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::DeleteL
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::DeleteL( const TDesC8& /*aSaveData*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::DeleteL() start" );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::DeleteL() exit" );
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::Uid
// -----------------------------------------------------------------------------
//
TUint32 CIptvOmaProvisioningAdapter::Uid() const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::Uid() start" );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::Uid() exit" );
    return PROVISIONING_IPTV_ADAPTER_UID;
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::DetailsL
// -----------------------------------------------------------------------------
//
TInt CIptvOmaProvisioningAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor */ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::DetailsL() start" );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::DetailsL() exit ");
    return KErrNotSupported;
    }  

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::SummaryCount
// -----------------------------------------------------------------------------
//
TInt CIptvOmaProvisioningAdapter::ItemCount() const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::ItemCount() start" );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::ItemCount() exit" );
    
    TInt count;
    if ( iServices )
        {
        count = iServices->Count();
        }
    else
        {
        count = 0;
        }
         
    return count;
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CIptvOmaProvisioningAdapter::SummaryTitle( TInt /*aIndex*/ ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SummaryTitle() start" );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SummaryTitle() exit" );
    return iSummaryTitle;
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC16& CIptvOmaProvisioningAdapter::SummaryText( TInt aIndex ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SummaryText() start" );
    
    if ( !iServices )
        {
        return KIptvEmptyDes;
        }
        
    TInt error;
    CIptvService& service = iServices->Service( aIndex, error );
    
    if ( error == KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: service name found, returning it as a summary text: %S", &(service.GetName()) );
        IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SummaryText() exit" );
        return service.GetName();
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: service name not found, returning empty des" );
        IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SummaryText() exit" );
        return KIptvEmptyDes;
        }
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::SaveL
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::SaveL( TInt /*aIndex*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SaveL() start" );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SaveL() exit" );
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CIptvOmaProvisioningAdapter::CanSetAsDefault( TInt /*aIndex*/ ) const
    {
    // service profile cannot be set as default via OMA CP.
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::CanSetAsDefault() start" );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::CanSetAsDefault() exit" );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWPIptvAdapter::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::SetAsDefaultL( TInt /*aIndex*/ )
    {
    // service profile cannot be set as default via OMA CP.
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SetAsDefaultL() start" );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SetAsDefaultL() exit" );
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::VisitL( CWPCharacteristic& aCharacteristic )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::VisitL(aCharacteristic) start" );

    IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: aCharacteristic.Type() == %d", aCharacteristic.Type() );
        
    switch ( aCharacteristic.Type() )
        {
        case KWPApplication:
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: case KWPApplication" );

            if ( iCurrentService )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: deleting iCurrentService" );
                delete iCurrentService;
                iCurrentService = NULL;
                }

            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: creating iCurrentService" );
            iCurrentService = CIptvService::NewL();
                    
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: starting to fill iCurrentService" );
            
            aCharacteristic.AcceptL( *this );

            IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: iAppId = %S", &iAppId );
            
            if ( iAppId == KIptvAppId ) 
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: was our service settings" );
                
                if ( iServices )
                    {
                    TInt i;
                    TBool serviceWasAlreadyAdded( EFalse );
                    TInt servicesCount = iServices->Count();
                    IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: iServices.Count() = %d", servicesCount );
                    for ( i = 0; ( i < iServices->Count() ) && !serviceWasAlreadyAdded; i++ )
                        {
                        if ( iServices->GetServiceRefL(i).CompareL( *iCurrentService ) )
                            {
                            serviceWasAlreadyAdded = ETrue;
                            }
                        }
                    if ( !serviceWasAlreadyAdded )
                        {
                        IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: adding iCurrentService to iServices" );
                        iServices->AddL( *iCurrentService );
                        IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: iServices.Count() = %d", iServices->Count() );
                        }
                    else
                        {
                        IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: iCurrentService was already in iServices, skipping add" );
                        }
                    }
                else
                    {
                    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: iServices didnt exist, skipping add" );
                    }
                }
            else
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: was not our setting, skipping add" );
                }

            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: deleting iCurrentService" );
            delete iCurrentService;
            iCurrentService = NULL;
            break;
            
        case KWPNamedCharacteristic:
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: case KWPNamedCharacteristic" );
            IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: name = %S", &aCharacteristic.Name() );
            break;
            
        case KWPAppAuth:
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: case KWPAppAuth" );
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: accepting" );
            aCharacteristic.AcceptL( *this );
            break;        
        }

    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::VisitL(aCharacteristic) exit" );
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::VisitL( CWPParameter& aParameter )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::VisitL(aParameter) start" );

    IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter ID    = %d", aParameter.ID() );
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter value = %S", &aParameter.Value() );

    switch ( aParameter.ID() )
        {
        //well known parameters
        case EWPParameterAppID:
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: case EWPParameterAppID" );
            IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: value = %S", &aParameter.Value() );
            iAppId.Set( aParameter.Value() );
            break;
        
        case EWPParameterToProxy:
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter EWPParameterToProxy -> ignoring" );
            break;
            
        case EWPParameterProviderID:
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter EWPParameterProviderID -> collecting" );
            if ( aParameter.Value().Length() > KIptvSmServicesDbProviderIdMaxLength )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: provider ID too long! -> skipping value" );
                }
            else
                {
                iCurrentService->SetProviderId( aParameter.Value() );
                }
            break;

        case EWPParameterName:
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter EWPParameterName -> collecting" );


            if ( aParameter.Value().Length() > KIptvSmServicesDbNameMaxLength )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: name too long! -> skipping value" );
                }
            else
                {
                iCurrentService->SetName( aParameter.Value() );
                }
            break;

        case EWPParameterToNapID:
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter EWPParameterToNapID -> collecting" );
            if ( aParameter.Value().Length() > KIptvSmServicesDbIapNameMaxLength )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: iap name too long! -> skipping value" );                
                }
            else
                {
                iCurrentService->SetIapNameL( aParameter.Value() );
                iCurrentService->SetFlags(
                    iCurrentService->GetFlags() | CIptvService::EReadOnlyIaps );
                }
            break;

        case EWPParameterAddr:
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter  EWPParameterAddr -> collecting" );
            if ( aParameter.Value().Length() > KIptvSmServicesDbAddressMaxLength )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: address too long! -> skipping value" );
                }
            else
                {
                iCurrentService->SetAddress( aParameter.Value() );
                }
            break;
  
        case EWPParameterAAuthName: /* 305 */
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter  EWPParameterAuthName -> collecting" );
            if ( aParameter.Value().Length() > KIptvSmPasswordsDbUserNameMaxLength )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: user name too long! -> skipping value" );
                }
            else
                {
                iCurrentService->SetUserName( aParameter.Value() );
                }
            break;
            
        case EWPParameterAAuthSecret: /* 306 */
            IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter  EWPParameterAuthSecret -> collecting" );
            if ( aParameter.Value().Length() > KIptvSmPasswordsDbPasswordMaxLength )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: password too long! -> skipping value" );
                }
            else
                {
                iCurrentService->SetPassword( aParameter.Value() );
                }
            break;

        //application spesific parameters                    
        case EWPNamedParameter:
            IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter name  = %S", &aParameter.Name() );
            IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter value = %S", &aParameter.Value() );

            if ( aParameter.Name() == KIptvParamNameApplicationMaxBandWidth )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter  APPLICATION/MAX-BANDWIDTH -> ignoring" );
                }
            else if ( aParameter.Name() == KIptvParamNameApplicationNetInfo )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter  APPLICATION/NETINFO -> ignoring" );
                }
            else if ( aParameter.Name() == KIptvParamNameApplicationMinUdpPort )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter APPLICATION/MIN-UDP-PORT -> ignoring" );
                }
            else if ( aParameter.Name() == KIptvParamNameApplicationInfraId )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter APPLICATION/INFRA-ID -> collecting" );
                
                if ( aParameter.Value() == KIptvProtocolRss )
                    {
                    TUid uid;
                    uid.iUid = KIptvRssPluginImplementationUid;
                    iCurrentService->SetEpgPluginUid( uid );
                    uid.iUid = 0;
                    iCurrentService->SetIptvPluginUid( uid );
                    uid.iUid = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;
                    iCurrentService->SetVodPluginUid( uid );
                    }
                else if (aParameter.Value() == KIptvProtocolDvbIpi)
                    {
                    TUid uid;
                    uid.iUid = KIptvSmartvisionPluginUid;
                    iCurrentService->SetEpgPluginUid( uid );  
                    iCurrentService->SetIptvPluginUid( uid );
                    uid.iUid = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;
                    iCurrentService->SetVodPluginUid( uid );
                    }
                else if ( aParameter.Value() == KIptvProtocolHTTPProp )
                    {                    
                    }
                else if ( aParameter.Value() == KIptvProtocolHTTP )
                    {                    
                    }
                else if ( aParameter.Value() == KIptvProtocolXMLTV )
                    {                    
                    TUid uid;
        	          uid.iUid = KIptvLiveTvXmlTvPluginUid;
        	          iCurrentService->SetIptvPluginUid( uid );
                    iCurrentService->SetEpgPluginUid( uid );
        	          uid.iUid = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;
        	          iCurrentService->SetVodPluginUid( uid );
                    }
                else
                    {
                    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: not a valid protocol" );
                    }
                
                }
            else if ( aParameter.Name() == KIptvParamNameApplicationTypeId )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter APPLICATION/TYPE-ID -> collecting" );

                if ( aParameter.Value() == KIptvVodCast )
                    {
                    iCurrentService->SetType( CIptvService::EVodCast );
                    iCurrentService->SetFlags( 0 );
                    }
                else if ( aParameter.Value() == KIptvVod )
                    {
                    iCurrentService->SetType( CIptvService::EVod );
                    iCurrentService->SetFlags( CIptvService::EMainService );
                    }
                else if ( aParameter.Value() == KIptvLiveTv )
                    {
                    iCurrentService->SetType( CIptvService::ELiveTv );
                    iCurrentService->SetFlags( CIptvService::EMainService );
                    }
                else if ( aParameter.Value() == KIptvBrowser )
                    {
                    iCurrentService->SetType( CIptvService::EBrowser );                
                    iCurrentService->SetFlags( CIptvService::EMainService );
                    }
                else if ( aParameter.Value() == KIptvApplication )
                    {
                    iCurrentService->SetType( CIptvService::EApplication );                
                    iCurrentService->SetFlags( CIptvService::EMainService );
                    }
                else if ( aParameter.Value() == KIptvMobileTv )
                    {
                    iCurrentService->SetType( CIptvService::EMobileTv );                
                    iCurrentService->SetFlags( CIptvService::EMainService );
                    }
                else if ( aParameter.Value() == KIptvSecureProvisioningServer )
                    {
                    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: ESecureProvisioningServer type service" );
                    iCurrentService->SetType(
                        CIptvService::ESecureProvisioningServer );
                    iCurrentService->SetFlags( CIptvService::EMainService );
                    }
                else if ( aParameter.Value() == KIptvVideoStore )
                    {
                    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: EVideoStore type service" );
                    iCurrentService->SetType( CIptvService::EVideoStore );                
                    iCurrentService->SetFlags( CIptvService::EMainService );
                    }
                else
                    {
                    // All other services go under other
                    iCurrentService->SetType( CIptvService::EOther );
                    iCurrentService->SetFlags( CIptvService::EMainService );
                    }
                    
                }
            else if ( aParameter.Name() == KIptvParamNameApplicationIconUrl )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter APPLICATION/ICON-URL -> collecting" );
                if ( aParameter.Value().Length() > KIptvSmServicesDbIconPathMaxLength )
                    {
                    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: icon path too long! -> skipping value" );
                    }
                else
                    {
                    SetIconPathL( aParameter.Value() );
                    }
                }
            else if ( aParameter.Name() == KIptvParamNameApplicationAccountUrl )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter APPLICATION/ACCOUNT-URL -> collecting" );
                if ( aParameter.Value().Length() > KIptvSmServicesDbAccountManagementUrlMaxLength )
                    {
                    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: parameter APPLICATION/ACCOUNT-URL too long, skipping" );
                    }
                else
                    {
                    iCurrentService->SetAccountManagementUrl( aParameter.Value() );
                    }
                }
            break;
            
        default:
            break;
        }
        
    
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::VisitL(aParameter) exit" );
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::SettingsSavedL
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::SettingsSavedL ( const TDesC8& aAppIdOfSavingItem,
                                                   const TDesC8& /*aAppRef*/,
                                                   const TDesC8& /*aStorageIdValue*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SettingsSavedL() start" );

    const TInt maxPrintBufSize = 50;
    TBuf<maxPrintBufSize> buf;
    buf.Copy( aAppIdOfSavingItem.Left( maxPrintBufSize - 1 ) );
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: aAppIdOfSavingItem = %S", &buf );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SettingsSavedL() exit" );
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::SavingFinalizedL()
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::SavingFinalizedL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SavingFinalizedL() start" );
    
    if ( iServices )
        {
        if ( iServices->Count() > 0 )
            {
            CIptvServiceManagementClient* smClient = CIptvServiceManagementClient::NewL( *this );
            CleanupStack::PushL( smClient ); // 1->
            CIptvServices* existingServices = CIptvServices::NewL();
            CleanupStack::PushL( existingServices ); // 2->  
            
            TInt i;
            MIptvServiceManagementClientObserver::TRespStatus respStatus(
                MIptvServiceManagementClientObserver::ESucceeded );

            for ( i = 0; i < iServices->Count(); i++ )
                {            
                //check if there is already a service with this provider id
                TBool serviceExists;
                TInt err = smClient->GetServicesL( iServices->GetServiceRefL(i).GetProviderId(),
                                                   *existingServices );
                if ( err == KErrNone )
                    {
                    if ( existingServices->Count() > 0 )
                        {
                        serviceExists = ETrue;
                        }
                    else
                        {
                        serviceExists = EFalse;
                        }
                    }
                else
                    {
                    serviceExists = EFalse;
                    }

                CIptvService& service = iServices->GetServiceRefL( i ); //creates service from binary data and returns ref to it

 
                if ( service.GetType() != CIptvService::ELiveTv )
                    {
 
                    service.SetFlags( service.GetFlags() | CIptvService::ESelected ); //a new call to iServices->GetServiceRefL() will destroy this change, be careful out there...
                            
                    if ( serviceExists )
                        {
                        TUint32 existingServiceId = existingServices->GetServiceRefL( 0 ).GetId();
                        service.SetId( existingServiceId ); //modify the new service instance
                        IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: service (%d) existed, updating old service", existingServiceId );
                        TRAP( err, smClient->UpdateServiceL( service, respStatus ) );
                        }
                    else
                        {                       
                        IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: adding a new service" );
                        TRAP( err, smClient->AddServiceL( service, respStatus ) );
                        }
                    
                    if ( respStatus != MIptvServiceManagementClientObserver::ESucceeded ||
                         err != KErrNone )
                        {
                        IPTVLOGSTRING3_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: adding service to database failed (%d,%d)!", respStatus, err );
                        }
                    else
                        {
                        IPTVLOGSTRING2_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: adding iServices(%d) succeeded.", i );                
                        }
 
                    }
 
                } //for
            CleanupStack::PopAndDestroy( existingServices ); // <-2
            CleanupStack::PopAndDestroy( smClient ); // <-1
            } // if ( iServices->Count() > 0 )
        } // if ( iServices )

    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: deleting iServices" );
    delete iServices;
    iServices = NULL;

    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter:: creating iServices" );
    iServices = CIptvServices::NewL();
    
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SavingFinalizedL() exit" );
    }

// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::AddServiceResp
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::AddServiceResp( TRespStatus /*aRespStatus*/ )
    {
    }
// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::UpdateServiceResp
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::UpdateServiceResp( TRespStatus /*aRespStatus*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::DeleteServiceResp
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::DeleteServiceResp( TRespStatus /*aRespStatus*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::GetServicesResp
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::GetServicesResp( TRespStatus /*aRespStatus*/, 
                                                   CDesC8ArraySeg* /*aServicesArray*/ ) 
    {
    }
    
// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::GetUsedIapResp
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::GetUsedIapResp( TUint32 /*aIapId*/,
                                                  const TDesC& /*aIapName*/,
                                                  CIptvNetworkSelection::TConnectionPermission /*aConnectionPermission*/,
                                                  TBool /*aWlanWhenGPRS*/,
                                                  CIptvNetworkSelection::TRespStatus /*aRespStatus*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::ServerShutdownResp
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::ServerShutdownResp( TRespStatus /*aRespStatus*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::HandleSmEvent
// -----------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::HandleSmEvent( CIptvSmEvent& /*aEvent*/ )
    {
    }

// ---------------------------------------------------------------------------
// CIptvOmaProvisioningAdapter::SetIconPathL()
// ---------------------------------------------------------------------------
//
void CIptvOmaProvisioningAdapter::SetIconPathL( const TDesC& aIconPath )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvOmaProvisioningAdapter::SetIconPathL" );

    // Check for a file name without drive letter.
    if ( ( aIconPath.Length() > 0 ) && ( aIconPath[0] == '\\' ) )
        {
        TBool accept( EFalse );
        TDriveList driveList;
        TDriveInfo driveInfo;

        if ( KErrNone == iFs.DriveList( driveList ) )
            {
            HBufC* tempName =
                HBufC::NewLC( aIconPath.Length() + KIptvDriveLetterSpace );
            TChar character;

            for ( TInt i = EDriveA; i <= EDriveZ && !accept; i++ )
                {
                if ( driveList[i] &&
                     ( KErrNone == iFs.Drive( driveInfo, i ) ) &&
                     ( KErrNone == RFs::DriveToChar( i, character ) ) )
                    {
                    tempName->Des().Zero();
                    tempName->Des().Append( character );
                    tempName->Des().Append( ':' );
                    tempName->Des().Append( aIconPath );

                    IPTVLOGSTRING2_LOW_LEVEL(
                        "Scanning icon from %S", tempName );
                    if ( BaflUtils::FileExists( iFs, *tempName ) )
                        {
                        IPTVLOGSTRING_LOW_LEVEL( " ---- Icon found" );
                        accept = ETrue;
                        }
                    }
                }

            if ( accept && iCurrentService )
                {
                iCurrentService->SetIconPath( tempName->Des() );
                }

            CleanupStack::PopAndDestroy( tempName );
            }
        }
    else if ( iCurrentService )
        {
        iCurrentService->SetIconPath( aIconPath );
        }
    }
