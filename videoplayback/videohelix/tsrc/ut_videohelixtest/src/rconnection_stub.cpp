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
* Description:  Connection management stub implementation 
*
*/

// Version : %version:  1 %

// #include "rconnection_stub.h"  // TODO: lets see if this is needed at all
#include <es_sock.h>
#include <commdbconnpref.h>
#include <rconnmon.h>  // RConnectionMonitor


#include "mpxvideo_debug.h"



EXPORT_C
RSocketServ::RSocketServ() 
{       
    MPX_ENTER_EXIT(_L("RSocketServ::RSocketServ() - stub impl"));
}
    
EXPORT_C    
TInt RSocketServ::Connect(TUint aMessageSlots )
{   
    MPX_ENTER_EXIT(_L("RSocketServ::Connect() - stub impl"));
    return KErrNone;
}
   
EXPORT_C 
TConnPref::TConnPref( ) 
{
    MPX_ENTER_EXIT(_L("TConnPref::TConnPref() - stub impl"));
}

EXPORT_C
TCommDbConnPref::TCommDbConnPref() 
{
    MPX_ENTER_EXIT(_L("TCommDbConnPref::TCommDbConnPref() - stub impl"));
}

EXPORT_C
void TCommDbConnPref::SetDialogPreference(TCommDbDialogPref aDialogPref)
{
    MPX_ENTER_EXIT(_L("TCommDbConnPref::SetDialogPreference() - stub impl"));
}

EXPORT_C
RConnection::RConnection(): iNewISPId(0)
{
    MPX_ENTER_EXIT(_L("RConnection::RConnection() - stub impl"));
}

EXPORT_C 
TInt RConnection::Open(RSocketServ& aSocketServer, TUint aConnectionType )
{
    MPX_ENTER_EXIT(_L("RConnection::Open() - stub impl"));
    return KErrNone;
}

EXPORT_C 
TInt RConnection::Start(TConnPref& aPref)
{
    MPX_ENTER_EXIT(_L("RConnection::Start() - stub impl"));
    return KErrNone;
}

EXPORT_C 
void RConnection::Close( )
{
    MPX_ENTER_EXIT(_L("RConnection::Close() - stub impl"));
}

EXPORT_C
RConnection::~RConnection()
{
    MPX_ENTER_EXIT(_L("RConnection::~RConnection - stub impl"));
}     

/*
EXPORT_C
RConnectionMonitor::RConnectionMonitor()
{
    MPX_ENTER_EXIT(_L("RConnectionMonitor::RConnectionMonitor() - stub impl")); 
}*/

EXPORT_C TInt TCommDbConnPref::Compare(const TCommDbConnPref& aPref) const
{
    MPX_ENTER_EXIT(_L("TCommDbConnPref::Compare() - stub impl"));
    return 0;  // not really called
}



EXPORT_C
TInt RConnectionMonitor::ConnectL()
{
    MPX_ENTER_EXIT(_L("RConnectionMonitor::ConnectL() - stub impl")); 
    return KErrNone;
}

EXPORT_C
void RConnectionMonitor::Close()
{
    MPX_ENTER_EXIT(_L("RConnectionMonitor::Close() - stub impl")); 
}

EXPORT_C
void RConnectionMonitor::GetConnectionCount(
               TUint& aConnectionCount,
               TRequestStatus& aStatus )
{
    MPX_ENTER_EXIT(_L("RConnectionMonitor::GetConnectionCount() - stub impl")); 
    
    // put value and change request status
    aConnectionCount = 1;
    aStatus = KErrNone;
}
    
EXPORT_C
TInt RConnectionMonitor::GetConnectionInfo(
                const TUint aIndex,
                TUint& aConnectionId,
                TUint& aSubConnectionCount ) const 
{
    MPX_ENTER_EXIT(_L("RConnectionMonitor::GetConnectionInfo() - stub impl")); 
   
    // This connection is handled as GPRS
    aConnectionId = 7;   
    
    return KErrNone;
}

EXPORT_C
void RConnectionMonitor::GetIntAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        TInt& aValue,
        TRequestStatus& aStatus ) 
{
    MPX_ENTER_EXIT(_L("RConnectionMonitor::GetIntAttribute() - stub impl")); 
    
    // this call is asking for bearer
    // connection 7 is GPRS others are WLAN
    if ( aConnectionId == 7 )
    {
        aValue = EBearerGPRS;
    }
    else 
    {
        aValue = EBearerWLAN;
    }
    
}

EXPORT_C
void RConnectionMonitor::GetUintAttribute(
        const TUint aConnectionId,
        const TUint aSubConnectionId,
        const TUint aAttribute,
        TUint& aValue,
        TRequestStatus& aStatus )
{
    MPX_ENTER_EXIT(_L("RConnectionMonitor::GetUintAttribute() - stub impl"));  
    
    
    // Asking iapId
    if (KIAPId == aAttribute && aConnectionId == 7)
    {
        aValue = 13;  // iap id 13, handled as GPRS
    }
    else
    {
        aValue = 10;   
    }
   
 
    aStatus = KErrNone;
}

EXPORT_C
void User::WaitForRequest(TRequestStatus& aStatus)
{
 //      TRequestStatus* ptrStatus = &aStatus;
  //     User::RequestComplete(ptrStatus,KErrNone);                  
        aStatus = KErrNone;
       return;
}


// TODO TRequeststatus and waitforrequest needs implementation

