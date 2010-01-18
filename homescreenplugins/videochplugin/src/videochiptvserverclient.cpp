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
* Description:    CVcxNsChIptvServerClient class definition*
*/


#include <e32cmn.h>
#include "videochiptvserverclient.h"
#include "videochmcategorylistener.h"
#include "CIptvServiceManagementClient.h"
#include "videochpublishabledata.h"
#include "videochpublisher.h"
#include "videoahpluginconstants.h"

// -----------------------------------------------------------------------------
// CVcxNsChIptvServerClient::CVcxNsChIptvServerClient
// -----------------------------------------------------------------------------
//
CVcxNsChIptvServerClient::CVcxNsChIptvServerClient( CVcxNsChPublisher* aPublisher,
                                                    CVcxNsChPublishableData* aPublishableData ) :
    iPublishableData( aPublishableData ),
    iPublisher( aPublisher )                                                    
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CVcxNsChIptvServerClient::~CVcxNsChIptvServerClient
// -----------------------------------------------------------------------------
//
CVcxNsChIptvServerClient::~CVcxNsChIptvServerClient()
    {
    delete iSmClient;
    delete iAhCategoryListener;
    }

// -----------------------------------------------------------------------------
// CVcxNsChIptvServerClient::NewL
// -----------------------------------------------------------------------------
//
CVcxNsChIptvServerClient* CVcxNsChIptvServerClient::NewL( 
        CVcxNsChPublisher* aPublisher,
        CVcxNsChPublishableData* aPublishableData )
    {
    CVcxNsChIptvServerClient* self = new (ELeave)CVcxNsChIptvServerClient( aPublisher,
                                                                           aPublishableData );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsChIptvServerClient::ConstructL
// -----------------------------------------------------------------------------
//
void CVcxNsChIptvServerClient::ConstructL()
    {
    TUid domain      = TUid::Uid( KVcxNsAhMediatorDomain );
    TUid category    = TUid::Uid( KVcxNsAhMediatorCategory );
    TVersion version = TVersion( KVcxNsAhEventVersion, 0, 0 );
        
    iAhCategoryListener = CVcxNsChMCategoryListener::NewL( this,
                                                           domain,
                                                           category,
                                                           version );

    TInt event = TInt( KVcxNsAhEventSuiteLoaded );
    iAhCategoryListener->RegisterAndSubscribeL( event );
        
    event = TInt( KVcxNsAhEventSuiteUnloaded );
    iAhCategoryListener->RegisterAndSubscribeL( event );
    }

// -----------------------------------------------------------------------------
// CVcxNsChIptvServerClient::HandleSmEvent
// -----------------------------------------------------------------------------
//
void CVcxNsChIptvServerClient::HandleSmEvent( CIptvSmEvent& aEvent )
    {
    if( aEvent.iEvent == CIptvSmEvent::EServiceDeleted || 
        aEvent.iEvent == CIptvSmEvent::EServiceAdded )
        {
        TInt id( KErrNotFound );
        
        TRAPD( err, id = iPublishableData->GetIntDataL( EVCxNsCPContentServiceGroup,
                                                        TPtrC8( KTvVideoId ) ) );
        
        if( err == KErrNone && id == aEvent.iServiceId )
            {
            if( aEvent.iEvent == CIptvSmEvent::EServiceDeleted )
                {
                // hide item:
                // first backup then delete content
                TRAPD( err, iPublisher->BackupContentL( EVCxNsCPContentServiceGroup ) );
                if( err == KErrNone )
                    {
                    TRAP_IGNORE( iPublisher->DeleteContentL( EVCxNsCPContentServiceGroup ) );
                    }
                }
            else 
                {
                // show item, just restore from backup                
                TRAPD( err, iPublisher->RestoreBackupL( EVCxNsCPContentServiceGroup ) );
                if( err == KErrNone )
                    {
                    TRAP_IGNORE( iPublisher->PublishContentL( EVCxNsCPContentServiceGroup ) );
                    }
                }
            }        
        }
    // other events do not require our attention
    }

// -----------------------------------------------------------------------------
// CVcxNsChIptvServerClient::StartIPTVServerInterfaceL
// -----------------------------------------------------------------------------
//
void CVcxNsChIptvServerClient::StartIPTVServerInterfaceL()
    {
    if( !iSmClient )
        {
        iSmClient = CIptvServiceManagementClient::NewL( *this );
        }
    }
    
// -----------------------------------------------------------------------------
// CVcxNsChIptvServerClient::StopIPTVServerInterface
// -----------------------------------------------------------------------------
//
void CVcxNsChIptvServerClient::StopIPTVServerInterface()
    {
    if( iSmClient )
        {
        delete iSmClient;
        iSmClient = 0;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsChIptvServerClient::GetServiceGroupIdL
// -----------------------------------------------------------------------------
//
TInt CVcxNsChIptvServerClient::GetServiceGroupIdL( const TDesC& aServiceUri )
    {
    User::LeaveIfNull( iSmClient );
    
    TInt id( KErrNotFound );
    if( aServiceUri.Length() <= 0 )
        {
        return id;
        }    
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    CDesC8ArraySeg* services = NULL;
    TInt error = iSmClient->GetServicesL( 0,
                                          CIptvServiceManagementClient::EDisplayOrderAscending,
                                          services,
                                          respStatus );  
    
    if ( error == KErrNone && 
         respStatus == MIptvServiceManagementClientObserver::ESucceeded )
        {
        CleanupStack::PushL( services );
        
        CIptvService* service = CIptvService::NewL();
        CleanupStack::PushL( service );
        TInt count( services->Count() );
        for ( TInt i = 0; i < count; i++ )
            {
            service->SetL( services->MdcaPoint(i) );
            
            if( !aServiceUri.CompareF( service->GetAddress() ) )
                {
                id = service->GetId();  
                
                // service found, make sure it's subscribed                       
                if ( !( service->GetFlags() & CIptvService::ESelected ) )
                    {
                    // factory settings changed to point service 
                    // that is setted as unsubscribed.
                    // save id, backup data and remove menu content
                    iPublishableData->SetDataL( EVCxNsCPContentServiceGroup,
                                                 TPtrC8( KTvVideoId ), id );
                    iPublisher->BackupContentL( EVCxNsCPContentServiceGroup, EFalse );
                    iPublisher->DeleteContentL( EVCxNsCPContentServiceGroup );
                    id = KErrNotFound;
                    }
                break;
                }
            }
        CleanupStack::PopAndDestroy( service );
        CleanupStack::PopAndDestroy( services );
        } 

    return id;
    }

// -----------------------------------------------------------------------------
// CVcxNsChIptvServerClient::MediatorEventL
// -----------------------------------------------------------------------------
//
void CVcxNsChIptvServerClient::MediatorEventL( TUid aDomain,
                                               TUid aCategory, 
                                               TInt aEventId, 
                                               const TDesC8& /* aData */ )
    {
    if( aDomain    == TUid::Uid( KVcxNsAhMediatorDomain ) &&
        aCategory  == TUid::Uid( KVcxNsAhMediatorCategory ) )
        {
        if( aEventId == KVcxNsAhEventSuiteLoaded )
            {
            StartIPTVServerInterfaceL();
            }
        if( aEventId == KVcxNsAhEventSuiteUnloaded )
            {
            StopIPTVServerInterface();
            }
        }
    }
