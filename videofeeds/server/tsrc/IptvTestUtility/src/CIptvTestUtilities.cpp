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




// INCLUDE FILES
#include <e32Base.h>
#include "mmf/common/mmfcontrollerpluginresolver.h"
#include "CIptvTestUtilities.h"
#include "CIptvTestSMApiUtil.h"
#include "CIptvTestMVApiUtil.h"
#include "CIptvTestVCApiUtil.h"
#include "CIptvTestVerifyData.h"
#include "CIptvService.h"
#include "TestUtilConnectionWaiter.h"
#include "VCXTestLog.h"
#include <tz.h>
#include <badesca.h>

#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvMediaContent.h"
#include "CIptvMyVideosVideoFullDetails.h"
#include "CIptvTestVodContentAllDetails.h"

#include "VCXTestCommon.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TUint32 KReservedSpaceForDriveCInPercentage(10);
_LIT(KDummyfilePathFormat, "%S:\\data\\temp");
_LIT(KDummyfileFormat, "%d.dat");
_LIT(KDoubleBacklash, "\\");

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvTestUtilities::CIptvTestUtilities
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvTestUtilities::CIptvTestUtilities()
    {
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvTestUtilities::ConstructL()
    {
    VCXLOGLO1(">>>CIptvTestUtilities::ConstructL");

    User::LeaveIfError( iFs.Connect() );

    PrintIaps();

    iIptvTestSMApiUtil = CIptvTestSMApiUtil::NewL();
    iIptvTestMVApiUtil = CIptvTestMVApiUtil::NewL();
    iIptvTestVCApiUtil = CIptvTestVCApiUtil::NewL();
    iIptvVerifyData = CIptvTestVerifyData::NewL();

    iVideoCreator = CIptvTestVideoCreator::NewL();

    iVcxCommon = CVCXTestCommon::NewL();

    VCXLOGLO1("<<<CIptvTestUtilities::ConstructL");
    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTestUtilities* CIptvTestUtilities::NewL()
    {
    VCXLOGLO1(">>>CIptvTestUtilities::NewL");
    CIptvTestUtilities* self = new( ELeave ) CIptvTestUtilities;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	VCXLOGLO1("<<<CIptvTestUtilities::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::~CIptvTestUtilities
// Destructor
// -----------------------------------------------------------------------------
//
CIptvTestUtilities::~CIptvTestUtilities()
    {
    VCXLOGLO1(">>>CIptvTestUtilities::~CIptvTestUtilities");

    delete iIptvTestSMApiUtil;
    iIptvTestSMApiUtil=NULL;

    delete iIptvTestMVApiUtil;
    iIptvTestMVApiUtil=NULL;

    delete iIptvTestVCApiUtil;
    iIptvTestVCApiUtil = NULL;

	delete iIptvVerifyData;
	iIptvVerifyData = NULL;

    delete iVideoCreator;
    iVideoCreator = NULL;

    iFs.Close();

    delete iVcxCommon;
    iVcxCommon = NULL;

    VCXLOGLO1("<<<CIptvTestUtilities::~CIptvTestUtilities");
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::PrintIptvService()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::PrintIptvService(CIptvService *aIptvService)
	{
	VCXLOGLO1(">>>CIptvTestUtilities::PrintIptvService");
	TUint8 i;

	VCXLOGLO1("");
	VCXLOGLO1("---- CIptvService Print ----");
	VCXLOGLO2("Service ID: %d", aIptvService->GetId());
	VCXLOGLO2("Service groupID: %d", aIptvService->GetGroupId());
	VCXLOGLO2("Service name: %S", &aIptvService->GetName());
	VCXLOGLO2("Service address: %S", &aIptvService->GetAddress());
	VCXLOGLO2("Service search url: %S", &aIptvService->SearchUrl());
	VCXLOGLO2("Service icon path: %S", &aIptvService->GetIconPath());
	VCXLOGLO2("Service EPG plugin UID: %x", aIptvService->GetEpgPluginUid());
	VCXLOGLO2("Service VOD plugin UID: %x", aIptvService->GetVodPluginUid());
	VCXLOGLO2("Service IPTV Plugin UID: %x", aIptvService->GetIptvPluginUid());
	VCXLOGLO2("Service Type: %d", aIptvService->GetType());
	VCXLOGLO2("Service provider id: %S", &aIptvService->GetProviderId());
	VCXLOGLO2("Service account management url: %S", &aIptvService->GetAccountManagementUrl());

	CIptvIapList& iapList = aIptvService->GetIapList();
	//TIptvIapList iapList;
	//iapList.SetL(aIptvService->GetIapList());

	TIptvIap iap;

	VCXLOGLO2("Service AP count: %d", iapList.Count());

	for(i = 0; i < iapList.Count(); i++)
		{
		TInt err = iapList.GetIap(i, iap);
		VCXLOGLO2("GetIap ret: %d", err);
		VCXLOGLO4("AP %d: ID %d PRIO: %d", i, iap.iId, iap.iPriority);
		}

	VCXLOGLO2("Service user name: %S", &aIptvService->GetUserName());
	VCXLOGLO2("Service password: %S", &aIptvService->GetPassword());
	VCXLOGLO2("Service desc: %S", &aIptvService->GetDesc());

	VCXLOGLO2("ESelected Flag = %d", aIptvService->GetFlags() & 1);
	VCXLOGLO2("EReadOnly Flag = %d", (aIptvService->GetFlags() & 2)>>1);
	VCXLOGLO2("EMainService Flag = %d", (aIptvService->GetFlags() & 4)>>2);
	VCXLOGLO2("EConnectionApproved Flag = %d", (aIptvService->GetFlags() & 8)>>3);

	VCXLOGLO2("Service application uid: %d", aIptvService->GetApplicationUid());
	VCXLOGLO2("Service display order: %d", aIptvService->GetDisplayOrder());


	TTime time = aIptvService->GetEpgUpdateTimeL();

	_LIT(KDateTimeString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
	TBuf<256> timeString;

	time.FormatL(timeString, KDateTimeString);
	VCXLOGLO2("Service EPG update: %S", &timeString);

	VCXLOGLO2("Service update time: %S", &timeString);

	TUint scheduleDlTime = aIptvService->ScheduleDlTime();
	VCXLOGLO2("ScheduleDlTime: %d", scheduleDlTime);

	TUint scheduleDlType = aIptvService->ScheduleDlType();
	VCXLOGLO2("ScheduleDlType: %d", scheduleDlType);

	TUint scheduleDlNetwork = aIptvService->ScheduleDlNetwork();
	VCXLOGLO2("ScheduleDlNetwork: %d", scheduleDlNetwork);

	time = aIptvService->GetScheduledLastDownloadTime();
	time.FormatL(timeString, KDateTimeString);
	VCXLOGLO2("Service scheduled last download time: %S", &timeString);


	VCXLOGLO1("<<<CIptvTestUtilities::PrintIptvService");
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::PrintIptvService()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::WriteVerifyIptvService(TDes& aCaseId, CIptvService *aIptvService)
	{
	VCXLOGLO1(">>>CIptvTestUtilities::WriteVerifyIptvService");
	TUint8 i;


	WriteVerifyData(aCaseId, _L("---- CIptvService Print ----"));
	WriteVerifyData(aCaseId, _L("----------------------------"));
	WriteVerifyData(aCaseId, _L("Service ID: %d"), aIptvService->GetId());
	WriteVerifyData(aCaseId, _L("Service groupID: %d"), aIptvService->GetGroupId());
	WriteVerifyData(aCaseId, _L("Service name: %S"), &aIptvService->GetName());
	WriteVerifyData(aCaseId, _L("Service address: %S"), &aIptvService->GetAddress());
	WriteVerifyData(aCaseId, _L("Service search url: %S"), &aIptvService->SearchUrl());
	WriteVerifyData(aCaseId, _L("Service icon path: %S"), &aIptvService->GetIconPath());
	WriteVerifyData(aCaseId, _L("Service EPG plugin UID: %x"), aIptvService->GetEpgPluginUid());
	WriteVerifyData(aCaseId, _L("Service VOD plugin UID: %x"), aIptvService->GetVodPluginUid());
	WriteVerifyData(aCaseId, _L("Service IPTV Plugin UID: %x"), aIptvService->GetIptvPluginUid());
	WriteVerifyData(aCaseId, _L("Service Type: %d"), aIptvService->GetType());
	WriteVerifyData(aCaseId, _L("Service provider id: %S"), &aIptvService->GetProviderId());
	WriteVerifyData(aCaseId, _L("Service account management url: %S"), &aIptvService->GetAccountManagementUrl());

	CIptvIapList& iapList = aIptvService->GetIapList();
	//TIptvIapList iapList;
	//iapList.SetL(aIptvService->GetIapList());

	TIptvIap iap;

	WriteVerifyData(aCaseId, _L("Service AP count: %d"), iapList.Count());

	for(i = 0; i < iapList.Count(); i++)
		{
		iapList.GetIap(i, iap);
		WriteVerifyData(aCaseId, _L("AP %d: ID %d PRIO: %d"), i, iap.iId, iap.iPriority);
		}


	WriteVerifyData(aCaseId, _L("Service user name: %S"), &aIptvService->GetUserName());
	WriteVerifyData(aCaseId, _L("Service password: %S"), &aIptvService->GetPassword());
	WriteVerifyData(aCaseId, _L("Service desc: %S"), &aIptvService->GetDesc());

	WriteVerifyData(aCaseId, _L("ESelected Flag = %d"), aIptvService->GetFlags() & 1);
	WriteVerifyData(aCaseId, _L("EReadOnly Flag = %d"), (aIptvService->GetFlags() & 2)>>1);
	WriteVerifyData(aCaseId, _L("EMainService Flag = %d"), (aIptvService->GetFlags() & 4)>>2);
	WriteVerifyData(aCaseId, _L("EConnectionApproved Flag = %d"), (aIptvService->GetFlags() & 8)>>3);

	WriteVerifyData(aCaseId, _L("Service application uid: %d"), aIptvService->GetApplicationUid());
	WriteVerifyData(aCaseId, _L("Service display order: %d"), aIptvService->GetDisplayOrder());

	WriteVerifyData(aCaseId, _L("---- CIptvService Print ----"));

	VCXLOGLO1("<<<CIptvTestUtilities::WriteVerifyIptvService");
	}


// -----------------------------------------------------------------------------
// CIptvTestUtilities::CreateIptvServiceInstanceL()
// Creates CIptvServiceInstance from parameters
// returns CIptvService* and the ownership is moved to caller
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvService* CIptvTestUtilities::CreateIptvServiceInstanceL(
                                                            CIptvServiceInfo& aService
                                                            )
    {

    VCXLOGLO1(">>>CIptvTestUtilities::CreateIptvServiceInstanceL");
    CIptvService* iptvService = CIptvService::NewL();

    TUint32 flags(0);

    TInt err = KErrNone;

    iptvService->SetId(0); //new id is generated anyways in DB, no matter what this is

    iptvService->SetName(aService.iName);

    iptvService->SetAddress(aService.iAddress);
    iptvService->SetIconPath(aService.iIconPath);

    iptvService->SetEpgPluginUid(aService.iEpgPluginUid);

    iptvService->SetVodPluginUid(aService.iVodPluginUid);

    iptvService->SetIptvPluginUid(aService.iIptvPluginUid);

    iptvService->SetType(aService.iServiceType);

    if(aService.iIapList)
    	{
    	err = iptvService->SetIapListL(*aService.iIapList);

	    if(err != KErrNone)
	    	{
	    	delete iptvService;
	    	User::Leave(err);
	    	}
    	}

    iptvService->SetUserName(aService.iUserName);
    iptvService->SetPassword(aService.iPassWord);

    iptvService->SetDesc(aService.iDesc);

    iptvService->SetProviderId(aService.iProviderId);

    iptvService->SetAccountManagementUrl(aService.iAccountManagementUrl);

    //flags
    if(aService.iSelectedFlag)
        {
        flags = flags | CIptvService::ESelected;
        }
    if(aService.iReadOnlyFlag)
        {
        flags = flags | CIptvService::EReadOnly;
        }
    if(aService.iMainServiceFlag)
        {
        flags = flags | CIptvService::EMainService;
        }
    if(aService.iConnectionApprovedFlag)
        {
        flags = flags | CIptvService::EConnectionApproved;
        }

    iptvService->SetFlags(flags);

    iptvService->SetApplicationUid(aService.iApplicationUid);

    iptvService->SetDisplayOrder(aService.iDisplayOrder);


    VCXLOGLO1("<<<CIptvTestUtilities::CreateIptvServiceInstanceL");
    return iptvService;
    }
// -----------------------------------------------------------------------------
// CIptvTestUtilities::VerifyServices
// Creates CIptvServiceInstance from parameters
// returns TRUE if services are equal, othervise false
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvTestUtilities::VerifyServices(CIptvService& aS1, CIptvService& aS2)
{
	VCXLOGLO1(">>>CIptvTestUtilities::VerifyServices");
	TBool rv(TRUE);
	TUint8 i(0);

	VCXLOGLO3("Comparing services: ID: %d - %d", aS1.GetId(), aS2.GetId());
	VCXLOGLO3(" - Name: %S - %S", &aS1.GetName(), &aS2.GetName() );

	//ids can't be verified now
	/*if(aS1.GetId() != aS2.GetId())
	{
		rv = FALSE;
		VCXLOGLO3("Id doesn't match: id1 = %d, id2 = %d", aS1.GetId(), aS2.GetId() );
	}*/

	if(aS1.GetName() != aS2.GetName())
	    {
	    rv = FALSE;
		VCXLOGLO3("Name doesn't match: name1 = %S, name2 = %S",&aS1.GetName(),&aS2.GetName());
	    }

	if(aS1.GetAddress() != aS2.GetAddress())
	    {
	     rv = FALSE;
		VCXLOGLO3("Address doesn't match: address1 = %S, address2 = %S",&aS1.GetAddress(),&aS2.GetAddress());
	    }

	if(aS1.GetIconPath() != aS2.GetIconPath())
	    {
	     rv = FALSE;
		VCXLOGLO3("IconPath doesn't match: IconPath1 = %S, IconPath2 = %S",&aS1.GetIconPath(),&aS2.GetIconPath());
	    }

	if(aS1.GetEpgPluginUid() != aS2.GetEpgPluginUid())
	    {
	     rv = FALSE;
		VCXLOGLO3("EpgPluginUid doesn't match: EpgPluginUid1 = %d, EpgPluginUid2 = %d",aS1.GetEpgPluginUid(),aS2.GetEpgPluginUid());
	    }

	  if(aS1.GetVodPluginUid() != aS2.GetVodPluginUid())
	    {
	     rv = FALSE;
		VCXLOGLO3("VodPluginUid doesn't match: VodPluginUid1 = %d, VodPluginUid2 = %d",aS1.GetVodPluginUid(),aS2.GetVodPluginUid());
	    }

	  if(aS1.GetIptvPluginUid() != aS2.GetIptvPluginUid())
	    {
	     rv = FALSE;
		VCXLOGLO3("IptvPluginUid doesn't match: IptvPluginUid1 = %d, IptvPluginUid2 = %d",aS1.GetIptvPluginUid(),aS2.GetIptvPluginUid());
	    }

        if(aS1.GetType() != aS2.GetType())
            {
            rv = FALSE;
            VCXLOGLO3("Type doesn't match: Type1 = %d Type2 = %d", aS1.GetType(), aS2.GetType());
            }

        //iaplist
    	//TIptvIapList iapListS1;
    	//TIptvIapList iapListS2;
    	CIptvIapList& iapListS1 = aS1.GetIapList();
    	CIptvIapList& iapListS2 = aS2.GetIapList();
	    TIptvIap iapS1;
	    TIptvIap iapS2;

       // iapListS1 = aS1.GetIapList();
    	//iapListS2 = aS2.GetIapList();

	    if(iapListS1.Count() != iapListS2.Count())
	        {
	        rv = FALSE;
            VCXLOGLO3("Count of iaps doesn't match: Service1 = %d Service2 = %d", iapListS1.Count(), iapListS2.Count());
	        }

	    if(rv) // Same Count of iaps
	        {
        	for(i = 0; i < iapListS1.Count(); i++)
        		{
        		TInt err = iapListS1.GetIap(i,  iapS1);
        		VCXLOGLO2("GetIap ret: %d", err);
        		User::LeaveIfError(err);

        		err = iapListS2.GetIap(i, iapS2);
        		VCXLOGLO2("GetIap ret: %d", err);
        		User::LeaveIfError(err);

        		if(iapS1.iId != iapS2.iId || iapS1.iPriority != iapS2.iPriority)
        		    {
        		    rv = FALSE;
        		    VCXLOGLO2("AP %d doesn't match", i);
        		    }
        		}
	        }

        if(aS1.GetUserName() != aS2.GetUserName())
            {
            rv = FALSE;
            VCXLOGLO3("UserName doesn't match: UserName1 = %S, UserName2 = %S",&aS1.GetUserName() ,&aS2.GetUserName());
            }

        if(aS1.GetPassword() != aS2.GetPassword())
            {
            rv = FALSE;
            VCXLOGLO3("Password doesn't match:Password1 = %S, Password2 = %S",&aS1.GetPassword() ,&aS2.GetPassword());
            }

        if(aS1.GetDesc() != aS2.GetDesc())
            {
            rv = FALSE;
            VCXLOGLO3("Desription doesn't match: Desription1 = %S, Desription2 = %S",&aS1.GetDesc() ,&aS2.GetDesc());
            }

        if(aS1.GetFlags() != aS2.GetFlags())
            {
            rv = FALSE;
            VCXLOGLO3("Flags doesn't match: 0x%x & 0x%x", aS1.GetFlags(), aS2.GetFlags() );

            VCXLOGLO1("Service1 flags: ");
            if(aS1.GetFlags() & CIptvService::ESelected)
            	{
            	VCXLOGLO1("ESelected");
            	}
            if(aS1.GetFlags() & CIptvService::EReadOnly)
            	{
            	VCXLOGLO1("EReadOnly");
            	}
            if(aS1.GetFlags() & CIptvService::EMainService)
            	{
            	VCXLOGLO1("EMainService");
            	}
            if(aS1.GetFlags() & CIptvService::EConnectionApproved)
            	{
            	VCXLOGLO1("EConnectionApproved");
            	}
            if(aS1.GetFlags() & CIptvService::EReadOnlyIaps)
            	{
            	VCXLOGLO1("EReadOnlyIaps");
            	}

			VCXLOGLO1("Service2 flags: ");
            if(aS2.GetFlags() & CIptvService::ESelected)
            	{
            	VCXLOGLO1("ESelected");
            	}
            if(aS2.GetFlags() & CIptvService::EReadOnly)
            	{
            	VCXLOGLO1("EReadOnly");
            	}
            if(aS2.GetFlags() & CIptvService::EMainService)
            	{
            	VCXLOGLO1("EMainService");
            	}
            if(aS2.GetFlags() & CIptvService::EConnectionApproved)
            	{
            	VCXLOGLO1("EConnectionApproved");
            	}
            if(aS2.GetFlags() & CIptvService::EReadOnlyIaps)
            	{
            	VCXLOGLO1("EReadOnlyIaps");
            	}

            }

        if(aS1.GetApplicationUid() != aS2.GetApplicationUid())
            {
            rv = FALSE;
            VCXLOGLO1("Application uid doesn't match");
            }

        /*if(aS1.GetDisplayOrder() != aS2.GetDisplayOrder())
            {
            rv = FALSE;
            VCXLOGLO1("Display order doesn't match");
            }
   		*/
   			if(aS1.GetProviderId() != aS2.GetProviderId())
            {
            rv = FALSE;
            VCXLOGLO3("Provider id doesn't match: id1 = %S, id2 = %S", &aS1.GetProviderId(), &aS2.GetProviderId());
            }

				if(aS1.GetAccountManagementUrl() != aS2.GetAccountManagementUrl())
            {
            rv = FALSE;
            VCXLOGLO3("AccountManagementUrl doesn't match: url1 = %S, url2 = %S", &aS1.GetAccountManagementUrl(), &aS2.GetAccountManagementUrl());
            }

	VCXLOGLO2("<<<CIptvTestUtilities::VerifyServices returns %d", rv);
	return rv;
}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetServiceFromDb
// Creates CIptvServiceInstance from parameters
// returns pointer to service if succeeded, otherwise NULL
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvService* CIptvTestUtilities::GetServiceFromDb(TInt aId)
    {
    return iIptvTestSMApiUtil->GetServiceFromDb(aId);
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetIapNameById
// Gets iap name by id
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvTestUtilities::GetIapNameById(TDes& aIapName, TUint32 aIapId)
    {
    return iVcxCommon->GetIapNameById( aIapName, aIapId );
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::PrintIaps
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::PrintIaps()
    {
    iVcxCommon->PrintIaps();
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetIapIdL
// -----------------------------------------------------------------------------
//
EXPORT_C  TBool CIptvTestUtilities::GetIapIdL(const TDesC& aIapName, TUint32& aIapId)
    {
    return iVcxCommon->GetIapIdL( aIapName, aIapId );
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetIapIdByNameL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvTestUtilities::GetIapIdByNameL(const TDesC& aIapName, TUint32& aIapId)
    {
    return iVcxCommon->GetIapIdByNameL( aIapName, aIapId );
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetIapIdByTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvTestUtilities::GetIapIdByTypeL(CVCXTestCommon::TIptvTestIapType aType, TUint32& aIapId, TInt aOrderNumber)
    {
    return iVcxCommon->GetIapIdByTypeL( aType, aIapId, aOrderNumber );
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetUsedIap
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvTestUtilities::GetUsedIap(TUint32 aServiceId, TUint32& aIapId,TDes& aIapName)
    {
    VCXLOGLO1(">>>CIptvTestUtilities::GetUsedIap");
    TBool rv = TRUE;

    rv = iIptvTestSMApiUtil->GetUsedIap(aServiceId, aIapId, aIapName);

    VCXLOGLO1("<<<CIptvTestUtilities::GetUsedIap");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetVideoInfo
// -----------------------------------------------------------------------------
//
TBool CIptvTestUtilities::GetVideoInfo(CIptvMyVideosGlobalFileId& aFileId, TUint32& aServiceId, TDes& aVideoName,TDes& aFileName)
    {
    VCXLOGLO1(">>>CIptvTestUtilities::GetVideoInfo");
    TBool rv = FALSE;

    rv = iIptvTestMVApiUtil->GetVideoInfo(aFileId,aServiceId,aVideoName,aFileName);

    VCXLOGLO1("<<<CIptvTestUtilities::GetVideoInfo");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetFileSize
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvTestUtilities::GetFileSize(TDesC& aFullPath, TUint32& aFileSize)
    {
    return iVcxCommon->GetFileSize( aFullPath, aFileSize );
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetContentIdsL
// returns all content ids from service
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvTestUtilities::GetContentIdsL(TIptvServiceId aServiceId, TUint32 aIapId, RArray<TIptvContentId>& aContentIdArray, TUint32 aMaxSize, TUint32 aMaxPlaytime)
    {
    VCXLOGLO1(">>>CIptvTestUtilities::GetContentIdsL");
    TBool rv = iIptvTestVCApiUtil->GetContentIdsL(aServiceId, aIapId, aContentIdArray, aMaxSize, aMaxPlaytime);
    VCXLOGLO1("<<<CIptvTestUtilities::GetContentIdsL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetServicesFromDbL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvTestUtilities::GetServicesFromDbL(RPointerArray<CIptvService>& aServicesArray, TUint32 aFlagsFilter, CIptvServiceManagementClient::TOrder aOrder)
	{
	VCXLOGLO1(">>>CIptvTestUtilities::GetServicesFromDbL");
    TBool rv = iIptvTestSMApiUtil->GetServicesFromDbL(aServicesArray, aFlagsFilter, aOrder);
    VCXLOGLO1("<<<CIptvTestUtilities::GetServicesFromDbL");
    return rv;
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetServicesFromDbL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvTestUtilities::GetServicesFromDbL(RPointerArray<CIptvService>& aServicesArray)
    {
    VCXLOGLO1(">>>CIptvTestUtilities::GetServicesFromDbL");
    TBool rv = iIptvTestSMApiUtil->GetServicesFromDbL(aServicesArray);
    VCXLOGLO1("<<<CIptvTestUtilities::GetServicesFromDbL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetServiceIdByNameL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::GetServiceIdByNameL(TDesC& aServiceName, TInt& aId)
	{
	VCXLOGLO1(">>>CIptvTestUtilities::GetServiceIdByNameL");

    RPointerArray<CIptvService> services;

    CleanupResetAndDestroyPushL(services);

    aId = -1;

    TInt rv = KErrNone;

    if( GetServicesFromDbL(services) )
    	{
    	TInt i;
    	// Newest are back
    	for(i=services.Count()-1; i>= 0; i--)
    		{
			if(services[i]->GetName() == aServiceName)
				{
				aId = services[i]->GetId();
				break;
				}
    		}
        if( services.Count() == 0 )
            {
            VCXLOGLO1("Service count 0");
            rv = KErrNotFound;
            }
    	}
    else
    	{
    	VCXLOGLO1("GetServicesFromDbL failed.");
    	rv = KErrGeneral;
    	}

    CleanupStack::PopAndDestroy(&services);

    VCXLOGLO1("<<<CIptvTestUtilities::GetServiceIdByNameL");
    return rv;
	}

// -----------------------------------------------------------------------------
// CIptvTestVerifyData::CreateVerifyData
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::CreateVerifyData(TDesC& aVerifyId, TBool aAppend)
    {
    return iVcxCommon->CreateVerifyData( aVerifyId, aAppend );
    }

// -----------------------------------------------------------------------------
// CIptvTestVerifyData::WriteVerifyData
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::WriteVerifyData(TDesC& aVerifyId, TDesC& aVerifyData)
    {
    return iVcxCommon->WriteVerifyData( aVerifyId, aVerifyData );
    }

// -----------------------------------------------------------------------------
// CIptvTestVerifyData::WriteVerifyData
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::WriteVerifyData(TDesC& aVerifyId, TRefByValue<const TDesC> aFmt, ... )
    {
    //VCXLOGLO1(">>>CIptvTestUtilities::WriteVerifyData(formatted)");

    VA_LIST argptr; //pointer to argument list
    VA_START( argptr, aFmt );

	HBufC* str;
	str = HBufC::NewLC(1024*3);

	str->Des().FormatList(aFmt, argptr);

	TInt result = KErrNone;
	TRAPD(err, result = iVcxCommon->WriteVerifyData(aVerifyId, *str));

	CleanupStack::PopAndDestroy(str);

	if(err != KErrNone)
		{
		VCXLOGLO2("iVcxCommon::WriteVerifyDataL caused a leave. %d", err);
		result = err;
		}

    //VCXLOGLO1("<<<CIptvTestUtilities::WriteVerifyData(formatted)");
    return result;
    }

// -----------------------------------------------------------------------------
// CIptvTestVerifyData::VerifyData
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::VerifyData(TDesC& aVerifyId, CIptvTestVerifyData::TVerifyResult& aVerifyResult)
    {
    return iVcxCommon->VerifyData( aVerifyId, aVerifyResult );
    }

// -----------------------------------------------------------------------------
// CIptvTestVerifyData::VerifyData
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::EnableVerifyTimestamps( TBool aUseTimestamps )
    {
    iVcxCommon->EnableVerifyTimestamps( aUseTimestamps );
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetVodContentContentDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::GetVodContentContentDetailsL(TUint32 aServiceId, TUint32 aContentId, CIptvVodContentContentFullDetails& aContentDetails)
	{
	return iIptvTestVCApiUtil->GetVodContentContentDetailsL(aServiceId, aContentId, aContentDetails);
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetVodEcgCategoryListL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::GetVodEcgCategoryListL(TUint32 aServiceId, TUint32 aParentCategoryId, RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList)
    {
    iIptvTestVCApiUtil->GetEcgCategoryListL( aServiceId, aParentCategoryId, aCategoryList );
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetVodEcgListL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::GetVodEcgListL(TUint32 aServiceId, TUint32 aCategoryId, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList)
    {
    iIptvTestVCApiUtil->GetEcgListL( aServiceId, aCategoryId, aEcgList );
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetVodContentAllDetailsListL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::GetVodContentAllDetailsListL( TUint32 aServiceId, RPointerArray<CIptvTestVodContentAllDetails>& aDetailsList )
	{
	iIptvTestVCApiUtil->GetVodContentAllDetailsListL( aServiceId, aDetailsList );
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetContentAccessListL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::GetContentAccessListL(TUint32 aServiceId, TUint32 aContentId, RPointerArray<CIptvMediaContent>& aCAList)
	{
	return iIptvTestVCApiUtil->GetContentAccessListL(aServiceId, aContentId, aCAList);
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::GetMyVideosVideoDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::GetMyVideosVideoDetailsL(CIptvMyVideosGlobalFileId& aFileId, CIptvMyVideosVideoFullDetails& aVideoDetails)
	{
	return iIptvTestMVApiUtil->GetMyVideosVideoDetailsL(aFileId, aVideoDetails);
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::CreateBadIapL
// Creates a not working, crappy iap with given name.
// Use with caution, because there is no easy way of removing created iaps
//
// @param aName name of the iap to be created
// @return iapId new iap's id.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvTestUtilities::CreateBadIapL(TDesC& aName)
    {
    return iVcxCommon->CreateBadIapL( aName );
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::ReallycleanL()
// Clean up MyVideos
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::ReallycleanL()
	{
	return iIptvTestMVApiUtil->ReallycleanL();
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::DeleteVideoL()
// Delete specified video, calls directly
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::DeleteVideoL(CIptvMyVideosGlobalFileId& aFileId)
	{
	return iIptvTestMVApiUtil->DeleteVideoL(aFileId);
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::ParseIntFromString
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvTestUtilities::ParseIntFromString(TInt& aInt, TDesC& aString)
	{
	VCXLOGLO1(">>>CIptvTestUtilities::ParseIntFromString");

	TLex lex(aString);
   	TInt err = lex.Val(aInt);

	VCXLOGLO1("<<<CIptvTestUtilities::ParseIntFromString");
	return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::AdvanceSystemTime1Second
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::AdvanceSystemTime1Second()
	{
	VCXLOGLO1(">>>CIptvTestUtilities::AdvanceSystemTime1Second");

    TTime now;
    now.HomeTime();

    TTimeIntervalSeconds second(1);
    now += second;

    TInt err = CIptvTestUtilities::SetSystemTime(now);

	VCXLOGLO1("<<<CIptvTestUtilities::AdvanceSystemTime1Second");

	return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::SetSystemTime()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::SetSystemTime(TTime aTime)
	{
	VCXLOGLO1(">>>CIptvTestUtilities::SetSystemTime");

    TTime noDST(aTime);
    RTz tz;
    TInt err = tz.Connect();
    if(err != KErrNone)
    	{
    	VCXLOGLO2("RTz::Connect failed: %d", err);
    	VCXLOGLO1("<<<CIptvTestUtilities::SetSystemTime");
    	return err;
    	}
    CleanupClosePushL(tz);
    err = tz.SetHomeTime(noDST);
    if(err != KErrNone)
    	{
    	VCXLOGLO2("RTz::SetHomeTime failed: %d", err);
    	VCXLOGLO1("<<<CIptvTestUtilities::SetSystemTime");
    	CleanupStack::PopAndDestroy(&tz);
    	return err;
    	}

	CleanupStack::PopAndDestroy(&tz);

	VCXLOGLO1("<<<CIptvTestUtilities::SetSystemTime");

	return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::SetTimeZone()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::SetTimeZone(TDesC& aTimeZone)
	{
	VCXLOGLO1(">>>CIptvTestUtilities::SetTimeZone");

    RTz tz;
    TInt err = tz.Connect();
    if(err != KErrNone)
    	{
    	VCXLOGLO2("RTz::Connect failed: %d", err);
    	VCXLOGLO1("<<<CIptvTestUtilities::SetTimeZone");
    	return err;
    	}
    CleanupClosePushL(tz);

    TBuf8<256> timezone;
    timezone.Copy( aTimeZone ); // Conversion

    CTzId* tzId = CTzId::NewL( timezone );
    CleanupStack::PushL(tzId);

    TRAP(err, tz.SetTimeZoneL( *tzId ) );
	CleanupStack::PopAndDestroy(tzId);
	CleanupStack::PopAndDestroy(&tz);

    if(err != KErrNone)
    	{
    	VCXLOGLO2("RTz::SetTimeZone failed: %d", err);
    	VCXLOGLO1("<<<CIptvTestUtilities::SetTimeZone");
    	return err;
    	}

	VCXLOGLO1("<<<CIptvTestUtilities::SetTimeZone");

	return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::SetUsedIap()
//
// Sets default iap for a service, and set connecting allowed for the iap.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::SetUsedIap(TUint32 aServiceId, TUint32 aIapId)
	{
	VCXLOGLO1(">>>CIptvTestUtilities::SetUsedIap");

	TInt err = iIptvTestSMApiUtil->SetUsedIap(aServiceId, aIapId);

	VCXLOGLO1("<<<CIptvTestUtilities::SetUsedIap");
	return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestUtilities::CreateVideoFileL()
//
// Creates a video file to file system.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::CreateVideoFileL( CIptvTestVideoCreator::TIptvTestVideoType aVideoType, TDesC& aFileName, TInt aSize )
    {
    VCXLOGLO1(">>>CIptvTestUtilities::CreateVideoFileL");
    iVideoCreator->CreateVideoL( aVideoType, aFileName, aSize );
    VCXLOGLO1("<<<CIptvTestUtilities::CreateVideoFileL");
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::CreateVideoFilesL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::CreateVideoFilesL( CIptvTestVideoCreator::TIptvTestVideoType aVideoType, TDesC& aFileName, TInt aCount, RPointerArray<HBufC>& aFileArray )
    {
    VCXLOGLO1(">>>CIptvTestUtilities::CreateVideoFilesL");
    iVideoCreator->CreateVideosL( aVideoType, aFileName, aCount, aFileArray );
    VCXLOGLO1("<<<CIptvTestUtilities::CreateVideoFilesL");
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::SetDriveFreeSpaceL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::SetDriveFreeSpaceL( TInt aDriveNumber, TUint aDesiredFreeSpace )
    {

    VCXLOGLO1(">>>CIptvTestUtilities::SetDriveFreeSpaceL");

    const TInt KIptvTest1KB = 1024;
    const TInt KIptvTest1MB = KIptvTest1KB*KIptvTest1KB;

    DeleteDummyFilesL( aDriveNumber );

#ifdef  __WINSCW__
    //  We don't want to fill the windows disk
    if(aDriveNumber == EDriveC)
        {
        VCXLOGLO1("<<<CIptvTestUtilities::SetDriveFreeSpaceL");
        return;
        }
#endif

    TInt64 spaceToReserve = 0;
    TInt64 freeSpace = 0;
    TInt64 usableSpace = 0;
    CalculateSpaceToReserveL(iFs, aDriveNumber, aDesiredFreeSpace, spaceToReserve, freeSpace, usableSpace);
    if( spaceToReserve <= 0 )
        {
        VCXLOGLO1("CIptvTestUtilities:: Already less than wanted space.");
        VCXLOGLO1("<<<CIptvTestUtilities::SetDriveFreeSpaceL");
        return;
        }

    TChar driveLetter;
    RFs::DriveToChar( aDriveNumber, driveLetter );

    TBuf<KIptvMaxPath> path;
    path.Format(KDummyfilePathFormat, &driveLetter);
    path.Append(KDoubleBacklash);

    BaflUtils::EnsurePathExistsL(iFs, path);

    // Create files of 256 MB size.
    TInt64 dummyFileSize = 256 * KIptvTest1MB;
    TInt count = 0;
    for( ; spaceToReserve > 0; spaceToReserve -= (dummyFileSize) )
        {
        // Check that there's still enough free space for the dummyfile
        TInt64 newSpaceToReserve = 0;
        CalculateSpaceToReserveL(iFs, aDriveNumber, aDesiredFreeSpace, newSpaceToReserve, freeSpace, usableSpace);
        if( newSpaceToReserve <= 0 )
            {
            VCXLOGLO1("CIptvTestUtilities:: Nothing to reserve anymore.");
            break;
            }

        // Not enough space. Start using 1 MB dummy files, or fill the memory at once if there's less than 1 MB free
        if(dummyFileSize >= usableSpace)
            {
            dummyFileSize = KIptvTest1MB;
            spaceToReserve = newSpaceToReserve;

            // If there's still not enough space, fill all at once and leave the loop
            if(dummyFileSize >= usableSpace)
                {
                dummyFileSize = newSpaceToReserve;
                spaceToReserve = 0;
                }
            }

        TBuf<256> filePath(path);
        TInt64 fileSize = dummyFileSize;
        RFile file;

        count++;
        filePath.AppendFormat(KDummyfileFormat, count);
        VCXLOGLO3("Creating %Ld b dummyfile to '%S'", fileSize, &filePath);
        TInt err = file.Replace(iFs, filePath, EFileWrite);
        if(err != KErrNone)
            {
            VCXLOGLO2("file.Replace fail: %d", err);
            }
        User::LeaveIfError(err);
        err = file.SetSize(fileSize);
        if(err != KErrNone)
            {
            VCXLOGLO2("file.SetSize fail: %d", err);
            }
        User::LeaveIfError(err);
        file.Close();
        }

    VCXLOGLO1("<<<CIptvTestUtilities::SetDriveFreeSpaceL");
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::CalculateSpaceToReserve
// -----------------------------------------------------------------------------
//
void CIptvTestUtilities::CalculateSpaceToReserveL( RFs &aFs, TInt aDriveNumber, TInt aDesiredSpace, TInt64& aSpaceToReserve, TInt64& aDriveFreeSpace, TInt64& aUsableSpace )
    {
    TVolumeInfo volumeInfo;
    User::LeaveIfError(aFs.Volume(volumeInfo, aDriveNumber));

    TInt reservedPercentage = 0;
    if(aDriveNumber == EDriveC)
        {
        reservedPercentage = KReservedSpaceForDriveCInPercentage;
        }
    TInt64 driveSize      = volumeInfo.iSize;
    TInt64 reservedSpace  = driveSize  / 100 *  reservedPercentage;

    aDriveFreeSpace      = volumeInfo.iFree;
    aUsableSpace    = aDriveFreeSpace - reservedSpace;

    aSpaceToReserve  = aUsableSpace - aDesiredSpace;

    // Have to leave little space, else setting filesize will fail.
    if(aDesiredSpace < 1024*5 && aDriveNumber != EDriveC)
        {
        aSpaceToReserve -= (1024*5 - aDesiredSpace);
        }

    VCXLOGLO2("CIptvTestUtilities: driveSize      = %Ld", driveSize);
    VCXLOGLO2("CIptvTestUtilities: reservedSpace  = %Ld", reservedSpace);
    VCXLOGLO2("CIptvTestUtilities: freeSpace      = %Ld", aDriveFreeSpace);
    VCXLOGLO2("CIptvTestUtilities: usableSpace    = %Ld", aUsableSpace);
    VCXLOGLO2("CIptvTestUtilities: desiredSpace   = %d", aDesiredSpace);
    VCXLOGLO2("CIptvTestUtilities: spaceToReserve  = %Ld", aSpaceToReserve);

   if (aDesiredSpace > aUsableSpace)
        {
        aSpaceToReserve = 0;
        VCXLOGLO1("CIptvTestUtilities:: Disk has less space than wanted to set free!");
        }
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::DeleteDummyFilesL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestUtilities::DeleteDummyFilesL( TInt aDriveNumber )
    {
    VCXLOGLO1(">>>CIptvTestUtilities::DeleteDummyFilesL");

    CFileMan* fileMan = CFileMan::NewL(iFs);
    CleanupStack::PushL(fileMan);

    TBuf<3> driveLetter;
    TChar driveChar;
    RFs::DriveToChar( aDriveNumber, driveChar );
    driveLetter.Append(driveChar);

    TBuf<KIptvMaxPath>path;

    path.Format( KDummyfilePathFormat, &driveLetter );
    path.Append(KDoubleBacklash);
    fileMan->RmDir(path);

    CleanupStack::PopAndDestroy(fileMan);

    VCXLOGLO1("<<<CIptvTestUtilities::DeleteDummyFilesL");
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::EnsureFileIsNotInUse()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::EnsureFileIsNotInUse( const TPtrC& aFileName )
    {
    TInt error( KErrNone );
    RFile file;
    // MDS or someone else could have the file in exclusive use. Try 5 times in 1 second intervals.
    for(TInt retry = 0; retry < 5; retry++)
        {
        error = file.Open(iFs, aFileName, EFileShareExclusive);
        if(error == KErrInUse)
            {
            User::After(1000000);
            }
        else
            {
            break;
            }
        }

    file.Close();
    VCXLOGLO2("CIptvTestUtilities::EnsureFileIsNotInUse: %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CIptvTestUtilities::RefreshService(TUint32 aServiceId, TTime aLastDownloadTime)
//
// Loads old service, creates a new duplicate and finally delete old.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvTestUtilities::RefreshService(TUint32 aServiceId, TTime aLastDownloadTime)
	{
	VCXLOGLO1(">>>CIptvTestUtilities::RefreshService");

	TInt err = iIptvTestSMApiUtil->RefreshService(aServiceId, aLastDownloadTime);

	VCXLOGLO1("<<<CIptvTestUtilities::RefreshService");
	return err;
	}

// -----------------------------------------------------------------------------
// CIptvServiceInfo::NewL()
// -----------------------------------------------------------------------------
EXPORT_C CIptvServiceInfo* CIptvServiceInfo::NewL()
{
    VCXLOGLO1(">>>CIptvServiceInfo::NewL");
    CIptvServiceInfo* self = new( ELeave ) CIptvServiceInfo;

    CleanupStack::PushL( self );
    self->ConstructL(NULL);
    CleanupStack::Pop();
	VCXLOGLO1("<<<CIptvServiceInfo::NewL");
    return self;
}

// -----------------------------------------------------------------------------
// CIptvServiceInfo::NewL()
// -----------------------------------------------------------------------------
EXPORT_C CIptvServiceInfo* CIptvServiceInfo::NewL(CIptvServiceInfo& aServiceInfo)
{
    VCXLOGLO1(">>>CIptvServiceInfo::NewL");
    CIptvServiceInfo* self = new( ELeave ) CIptvServiceInfo;

    CleanupStack::PushL( self );
    self->ConstructL(&aServiceInfo);



    CleanupStack::Pop();
	VCXLOGLO1("<<<CIptvServiceInfo::NewL");
    return self;
}

// -----------------------------------------------------------------------------
// CIptvServiceInfo::NewL()
// -----------------------------------------------------------------------------
EXPORT_C CIptvServiceInfo::~CIptvServiceInfo()
{
	if(iIapList)
	{
		delete iIapList;
		iIapList = NULL;
	}
}

// -----------------------------------------------------------------------------
// CIptvServiceInfo::CIptvServiceInfo()
// -----------------------------------------------------------------------------
CIptvServiceInfo::CIptvServiceInfo()
{

}

// -----------------------------------------------------------------------------
// CIptvServiceInfo::ConstructL()
// -----------------------------------------------------------------------------
void CIptvServiceInfo::ConstructL(CIptvServiceInfo* aServiceInfo)
{
    if(aServiceInfo)
		{
		iId = aServiceInfo->iId;
		iName = aServiceInfo->iName;
		iAddress = aServiceInfo->iAddress;
		iIconPath = aServiceInfo->iIconPath;
		iEpgPluginUid = aServiceInfo->iEpgPluginUid;
		iVodPluginUid = aServiceInfo->iVodPluginUid;
		iIptvPluginUid = aServiceInfo->iIptvPluginUid;
		iServiceType = aServiceInfo->iServiceType;

		if(aServiceInfo->iIapList)
			{
			CIptvIapList* iapList = CIptvIapList::NewL();
			CleanupStack::PushL(iapList);

			for(TInt i=0; i<aServiceInfo->iIapList->Count(); i++)
				{
				TIptvIap iap;
				User::LeaveIfError( aServiceInfo->iIapList->GetIap(i, iap) );
				iapList->AddIap(iap);
				}

			iIapList = iapList;

			CleanupStack::Pop(iapList);
			}

		iUserName = aServiceInfo->iUserName;
		iPassWord = aServiceInfo->iPassWord;
		iDesc = aServiceInfo->iDesc;
		iSelectedFlag = aServiceInfo->iSelectedFlag;
	    iReadOnlyFlag = aServiceInfo->iReadOnlyFlag;
	    iMainServiceFlag = aServiceInfo->iMainServiceFlag;
	    iConnectionApprovedFlag = aServiceInfo->iConnectionApprovedFlag;
	    iApplicationUid = aServiceInfo->iApplicationUid;
	    iDisplayOrder = aServiceInfo->iDisplayOrder;
	    iProviderId = aServiceInfo->iProviderId;
	    iAccountManagementUrl = aServiceInfo->iAccountManagementUrl;
		}
}

//  End of File
