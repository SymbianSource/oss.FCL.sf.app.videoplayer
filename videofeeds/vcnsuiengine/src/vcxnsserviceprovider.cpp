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


// Version : %version: Rel6_49 %

// INCLUDE FILES
#include <bldvariant.hrh>
#include <bautils.h>

#include "IptvDebug.h"
#include "CIptvIapList.h"
#include "CIptvVodContentClient.h"
#include "CIptvServiceManagementClient.h"
#include "vcxnsserviceproviderobserver.h"
#include "vcxnsserviceclienthandler.h"
#include "vcxnscontentclienthandler.h"
#include "vcxnsserviceprovider.h"
#include "vcxnscontentprovider.h"
#include "vcxnscategoryprovider.h"
#include "vcxnscategory.h"
#include "vcxnsuiengine.h"
#include "vcxnsupdateprogresshandler.h"
#include "vcxnscontentsearchhandler.h"

_LIT( KVcxNsRtspProtocol, "rtsp://" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::CVcxNsServiceProvider()
// -----------------------------------------------------------------------------
//
CVcxNsServiceProvider::CVcxNsServiceProvider( CVcxNsUiEngine& aUiEngine ) : iUiEngine( aUiEngine )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsServiceProvider* CVcxNsServiceProvider::NewL( CVcxNsUiEngine& aUiEngine )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceProvider::NewL()");

    CVcxNsServiceProvider* self =  new (ELeave) CVcxNsServiceProvider( aUiEngine );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::~CVcxNsServiceProvider()
// -----------------------------------------------------------------------------
//
CVcxNsServiceProvider::~CVcxNsServiceProvider()
    {
    iUpdateQueue.Reset();
    iServiceObservers.Reset();
    iServiceList.ResetAndDestroy();
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::GetSubscribedServicesL()
// -----------------------------------------------------------------------------
//
RPointerArray<CVcxNsService>& CVcxNsServiceProvider::GetSubscribedServicesL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(
        "UI Engine ## CVcxNsServiceProvider::GetSubscribedServicesL()");

    if( iServiceList.Count() > 0 )
        {
        return iServiceList;
        }
     
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CDesC8ArraySeg* services = NULL;
    TUint32 flags = 0;

    TInt error = iUiEngine.GetServiceClientHandlerL()->GetServiceManagementClientL()->GetServicesL( 
                         CIptvServiceManagementClient::ESelectedServices 
                         || CIptvServiceManagementClient::EBrowser,
                         CIptvServiceManagementClient::EDisplayOrderDescending,
                         services,
                         respStatus );
    
    
    if ( error == KErrNone )
        {
        CleanupStack::PushL( services );
        
        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );
        
        for ( TInt i = 0; i < services->Count(); i++ )
            {
            service->SetL( services->MdcaPoint(i) );
            flags = service->GetFlags();
            
            if ( ( flags & CIptvService::ESelected ) && 
                ( ( service->GetType() == CIptvService::EVod )
                    || ( service->GetType() == CIptvService::EVodCast )
                    || ( service->GetType() == CIptvService::EServiceGroup ) ) )
                {
  		    	CVcxNsService* newService = CVcxNsService::NewL();
                CleanupStack::PushL( newService );
  		    	newService->SetServiceId( service->GetId() );
  		    	newService->SetNameL( service->GetName() );
  		    	newService->SetGroupId( service->GetGroupId() );
  		    	
  		    	if ( service->GetType() == CIptvService::EServiceGroup )
  		    	    {
                    newService->SetType( CVcxNsService::EVcxNsMainGroup );
                    }
  		    	else if ( flags & CIptvService::EReadOnly )
  		    	    {
  		    	    newService->SetType( CVcxNsService::EVcxNsReadOnly );
  		    	    }
  		    	else
  		    	    {
  		    	    newService->SetType( CVcxNsService::EVcxNsEditable ); 
  		    	    }
                
  		    	if ( EnsureFileExists( service->GetIconPath() ))
  		    	    {
  		    	    newService->SetIconPathL( service->GetIconPath() );
  		    	    }

                newService->SetUpdateTime( service->GetEpgUpdateTimeL() );
                newService->SetUpdateStatus( CVcxNsService::ENotUpdating );
                newService->SetUserNameL( service->GetUserName() );
                newService->SetPasswordL( service->GetPassword() );
                newService->SetDescL( service->GetDesc() );
                newService->SetIsReadOnlyIap( flags & CIptvService::EReadOnlyIaps ? ETrue : EFalse );
                newService->SetAccountMgmtUriL( service->GetAccountManagementUrl() );
                
                if( flags & CIptvService::EReadOnlyIaps && service->GetIapList().Count() > 0 )
                    {
                    // if readonly, more than one?
                    newService->SetIap( service->GetIapList().IapL(0).iId );
                    }
                
                iServiceList.AppendL( newService );
                CleanupStack::Pop( newService );
                }
            }

        for ( TInt i = 0; i < services->Count(); i++ )
            {
            service->SetL( services->MdcaPoint(i) );
            flags = service->GetFlags();

            if ( ( service->GetType() == CIptvService::EBrowser )
                    &&  service->GetName().CompareF( KVcxNsVideoDirectory ) == 0 )
                {
                CVcxNsService* videoDirectory = CVcxNsService::NewL();
                CleanupStack::PushL( videoDirectory );

                //Name is localisation string, needs to be handled in UI side.
                videoDirectory->SetServiceId( service->GetId() );
                videoDirectory->SetNameL( service->GetName() );
                videoDirectory->SetCustomUriL( service->GetAddress() );
                videoDirectory->SetIconPathL( service->GetIconPath() );
                videoDirectory->SetType( CVcxNsService::EVcxNsVideoDirectory );

                //Add Video directory item always at the end of the list
                iServiceList.AppendL( videoDirectory );
                CleanupStack::Pop( videoDirectory );

				break; // Allow just one Video Directory
                }
            }

        CleanupStack::PopAndDestroy( service );
        CleanupStack::PopAndDestroy( services );
        }
 
    return iServiceList;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::GetActiveGroupListL()
// -----------------------------------------------------------------------------
//
RPointerArray<CVcxNsService>& CVcxNsServiceProvider::GetActiveGroupListL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(
        "UI Engine ## CVcxNsServiceProvider::GetActiveGroupListL()");

    CVcxNsService* service = GetActiveServiceData();

    if( !service )
        {
        User::Leave( KErrNotFound );
        }
    
    if ( service->Type() == CVcxNsService::EVcxNsMainGroup )
        {
        if ( service->ServiceGroup().Count() <= 0 )
            {
            GetGroupedServicesL( service->GroupId() );
            }
        }

    return service->ServiceGroup();
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::OpenServiceL( TInt aIndex )
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::OpenServiceL( TInt aIndex )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsServiceProvider::OpenServiceL (%d)", aIndex );
    
     if ( iUiEngine.VcAppState() == EStateCustomView ) 
        {
        OpenServiceFromGroupL( aIndex );
        return;
        }
    
    if( !( aIndex >= 0 && aIndex < iServiceList.Count() ) )
        {
        return;
        }
    
    if ( iServiceList[aIndex]->Type() == CVcxNsService::EVcxNsVideoDirectory ) 
        {
        OpenToBrowserL( iServiceList[aIndex]->CustomUri() );
        }
    else
        {
        TUint32 serviceToOpen = iServiceList[aIndex]->GetServiceId();
        
        OpenVideoServiceL( serviceToOpen, aIndex );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::OpenVideoService( TInt aIndex )
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::OpenVideoServiceL( TUint32 aServiceId, TInt aIndex )
    {
    if( aIndex < 0 && aIndex >= iServiceList.Count() )
        {
        return;
        }
    
    TBool shouldBeUpdated( EFalse );
    
    if ( iServiceList[aIndex]->GetUpdateStatus() != CVcxNsService::EUpdateOngoing )
        {
	    if ( iUpdateQueue.Count() > 0 )
	        {
	        IPTVLOGSTRING_LOW_LEVEL("-- Refresh feeds ongoing --");
	        if ( iUpdateQueue.Find( aServiceId ) > KErrNotFound )
	            {
	            shouldBeUpdated = ETrue;
	            }
	        }
	    else if ( iServiceList[aIndex]->Type() == CVcxNsService::EVcxNsMainGroup )
	        {
	        TTime updated( iServiceList[aIndex]->GetUpdateTime() );
	        // See if more than an hour from last update
	        TTime now;
	        now.UniversalTime();
	        TTimeIntervalHours hourInterval( 0 );
	        TInt error( updated.HoursFrom( now, hourInterval ) );
	        if ( error != KErrNone || Abs( hourInterval.Int() ) > 0 )
	            {
	            shouldBeUpdated = ETrue;
	            }
	        }
        }
    
    CVcxNsContentClientHandler* contentClientHandler = 
        iUiEngine.GetContentClientHandlerL();
    
    User::LeaveIfNull( contentClientHandler );

    TBool updateAvailable ( EFalse );
    
    //Check if service update for this available
    CIptvVodContentClient* contentClient = 
        contentClientHandler->GetVodContentClientL( aServiceId );

    User::LeaveIfNull( contentClient );
    
    CVcxNsService::TServiceUpdateStatus statusOpening =
        iServiceList[aIndex]->GetUpdateStatus();
    
    if ( !shouldBeUpdated &&  
         !( ( statusOpening == CVcxNsService::EStartingPending || 
            statusOpening == CVcxNsService::EUpdateOngoing ) ) )
        {
        IPTVLOGSTRING_LOW_LEVEL("-- Check update availability --");
        contentClient->UpdateAvailableL( updateAvailable );
        }
    
    if ( updateAvailable || shouldBeUpdated )
        {
        //Update will start for selected service. 
        // Cancel any update ongoing before opening service:
        for ( TInt i = 0; i < iServiceList.Count(); i++ )
            {
            CVcxNsService::TServiceUpdateStatus status = 
                iServiceList[i]->GetUpdateStatus();
            TUint32 currentlyUpdatedServiceId = iServiceList[i]->GetServiceId();

            if ( status == CVcxNsService::EStartingPending  ||
                 status == CVcxNsService::EUpdateOngoing &&
                 currentlyUpdatedServiceId != aServiceId )
                {
                contentClientHandler->GetVodContentClientL( 
                            currentlyUpdatedServiceId )->CancelUpdate();

                // Update was ongoing, start it whenever possible.
                SetBackToUpdateQueue( currentlyUpdatedServiceId );
                
                CVcxNsUpdateProgressHandler* progressHandler = iUiEngine.GetProgressHandler();
                progressHandler->Cancel();
                
                iServiceList[i]->SetUpdateStatus( CVcxNsService::ENotUpdating );
                ServiceUpdated( i );
                break;
                }
             }
        
        if ( iServiceList[aIndex]->Type() == CVcxNsService::EVcxNsMainGroup
                || iServiceList[aIndex]->Type() == CVcxNsService::EVcxNsEditable
                || iServiceList[aIndex]->Type() == CVcxNsService::EVcxNsReadOnly )
            {
            StartManualUpdateL( aServiceId );
            }
        }
        
    
    iUiEngine.SetActiveService( aServiceId );
    
    if ( iServiceList[aIndex]->Type() == CVcxNsService::EVcxNsMainGroup )
        {
        iUiEngine.SetVcAppState( EStateCustomView );
        }
    else
        {
        RPointerArray<CVcxNsCategory> categorys = iUiEngine.GetCategoryProviderL()->GetCategorysL();
        
        if( categorys.Count() == 0 )
            {
            iUiEngine.SetActiveCategory( KIptvVodContentCategoryRootId );
            iUiEngine.SetVcAppState( EStateContentView );            
            }
        else if( categorys.Count() == 1 &&
                 categorys[0]->GetCategoryId() == KIptvRssSearchCategoryId )
            {
            iUiEngine.SetActiveCategory( (categorys[0])->GetCategoryId() );
            iUiEngine.SetVcAppState( EStateCategoryView );
            }
        else if( categorys.Count() == 1 )
            {
            iUiEngine.SetActiveCategory( (categorys[0])->GetCategoryId() );
            iUiEngine.SetVcAppState( EStateContentView );
            }
        else
            {
            iUiEngine.SetVcAppState( EStateCategoryView );
            }
        }
    }


// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::OpenServiceL
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::OpenServiceL( TUint32 aServiceId )
    {
    TBool found( EFalse );
    for ( TInt i = 0; i < iServiceList.Count(); i++ )
        {
        if ( iServiceList[i]->GetServiceId() == aServiceId )
            {
            OpenServiceL( i );
            found = ETrue;
            break;
            }
        }
    
    if ( !found )
        {
        //First, remove old services
        RemoveServiceData();
        //Get new list 
        GetSubscribedServicesL();
    
        for ( TInt i = 0; i < iServiceList.Count(); i++ )
            {
            if ( iServiceList[i]->GetServiceId() == aServiceId )
                {
                OpenServiceL( i );
                break;
                }
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::GetCurrentHighlight
// -----------------------------------------------------------------------------
//
TInt CVcxNsServiceProvider::GetCurrentHighlight()
    {
    CVcxNsService* service = GetActiveServiceData();
    if ( service )
        {
        return service->HighlightCategoryIndex();
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::SetCurrentHighlight
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::SetCurrentHighlight( TInt aIndex )
    {
    CVcxNsService* service = GetActiveServiceData();
    if ( service )
        {
        return service->SetHighlightCategoryIndex( aIndex );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::RemoveServiceL
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::RemoveServiceL( TInt aIndex )
    {
    CVcxNsService* service = GetServiceData( aIndex );
    if ( service )
        {
        iUiEngine.GetServiceClientHandlerL()->DeleteFeedL( 
                service->GetServiceId() );
        
        for ( TInt i = 0; i < iServiceList.Count(); i++ )
            {
            if ( iServiceList[i]->GetServiceId() 
                    == service->GetServiceId() )
                {
                CVcxNsService* temp = iServiceList[i];
                delete temp;
                iServiceList.Remove( i );
                iServiceList.Compress();
                }
            }

        for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
            {
            if ( iServiceObservers[i] )
                {
                iServiceObservers[i]->ServiceRemoved( aIndex, iServiceList );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Get active service data
// -----------------------------------------------------------------------------
//
CVcxNsService* CVcxNsServiceProvider::GetServiceData( TInt aIndex )
    {
    if ( aIndex >= 0 && 
         iServiceList.Count() > aIndex )
        {
        return iServiceList[ aIndex ];
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::GetServiceDataById()
// -----------------------------------------------------------------------------
//
CVcxNsService* CVcxNsServiceProvider::GetServiceDataById( TUint32 aServiceId )
    {
    TRAPD( leave, GetSubscribedServicesL() );

    if ( leave != KErrNone )
        {
        return NULL;
        }
    
    for( TInt i = 0; i < iServiceList.Count(); i++ )
        {
        if ( ( iServiceList[i])->GetServiceId() == aServiceId )
            {
            return iServiceList[i];
            }
        else if ( iServiceList[i]->Type() == CVcxNsService::EVcxNsMainGroup )
            {
            RPointerArray<CVcxNsService>& group = iServiceList[i]->ServiceGroup();
            for( TInt index = 0; index < group.Count(); index++ )
                {
                if ( group[index]->GetServiceId() == aServiceId )
                    {
                    return group[index];
                    }
                }            
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// Get active service data
// -----------------------------------------------------------------------------
//
CVcxNsService* CVcxNsServiceProvider::GetActiveServiceData()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::GetActiveServiceData");
    return GetServiceDataById( iUiEngine.ActiveService() );    
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::RemoveServiceData
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::RemoveServiceData()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::RemoveServiceData");
    iServiceList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::HandleBack()
// -----------------------------------------------------------------------------
//
TBool CVcxNsServiceProvider::HandleBack()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::HandleBack");
    return iUiEngine.HandleBack();
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::RefreshAllServicesL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::RefreshAllServicesL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::RefreshAllServicesL");
    RPointerArray<CVcxNsService>& servicelist = GetSubscribedServicesL();
    
    if ( servicelist.Count() > 0 && iUpdateQueue.Count() <= 0 )
        {
        if ( !( servicelist[0]->Type() == CVcxNsService::EVcxNsVideoDirectory ) )
            {
            // Start updating first immediately
            TUint32 serviceId = (servicelist[0])->GetServiceId();
            iUiEngine.GetContentClientHandlerL()->UpdateEcgL( serviceId );
            }

        // Mark rest of the services to be pending to be updated later
        for ( TInt i = 1; i < servicelist.Count(); i++ )
            {
            if ( !( servicelist[i]->Type() == CVcxNsService::EVcxNsVideoDirectory ) )
                {
                iUpdateQueue.Append( (servicelist[i])->GetServiceId() );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::CancelUpdateQueue()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::CancelUpdateQueue()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::UpdatesInQueue");
    if ( iUpdateQueue.Count() > 0 )
        {
        iUpdateQueue.Reset();
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::StartManualUpdateL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::StartManualUpdateL( TUint32 aServiceId )
    {
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcxNsServiceProvider::StartManualUpdateL (%d)", (TInt)aServiceId );
    TInt foundIndex = iUpdateQueue.Find( aServiceId );
    
    if ( foundIndex >= 0 && foundIndex < iUpdateQueue.Count() )
        {
        for ( TInt i = 0; i < iServiceList.Count(); i++ )
            {
            if ( (iServiceList[i])->GetServiceId() 
                    == iUpdateQueue[foundIndex] ) 
                {
                UpdateServiceL( (*iServiceList[i] ) );
                iUpdateQueue.Remove( foundIndex );
                return;
                }
            }
        }
    else
        {
        for ( TInt i = 0; i < iServiceList.Count(); i++ )
            {
            if ( ( iServiceList[i])->GetServiceId() 
                    == aServiceId ) 
                {
                UpdateServiceL( (*iServiceList[i]) );
                return;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::UpdateServiceL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::UpdateServiceL( CVcxNsService& aService )
    {
    aService.SetUpdateStatus( CVcxNsService::EStartingPending );
    iUiEngine.GetContentClientHandlerL()->UpdateEcgL( aService.GetServiceId() );
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::SetBackToUpdateQueue()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::SetBackToUpdateQueue( TUint32 aServiceId )
    {
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcxNsServiceProvider::SetBackToUpdateQueue (%d)", (TInt)aServiceId );

    TInt foundIndex = iUpdateQueue.Find( aServiceId );
    if ( foundIndex >= 0 )
        {
        return;
        }
    
    iUpdateQueue.Insert( aServiceId, 0 );
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::OpenVideoDirectoryL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::OpenVideoDirectoryL()
    {
    for ( TInt index = 0; index < iServiceList.Count(); index++ )
        {
        if ( iServiceList[index]->Type() == CVcxNsService::EVcxNsVideoDirectory ) 
            {
            OpenToBrowserL( iServiceList[index]->CustomUri() );
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::ManageAccountL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::ManageAccountL( TInt aSelected )
    {
    if ( aSelected >= 0 && aSelected < iServiceList.Count() )
        {
        CVcxNsService* service = NULL;
        service = GetServiceData( aSelected );
        if ( service )
            {
            for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
                {
                if( iServiceObservers[i] )
                    {
                    iServiceObservers[i]->OpenBrowserLinkL( 
                        service->AccountMgmtUri() );
                    }
                }
            iUiEngine.SetVcAppState( EStateBrowser );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::SetAuthenticationDataL
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::SetAuthenticationDataL( 
    const TDesC& aUserName,
    const TDesC& aPasswd )
    {
    CVcxNsService* activeService = GetActiveServiceData();
    if ( activeService  )
        {
        activeService->SetUserNameL( aUserName );
        activeService->SetPasswordL( aPasswd );
        
        MIptvServiceManagementClientObserver::TRespStatus respStatus;
        CDesC8ArraySeg* services = NULL;
        TUint32 serviceId = activeService->GetServiceId();

        TInt error = 
            iUiEngine.GetServiceClientHandlerL()->GetServiceManagementClientL()->GetServicesL( 
                    serviceId, serviceId, services, respStatus );

        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );
        
        if ( error == KErrNone && services->MdcaCount() > 0 )
            {
            CleanupStack::PushL( services );
            service->SetL( services->MdcaPoint(0) );
            service->SetUserName( aUserName );
            service->SetPassword( aPasswd );
            CleanupStack::PopAndDestroy( services );
            }

        iUiEngine.GetServiceClientHandlerL()->UpdateFeedL( *service );
        CleanupStack::PopAndDestroy( service );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::RegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::RegisterObserver( MVcxNsServiceProviderObserver* aObserver )
    {
    for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
        {
        if( iServiceObservers[i] == aObserver )
            {
            return;
            }
        }   
    iServiceObservers.Append( aObserver );
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::DeRegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::DeRegisterObserver( MVcxNsServiceProviderObserver* aObserver )
    {
    for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
        {
        if( iServiceObservers[i] == aObserver )
            {
            iServiceObservers.Remove( i );
            iServiceObservers.Compress();
            }
        }
   }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::ServiceAdded()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::ServiceAdded( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::ServiceAdded");
    for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
        {
        if( iServiceObservers[i] )
            {
            iServiceObservers[i]->ServiceAdded( aIndex );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::ServiceUpdated()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::ServiceUpdated( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::ServiceUpdated");
    for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
        {
        if( iServiceObservers[i] )
            {
            iServiceObservers[i]->ServiceUpdated( aIndex );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::HandleAppStateChangedL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::HandleAppStateChangedL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::HandleAppStateChangedL");
    for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
        {
        if( iServiceObservers[i] )
            {
            iServiceObservers[i]->HandleAppStateChangedL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::RefreshView()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::RefreshView()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::RefreshView");
    for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
        {
        if( iServiceObservers[i] )
            {
            iServiceObservers[i]->RefreshView();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::UpdateServiceUpdateStatus()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::UpdateServiceUpdateStatusL( 
    TUint32 aServiceId,
    CVcxNsService::TServiceUpdateStatus aStatus )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::UpdateServiceUpdateStatus");
    CVcxNsService* service = GetServiceDataById( aServiceId );

    if ( service )
        {
        service->SetUpdateStatus( aStatus );
        if( aStatus == CVcxNsService::ENotUpdating )
            {
            TTime newUpdateTime;
            TInt error ( KErrNone );
            // This wrapped trap generates false codescanner warning.
            error = iUiEngine.GetContentClientHandlerL()->GetVodContentClientL( 
                      aServiceId )->GetUpdateTimeL( newUpdateTime );

            if( error == KErrNone && newUpdateTime != 0 )
                {
                service->SetUpdateTime( newUpdateTime );
                }
            }
        
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::RefreshNextPendingService()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::RefreshNextPendingService()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::RefreshNextPendingService");
    
    TBool updating( EFalse );

    if ( iUpdateQueue.Count() > 0 )
        {
        TUint32 serviceId = iUpdateQueue[0]; 
        CVcxNsService* service = NULL; 
        for ( TInt i = 0; i < iServiceList.Count(); i++ )
            {
            if ( (iServiceList[i])->GetServiceId() 
                    == serviceId )
                {
                service = iServiceList[i];
                }
            if( iServiceList[i]->GetUpdateStatus() == CVcxNsService::EUpdateOngoing )
                {
                updating = ETrue;
                break;
                }
            }

        if ( service && !updating )
            {
            service->SetUpdateStatus( CVcxNsService::EStartingPending );
            TRAP_IGNORE( iUiEngine.GetContentClientHandlerL()->UpdateEcgL( serviceId ) );
            iUpdateQueue.Remove( 0 );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::UpdateService()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::UpdateService( TUint aServiceId )
    {
    TInt idx = ServiceIndex( aServiceId );
    
    if( idx != KErrNotFound  )
        {
        ServiceUpdated( idx );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::ReloadThumbnailPathL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::ReloadThumbnailPathL( TUint aServiceId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::ReloadThumbnailPathL");
    
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CDesC8ArraySeg* services = NULL;
    TInt error = iUiEngine.GetServiceClientHandlerL()->GetServiceManagementClientL()->
                    GetServicesL( aServiceId, aServiceId, services, respStatus );
    
    if( error == KErrNone && 
        respStatus == MIptvServiceManagementClientObserver::ESucceeded &&
        services )
        {
        CleanupStack::PushL( services );

        if ( services->Count() > 0 )
            {
            CIptvService* service = CIptvService::NewL();
            CleanupStack::PushL( service );
    
            service->SetL( services->MdcaPoint( 0 ) );
    
            if( EnsureFileExists( service->GetIconPath() ) )
                {
                CVcxNsService* updated = GetServiceDataById( aServiceId );
                if ( updated )
                    {
                    updated->SetIconPathL( service->GetIconPath() );
                    }
                }    
            CleanupStack::PopAndDestroy( service );
            }

        CleanupStack::PopAndDestroy( services );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::ServiceIndex()
// -----------------------------------------------------------------------------
//
TInt CVcxNsServiceProvider::ServiceIndex( TUint32 aServiceId )
    {
    for( TInt i = 0; i < iServiceList.Count(); i++ )
        {
        if ( (iServiceList[i])->GetServiceId() == aServiceId )
            {
            return i;
            }
        }
    
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::EnsureFileExists()
// -----------------------------------------------------------------------------
//
TBool CVcxNsServiceProvider::EnsureFileExists( const TDesC& aPath )
    {
    TBool exists( EFalse );
    
    if ( aPath.Length() > 0 )
        {
        exists = BaflUtils::FileExists( iFs, aPath );
        }

    return exists;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::HasFeedDetails()
// -----------------------------------------------------------------------------
//
TBool CVcxNsServiceProvider::HasFeedDetails( TInt aSelected )
    {
    TBool hasDetails (EFalse);
    
    if ( aSelected >= 0 && aSelected < iServiceList.Count() )
        {
        CVcxNsService* service = NULL;
        service = GetServiceData( aSelected );
                
        if ( service->GetDesc() != KNullDesC )
            {
            hasDetails = ETrue;
            }
        }
    return hasDetails;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::MoveServiceL()
// -----------------------------------------------------------------------------
//
TInt CVcxNsServiceProvider::MoveServiceL( TInt aSourceIndex, TInt aTargetIndex )
    {
    if ( aSourceIndex == aTargetIndex ||
         aSourceIndex < 0 || 
         aTargetIndex < 0 ||
         aSourceIndex >= iServiceList.Count() ||
         aTargetIndex >= iServiceList.Count() )
        {
        return KErrArgument;
        }
    
    TUint32 movedId = iServiceList[ aSourceIndex ]->GetServiceId();
    TUint32 dismountedId = iServiceList[ aTargetIndex ]->GetServiceId();
    
    CDesC8ArraySeg* services = NULL;
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    
    CIptvServiceManagementClient* smClient = 
        iUiEngine.GetServiceClientHandlerL()->GetServiceManagementClientL();

    TInt error = smClient->GetServicesL( dismountedId, dismountedId, services, respStatus  );
    
    if ( error != KErrNone || 
         respStatus != MIptvServiceManagementClientObserver::ESucceeded ||
         !services || 
         services->Count() <= 0 )
        {
        return KErrNotFound;
        }
         
    CleanupStack::PushL( services );
    
    CIptvService* service = NULL;
    service = CIptvService::NewL();
    CleanupStack::PushL( service );
    
    service->SetL( services->MdcaPoint( 0 ) );
    TUint32 newDispOrder = service->GetDisplayOrder();
     
    CleanupStack::PopAndDestroy( service );
    CleanupStack::PopAndDestroy( services );
          
    error = smClient->GetServicesL( movedId, movedId, services, respStatus  );
    
    if ( error != KErrNone || 
         respStatus != MIptvServiceManagementClientObserver::ESucceeded ||
         !services || 
         services->Count() <= 0 )
        {
        return KErrNotFound;
        }     
    
    service = CIptvService::NewL();
    CleanupStack::PushL( service );
    CleanupStack::PushL( services );
      
    service->SetL( services->MdcaPoint( 0 ) );
    service->SetDisplayOrder( newDispOrder );
    
    TInt updateResult = smClient->UpdateServiceL( *service, respStatus );
    
    if( updateResult == KErrNone &&
        respStatus == MIptvServiceManagementClientObserver::ESucceeded )
        {
        CVcxNsService* tmp = iServiceList[ aSourceIndex ];
        iServiceList.Remove( aSourceIndex );
        iServiceList.Insert( tmp, aTargetIndex );
        }
    
    CleanupStack::PopAndDestroy( services );
    CleanupStack::PopAndDestroy( service );
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::HasAccountManagementUri()
// -----------------------------------------------------------------------------
//
TBool CVcxNsServiceProvider::HasAccountManagementUri( TInt aSelected )
    {
    TBool hasAccManagementUri( EFalse );
    
    if ( aSelected >= 0 && aSelected < iServiceList.Count() )
        {
        CVcxNsService* service = NULL;
        service = GetServiceData( aSelected );
                
        if ( service && service->AccountMgmtUri().Length() > 0 )
            {
            hasAccManagementUri = ETrue;
            }
        }
    return hasAccManagementUri;
    }

// -----------------------------------------------------------------------------
// Update service in cache ( thumbnail info, update time )
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::UpdateServiceInCacheL( TUint32 aServiceId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsServiceProvider::UpdateServiceInCacheL()");
    
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CDesC8ArraySeg* services = NULL;
    TInt error = iUiEngine.GetServiceClientHandlerL()->GetServiceManagementClientL()->
                    GetServicesL( aServiceId, aServiceId, services, respStatus );
    
    if( error == KErrNone && 
        respStatus == MIptvServiceManagementClientObserver::ESucceeded && 
        services )
        {
        CleanupStack::PushL( services );
        
        if ( services->Count() > 0  )
            {
            CIptvService* service = CIptvService::NewL();
            CleanupStack::PushL( service );
    
            service->SetL( services->MdcaPoint( 0 ) );
    
            CVcxNsService* updated = GetServiceDataById( aServiceId );
            if ( updated )
                {
                updated->SetIconPathL( service->GetIconPath() );
                updated->SetUpdateTime( service->GetEpgUpdateTimeL() );
                }
    
            CleanupStack::PopAndDestroy( service );
            }
        
        CleanupStack::PopAndDestroy( services );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::GetGroupedServicesL()
// -----------------------------------------------------------------------------
//
RPointerArray<CVcxNsService>& CVcxNsServiceProvider::GetGroupedServicesL( 
    TUint32 aGroupId )
    {
    IPTVLOGSTRING_HIGH_LEVEL(
        "UI Engine ## CVcxNsServiceProvider::GetGroupedServicesL( )");

    CVcxNsService* groupService = NULL;
    
    for ( TInt i = 0; i < iServiceList.Count(); i++ )
        {
        if ( iServiceList[i]->GroupId() == aGroupId )
            {
            groupService = iServiceList[i];
            break;
            }
        }
    
    if ( !groupService )
        {
        User::Leave( KErrArgument );
        }

    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CDesC8ArraySeg* services = NULL;
    TUint32 flags = 0;

    TInt error = iUiEngine.GetServiceClientHandlerL()->GetServiceManagementClientL()->GetServicesL( 
                         CIptvServiceManagementClient::EGroupedServices, 
                         CIptvServiceManagementClient::EDisplayOrderDescending,
                         services,
                         respStatus );
    
    if ( error == KErrNone )
        {
        CleanupStack::PushL( services );
        
        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );
        
        for ( TInt i = 0; i < services->Count(); i++ )
            {
            service->SetL( services->MdcaPoint(i) );
            flags = service->GetFlags();
            
            if ( flags & CIptvService::EGroupedService 
                    && service->GetGroupId() == aGroupId )
                {
                CVcxNsService* newService = CVcxNsService::NewL();
                CleanupStack::PushL( newService );
                newService->SetServiceId( service->GetId() );
                newService->SetNameL( service->GetName() );
                newService->SetGroupId( service->GetGroupId() );
                newService->SetCustomUriL( service->GetAddress() );
               
                CIptvService::TServiceType type = service->GetType();
                
                if ( type == CIptvService::EVod || type == CIptvService::EVodCast )
                    {
                    newService->SetType( CVcxNsService::EVcxNsGroupedVod );
                    }
                else if ( type == CIptvService::EBrowser )
                    {
                    newService->SetType( CVcxNsService::EVcxNsGroupedBrowser );
                    }
                else if ( type == CIptvService::ESearch )
                    {
                    newService->SetType( CVcxNsService::EVcxNsGroupedSearch );
                    }
                else if ( type == CIptvService::EUpload )
                    {
                    newService->SetType( CVcxNsService::EVcxNsGroupedUpload );
                    }

                if ( EnsureFileExists( service->GetIconPath() ))
                    {
                    newService->SetIconPathL( service->GetIconPath() );
                    }

                newService->SetUpdateTime( service->GetEpgUpdateTimeL() );
                newService->SetUpdateStatus( CVcxNsService::ENotUpdating );
                newService->SetUserNameL( service->GetUserName() );
                newService->SetPasswordL( service->GetPassword() );
                newService->SetDescL( service->GetDesc() );
                newService->SetIsReadOnlyIap( flags & CIptvService::EReadOnlyIaps ? ETrue : EFalse );
                newService->SetAccountMgmtUriL( service->GetAccountManagementUrl() );
                
                if( flags & CIptvService::EReadOnlyIaps )
                    {
                    // if readonly, more than one?
                    newService->SetIap( service->GetIapList().IapL(0).iId );
                    }
                
                groupService->AddSubServiceL( newService );
                CleanupStack::Pop( newService );
                }
            }
        
        CleanupStack::PopAndDestroy( service );
        CleanupStack::PopAndDestroy( services );
        }
    
    return groupService->ServiceGroup();
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceProvider::OpenServiceFromGroupL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::OpenServiceFromGroupL( TInt aIndex )
    {
    CVcxNsService* activeGroup = NULL;
    activeGroup = GetActiveServiceData();
    if ( activeGroup && 
         activeGroup->Type() == CVcxNsService::EVcxNsMainGroup )
        {
        RPointerArray<CVcxNsService>& groupedServices = activeGroup->ServiceGroup();
        if ( groupedServices.Count() > aIndex )
            {
            CVcxNsService* serviceToOpen = groupedServices[aIndex];
            if ( serviceToOpen )
                {
                CVcxNsService::TVcxNsVodServiceType type = groupedServices[aIndex]->Type();
                if ( type == CVcxNsService::EVcxNsGroupedVod )
                    {
                    iUiEngine.SetActiveServiceGroup( activeGroup->GetServiceId() );
                    iUiEngine.SetActiveService( serviceToOpen->GetServiceId() );
                    RPointerArray<CVcxNsCategory>& categorys = iUiEngine.GetCategoryProviderL()->GetCategorysL();
                    
                    if( categorys.Count() == 0 )
                        {
                        iUiEngine.SetActiveCategory( KIptvVodContentCategoryRootId );
                        iUiEngine.SetVcAppState( EStateContentView );            
                        }
                    else if( categorys.Count() == 1 &&
                             categorys[0]->GetCategoryId() == KIptvRssSearchCategoryId )
                        {
                        iUiEngine.SetActiveCategory( (categorys[0])->GetCategoryId() );
                        iUiEngine.SetVcAppState( EStateCategoryView );
                        }
                    else if( categorys.Count() == 1 )
                        {
                        iUiEngine.SetActiveCategory( (categorys[0])->GetCategoryId() );
                        iUiEngine.SetVcAppState( EStateContentView );
                        }
                    else
                        {
                        iUiEngine.SetVcAppState( EStateCategoryView );
                        }
                    }
                else if ( type == CVcxNsService::EVcxNsGroupedBrowser )
                    {
                    if ( ( serviceToOpen->CustomUri().Length() > 0 ) && 
                         ( serviceToOpen->CustomUri().FindF( KVcxNsRtspProtocol ) == 0 ) )
                        {
                        OpenToPlayerL( serviceToOpen->GetName(), 
                                       serviceToOpen->CustomUri() );
                        }
                    else
                        {
                        OpenToBrowserL( serviceToOpen->CustomUri() );
                        }

                    }
                else if ( type == CVcxNsService::EVcxNsGroupedSearch )
                    {
                    iUiEngine.SetActiveServiceGroup( activeGroup->GetServiceId() );
                    iUiEngine.SetActiveService( serviceToOpen->GetServiceId() );
                    iUiEngine.SetActiveCategory( KIptvRssSearchCategoryId );
                    
                    if ( iUiEngine.GetContentProviderL()->GetContentsL().Count() > 0 )
                        {
                        iUiEngine.SetVcAppState( EStateContentView );
                        iUiEngine.ServiceProvider()->HandleAppStateChangedL();
                        }
                    else
                        {
                        iUiEngine.GetContentSearchHandlerL()->StartNewSearchL();
                        }
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Open link to browser
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::OpenToBrowserL( const TDesC& aUri )
    {
    if ( aUri.Length() > 0 )
        {
        for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
            {
            if( iServiceObservers[i] )
                {
                iServiceObservers[i]->OpenBrowserLinkL( aUri );
                }
            }
        iUiEngine.SetVcAppState( EStateBrowser );
        }
    }

// -----------------------------------------------------------------------------
// Open rtsp link to player
// -----------------------------------------------------------------------------
//
void CVcxNsServiceProvider::OpenToPlayerL( const TDesC& /*aName*/, const TDesC& aUri )
    {
    if ( !( aUri.FindF( KVcxNsRtspProtocol ) == 0 ) )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        /***
         * Send player custom message to UI and start playing:
         * iIcon = Service icon
         ***/
        for ( TInt i = 0; i < iServiceObservers.Count(); i++ )
            {
            if( iServiceObservers[i] )
                {
                iServiceObservers[i]->OpenStreamingLinkL( aUri );
                }
            }     
        }
    }


