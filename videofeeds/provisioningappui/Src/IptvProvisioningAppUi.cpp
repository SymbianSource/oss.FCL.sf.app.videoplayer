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
* Description: 
*
*/




// INCLUDES
#include <eikdoc.h>
#include "IptvDebug.h"
#include "CIptvIapList.h"
#include "CIptvUtil.h"
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <IptvProvisioningApp.rsg>
#include <aknnotewrappers.h>
#include <apgcli.h> // RApaLsSession
#include <bautils.h>

#include "CIptvService.h"
#include "CIptvServiceManagementClient.h"
#include "CIptvProvisioningProcessor.h"
#include "IptvProvisioningAppUi.h"
#include "IptvProvisioningApp.h"
#include "IptvProvisioningDocument.h"
#include "iptvlocalisationliterals.h"
#include <videoplayeractivationmessage.h>

// CONSTANTS
const TUid KUidMpxVideoPlayerApplication = { 0x200159B2 };
const TUid KMessageUid = { 10500 };

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::CIptvProvisioningAppUi()
// ----------------------------------------------------------------------------
//
CIptvProvisioningAppUi::CIptvProvisioningAppUi()
    {
    // None
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::ConstructL()
// ----------------------------------------------------------------------------

void CIptvProvisioningAppUi::ConstructL()
    {
    // Call base classes ConstructL method.
    BaseConstructL( ENoScreenFurniture | EAknEnableMSK );

    StatusPane()->MakeVisible( EFalse );
    HideApplicationFromFSW( ETrue );
    iEcomSession = REComSession::OpenL();
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::~CIptvProvisioningAppUi()
// ----------------------------------------------------------------------------
//
CIptvProvisioningAppUi::~CIptvProvisioningAppUi()
    {
    iEcomSession.Close();
    REComSession::FinalClose();
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::HandleKeyEventL( )
// ----------------------------------------------------------------------------
//
TKeyResponse CIptvProvisioningAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aType*/ )
    {
    // Do not consume any keyevents in here
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::HandleCommandL( TInt aCommand)
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::HandleCommandL( TInt /*aCommand*/ )
    {
    // Since there is no menubars etc there is no need for the
    // implementation of this method
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::ProcessCommandParametersL( TInt aCommand)
// ----------------------------------------------------------------------------
//
TBool CIptvProvisioningAppUi::ProcessCommandParametersL(
    TApaCommand /*aCommand*/,
    TFileName& /*aDocumentName*/,
    const TDesC8& /*aTail*/ )
    {
    // Always return ETrue to make CIptvProvisioningDocument's
    // OpenFileL to be called.
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::HandleFileL( TInt aCommand)
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::HandleFileL( RFile& aFile )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvProvisioningAppUi::HandleFileL");
    // Create instance of the processor which takes care of
    // parsing the content of the xml file.
    CIptvProvisioningProcessor* processor = CIptvProvisioningProcessor::NewL();
    CleanupStack::PushL( processor );

    // Ask processor to handle aFile that is opened by the framework.
    CIptvServices* services = NULL;
    TInt errorCode = KErrNone;
    TRAPD( provisioningError,
           services = processor->ExternalProvisionL( aFile, errorCode ) );
    aFile.Close();

    // If leave happened, set the error status so that note will be shown.
    if ( errorCode == KErrNone && provisioningError != KErrNone )
        {
        errorCode = provisioningError;
        }

    // Since processor is not needed in here any more, we can destroy
    // it now
    CleanupStack::PopAndDestroy( processor );

    // If there was something wrong while parsing the provisioning data
    // just stop things immediatelly. Xml file MUST be constructed in
    // the way that processor wont get any error messages during parsing.
    if ( errorCode != KErrNone )
        {
        if ( services )
            {
            delete services;
            }

        if ( errorCode == KIptvInvalidServiceType )
            {
            ShowInvalidServiceTypeNoteL();
            }
        else
            {
            ShowInvalidDataNoteL();
            }

        User::Exit( 0 );
        }

    if ( services )
        {
        CleanupStack::PushL( services );
        HandleServicesAdditionL( services );
        CleanupStack::PopAndDestroy( services );
        }

    // Finally close this app since our job here is done
    User::Exit( 0 );
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::HandleServiceAdditionL()
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::HandleServicesAdditionL( CIptvServices* aServices )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningAppUi::HandleServicesAdditionL" );

    if ( !aServices )
        {
        IPTVLOGSTRING_LOW_LEVEL( "Leave: NULL argument aServices" );
        User::Leave( KErrArgument );
        }

    IPTVLOGSTRING2_LOW_LEVEL( "Received %d services", aServices->Count() );

    TInt counter( ( aServices->Count() > KMaxNumberOfServices ) ?
        KMaxNumberOfServices : aServices->Count() );
    CIptvService* service = NULL;
    TInt32 launchId( KErrNotFound );
    TUint32 receivedGgroupId( 0 );
    TUint32 lauchGroupId( 0 );

    // Create instance of service management client
    CIptvServiceManagementClient* client =
        CIptvServiceManagementClient::NewL( *this );
    CleanupStack::PushL( client );

    // Array for existing group items
    RArray<TUint32> existingGroupIds;
    CleanupClosePushL( existingGroupIds );
    FindExistingGroupedIdsL( client, existingGroupIds );

    // Orginal group items order
    RArray<TUint32> groupItemsOrder;
    CleanupClosePushL( groupItemsOrder );

    // Add new services
    for ( TInt i = 0; i < counter; i++ )
        {
        service = aServices->GetServiceL( i );
        CleanupStack::PushL( service );

        // Check that plugin for service exist and user want's to install
        // service before proceeding
        if ( IsServiceValidL( *service ) )
            {
            if ( VerifyServiceGroupL(
                client, *service, receivedGgroupId, lauchGroupId ) )
                {
                CIptvServices* tempServices = CIptvServices::NewL();
                CleanupStack::PushL( tempServices );
                TInt err( client->GetServicesL(
                    service->GetProviderId(), *tempServices ) );

                // If get services was successful and there
                // was no service for serviceId found, we can add new
                // service to the database.
                if ( err == KErrNone )
                    {
                    TBool feed( EFalse );
                    TBool noNote( service->GetGroupId() > 0 &&
                        service->GetType() != CIptvService::EServiceGroup );
                    TRespStatus status( EEmpty );

                    if ( tempServices->Count() == 0 )
                        {
                        // Add new service
                        if ( noNote || DoesUserWantToInstallServiceL( *service ) )
                            {
                            IPTVLOGSTRING_LOW_LEVEL("Calling client->AddServiceL");

                            // Before adding the service to service list, check if has
                            // any IAPs defined. If doesn't, add some if we are allowed.
                            if ( ( service->GetIapList().Count() == 0 ) &&
                                !( service->GetFlags() & CIptvService::EReadOnlyIaps ) )
                                {
                                CIptvIapList* iapList = CIptvIapList::NewL();
                                CleanupStack::PushL( iapList );

                                // Get IAP list for the new service.
                                if ( GetIapListForNewServiceL( client, *iapList ) == KErrNone )
                                    {
                                    service->SetIapListL( *iapList );
                                    }

                                CleanupStack::PopAndDestroy( iapList );
                                }

                            // Send request to add parsed service to the database
                            err = client->AddServiceL( *service, status );

                            // Save orginal order of services in vcfg
                            if ( noNote && err == KErrNone )
                                {
                                groupItemsOrder.Append(
                                    GetServiceIdByProviderIdL(
                                        client, service->GetProviderId() ) );
                                }

                            // Only vod & vodcast & servicegroup services are not installed to main level.
                            if ( service->GetType() == CIptvService::EVod ||
                                 service->GetType() == CIptvService::EVodCast ||
                                 service->GetType() == CIptvService::EServiceGroup )
                                {
                                feed = ETrue;
                                }

                            // First new added succesfully, store the serviceId for query.
                            if ( err == KErrNone && status == ESucceeded  &&
                                 launchId == KErrNotFound && feed && !noNote )
                                {
                                launchId = GetServiceIdByProviderIdL(
                                    client, service->GetProviderId() );
                                }
                            }
                        else
                            {
                            err = KErrCancel;
                            }
                        }
                    else if ( tempServices->Count() == 1 ) // Update
                        {
                        if ( noNote || OverwriteExistingServiceL( service->GetName() ) )
                            {
                            IPTVLOGSTRING_LOW_LEVEL("Calling client->UpdateServiceL");

                            // Get the service and dig out the auto-increment service-id
                            // from existing service and set it to the one to be updated
                            CIptvService* existingService = tempServices->GetServiceL( 0 );
                            CleanupStack::PushL( existingService );

                            const TInt existingId( existingService->GetId() );
                            service->SetId( existingId );
                            const TInt existingGroup( existingGroupIds.Find( existingId ) );
                            if ( existingGroup > KErrNotFound )
                                {
                                existingGroupIds.Remove( existingGroup );
                                }

                            // Before updating service, chech if it has any IAPs defined.
                            // If it doesn't, take IAPs from old service if we are allowed.
                            if ( ( service->GetIapList().Count() == 0 ) &&
                                !( service->GetFlags() & CIptvService::EReadOnlyIaps ) )
                                {
                                service->SetIapListL( existingService->GetIapList() );
                                }

                            err = client->UpdateServiceL( *service, status );

                            // Save orginal order of services in vcfg
                            if ( noNote && err == KErrNone )
                                {
                                groupItemsOrder.Append( existingId );
                                }

                            // Only vod & vodcast services are not installed to main level.
                            if ( service->GetType() == CIptvService::EVod ||
                                 service->GetType() == CIptvService::EVodCast ||
                                 service->GetType() == CIptvService::EServiceGroup )
                                {
                                feed = ETrue;
                                }

                            // First one updated succesfully, store the serviceId for query.
                            if ( err == KErrNone && status == ESucceeded &&
                                 launchId == KErrNotFound && feed && !noNote )
                                {
                                IPTVLOGSTRING2_LOW_LEVEL("CIptvProvisioningAppUi::HandleServicesAdditionL update id:%d", existingService->GetId() );
                                launchId = existingService->GetId();
                                }

                            CleanupStack::PopAndDestroy( existingService );
                            }
                        else
                            {
                            err = KErrCancel;
                            }
                        }
                    else
                        {
                        // What if there is two services found from db with
                        // same ProvidedId.
                        // Situation might be that two separate service providers
                        // deliver their provisioning xml files and they have same
                        // ProviderId. This situation should be rather rare case
                        // because ProviderId is usually a url pointing to some
                        // provider specific address.
                        }

                    // Check, everything go right. Currently there is no feedback defined
                    // to be shown for the user was the operation successful or not
                    if ( err == KErrNone && status == ESucceeded )
                        {
                        // Everything went just fine. If in the future there is a need
                        // to display note to the user about successful operation,
                        // add it here
                        IPTVLOGSTRING_LOW_LEVEL("Add/Update service: err = KErrNone, status = ESucceeded");
                        if ( !noNote && !feed )
                            {
                            // Done note shown only if not feed type
                            ShowDoneNoteL( feed );
                            }
                        }
                    else
                        {
                        // Service was not installed to the service database. If user
                        // needs to be notified about the error situation, do it here.
                        IPTVLOGSTRING3_LOW_LEVEL("Add/Update service: err = %d, status = %d", err, status );
                        }
                    }

                CleanupStack::PopAndDestroy( tempServices );
                }
            }

        CleanupStack::PopAndDestroy( service );
        }

    // Handle grouped items
    if ( receivedGgroupId > 0 )
        {
        // Update parsed indication of group feed
        SetGroupReceivedL( client, receivedGgroupId );
        // Remove existing non updated group services
        DeleteExistingGroupedIdsL( client, existingGroupIds, receivedGgroupId );
        // Verify and force order of group items
        ForceOrderOfGroupedServicesL( client, groupItemsOrder );
        }

    CleanupStack::PopAndDestroy( &groupItemsOrder );
    CleanupStack::PopAndDestroy( &existingGroupIds );
    CleanupStack::PopAndDestroy( client );

    // All db modification made. Now show query to open first added feed.
    if ( launchId > KErrNotFound )
        {
        QueryAddedServiceOpenL( launchId, lauchGroupId );
        }
    }

// -----------------------------------------------------------------------------
// CIptvProvisioningAppUi::GetIapListForNewServiceL()
// Gathers list of IAPs from other user defined services.
// -----------------------------------------------------------------------------
//
TInt CIptvProvisioningAppUi::GetIapListForNewServiceL(
    CIptvServiceManagementClient* aClient,
    CIptvIapList& aIapList ) const
    {
    if ( !aClient )
        {
        IPTVLOGSTRING_LOW_LEVEL( "Leave: NULL argument aClient" );
        User::Leave( KErrArgument );
        }

    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CDesC8ArraySeg* services = NULL;

    TInt error = aClient->GetServicesL( 0,
                    CIptvServiceManagementClient::EDisplayOrderAscending,
                    services,
                    respStatus );

    if ( error == KErrNone && services )
        {
        CleanupStack::PushL( services );

        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );

        // Go through all services.
        for (TInt i = 0; i < services->Count(); i++ )
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

        if ( aIapList.Count() > 0 )
            {
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CIptvProvisioningAppUi::DoesUserWantToInstallServiceL( CIptvService& )
// ---------------------------------------------------------------------------
//
TBool CIptvProvisioningAppUi::DoesUserWantToInstallServiceL(
    CIptvService& aService ) const
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvProvisioningAppUi::DoesUserWantToInstallServiceL" );

    TBool install( EFalse );

    // Create confirmation text combining resourse text and localised name
    // of the service.
    HBufC* confirmationText;
    HBufC* localisedServiceName( NULL );
    if ( 0 == aService.GetName().CompareF( KQtnIptvVideoStoreList ) )
        {
        localisedServiceName = StringLoader::LoadLC( R_IPTV_VIDEO_STORE_LIST );
        confirmationText = StringLoader::LoadLC(
                            R_IPTV_INSTALL_PROVISIONING_SERVICE,
                            *localisedServiceName );
        }
    else
        {
        confirmationText = StringLoader::LoadLC(
                            R_IPTV_INSTALL_PROVISIONING_SERVICE,
                            aService.GetName() );
        }

    CAknQueryDialog* dialog = new( ELeave ) CAknQueryDialog(
                                *confirmationText,
                                CAknQueryDialog::ENoTone );

    // Kick the dialog to the screen and wait for an answer.
    if ( dialog->ExecuteLD( R_IPTV_PROVISIONING_QUERY_INSTALL_SERVICE )
      == EAknSoftkeyOk )
        {
        install = ETrue;
        }

    // Destroy the text allocated by string loader
    CleanupStack::PopAndDestroy( confirmationText );
    if ( localisedServiceName )
        {
        CleanupStack::PopAndDestroy( localisedServiceName );
        }

    return install;
    }

// ---------------------------------------------------------------------------
// CIptvProvisioningAppUi::OverwriteExistingServiceL( )
// ---------------------------------------------------------------------------
//
TBool CIptvProvisioningAppUi::OverwriteExistingServiceL(
    const TDesC& aServiceName ) const
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvProvisioningAppUi::OverwriteExistingServiceL" );
    TBool overWrite( EFalse );

    // Create confirmation text combining resourse text and name of the
    // service.
    HBufC* confirmationText;
    HBufC* localisedServiceName( NULL );
    if ( 0 == aServiceName.CompareF( KQtnIptvVideoStoreList ) )
        {
        localisedServiceName = StringLoader::LoadLC( R_IPTV_VIDEO_STORE_LIST );
        confirmationText = StringLoader::LoadLC(
            R_IPTV_RESET_EXISTING_SERVICE, *localisedServiceName );
        }
    else
        {
        confirmationText = StringLoader::LoadLC(
            R_IPTV_RESET_EXISTING_SERVICE, aServiceName );
        }

    CAknQueryDialog* dialog = new( ELeave ) CAknQueryDialog(
            *confirmationText,
            CAknQueryDialog::ENoTone );

    // Display the dialog to the screen and wait for an answer.
    if ( dialog->ExecuteLD(
        R_IPTV_PROVISIONING_VOD_RESET_EXISTING_SERVICE ) == EAknSoftkeyOk )
        {
        overWrite = ETrue;
        }

    // Destroy the text allocated by string loader.
    CleanupStack::PopAndDestroy( confirmationText );
    if ( localisedServiceName )
        {
        CleanupStack::PopAndDestroy( localisedServiceName );
        }

    return overWrite;
  }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::ShowInvalidDataNoteL
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::ShowInvalidDataNoteL() const
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvProvisioningAppUi::ShowInvalidDataNoteL");

    HBufC* text = StringLoader::LoadLC( R_IPTV_INVALID_DATA );
    CAknErrorNote* note = new( ELeave ) CAknErrorNote( R_AKN_ERROR_NOTE );

    note->ExecuteLD(*text);
    CleanupStack::PopAndDestroy(text);
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::ShowInvalidServiceTypeNoteL
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::ShowInvalidServiceTypeNoteL() const
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvProvisioningAppUi::ShowInvalidServiceTypeNoteL");

    HBufC* text = StringLoader::LoadLC( R_IPTV_INVALID_SERVICE_TYPE );
    CAknErrorNote* note = new( ELeave ) CAknErrorNote( R_AKN_ERROR_NOTE );

    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::ShowDoneNoteL
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::ShowDoneNoteL( TBool aFeed ) const
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvProvisioningAppUi::ShowDoneNoteL");

    HBufC* text;
    if ( aFeed )
        {
        text = StringLoader::LoadLC( R_IPTV_INSTALLED );
        }
    else
        {
        text = StringLoader::LoadLC( R_IPTV_DONE );
        }

    CAknInformationNote* note =
        new( ELeave ) CAknInformationNote( R_AKN_INFORMATION_NOTE );

    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::GetServiceIdByProviderIdL
// ----------------------------------------------------------------------------
//
TUint32 CIptvProvisioningAppUi::GetServiceIdByProviderIdL(
    CIptvServiceManagementClient* aClient,
    const TDesC& aProviderId )
    {
    CIptvServices* services = CIptvServices::NewL();
    CleanupStack::PushL( services );
    User::LeaveIfError( aClient->GetServicesL( aProviderId, *services ) );
    TUint32 serviceId( services->GetServiceRefL( 0 ).GetId() );
    CleanupStack::PopAndDestroy( services );
    return serviceId;
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::QueryAddedServiceOpenL
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::QueryAddedServiceOpenL(
    TUint32 aServiceId, TUint32 aGroupId )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningAppUi::QueryAddedServiceOpenL" );

    HBufC* confirmationText = StringLoader::LoadLC(
            R_IPTV_ADDED_SERVICE_OPEN );

    CAknQueryDialog* dialog = new( ELeave ) CAknQueryDialog(
            *confirmationText, CAknQueryDialog::ENoTone );

    // Display the dialog to the screen and wait for an answer.

    TInt result = dialog->ExecuteLD( R_IPTV_PROVISIONING_VOD_QUERY_ADDED_SERVICE_OPEN );
    
    if( result == EAknSoftkeyYes || result == EAknSoftkeyOk )
        {
        CleanupStack::PopAndDestroy( confirmationText );

        // Start / send message to Main application

        RWsSession session;
        User::LeaveIfError( session.Connect() );
        TApaTaskList taskList( session );
        TApaTask task = taskList.FindApp( KUidMpxVideoPlayerApplication );

        if( task.Exists() )
            {
            // VC is open.
            task.BringToForeground();

            TVideoPlayerActivationMessage params;
            params.iMsgType = TVideoPlayerActivationMessage::ELaunchServiceById;
            params.iMsgSender = TVideoPlayerActivationMessage::ENotification;
            params.iServiceId = aServiceId;
            params.iGroupId = aGroupId;
            params.iActive = 1;
            params.iSingle = ETrue;
            params.iFileId = 0;
            params.iDrive = 0;
            params.iFullPath = KNullDesC;

            TPckg<TVideoPlayerActivationMessage> message( params );
            TInt error = task.SendMessage( KMessageUid, message );

            IPTVLOGSTRING2_LOW_LEVEL( "CIptvVodNotificationLauncher::send message error = %d", error );
            }
        else
            {
            // Task doesn't exist, launch a new instance of an application
            RApaLsSession lsSession;
            // connect to AppArc server
            User::LeaveIfError( lsSession.Connect() );

            TApaAppInfo appInfo;
            if ( lsSession.GetAppInfo( appInfo, KUidMpxVideoPlayerApplication ) == KErrNone )
                {
                CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
                cmdLine->SetExecutableNameL( appInfo.iFullName );

                TVideoPlayerActivationMessage params;
                params.iMsgType = TVideoPlayerActivationMessage::ELaunchServiceById;
                params.iMsgSender = TVideoPlayerActivationMessage::ENotification;
                params.iServiceId = aServiceId;
                params.iGroupId = aGroupId;
                params.iActive = 1;
                params.iSingle = ETrue;
                params.iFileId = 0;
                params.iDrive = 0;
                params.iFullPath = KNullDesC;
                TPckg<TVideoPlayerActivationMessage> message( params );
                cmdLine->SetTailEndL( message );
                User::LeaveIfError( lsSession.StartApp( *cmdLine ) );
                CleanupStack::PopAndDestroy( cmdLine );
                }
            }
        }
    else
        {
        CleanupStack::PopAndDestroy( confirmationText );
        }
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::IsServiceValidL
// ----------------------------------------------------------------------------
//
TBool CIptvProvisioningAppUi::IsServiceValidL( CIptvService& aService )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvProvisioningAppUi::IsServiceValidL");

    TBool liveIncluded( EFalse );
     
    TBool valid = EFalse;
    RImplInfoPtrArray vodArray;
    CleanupClosePushL( vodArray );
    iEcomSession.ListImplementationsL( KRssPluginInterfaceUid, vodArray );
    RImplInfoPtrArray xmlTvArray;
    CleanupClosePushL( xmlTvArray );
    iEcomSession.ListImplementationsL( KXmlTvPluginInterfaceUid, xmlTvArray );

    // Here we check:
    // 1) There is at least one implementation for the pluginuid defined in
    //    service object
    // 2) Service type is either Vod OR
    // 3) Service type is Live AND live is included in the build AND there is a
    //    plugin for live tv available (either smart vision plugin or xml tv plugin
    // 4) Service type is Browser, Search or Upload
    if ( ( aService.GetType() == CIptvService::EVod ||
           aService.GetType() == CIptvService::EServiceGroup ||
         ( aService.GetType() == CIptvService::EVodCast &&
             vodArray.Count() > 0 ) ) ||
         ( aService.GetType() == CIptvService::ELiveTv &&
             liveIncluded && xmlTvArray.Count() > 0 ) ||
         ( aService.GetType() == CIptvService::EBrowser ||
           aService.GetType() == CIptvService::ESearch ||
           aService.GetType() == CIptvService::EUpload ) )
        {
        valid = ETrue;
        }

    CleanupStack::PopAndDestroy( &xmlTvArray );
    CleanupStack::PopAndDestroy( &vodArray );
    return valid;
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::FindExistingGroupedIdsL()
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::FindExistingGroupedIdsL(
    CIptvServiceManagementClient* aClient,
    RArray<TUint32>& aExistingGroupIds )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningAppUi::FindExistingGroupedIdsL" );

    // Find exisitng group feeds
    CDesC8ArraySeg* services = NULL;
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    aClient->GetServicesL(
        CIptvServiceManagementClient::EGroupedServices,
        CIptvServiceManagementClient::EDateAscending,
        services, respStatus );

    // Found any
    if ( services )
        {
        CleanupStack::PushL( services );
        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );

        // Go through all grouped services
        for ( TInt i( 0 ); i < services->Count(); i++ )
            {
            service->SetL( services->MdcaPoint( i ) );
            aExistingGroupIds.Append( service->GetId() );
            }

        CleanupStack::PopAndDestroy( service );
        CleanupStack::PopAndDestroy( services );
        }
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::DeleteExistingGroupedIdsL()
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::DeleteExistingGroupedIdsL(
    CIptvServiceManagementClient* aClient,
    RArray<TUint32>& aExistingGroupIds,
    const TUint32 aReceivedGroup )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningAppUi::DeleteExistingGroupedIdsL" );

    if ( aExistingGroupIds.Count() > 0 )
        {
        // Find exisitng group feeds
        CDesC8ArraySeg* services = NULL;
        MIptvServiceManagementClientObserver::TRespStatus respStatus;
        aClient->GetServicesL(
            CIptvServiceManagementClient::EGroupedServices,
            CIptvServiceManagementClient::EDateAscending,
            services, respStatus );

        // Found any
        if ( services )
            {
            CleanupStack::PushL( services );
            CIptvService* service = CIptvService::NewL();
            CleanupStack::PushL( service );
            // File server for icons deleting
            RFs fs;
            CleanupClosePushL( fs );
            User::LeaveIfError( fs.Connect() );

            // Go through all services
            for ( TInt i( 0 ); i < services->Count(); i++ )
                {
                service->SetL( services->MdcaPoint( i ) );
                if ( service->GetGroupId() == aReceivedGroup )
                    {
                    const TInt index(
                        aExistingGroupIds.Find( service->GetId() ) );
                    if ( index > KErrNotFound )
                        {
                        // Delete icon file if downloaded and exist
                        if ( service->GetIconPath() != KIptvEmptyDes )
                            {
                            if ( BaflUtils::FileExists( fs, service->GetIconPath() ) )
                                {
                                fs.Delete( service->GetIconPath() );
                                }
                            }

                        // Selete service
                        aClient->DeleteServiceL( aExistingGroupIds[index], respStatus );
                        if ( respStatus != MIptvServiceManagementClientObserver::ESucceeded )
                            {
                            IPTVLOGSTRING_LOW_LEVEL( "Error during deleting existing grouped service !" );
                            User::Leave( KErrGeneral );
                            }
                        }
                    }
                }

            // Verify grouped services items order
            CleanupStack::PopAndDestroy( &fs );
            CleanupStack::PopAndDestroy( service );
            CleanupStack::PopAndDestroy( services );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::ForceOrderOfGroupedServicesL()
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::ForceOrderOfGroupedServicesL(
    CIptvServiceManagementClient* aClient,
    RArray<TUint32>& aGroupItemsOrder )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningAppUi::ForceOrderOfGroupedServicesL" );

    // Find exisitng group feeds
    CDesC8ArraySeg* services = NULL;
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    aClient->GetServicesL(
        CIptvServiceManagementClient::EGroupedServices,
        CIptvServiceManagementClient::EDisplayOrderAscending,
        services, respStatus );

    // Found any
    if ( services )
        {
        CleanupStack::PushL( services );

        for ( TInt i( 0 ); i < aGroupItemsOrder.Count(); i++ )
            {
            const TUint expectedId( aGroupItemsOrder[i] );
            CIptvService* service = CIptvService::NewL();
            CleanupStack::PushL( service );
            service->SetL( services->MdcaPoint( i ) );

            // Verify order
            TBool needUpdate( EFalse );
            if ( service->GetId() != expectedId )
                {
                // Find missplaced service
                for ( TInt j( 0 ); j < services->Count(); j++ )
                    {
                    CIptvService* second = CIptvService::NewL();
                    CleanupStack::PushL( second );
                    second->SetL( services->MdcaPoint( j ) );
                    if ( second->GetId() == expectedId )
                        {
                        // Swap those two service display order
                        TRespStatus status( EEmpty );
                        second->SetDisplayOrder( service->GetDisplayOrder() );
                        TInt err( aClient->UpdateServiceL( *second, status ) );

                        // Verify status
                        if ( err != KErrNone || status != ESucceeded )
                            {
                            IPTVLOGSTRING3_LOW_LEVEL(
                                "CIptvProvisioningAppUi::ForceOrderOfGroupedServicesL(), err: %d, status: %d",
                                err, status );
                            User::Leave( KErrCompletion );
                            }

                        // Swap done
                        needUpdate = ETrue;
                        j = services->Count();
                        }

                    CleanupStack::PopAndDestroy( second );
                    }
                }

            CleanupStack::PopAndDestroy( service );

            // Local services list need to be updated when display order swapped
            if ( needUpdate )
                {
                CleanupStack::Pop( services );
                delete services; services = NULL;
                aClient->GetServicesL(
                    CIptvServiceManagementClient::EGroupedServices,
                    CIptvServiceManagementClient::EDisplayOrderAscending,
                    services, respStatus );
                CleanupStack::PushL( services );
                }
            }

        CleanupStack::PopAndDestroy( services );
        }
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::VerifyServiceGroupL()
// ----------------------------------------------------------------------------
//
TBool CIptvProvisioningAppUi::VerifyServiceGroupL(
    CIptvServiceManagementClient* aClient,
    CIptvService& aService,
    TUint32& aGroupId,
    TUint32& aLaunchId )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningAppUi::VerifyServiceGroupL" );

    const TUint32 groupId( aService.GetGroupId() );
    if ( groupId == 0 ||
       ( groupId > 0 && aService.GetType() == CIptvService::EServiceGroup ) )
        {
        // Group id for service lauch
        if ( groupId > 0 && aLaunchId == 0 )
            {
            aLaunchId = groupId;
            }

        return ETrue; // Not grouped feed
        }

    // Find group feeds
    CDesC8ArraySeg* services = NULL;
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    aClient->GetServicesL(
        CIptvServiceManagementClient::EServiceGroup,
        CIptvServiceManagementClient::EDateAscending,
        services, respStatus );

    // Found any
    TBool found( EFalse );
    if ( services )
        {
        CleanupStack::PushL( services );
        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );

        // Go through all services
        for ( TInt i( 0 ); !found && i < services->Count(); i++ )
            {
            service->SetL( services->MdcaPoint( i ) );
            if ( service->GetGroupId() == groupId )
                {
                found = ETrue; // ok to install
                aGroupId = groupId;
                }
            }

        CleanupStack::PopAndDestroy( service );
        CleanupStack::PopAndDestroy( services );
        }

    return found;
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::SetGroupReceivedL()
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::SetGroupReceivedL(
    CIptvServiceManagementClient* aClient,
    const TUint32& aGroupId )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningAppUi::SetGroupReceivedL" );

    // Find group feeds
    CDesC8ArraySeg* services = NULL;
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    aClient->GetServicesL(
        CIptvServiceManagementClient::EServiceGroup,
        CIptvServiceManagementClient::EDateAscending,
        services, respStatus );

    // Find and update flag
    if ( services )
        {
        CleanupStack::PushL( services );
        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );

        // Go through all services
        for ( TInt i( 0 ); i < services->Count(); i++ )
            {
            service->SetL( services->MdcaPoint( i ) );
            if ( service->GetGroupId() == aGroupId )
                {
                service->SetFlags(
                    service->GetFlags() | CIptvService::EGroupReceived );
                TRespStatus status( EEmpty );
                User::LeaveIfError(
                    aClient->UpdateServiceL( *service, status ) );
                i = services->Count();
                }
            }

        CleanupStack::PopAndDestroy( service );
        CleanupStack::PopAndDestroy( services );
        }
    }

// Methods implemented to fullfill inheritance of
// MIptvServiceManagementClientObserver. Implementation of the inherited
// methods is empty since we do not use asynchorous versions of the methods
// but nevertheless CIptvServiceManagementClient requires the observer to
// be given for the NewL

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::AddServiceResp( TRespStatus )
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::AddServiceResp(
    TRespStatus /*aRespStatus*/)
    {
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::UpdateServiceResp( TRespStatus )
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::UpdateServiceResp(
    TRespStatus /*aRespStatus*/)
    {
    }


// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::DeleteServiceResp( TRespStatus )
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::DeleteServiceResp(
    TRespStatus /*aRespStatus*/ )
    {
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::GetServicesResp( TRespStatus, CDesC8ArraySeg* )
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::GetServicesResp(
    TRespStatus /*aRespStatus*/,
    CDesC8ArraySeg* /*aServicesArray*/ )
    {
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::GetUsedIapResp( TUInt,
//                                         const TDesC&,
//                                         TConnectionPermission,
//                                         TRespStatus )
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::GetUsedIapResp(
    TUint32 /*aIapId*/,
    const TDesC& /*aIapName*/,
    CIptvNetworkSelection::TConnectionPermission /*aConnectionPermission*/,
    TBool /*aWlanWhenGPRS*/,
    CIptvNetworkSelection::TRespStatus /*aRespStatus*/ )
    {
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningAppUi::ServerShutdownResp( TRespStatus )
// ----------------------------------------------------------------------------
//
void CIptvProvisioningAppUi::ServerShutdownResp(
    TRespStatus /*aRespStatus*/ )
    {
    }
