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
* Description:   implements functionality for storing and logging*
*/




// INCLUDE FILES
#include "CIptvTestDlFileInfo.h"
#include "VCXTestLog.h"



// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT(KIPTVTestLoggerFolder,"IPTV");
_LIT(KIPTVTestLoggerFile,"IptvDlLog.txt");
// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::NewL
//  
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTestDlFileInfo* CIptvTestDlFileInfo::NewL()
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::NewL");   
    CIptvTestDlFileInfo* self = new (ELeave) CIptvTestDlFileInfo();
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop();
    VCXLOGLO1("<<<CIptvTestDlFileInfo::NewL");   
    return self;    
    }
// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::~CIptvTestDlFileInfo
// destructor
// -----------------------------------------------------------------------------
//    
EXPORT_C CIptvTestDlFileInfo::~CIptvTestDlFileInfo()
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::~CIptvTestDlFileInfo");
    if(iDlFileInfoError==EDlFileInfoDlGoing) 
        {
        iDlFileInfoError=EDlFileInfoDestructorCalled;
        iStopTime.HomeTime();
        Print();
        }
    
    iDlEvents.Close();
    
    if(iContentUrl)
        {
        delete iContentUrl;
        iContentUrl = NULL;
        }
        
    if(iIptvTestLogger)
	    {
	    delete iIptvTestLogger;
	    iIptvTestLogger=NULL;	
	    }
	    
    VCXLOGLO1("<<<CIptvTestDlFileInfo::~CIptvTestDlFileInfo");   
    }

// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::CIptvTestDlFileInfo
// 
// -----------------------------------------------------------------------------
//
CIptvTestDlFileInfo::CIptvTestDlFileInfo()
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::CIptvTestDlFileInfo");   
    
    VCXLOGLO1("<<<CIptvTestDlFileInfo::CIptvTestDlFileInfo");       
    }
// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::ConstructL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvTestDlFileInfo::ConstructL( )
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::ConstructL");   
    iIptvTestLogger = CIptvTestLogger::NewL(KIPTVTestLoggerFolder(), KIPTVTestLoggerFile());
    iDlSucceed = EFalse;
    VCXLOGLO1("<<<CIptvTestDlFileInfo::ConstructL");       
    }


// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::DlStarted
// stores download information
// -----------------------------------------------------------------------------
//
void CIptvTestDlFileInfo::DlStarted(
        							TUint32 aIapId,
        							TDesC& aIapName,
        							TIptvServiceId aServiceId,
        							TDesC& aServiceName,
        							TIptvContentId aContentId,
        							TIptvDownloadType aDlType,
        							HBufC* aDlUrl 
        							)
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::DlStarted"); 
      
    iStartTime.HomeTime();	
    iDlFileInfoError = EDlFileInfoDlGoing;
 	iIapId = aIapId;
 	iIapName.Append(aIapName);
 	iServiceId = aServiceId;
 	iServiceName.Append(aServiceName);
 	iContentId = aContentId;
 	iDlType = aDlType;
 	
 	if(!iContentUrl)
 	    {
 	    iContentUrl = aDlUrl;
 	    }
 	else
     	{
     	// panic?!
     	VCXLOGLO1("Pointer lost -> memory leak!!!!!!!!!!!!!!!!!!");       
     	}   
 	VCXLOGLO1("<<<CIptvTestDlFileInfo::DlStarted");   
	}
// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::DlUpdateState
// stores download information
// -----------------------------------------------------------------------------
//	        
void CIptvTestDlFileInfo::DlUpdateState( 
        						TUint32 aState,
        						TIptvDlError aError)
	{
	VCXLOGLO1(">>>CIptvTestDlFileInfo::DlUpdateState");
	
	TIptvTestDlEvents dlEvent;	
	dlEvent.SetValues(aState,aError);
    iDlEvents.Append(dlEvent);	  
	
	if(aState == KIptvVodDlDownloadSucceeded)
	{
	    iDlSucceed = ETrue;
	}
	
	VCXLOGLO1("<<<CIptvTestDlFileInfo::DlUpdateState");   	
	}
// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::DlGoing
// stores download information
// -----------------------------------------------------------------------------
//	        
void CIptvTestDlFileInfo::DlGoing( TDesC& aFullPath,
								TUint32 aState, 
								TIptvDlError aError )
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::DlGoing"); 
    iFullPath.Zero();  
	iFullPath.Append(aFullPath);
	TIptvTestDlEvents dlEvent;	
	dlEvent.SetValues(aState,aError);
    iDlEvents.Append(dlEvent);
	VCXLOGLO1("<<<CIptvTestDlFileInfo::DlGoing");   
    }
// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::DlStopped
// stores download information
// -----------------------------------------------------------------------------
//        						
void CIptvTestDlFileInfo::DlStopped(
						TUint32	aFileSize,
        				TUint32 aState, 
        				TIptvDlError aError)
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::NewL");   
	iStopTime.HomeTime();
	if(iDlFileInfoError==EDlFileInfoDlGoing)
	    {
	    iDlFileInfoError = EDlFileInfoIdle;
        }
	iFileSize = aFileSize;
	TIptvTestDlEvents dlEvent;	
	dlEvent.SetValues(aState,aError);
    iDlEvents.Append(dlEvent);
	VCXLOGLO1("<<<CIptvTestDlFileInfo::NewL");   
    }
// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::DlStartFailed
// stores download information
// -----------------------------------------------------------------------------
//        						
void CIptvTestDlFileInfo::DlStartFailed(
        				TIptvDlError aError)
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::DlStartFailed");   
	iStopTime.HomeTime();
	TIptvTestDlEvents dlEvent;	
	dlEvent.SetValues(0,aError);
    iDlEvents.Append(dlEvent);
	iDlFileInfoError = EDlFileInfoStartFailed;
	VCXLOGLO1("<<<CIptvTestDlFileInfo::DlStartFailed");   
    }
// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::GetFileName
// retuns fullpath of the file
// -----------------------------------------------------------------------------
//
TDesC& CIptvTestDlFileInfo::GetFileName()
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::GetFileName");   
    VCXLOGLO1("<<<CIptvTestDlFileInfo::GetFileName");   
    return iFullPath;
    }
// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::DlTimeOut
// Prints and removes download information when download didn't get ready in time.
// -----------------------------------------------------------------------------
//        							
void CIptvTestDlFileInfo::DlTimeOut()
    {
    VCXLOGLO1(">>>CIptvTestDlFileInfo::DlTimeOut"); 
    iStopTime.HomeTime();
    iDlFileInfoError = EDlFileInfoTimeOut;
    VCXLOGLO1("<<<CIptvTestDlFileInfo::DlTimeOut"); 
    }
// -----------------------------------------------------------------------------
// CIptvTestDlFileInfo::Print
// Print information about download to log
// -----------------------------------------------------------------------------
//        							
void CIptvTestDlFileInfo::Print()
{
    VCXLOGLO1(">>>CIptvTestDlFileInfo::Print");   
	_LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3");
    _LIT(KTimeString,"%-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");

    TBuf<255> logBuf;

	TBuf<30> startDateString;
    TBuf<30> startTimeString;    
    
    TBuf<30> endDateString;
    TBuf<30> endTimeString;
    
    TBuf<30> eventTimeString;
    
    TTimeIntervalSeconds tTimeSeconds;
    TUint32 dlTime = 0;
    TUint32 dlSpeed = 0;
    TInt i = 0;
    
    iStartTime.FormatL(startDateString,KDateString);
    iStartTime.FormatL(startTimeString,KTimeString);
    
    iStopTime.FormatL(endDateString,KDateString);
    iStopTime.FormatL(endTimeString,KTimeString);
	
	iIptvTestLogger->Log(_L("--------------------------------------------------------------------------"));
    iIptvTestLogger->Log(_L("Download start: %S %S"),&startDateString, &startTimeString);
    logBuf.AppendFormat(_L("serviceid: %d, Servicename: %S"),iServiceId, &iServiceName);    
    iIptvTestLogger->Log(_L("%S"),&logBuf);
    logBuf.Zero();
    
    logBuf.AppendFormat(_L("IapId: %d, Iapname: %S"),iIapId, &iIapName);
    iIptvTestLogger->Log(_L("%S"),&logBuf);
    logBuf.Zero();
    
    logBuf.AppendFormat(_L("ContentId: %d"),iContentId);
    iIptvTestLogger->Log(_L("%S"),&logBuf);
    logBuf.Zero();
    if(iContentUrl)
        {
        iIptvTestLogger->Log(_L("Download URL: %S"),iContentUrl);
        }
    else
        {
        iIptvTestLogger->Log(_L("Download URL: not found"));    
        }
    iIptvTestLogger->Log(_L("Videoname: %S"),&iFullPath);
    
    logBuf.AppendFormat(_L("Filesize (bytes): %d"),iFileSize);
    iIptvTestLogger->Log(_L("%S"),&logBuf);
    logBuf.Zero();
     
    iIptvTestLogger->Log(_L("Download stop: %S %S"),&endDateString, &endTimeString);
    
    //Download time
    iStopTime.SecondsFrom(iStartTime,tTimeSeconds);//calculates time difference
    dlTime = tTimeSeconds.Int();
    logBuf.AppendFormat(_L("Download time (sec): %d"),dlTime);
    iIptvTestLogger->Log(_L("%S"),&logBuf);
    logBuf.Zero();
    
    //Download speed  
    if(dlTime>0)
        {
        dlSpeed = iFileSize / dlTime;    
        }
    logBuf.AppendFormat(_L("Download speed (bytes/sec): %d"),dlSpeed);
    iIptvTestLogger->Log(_L("%S"),&logBuf);
    logBuf.Zero();
    
    iIptvTestLogger->Log(_L("Events from Iptvserver:"));
    
    for(i = 0; i < iDlEvents.Count(); i++)
        {
        iDlEvents[i].iTime.FormatL(eventTimeString,KTimeString);
        logBuf.AppendFormat(_L("time: %S state: %d   error: %d"),&eventTimeString, iDlEvents[i].iState,iDlEvents[i].iError);
        iIptvTestLogger->Log(_L("%S"),&logBuf);
        logBuf.Zero();
        }
        
    if(iDlSucceed)
        {
        iIptvTestLogger->Log(_L("Download succeeded"));
        }
    else
        {
        iIptvTestLogger->Log(_L("Download didn't succeed"));
        }
    
    if(iDlFileInfoError==EDlFileInfoTimeOut)
        {
        iIptvTestLogger->Log(_L("Timeout, download didn't get ready!"));
        }
    else if(iDlFileInfoError==EDlFileInfoStartFailed)
        {
        iIptvTestLogger->Log(_L("Download didn't start at all!"));
        }
    else if(iDlFileInfoError==EDlFileInfoDestructorCalled)
        {
        iIptvTestLogger->Log(_L("Destructor of testcase called, download didn't get ready!"));
        }
    else
        {
        //do nothing    
        }
    
    iIptvTestLogger->Log(_L("--------------------------------------------------------------------------"));   
    VCXLOGLO1("<<<CIptvTestDlFileInfo::Print");   
}
TIptvTestDlEvents::TIptvTestDlEvents()
    {
    iState = 0;
    }

TIptvTestDlEvents::~TIptvTestDlEvents()
    {
    }
    
void TIptvTestDlEvents::SetValues(
                                TUint32 aState,
                                TIptvDlError aError)
    {
    iTime.HomeTime();
	iState = aState;
	iError  = aError; 
    }
//  End of File  
/*
--------------------------------------------------------------------------
Download start failed: 22/06/2006 16:38:33.05
Serviceid: 0, Servicename: Kärppäkanava
IapId: 11, Iapname: Ethernet with Daemon Dynamic IP
ContentId = 2
Dltype = 2
error = 4
--------------------------------------------------------------------------

--------------------------------------------------------------------------
Download start: 22/06/2006 16:38:33.05
Serviceid: 0, Servicename: Kärppäkanava
IapId: 11, Iapname: Ethernet with Daemon Dynamic IP
ContentId = 2
Videoname: Karpat-Assat 2-1
Filename: e:\E147423941116553376000000003.3gp
Filesize (bytes):1942653
Download time (s):102
Download stop: 22/06/2006 16:39:14.28
error = 133
Downloads going start: 1    end: 1
--------------------------------------------------------------------------

*/
