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
* Description:   Object to offer some common utility functions to Iptv engine testing*
*/




#ifndef CIPTVTESTVCAPIUTIL_H
#define CIPTVTESTVCAPIUTIL_H

//  INCLUDES
//#include <TIptvIapList.h>
//#include <CIptvService.h>
#include "CIptvVodContentClient.h"
#include "MIptvVodContentClientObserver.h"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
//class CIptvService;
class CIptvVodContentContentFullDetails;
class CIptvTestVodContentAllDetails;

// CLASS DECLARATION

/**
*
*
*
*
*  @since
*/
class CIptvTestVCApiUtil : public CBase, public MIptvVodContentClientObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvTestVCApiUtil* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvTestVCApiUtil();


    public:
		//functions from baseclasses
		void HandleEpgManagerMsgL(TInt aMsg, TInt aInfo, TIptvServiceId aServiceId);

        // New functions
        /**
        *
        * @since
        * @param
        */
		TBool GetContentIdsL(TIptvServiceId aServiceId, TUint32 aIAPId, RArray<TIptvContentId>& aContentIdArray, TUint32 aMaxSize, TUint32 aMaxPlaytime);

	    /**
        *
        */
        void GetEcgCategoryListL(TUint32 aServiceId, TUint32 aParentCategoryId, RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList);

        /**
        *
        */
        void GetEcgListL(TUint32 aServiceId, TUint32 aCategoryId, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList);
		
		/**
        *
        */
		TInt GetVodContentContentDetailsL(TUint32 aServiceId, TUint32 aContentId, CIptvVodContentContentFullDetails& aContentDetails);

		/***
		*
		*/
		TInt GetContentAccessListL(TUint32 aServiceId, TUint32 aContentId, RPointerArray<CIptvMediaContent>& aCAList);

        /**
        *
        */
		void GetVodContentAllDetailsListL( TUint32 aServiceId, RPointerArray<CIptvTestVodContentAllDetails>& aDetailsList );

    private:

        /**
        * C++ default constructor.
        */
        CIptvTestVCApiUtil();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        enum TIptvTestActiveWaitCmd
        	{
        	EActiveWaitStart,
        	EActiveWaitStop
        	};

        void ActiveWait( TIptvTestActiveWaitCmd aActiveWaitCmd );
        
        /**
         * Checks that VOD content client member is up to date. 
         */
        void CheckVodContentClient( TInt aServiceId );

    private:    // Data
        TInt iCurrentService; 
        CIptvVodContentClient*  iIptvVodContentClient;
        CActiveSchedulerWait* 	iActiveSchedulerWait;
        
	    TInt iEpgUpdateError;
    };


#endif      // CIPTVTESTVCAPIUTIL_H

// End of File
