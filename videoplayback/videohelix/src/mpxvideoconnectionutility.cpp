/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Video streaming connection management implementation
*
*/

#include <es_sock.h>
#include <commdbconnpref.h>
#include <rconnmon.h>  // RConnectionMonitor
#include <mmf/common/mmfcontrollerframeworkbase.h>

#include "mpxvideoconnectionutility.h"
#include "mpxvideo_debug.h"


TInt MpxVideoConnectionUtility::CheckAccessPointL(TInt aApId) 
{ 
    MPX_ENTER_EXIT(_L("MpxVideoConnectionUtility::checkAccessPointL()"));
    MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() aApId=%d") , aApId );
    
    // If access point is GPRS access point then instruct playback middleware to use 
    // access point from media settings. Streaming access point may be different than AP
    // used for browsing
    
    TBool isGprs( EFalse );  // or packet data in general
    TBool isWLANOpen( EFalse );
    TInt  wlanApId( KUseDefaultIap );  // if WLAN is open
    
    // Try to start default connection in case when 
    // valid ap was not given by client
    if (aApId < 0)
    {
        MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() client didn't give valid ApId")  );
        StartDefaultConnectionL();
    }
    
    MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() getting connections")  );
    
    // Resolve connections type, only open connections matter here 
    RConnectionMonitor conMon; 
    User::LeaveIfError( conMon.ConnectL() );
    CleanupClosePushL(conMon);
    
    TRequestStatus status;
    TUint connectionCount(0);    
    conMon.GetConnectionCount(connectionCount, status);
    User::WaitForRequest( status );   // Answer is needed now
    User::LeaveIfError(status.Int());
    
    MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() connectionCount=%d"), connectionCount  );
    // loop connectionsconnectionCount
    TUint connectionId(0);
    TUint subConnCount(0);
    // Start looping from 1
    for (TUint i=1; i <= connectionCount; ++i)
    {
        MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() getting connection=%d"), i  );
        conMon.GetConnectionInfo(i, connectionId, subConnCount );
        MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() got connection=%d, connId=%d"), i, connectionId  );
        
        TUint iapIdToCheck;                                              
        conMon.GetUintAttribute( connectionId, 0, KIAPId, iapIdToCheck, status );
        // Note: Codescanner generates here pointless warning. Data is needed now
        User::WaitForRequest ( status ); 
        MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() got IapId status=%d"), status.Int()  );
        User::LeaveIfError( status.Int() );
     
        // Connection given by client matches to connection that is open
        TInt bearer;  // TConnMonBearerType                                            
        conMon.GetIntAttribute( connectionId, 0, KBearer, bearer, status );    
        User::WaitForRequest ( status ); 
        MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() got bearer status=%d"), status.Int()  );
        User::LeaveIfError( status.Int() );
        
        MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL()iapIdToCheck=%d, bearer=%d"), iapIdToCheck, bearer );
        
        if (EBearerWLAN == bearer) 
        {
            isWLANOpen = ETrue;
            wlanApId = iapIdToCheck;
        }
                
        if (iapIdToCheck == aApId)
        {    
            // This is connection given by client, check type 
            if (EBearerGPRS == bearer || EBearerWCDMA == bearer || 
                    EBearerEdgeGPRS == bearer || EBearerCDMA2000 == bearer ) 
            {
                MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() given connection is packet data connection")  );
                isGprs = ETrue;
            }
            // Exact match, either GPRS or WLAN. No need to check further.
            break; 
        }      
    } // for 
    
    // Close connection monitor
    CleanupStack::PopAndDestroy(); 
    
    // If type is GPRS/packed data, return KUseDefaultIap. 
    // AP from Media Settings will be used for streaming
    if (isGprs) 
    {
        // Let playback middleware use connection from media settings
        MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() packet data connection is open: KUseDefaultIap") );
        aApId = KUseDefaultIap;
    }
    else if (isWLANOpen)
    {
        // WLAN was started or given AP was WLAN
        MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() wlan connection is open ") );
        aApId = wlanApId;
    }
    // If neither of above matched then aApId stays invalid and 
    // playback middleware will select access point or use Media Settings access point
 
    MPX_DEBUG( _L("MpxVideoConnectionUtility::checkAccessPointL() return=%d") , aApId );
    return aApId; 
}


void MpxVideoConnectionUtility::StartDefaultConnectionL()
{
    MPX_ENTER_EXIT(_L("MpxVideoConnectionUtility::StartDefaultConnectionL()"));
    
    // There is no connection open. User may have clicked RTSP url from offline browser page
    // Start connection using RConnection
    RSocketServ sockServHandle;
    User::LeaveIfError(sockServHandle.Connect() );   // can't survive if it leaves now
    CleanupClosePushL(sockServHandle);
    
    RConnection connection;
    User::LeaveIfError(connection.Open(sockServHandle) );
    CleanupClosePushL(connection);

    TCommDbConnPref connPref;
    // No parameters should be needed
    // to get default connection open, fails if no iaps are configured
    connPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
    
    // Ignore error. Can't do anything if default connection doesn't start
    // Playback middleware may start access point from media settings
    TInt err = connection.Start(connPref);
    
    MPX_DEBUG( _L("MpxVideoConnectionUtility::StartDefaultConnectionL() start err=%d"), err  );
    
    // Socket server and Rconnection close
    CleanupStack::PopAndDestroy( 2 );
}
