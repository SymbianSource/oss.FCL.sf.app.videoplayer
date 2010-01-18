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


#ifndef CIPTVTESTUTILITIES_H
#define CIPTVTESTUTILITIES_H

//  INCLUDES
#include "CIptvIapList.h"
#include "MTestUtilConnectionObserver.h"
#include "VCXTestConstants.h"
#include "VCXTestCommon.h"

#include "CIptvService.h"
#include "CIptvTestVerifyData.h"
#include "CIptvTestVideoCreator.h"
#include "CIptvServiceManagementClient.h"

// MACROS
// FORWARD DECLARATIONS
//class CIptvService;
class CIptvTestSMApiUtil;
class CIptvTestMVApiUtil;
class CIptvTestVCApiUtil;
class CIptvMyVideosGlobalFileId;
class CIptvIapList;
class CIptvTestVodContentAllDetails;
class CIptvVodContentContentFullDetails;
class CIptvVodContentContentBriefDetails;
class CIptvVodContentCategoryBriefDetails;
class CIptvMyVideosVideoFullDetails;
class CIptvMediaContent;

// DATA TYPES

class CIptvServiceInfo : public CBase
	{
	public:

        /**
        * Two-phased constructor.
        */
		IMPORT_C static CIptvServiceInfo* NewL();
		IMPORT_C static CIptvServiceInfo* NewL(CIptvServiceInfo& aServiceInfo);

        /**
        * Destructor
        */
		virtual ~CIptvServiceInfo();


	private:

        /**
        * C++ default constructor.
        */
        CIptvServiceInfo();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CIptvServiceInfo* aServiceInfo);

	public:

		TUint iId;
		TBuf16<255> iName;
		TBuf16<255> iAddress;
		TBuf16<255> iIconPath;
		TUid iEpgPluginUid;
		TUid iVodPluginUid;
		TUid iIptvPluginUid;
		CIptvService::TServiceType iServiceType;
		CIptvIapList* iIapList;
		TBuf16<255> iUserName;
		TBuf16<255> iPassWord;
		TBuf16<255> iDesc;
		TBool iSelectedFlag;
	    TBool iReadOnlyFlag;
	    TBool iMainServiceFlag;
	    TBool iConnectionApprovedFlag;
	    TInt32 iApplicationUid;
	    TInt32 iDisplayOrder;
	    TBuf16<255> iProviderId;
	    TBuf16<255> iAccountManagementUrl;
	};

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*  Class for helper functions that have depencies to Video Center.
*  Add functions without depencies to VCXTestCommon.
*
*  @since
*/
class CIptvTestUtilities : public CBase, public MTestUtilConnectionObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvTestUtilities* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvTestUtilities();

    public: // New functions

        /**
        * Prints service information to debug output
        * @since
        * @param
        */
        IMPORT_C void PrintIptvService(CIptvService *aIptvService);

        /**
        * Writes service information into verify stream.
        * @since
        * @param
        */
        IMPORT_C void WriteVerifyIptvService(TDes& aCaseId, CIptvService *aIptvService);

        /**
        * Creates instance of service class.
        * @since
        * @param
        */
        IMPORT_C CIptvService* CreateIptvServiceInstanceL( CIptvServiceInfo& aService );

        /**
        * Compares two services.
        * @since
        * @param
        */
        IMPORT_C TBool VerifyServices(CIptvService& aS1, CIptvService& aS2);

        /**
        * Gets VOD content IDs for a service.
        * @since
        * @param
        */
        IMPORT_C TBool GetContentIdsL(TIptvServiceId aServiceId, TUint32 aIapId, RArray<TIptvContentId>& aContentIdArray, TUint32 aMaxSize, TUint32 aMaxPlaytime);

        /**
        * Gets size of a file.
        * @since
        * @param
        */
        IMPORT_C TBool GetFileSize(TDesC& aFullPath, TUint32& aFileSize);

        /**
        * Gets service from service DB.
        * @since
        * @param
        */
        IMPORT_C CIptvService* GetServiceFromDb(TInt aId);

        /**
        * Searches AP by name.
        * First searches if there's IAP matchin for the name. After that following rules are used:
        * If name is Internet tries to find first GPRS AP, Internet2 second GPRS AP
        * If name is Wlan tries to find first WLAN AP, Wlan2 second WLAN AP
        * If AP is not found returns on of the known APs, if one is found.
        */
        IMPORT_C TBool GetIapIdL(const TDesC& aIapName, TUint32& aIapId);

        /**
        * Searches AP by name.
        */
		IMPORT_C TBool GetIapIdByNameL(const TDesC& aIapName, TUint32& aIapId);

        /**
        * Searches for AP as defined in TIptvTestIapType. In emulator always returns "Ethernet with Daemon Dynamic IP"
        * @since
        * @param aType AP type which is wanted
        * @param aIapId id of found AP is written here
        * @param aOrderNumber return nTh AP of the specified type
        * @return KErrNotFound if no AP with the type is found, KErrNone if no errors,
        */
		IMPORT_C TInt GetIapIdByTypeL(CVCXTestCommon::TIptvTestIapType aType, TUint32& aIapId, TInt aOrderNumber);

        /**
        * Returns a name of AP
        */
        IMPORT_C TBool GetIapNameById(TDes& aIapName, TUint32 aIapId);

        /**
        * Prints APs in COMMDB into debug output.
        */
        IMPORT_C void PrintIaps();

        /**
        * Returns used AP for a service.
        */
        IMPORT_C TBool GetUsedIap(TUint32 aServiceId, TUint32& aIapId,TDes& aIapName);

        /**
        * Sets used AP for a service.
        * @return KErrNone if succeed, otherwise an error code
        */
	    IMPORT_C TInt SetUsedIap(TUint32 aServiceId, TUint32 aIapId);

    	/**
        * Gets the services from the services DB
        * @param aServicesArray array where services will be added
        * @param aFlagsFilter only services with these flags will be added, use 0 to skip flags test
        * @param aOrder CIptvServiceManagementClient::TOrder enumeration to sort the services array or -1 to use no sort
        */
		IMPORT_C TBool GetServicesFromDbL(RPointerArray<CIptvService>& aServicesArray, TUint32 aFlags, CIptvServiceManagementClient::TOrder aOrder);

        /**
        * Gets the services from the services DB
        * @param aServicesArray array where services will be added
        */
        IMPORT_C TBool GetServicesFromDbL(RPointerArray<CIptvService>& aServicesArray);

        /**
        * Gets ID of service by name.
        * @since
        * @param
        */
		IMPORT_C TInt GetServiceIdByNameL(TDesC& aServiceName, TInt& aId);

        /**
        * Gets video info.
        * @since
        * @param
        */
        TBool GetVideoInfo(CIptvMyVideosGlobalFileId& aFileId,
                            TUint32& aServiceId,
                            TDes& aName,
                            TDes& aFileName);

  		/**
        * Initializes the verify file, deletes if the file exists
        * @since
        * @param aVerifyId An id to identify the file where verify data is written
        * @return Error code, KErrNone if no errors,
        */
		IMPORT_C TInt CreateVerifyData(TDesC& aVerifyId, TBool aAppend);

		/**
        * Writes verify data to a file
        * @since
        * @param aVerifyId An id to identify the file where verify data is written
        * @param aVerifyData Data to write into the file
        * @return Error code, KErrNone if no errors,
        */
		IMPORT_C TInt WriteVerifyData(TDesC& aVerifyId, TDesC& aVerifyData);

		/**
        * Writes verify data to a file
        * @since
        * @param aVerifyId An id to identify the file where verify data is written
        * @param aVerifyData Data to write into the file
        * @return Error code, KErrNone if no errors,
        */
		IMPORT_C TInt WriteVerifyData(TDesC& aVerifyId, TRefByValue<const TDesC> aFmt, ... );

		/**
        *
        * @since
        * @param aVerifyId An id to identify the files which are compared
        * @param aVerifyResult Result is written here
        * @return Error code, KErrNone if no errors,
        */
		IMPORT_C TInt VerifyData(TDesC& aVerifyId, CIptvTestVerifyData::TVerifyResult& aVerifyResult);

        /**
        * Enables timestamp writing into the verify file.
        * @since
        * @param aUseTimestamps
        */
        IMPORT_C void EnableVerifyTimestamps( TBool aUseTimestamps );

        /**
        * Gets content access list
        * @since
        * @param
        */
		IMPORT_C TInt GetContentAccessListL(TUint32 aServiceId, TUint32 aContentId, RPointerArray<CIptvMediaContent>& aCAList);

        /**
        * Gets content details
        * @since
        * @param
        */
		IMPORT_C TInt GetVodContentContentDetailsL(TUint32 aServiceId, TUint32 aContentId, CIptvVodContentContentFullDetails& aContentDetails);

        /**
        * Gets ECG category list for a service.
        */
        IMPORT_C void GetVodEcgCategoryListL(TUint32 aServiceID, TUint32 aParentCategoryId, RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList);

        /**
        * Gets ecg list for a service.
        */
        IMPORT_C void GetVodEcgListL(TUint32 aServiceId, TUint32 aCategoryId, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList);

        /**
        * Gets all content details
        * @since
        * @param
        */
		IMPORT_C void GetVodContentAllDetailsListL( TUint32 aServiceId, RPointerArray<CIptvTestVodContentAllDetails>& aDetailsList );

        /**
        * Gets details for a video
        * @since
        * @param
        */
		IMPORT_C TInt GetMyVideosVideoDetailsL(CIptvMyVideosGlobalFileId& aFileId, CIptvMyVideosVideoFullDetails& aVideoDetails);

        /**
        * Converts string to integer.
		* @param aInt parsed int is stored here
        * @param aString string containing the number
        * @return KErrNone if succeed, otherwise an error code
        */
		IMPORT_C static TInt ParseIntFromString(TInt& aInt, TDesC& aString);

        /**
        * Creates bad iap for testing purposes
        * @param aName name of the iap to be created
        */
		IMPORT_C TUint32 CreateBadIapL(TDesC& aName);

        /**
        * TInt ReallycleanL();
        * @param  -
        * @return error
        */
		IMPORT_C TInt ReallycleanL();

       /**
        * TInt DeleteVideoL();
        * @param  -
        * @return error
        */
		IMPORT_C TInt DeleteVideoL(CIptvMyVideosGlobalFileId& aFileId);

		/**
        * static TInt AdvanceSystemTime1Second();
        * @return KErrNone if succeed, otherwise an error code
        */
		IMPORT_C static TInt AdvanceSystemTime1Second();

        /**
        * static TInt SetSystemTime();
        * @return KErrNone if succeed, otherwise an error code
        */
		IMPORT_C static TInt SetSystemTime(TTime aTime);

        /**
        * static TInt SetTimeZone();
        * @return KErrNone if succeed, otherwise an error code
        */
		IMPORT_C static TInt SetTimeZone(TDesC& aTimeZone);

        /**
        * static TInt RefreshService(TUint32 aServiceId);
        * @return KErrNone if succeed, otherwise an error code
        */
	    IMPORT_C TInt RefreshService(TUint32 aServiceId, TTime aLastDownloadTime);

	    /*
	     * Creates a video file to the system.
	     */
	    IMPORT_C void CreateVideoFileL( CIptvTestVideoCreator::TIptvTestVideoType aVideoType, TDesC& aFileName, TInt aSize );

        /*
         * Creates video files to the system.
         */
        IMPORT_C void CreateVideoFilesL( CIptvTestVideoCreator::TIptvTestVideoType aVideoType, TDesC& aFileName, TInt aCount, RPointerArray<HBufC>& aFileArray );

	    /*
	     * Sets free space for a drive.
	     */
	    IMPORT_C void SetDriveFreeSpaceL( TInt aDriveNumber, TUint aDesiredFreeSpace );

        /**
         * Deletes files created by SetDriveFreeSpaceL. Should be called by any test module which
         * has reserved disk space.
         */
        IMPORT_C void DeleteDummyFilesL( TInt aDriveNumber );

        /*
         * Returns KErrNone if file is not in use.
         */
        IMPORT_C TInt EnsureFileIsNotInUse( const TPtrC& aFileName );

	    /*
	     * CalculateSpaceToReserveL
	     */
	    void CalculateSpaceToReserveL( RFs &aFs, TInt aDriveNumber, TInt aDesiredSpace, TInt64& aSpaceToReserve, TInt64& aDriveFreeSpace, TInt64& aUsableSpace );

    public: // From base classes

        void ConnectionCreated() {};
        void ConnectionClosed() {};
        void ConnectionFailed() {};
        void ConnectionAlreadyExists() {};
        void ConnectionTimeout() {};

    private:

        /**
        * C++ default constructor.
        */
        CIptvTestUtilities();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
	    CIptvTestSMApiUtil*    iIptvTestSMApiUtil;
	    CIptvTestMVApiUtil*    iIptvTestMVApiUtil;
	    CIptvTestVCApiUtil*    iIptvTestVCApiUtil;
	    CIptvTestVerifyData*   iIptvVerifyData;
        CIptvTestVideoCreator* iVideoCreator;
        RFs iFs;
        CVCXTestCommon* iVcxCommon;
    };


#endif      // CIPTVTESTUTILITIES_H

// End of File
