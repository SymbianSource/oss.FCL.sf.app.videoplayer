/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Provides interface to Service Management and Network Selection*
*/




// INCLUDE FILES
#include <e32svr.h>
#include "IptvDebug.h"

#include "IptvClientServerCommon.h"
#include "CIptvService.h"
#include "CIptvServices.h"
#include "CIptvServiceManagementClient.h"
#include "MIptvServiceManagementClientObserver.h"
#include "CIptvServiceManager.h"
#include "IptvServer.pan"
#include "CIptvSmEvent.h"
#include "CIptvEventListener.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CIptvServiceManagementClient* CIptvServiceManagementClient::NewL(MIptvServiceManagementClientObserver& aClient)
    {
    CIptvServiceManagementClient* self =
        new ( ELeave ) CIptvServiceManagementClient(aClient);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return( self ) ;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::ConstructL()
    {
    User::LeaveIfError( iSession.Connect() );
    iMsg = NULL;

    CIptvSmEvent* event = CIptvSmEvent::NewL();
    CleanupStack::PushL(event); // 1->
    iEventListener = CIptvEventListener::NewL( *this,
                                               event /* ownership moves */,
                                               EIptvEngineSmGetEventSizeReq,
                                               EIptvEngineSmGetEventData,
                                               EIptvEngineSmCancelGetEventSizeReq );
    CleanupStack::Pop(event); // <-1
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::CIptvServiceManagementClient()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
EXPORT_C CIptvServiceManagementClient::CIptvServiceManagementClient(MIptvServiceManagementClientObserver& aClient)
: CIptvClientBase(), iClient(aClient), iMsgPtr((unsigned char*)0, 0) 
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::~CIptvServiceManagementClient()
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvServiceManagementClient::~CIptvServiceManagementClient()
    {
    IPTVLOGSTRING_LOW_LEVEL("~CIptvServiceManagementClient()");

    delete iEventListener;
    
    Cancel(); // Causes call to DoCancel() if there is message pending

    iSession.Close();
                    
    delete iMsg;
    iMsg = NULL;
    IPTVLOGSTRING_LOW_LEVEL("~CIptvServiceManagementClient() exit");    	
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::AddServiceReq()
// Adds a service to Service data base.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvServiceManagementClient::AddServiceReqL(CIptvService& aService)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::AddServiceReq()");

    if ( IsActive() )
        {
        return KErrNotReady;
        }

    BuildAddServiceReqL( aService );

    //send it
    iSession.SendRequest( EIptvEngineSmAddServiceReq, iMsgPtr, iStatus );
    SetActive();
    iRequestPendingFor = EServiceManagerRequestPending;
    iResponseMsgId = EIptvEngineSmAddServiceResp;
            
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::AddServiceReq() exit");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::AddServiceL()
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CIptvServiceManagementClient::AddServiceL(
    CIptvService& aService,
	MIptvServiceManagementClientObserver::TRespStatus& aRespStatus)
    {
    BuildAddServiceReqL( aService );
    TInt err = iSession.SendRequest( EIptvEngineSmAddServiceReq, iMsgPtr );
    if ( err == KErrNone )
        {
        aRespStatus = GetRespStatusL();
        }
    else
        {
        aRespStatus = MIptvServiceManagementClientObserver::EGeneralError;
        }
    return err;    
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::UpdateServiceReqL()
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CIptvServiceManagementClient::UpdateServiceReqL(CIptvService& aService)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::UpdateServiceReq()");

    if(IsActive())
        {
        return KErrNotReady;
        }

    BuildUpdateServiceReqL(aService);

    //send it
    iSession.SendRequest(EIptvEngineSmUpdateServiceReq, iMsgPtr, iStatus);
    SetActive();
    iRequestPendingFor = EServiceManagerRequestPending;
    iResponseMsgId     = EIptvEngineSmUpdateServiceResp;

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::UpdateServiceReq() exit");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::UpdateServiceL()
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CIptvServiceManagementClient::UpdateServiceL(CIptvService& aService,
                                                          MIptvServiceManagementClientObserver::TRespStatus& aRespStatus)
    {
    BuildUpdateServiceReqL(aService);
    TInt err = iSession.SendRequest(EIptvEngineSmUpdateServiceReq, iMsgPtr);
    if(err == KErrNone)
        {
        aRespStatus = GetRespStatusL();
        }
    else
        {
        aRespStatus = MIptvServiceManagementClientObserver::EGeneralError;
        }

    delete iMsg;
    iMsg = NULL;

    return err;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::GetServicesReq()
// Requests services from the database. The request is done in 2 phases, first
// request gets only data size, second transaction gets the data.
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CIptvServiceManagementClient::GetServicesReqL(TUint32 aSearchLimitFlags,
                                                            TOrder aOrder)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetServicesReqL(aSearchLimitFlags, aOrder)");

    if(IsActive())
        {
        return KErrNotReady;
        }

    BuildGetServicesSizeReqL(aSearchLimitFlags, aOrder);
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: Sending EIptvEngineGetServicesSizeReq");                
    iSession.SendRequest(EIptvEngineSmGetServicesSizeReq, iMsgPtr, iStatus);
    SetActive();
    iRequestPendingFor = EServiceManagerRequestPending;
    iResponseMsgId     = EIptvEngineSmGetServicesSizeResp;

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetServicesReqL(aSearchLimitFlags, aOrder) exit");
    return KErrNone;
    }
    
EXPORT_C TInt CIptvServiceManagementClient::GetServicesL(
    TUint32 aSearchLimitFlags,
    TOrder aOrder,
    CDesC8ArraySeg*& aServicesArray,
    MIptvServiceManagementClientObserver::TRespStatus& aRespStatus )
    {    
    BuildGetServicesSizeReqL( aSearchLimitFlags, aOrder );
    
    TInt err = iSession.SendRequest( EIptvEngineSmGetServicesSizeReq, iMsgPtr );
    if ( err != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: GetServicesL() error when requesting size");
        aRespStatus = MIptvServiceManagementClientObserver::EGeneralError;
        return err;
        }
    else
        {
        RDesReadStream readStream;
        readStream.Open( iMsgPtr);
        CleanupClosePushL( readStream );
        TUint8  status = readStream.ReadUint8L();
        TUint32 size   = readStream.ReadUint32L();    
        CleanupStack::PopAndDestroy( &readStream );
                    
        if ( status != MIptvServiceManagementClientObserver::ESucceeded )
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: GetServicesL() error when requesting size");
            aRespStatus = ( MIptvServiceManagementClientObserver::TRespStatus )status;
            return KErrGeneral;
            }
    
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: Size: %d", size);

        if ( iMsg )
            {
            delete iMsg;
            iMsg = NULL;
            }
        			
        iMsg = HBufC8::NewL( size );
        iMsgPtr.Set( iMsg->Des() );
             
        err = iSession.SendRequest( EIptvEngineSmGetServicesDataReq, iMsgPtr );
        if ( err != KErrNone )
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: EIptvEngineSmGetServicesDataReq failed");
            aRespStatus = MIptvServiceManagementClientObserver::EGeneralError;
            return err;
            }
        else
            {
            CDesC8ArraySeg* services = NULL;
            TRAPD( err, services = InternalizeGetServicesDataRespL() );
            if ( err != KErrNone )
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: failed to internalize GetServicesDataResp ipc msg");
                aRespStatus = MIptvServiceManagementClientObserver::EGeneralError;
                return err;
                }
            aRespStatus    = MIptvServiceManagementClientObserver::ESucceeded;
            aServicesArray = services;
            return KErrNone;
            }
        }
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::GetServicesReqL()
// Gets service entries from Services database. Request is done asynchronously.
// GetServicesResp is the callback function.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvServiceManagementClient::GetServicesReqL(TUint32 aStartId, TUint32 aEndId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetServicesReqL(aStartId, aEndId)");

    if(IsActive())
        {
        return KErrNotReady;
        }

    BuildGetServicesUsingIndexSizeReqL(aStartId, aEndId);
         
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: Sending EIptvEngineGetServicesUsingIdSizeReq");
        
    iSession.SendRequest(EIptvEngineSmGetServicesUsingIdSizeReq, iMsgPtr, iStatus);
    SetActive();
    iRequestPendingFor = EServiceManagerRequestPending;
    iResponseMsgId     = EIptvEngineSmGetServicesUsingIdSizeResp;

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetServicesReqL(aStartId, aEndId) exit");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::GetServices()
// -----------------------------------------------------------------------------
//    		
EXPORT_C TInt CIptvServiceManagementClient::GetServicesL(TUint32 aStartId,
                                                         TUint32 aEndId,
                                                         CDesC8ArraySeg*& aServicesArray,
                                                         MIptvServiceManagementClientObserver::TRespStatus& aRespStatus)
    {
    BuildGetServicesUsingIndexSizeReqL(aStartId, aEndId);
    
    TInt err = iSession.SendRequest(EIptvEngineSmGetServicesUsingIdSizeReq, iMsgPtr);
    if (err != KErrNone)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: Error in SendRequest");
        aRespStatus    = MIptvServiceManagementClientObserver::EGeneralError;
        aServicesArray = NULL;
        return err;
        }

    TUint8  status;
    TUint32 size;
    
    RDesReadStream readStream;
    readStream.Open(iMsgPtr);
    CleanupClosePushL( readStream );
    status = readStream.ReadUint8L();
                    
    if(status != MIptvServiceManagementClientObserver::ESucceeded)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: Error");
        iClient.GetServicesResp((MIptvServiceManagementClientObserver::TRespStatus)status, NULL);
        CleanupStack::PopAndDestroy( &readStream );
        return KErrGeneral;
        }
        
    size = readStream.ReadUint32L();    
    CleanupStack::PopAndDestroy( &readStream );

    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: Size: %d", size);

    if (iMsg)
        {
        delete iMsg;
        iMsg = NULL;
        }
        			
    iMsg = HBufC8::NewL(size);
    iMsgPtr.Set(iMsg->Des());
             
    err = iSession.SendRequest(EIptvEngineSmGetServicesUsingIdDataReq, iMsgPtr);

    if (err != KErrNone)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: EIptvEngineSmGetServicesUsingIdDataReq failed");
        aRespStatus = MIptvServiceManagementClientObserver::EGeneralError;
        return err;
        }
    else
        {
        CDesC8ArraySeg* services = NULL;
    
        TRAPD(err, services = InternalizeGetServicesDataRespL());
        if (err != KErrNone)
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: failed to internalize GetServicesDataResp ipc msg");
            aRespStatus = MIptvServiceManagementClientObserver::EGeneralError;
            return err;
            }
        aRespStatus    = MIptvServiceManagementClientObserver::ESucceeded;
        aServicesArray = services;
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::DeleteServiceReqL()
// Deletes a service from the database.
// -----------------------------------------------------------------------------
//
	
EXPORT_C TInt CIptvServiceManagementClient::DeleteServiceReqL(TUint32 aId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::DeleteServiceReqL()");

    if(IsActive())
        {
        return KErrNotReady;
        }

    BuildDeleteServiceReqL(aId);
        	  
    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: Making delete request for ID: %d", aId);
        
    iSession.SendRequest(EIptvEngineSmDeleteServiceReq, iMsgPtr, iStatus);
    SetActive();
    iRequestPendingFor = EServiceManagerRequestPending;
    iResponseMsgId     = EIptvEngineSmDeleteServiceResp;

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::DeleteServiceReq() exit");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::DeleteServiceL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvServiceManagementClient::DeleteServiceL(TUint32 aId,
                                                          MIptvServiceManagementClientObserver::TRespStatus& aRespStatus)
    {
    BuildDeleteServiceReqL(aId);
    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: Making delete request for ID: %d", aId);
    TInt err = iSession.SendRequest(EIptvEngineSmDeleteServiceReq, iMsgPtr);
    if (err != KErrNone)
        {
        aRespStatus = MIptvServiceManagementClientObserver::EGeneralError;
        }
    else
        {
        aRespStatus = GetRespStatusL();
        }
    return err;
    }
    
// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::ServerShutDownReq()
// Requests server to shutdown itself.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvServiceManagementClient::ServerShutdownReq()
    {
    if(IsActive())
        {
        return KErrNotReady;
        }

    iSession.SendRequest(EIptvEngineServerShutdownReq, iStatus);
    SetActive();
    iRequestPendingFor = EServerRequestPending;
    iResponseMsgId     = EIptvEngineShutdownResp;
    
    return KErrNone;
    }

EXPORT_C TInt CIptvServiceManagementClient::ServerShutdown(MIptvServiceManagementClientObserver::TRespStatus& /*aRespStatus*/)
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::CancelRequest()
// Cancels an outstanding request.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvServiceManagementClient::CancelRequest()
    {
    Cancel() ; // Causes call to DoCancel() if there is message pending
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::RunL()
// Invoked to handle responses from Iptv server.
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::RunL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::RunL()");

    iRequestPendingFor = ENoRequestPending;
    TInt error = iStatus.Int();

    if(error != KErrNone)
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: error received from server: %d", error);
        }
        
    switch ( iResponseMsgId )
        {
        case EIptvEngineShutdownResp:
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: EIptvEngineShutdownResp");
            iClient.ServerShutdownResp(GetRespStatusL());
            break;

        case EIptvEngineSmGetServicesSizeResp:
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: EIptvEngineSmGetServicesSizeResp");
            HandleGetServicesSizeRespL(EIptvEngineSmGetServicesSizeResp);
            break;
        case EIptvEngineSmGetServicesDataResp:
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: EIptvEngineSmGetServicesDataResp");
            HandleGetServicesDataRespL(EIptvEngineSmGetServicesDataResp);
            break;

        case EIptvEngineSmGetServicesUsingIdSizeResp:
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: EIptvEngineSmGetServicesUsingIdSizeResp");
            HandleGetServicesSizeRespL(EIptvEngineSmGetServicesUsingIdSizeResp);
            break;
        case EIptvEngineSmGetServicesUsingIdDataResp:
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: EIptvEngineSmGetServicesUsingIdDataResp");
            HandleGetServicesDataRespL(EIptvEngineSmGetServicesUsingIdDataResp);
            break;
               	        	        	        	        	
        case EIptvEngineSmAddServiceResp:
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: EIptvEngineSmAddServiceResp");
            iClient.AddServiceResp(GetRespStatusL());
            break;
        		
        case EIptvEngineSmDeleteServiceResp:
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: EIptvEngineSmDeleteServiceResp");
            iClient.DeleteServiceResp(GetRespStatusL());
            break;

        case EIptvEngineSmUpdateServiceResp:
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: EIptvEngineSmUpdateServiceResp");
            iClient.UpdateServiceResp(GetRespStatusL());
            break;
        	        
        case EIptvEngineNsGetUsedIapResp:
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: EIptvEngineNsGetUsedIapResp");

            TUint32 iapId;
            TBuf<KIptvNsIapNameMaxLength> iapName;
            CIptvNetworkSelection::TConnectionPermission connPerm;
            CIptvNetworkSelection::TRespStatus respStatus;
            TUint8 wlanWhenGprs;

            if ( iStatus.Int() == KErrNone )
                {
                HandleGetUsedIapRespL( iapId, iapName, connPerm, wlanWhenGprs, respStatus );
                }
            else
                {
                IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManagementClient:: message completed with error: %d", iStatus.Int());
                respStatus = CIptvNetworkSelection::EGeneralError;
                }
            iClient.GetUsedIapResp(iapId, iapName, connPerm, wlanWhenGprs, respStatus);
            }
            break;
            
        default:
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManagementClient:: UNKNOWN MESSAGE, PANICING");
            User::Panic( KIptvServiceManagementClient, EIptvUnknownMessage ); // Unexpected error
            break;
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::RunL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::DoCancel()
// Cancels any outstanding operation.
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::DoCancel()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::DoCancel()");
    
    switch(iRequestPendingFor)
        {
        case EServerRequestPending:
            iSession.SendRequest(EIptvEngineServerCancel); //sync
            break;
            
        case EServiceManagerRequestPending:
            iSession.SendRequest(EIptvEngineSmCancel); //sync
            break;
            
        case ENetworkSelectionRequestPending:
            {
            TInt err = iSession.SendRequest(EIptvEngineNsCancel); //sync
            if(err == KErrNone)
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: EIptvEngineNsCancel successful"); 
                }
            else
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: EIptvEngineNsCancel failed"); 
                }
            }
            break;
            
        default:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: there was no message pending, ignoring cancel req");
            break;
        }
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::DoCancel() exit");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::HandleGetServicesSizeRespL()
// Handles EIptvEngineGetServicesSizeResp and EIptvEngineGetServicesUsingIdSizeResp
// messages.
// Allocates new iMsg of the size fetched from the incoming message and sends
// corresponding data request message to server.
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::HandleGetServicesSizeRespL(TUint8 aMsgId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::HandleGetServicesSizeResp()");

    if(iStatus.Int() != KErrNone)
        {
        //operation leaved on server side -> mark request as failed
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: operation leaved on server side->EGeneralError");
        iClient.GetServicesResp(MIptvServiceManagementClientObserver::EGeneralError, NULL);
        return;
        }

    TUint8  status;
    TUint32 size;
    
    RDesReadStream readStream;
    readStream.Open(iMsgPtr);
    CleanupClosePushL( readStream );
    status = readStream.ReadUint8L();
                    
    if(status != MIptvServiceManagementClientObserver::ESucceeded)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: Error");
        iClient.GetServicesResp((MIptvServiceManagementClientObserver::TRespStatus)status, NULL);
        CleanupStack::PopAndDestroy( &readStream );
        return;
        }
        
    size = readStream.ReadUint32L();    
    CleanupStack::PopAndDestroy( &readStream );

    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: Size: %d", size);

    delete iMsg;
    iMsg = NULL;
    iMsg = HBufC8::NewL(size);
    iMsgPtr.Set(iMsg->Des());
             
    switch(aMsgId)
        {
        case EIptvEngineSmGetServicesSizeResp:
            iSession.SendRequest(EIptvEngineSmGetServicesDataReq, iMsgPtr, iStatus);
            iResponseMsgId = EIptvEngineSmGetServicesDataResp;
            break;
        case EIptvEngineSmGetServicesUsingIdSizeResp:
            iSession.SendRequest(EIptvEngineSmGetServicesUsingIdDataReq, iMsgPtr, iStatus);
            iResponseMsgId = EIptvEngineSmGetServicesUsingIdDataResp;
            break;
        }
        
    SetActive();
    iRequestPendingFor = EServiceManagerRequestPending;
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::HandleGetServicesSizeRespL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::HandleGetServicesDataReqCompletes()
// Handles EIptvEngineGetServicesDataResp and EIptvEngineGetServicesUsingIdDataResp
// messages.
// Unpacks data from stream format(externalize/internalize) and sends services to
// client as an array of data descriptors fetched from CIptvService::GetL().
// -----------------------------------------------------------------------------
//    
void CIptvServiceManagementClient::HandleGetServicesDataRespL(TUint8 aMsgId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::HandleGetServicesDataRespL()");	

    if(iStatus.Int() != KErrNone)
        {
        //operation leaved on server side -> mark request as failed
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: operation leaved on server side->EGeneralError");
        iClient.GetServicesResp(MIptvServiceManagementClientObserver::EGeneralError, NULL);
        return;
        }

    CDesC8ArraySeg* services = NULL;
    
    TRAPD(err, services = InternalizeGetServicesDataRespL());
    if (err != KErrNone)
        {
        iClient.GetServicesResp(MIptvServiceManagementClientObserver::EGeneralError, NULL);
        return;
        }
                
    switch(aMsgId)
        {
        case EIptvEngineSmGetServicesDataResp:
            iClient.GetServicesResp(MIptvServiceManagementClientObserver::ESucceeded, services);
            break;
        case EIptvEngineSmGetServicesUsingIdDataResp:
            iClient.GetServicesResp(MIptvServiceManagementClientObserver::ESucceeded, services);
            break;
        default:
            break;
        }
      
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::HandleGetServicesDataRespL() exit");	
    }
    
// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::SetUsedIapL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvServiceManagementClient::SetUsedIapL(TUint32 aIapId,
                                                        TUint32 aServiceId,
                                                        TBool aSetToDefaultForService,
                                                        CIptvNetworkSelection::TRespStatus& aRespStatus)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::SetUsedIapL()");

    if(IsActive())
        {
        User::Leave(KErrNotReady);
        }

    delete iMsg;
    iMsg = NULL;

    /* EIptvEngineNsSetUsedIap message format:
     * -------------------------------
     * TUint32 aIapId
     * TUint32 aServiceId
     * TUint8  aSetToDefaultForService
     * -------------------------------
     */
         
    /* EIptvEngineNsSetUsedIap response message format
     * --------------------
     * TUint8 aRespStatus
     * --------------------
     */
        	  
    iMsg = HBufC8::NewL(4+4+1);
    iMsgPtr.Set(iMsg->Des());
        
    RDesWriteStream writeStream;
    writeStream.Open(iMsgPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(aIapId);
    writeStream.WriteUint32L(aServiceId);
    writeStream.WriteUint8L(static_cast<TUint8>(aSetToDefaultForService));
    writeStream.WriteUint8L(static_cast<TUint8>(aRespStatus));        
    CleanupStack::PopAndDestroy( &writeStream );
    TInt err( KErrNotReady );
    if ( !iSession.IsNull() )
        {
        err = iSession.SendRequest(EIptvEngineNsSetUsedIap, iMsgPtr);    
        }
    
    if(err == KErrNone)
        {    
        RDesReadStream readStream;
        readStream.Open(iMsgPtr);
        CleanupClosePushL( readStream );
        aRespStatus = static_cast<CIptvNetworkSelection::TRespStatus>(readStream.ReadUint8L());
        CleanupStack::PopAndDestroy( &readStream );
        }
    else
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: SendRequest(EIptvEngineNsSetUsedIap) failed, error code = %d", err);
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: aRespStatus set to CIptvNetworkSelection::EGeneralError");
        aRespStatus = CIptvNetworkSelection::EGeneralError;
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::SetUsedIapL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::ClearUsedIap()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvServiceManagementClient::ClearUsedIap()
    {
    return iSession.SendRequest(EIptvEngineNsClearUsedIap);    
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::GetUsedIapReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvServiceManagementClient::GetUsedIapReqL( TUint32 aServiceId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetUsedIapReqL()");

    if ( IsActive() )
        {
        User::Leave( KErrNotReady );
        }
    BuildGetUsedIapReqL( aServiceId );    
        	          
    iSession.SendRequest(EIptvEngineNsGetUsedIapReq, iMsgPtr, iStatus);
    SetActive();
    iRequestPendingFor = ENetworkSelectionRequestPending;
    iResponseMsgId     = EIptvEngineNsGetUsedIapResp;

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetUsedIapReqL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::GetUsedIapL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvServiceManagementClient::GetUsedIapL( TUint32 aServiceId,
                                                         TUint32& aIapId,
                                                         TDes& aIapName,
                                                         CIptvNetworkSelection::TConnectionPermission& aConnectionPermission,
                                                         CIptvNetworkSelection::TRespStatus& aRespStatus )
    {
    BuildGetUsedIapReqL( aServiceId );

    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: sending EIptvEngineNsGetUsedIapReq for service %d", aServiceId);
    TInt err = iSession.SendRequest( EIptvEngineNsGetUsedIapReq, iMsgPtr );
    
    if ( err != KErrNone )
        {
        User::Leave( err );
        }
    else
        {
        TUint8 temp;            // Not used syncronous call
        HandleGetUsedIapRespL( aIapId, aIapName, aConnectionPermission, temp, aRespStatus );
        }
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::IsConnectionAllowedL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvServiceManagementClient::IsConnectionAllowedL(TUint32 aServiceId,
                                                                 CIptvNetworkSelection::TConnectionPermission& aConnectionPermission,
                                                                 CIptvNetworkSelection::TRespStatus& aRespStatus)
    {
        
    if(IsActive())
        {
        User::Leave(KErrNotReady);
        }

    delete iMsg;
    iMsg = NULL;
        
    /* EIptvEngineNsIsConnectionAllowed message format:
     * -------------------
     * TUint32 aServiceId   
     * -------------------
     */

    /* EIptvEngineNsIsConnectionAllowed response message format:
     * -----------------------------
     * TUint8  aRespStatus
     * TUint8  aConnectionPermission
     * -----------------------------
     */
	  
    iMsg = HBufC8::NewL(4);         
    iMsgPtr.Set(iMsg->Des());
        
    RDesWriteStream writeStream;
    writeStream.Open(iMsgPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(aServiceId);
    CleanupStack::PopAndDestroy( &writeStream );
        	          
    TInt error  = iSession.SendRequest(EIptvEngineNsIsConnectionAllowed, iMsgPtr);
    if(error == KErrNone)
        {
        RDesReadStream readStream;
        readStream.Open(iMsgPtr);
        CleanupClosePushL( readStream );
        aRespStatus           = static_cast<CIptvNetworkSelection::TRespStatus>(readStream.ReadUint8L());
        aConnectionPermission = static_cast<CIptvNetworkSelection::TConnectionPermission>(readStream.ReadUint8L());
        CleanupStack::PopAndDestroy( &readStream );
        }
    else
        {
        aRespStatus = CIptvNetworkSelection::EGeneralError;
        }

    User::LeaveIfError(error);
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::SetConnectionAllowedL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvServiceManagementClient::SetConnectionAllowedL(TBool aConnectionAllowed,
                                                                  TUint32 aIapId,
                                                                  CIptvNetworkSelection::TRespStatus& aRespStatus)
    {

    if(IsActive())
        {
        User::Leave(KErrNotReady);
        }

    delete iMsg;
    iMsg = NULL;
        
    /* EIptvEngineNsSetConnectionAllowed message format:
     * --------------------------
     * TUint8  aConnectionAllowed
     * TUint32 aIapId
     * --------------------------
     */
         
    /* response message format:
     * -------------------
     * TUint8 aRespStatus
     * -------------------
     */
	  
    iMsg = HBufC8::NewL(1+4);         
    iMsgPtr.Set(iMsg->Des());
        
    RDesWriteStream writeStream;
    writeStream.Open(iMsgPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint8L(aConnectionAllowed);
    writeStream.WriteUint32L(aIapId);
    CleanupStack::PopAndDestroy( &writeStream );
        
    TInt error = iSession.SendRequest(EIptvEngineNsSetConnectionAllowed, iMsgPtr);
    
    if(error == KErrNone)
        {
        RDesReadStream readStream;
        readStream.Open(iMsgPtr);
        CleanupClosePushL( readStream );
        aRespStatus = static_cast<CIptvNetworkSelection::TRespStatus>(readStream.ReadUint8L());
        CleanupStack::PopAndDestroy( &readStream );
        }
    else
        {
        aRespStatus = CIptvNetworkSelection::EGeneralError;
        }
        
    User::LeaveIfError(error);
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::GetRespStatusL()
// -----------------------------------------------------------------------------
//
MIptvServiceManagementClientObserver::TRespStatus CIptvServiceManagementClient::GetRespStatusL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetRespStatusL()");

    if(iStatus.Int() != KErrNone)
        {
        //operation leaved on server side -> mark request as failed
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: operation leaved on server side (error code = %d)->EGeneralError", iStatus.Int());
        return MIptvServiceManagementClientObserver::EGeneralError;
        }

    RDesReadStream readStream;
    readStream.Open(iMsgPtr);
    CleanupClosePushL( readStream );
    MIptvServiceManagementClientObserver::TRespStatus respStatus = MIptvServiceManagementClientObserver::EGeneralError;
    
    TRAPD(err,
    respStatus = static_cast<MIptvServiceManagementClientObserver::TRespStatus>(readStream.ReadUint8L());
    );
    
    if (err != KErrNone)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: failed to read status!");
        respStatus = MIptvServiceManagementClientObserver::EGeneralError;
        }
        
    CleanupStack::PopAndDestroy( &readStream );

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetRespStatusL() exit");
    return respStatus;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::GetServicesL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvServiceManagementClient::GetServicesL(const TDesC& aProviderId,
                                                         CIptvServices& aServices)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetServicesL(aProviderId)");

    if(IsActive())
        {
        return KErrNotReady;
        }

    /* EIptvEngineSmGetServiceSizeUsingProviderId client->server message format:
     * ------------------------------------
     * TUint32 aProviderIdLength
     * TBuf<aProviderIdLength>  aProviderId
     * ------------------------------------
     */

    /* EIptvEngineSmGetServiceSizeUsingProviderId server->client message format:
     * -------------------------------
     * TUint32 aSize
     * -------------------------------
     */
         
    /* EIptvEngineSmGetServiceDataUsingProviderId client->server message format:
     * --------------------
     * --------------------
     */

    /* EIptvEngineSmGetServiceDataUsingProviderId server->client message format:
     * ---------------------------------------------
     * CIptvServices object in binary format 
     * ---------------------------------------------
     */

    //build EIptvEngineSmGetServiceSizeUsingProviderId client->server        	  
    HBufC8* ipcMsg = HBufC8::NewL(4 + aProviderId.Length()*2);
    CleanupStack::PushL(ipcMsg); 
    TPtr8 ipcMsgPtr(ipcMsg->Des());
    ipcMsgPtr.Zero();    
    
    RDesWriteStream writeStream;
    writeStream.Open(ipcMsgPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(aProviderId.Length());
    TInt i;
    for(i = 0; i < aProviderId.Length(); i++)
        {
        writeStream.WriteInt16L(aProviderId[i]);
        }
        
    CleanupStack::PopAndDestroy( &writeStream );
    
    //send size request    
    TInt resp = iSession.SendRequest(EIptvEngineSmGetServiceSizeUsingProviderId, ipcMsgPtr);

    if(resp != KErrNone)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: size resp error");
        CleanupStack::PopAndDestroy(ipcMsg); 
        return resp;
        }
    
    //build data request
    TUint32 dataSize;
    RDesReadStream readStream;
    readStream.Open(ipcMsgPtr);
    CleanupClosePushL( readStream );
    dataSize = readStream.ReadUint32L();
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy(ipcMsg); 
    ipcMsg = NULL;
    ipcMsg = HBufC8::NewL(dataSize);
    CleanupStack::PushL(ipcMsg); 
    ipcMsgPtr.Set(ipcMsg->Des());

    //send data request
    resp = iSession.SendRequest(EIptvEngineSmGetServiceDataUsingProviderId, ipcMsgPtr);    
    
    if(resp != KErrNone)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: data resp error");
        CleanupStack::PopAndDestroy(ipcMsg); 
        return resp;
        }

    //read services data
    readStream.Open(ipcMsgPtr);
    CleanupClosePushL( readStream );
    aServices.InternalizeL(readStream);
    CleanupStack::PopAndDestroy( &readStream );

    CleanupStack::PopAndDestroy(ipcMsg); // <-1

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: Get services using provider id successful.");
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::GetServicesL(aProviderId) exit");
    return resp;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::SetAllIapsReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvServiceManagementClient::SetAllIapsReqL(CIptvIapList& /* aIapList */,
                                                           TBool /*aIgnoreReadOnlyFlag*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::SetAllIapsReqL()");
    
    if(IsActive())
        {
        User::Leave(KErrNotReady);
        }

    delete iMsg;
    iMsg = NULL;

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: !!! Not Supported Yet!!!");
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::SetAllIapsReqL() exit");
    User::Leave(KErrNotSupported); //not supported yet
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::SetAllIapsL()
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvServiceManagementClient::SetAllIapsL(
    CIptvIapList& /*aIapList*/,
    TBool /*aIgnoreReadOnlyFlag*/,
    MIptvServiceManagementClientObserver::TRespStatus& /*aRespStatus*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::SetAllIapsL()");

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient:: !!! Not Supported Yet!!!");    
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::SetAllIapsL() exit");
    User::Leave( KErrNotSupported );
    }
// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::HandleEvent()
// -----------------------------------------------------------------------------
//    
void CIptvServiceManagementClient::HandleEvent( MIptvEvent& aEvent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::HandleEvent()");
    
    //this event is never routed to synchronizer
    iClient.HandleSmEvent( static_cast<CIptvSmEvent&>( aEvent ) );
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManagementClient::HandleEvent() exit");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::BuildAddServiceReqL()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::BuildAddServiceReqL( CIptvService& aService )
    {
    delete iMsg;
    iMsg = NULL;
    iMsg = HBufC8::NewL( aService.CountExternalizeSize() );
    iMsgPtr.Set( iMsg->Des() );
    iMsgPtr.Zero();

    //initialize stream object to use ipc msg as a data buffer 
    RDesWriteStream writeStream;
    writeStream.Open( iMsgPtr );
    CleanupClosePushL( writeStream );
    aService.ExternalizeL( writeStream );
    CleanupStack::PopAndDestroy( &writeStream );
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::BuildUpdateServiceReqL()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::BuildUpdateServiceReqL( CIptvService& aService )
    {
    delete iMsg;
    iMsg = NULL;
    iMsg = HBufC8::NewL( aService.CountExternalizeSize() );
    iMsgPtr.Set( iMsg->Des() );
 
    iMsgPtr.Zero();

    RDesWriteStream writeStream;
        
    //initialize stream object to use ipc msg as a data buffer 
    writeStream.Open( iMsgPtr );
    CleanupClosePushL( writeStream );
    aService.ExternalizeL(writeStream);
    CleanupStack::PopAndDestroy( &writeStream );
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::BuildGetServicesReqL()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::BuildGetServicesSizeReqL(
    TUint32 aSearchLimitFlags,
    TOrder aOrder)
    {
    delete iMsg;
    iMsg = NULL;
    iMsg = HBufC8::NewL(4 /* TUint32 aSearchLimitFlags*/ + 1 /* TUint8 aOrder*/);
    iMsgPtr.Set( iMsg->Des() );
    iMsgPtr.Zero();
 
    RDesWriteStream writeStream;
    writeStream.Open( iMsgPtr );
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L( aSearchLimitFlags );
    writeStream.WriteUint8L( aOrder );
    CleanupStack::PopAndDestroy( &writeStream );
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::InternalizeGetServicesDataRespL()
// -----------------------------------------------------------------------------
//
CDesC8ArraySeg* CIptvServiceManagementClient::InternalizeGetServicesDataRespL()
    {
    RDesReadStream readStream;
    readStream.Open(iMsgPtr); // open
    CleanupClosePushL( readStream );
    TUint8 status = readStream.ReadUint8L();

    if(status != MIptvServiceManagementClientObserver::ESucceeded)
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManagementClient:: get services data req failed: %d", status);	
        CleanupStack::PopAndDestroy( &readStream );
        User::Leave(KErrGeneral);
        }

    TUint32 itemCount = readStream.ReadUint32L();
    TUint32 i;

    CDesC8ArraySeg* stringArray;
    stringArray = new (ELeave) CDesC8ArraySeg(10);			
    CleanupStack::PushL(stringArray); 
    
    CIptvService* service = CIptvService::NewL();
    CleanupStack::PushL(service); 
        
    for(i = 0; i < itemCount; i++)
        {
        service->InternalizeL(readStream);
        HBufC8* serviceData = service->GetL();
        CleanupStack::PushL(serviceData); 
        stringArray->AppendL(serviceData->Des());
        CleanupStack::PopAndDestroy(serviceData); 
        }
    
    CleanupStack::PopAndDestroy(service); 
    CleanupStack::Pop(stringArray); //  don't delete since it's given to caller    
    CleanupStack::PopAndDestroy( &readStream );

    delete iMsg;
    iMsg = NULL;
    
    return stringArray;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::BuildDeleteServiceReqL()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::BuildDeleteServiceReqL(TUint32 aId)
    {
    delete iMsg;
    iMsg = NULL;
    iMsg = HBufC8::NewL(4);
    iMsgPtr.Set(iMsg->Des());
        
    RDesWriteStream writeStream;
    writeStream.Open(iMsgPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(aId);
    CleanupStack::PopAndDestroy( &writeStream );
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::BuildGetServicesUsingIndexSizeReq()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::BuildGetServicesUsingIndexSizeReqL(TUint32 aStartId, TUint32 aEndId)
    {
    delete iMsg;
    iMsg = NULL;
    iMsg = HBufC8::NewL(4 /* TUint32 aStartId */ + 4 /* TUint32 aEndId */);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream writeStream;
    writeStream.Open(iMsgPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(aStartId); //+4
    writeStream.WriteUint32L(aEndId);   //+4
    CleanupStack::PopAndDestroy( &writeStream );
    }    

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::BuildGetUsedIapReqL()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::BuildGetUsedIapReqL( TUint32 aServiceId )
    {    
    delete iMsg;
    iMsg = NULL;

    /* EIptvEngineNsGetUsedIapReq message format:
     * ------------------
     * TUint32 aServiceId
     * ------------------
     */
        
    /* EIptvEngineNsGetUsedIapResp message format:
     * -------------------
     * TUint8  aRespStatus
     * TUint32 aIapId
     * TUint16 aIapNameLength
     * <aIapNameLength * TUint16> aIapNameData (contains SSID if iapname not found)
     * TUint8  aConnectionPermission
     * -------------------
     */

    iMsg = HBufC8::NewL( 1+4+2+(KIptvNsIapNameMaxLength*2)+1 );
    iMsgPtr.Set( iMsg->Des() );
        
    RDesWriteStream writeStream;
    writeStream.Open(iMsgPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L( aServiceId );
    CleanupStack::PopAndDestroy( &writeStream );
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementClient::HandleGetUsedIapRespL()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementClient::HandleGetUsedIapRespL( TUint32& aIapId,
                                                          TDes& aIapName,
                                                          CIptvNetworkSelection::TConnectionPermission& aConnectionPermission,
                                                          TUint8& aWlanWhenGprs,
                                                          CIptvNetworkSelection::TRespStatus& aRespStatus )
    {
    /* EIptvEngineNsGetUsedIapReq message format:
     * ------------------
     * TUint32 aServiceId
     * ------------------
     */
        
    /* EIptvEngineNsGetUsedIapResp message format:
     * -------------------
     * TUint8  aRespStatus
     * TUint32 aIapId
     * TUint16 aIapNameLength
     * <aIapNameLength * TUint16> aIapNameData (contains SSID if iapname not found)
     * TUint8  aConnectionPermission
     * TUint8  aWlanWhenGprs
     * -------------------
     */
            
    RDesReadStream readStream;
    readStream.Open( iMsgPtr );
    CleanupClosePushL( readStream );
    aRespStatus 
        = static_cast<CIptvNetworkSelection::TRespStatus>(readStream.ReadUint8L());
    aIapId                 = readStream.ReadUint32L();
    CIptvUtil::ReadDesFromStreamL( aIapName, readStream );
    aConnectionPermission = 
        static_cast<CIptvNetworkSelection::TConnectionPermission>( readStream.ReadUint8L() );
    aWlanWhenGprs = readStream.ReadUint8L();
    CleanupStack::PopAndDestroy( &readStream );
    }
