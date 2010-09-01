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
* Description:    Handles messages from client*
*/





// INCLUDE FILES
#include "IptvDebug.h"
#include <bautils.h>

#include "IptvClientServerCommon.h"
#include "CIptvSmEvent.h"
#include "CIptvService.h"
#include "CIptvServices.h"
#include "CIptvServiceMsgHandler.h"
#include "CIptvServer.h"
#include "MIptvServiceManagementClientObserver.h"
#include "CIptvEpgManagerImpl.h"
#include "CIptvEpgSession.h"
#include "CIptvEventGenerator.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvServiceMsgHandler::CIptvServiceMsgHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvServiceMsgHandler::CIptvServiceMsgHandler(CIptvServer& aServer)
: iServer(aServer)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::CIptvServiceMsgHandler()");
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::CIptvServiceMsgHandler() exit");
    }

// -----------------------------------------------------------------------------
// CIptvServiceMsgHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvServiceMsgHandler::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::ConstructL()");

    CIptvSmEvent* eventProto = CIptvSmEvent::NewL();
    CleanupStack::PushL(eventProto); // 1->
    iEventGenerator = CIptvEventGenerator::NewL(eventProto /* ownership moves */);
    CleanupStack::Pop(eventProto); // <-1
    iServer.iServiceManager->RegisterObserver(this);

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::ConstructL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvServiceMsgHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvServiceMsgHandler* CIptvServiceMsgHandler::NewL(CIptvServer& aServer)
    {
    CIptvServiceMsgHandler* self = new( ELeave ) CIptvServiceMsgHandler(aServer);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
CIptvServiceMsgHandler::~CIptvServiceMsgHandler()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::~CIptvServiceMsgHandler()");
    delete iServicesDataRespIpcMsg;
    delete iServicesSizeRespIpcMsg;
    delete iServices;
    delete iEventGenerator;
    iServer.iServiceManager->DeRegisterObserver(this);
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::~CIptvServiceMsgHandler() exit");
    }

// ------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleServiceManagerMsgL
// Handles messages meant for Service Manager.
// ------------------------------------------------------------------
//
void CIptvServiceMsgHandler::HandleServiceManagerMsgL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleServiceManagerMsgL()");

    switch(aMessage.Function())
        {
        case EIptvEngineSmGetServicesSizeReq: //get services from database, return size
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineSmGetServicesSizeReq");
            HandleGetServicesSizeReqL(aMessage);
            break;
            
        case EIptvEngineSmGetServicesDataReq: //get services from database, return data
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineSmGetServicesDataReq");
            HandleGetServicesDataReqL(aMessage);
            break;
        	
        case EIptvEngineSmGetServicesUsingIdSizeReq:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineSmGetServicesUsingIdSizeReq");
            HandleGetServicesUsingIdSizeReqL(aMessage);
            break;
            
        case EIptvEngineSmGetServicesUsingIdDataReq:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineSmGetServicesUsingIdDataReq");
            HandleGetServicesUsingIdDataReqL(aMessage);
            break;
        	        	        	        	        	
        case EIptvEngineSmAddServiceReq:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineSmAddServiceReq");
            HandleAddServiceReqL(aMessage);
            break;
        	
        case EIptvEngineSmDeleteServiceReq:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineSmDeleteServiceReq");
            HandleDeleteServiceReqL(aMessage);
            break;
    	
        case EIptvEngineSmUpdateServiceReq:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineSmUpdateServiceReq");
            HandleUpdateServiceReqL(aMessage);
            break;
        
        case EIptvEngineSmGetServiceSizeUsingProviderId:
            HandleGetServiceSizeUsingProviderIdL(aMessage);
            break;
            
        case EIptvEngineSmGetServiceDataUsingProviderId:
            HandleGetServiceDataUsingProviderIdL(aMessage);
            break;
    
        case EIptvEngineSmCancel:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineSmCancel");
            //All messages are currently processed synchronously in server side -> no need to do anything
            aMessage.Complete(KErrNone);
            break;
        
        case EIptvEngineSmGetEventSizeReq:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineGetEventSizeReq");
            if (iEventGenerator)
                {
                iEventGenerator->SizeRequestL(aMessage);
                }
            break;
            
        case EIptvEngineSmGetEventData:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineGetEventData");
            if (iEventGenerator)
                {
                iEventGenerator->DataRequestL(aMessage);
                }
            break;

        case EIptvEngineSmCancelGetEventSizeReq:
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: EIptvEngineSmCancelGetEventSizeReq");
            delete iEventGenerator;
            iEventGenerator = NULL;
            aMessage.Complete(KErrNone);
            break;

        }
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleServiceManagerMsg() exit");
    }

// ------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleGetServicesSizeReqL
// ------------------------------------------------------------------
//
void CIptvServiceMsgHandler::HandleGetServicesSizeReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServicesSizeReqL()");
                
    //fetch data from the client address space
    TUint32 ipcMsgSize = aMessage.GetDesLengthL(0);
    HBufC8* ipcMsg = HBufC8::NewL(ipcMsgSize);
    CleanupStack::PushL(ipcMsg);
    ipcMsg->Des().Zero();
    TPtr8 ipcMsgPtr(ipcMsg->Des());
    aMessage.ReadL(0, ipcMsgPtr, 0);

    RDesReadStream readStream;
    readStream.Open(ipcMsgPtr);
    CleanupClosePushL( readStream );
    //See CIptvServiceManagementClient::TSearchLimitFlag
    TUint32 searchLimitFlags;
    TUint8 order;
    
    searchLimitFlags = readStream.ReadUint32L();
    order            = readStream.ReadUint8L();
    
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy(ipcMsg);
    
    if ( iServices )
        {
        delete iServices;
        iServices = NULL;
        }
                
    TRAPD(err, iServices = iServer.iServiceManager->GetServicesL(searchLimitFlags,
                                                                static_cast<CIptvServiceManagementClient::TOrder>(order),
                                                                0 /* 0 = use latest version */));
    if(err != KErrNone)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: Failed to get services from DB!");

        SendStatusRespL( aMessage,
                         MIptvServiceManagementClientObserver::EGeneralError,
                         err );
        }
    else
        {        
	      //build iServicesSizeRespIpcMsg and iServicesDataRespIpcMsg from iServicesArray
        BuildServiceIpcMsgRespsL();
				
        aMessage.WriteL(0, iServicesSizeRespIpcMsg->Des(), 0);

        aMessage.Complete(err);
		
        delete iServicesSizeRespIpcMsg;
        iServicesSizeRespIpcMsg = NULL;
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServicesSizeReqL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleGetServicesDataReqL
// ---------------------------------------------------------------------------
//	    
void CIptvServiceMsgHandler::HandleGetServicesDataReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServicesDataReqL()");

    aMessage.WriteL(0, iServicesDataRespIpcMsg->Des(), 0);

    aMessage.Complete(KErrNone);
        	
    delete iServicesDataRespIpcMsg;
    iServicesDataRespIpcMsg = NULL;

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServicesDataReqL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleGetServicesUsingIdSizeReqL
// ---------------------------------------------------------------------------
//	
void CIptvServiceMsgHandler::HandleGetServicesUsingIdSizeReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServicesUsingIdSizeReqL()");

    //fetch data from the client address space
    TUint32 ipcMsgSize = aMessage.GetDesLengthL(0);
    HBufC8* ipcMsg = HBufC8::NewL(ipcMsgSize);
    CleanupStack::PushL(ipcMsg);  
    ipcMsg->Des().Zero();
    TPtr8 ipcMsgPtr(ipcMsg->Des());
    aMessage.ReadL(0, ipcMsgPtr, 0);

    //get start and end indexes
    TUint32 startIndex;
    TUint32 endIndex;

    RDesReadStream readStream;
    readStream.Open(ipcMsgPtr);
    CleanupClosePushL( readStream );
    startIndex = readStream.ReadUint32L();
    endIndex   = readStream.ReadUint32L();
    CleanupStack::PopAndDestroy( &readStream );

    //Make the db query
    if(iServices)
        {
        delete iServices;
        iServices = NULL;
        }        
    TRAPD(err, iServices = iServer.iServiceManager->GetServicesUsingIdL(startIndex, endIndex, ETrue /* do open/close */));

    if(err != KErrNone)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: Failed to get services from DB!");

        delete iServices;
        iServices = NULL;
        SendStatusRespL( aMessage,
                         MIptvServiceManagementClientObserver::EGeneralError,
                         err );
        }
    else
        {
        //build iServicesSizeRespIpcMsg and iServicesDataRespIpcMsg from iServices
        BuildServiceIpcMsgRespsL();

        aMessage.WriteL(0, iServicesSizeRespIpcMsg->Des(), 0);
        aMessage.Complete(err);
    
        delete iServicesSizeRespIpcMsg;
        iServicesSizeRespIpcMsg = NULL;
        }
        
    CleanupStack::PopAndDestroy(ipcMsg);
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServicesUsingIdSizeReqL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleGetServicesUsingIdDataReqL
// ---------------------------------------------------------------------------
//	
void CIptvServiceMsgHandler::HandleGetServicesUsingIdDataReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServicesUsingIdDataReqL()");

    aMessage.WriteL(0, iServicesDataRespIpcMsg->Des(), 0);
    aMessage.Complete(KErrNone);
    
    delete iServicesDataRespIpcMsg;
    iServicesDataRespIpcMsg = NULL;
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServicesUsingIdDataReqL() exit");
    }
    
// ---------------------------------------------------------------------------
// Used for EIptvEngineGetServicesSizeReq and EIptvEngineGetServicesUsingIdSizeReq.
// Builds size (= iServicesSizeRespIpcMsg) and data (= iServicesDataRespIpcMsg)
// messages from iServices.
// iServicesSizeRespIpcMsg message contains iServicesDataRespIpcMsg length. 
// ---------------------------------------------------------------------------
//	
void CIptvServiceMsgHandler::BuildServiceIpcMsgRespsL()
    {
    if(iServicesSizeRespIpcMsg)
        {
        delete iServicesSizeRespIpcMsg;
        iServicesSizeRespIpcMsg = NULL;
        }
        
    const TInt KServicesSizeRespLength = 1/*status*/+4/*data size*/;
    iServicesSizeRespIpcMsg = HBufC8::NewL(KServicesSizeRespLength);		
    TPtr8 servicesSizeRespIpcMsgPtr(iServicesSizeRespIpcMsg->Des());

    //count iServicesDataRespIpcMsg size
    TInt32 dataSize;

    dataSize = 1 /* message status */ + iServices->CountExternalizeSize();
		
    //build iServicesSizeRespIpcMsg
    RDesWriteStream writeStream;
    writeStream.Open(servicesSizeRespIpcMsgPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint8L(MIptvServiceManagementClientObserver::ESucceeded);
    writeStream.WriteUint32L(dataSize);
    CleanupStack::PopAndDestroy( &writeStream );

    //build iServicesDataRespIpcMsg
    if(iServicesDataRespIpcMsg)
        {
        delete iServicesDataRespIpcMsg;
        iServicesDataRespIpcMsg = NULL;
        }

    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceMsgHandler:: allocating %d bytes", dataSize);
             
    iServicesDataRespIpcMsg = HBufC8::NewL(dataSize);
    TPtr8 servicesDataRespIpcMsgPtr(iServicesDataRespIpcMsg->Des());
    writeStream.Open(servicesDataRespIpcMsgPtr);
    writeStream.WriteUint8L(MIptvServiceManagementClientObserver::ESucceeded);    
    iServices->ExternalizeL(writeStream);
    writeStream.Close();
            
    delete iServices;
    iServices = NULL;    
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleAddServiceReqL
// ---------------------------------------------------------------------------
//	
void CIptvServiceMsgHandler::HandleAddServiceReqL(const RMessage2& aMessage)
    {
    
    //fetch data from the client address space
    TUint32 ipcMsgSize = aMessage.GetDesLengthL(0);
    HBufC8* ipcMsg = HBufC8::NewL(ipcMsgSize);
    CleanupStack::PushL(ipcMsg);  
    ipcMsg->Des().Zero();
    TPtr8 ipcMsgPtr(ipcMsg->Des());
    aMessage.ReadL(0, ipcMsgPtr, 0);

    //create CIptvService class from the IPC msg
    RDesReadStream readStream;
    readStream.Open(ipcMsgPtr);
    CleanupClosePushL( readStream );
    CIptvService* iptvService = CIptvService::NewL();
    CleanupStack::PushL(iptvService);  
    iptvService->InternalizeL(readStream);
     
    //add CIptvService to db    		       
    iServer.iServiceManager->AddServiceL(*iptvService, ETrue /* do open/close */);
    
    SendStatusRespL( aMessage,
                     MIptvServiceManagementClientObserver::ESucceeded,
                     KErrNone );
		
    CleanupStack::PopAndDestroy(iptvService);
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy(ipcMsg);            
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleDeleteServiceReqL
// ---------------------------------------------------------------------------
//	
void CIptvServiceMsgHandler::HandleDeleteServiceReqL( const RMessage2& aMessage )
    {
    TUint32 ipcMsgSize( aMessage.GetDesLengthL( 0 ) );
    HBufC8* readIpcMsg = HBufC8::NewL( ipcMsgSize );
    CleanupStack::PushL( readIpcMsg ); //1->
    TPtr8 readIpcMsgPtr( readIpcMsg->Des() );

    aMessage.ReadL( 0, readIpcMsgPtr, 0 );
    
    RDesReadStream readStream;
    readStream.Open( readIpcMsgPtr );
    CleanupClosePushL( readStream );
    TUint32 serviceId = readStream.ReadUint32L();
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( readIpcMsg ); // <-1

    // Read service for type
    CIptvService* service = iServer.iServiceManager->
        GetServiceL( serviceId, ETrue /* do open/close */ );
    
    if ( service )
        {
        CIptvService::TServiceType serviceType( service->GetType() );
        TUint32 groupId( service->GetGroupId() );
        delete service;

        TInt err( KErrNone );
        if ( serviceType == CIptvService::EServiceGroup )
            {
            // All items in service group need to be deleted also 
            err = DeleteGroupL( groupId ); 
            }
        else
            {
            err = DeleteServiceL( serviceId, serviceType );
            }

        // Returns KErrNone even when no services are deleted   
        if ( err != KErrNone )
            {
            SendStatusRespL( aMessage,
                MIptvServiceManagementClientObserver::EGeneralError, err );
            }
        else
            {
            SendStatusRespL( aMessage,
                MIptvServiceManagementClientObserver::ESucceeded, err );
            }                       
        }
    else
        {
        SendStatusRespL( aMessage,
            MIptvServiceManagementClientObserver::EServiceNotFound, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleUpdateServiceReqL
// ---------------------------------------------------------------------------
//	
void CIptvServiceMsgHandler::HandleUpdateServiceReqL(const RMessage2& aMessage)
    {

    //fetch data from the client address space
    TUint32 ipcMsgSize = aMessage.GetDesLengthL(0);
    HBufC8* ipcMsg = HBufC8::NewL(ipcMsgSize);
    CleanupStack::PushL(ipcMsg);  
    ipcMsg->Des().Zero();
    TPtr8 ipcMsgPtr(ipcMsg->Des());
    aMessage.ReadL(0, ipcMsgPtr, 0);

    //create CIptvService class from the IPC msg
    CIptvService* iptvService = CIptvService::NewL();
    CleanupStack::PushL(iptvService);  
    RDesReadStream readStream;
    readStream.Open(ipcMsgPtr);
    CleanupClosePushL( readStream );
    iptvService->InternalizeL(readStream);
    CleanupStack::PopAndDestroy( &readStream );
     
    TRAPD( err, SetEpgLastUpdateTimeL( iptvService->GetId(), 
            iptvService->GetEpgUpdateTimeL() ) )
    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceMsgHandler::HandleUpdateServiceReqL Service db LastUpdate updated, err=%d", err);
    
    //add CIptvService to db    		       
    TRAP(err, iServer.iServiceManager->UpdateServiceL(*iptvService));
    if(err != KErrNone)
        {        
        if(err == KErrNotFound)
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: returning EServiceNotFound.");
            SendStatusRespL( aMessage,
                             MIptvServiceManagementClientObserver::EServiceNotFound,
                             KErrNone );
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: returning EGeneralError.");
            SendStatusRespL( aMessage,
                             MIptvServiceManagementClientObserver::EGeneralError,
                             err );
            }
        }
    else
        {
        SendStatusRespL( aMessage,
                         MIptvServiceManagementClientObserver::ESucceeded,
                         KErrNone );
        }
		
    CleanupStack::PopAndDestroy(iptvService);
    CleanupStack::PopAndDestroy(ipcMsg);
            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CIptvServiceMsgHandler::SetEpgLastUpdateTimeL( TUint32 aServiceId, TTime& aLastUpdate )
    {
    CIptvEpgManagerImpl& epgManager = iServer.GetEpgManager();
    TInt temp( KErrNone );
    CIptvEpgSession* epgSession = 
        epgManager.CreateSessionL( aServiceId, NULL, temp, EFalse );    
    CleanupStack::PushL( epgSession );
    epgSession->SetLastUpdateTimeL( aLastUpdate );
    CleanupStack::PopAndDestroy( epgSession );             
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::SendStatusRespL
// ---------------------------------------------------------------------------
//	
void CIptvServiceMsgHandler::SendStatusRespL(
        const RMessage2& aMessage,
        MIptvServiceManagementClientObserver::TRespStatus aError,
        TInt aGlobalError )
    {
    const TInt KStatusRespLength = 2;
    TBuf8<KStatusRespLength> resp;
    resp.Zero();
    RDesWriteStream writeStream;
    
    writeStream.Open(resp);
    CleanupClosePushL(writeStream); // 1->
    writeStream.WriteUint8L(aError);
    CleanupStack::PopAndDestroy(&writeStream); // <-1
    aMessage.WriteL(0, resp, 0);
    
    aMessage.Complete(aGlobalError);
    }
 
// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleGetServiceSizeUsingProviderIdL
// ---------------------------------------------------------------------------
//	
void CIptvServiceMsgHandler::HandleGetServiceSizeUsingProviderIdL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServiceSizeUsingProviderId()");

    HBufC8* ipcMsg = GetDataFromClientAddrSpaceLC(aMessage); // 1->
    TPtr8 ipcMsgPtr(ipcMsg->Des());

    if ( iServices )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: iService already in use, aborting");
        aMessage.Complete(KErrServerBusy);
        CleanupStack::PopAndDestroy(ipcMsg);     // <-1
        aMessage.Complete(KErrServerBusy);
        return;   
        }

    //read data from the message
    RDesReadStream readStream;
    readStream.Open(ipcMsgPtr);
    CleanupClosePushL( readStream );
    TUint32 providerIdLength = readStream.ReadUint32L();
    HBufC* providerId = HBufC::NewL(providerIdLength);
    TPtr providerIdPtr(providerId->Des());
    CleanupStack::PushL(providerId); // 2->
    TInt i;
    for(i = 0; i < providerIdLength; i++)
        {
        providerId->Des().Append(readStream.ReadInt16L());
        }
    readStream.Close();

    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceMsgHandler:: provider id: %S", &providerIdPtr);
        
    //get services from service manager    
    iServices = iServer.iServiceManager->GetServicesL(providerId->Des(), ETrue /* do open/close */);
    CleanupStack::PopAndDestroy(providerId); // <-2
    CleanupStack::PopAndDestroy( &readStream );
    if ( ! iServices )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: Could not get any services.");
        aMessage.Complete(KErrNotFound);
        CleanupStack::PopAndDestroy(ipcMsg);     // <-1
        return;
        }
            
    //write size to ipc msg
    ipcMsg->Des().Zero();
    RDesWriteStream writeStream;
    writeStream.Open(ipcMsgPtr);
    CleanupClosePushL(writeStream); // 1->
    writeStream.WriteUint32L(iServices->CountExternalizeSize());
    CleanupStack::PopAndDestroy(&writeStream); // <-1
    
    //write to client addr space
    aMessage.WriteL(0, ipcMsgPtr, 0);
    aMessage.Complete(KErrNone);
    
    CleanupStack::PopAndDestroy(ipcMsg);     // <-1
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServiceSizeUsingProviderId() exit");    
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleGetServiceDataUsingProviderIdL
// ---------------------------------------------------------------------------
//
void CIptvServiceMsgHandler::HandleGetServiceDataUsingProviderIdL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServiceDataUsingProviderIdL()");

    if ( ! iServices )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler:: iService = NULL, can't return anything");
        aMessage.Complete(KErrNotFound);
        return;    
        }

    HBufC8* ipcMsg = HBufC8::NewL(iServices->CountExternalizeSize());
    CleanupStack::PushL(ipcMsg); // 1->
    TPtr8 ipcMsgPtr(ipcMsg->Des());
    ipcMsgPtr.Zero();

    //write services to ipc msg
    RDesWriteStream writeStream;
    writeStream.Open(ipcMsgPtr);
    CleanupClosePushL( writeStream );
    iServices->ExternalizeL(writeStream);
    CleanupStack::PopAndDestroy( &writeStream );

    delete iServices;
    iServices = NULL;
    
    //write to client addr space
    aMessage.WriteL(0, ipcMsgPtr, 0);
    aMessage.Complete(KErrNone);
    CleanupStack::PopAndDestroy(ipcMsg);     // <-1
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleGetServiceDataUsingProviderIdL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::GetDataFromClientAddrSpaceLC
// ---------------------------------------------------------------------------
//
HBufC8* CIptvServiceMsgHandler::GetDataFromClientAddrSpaceLC(const RMessage2& aMessage)
    {
    //fetch data from the client address space
    TUint32 ipcMsgSize = aMessage.GetDesLengthL(0);
    HBufC8* ipcMsg = HBufC8::NewL(ipcMsgSize);
    CleanupStack::PushL(ipcMsg);     // 1->
    TPtr8 ipcMsgPtr(ipcMsg->Des());
    ipcMsgPtr.Zero();
    aMessage.ReadL(0, ipcMsgPtr, 0);
    return ipcMsg;
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleSmEvent
// ---------------------------------------------------------------------------
//
void CIptvServiceMsgHandler::HandleSmEvent(CIptvSmEvent& aEvent)
    {
    TRAPD(err, HandleSmEventL(aEvent));
    if (err != KErrNone)
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceMsgHandler::HandleSmEventL() leaved, err = %d", err);
        }
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::HandleSmEventL
// ---------------------------------------------------------------------------
//    
void CIptvServiceMsgHandler::HandleSmEventL(CIptvSmEvent& aEvent)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleSmEvent()");

    if (iEventGenerator)
        {
        iEventGenerator->AddEventL( aEvent );
        }

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceMsgHandler::HandleSmEvent() exit");
    }
    
// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::DeleteGroupL
// ---------------------------------------------------------------------------
//  
TInt CIptvServiceMsgHandler::DeleteGroupL( const TUint32 aGroupId )
    {
    // Get all services
    CIptvServices* services = iServer.iServiceManager->GetServicesL(
        CIptvServiceManagementClient::TSearchLimitFlag( 0 ),
        CIptvServiceManagementClient::EDisplayOrderDescending,
        0 /*Latest*/ );

    if ( services != NULL )
        {
        CleanupStack::PushL( services );
        RFs fs;
        CleanupClosePushL( fs );
        User::LeaveIfError( fs.Connect() );
        
        // Find all services matching to group.
        for ( TInt i( 0 ); i < services->Count(); i++ )
            {
            CIptvService* tempService = services->GetServiceL( i );
            CleanupStack::PushL( tempService );
            
            if ( aGroupId == tempService->GetGroupId() )
                {
                // Delete icon file if downloaded and exist
                if ( tempService->GetIconPath() != KIptvEmptyDes )
                    {
                    if ( BaflUtils::FileExists( fs, tempService->GetIconPath() ) )
                        {
                        fs.Delete( tempService->GetIconPath() );
                        }
                    }
                
                // Delete service
                DeleteServiceL( tempService->GetId(), tempService->GetType() );
                }
    
            CleanupStack::PopAndDestroy( tempService );
            }

        CleanupStack::PopAndDestroy( &fs );
        CleanupStack::PopAndDestroy( services );
        return KErrNone;
        }
    
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CIptvServiceMsgHandler::DeleteServiceL
// ---------------------------------------------------------------------------
//  
TInt CIptvServiceMsgHandler::DeleteServiceL(
    const TUint32 aServiceId,
    const CIptvService::TServiceType aServiceType )
    {
    // Delete also the EPG DB file for service types that have a DB.
    if ( aServiceType == CIptvService::EVod ||
         aServiceType == CIptvService::EVodCast ||
         aServiceType == CIptvService::ELiveTv )
        {
        TRAP_IGNORE( // Ignore problems with Epg Manager.
            {
            CIptvEpgManagerImpl& epgManager = iServer.GetEpgManager();
            TInt temp( KErrNone );
            CIptvEpgSession* epgSession = 
                epgManager.CreateSessionL( aServiceId, NULL, temp, EFalse );    

            CleanupStack::PushL( epgSession );
            epgSession->DeleteDatabaseFile();
            CleanupStack::PopAndDestroy( epgSession );             
            } );
        }

    return iServer.iServiceManager->DeleteServiceL( aServiceId );
    }

//  End of File  
