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


#ifndef CVCXSCHEDULEDDOWNLOADTESTER_H_
#define CVCXSCHEDULEDDOWNLOADTESTER_H_

// INCLUDES
#include <e32base.h>
#include "VCXTestMyVideosObserver.h"
#include "VCXTestStifScriptBaseExt.h"
#include "VCXTestMessageWait.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CVCXTestMyVideosCollectionClient;
class CCseSchedulerApi;

// DATA TYPES

// CLASS DECLARATION

/**
*  CVCXScheduledDownloadTester 
*
*  @lib ?library
*  @since ?Series60_version
*/
class CVCXScheduledDownloadTester : public CBase, 
                                    public MVCXTestMyVideosObserver, 
                                    public MVCXTestMessageWaitObserver
    {
        
    public:  // Constructors and destructor
         
         /**
         * Two-phased constructor.
         */
         static CVCXScheduledDownloadTester* NewL( CVCXTestStifScriptBaseExt* aStifScriptBase );
    
         /**
         * Destructor.
         */
         virtual ~CVCXScheduledDownloadTester();     
    
    private:
        
        /**
        * C++ default constructor.
        */
        CVCXScheduledDownloadTester( CVCXTestStifScriptBaseExt* aStifScriptBase );
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
        
    private: // Functions from base classes.
        
        /**
         *  
         */
        void VideoEventL( TVCXTestVideoEvent aEvent, TUint32 aParam1, TUint32 aParam2, TInt aError );
    
        void MessageWaitComplete( TInt aError );
        
    public: // New functions
    
        void RefreshL();
        
        void RemoveAllMediasL();
        
        void RemoveVideoL( TUint aIndex );
        
        void DownloadL( const TDesC& aUrl, TUint aIapId, const TDesC& aUserName, const TDesC& aPassword );
        
        void CancelDownloadL( TUint aIndex );
        
        void WaitForVideoEventsL( TInt aTimeoutSeconds, RArray<TInt32> aEvents );
        
        void CoolDown();
        
        TInt VideoCount();
        
        TInt DownloadCount();
        
        void RemoveAllSchedulesL();
        
        TInt GetLastError();
        
    private: // Data

        CVCXTestMyVideosCollectionClient* iMyVideosCollectionClient;
        
        CVCXTestStifScriptBaseExt* iStifScriptBase;
        
        CVCXTestMessageWait* iMessageWait;
        
        CCseSchedulerApi*   iSchedulerClient;
        
        TInt iError;
    };

#endif /* CVCXSCHEDULEDDOWNLOADTESTER_H_ */
