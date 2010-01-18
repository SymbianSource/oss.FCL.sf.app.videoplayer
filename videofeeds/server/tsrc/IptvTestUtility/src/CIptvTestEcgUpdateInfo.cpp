/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   implements logging functionality*
*/




// INCLUDE FILES
#include "CIptvTestEcgUpdateInfo.h"
#include "VCXTestLog.h"



// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT(KIPTVTestLoggerFolder,"IPTV");
_LIT(KIPTVTestLoggerFile,"IptvEcgUpdateLog.txt");
// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvTestDlInfo::
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTestEcgUpdateInfo* CIptvTestEcgUpdateInfo::NewL()
    {
    VCXLOGLO1(">>>CIptvTestEcgUpdateInfo::NewL");   
    CIptvTestEcgUpdateInfo* self = new (ELeave) CIptvTestEcgUpdateInfo();
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop();
    VCXLOGLO1("<<<CIptvTestEcgUpdateInfo::NewL");   
    return self;    
    }
// -----------------------------------------------------------------------------
// CIptvTestEcgUpdateInfo::~CIptvTestEcgUpdateInfo
// destructor
// -----------------------------------------------------------------------------
//    
EXPORT_C CIptvTestEcgUpdateInfo::~CIptvTestEcgUpdateInfo()
    {
    VCXLOGLO1(">>>CIptvTestEcgUpdateInfo::~CIptvTestEcgUpdateInfo");   
    if(iIptvTestLogger)
	    {
	    delete iIptvTestLogger;
	    iIptvTestLogger=NULL;	
	    }
	    
	if(iIptvTestUtilities)
	    {
	    delete iIptvTestUtilities;
	    iIptvTestUtilities=NULL;	
	    }

    VCXLOGLO1("<<<CIptvTestEcgUpdateInfo::~CIptvTestEcgUpdateInfo");   
    }

// -----------------------------------------------------------------------------
// CIptvTestEcgUpdateInfo::CIptvTestEcgUpdateInfo
// 
// -----------------------------------------------------------------------------
//
CIptvTestEcgUpdateInfo::CIptvTestEcgUpdateInfo()
    {
    VCXLOGLO1(">>>CIptvTestEcgUpdateInfo::CIptvTestEcgUpdateInfo");   
    
    VCXLOGLO1("<<<CIptvTestEcgUpdateInfo::CIptvTestEcgUpdateInfo");       
    }
// -----------------------------------------------------------------------------
// CIptvTestEcgUpdateInfo::ConstructL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvTestEcgUpdateInfo::ConstructL( )
    {
    VCXLOGLO1(">>>CIptvTestEcgUpdateInfo::ConstructL");   
    iIptvTestLogger = CIptvTestLogger::NewL(KIPTVTestLoggerFolder(), KIPTVTestLoggerFile());
    iIptvTestUtilities = CIptvTestUtilities::NewL();
    

    VCXLOGLO1("<<<CIptvTestEcgUpdateInfo::ConstructL");       
    }


// -----------------------------------------------------------------------------
// CIptvTestEcgUpdateInfo::EcgUpdateStart
// 
// -----------------------------------------------------------------------------
// 
EXPORT_C void CIptvTestEcgUpdateInfo::EcgUpdateStart(TUint32 aServiceId, TUint32 aIapId)
    {
    VCXLOGLO1(">>>CIptvTestEcgUpdateInfo::EcgUpdateStart");
    VCXLOGLO3("aServiceId = %d, aIapId = %d",aServiceId, aIapId );
    _LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3");
    _LIT(KTimeString,"%-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
    TBuf<30> dateString;
    TBuf<30> timeString;
    
    CIptvService* p = NULL;
    TBuf<255> iapName;
    TBuf<255> serviceName;
    TBuf<255> serviceAddr;
    TBuf<255> logBuf;
    
    TTime startTime;
    startTime.HomeTime();
    startTime.FormatL(dateString,KDateString);
    startTime.FormatL(timeString,KTimeString);
       
    p = iIptvTestUtilities->GetServiceFromDb(aServiceId);
    
    if(iIptvTestUtilities->GetIapNameById(iapName, aIapId) != TRUE)
        {
        iapName.Append(_L("N/A"));   
        }
       
    if(p)
        {
        serviceName.Append(p->GetName());
        serviceAddr.Append(p->GetAddress());    
        }
    else
        {
        serviceName.Append(_L("Unknown"));
        serviceAddr.Append(_L("Unknown"));        
        }
    
    if(p)
        {
        delete p;
        p = NULL;    
        }
    iIptvTestLogger->Log(_L("--------------------------------------------------------------------------"));
    iIptvTestLogger->Log(_L("Ecgupdate start: %S %S"),&dateString, &timeString);
    logBuf.AppendFormat(_L("serviceid: %d, Servicename: %S"),aServiceId, &serviceName);    
    iIptvTestLogger->Log(_L("%S"),&logBuf);
    logBuf.Zero();
    
    iIptvTestLogger->Log(_L("Serviceaddress: %S "),&serviceAddr);
    logBuf.AppendFormat(_L("IapId: %d, Iapname: %S"),aIapId, &iapName);
    iIptvTestLogger->Log(_L("%S"),&logBuf);
    
    VCXLOGLO1("<<<CIptvTestEcgUpdateInfo::EcgUpdateStart");    
    }
    
// -----------------------------------------------------------------------------
// CIptvTestEcgUpdateInfo::EcgUpdateStop
// 
// -----------------------------------------------------------------------------
//     
EXPORT_C void CIptvTestEcgUpdateInfo::EcgUpdateStop(TUint32 aServiceId, TInt aError, TInt aInfo)
    {
    VCXLOGLO1(">>>CIptvTestEcgUpdateInfo::EcgUpdateStop");
    TBuf<255> logBuf;
    _LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3");
    _LIT(KTimeString,"%-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
    TBuf<30> dateString;
    TBuf<30> timeString;
    
    TTime endTime;
    endTime.HomeTime();
    endTime.FormatL(dateString,KDateString);
    endTime.FormatL(timeString,KTimeString);
    
    iIptvTestLogger->Log(_L("Ecgupdate stop: %S %S"),&dateString, &timeString);
    logBuf.AppendFormat(_L("serviceid: %d, error = %d, info = %d"),aServiceId, aError, aInfo); 
    iIptvTestLogger->Log(_L("%S"),&logBuf);
    
    VCXLOGLO4("aServiceId = %d, aError = %d, aInfo = %d",aServiceId, aError, aInfo );
    iIptvTestLogger->Log(_L("--------------------------------------------------------------------------"));   
    VCXLOGLO1("<<<CIptvTestEcgUpdateInfo::EcgUpdateStop");        
    }    


   
// -----------------------------------------------------------------------------
// CIptvTestVerifyData::WriteVerifyData
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvTestEcgUpdateInfo::WriteLogL(TDesC& aStr)
    {
    VCXLOGLO1(">>>CIptvTestEcgUpdateInfo::WriteLog");
	iIptvTestLogger->Log(_L("%S"),&aStr);
	VCXLOGLO1("<<<CIptvTestEcgUpdateInfo::WriteLog");    
    }

// -----------------------------------------------------------------------------
// CIptvTestVerifyData::WriteVerifyData
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvTestEcgUpdateInfo::WriteLogL(TRefByValue<const TDesC> aFmt, ... )
    {
    VCXLOGLO1(">>>CIptvTestEcgUpdateInfo::WriteLog(formatted)");

    VA_LIST argptr; //pointer to argument list 
    VA_START( argptr, aFmt );
    
	HBufC* str;
	str = HBufC::NewLC(1024*8);
	
	str->Des().FormatList(aFmt, argptr);
	
	TPtr16 ptr = str->Des();
	iIptvTestLogger->Log(_L("%S"), &ptr);

	CleanupStack::PopAndDestroy(str);
	
    VCXLOGLO1("<<<CIptvTestEcgUpdateInfo::WriteLog(formatted)");   
    }

//  End of File  
