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
* Description:   implements logging functionality of the download information*
*/




// INCLUDE FILES
#include "CIptvTestDlInfo.h"
#include "VCXTestLog.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvTestUtilities.h"
#include "CIptvMediaContent.h"
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "CIptvVodContentContentFullDetails.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

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
EXPORT_C CIptvTestDlInfo* CIptvTestDlInfo::NewL()
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::NewL");
	CIptvTestDlInfo* self = new (ELeave) CIptvTestDlInfo();
	CleanupStack::PushL(self);
	self->ConstructL( );
	CleanupStack::Pop();
	VCXLOGLO1("<<<CIptvTestDlInfo::NewL");
	return self;
	}
// -----------------------------------------------------------------------------
// CIptvTestDlInfo::~CIptvTestDlInfo
// destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTestDlInfo::~CIptvTestDlInfo()
    {
	VCXLOGLO1(">>>CIptvTestDlInfo::~CIptvTestDlInfo");
	TUint i=0;
	if(iIptvTestUtilities)
		{
		delete iIptvTestUtilities;
		iIptvTestUtilities=NULL;
		}

	//print and remove remaining file infos
	for(i = 0;i < iDlFileInfoArray.Count(); i++)
	    {
		RemoveDlFileInfoItem(i);
		}
	iDlFileInfoArray.ResetAndDestroy();

	VCXLOGLO1("<<<CIptvTestDlInfo::~CIptvTestDlInfo");
	}

// -----------------------------------------------------------------------------
// CIptvTestDlInfo::CIptvTestDlInfo
//
// -----------------------------------------------------------------------------
//
CIptvTestDlInfo::CIptvTestDlInfo()
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::CIptvTestDlInfo");

	VCXLOGLO1("<<<CIptvTestDlInfo::CIptvTestDlInfo");
	}
// -----------------------------------------------------------------------------
// CIptvTestDlInfo::ConstructL()
//
// -----------------------------------------------------------------------------
//
void CIptvTestDlInfo::ConstructL( )
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::ConstructL");
	iIptvTestUtilities = CIptvTestUtilities::NewL();

	VCXLOGLO1("<<<CIptvTestDlInfo::ConstructL");
	}

// -----------------------------------------------------------------------------
// CIptvTestDlInfo::DlStop
// destructor
// -----------------------------------------------------------------------------
//

TBool CIptvTestDlInfo::FindDlFileInfo(TInt& aIndex,TIptvServiceId aServiceId,TIptvContentId aContentId)
	{

	VCXLOGLO1(">>>CIptvTestDlInfo::FindDlFileInfo");
	_LIT(KFindDlFileInfo, "CIptvTestDlInfo::FindDlFileInfo:");
	TBool rv = FALSE;
	TUint i=0;
	for(i = 0;i < iDlFileInfoArray.Count(); i++)
			{
			if(iDlFileInfoArray[i]->iServiceId == aServiceId && iDlFileInfoArray[i]->iContentId == aContentId)
					{
					aIndex = i;
					rv = TRUE;
					break;
					}
			}
	//panic if not found!!
	if(!rv)
	    {
	    VCXLOGLO1("DlFileInfo instance not found, PANICing!!!");
	    User::Panic( KFindDlFileInfo, 0 );
        }

	VCXLOGLO1("<<<CIptvTestDlInfo::FindDlFileInfo");
	return rv;
	}

// -----------------------------------------------------------------------------
// CIptvTestDlInfo::RemoveDlFileInfoItem
// Removes download information when download has been ended
// -----------------------------------------------------------------------------
//
TBool CIptvTestDlInfo::RemoveDlFileInfoItem(TUint32 index)
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::RemoveDlFileInfoItem");
	_LIT(KRemoveDlFileInfo, "CIptvTestDlInfo::RemoveDlFileInfoItem:");

	TBool rv = ETrue;
	CIptvTestDlFileInfo * p = NULL;
	//check that index is not out of range
	if(index >= iDlFileInfoArray.Count())
		{
		rv = EFalse;
		VCXLOGLO1("DlFileInfo instance not found, PANICing!!!");
	    User::Panic( KRemoveDlFileInfo, 0 );
		}
	else
		{
		p = iDlFileInfoArray[index];
		iDlFileInfoArray.Remove(index);
		if(p)
			{
			delete p;
			p = NULL;
			}
		}
	VCXLOGLO1(">>>CIptvTestDlInfo::RemoveDlFileInfoItem");
	return rv;
	}

// -----------------------------------------------------------------------------
// CIptvTestDlInfo::CreateDlFileInfoItem
// Creates new CIptvTestDlFileInfo item when new download has been started
// -----------------------------------------------------------------------------
//
CIptvTestDlFileInfo* CreateDlFileInfoItem()
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::CreateDlFileInfoItem");
	CIptvTestDlFileInfo* p = NULL;

	p = CIptvTestDlFileInfo::NewL();

	VCXLOGLO1("<<<CIptvTestDlInfo::CreateDlFileInfoItem");
	return p;
	}

// -----------------------------------------------------------------------------
// CIptvTestDlInfo::DlStart
// Creates new item and stores information about new download
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestDlInfo::DlStart(TUint32 aIapId,
					TIptvServiceId aServiceId,
					TIptvContentId aContentId,
					TUint32 aDlContentNum,
					TIptvDownloadType aDlType)
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::DlStart");
	CIptvTestDlFileInfo* p = CreateDlFileInfoItem();

	HBufC* contentUrl = NULL;

	//get Iapname, servicename
	TBuf<255> iapName;
	TBuf<255> serviceName;

	//Get the name of the service

	CIptvService* pService = iIptvTestUtilities->GetServiceFromDb(aServiceId);
	if(pService)
    	{
    	serviceName.Append(pService->GetName());
    	delete pService;
    	pService = NULL;
    	}
	else
    	{
    	//do nothing
    	}

	//Get the name of the IAP
	iIptvTestUtilities->GetIapNameById(iapName, aIapId);

	RPointerArray<CIptvMediaContent> CAList;

	TInt err = KErrNone, ret = KErrNone;
	TRAP(err, ret = iIptvTestUtilities->GetContentAccessListL(aServiceId, aContentId, CAList));

	CleanupResetAndDestroyPushL(CAList);

    if(err != KErrNone)
    	{
    	VCXLOGLO2("* ERROR * GetContentAccessListLApiTest leaved !: %d", err);
    	}
    else if(ret != KErrNone)
    	{
		VCXLOGLO2("* ERROR * GetContentAccessListLApiTest returned error: %d", ret);
    	}
	else
		{
	    if(CAList.Count() <= aDlContentNum)
	    	{
			VCXLOGLO1("* ERROR * Content has not enough media contents. Downloading the first content.");
			aDlContentNum = 0;
	    	}
	    else
		if(CAList.Count() == 0)
			{
			VCXLOGLO1("* ERROR * Content has no media content. Can't start download!");

			}
		else
			{
			// Everything ok to start the DL
			iDlFileInfoArray.Append(p);

			const TPtrC url = CAList[aDlContentNum]->GetMediaContentUrl();

			TInt length = url.Length();
	        contentUrl = HBufC::NewLC(length);

	        TPtr16 pContentUrl = contentUrl->Des();
	        pContentUrl.Copy( url );
	        CleanupStack::Pop(contentUrl);

	        VCXLOGLO2("Content download url = %S", contentUrl);

			//store information to item
			p->DlStarted(
			            aIapId,
			            iapName,
			            aServiceId,
			            serviceName,
			            aContentId,
			            aDlType,
			            contentUrl); //ownership of the pointer moved!!!
				}

		}


    CleanupStack::PopAndDestroy(&CAList);

	VCXLOGLO1("<<<CIptvTestDlInfo::DlStart");
	}
// -----------------------------------------------------------------------------
// CIptvTestDlInfo::DlStartFailed
// stores the error id to download information
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestDlInfo::DlStartFailed(TIptvServiceId aServiceId,
							TIptvContentId aContentId,
							TIptvDlError aError)
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::DlStartFailed");
	TInt index = 0;

	//find the actual download information
	FindDlFileInfo(index, aServiceId, aContentId);

	//store the error id to downloadinformation
	iDlFileInfoArray[index]->DlStartFailed(aError);

	//print the information to log
	iDlFileInfoArray[index]->Print();

	//Remove the item which contains information about this download
	RemoveDlFileInfoItem(index);
	VCXLOGLO1(">>>CIptvTestDlInfo::DlStartFailed");
	}
// -----------------------------------------------------------------------------
// CIptvTestDlInfo::DlGoing
// stores name of the file to download information
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestDlInfo::DlGoing(TIptvServiceId aServiceId,
					TIptvContentId aContentId,
					CIptvMyVideosGlobalFileId& aFileId,
					TUint32 aState,
					TIptvDlError aError )
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::DlGoing");

	TBuf<255> videoName;//created because of the interface, not used
	TBuf<255> fullPath;
	TInt index = 0;

    TUint32 serviceId = 0; //created because of the interface, not used

	iIptvTestUtilities->GetVideoInfo(aFileId,serviceId,videoName,fullPath);

	//find the actual download information
	FindDlFileInfo(index, aServiceId, aContentId);

	//store the info to download information
	iDlFileInfoArray[index]->DlGoing(fullPath, aState, aError);

	VCXLOGLO1("<<<CIptvTestDlInfo::DlGoing");
	}
// -----------------------------------------------------------------------------
// CIptvTestDlInfo::DlUpdateState
// updates state of the download to download information
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestDlInfo::DlUpdateState(TIptvServiceId aServiceId,
							TIptvContentId aContentId,
							TUint32 aState,
							TIptvDlError aError )
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::DlUpdateState");
	TInt index = 0;

	//find the actual download information
	FindDlFileInfo(index, aServiceId, aContentId);

	//update state to download information
	iDlFileInfoArray[index]->DlUpdateState( aState, aError);
	VCXLOGLO1(">>>CIptvTestDlInfo::DlUpdateState");
	}
// -----------------------------------------------------------------------------
// CIptvTestDlInfo::DlStop
// updates state of the download to download information prints the info to log
// and removes the item
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestDlInfo::DlStop(TIptvServiceId aServiceId,
					TIptvContentId aContentId,
					TUint32 aState,
					TIptvDlError aError )
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::DlStop");
	TInt index = 0;
	TUint32 fileSize = 0;
	//find the actual download information
	FindDlFileInfo(index, aServiceId, aContentId);

	//Get the name of the downloaded file
	TDesC& fileName = iDlFileInfoArray[index]->GetFileName();

	//Get the size of the downloaded file
	iIptvTestUtilities->GetFileSize(fileName,fileSize);

	//store the information about stopping
	iDlFileInfoArray[index]->DlStopped(fileSize, aState, aError);

	//print the information to log
	iDlFileInfoArray[index]->Print();

	//Remove the item which contains information about this download
	RemoveDlFileInfoItem(index);
	VCXLOGLO1(">>>CIptvTestDlInfo::DlStop");
	}
// -----------------------------------------------------------------------------
// CIptvTestDlInfo::DlTimeout
// Prints and removes download information when download didn't get ready in time.
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestDlInfo::DlTimeOut(TIptvServiceId aServiceId,
					TIptvContentId aContentId
					 )
	{
	VCXLOGLO1(">>>CIptvTestDlInfo::DlTimeOut");

	TInt index = 0;

    //find the actual download information
	FindDlFileInfo(index, aServiceId, aContentId);

	//store the information about timeout
	iDlFileInfoArray[index]->DlTimeOut();

	//print the information to log
	//iDlFileInfoArray[index]->Print();

	//Remove the item which contains information about this download
	//RemoveDlFileInfoItem(index); changed, waiting for deleted - event

    VCXLOGLO1("<<<CIptvTestDlInfo::DlTimeOut");
    }
//  End of File
