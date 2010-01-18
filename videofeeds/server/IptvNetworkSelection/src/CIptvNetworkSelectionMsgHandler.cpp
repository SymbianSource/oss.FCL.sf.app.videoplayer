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
* Description: 
*
*/





// INCLUDE FILES
#include <s32mem.h>

#include "IptvClientServerCommon.h"
#include "CIptvNetworkSelection.h"
#include "CIptvNetworkSelectionMsgHandler.h"
#include "MIptvServiceManagementClientObserver.h"
#include "CIptvServer.h"
#include "IptvDebug.h"

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
// CIptvNetworkSelectionMsgHandler::CIptvNetworkSelectionMsgHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvNetworkSelectionMsgHandler::CIptvNetworkSelectionMsgHandler(CIptvServer& aServer)
: iServer(aServer)
    {
    iMsgPending = EIptvEngineNone;
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelectionMsgHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvNetworkSelectionMsgHandler* CIptvNetworkSelectionMsgHandler::NewL(CIptvServer& aServer)
    {
    CIptvNetworkSelectionMsgHandler* self = 
        new( ELeave ) CIptvNetworkSelectionMsgHandler( aServer );

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CIptvNetworkSelectionMsgHandler::~CIptvNetworkSelectionMsgHandler()
    {
    delete iIpcMsg;
    }

// ------------------------------------------------------------------
// CIptvNetworkSelectionMsgHandler::HandleMsgL
// ------------------------------------------------------------------
//
void CIptvNetworkSelectionMsgHandler::HandleMsgL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelectionMsgHandler::HandleMsgL()");
    
    switch ( aMessage.Function() )
        {

        case EIptvEngineNsSetUsedIap:
            {
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

            /* Get IPC message */
            TUint32 ipcMsgSize = aMessage.GetDesLengthL(0);
            HBufC8* ipcMsg;
            ipcMsg = HBufC8::NewL(ipcMsgSize);
            CleanupStack::PushL(ipcMsg); // 1->
            ipcMsg->Des().Zero();
            TPtr8 ipcMsgPtr(ipcMsg->Des());					
            aMessage.ReadL(0, ipcMsgPtr, 0);

            /* read message data */						
            RDesReadStream readStream;			
            readStream.Open(ipcMsg->Des());
            CleanupClosePushL( readStream );
            TUint32 iapId                 = readStream.ReadInt32L();
            TUint32 serviceId             = readStream.ReadInt32L();
            TUint8 setToDefaultForService = readStream.ReadInt8L();
            CleanupStack::PopAndDestroy( &readStream );

            CIptvNetworkSelection::TRespStatus resp;
            
            //this is trapped in CIptvServerSession::ServiceL()
            iServer.iNetworkSelection->SetUsedIapL(iapId, serviceId, setToDefaultForService, resp);

            /* send resp */
            ipcMsgPtr.Zero();
            RDesWriteStream writeStream;
            writeStream.Open(ipcMsgPtr);
            CleanupClosePushL( writeStream );
            writeStream.WriteUint8L(static_cast<TUint8>(resp));
            CleanupStack::PopAndDestroy( &writeStream );
            aMessage.WriteL(0, ipcMsgPtr, 0);
            aMessage.Complete(KErrNone);
            CleanupStack::PopAndDestroy(ipcMsg); // <-1
            }
            break;
            
        case EIptvEngineNsGetUsedIapReq:
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
             * -------------------
             */

            iMsgPending = EIptvEngineNsGetUsedIapReq;
             
            //Store aMessage for later use (GetUsedIapResp())
            iMessage = aMessage;
            
            /* Get IPC message */
            TUint32 ipcMsgSize = aMessage.GetDesLengthL( 0 );

            delete iIpcMsg;
            iIpcMsg = NULL;

            iIpcMsg = HBufC8::NewL( ipcMsgSize );
            iIpcMsg->Des().Zero();
            TPtr8 ipcMsgPtr( iIpcMsg->Des() );
            aMessage.ReadL( 0, ipcMsgPtr, 0 );

            /* read message data */						
            RDesReadStream readStream;
            readStream.Open( iIpcMsg->Des() );
            CleanupClosePushL( readStream );
            TUint32 serviceId = readStream.ReadInt32L();
            CleanupStack::PopAndDestroy( &readStream );

            /* call network selection */            
            iGetUsedIapReq.iNsObserver = this;
            iGetUsedIapReq.iServiceId  = serviceId;
            iServer.iNetworkSelection->GetUsedIapReqL( &iGetUsedIapReq );

            if ( iGetUsedIapReq.iRespStatus == CIptvNetworkSelection::EDoingWlanScan )
                {
                //Start waiting for call to GetUsedIapResp()
                //Client shouldnt send anything to us meanwhile
                }
            else
                {
                //NS responded synchronously, lets handle the resp message
                GetUsedIapResp( &iGetUsedIapReq );
                }
            }
            break;
        
        case EIptvEngineNsClearUsedIap:
            {
            iServer.iNetworkSelection->ClearUsedIap();
            aMessage.Complete(KErrNone);
            }
            break;
                
        case EIptvEngineNsSetConnectionAllowed:
            {

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

            /* Get IPC message */
            TUint32 ipcMsgSize = aMessage.GetDesLengthL(0);
            HBufC8* ipcMsg;
            ipcMsg = HBufC8::NewL(ipcMsgSize);
            CleanupStack::PushL(ipcMsg); // 1->
            ipcMsg->Des().Zero();
            TPtr8 ipcMsgPtr(ipcMsg->Des());					
            aMessage.ReadL(0, ipcMsgPtr, 0);

            /* read message data */						
            RDesReadStream readStream;			
            readStream.Open(ipcMsg->Des());
            CleanupClosePushL( readStream );
            TUint8 connectionAllowed = readStream.ReadInt8L();
            TUint32 iapId            = readStream.ReadInt32L();
            CleanupStack::PopAndDestroy( &readStream );

            /* call network selection */
            CIptvNetworkSelection::TRespStatus respStatus;            
            iServer.iNetworkSelection->SetConnectionAllowedL(connectionAllowed, iapId, respStatus);
            
            /* write resp */
            ipcMsgPtr.Zero();
            RDesWriteStream writeStream;
            writeStream.Open(ipcMsgPtr);
            CleanupClosePushL( writeStream );
            writeStream.WriteUint8L(static_cast<TUint8>(respStatus));
            CleanupStack::PopAndDestroy( &writeStream );
            aMessage.WriteL(0, ipcMsgPtr, 0);
            
            /* send resp */
            aMessage.Complete(KErrNone);
            CleanupStack::PopAndDestroy(ipcMsg); // <-1
            }
            break;
           
        case EIptvEngineNsIsConnectionAllowed:
            {
                
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
                
            /* Get IPC message */
            TUint32 ipcMsgSize = aMessage.GetDesLengthL(0);
            HBufC8* ipcMsg;
            ipcMsg = HBufC8::NewL(ipcMsgSize);
            CleanupStack::PushL(ipcMsg); // 1->
            ipcMsg->Des().Zero();
            TPtr8 ipcMsgPtr(ipcMsg->Des());					
            aMessage.ReadL(0, ipcMsgPtr, 0);

            /* read message data */						
            RDesReadStream readStream;			
            readStream.Open(ipcMsg->Des());
            CleanupClosePushL( readStream );
            TUint32 serviceId = readStream.ReadUint32L();
            CleanupStack::PopAndDestroy( &readStream );

            /* call network selection */
            CIptvNetworkSelection::TRespStatus respStatus = CIptvNetworkSelection::ESucceeded;
            CIptvNetworkSelection::TConnectionPermission connectionPermission;
            connectionPermission = iServer.iNetworkSelection->IsConnectionAllowedL(serviceId);
            
            /* write resp */
            RDesWriteStream writeStream;
            writeStream.Open(ipcMsgPtr);
            CleanupClosePushL( writeStream );
            writeStream.WriteUint8L(static_cast<TUint8>(respStatus));
            writeStream.WriteUint8L(static_cast<TUint8>(connectionPermission));
            CleanupStack::PopAndDestroy( &writeStream );
            aMessage.WriteL(0, ipcMsgPtr, 0);

            /* send resp */
            aMessage.Complete(KErrNone);
            CleanupStack::PopAndDestroy(ipcMsg); // <-1
            }
            break;
            
         case EIptvEngineNsCancel:
            {
            switch(iMsgPending)
                {
                case EIptvEngineNsGetUsedIapReq:
                    {
                    TInt err( iServer.iNetworkSelection->CancelGetUsedIapReq( this ) ); 
                    aMessage.Complete( err );
                    }
                    break;
                    
                default:
                    aMessage.Complete(KErrNotFound);
                    break;
                }
            iMsgPending = EIptvEngineNone;
            }
            break;
        }

    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelectionMsgHandler::HandleMsgL() exit");
    }

// ------------------------------------------------------------------
// CIptvNetworkSelectionMsgHandler::GetUsedIapResp
// ------------------------------------------------------------------
//
void CIptvNetworkSelectionMsgHandler::GetUsedIapResp(TIptvGetUsedIapReq* /*aRequestOb*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelectionMsgHandler::GetUsedIapResp()");

    TRAPD( err, GetUsedIapRespL() );

    IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelectionMsgHandler::GetUsedIapResp() trap err:%d", err);
    iMessage.Complete( err );

    iMsgPending = EIptvEngineNone;
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelectionMsgHandler::GetUsedIapResp() exit");
    }

// ------------------------------------------------------------------
// CIptvNetworkSelectionMsgHandler::GetUsedIapRespL
// ------------------------------------------------------------------
//
void CIptvNetworkSelectionMsgHandler::GetUsedIapRespL()
    {
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
    
    TBuf8<1+4+2+(KIptvNsIapNameMaxLength*2)+1> resp;
    /* write resp */
    RDesWriteStream writeStream;
    
    writeStream.Open( resp );
    CleanupClosePushL( writeStream ); // 1->        
    writeStream.WriteUint8L( iGetUsedIapReq.iRespStatus );
    writeStream.WriteUint32L( iGetUsedIapReq.iIapId );

    IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelectionMsgHandler:: IAP ID = %d", iGetUsedIapReq.iIapId);

    //iap name
    TUint16 iapNameLength = iGetUsedIapReq.iIapName.Length();
    writeStream.WriteUint16L( iapNameLength );
    TInt i;
    for ( i = 0; i < iapNameLength; i++ )
        {
        writeStream.WriteUint16L( iGetUsedIapReq.iIapName[i] );
        }

    writeStream.WriteUint8L( iGetUsedIapReq.iConnectionPermission );

    writeStream.WriteUint8L( iGetUsedIapReq.iWLANWhenGPRS );
     
    writeStream.Close();
    CleanupStack::Pop( &writeStream ); // <-1

    /* send resp */
    iMessage.WriteL( 0, resp, 0 );
    }
