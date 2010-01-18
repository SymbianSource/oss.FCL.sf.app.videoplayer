/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description*
*/



#ifndef IPTVVODCONTENTAPITEST_H
#define IPTVVODCONTENTAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include "MIptvVodContentClientObserver.h"

#include "CIptvVodContentClient.h"
#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"
#include "MIptvTestTimerObserver.h"
#include "TIptvRssSearchQuery.h"

// CONSTANTS
const TInt KBadCharacters = 6001;
const TInt KIptvTestSearchCategoryId = -3; // STIF does not understand real search category id which is KMaxTUint32 - 1

_LIT( KBadCharactersString, "\t\\\"\a\b\f\n\r\t\v\'\?");
// Logging path
_LIT( KIptvVodContentApiTestLogPath, "\\logs\\testframework\\IptvVodContentApiTest\\" );
// Log file
_LIT( KIptvVodContentApiTestLogFile, "IptvVodContentApiTest.txt" );


// MACROS

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvVodContentApiTest;
class CIptvVodContentClient;
class CIptvTestTimer;
class CIptvTestUtilities;
class CIptvTestEcgUpdateInfo;

// DATA TYPES

// CLASS DECLARATION

/**
*  CIptvVodContentApiTest test class for STIF Test Framework TestScripter.
*
*  @lib
*  @since
*/
NONSHARABLE_CLASS(CIptvVodContentApiTest) : public CScriptBase, public MIptvVodContentClientObserver, public MIptvTestTimerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIptvVodContentApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CIptvVodContentApiTest();

        /**
        * TIptvVodContentApiTestSpecialAction
        */
        enum TIptvVodContentApiTestSpecialAction
            {
            ENone,
            EIgnoreUpdateNotStarted,    // Used when testing sending multiple UpdateECG requests without waiting previous to complete.
            EBriefContentDetails,       // Writes only one line into verify file from content getters
            EUpdateSucceedsAlways,      // ECG update always signals KErrNone to script, possible errors show only in log
            };

    public: // New functions


    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since
        * @param aItem                  Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        /**
        * From MIptvVodContentClientObserver Messages from EPG manager are handled here. Messages are defined in CIptvUtil.h
        * @since
        * @param aMsg                   Message
        * @param aInfo                  Info
        * @param aServiceId             Service id.
        */
        virtual void HandleEpgManagerMsgL(TInt aMsg, TInt aInfo, TIptvServiceId aServiceId);

        /**
        * From MIptvTestTimerObserver Asynchronous callback observer function signaling application that timer has elapsed.
        * @since
        * @param aError                 Error code.
        */
        virtual void TimerComplete(TInt aTimerId, TInt aError);

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CIptvVodContentApiTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since
        */
        void Delete();

        /**
        * Test methods are listed below.
        */

        /**
        * Enables writing the content data etc. into file which can be used for verifying the test case result.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt EnableVerifying( CStifItemParser& aItem );

        /**
        * Compares the current verifying file against file which has been verified by the user.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt Verify( CStifItemParser& aItem );

        /**
        * This is used to perform more rare and special actions. See TIptvVodContentApiTestSpecialAction.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt SetSpecialAction( CStifItemParser& aItem );

        /**
        * Creates and initializes the test module.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt CreateL( CStifItemParser& aItem );

        /**
        * Deletes all Video Center EPG data.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt DeleteEpgDb( CStifItemParser& aItem );

        /**
        * Tests CIptvVodContentClient::GetEcgListL.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt GetEcgList( CStifItemParser& aItem );

        /**
        * Tests CIptvVodContentClient::GetEcgCategoryListL.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt EcgCategoryList( CStifItemParser& aItem );

        /**
        * Tests CIptvVodContentClient::GetEcgAllListL.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt EcgEcgAllList( CStifItemParser& aItem );

        /**
        * CIptvVodContentClient::GetCategoryDetailsL
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt CategoryDetails( CStifItemParser& aItem );

        /**
        * Calls CIptvVodContentClient::GetEcgCategoryListL and verifies that CIptvVodContentClient::GetParentCategoryL for
        * all returned categories returns the correct category Id.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt ParentCategory( CStifItemParser& aItem );

        /**
        * Calls GetContentDetailsL.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt ContentDetailsList( CStifItemParser& aItem );

        /**
        * Prints CIptvVodContentCategoryBriefDetails of defined category and CIptvVodContentContentBriefDetails and CIptvVodContentContentFullDetails
        * for all the content of the category.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt VerifyAllCategoryContentL ( CStifItemParser& aItem );

        /**
        * Starts asyncronous ECG update by calling CIptvVodContentClient::UpdateEcgL.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt UpdateEcg( CStifItemParser& aItem );

        /**
        * Sets the timeout in seconds for the case.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt SetTestCaseTimeOut( CStifItemParser& aItem );

        /**
        * Gets id for the defined IAP name and calls CIptvVodContentClient::SetIapL.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt SelectIapL( CStifItemParser& aItem );

        /**
        * Calls CIptvVodContentClient::GetUpdateTimeL.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt GetUpdateTimeL( CStifItemParser& aItem );

        /**
        * Disables thumbnail downloading for Video Center using cenrep.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt DisableThumbnails( CStifItemParser& aItem );

        /**
        * Enables thumbnail downloading for Video Center using cenrep.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt EnableThumbnails( CStifItemParser& aItem );

        /**
        * Sets Video Center parental control age in the cenrep.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt SetParentalControlAge( CStifItemParser& aItem );

        /**
        * Starts asynchronous search operation by calling CIptvVodContentClient::Search.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt Search( CStifItemParser& aItem );

        /**
        * Calls CIptvVodContentClient::CancelSearch.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt CancelSearch( CStifItemParser& aItem );

        /**
        * Calls CIptvVodContentClient::CancelUpdate.
        * @since
        * @param aItem                  No params.
        * @return Symbian OS error code.
        */
        virtual TInt CancelUpdate( CStifItemParser& aItem );
        
        /**
        * Calls CIptvVodContentClient::SetLastPositionL.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt SetLastPosition( CStifItemParser& aItem );
        
        /**
        * Calls CIptvVodContentClient::UpdateAvailableL.
        * @since
        * @param aItem TBool, ETrue if there's need for update.
        * @return Symbian OS error code.
        */
        virtual TInt UpdateAvailableL( CStifItemParser& aItem );
        
        /**
        * Verifies category count.
        * @since
        * @param aCount                  Expected count of categories. 
        * @return Symbian OS error code.
        */
        virtual TInt CategoryCountL( CStifItemParser& aItem );
        
        /**
        * Verifies content count.
        * @since
        * @param aCount                  Expected count of content. 
        * @return Symbian OS error code.
        */
        virtual TInt ContentCountL( CStifItemParser& aItem );        
        
        /**
        * Deletes the resources used by the test module.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt Destroy( CStifItemParser& aItem );

        /**
        * Sets message which will be signaled to the test scripter.
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt SetMsgWhichToSignal( CStifItemParser& aItem );

        /**
        * Sets message which will be ignored
        * @since
        * @param aItem                  Contains the parameters for the method.
        * @return Symbian OS error code.
        */
        virtual TInt IgnoreMsg( CStifItemParser& aItem );

        /**
        * Other methods
        */

        /**
        * Creates the instance of CIptvVodContentClient for defined service and initializes other member variables.
        * @since
        * @param aServiceName           Service name or order number in services database.
        * @return Symbian OS error code.
        */
        TInt CreateInstance(TDesC& aServiceName);

        /**
        * Deallocates the resources.
        */
        void DeAllocateResources();

        /**
        * TestGetEcgList
        */
        TInt TestGetEcgList(TIptvCategoryId aCategoryId, TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount);

        /**
        * TestGetEcgCategoryList
        */
        TInt TestGetEcgCategoryList(TIptvCategoryId aParentCategoryId);

        /**
        * TestGetEcgAllList
        */
        TInt TestGetEcgAllList(TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount);

        /**
        * TestGetEcgCategoryDetails
        */
        TInt TestGetEcgCategoryDetails(TIptvCategoryId aCategoryId);

        /**
        * TestGetParentCategory
        */
        TInt TestGetParentCategory(TIptvCategoryId aParentCategoryId);

        /**
        * TestGetContentDetailsList
        */
        TInt TestGetContentDetailsList(TIptvContentId aContentId);

        /**
        * TestUpdateEcg
        */
        TInt TestUpdateEcg( );

        /**
        * TestGetUpdateTimeL
        */
        TInt TestGetUpdateTimeL( TTime& aTime );

        /**
        * TestVerifyAllCategoryContentL
        */
        TInt TestVerifyAllCategoryContentL(TIptvCategoryId aParentCategoryId);

        /**
        * Test category count.
        * @since
        * @param aCount                  Expected count of categories. 
        * @return Symbian OS error code.
        */
        virtual TInt TestCategoryCountL( TInt aExpectedCount );
        
        /**
        * Test content count.
        * @since
        * @param aCount                  Expected count of content. 
        * @return Symbian OS error code.
        */
        virtual TInt TestContentCountL( TInt aExpectedCount );  
        
        /**
        * PrintVodContentBriefDetails
        */
        void PrintVodContentBriefDetails(CIptvVodContentContentBriefDetails& aDetails);

        /**
        * PrintVodContentFullDetails
        */
        void PrintVodContentFullDetails(TUint32 aContentId, CIptvVodContentContentFullDetails& aDetails);

        /**
        * PrintVodContentCategoryBriefDetails
        */
        void PrintVodContentCategoryBriefDetails(CIptvVodContentCategoryBriefDetails& aDetails);

        /**
        * WriteVerifyVodContentBriefDetails
        */
        void WriteVerifyVodContentBriefDetails(CIptvVodContentContentBriefDetails& aDetails);

        /**
        * WriteVerifyVodContentFullDetails
        */
        void WriteVerifyVodContentFullDetails(TUint32 aContentId, CIptvVodContentContentFullDetails& aDetails);

        /**
        * WriteVerifyVodContentCategoryBriefDetails
        */
        void WriteVerifyVodContentCategoryBriefDetails(CIptvVodContentCategoryBriefDetails& aDetails);

        /**
        * WriteVerifyCAList
        */
        void WriteVerifyCAList(RPointerArray<CIptvMediaContent>& aCAList);

        /**
        * SetIap
        */
        TInt SetIap(const TUint32 aIap);

        /**
        * AsyncCompleted
        */
        void AsyncCompleted(TInt aError);

        /**
        * GetDateInts
        */
        void GetDateInts(TDateTime aDateTime, TInt& year, TInt& month, TInt& day, TInt& hour, TInt& minute, TInt& second );

        /**
        * RemoveServiceNumberFromThumbnailPathL
        */
        void RemoveServiceNumberFromThumbnailPathL(TDes16& aThumbnailPath);

    public:     // Data

    protected:  // Data

    private:    // Data

        // Instance of VodContentClient
        CIptvVodContentClient*   iIptvVodContenClient;

        //Update log
        CIptvTestEcgUpdateInfo* iIptvTestEcgUpdateInfo;

        // ID of current service
        TIptvServiceId iServiceId;

        // ID of used IAP
        TUint32 iIapId;

        // Instance of IptvTestTimer
        CIptvTestTimer*          iIptvTestTimer;

        //Instance of IptvTestUtility class
        CIptvTestUtilities* iIptvTestUtilities;

        // Current CaseID
        TBuf<32> iCaseId;

        // Current SpecialAction
        TIptvVodContentApiTestSpecialAction iSpecialAction;

        // Was last ECG update successfull
        TBool iEcgUpdateSuccesfull;

        // Timeout in minutes
        TInt iTimeoutMinutes;

        // The expected count of downloaded thumbnails.
        TInt iExpectedThumbnailCount;

        // Current count of downloaded thumbnails
        TInt iThumbnailCount;

        // The last update time for service when update started.
        TTime iLastUpdateTimeFromService;

        // EPG message which will be signaled to the test scripter.
        TInt iMsgWhichToSignal;

        // Next EPG msg which matches with this is ignored.
        TInt iIgnoreMsg;

        // Retries to do if update fails.
        TInt iRetryCount;

        // String used in search
        TIptvRssSearchQuery iSearchQuery;
    };

#endif      // IPTVVODCONTENTAPITEST_H

// End of File
