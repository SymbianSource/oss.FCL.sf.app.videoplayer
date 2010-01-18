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
* Description:   class to store information about download*
*/




#ifndef CIPTVTESTDLFILEINFO_H
#define CIPTVTESTDLFILEINFO_H

//  INCLUDES
#include <e32base.h>
#include "CIptvTestLogger.h"
#include "CIptvUtil.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CIptvTestLogger;

//small class for saving download events
NONSHARABLE_CLASS(TIptvTestDlEvents)
    {
    public:
        TIptvTestDlEvents(); 
        virtual ~TIptvTestDlEvents();
        void SetValues(TUint32 aState, TIptvDlError aError);
               
    public:
        TTime iTime;
        TUint32 iState;
        TIptvDlError iError;
    };


//class for saving fileinfo while downloading

/**
*  @since
*/

    
class CIptvTestDlFileInfo : public CBase
    {
    public:  // Constructors and destructor
        
         /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvTestDlFileInfo* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvTestDlFileInfo();
        
        enum TDlFileInfoError
            {
            EDlFileInfoIdle,
            EDlFileInfoDlGoing,
            EDlFileInfoTimeOut,
            EDlFileInfoStartFailed,
            EDlFileInfoDestructorCalled
            };
					
		void DlStarted(
						TUint32 aIapId,
						TDesC& aIapName,
						TIptvServiceId aServiceId,
						TDesC& aServiceName,
						TIptvContentId aContentId,
						TIptvDownloadType aDlType,
						HBufC* aDlUrl 
						);
        
        void DlUpdateState( 
						TUint32 aState, 
						TIptvDlError aError);
        
        void DlGoing( 
				TDesC& aFullPath,
				TUint32 aState, 
				TIptvDlError aError );
        						
        void DlStopped(
    			TUint32	aFileSize, 
    			TUint32 aState, 
    			TIptvDlError aError);
    			
    	void DlTimeOut();
        							
        void DlStartFailed(TIptvDlError aError);
        							
        void Print();
        
        TDesC& GetFileName();
    
    protected: 
	    
   
    private:
				/**
        * C++ default constructor.
        */
        CIptvTestDlFileInfo();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
      
    public:
    	TUint32 iServiceId;
    	TUint32 iContentId;
    
    private:    // Data
    	TUint32 	iIapId;
    	TBuf<255> 	iIapName;
    	TBuf<255> 	iServiceName;
    	HBufC*      iContentUrl;    			
    	TIptvDownloadType iDlType;
        TBuf<255>   iFullPath;
        TBool       iDlSucceed;
        TUint32		iFileSize;
        TTime       iStartTime;
        TTime		iStopTime;
        TDlFileInfoError iDlFileInfoError;
        
        CIptvTestLogger* iIptvTestLogger;

        RArray<TIptvTestDlEvents> iDlEvents;
    
    };

#endif      // CIPTVTESTDLFILEINFO_H   
            
// End of File

