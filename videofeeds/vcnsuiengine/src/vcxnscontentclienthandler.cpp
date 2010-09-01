/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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




#include <bldvariant.hrh>
#include "IptvDebug.h"
#include "CIptvUtil.h"
#include <ipvideo/vcxconnectionutility.h>

#include "CIptvVodContentClient.h"
#include "vcxnscontentclienthandler.h"
#include "vcxnsuiengine.h"
#include "vcxnseventrouter.h"
#include "vcxnsserviceprovider.h"
#include "vcxnsservice.h"

// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::CVcxNsContentClientHandler()
// -----------------------------------------------------------------------------
//
CVcxNsContentClientHandler::CVcxNsContentClientHandler( 
    CVcxNsUiEngine& aUiEngine,
    CVcxConnectionUtility& aConnUtil )
:   iUiEngine( aUiEngine ),
    iConnUtil ( aConnUtil )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsContentClientHandler* CVcxNsContentClientHandler::NewL( 
    CVcxNsUiEngine& aUiEngine,
    CVcxConnectionUtility& aConnUtil )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsContentClientHandler::NewL()");
    
    CVcxNsContentClientHandler* self = 
        new (ELeave) CVcxNsContentClientHandler( aUiEngine, aConnUtil );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;    
    }
    
// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentClientHandler::ConstructL()
    {
    iConnUtil.RegisterObserverL( this );	
    }

// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::~CVcxNsContentClientHandler()
// -----------------------------------------------------------------------------
//
CVcxNsContentClientHandler::~CVcxNsContentClientHandler()
    {
    iVodContentClients.ResetAndDestroy(); 
    
    iVodContentClientIds.Reset();
   
    iIapAsked.Reset();
    
    iConnUtil.RemoveObserver( this );
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::GetVodContentClientL()
// -----------------------------------------------------------------------------
//
CIptvVodContentClient* CVcxNsContentClientHandler::GetVodContentClientL( TUint32 aServiceId )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsContentClientHandler::GetVodContentClientL()");

    CIptvVodContentClient* vodContentClient = NULL;

    // Check if content client for this service ID already exists.
    for ( TInt i = 0; i < iVodContentClientIds.Count(); i++ )
        {
        if ( iVodContentClientIds[i] == aServiceId )
            {
            vodContentClient = iVodContentClients[i];
            break;
            }
        }

    // Create client if necessary.
    if ( !vodContentClient )
        {
        vodContentClient = CIptvVodContentClient::NewL( aServiceId, *this );
       
        iVodContentClients.AppendL( vodContentClient );
        iVodContentClientIds.AppendL( aServiceId );
        //Set this to ETrue by default - if user cancels in connection
        //query, then set to EFalse
        iIapAsked.AppendL( ETrue );
        }

    return vodContentClient;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::UpdateEcgL()
// -----------------------------------------------------------------------------
//
TInt CVcxNsContentClientHandler::UpdateEcgL( TUint32 aServiceId )
    {
    CIptvVodContentClient* client = GetVodContentClientL( aServiceId );
    
    if ( !( IapAsked( aServiceId ) ) )
        {
        HandleEpgManagerMsgL( KIptvErrorVodNoIap, 0, aServiceId );
        }
        
    return client->UpdateEcgL();
    }

// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::StoreMpxIdToEcgDbL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentClientHandler::StoreMpxIdToEcgDbL( TUint32 aServiceId,
                                                     TUint32 aContentId,
                                                     TUint32 aCaIndex,
                                                     TUint32 aMpxId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentClientHandler::StoreMpxIdToEcgDbL");

    GetVodContentClientL(aServiceId)->SetMpxIdL( aContentId, aCaIndex, aMpxId );
    }

// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::StoreLastPlayPosL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentClientHandler::StoreLastPlayPosL( TUint32 aServiceId,
                                                    TUint32 aContentId,
                                                    TUint32 aCaIndex,
                                                    TReal32 aPos )
    {
    TUint32 pos( aPos );
    
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsContentClientHandler::StoreLastPlayPosL ( %d )", pos );

    GetVodContentClientL(aServiceId)->SetLastPositionL( aContentId, aCaIndex, pos );
    }

// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::HandleEpgManagerMsgL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentClientHandler::HandleEpgManagerMsgL( 
    TInt aMsg, 
    TInt aInfo, 
    TIptvServiceId aServiceId )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsContentClientHandler::HandleEpgManagerMsgL()");

    TInt error( KErrNone );
    
    if( aMsg == KIptvErrorVodNoIap || aMsg == KIptvErrorRssSearchNoIap &&
          ( iUiEngine.VcAppState() == EStateServiceView ||
            iUiEngine.VcAppState() == EStateCategoryView ||
            iUiEngine.VcAppState() == EStateContentView ||
            iUiEngine.VcAppState() == EStateCustomView ) )
        {
        TUint32 iap( 0 );
        error = iConnUtil.GetIap( iap, EFalse );

        if ( error == KErrNone ) 
            {
            GetVodContentClientL( aServiceId )->SetIapL( iap );
            SetIapAsked( aServiceId, ETrue );
            }
        else
            {
            GetVodContentClientL( aServiceId )->CancelUpdate();
            SetIapAsked( aServiceId, EFalse );
            
            if ( iUiEngine.ServiceProvider() )
                {
                CVcxNsService* service = NULL;
                
                service = iUiEngine.ServiceProvider()->GetServiceDataById( aServiceId );
                
				if ( service )
                    {
                    service->SetUpdateStatus( CVcxNsService::ENotUpdating );
                    }
                
                iUiEngine.ServiceProvider()->CancelUpdateQueue();
                }
            }
        }
    if( error == KErrNone )
        {
        iUiEngine.GetEventRouter()->HandleEpgManagerMsgL( aMsg, aInfo, aServiceId );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::RequestIsRoamingAllowedL()
// -----------------------------------------------------------------------------
//
TBool CVcxNsContentClientHandler::RequestIsRoamingAllowedL()
    {
    // we accept roaming every time and let the
    // update decide whether to use new iap or not
    // when new iap is available
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::IapChangedL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentClientHandler::IapChangedL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsContentClientHandler::IapChangedL() enter");
 
    // Find out if any of the services is currently updating to make sure
    // GetIap() is only called when really needed to avoid unnecessary
    // IAP selection dialog.
    RPointerArray<CVcxNsService> servicesList =
        iUiEngine.GetServiceProviderL()->GetSubscribedServicesL();

    TBool updateOngoing( EFalse );
    const TInt serviceCount = servicesList.Count();

    IPTVLOGSTRING2_HIGH_LEVEL("UI Engine ## CVcxNsContentClientHandler::IapChangedL() service count: %d", serviceCount );

    for ( TInt i=0; i < serviceCount && !updateOngoing; i++ )
        {
        if ( servicesList[i]->GetUpdateStatus() == CVcxNsService::EUpdateOngoing )
            {
            IPTVLOGSTRING2_HIGH_LEVEL("UI Engine ## CVcxNsContentClientHandler::IapChangedL() service index %d is updating", i );
            updateOngoing = ETrue;
            }
        }

    IPTVLOGSTRING2_HIGH_LEVEL("UI Engine ## CVcxNsContentClientHandler::IapChangedL() updateOngoing: %d", updateOngoing );

    if ( updateOngoing )
        {
        // connectionutility notifies that iap has changed.
        // get new iap and save it to all services.
        // update server decides whether to use new iap or not
        TInt count( iVodContentClients.Count() );
        TUint32 iap( 0 );
        TInt err =  iConnUtil.GetIap( iap, EFalse ); 
        if( err != KErrNone )
            {
            for ( TInt i = 0; i < count; i++ )
                {
                ( iVodContentClients[i] )->SetIapL( iap );        
                }    
            }
        }

    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsContentClientHandler::IapChangedL() return");
    }

// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::IapAsked
// -----------------------------------------------------------------------------
//
TBool CVcxNsContentClientHandler::IapAsked( TUint32 aServiceId )
    {
    for ( TInt i = 0; i < iVodContentClientIds.Count(); i++ )
        {
        if ( iVodContentClientIds[i] == aServiceId )
            {
            return iIapAsked[i];
            }
        }    
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentClientHandler::SetIapAsked
// -----------------------------------------------------------------------------
//
void CVcxNsContentClientHandler::SetIapAsked( TUint32 aServiceId, TBool aSet )
    {
    for ( TInt i = 0; i < iVodContentClientIds.Count(); i++ )
        {
        if ( iVodContentClientIds[i] == aServiceId 
             && iIapAsked.Count() > i )
            {
            iIapAsked[i] = aSet;
            }
        }    
    }

