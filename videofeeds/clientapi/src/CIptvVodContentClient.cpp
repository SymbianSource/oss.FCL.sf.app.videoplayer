/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Implements VoD ECG content API.*
*/




#include <e32svr.h>
#include <s32mem.h> 
#include "IptvDebug.h"
#include "CIptvMediaContent.h"

#include "MIptvVodContentClientObserver.h"
#include "CIptvVodContentClient.h"
#include "IptvClientServerCommon.h"
#include "IptvServer.pan"
#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvVodContentUpdateObserver.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "TIptvRssSearchQuery.h"

const TInt KTUint32SizeInBytes( 4 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvVodContentClient::CIptvVodContentClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CIptvVodContentClient::CIptvVodContentClient(TIptvServiceId aServiceId, MIptvVodContentClientObserver& aClientObserver) : 
    CActive( EPriorityStandard ), 
    iClientObserver( aClientObserver ), 
    iMsg(NULL), 
    iMsgPtr((unsigned char*)0, 0), 
    iServiceId(aServiceId), 
    iObserver(NULL)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVodContentClient::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::ConstructL");         
             
    User::LeaveIfError(iSession.Connect());
    iObserver = CIptvVodContentUpdateObserver::NewL(iClientObserver, iSession, iServiceId);
    TRequestStatus status;
    SendRequestL(EIptvEngineVodContentServiceId, iServiceId, status);

    if (status.Int() != KErrNone)    
        {
        User::Leave(status.Int());
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodContentClient* CIptvVodContentClient::NewL(TIptvServiceId aServiceId, MIptvVodContentClientObserver& aClientObserver)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::NewL");         

	CIptvVodContentClient* self = new ( ELeave ) CIptvVodContentClient(aServiceId, aClientObserver);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodContentClient *CIptvVodContentClient::NewLC(TIptvServiceId aServiceId, MIptvVodContentClientObserver& aClientObserver)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::NewLC");         

	CIptvVodContentClient* self = new ( ELeave ) CIptvVodContentClient(aServiceId, aClientObserver);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::~CIptvVodContentClient
// Destructor.
// -----------------------------------------------------------------------------
//
CIptvVodContentClient::~CIptvVodContentClient()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::~CIptvVodContentClient");         

    Cancel();         
    SendCancelRequest();

    delete iMsg;
    delete iObserver;   
    iSession.Close();                    
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::CleanupL
// 
// -----------------------------------------------------------------------------
//
void CIptvVodContentClient::CleanupL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::CleanupL");

    delete iMsg;
    iMsg = NULL;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::GetEcgCategoryList
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::GetEcgCategoryListL(TIptvCategoryId aParentCategoryId, RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetEcgCategoryListL");

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {        
        SendSizeRequestL(EIptvEngineVodContentGetEcgCategoryListSizeReq, aParentCategoryId, status);        
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        else
            {
            HandleVodContentGetEcgCategoryListSizeReqL();
            HandleVodContentGetEcgCategoryListDataReqL(aCategoryList);
            }
        }

#ifdef _DEBUG

    TInt count = aCategoryList.Count();
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetEcgCategoryListL ----------------------------------------------------");
    IPTVLOGSTRING2_LOW_LEVEL("CIptvVodContentClient::GetEcgCategoryListL Parent id = %d", aParentCategoryId);
    IPTVLOGSTRING2_LOW_LEVEL("CIptvVodContentClient::GetEcgCategoryListL Category count = %d", count);

    for (TInt i = 0; i < count; i++)
        {
        TDesC& name = aCategoryList[i]->iName;
        IPTVLOGSTRING2_LOW_LEVEL("CIptvVodContentClient::GetEcgCategoryListL Category name = %S", &name);        
        IPTVLOGSTRING2_LOW_LEVEL("CIptvVodContentClient::GetEcgCategoryListL Category ID = %d", aCategoryList[i]->iCategoryId);        
        }

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetEcgCategoryListL ----------------------------------------------------");

#endif

    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::GetCategoryDetails
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::GetCategoryDetailsL(TIptvCategoryId aCategoryId, CIptvVodContentCategoryBriefDetails& aCategory)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetCategoryDetailsL");         

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {       
        SendSizeRequestL(EIptvEngineVodContentGetCategoryDetailsSizeReq, aCategoryId, status);
        if (status.Int() != KErrNone)
            {
            CleanupL();            
            }
        else
            {
            HandleVodContentGetCategoryDetailsSizeReqL();
            HandleVodContentGetCategoryDetailsDataReqL(aCategory);
            }
        }    
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::GetParentCategory
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::GetParentCategoryL(TIptvCategoryId aCategoryId, TIptvCategoryId& aParentId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetParentCategoryL");         

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {        
        SendRequestL(EIptvEngineVodContentGetParentCategoryReq, aCategoryId, status);
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        else
            {
            HandleVodContentGetParentCategoryReqL(aParentId);
            }
        }    
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::GetEcgList
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::GetEcgListL(TIptvCategoryId aCategoryId, TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetEcgListL");         

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {        
        SendSizeRequestL(EIptvEngineVodContentGetEcgListSizeReq, aCategoryId, aFrom, aAmount, aSearchString, status);
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        else
            {
            HandleVodContentGetEcgListSizeReqL();
            HandleVodContentGetEcgListDataReqL(aTotalAmount, aEcgList);
            }  
        }

#ifdef _DEBUG

    TInt count = aEcgList.Count();
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetEcgListL ----------------------------------------------------");
    IPTVLOGSTRING2_LOW_LEVEL("CIptvVodContentClient::GetEcgListL Content info for category = %d", aCategoryId);
    IPTVLOGSTRING2_LOW_LEVEL("CIptvVodContentClient::GetEcgListL Item count = %d", count);

    for (TInt i = 0; i < count; i++)
        {
        TDesC& name = aEcgList[i]->iName;
        IPTVLOGSTRING2_LOW_LEVEL("CIptvVodContentClient::GetEcgListL Name = %S", &name);
        IPTVLOGSTRING2_LOW_LEVEL("CIptvVodContentClient::GetEcgListL GFID = %d", aEcgList[i]->iFileId->iFileId);                    
        IPTVLOGSTRING2_LOW_LEVEL("CIptvVodContentClient::GetEcgListL GDID = %d", aEcgList[i]->iFileId->iDrive);                    
        }

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetEcgListL ----------------------------------------------------");

#endif

    return status.Int();  
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::GetEcgAllList
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::GetEcgAllListL(TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList)
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetEcgAllListL");         

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {
        SendSizeRequestL(EIptvEngineVodContentGetEcgAllListSizeReq, aFrom, aAmount, aSearchString, status);    
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        else
            {            
            HandleVodContentGetEcgAllListSizeReqL();
            HandleVodContentGetEcgAllListDataReqL(aTotalAmount, aEcgList);
            }
        }
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::GetContentDetails
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::GetContentDetailsL(TIptvContentId aContentId, CIptvVodContentContentFullDetails& aContentFullDetails)
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetContentDetailsL");         

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {                
        SendSizeRequestL(EIptvEngineVodContentGetContentDetailsSizeReq, aContentId, status);
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        else
            {            
            HandleVodContentGetContentDetailsSizeReqL();
            HandleVodContentGetContentDetailsDataReqL(aContentFullDetails);
            }
        }
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SetIapL
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::SetIapL(const TUint32 aIap)
    {           
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SetIapL");         

    TRequestStatus status(KErrNone);
    if (!IsActive())
        { 
        SendSizeRequestL(EIptvEngineVodContentSetIap, aIap, status);
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        }
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::UpdateEcg
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::UpdateEcgL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::UpdateEcgL");

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {        
        SendRequestL(EIptvEngineVodContentUpdateEcg, 0, status);
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        }
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::CheckGroupL
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::CheckGroupL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::CheckGroupL");

    TRequestStatus status( KErrNone );
    if ( !IsActive() )
        {        
        SendRequestL( EIptvEngineVodContentCheckGroup, 0, status );
        if ( status.Int() != KErrNone )
            {
            CleanupL();
            }
        }
    
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::GetUpdateTimeL
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::GetUpdateTimeL(TTime& aTime)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetUpdateTimeL");

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {
        SendUpdateTimeRequestL(status);
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        else
            {
            HandleVodContentGetUpdateTimeL(aTime);
            }
        }
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::GetContentAccessListL
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::GetContentAccessListL(TUint32 aContentId, RPointerArray<CIptvMediaContent>& aCAList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetContentAccessListL");

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {        
        SendSizeRequestL(EIptvEngineVodContentGetCAListSizeReq, aContentId, status);
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        else
            {
            HandleVodContentGetCAListSizeReqL();
            HandleVodContentGetCAListDataReqL(aCAList);
            }
        }   
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SetLastPositionL
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::SetLastPositionL(TUint32 aContentId, TUint32 aIndex, TUint32 aPosition)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::GetContentAccessListL");

    TRequestStatus status(KErrNone);
    if (!IsActive())
        {        
        SendLastPositionL(EIptvEngineVodContentSetLastPosition, aContentId, aIndex, aPosition, status);
        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        }
    return status.Int();    
    }


// -----------------------------------------------------------------------------
// CIptvVodContentClient::SetMpxIdL
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::SetMpxIdL( TUint32 aContentId,
                                                TUint32 aCaIndex,
                                                TUint32 aMpxId )
    {
    TRequestStatus status( KErrServerBusy );
    if ( !IsActive() )
        {        
        SendMpxIdL( EIptvEngineVodContentSetMpxId, aContentId, aCaIndex, aMpxId, status );
        if ( status.Int() != KErrNone )
            {
            CleanupL();
            }
        }
    return status.Int();    
    
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendLastPositionL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendLastPositionL(TInt aMessage, TUint32 aContentId, TUint32 aIndex, TUint32 aPosition, TRequestStatus& aStatus)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendLastPositionL");
    
    delete iMsg;
    iMsg = NULL;
    
    iMsg = HBufC8::NewL(KIptvVodSendLastPositionSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    stream.WriteUint32L(aContentId);
    stream.WriteUint32L(aIndex);
    stream.WriteUint32L(aPosition);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, aStatus);
    User::WaitForRequest(aStatus);
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendMpxIdL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendMpxIdL( TInt aMessage,
                                        TUint32 aContentId,
                                        TUint32 aCaIndex,
                                        TUint32 aMpxId,
                                        TRequestStatus& aStatus )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendMpxIdL");
    
    delete iMsg;
    iMsg = NULL;
    
    iMsg = HBufC8::NewL( KIptvVodSendMpxIdSize );
    iMsgPtr.Set( iMsg->Des() );

    RDesWriteStream stream;
    stream.Open( iMsgPtr );
    CleanupClosePushL( stream );
    stream.WriteUint32L( aContentId );
    stream.WriteUint32L( aCaIndex);
    stream.WriteUint32L( aMpxId );
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest( aMessage, iMsgPtr, aStatus );
    User::WaitForRequest( aStatus );
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::RunL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::RunL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::RunL");            
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::DoCancel
// -----------------------------------------------------------------------------
void CIptvVodContentClient::DoCancel()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::DoCancel");            
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetEcgCategoryListSizeReq
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetEcgCategoryListSizeReqL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetEcgCategoryListSizeReqL");            

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 dataSize = stream.ReadUint32L();                       	    
        CleanupStack::PopAndDestroy( &stream );
    
        IPTVLOGSTRING2_LOW_LEVEL("HandleVodContentGetEcgCategoryListSizeReq:: Data size = %d", dataSize);
        
        delete iMsg;
        iMsg = NULL;
       
        iMsg = HBufC8::NewL(dataSize);
        iMsgPtr.Set(iMsg->Des());
     
        TRequestStatus status;
        iSession.SendRequest(EIptvEngineVodContentGetEcgCategoryListDataReq, iMsgPtr, status);
        User::WaitForRequest(status);  

        if (status.Int() != KErrNone)
            {
            CleanupL();
            }     
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetEcgCategoryListDataReq
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetEcgCategoryListDataReqL(RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList)
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetEcgCategoryListDataReqL");            

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 count = stream.ReadUint32L();
        for (TUint32 i = 0; i < count; i++)
            {
            CIptvVodContentCategoryBriefDetails* brief = CIptvVodContentCategoryBriefDetails::NewL();
            CleanupStack::PushL(brief);
            brief->InternalizeL(stream);
            aCategoryList.AppendL(brief);
            CleanupStack::Pop(brief);            
            }
        CleanupStack::PopAndDestroy( &stream );
        delete iMsg;
        iMsg = NULL;
        }                
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentClient::SendRequestL(
    TInt aMessage,
    CIptvMyVideosGlobalFileId& aFileIdParam,
    TUint32 aUintParam )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "My Videos Mgr ## CIptvMyVideosClient::SendRequestL" );

    TUint32 dataSize(
        aFileIdParam.CountExternalizeSize() + KTUint32SizeInBytes );

    HBufC8* ipcMsg = HBufC8::NewL( dataSize );
    CleanupStack::PushL( ipcMsg ); // 1->
            
    TPtr8 ipcMsgPtr( ipcMsg->Des() );
    ipcMsgPtr.Zero();

    RDesWriteStream stream;
    stream.Open( ipcMsgPtr );
    CleanupClosePushL( stream );
    aFileIdParam.ExternalizeL( stream );
    stream.WriteUint32L( aUintParam );
    CleanupStack::PopAndDestroy( &stream );

    TRequestStatus status;
    iSession.SendRequest( aMessage, ipcMsgPtr, status );
    User::WaitForRequest( status);

    CleanupStack::PopAndDestroy( ipcMsg ); // <-1

    return status.Int();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendRequestL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendRequestL(TInt aMessage, TUint32 aParam, TRequestStatus& aStatus)
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendRequestL");

    SendSizeRequestL(aMessage, aParam, aStatus);
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendSizeRequest
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendSizeRequestL(TInt aMessage, TUint32 aParam, TRequestStatus& aStatus)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendSizeRequestL");
    
    delete iMsg;
    iMsg = NULL;
    
    iMsg = HBufC8::NewL(KIptvSizeRequestSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );
    stream.WriteUint32L(aParam);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, aStatus);
    User::WaitForRequest(aStatus);
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendSizeRequest
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendSizeRequestL( TInt aMessage,
                                              TUint32 aUintParam1,
                                              TUint32 aUintParam2,
                                              TUint32 aUintParam3,
                                              const TDesC& aDesCParam,
                                              TRequestStatus& aStatus )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendSizeRequestL");
    
    delete iMsg;
    iMsg = NULL;
        
    // length (2), TUint32 aUintParam1, TUint32 aUintParam2, TUint32 aUintParam3, TDesC& aDesCParam)
    TUint32 length = aDesCParam.Length();
    TUint32 dataSize = 2 + 4 + 4 + 4 + (length * 2);
    IPTVLOGSTRING2_LOW_LEVEL("SendSizeRequestL:: data size = %d", dataSize);
    
    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);
    CleanupClosePushL( stream );      
    stream.WriteUint32L(aUintParam1);
    stream.WriteUint32L(aUintParam2);
    stream.WriteUint32L(aUintParam3);
    CIptvUtil::WriteDesToStreamL(aDesCParam, stream);    
    CleanupStack::PopAndDestroy( &stream );
    
    iSession.SendRequest(aMessage, iMsgPtr, aStatus);
    User::WaitForRequest(aStatus);            
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendSizeRequest
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendSizeRequestL( TInt aMessage,
                                              TUint32 aUintParam1,
                                              TUint32 aUintParam2,
                                              const TDesC& aDesCParam,
                                              TRequestStatus& aStatus )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendSizeRequestL");
    
    delete iMsg;
    iMsg = NULL;
            
    // length (2), TUint32 aUintParam1, TUint32 aUintParam2, TDesC& aDesCParam)
    TUint32 length = aDesCParam.Length();
    TUint32 dataSize = 2 + 4 + 4 + (length * 2);
    IPTVLOGSTRING2_LOW_LEVEL("SendSizeRequestL:: data size = %d", dataSize);
    
    iMsg = HBufC8::NewL(dataSize);
    iMsgPtr.Set(iMsg->Des());

    RDesWriteStream stream;
    stream.Open(iMsgPtr);  
    CleanupClosePushL( stream );     
    stream.WriteUint32L(aUintParam1);
    stream.WriteUint32L(aUintParam2);
    CIptvUtil::WriteDesToStreamL(aDesCParam, stream);
    CleanupStack::PopAndDestroy( &stream );

    iSession.SendRequest(aMessage, iMsgPtr, aStatus);
    User::WaitForRequest(aStatus);           
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetCategoryDetailsSizeReq
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetCategoryDetailsSizeReqL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetCategoryDetailsSizeReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );   
        TUint32 dataSize = stream.ReadUint32L();                       	    
        CleanupStack::PopAndDestroy( &stream );

        IPTVLOGSTRING2_LOW_LEVEL("HandleVodContentGetCategoryDetailsSizeReq:: Data size = %d", dataSize);    
        
        delete iMsg;
        iMsg = NULL;
        
        iMsg = HBufC8::NewL(dataSize);
        iMsgPtr.Set(iMsg->Des());                                                  
     
        TRequestStatus status;
        iSession.SendRequest(EIptvEngineVodContentGetCategoryDetailsDataReq, iMsgPtr, status);
        User::WaitForRequest(status);

        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetCategoryDetailsDataReq
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetCategoryDetailsDataReqL(CIptvVodContentCategoryBriefDetails& aCategory)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetCategoryDetailsDataReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aCategory.InternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );    
        }   
       
    delete iMsg;
    iMsg = NULL;        
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetParentCategoryReqL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetParentCategoryReqL(TIptvCategoryId& aParentId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetParentCategoryReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aParentId = stream.ReadUint32L();                       	    
        CleanupStack::PopAndDestroy( &stream );      
        }   
       
    delete iMsg;
    iMsg = NULL;      
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetEcgListSizeReq
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetEcgListSizeReqL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetEcgListSizeReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 dataSize = stream.ReadUint32L();                       	    
        CleanupStack::PopAndDestroy( &stream );   
    
        IPTVLOGSTRING2_LOW_LEVEL("HandleVodContentGetEcgListSizeReq:: Data size = %d", dataSize);    
        
        delete iMsg;
        iMsg = NULL;
        
        iMsg = HBufC8::NewL(dataSize);
        iMsgPtr.Set(iMsg->Des());                                                  
     
        TRequestStatus status;
        iSession.SendRequest(EIptvEngineVodContentGetEcgListDataReq, iMsgPtr, status);
        User::WaitForRequest(status);

        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetCAListSizeReqL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetCAListSizeReqL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetCAListSizeReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 dataSize = stream.ReadUint32L();                       	    
        CleanupStack::PopAndDestroy( &stream );   
    
        IPTVLOGSTRING2_LOW_LEVEL("HandleVodContentGetCAListSizeReqL:: Data size = %d", dataSize);    
        
        delete iMsg;
        iMsg = NULL;
        
        iMsg = HBufC8::NewL(dataSize);
        iMsgPtr.Set(iMsg->Des());                                                  
     
        TRequestStatus status;        
        iSession.SendRequest(EIptvEngineVodContentGetCAListDataReq, iMsgPtr, status);
        User::WaitForRequest(status);

        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetCAListDataReqL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetCAListDataReqL(RPointerArray<CIptvMediaContent>& aCAList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetCAListDataReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 count = stream.ReadUint32L();        
        for (TUint32 i = 0; i < count; i++)
            {
            CIptvMediaContent* content = CIptvMediaContent::NewL();
            CleanupStack::PushL(content);
            content->InternalizeL(stream);
            aCAList.AppendL(content);
            CleanupStack::Pop(content);            
            }
        CleanupStack::PopAndDestroy( &stream );
        }    

    delete iMsg;
    iMsg = NULL;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetEcgListSizeReq
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetEcgListDataReqL(TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetEcgListDataReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 count = stream.ReadUint32L();
        aTotalAmount = stream.ReadUint32L();
        for (TUint32 i = 0; i < count; i++)
            {
            CIptvVodContentContentBriefDetails* brief = CIptvVodContentContentBriefDetails::NewL();
            CleanupStack::PushL(brief);
            brief->InternalizeL(stream);
            aEcgList.AppendL(brief);
            CleanupStack::Pop(brief);            
            }
        CleanupStack::PopAndDestroy( &stream );
        }    

    delete iMsg;
    iMsg = NULL;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetEcgAllListSizeReqL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetEcgAllListSizeReqL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetEcgAllListSizeReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 dataSize = stream.ReadUint32L();                       	    
        CleanupStack::PopAndDestroy( &stream );   
    
        IPTVLOGSTRING2_LOW_LEVEL("HandleVodContentGetEcgAllListSizeReqL:: Data size = %d", dataSize);    
        
        delete iMsg;
        iMsg = NULL;
        
        iMsg = HBufC8::NewL(dataSize);
        iMsgPtr.Set(iMsg->Des());                                                  
     
        TRequestStatus status;
        iSession.SendRequest(EIptvEngineVodContentGetEcgAllListDataReq, iMsgPtr, status);
        User::WaitForRequest(status);

        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetEcgAllListDataReqL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetEcgAllListDataReqL(TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetEcgAllListDataReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 count = stream.ReadUint32L();
        aTotalAmount = stream.ReadUint32L();
        for (TUint32 i = 0; i < count; i++)
            {
            CIptvVodContentContentBriefDetails* brief = CIptvVodContentContentBriefDetails::NewL();
            CleanupStack::PushL(brief);
            brief->InternalizeL(stream);
            aEcgList.AppendL(brief);
            CleanupStack::Pop(brief);            
            }
        CleanupStack::PopAndDestroy( &stream );
        }    

    delete iMsg;
    iMsg = NULL;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetContentDetailsSizeReqL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetContentDetailsSizeReqL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetContentDetailsSizeReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 dataSize = stream.ReadUint32L();                       	    
        CleanupStack::PopAndDestroy( &stream );

        IPTVLOGSTRING2_LOW_LEVEL("HandleVodContentGetContentDetailsSizeReqL:: Data size = %d", dataSize);    
        
        delete iMsg;
        iMsg = NULL;
        
        iMsg = HBufC8::NewL(dataSize);
        iMsgPtr.Set(iMsg->Des());                                                  
     
        TRequestStatus status;
        iSession.SendRequest(EIptvEngineVodContentGetContentDetailsDataReq, iMsgPtr, status);
        User::WaitForRequest(status);

        if (status.Int() != KErrNone)
            {
            CleanupL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetContentDetailsDataReqL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetContentDetailsDataReqL(CIptvVodContentContentFullDetails& aContentFullDetails)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetContentDetailsDataReqL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aContentFullDetails.InternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );  
        }   
       
    delete iMsg;
    iMsg = NULL;        
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendCancelRequest
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendCancelRequest()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendCancelRequest");

    TRequestStatus status;
    iSession.SendRequest(EIptvEngineVodContentCancel, status);
    User::WaitForRequest(status);
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendCancelUpdateRequest
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendCancelUpdateRequest()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendCancelUpdateRequest");

    TRequestStatus status;
    iSession.SendRequest( EIptvEngineVodContentCancelUpdate, status );
    User::WaitForRequest(status);
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendUpdateTimeRequestL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendUpdateTimeRequestL(TRequestStatus& aStatus)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendUpdateTimeRequestL");

    delete iMsg;
    iMsg = NULL;
    
    iMsg = HBufC8::NewL(KIptvVodGetUpdateTimeRequestSize);
    iMsgPtr.Set(iMsg->Des());
        
    iSession.SendRequest(EIptvEngineVodGetUpdateTime, iMsgPtr, aStatus);
    User::WaitForRequest(aStatus);
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::HandleVodContentGetUpdateTimeL
// -----------------------------------------------------------------------------
void CIptvVodContentClient::HandleVodContentGetUpdateTimeL(TTime& aTime)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentGetUpdateTimeL");

    if (iMsg)
        {
        RDesReadStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        TUint32 lower = stream.ReadUint32L();
        TUint32 upper = stream.ReadUint32L();
        TInt64 time = 0;
        time = (TInt64) lower;    
        TInt64 longUpper = (TInt64) upper;
        longUpper = longUpper << 32;        
        longUpper &= (0xFFFFFFFF00000000ULL);    
        time |= longUpper;
        aTime = time;  
        CleanupStack::PopAndDestroy( &stream );       
        }    

    delete iMsg;
    iMsg = NULL;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::Search
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::Search(
							TIptvRssSearchQuery& aQuery
							)
	{
	TInt retVal = KErrNotSupported;

	if( !IsActive() )
		{
	    TPckgBuf<TIptvRssSearchQuery> pckg( aQuery );

	    TIpcArgs args( &pckg );

	    retVal = iSession.SendRequest( EIptvEngineVodContentRssSearch, args );
		}
	else
		{
		retVal = KErrNotReady;
		}

	return retVal;
	}

// -----------------------------------------------------------------------------
// CIptvVodContentClient::CancelSearch
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodContentClient::CancelSearch()
	{
	TInt retVal = KErrNotSupported;

	if( !IsActive() )
		{
		TRequestStatus status;
		iSession.SendRequest( EIptvEngineVodContentRssSearchCancel, status );
		User::WaitForRequest( status );
		retVal = KErrNone;
		}

	return retVal;
	}

// -----------------------------------------------------------------------------
// Reset global Id.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodContentClient::ResetGlobalId(
    CIptvMyVideosGlobalFileId& aFileId,
    TUint32 aServiceId )
    {
    if ( !IsActive() )
        {
        TRAP_IGNORE( SendRequestL(
            EIptvEngineVodContentResetGlobalId,
            aFileId,
            aServiceId ) );
        }
    }

// -----------------------------------------------------------------------------
// Check if update needed 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvVodContentClient::UpdateAvailableL( TBool& aAvailable )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::UpdateAvailableL");

    TRequestStatus status( KErrNone );
    if (!IsActive())
        {
        SendUpdateCheckRequestL( status );
        if ( status.Int() != KErrNone )
            {
            CleanupL();
            }
        else
            {
            HandleVodContentUpdateCheckL( aAvailable );
            }
        }
    return status.Int();    
    }

// -----------------------------------------------------------------------------
// Cancel ongoing content update
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodContentClient::CancelUpdate()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::CancelUpdate()");
    Cancel();
    SendCancelUpdateRequest();
    }

// -----------------------------------------------------------------------------
// Send update check request
// -----------------------------------------------------------------------------
void CIptvVodContentClient::SendUpdateCheckRequestL( TRequestStatus& aStatus )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::SendUpdateCheckRequestL");

    delete iMsg;
    iMsg = NULL;

    iMsg = HBufC8::NewL( KIptvVodUpdateCheckRequestSize );
    iMsgPtr.Set(iMsg->Des());
        
    iSession.SendRequest( EIptvEngineVodContentCheckUpdate, iMsgPtr, aStatus );
    User::WaitForRequest(aStatus);
    }

// -----------------------------------------------------------------------------
// Handle content update check
// -----------------------------------------------------------------------------
//
void CIptvVodContentClient::HandleVodContentUpdateCheckL( TBool& aAvailable )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentClient::HandleVodContentUpdateCheckL");

    if ( iMsg )
        {
        RDesReadStream stream;
        stream.Open( iMsgPtr );
        CleanupClosePushL( stream );
        TInt32 updateValue = stream.ReadInt32L();
        TInt32 updateAvailable = (TBool) updateValue;
        aAvailable = updateAvailable;  
        CleanupStack::PopAndDestroy( &stream );
        }    

    delete iMsg;
    iMsg = NULL;
    }
