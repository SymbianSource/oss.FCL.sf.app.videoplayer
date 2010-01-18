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
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroyPushL

#include "VCXTestLog.h"
#include "CIptvTestVCApiUtil.h"
#include "CIptvVodContentClient.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvMediaContent.h"
#include "VCXTestLog.h"
#include "CIptvTestVodContentAllDetails.h"

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
// CIptvTestVCApiUtil::CIptvTestVCApiUtil
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvTestVCApiUtil::CIptvTestVCApiUtil()
    {
    }

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvTestVCApiUtil::ConstructL()
    {
    iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait;
    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvTestVCApiUtil* CIptvTestVCApiUtil::NewL()
    {
    VCXLOGLO1(">>>CIptvTestVCApiUtil::NewL");
    CIptvTestVCApiUtil* self = new( ELeave ) CIptvTestVCApiUtil;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	VCXLOGLO1("<<<CIptvTestVCApiUtil::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::NewL
// Destructor
// -----------------------------------------------------------------------------
//
CIptvTestVCApiUtil::~CIptvTestVCApiUtil()
    {
    VCXLOGLO1(">>>CIptvTestVCApiUtil::~CIptvTestSMApiUtil");
    if(iActiveSchedulerWait)
	    {
	    ActiveWait(EActiveWaitStop);
	    delete iActiveSchedulerWait;
	    iActiveSchedulerWait = NULL;
	  	}
    delete iIptvVodContentClient;
    iIptvVodContentClient = NULL;
    VCXLOGLO1("<<<CIptvTestVCApiUtil::~CIptvTestVCApiUtil");
    }

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::GetContentIdsL
// Creates CIptvServiceInstance from parameters
// returns pointer to service if succeeded, otherwise NULL
// -----------------------------------------------------------------------------
//
TBool CIptvTestVCApiUtil::GetContentIdsL(TIptvServiceId aServiceId, TUint32 aIAPId, RArray<TIptvContentId>& aContentIdArray, TUint32 aMaxSize, TUint32 aMaxPlaytime)
    {
   	VCXLOGLO1(">>>CIptvTestVCApiUtil::GetContentIdsL");
    TBool rv = TRUE;
    TInt resp = KErrNone;

    TUint32 total=0;

    TBuf<255> string;
    string.Append(_L(""));

    TTime currentTime;
    TTime lastUpdateTime;
    TTimeIntervalMinutes minutes;

    iEpgUpdateError = KErrNone;

    currentTime.HomeTime();
    CheckVodContentClient( aServiceId );

    //Set IAP
    TRAPD(error,resp = iIptvVodContentClient->SetIapL(aIAPId));
    if(KErrNone != error || KErrNone != resp)
        {
        VCXLOGLO1("CIptvTestVCApiUtil: Couldn't set IAP!!");
        return FALSE;
        }

    resp = iIptvVodContentClient->GetUpdateTimeL(lastUpdateTime);

    if(resp != KErrNone)
        {
        VCXLOGLO2("CIptvTestVCApiUtil: GetUpdateTimeL error = %d", resp);
        VCXLOGLO1("<<<CIptvTestVCApiUtil::GetContentIdsL");
        return FALSE;
        }

    currentTime.MinutesFrom(lastUpdateTime, minutes);

    if(minutes.Int() > 60)
	    {
	    VCXLOGLO1("Epg update starts");
	    resp = iIptvVodContentClient->UpdateEcgL();
	    if(resp != KErrNone)
            {
            VCXLOGLO2("CIptvTestVCApiUtil: UpdateEcgL error = %d", resp);
            VCXLOGLO1("<<<CIptvTestVCApiUtil::GetContentIdsL");
            return FALSE;
            }

	    ActiveWait(EActiveWaitStart);//wait for Epg download
	    if(iEpgUpdateError != KIptvErrorEpgUpdateSuccessed)
            {//update failed!!
            VCXLOGLO2("CIptvTestVCApiUtil: EpgUpdate error = %d", iEpgUpdateError);
            VCXLOGLO1("<<<CIptvTestVCApiUtil::GetContentIdsL");
            return FALSE;
            }
	    }

   	RPointerArray<CIptvVodContentContentBriefDetails> data;
	CleanupResetAndDestroyPushL(data);

	VCXLOGLO2("count = %d", data.Count());

    resp = iIptvVodContentClient->GetEcgAllListL(string, 0, 1500, total, data);
    if(resp == KErrNone)
        {
        VCXLOGLO2("totalAmount = %d", total);

        for (TInt i = 0; i < data.Count(); i++)
            {
            VCXLOGLO2("Content id = %d",data[i]->iContentId);
            VCXLOGLO2("Content name = %S", &data[i]->iName);
            if( (data[i]->iSize <= aMaxSize || aMaxSize == 0) && (data[i]->iPlaytime < aMaxPlaytime || aMaxPlaytime == 0) )
            	{
            	aContentIdArray.Append(data[i]->iContentId);
            	}
            }
        }
    else
        {
        VCXLOGLO2("CIptvTestVCApiUtil: GetEcgAllListL error = %d", resp);
        rv = FALSE;
        }

    VCXLOGLO2("Added %d videos", aContentIdArray.Count());

    CleanupStack::PopAndDestroy(&data);
	data.Close();

    VCXLOGLO1("<<<CIptvTestVCApiUtil::GetContentIdsL");
    // check rv!!
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::GetEcgCategoryListL
// -----------------------------------------------------------------------------
//
void CIptvTestVCApiUtil::GetEcgCategoryListL(TUint32 aServiceId, TUint32 aParentCategoryId, RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList)
    {
    VCXLOGLO1(">>>CIptvTestVCApiUtil::GetEcgCategoryListL");
    CheckVodContentClient( aServiceId );
    User::LeaveIfError( iIptvVodContentClient->GetEcgCategoryListL(aParentCategoryId, aCategoryList) );
    for( TInt i = 0; i < aCategoryList.Count(); i++)
        {
        VCXLOGLO3("CIptvTestVCApiUtil:: Category '%S' with id '%d'", &aCategoryList[i]->iName, aCategoryList[i]->iCategoryId);        
        }
    VCXLOGLO1("<<<CIptvTestVCApiUtil::GetEcgCategoryListL");
    }

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::GetEcgListL
// -----------------------------------------------------------------------------
//
void CIptvTestVCApiUtil::GetEcgListL(TUint32 aServiceId, TUint32 aCategoryId, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList )
    {
    VCXLOGLO1(">>>CIptvTestVCApiUtil::GetEcgListL");
    TBuf<10> searchString(_L("") );
    TUint32 totalAmount = 0;
    CheckVodContentClient( aServiceId );
    User::LeaveIfError( iIptvVodContentClient->GetEcgListL( aCategoryId, searchString, 0, 0, totalAmount, aEcgList ) );
    for( TInt i = 0; i < aEcgList.Count(); i++ )
        {
        VCXLOGLO4("CIptvTestVCApiUtil:: Content '%S' at index '%d' with id '%d'.", &aEcgList[i]->iName, i, aEcgList[i]->iContentId);                    
        }
    VCXLOGLO1("<<<CIptvTestVCApiUtil::GetEcgListL");
    }

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::GetVodContentContentDetailsL
// -----------------------------------------------------------------------------
//
TInt CIptvTestVCApiUtil::GetVodContentContentDetailsL(TUint32 aServiceId, TUint32 aContentId, CIptvVodContentContentFullDetails& aContentDetails)
	{
    VCXLOGLO1(">>>CIptvTestVCApiUtil::GetVodContentContentDetailsL");
    CheckVodContentClient( aServiceId );
    TInt ret = iIptvVodContentClient->GetContentDetailsL(aContentId, aContentDetails);        
    VCXLOGLO1("<<<CIptvTestVCApiUtil::GetVodContentContentDetailsL");
    return ret;
	}

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::GetContentAccessListL
// -----------------------------------------------------------------------------
//
TInt CIptvTestVCApiUtil::GetContentAccessListL(TUint32 aServiceId, TUint32 aContentId, RPointerArray<CIptvMediaContent>& aCAList)
	{
    VCXLOGLO1(">>>CIptvTestVCApiUtil::GetContentAccessListL");
    CheckVodContentClient( aServiceId );
    TInt ret = iIptvVodContentClient->GetContentAccessListL(aContentId, aCAList);
    for( TInt a = 0; a < aCAList.Count(); a++ )
        {
        VCXLOGLO3("CIptvTestVCApiUtil:: mediaContent index: %d, expression: %d", a, aCAList[a]->iExpression);
        }    
    VCXLOGLO1("<<<CIptvTestVCApiUtil::GetContentAccessListL");
    return ret;
	}

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::GetVodContentAllDetailsListL
// -----------------------------------------------------------------------------
//
void CIptvTestVCApiUtil::GetVodContentAllDetailsListL( TUint32 aServiceId, RPointerArray<CIptvTestVodContentAllDetails>& aDetailsList )
	{
    VCXLOGLO1(">>>CIptvTestVCApiUtil::GetVodContentAllDetailsListL");

    CheckVodContentClient( aServiceId );
    RPointerArray<CIptvVodContentCategoryBriefDetails> categoryList;
    CleanupResetAndDestroyPushL( categoryList );

    User::LeaveIfError( iIptvVodContentClient->GetEcgCategoryListL( 0, categoryList) );

    TInt count = categoryList.Count();

    // Loop categories
    for (TInt i = 0; i < count; i++)
        {
        VCXLOGLO3("CIptvTestVCApiUtil:: Category '%S' with id '%d'", &categoryList[i]->iName, categoryList[i]->iCategoryId);

        RPointerArray<CIptvVodContentContentBriefDetails> ecgList;
        CleanupResetAndDestroyPushL( ecgList );

        TBuf<10> searchString(_L("") );
        TUint32 totalAmount = 0;
        User::LeaveIfError( iIptvVodContentClient->GetEcgListL( categoryList[i]->iCategoryId, searchString, 0, 0, totalAmount, ecgList ) );

        // Loop ECG brief details
        for (TInt e = 0; e < ecgList.Count(); e++)
            {
            CIptvVodContentContentFullDetails* contentDetails = CIptvVodContentContentFullDetails::NewL();
            CleanupStack::PushL( contentDetails );

            VCXLOGLO4("CIptvTestVCApiUtil:: Get full details, content '%S' at index '%d' with id '%d'.", &ecgList[e]->iName, e, ecgList[e]->iContentId);            
            User::LeaveIfError( iIptvVodContentClient->GetContentDetailsL( ecgList[e]->iContentId, *contentDetails ) );

            // Construct all details 
            CIptvTestVodContentAllDetails* allDetails = CIptvTestVodContentAllDetails::NewL();
            aDetailsList.Append( allDetails );

            User::LeaveIfError( iIptvVodContentClient->GetContentAccessListL( ecgList[e]->iContentId, allDetails->iMediaContents) );

            TInt mediaCount = allDetails->iMediaContents.Count();
            for( TInt a = 0; a < mediaCount; a++ )
                {
                VCXLOGLO3("CIptvTestVCApiUtil:: mediaContent index: %d, expression: %d", a, allDetails->iMediaContents[a]->iExpression);
                }
            
            allDetails->iCategoryId = categoryList[i]->iCategoryId;
            allDetails->iFullDetails = contentDetails;
            allDetails->iBriefDetails = ecgList[e];

            CleanupStack::Pop( contentDetails );
            }

        CleanupStack::Pop( &ecgList );
        ecgList.Reset();
        }

    CleanupStack::PopAndDestroy( &categoryList );
    VCXLOGLO1("<<<CIptvTestVCApiUtil::GetVodContentAllDetailsListL");
	}

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::CheckVodContentClient
// -----------------------------------------------------------------------------
//
void CIptvTestVCApiUtil::CheckVodContentClient( TInt aServiceId )
    {
    if( !iIptvVodContentClient || iCurrentService != aServiceId )
        {
        delete iIptvVodContentClient;
        iIptvVodContentClient = NULL;
        iCurrentService = aServiceId;
        iIptvVodContentClient = CIptvVodContentClient::NewL( aServiceId, *this );
        }
    }

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::ActiveWait
// Starts or stops CActiveSchedulerWait.
// -----------------------------------------------------------------------------
//
void CIptvTestVCApiUtil::ActiveWait( TIptvTestActiveWaitCmd aActiveWaitCmd )
    {
    VCXLOGLO1(">>>CIptvTestVCApiUtil::ActiveWait");
    VCXLOGLO2("aActiveWaitCmd=%d", aActiveWaitCmd );

    switch( aActiveWaitCmd )
        {
        case EActiveWaitStart:
            {
            if( !iActiveSchedulerWait->IsStarted() )
                {
                VCXLOGLO1("iActiveSchedulerWait->Start()");
                iActiveSchedulerWait->Start();
                }
            else
                {
                VCXLOGLO1("ActiveSchedulerWait already started");
                }
            }
            break;
        case EActiveWaitStop:
            {
            if( iActiveSchedulerWait->IsStarted() )
                {
            VCXLOGLO1("iActiveSchedulerWait->AsyncStop()");
                iActiveSchedulerWait->AsyncStop();
                }
            else
                {
                VCXLOGLO1("ActiveSchedulerWait already stopped");
                }
            }
            break;
        default:
            break;
        }

    VCXLOGLO1("<<<CIptvTestVCApiUtil::ActiveWait");
    }

// -----------------------------------------------------------------------------
// CIptvTestVCApiUtil::
//
// -----------------------------------------------------------------------------
//
void CIptvTestVCApiUtil::HandleEpgManagerMsgL(TInt aMsg, TInt aInfo, TIptvServiceId aServiceId)
    {
    VCXLOGLO1(">>>CIptvTestVCApiUtil::HandleEpgManagerMsgL");
    VCXLOGLO2("CIptvTestVCApiUtil::HandleEpgManagerMsgL: aMsg = %d", aMsg);
    VCXLOGLO2("CIptvTestVCApiUtil::HandleEpgManagerMsgL: aInfo = %d", aInfo);
    VCXLOGLO2("CIptvTestVCApiUtil::HandleEpgManagerMsgL: aServiceId = %d", aServiceId);

     switch (aMsg)
        {
        case KIptvErrorEpgUpdateFailed:
            {
            VCXLOGLO1("CIptvTestVCApiUtil:Epg Update failed");
            iEpgUpdateError = aMsg;
            ActiveWait(EActiveWaitStop);
            /*****
            typedef enum
            {
            EIptvDlNoError,

            //errors before the download starts
            EIptvDlMessageAlreadyPending,
            EIptvDlTypeNotSupported,
              EIptvDlMaxDownloadsExceeded,
              KIptvDlCouldNotGetServiceData, //wrong service id?
              EIptvDlEpgManagerNotReady,
              EIptvDlCouldNotGetContentFullDetails,
              EIptvDlFailedToAddNewVideoToMyVideos,
              EIptvDlCouldNotConnectToS60DlMgr,
              EIptvDlCouldNotSetIapAttrToS60DlMgr,
              EIptvDlCouldNotCreateS60Download,
              EIptvDlCouldNotStartS60Download,

            //errors during & after download
              EIptvDlConnectionFailed, //IAP wrong or no network coverage
              EIptvDlAuthFailed,
              EIptvDlProxyAuthFailed,
              EIptvDlContentNotFound, // HTTP 402
              EIptvDlDiskFull,
              EIptvDlDestFileInUse,
              EIptvDlBadUrl,          //syntax error in url
              EIptvDlMmcRemoved,
              EIptvDlGeneral          //wrong url, content removed, network does not work, ...
              } TIptvDlError;

            *****/
            }
            break;
        case KIptvErrorEpgUpdateSuccessed:
            {
            VCXLOGLO1("CIptvTestVCApiUtil:Epg Update succeeded");
            iEpgUpdateError = aMsg;
            ActiveWait(EActiveWaitStop);
            }
            break;
        case KIptvVodUpdateStarted:
            {
            VCXLOGLO1("CIptvTestVCApiUtil:Epg Update started");
            iEpgUpdateError = aMsg;
            }
            break;
        case KIptvVodUpdateNotStarted:
            {
            VCXLOGLO1("CIptvTestVCApiUtil:Epg Update not started");
            iEpgUpdateError = aMsg;
            ActiveWait(EActiveWaitStop);
            }
            break;

        case KIptvErrorVodNoIap:
            {
            VCXLOGLO1("CIptvTestVCApiUtil:Epg No Iap");
            iEpgUpdateError = aMsg;
            ActiveWait(EActiveWaitStop);
            }
            break;

        default:
            {
             VCXLOGLO1("CIptvTestVCApiUtil::aMsg default case");
            iEpgUpdateError = aMsg;
            //ActiveWait(EActiveWaitStop);
            }
        }

    /**********/
    VCXLOGLO1("<<<CIptvTestVCApiUtil::aMsg");
    }

//  End of File
