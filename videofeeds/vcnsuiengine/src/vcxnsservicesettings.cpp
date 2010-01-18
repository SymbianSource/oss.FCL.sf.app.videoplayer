/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include <bldvariant.hrh>
#include "IptvDebug.h"
#include "CIptvIapList.h"

#include "CIptvServiceManagementClient.h"
#include "vcxnsservicesettings.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::CVcxNsServiceSettings()
// 
// -----------------------------------------------------------------------------
//
CVcxNsServiceSettings::CVcxNsServiceSettings( )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::CVcxNsServiceSettings()
// 
// -----------------------------------------------------------------------------
//
CVcxNsServiceSettings::~CVcxNsServiceSettings( )
    {
    delete iIptvServiceManagementClient;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::NewL()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsServiceSettings* CVcxNsServiceSettings::NewL( )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceSettings::NewL()");
    
    return new (ELeave) CVcxNsServiceSettings();
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::GetAllServicesL()
// Note: Returns NULL if an error occurs.
// -----------------------------------------------------------------------------
//
EXPORT_C CDesC8ArraySeg* CVcxNsServiceSettings::GetAllServicesL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceSettings::GetAllServicesL()");

    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CDesC8ArraySeg* services = NULL;

    TInt error = GetServiceManagementClientL()->GetServicesL( 0, 
                                                              CIptvServiceManagementClient::EDisplayOrderDescending,
                                                              services,
                                                              respStatus );
    if ( error == KErrNone )
        {
        return services;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::GetAllSelectedServicesL()
// Note: Returns NULL if an error occurs.
// -----------------------------------------------------------------------------
//
EXPORT_C CDesC8ArraySeg* CVcxNsServiceSettings::GetAllSelectedServicesL( TInt& aError )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceSettings::GetAllSelectedServicesL()");
    
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CDesC8ArraySeg* services = NULL;
    
    aError = GetServiceManagementClientL()->GetServicesL( CIptvServiceManagementClient::ESelectedServices, 
                                                          CIptvServiceManagementClient::EDisplayOrderDescending,
                                                          services,
                                                          respStatus );
    if ( aError == KErrNone )
        {
        return services;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::GetServicesByTypeL()
// Note: Returns NULL if an error occurs.
// -----------------------------------------------------------------------------
//
EXPORT_C CDesC8ArraySeg* CVcxNsServiceSettings::GetServicesByTypeL( CIptvService::TServiceType aServiceType )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceSettings::GetServicesByTypeL()");

    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CDesC8ArraySeg* services = NULL;

    CIptvServiceManagementClient::TSearchLimitFlag flag = (CIptvServiceManagementClient::TSearchLimitFlag) 0;

    switch ( aServiceType )
        {
        case CIptvService::EVod:
            // using flag EVodServices to also include service groups and vodcasts.
            flag = CIptvServiceManagementClient::EVodServices;
            break;
        case CIptvService::ELiveTv:
            flag = CIptvServiceManagementClient::ELiveTv;
            break;
        case CIptvService::EVodCast:
            flag = CIptvServiceManagementClient::EVodCast;
            break;
        case CIptvService::EBrowser:
            flag = CIptvServiceManagementClient::EBrowser;
            break;
        case CIptvService::EVideoRemote:     
            flag = CIptvServiceManagementClient::EVideoRemote;
            break;               
        case CIptvService::EApplication:
            flag = CIptvServiceManagementClient::EApplication;
            break;        
        case CIptvService::EMobileTv:
            flag = CIptvServiceManagementClient::EMobileTv;
            break;
        case CIptvService::EOther:
            flag = CIptvServiceManagementClient::EOther;
            break;        
        default:     
            flag = (CIptvServiceManagementClient::TSearchLimitFlag) 0;
            break;
        }

    TInt error = GetServiceManagementClientL()->GetServicesL( flag, 
                                                              CIptvServiceManagementClient::EDisplayOrderDescending,
                                                              services,
                                                              respStatus );                                                             
    if ( error == KErrNone )
        {
        return services;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::GetIapListForNewServiceL()
// Gathers list of IAPs from other user defined services.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVcxNsServiceSettings::GetIapListForNewServiceL( CIptvIapList& aIapList )
    {
    CDesC8ArraySeg* services = GetAllServicesL();
    
    if ( services )
        {
        CleanupStack::PushL( services );

        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );

        // Go through all services.
        for (TInt i = 0; i < services->Count(); i++)
            {
            service->SetL( services->MdcaPoint(i) );

            TUint32 flags = service->GetFlags();

            // Only copy IAPs from user modifiable services (not provisioned).
            if ( ! (flags & CIptvService::EReadOnlyIaps) )
                {
                // Go through all IAPs for this service.
                for ( TUint8 j = 0; j < service->iIapList->Count(); j++ )
                    {
                    TIptvIap& existingIap = service->iIapList->IapL( j ); 

                    // Check that this IAP is not already on user's list.
                    TBool bFound = EFalse;
                    for ( TUint8 k = 0; k < aIapList.Count(); k++ )
                        {
                        TIptvIap& userIap = aIapList.IapL( k );
                        
                        if ( userIap.iId == existingIap.iId )
                            {
                            bFound = ETrue;
                            break;
                            }
                        }
                      
                    // Add IAP to user's list.  
                    if ( ! bFound )
                        {
                        aIapList.AddIap( existingIap );
                        }
                    }
                }
            }

        CleanupStack::PopAndDestroy( service );
        services->Reset();
        CleanupStack::PopAndDestroy( services );

        if (aIapList.Count() > 0)
            {
            return KErrNone;            
            }
        }

    return KErrNotFound;
    }
// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::AddServiceL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVcxNsServiceSettings::AddServiceL( const TDesC& aServiceName,
                                             const TDesC& aAddress,
                                             const TDesC& aDescription,
                                             CIptvIapList& aIapList,
                                             CIptvService::TServiceType aServiceType,
                                             const TUid& aEpgPluginUid,
                                             const TUid& aIptvPluginUid,
                                             const TUid& aVodPluginUid )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceSettings::AddServiceL()");

    if ( aServiceName.Length() == 0 || aAddress.Length() == 0 )
        {
        return KErrArgument;
        }

    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CIptvService*                                     iptvService = CIptvService::NewL();
    CleanupStack::PushL( iptvService );

    iptvService->SetId( 0 );
    iptvService->SetName( aServiceName );
    iptvService->SetAddress( aAddress );
    iptvService->SetIconPath( KNullDesC );
    iptvService->SetType( aServiceType );
    iptvService->SetDesc( aDescription );
    iptvService->SetFlags( CIptvService::ESelected );
    iptvService->SetApplicationUid( 0 );
    iptvService->SetDisplayOrder( 0 );
    iptvService->iIapList->SetL( aIapList );
    
    iptvService->SetEpgPluginUid(aEpgPluginUid);
    iptvService->SetIptvPluginUid(aIptvPluginUid);
    iptvService->SetVodPluginUid(aVodPluginUid);

    TInt error = GetServiceManagementClientL()->AddServiceL( *iptvService, respStatus );



    CleanupStack::PopAndDestroy( iptvService );
    return error;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::UpdateServiceL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsServiceSettings::UpdateServiceL( CIptvService& aIptvService )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceSettings::UpdateServiceL()");
    
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    
    GetServiceManagementClientL()->UpdateServiceL( aIptvService, respStatus );
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::DeleteServiceL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsServiceSettings::DeleteServiceL( TUint32 aItemId )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceSettings::DeleteServiceL()");

    MIptvServiceManagementClientObserver::TRespStatus respStatus;

    GetServiceManagementClientL()->DeleteServiceL( aItemId, respStatus );
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::GetServiceL()
// Note: Method is not efficient, so use only if absolutely necessary.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVcxNsServiceSettings::GetServiceL( TUint32 aServiceId, CIptvService& aIptvService )
    {
    /*CDesC8ArraySeg* services = NULL;
    MIptvServiceManagementClientObserver::TRespStatus respStatus;

    if ( ! iIptvServiceManagementClient )
        {
        iIptvServiceManagementClient = CIptvServiceManagementClient::NewL( *this );
        }

    TInt error = iIptvServiceManagementClient->GetServicesL( aServiceId, aServiceId, services, respStatus );

    if ( error == KErrNone && services->MdcaCount() > 0)
        {
        CleanupStack::PushL( services );
        aIptvService.SetL( services->MdcaPoint(0) );
        CleanupStack::PopAndDestroy( services );
        return KErrNone;
        }

    return KErrNotFound;
    */

    //Get only this specified service   
    CDesC8ArraySeg* services = GetAllServicesL();
    TInt            error    = KErrNotFound;

    if ( services )
        {
        CleanupStack::PushL( services );

        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );

        // Go through all services.
        for ( TInt i = 0; i < services->Count(); i++ )
            {
            service->SetL( services->MdcaPoint(i) );
            
            if ( service->GetId() == aServiceId )
                {
                aIptvService.SetL( services->MdcaPoint(i) );
                error = KErrNone;
                }
            }

        CleanupStack::PopAndDestroy( service );
        CleanupStack::PopAndDestroy( services );
        }
    
    return error;    
    
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::GetServiceManagementClientL()
// -----------------------------------------------------------------------------
//
CIptvServiceManagementClient* CVcxNsServiceSettings::GetServiceManagementClientL()
    {
    if ( ! iIptvServiceManagementClient )
        {
        iIptvServiceManagementClient = CIptvServiceManagementClient::NewL( *this );
        }
    
    return iIptvServiceManagementClient;    
    }


// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::GetScheduledDownloadSettingsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVcxNsServiceSettings::GetScheduledDownloadSettingsL(
    TIptvServiceId aServiceId,
    TIptvVodScheduleDownloadtimeCombination& aDownloadTime,
    TIptvVodScheduleConnectionCondition& aCondition,
    TIptvVodScheduleDownloadTypeCombination& aType )
    {
    CIptvService* service = CIptvService::NewL();
    CleanupStack::PushL( service );
    
    TInt err = GetServiceL( aServiceId, *service );
    
    if ( err == KErrNone )
        {
        aDownloadTime = service->ScheduleDlTime();
        aCondition = static_cast<TIptvVodScheduleConnectionCondition>(
                        service->ScheduleDlNetwork() );
        aType = service->ScheduleDlType();
        }
    
    CleanupStack::PopAndDestroy( service );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceSettings::SetScheduledDownloadSettingsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVcxNsServiceSettings::SetScheduledDownloadSettingsL(
    TIptvServiceId aServiceId,
    TIptvVodScheduleDownloadtimeCombination aDownloadTime,
    TIptvVodScheduleConnectionCondition aCondition,
    TIptvVodScheduleDownloadTypeCombination aType )
    {
    CIptvService* service = CIptvService::NewL();
    CleanupStack::PushL( service );
    
    TInt err = GetServiceL( aServiceId, *service );

    if ( err == KErrNone )
        {
        service->SetScheduleDlTime( aDownloadTime );
        service->SetScheduleDlNetwork( aCondition );
        service->SetScheduleDlType( aType );

        //  If this is the first time scheduling for this service.
        if ( service->GetScheduledLastDownloadTime().Int64() == 0 &&
             aCondition != EManual &&
             aDownloadTime != ENoSchedule )
            {
            TTime current;
            current.UniversalTime();
            service->SetScheduledLastDownloadTime( current );
            }
        // If we move back to manual download state, let's reset the 'last
        // scheduled download time' so that old scheduling for service does
        // not confuse user when he later on sets scheduling on again.
        else if ( aCondition == EManual )
            {
            TTime zero( 0 );
            
            service->SetScheduledLastDownloadTime( zero );
            }

        UpdateServiceL( *service );
        }

    CleanupStack::PopAndDestroy( service );

    return err;
    }
