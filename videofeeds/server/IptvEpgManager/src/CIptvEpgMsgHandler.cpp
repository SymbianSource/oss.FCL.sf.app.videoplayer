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
* Description:    ?Description*
*/




// INCLUDE FILES
#include <s32mem.h>
#include "IptvDebug.h"
#include <bautils.h>
#include "CIptvMediaContent.h"
#include "TIptvRssSearchQuery.h"
#include "CIptvEpgDatabase.h"

#include "IptvLiveEpgConstants.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "IptvClientServerCommon.h"
#include "CIptvEpgMsgHandler.h"
#include "CIptvServer.h"
#include "CIptvEpgManagerImpl.h"
#include "CIptvEpgVodClientImpl.h"
#include "CIptvEpgSession.h"
#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvEpgVodMsqQueue.h"

// CONSTANTS
const TInt KIptvFour = 4;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::CIptvEpgMsgHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvEpgMsgHandler::CIptvEpgMsgHandler(CIptvServer& aServer) : 
    iVodMsgQueue (NULL ),
    iEpgSession ( NULL ), 
    iServer( aServer ), 
    iCategory( NULL ), 
    iServiceId( 0 ),
	iEpgDatabase( NULL )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::CIptvEpgMsgHandler");
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvEpgMsgHandler::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::ConstructL");

    iVodMsgQueue = CIptvEpgVodMsqQueue::NewL();
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvEpgMsgHandler* CIptvEpgMsgHandler::NewL(CIptvServer& aServer)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::NewL");

    CIptvEpgMsgHandler* self = new( ELeave ) CIptvEpgMsgHandler(aServer);   
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::~CIptvEpgMsgHandler()
// Destructor.
// -----------------------------------------------------------------------------
//   
CIptvEpgMsgHandler::~CIptvEpgMsgHandler()
    {        
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::~CIptvEpgMsgHandler");
        
    iCategoryBriefDetailsArray.ResetAndDestroy();
    iContentBriefDetailsArray.ResetAndDestroy();
    iMediaContentArray.ResetAndDestroy();        
    
    delete iContentFullDetails;
    delete iCategory;
    delete iVodMsgQueue;             
    delete iEpgSession;
    delete iEpgDatabase;
    }

// ------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleEpgMsg
// ------------------------------------------------------------------
//
void CIptvEpgMsgHandler::HandleEpgMsgL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleEpgMsgL");

    switch ( aMessage.Function() )
        {
        case EIptvEngineVodContentUpdateEcg:
            {
            TInt err( KErrNotReady );
            if ( iEpgSession )
                {
                err = iEpgSession->UpdateEcg( ETrue );
                }
            aMessage.Complete( err );                
            }
            break;
        case EIptvEngineVodContentCheckGroup:
            {
            TInt err( KErrNotReady );
            if ( iEpgSession )
                {
                err = iEpgSession->UpdateEcg( EFalse );
                }
            aMessage.Complete( err );                
            }
            break;
        case EIptvEngineVodContentCancel:
            {
            delete iEpgSession;
            iEpgSession = NULL;
            iVodMsgQueue->CancelRequest();
            aMessage.Complete(KErrNone);
            }
            break;
        case EIptvEngineVodContentCancelUpdate:
            {
            if ( iEpgSession )
                {
                iEpgSession->VodPluginCancelled();
                }
            iVodMsgQueue->CancelMsgQueue();
            aMessage.Complete(KErrNone);
            }
            break;

        case EIptvEngineVodContentRssSearch:
        	{
        	TInt err( KErrNotReady );
        	if( iEpgSession )
        		{
	            TPckgBuf<TIptvRssSearchQuery> pckg;
	            TRAP( err, aMessage.ReadL( 0, pckg ) );
	            if( err == KErrNone )
	            	{
	        		err = iEpgSession->Search( pckg() );
	            	}
        		}
    		aMessage.Complete( err );
        	}
        	break;
        case EIptvEngineVodContentRssSearchCancel:
        	{
        	TInt err( KErrNotReady );
        	if( iEpgSession )
        		{
        		err = iEpgSession->CancelSearch();
        		}
        	aMessage.Complete( err );
        	}
        	break;
        case EIptvEngineLiveTvUpdateEPGReq:
			{
			TInt err( KErrNotReady );
			if ( iEpgSession )
        		{
        		err = iEpgSession->UpdateEcg( ETrue );
        		}
        	aMessage.Complete( err );
			}
        	break;
		case EIptvEngineLiveTvUpdateChannelReq:
            {
            TRAPD( err, HandleUpdateChannelOrderReqL( aMessage ) );
			aMessage.Complete( err );	
			break;
            }
        case EIptvEngineVodContentObserver:
            iVodMsgQueue->SetRequest(aMessage);
            break;
        case EIptvEngineVodContentServiceId: // fall through
        case EIptvEngineLiveTvContentServiceId:
            HandleServiceIdL(aMessage);            
            break;
        case EIptvEngineVodContentGetCAListSizeReq:
            HandleGetCAListSizeReqL(aMessage);
            break;
        case EIptvEngineVodContentGetCAListDataReq:
            HandleGetCAListDataReqL(aMessage);
            break;                
        case EIptvEngineVodContentSetLastPosition:
            SetLastPositionL(aMessage);
            break;
        case EIptvEngineVodContentSetMpxId:
            HandleSetMpxIdL(aMessage);
            break;  
        case EIptvEngineVodContentGetEcgCategoryListSizeReq:            
            HandleGetEcgCategoryListSizeReqL(aMessage);
            break;
        case EIptvEngineVodContentGetEcgCategoryListDataReq:
            HandleGetEcgCategoryListDataReqL(aMessage);
            break;
        case EIptvEngineVodContentGetCategoryDetailsSizeReq:
            HandleGetCategoryDetailsSizeReqL(aMessage);
            break;
        case EIptvEngineVodContentGetCategoryDetailsDataReq:
            HandleGetCategoryDetailsDataReqL(aMessage);
            break;
        case EIptvEngineVodContentGetParentCategoryReq:
            HandleGetParentCategoryReqL(aMessage);
            break;
        case EIptvEngineVodContentGetEcgListSizeReq:             
            HandleGetEcgListSizeReqL(aMessage);
            break;
        case EIptvEngineVodContentGetEcgListDataReq:
            HandleGetEcgListDataReqL(aMessage);
            break;
        case EIptvEngineVodContentGetEcgAllListSizeReq:
            HandleGetEcgAllListSizeReqL(aMessage);
            break;
        case EIptvEngineVodContentGetEcgAllListDataReq:
            HandleGetEcgAllListDataReqL(aMessage);
            break;
        case EIptvEngineVodContentGetContentDetailsSizeReq:            
            HandleGetContentDetailsSizeReqL(aMessage);
            break;
        case EIptvEngineVodContentGetContentDetailsDataReq:
            HandleGetContentDetailsDataReqL(aMessage);
            break;
        case EIptvEngineVodGetUpdateTime:
            HandleGetUpdateTimeL(aMessage);
            break;
        case EIptvEngineLiveTvSetIap: // fall through            
        case EIptvEngineVodContentSetIap:
            HandleSetIapL(aMessage);
            break;
        case EIptvEngineVodContentResetGlobalId:
            HandleResetGlobalIdL( aMessage );
            break;
        case EIptvEngineVodContentCheckUpdate:
            HandleContentCheckUpdateL( aMessage );
            break;
        default:
            break;
        }
    }

// ------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleUpdateChannelOrderReqL
// ------------------------------------------------------------------
//
void CIptvEpgMsgHandler::HandleUpdateChannelOrderReqL( const RMessage2& aMessage ) 
	{
	IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleUpdateChannelOrderReqL");
	// aMessage contains service provider id, channel id 
	// and order number
	TUint32 serviceId( 0 );
	TInt64 channelId( 0 );
	TUint32 order( 0 );
	
	TBuf8<KIptvChannelOrderReqSize> data;        
    aMessage.ReadL( 0, data, 0 );   
    RDesReadStream stream;
	CleanupClosePushL( stream );
    stream.Open( data );       
    serviceId = stream.ReadUint32L();
	stream >> channelId;
	order = stream.ReadUint32L();
    CleanupStack::PopAndDestroy( &stream ); // closes stream
                
    if ( !iEpgDatabase ) 
		{
		// remove non-working code below?
		RFs fs;
		CleanupClosePushL( fs );
	    User::LeaveIfError( fs.Connect() );
	    TBuf<KIptvMaxPath> path;
	    CIptvUtil::GetPathL( fs, EIptvPathEcg, path );
	    CleanupStack::PopAndDestroy( &fs ); // closes fs

	    path.Append( KLiveTvEPGDatabaseName );
		iEpgDatabase = CIptvEpgDatabase::NewL( path );
		}
	iEpgDatabase->UpdateChannelOrderL( serviceId, channelId, order );	
	}

// ------------------------------------------------------------------
// CIptvEpgMsgHandler::SendMessageToClientL
// ------------------------------------------------------------------
//
void CIptvEpgMsgHandler::SendMessageToClientL(TInt aMsg, TInt aInfo)
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::SendMessageToClientL %d", aMsg);

    // Service id = 0 because we dont have to bring this information
    // over IPC. Service ID is already on the client side.    
    iVodMsgQueue->SendMessageToClientL(aMsg, aInfo);
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetEcgCategoryListSizeReqL()
// 
// -----------------------------------------------------------------------------
//   
void CIptvEpgMsgHandler::HandleGetEcgCategoryListSizeReqL(const RMessage2& aMessage)
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgCategoryListSizeReqL");

    TBuf8<KIptvSizeRequestSize> data;        
    aMessage.ReadL(0, data, 0);       
    RDesReadStream stream;
    stream.Open(data);  
    CleanupClosePushL( stream );     
    TIptvCategoryId parentCategoryId = stream.ReadUint32L();     
    CleanupStack::PopAndDestroy( &stream );
        
    // Get Epg stuff from DB
    TInt error = KErrNotReady;
    if (iEpgSession)
        {    
        error = iEpgSession->GetVodClientImplL()->GetEcgCategoryListL(parentCategoryId, iCategoryBriefDetailsArray);
        if (error != KErrNone)
            {
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgCategoryListSizeReqL error %d", error);            
            }       
        }
    else
        {                        
        IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgCategoryListSizeReqL Epg session is NULL");        
        }
   
    TInt count = iCategoryBriefDetailsArray.Count();
    IPTVLOGSTRING2_LOW_LEVEL("Item count = %d", count);
    
    // count in the beginning
    TUint32 dataSize = KIptvFour;
    for (TInt i = 0; i < count; i++)
        {                        
        dataSize += iCategoryBriefDetailsArray[i]->CountExternalizeSize();
        }

    IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::Category brief details array data size = %d", dataSize);
    iDataSize = dataSize;

    // Send data size to client
    data.FillZ();
    RDesWriteStream writeStream;
    writeStream.Open(data);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(dataSize);
    CleanupStack::PopAndDestroy( &writeStream );

	aMessage.WriteL(0, data, 0);
    aMessage.Complete(error);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetEcgCategoryListDataReqL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvEpgMsgHandler::HandleGetEcgCategoryListDataReqL(const RMessage2& aMessage)
    {   
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgCategoryListDataReqL");
          
    HBufC8* data = HBufC8::NewL(iDataSize);
    CleanupStack::PushL(data);
    TPtr8 dataPtr(data->Des());
    
    RDesWriteStream stream;
    stream.Open(dataPtr);
    CleanupClosePushL( stream );
    TUint32 count = iCategoryBriefDetailsArray.Count();
    stream.WriteUint32L(count);
    for (TInt i = 0; i < count; i++)
        {
        iCategoryBriefDetailsArray[i]->ExternalizeL(stream);        
        }
    CleanupStack::PopAndDestroy( &stream );
    aMessage.WriteL(0, dataPtr, 0);
    CleanupStack::PopAndDestroy(data);
    iCategoryBriefDetailsArray.ResetAndDestroy();
    aMessage.Complete(KErrNone);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetCategoryDetailsSizeReqL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvEpgMsgHandler::HandleGetCategoryDetailsSizeReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetCategoryDetailsSizeReqL");

    TBuf8<KIptvSizeRequestSize> data;        
    aMessage.ReadL(0, data, 0);       
    RDesReadStream stream;
    stream.Open(data); 
    CleanupClosePushL( stream );      
    TIptvCategoryId categoryId = stream.ReadUint32L();     
    CleanupStack::PopAndDestroy( &stream );
    
    // Get Epg stuff from DB        
    delete iCategory;
    iCategory = NULL;        
    iCategory = CIptvVodContentCategoryBriefDetails::NewL();
    
    TInt error = KErrNotReady;
    if (iEpgSession)
        {
        error = iEpgSession->GetVodClientImplL()->GetCategoryDetailsL(categoryId, *iCategory);
        if (error != KErrNone)
            {                
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetCategoryDetailsSizeReqL error %d", error);            
            }
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetCategoryDetailsSizeReqL Epg session is NULL");        
        }       

    TUint32 dataSize = 0;
    if (iCategory)
        {
        dataSize = iCategory->CountExternalizeSize();
        IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::Category full details data size = %d", dataSize);
        iDataSize = dataSize;
        }
    
    // Send data size to client
    data.FillZ();
    RDesWriteStream writeStream;
    writeStream.Open(data);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(dataSize);
    CleanupStack::PopAndDestroy( &writeStream );

	aMessage.WriteL(0, data, 0);    
    aMessage.Complete(error);                       
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetCategoryDetailsDataReqL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvEpgMsgHandler::HandleGetCategoryDetailsDataReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetCategoryDetailsDataReqL");
    
    HBufC8* data = HBufC8::NewL(iDataSize);
    CleanupStack::PushL(data);
    TPtr8 dataPtr(data->Des());
    
    RDesWriteStream stream;
    stream.Open(dataPtr);
    CleanupClosePushL( stream );
    iCategory->ExternalizeL(stream);
    CleanupStack::PopAndDestroy( &stream );   
        
    aMessage.WriteL(0, dataPtr, 0);
    CleanupStack::PopAndDestroy(data);
    delete iCategory;
    iCategory = NULL;      
    aMessage.Complete(KErrNone);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetParentCategoryReqL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvEpgMsgHandler::HandleGetParentCategoryReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetParentCategoryReqL");
    
    TBuf8<KIptvSizeRequestSize> data;        
    aMessage.ReadL(0, data, 0);
    RDesReadStream stream;
    stream.Open(data);       
    CleanupClosePushL( stream );
    TIptvCategoryId categoryId = stream.ReadUint32L();     
    CleanupStack::PopAndDestroy( &stream );
    IPTVLOGSTRING2_LOW_LEVEL("HandleGetParentCategoryReqL category = %d", categoryId);        

    // Get Epg stuff from DB    
    TIptvCategoryId parent = 0;
    TInt error = KErrNotReady;
    if (iEpgSession)
        {
        error = iEpgSession->GetVodClientImplL()->GetParentCategoryL(categoryId, parent);
        if (error != KErrNone)
            {                
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetParentCategoryReqL error %d", error);            
            }
        }
    
    // Send parent category to client
    data.FillZ();
    RDesWriteStream writeStream;
    writeStream.Open(data);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(parent);
    CleanupStack::PopAndDestroy( &writeStream );

	aMessage.WriteL(0, data, 0);
    aMessage.Complete(error);
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetEcgListSizeReqL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvEpgMsgHandler::HandleGetEcgListSizeReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgListSizeReqL");

    TInt dataSize = aMessage.GetDesLengthL(0);
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgListSizeReqL:: data size = %d", dataSize);

    HBufC8* data = HBufC8::NewL(dataSize);
    CleanupStack::PushL(data);
    TPtr8 dataPtr(data->Des());
    aMessage.ReadL(0, dataPtr, 0);
            
    RDesReadStream stream;
    stream.Open(dataPtr);    
    CleanupClosePushL( stream );
    TUint32 categoryKey = stream.ReadUint32L();    
    TUint32 from = stream.ReadUint32L();
    TUint32 amount = stream.ReadUint32L();
    TBuf<KIptvEpgContentNameMaxLength> search;
    CIptvUtil::ReadDesFromStreamL(search, stream);
    CleanupStack::PopAndDestroy( &stream );

    // Get Epg stuff from DB 
    TUint32 totalAmount = 0;
    TInt error = KErrNotReady;
    if (iEpgSession)
        {
        error = iEpgSession->GetVodClientImplL()->GetEcgListL(categoryKey, search, from, amount, totalAmount, iContentBriefDetailsArray);    
        if (error != KErrNone)
            {                
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgListSizeReqL error %d", error);
            }
        }
    
    iTotalAmount = totalAmount;
    TInt count = iContentBriefDetailsArray.Count();
    IPTVLOGSTRING2_LOW_LEVEL("Item count = %d", count);
    
    // count + totalAmount
    dataSize = KIptvFour + KIptvFour;
    for (TInt i = 0; i < count; i++)
        {                        
        dataSize += iContentBriefDetailsArray[i]->CountExternalizeSize();
        }

    IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::Content brief details array data size = %d", dataSize);
    iDataSize = dataSize;

    // Send data size to client
    dataPtr.FillZ();
    RDesWriteStream writeStream;
    writeStream.Open(dataPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(dataSize);
    CleanupStack::PopAndDestroy( &writeStream );

	aMessage.WriteL(0, dataPtr, 0);
    CleanupStack::PopAndDestroy(data);  
    aMessage.Complete(error);
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetEcgListSizeReqL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvEpgMsgHandler::HandleGetEcgListDataReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgListDataReqL");

    HBufC8* data = HBufC8::NewL(iDataSize);
    CleanupStack::PushL(data);
    TPtr8 dataPtr(data->Des());
    
    RDesWriteStream stream;
    stream.Open(dataPtr);
    CleanupClosePushL( stream );
    TUint32 count = iContentBriefDetailsArray.Count();
    stream.WriteUint32L(count);
    stream.WriteUint32L(iTotalAmount);
    for(TInt i = 0; i < count; i++)
        {
        iContentBriefDetailsArray[i]->ExternalizeL(stream);
        }
    CleanupStack::PopAndDestroy( &stream );
    aMessage.WriteL(0, dataPtr, 0);
    iTotalAmount = 0;
    CleanupStack::PopAndDestroy(data);
    iContentBriefDetailsArray.ResetAndDestroy();    
    aMessage.Complete(KErrNone);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetEcgAllListSizeReqL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvEpgMsgHandler::HandleGetEcgAllListSizeReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgAllListSizeReqL");

    TInt dataSize = aMessage.GetDesLengthL(0);
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgAllListSizeReqL:: data size = %d", dataSize);

    HBufC8* data = HBufC8::NewL(dataSize);
    CleanupStack::PushL(data);
    TPtr8 dataPtr(data->Des());
    aMessage.ReadL(0, dataPtr, 0);
            
    RDesReadStream stream;
    stream.Open(dataPtr);        
    CleanupClosePushL( stream );
    TUint32 from = stream.ReadUint32L();
    TUint32 amount = stream.ReadUint32L();
    TBuf<KIptvEpgContentNameMaxLength> search;
    CIptvUtil::ReadDesFromStreamL(search, stream);    
    CleanupStack::PopAndDestroy( &stream );
    
    // Get Epg stuff from DB 
    TUint32 totalAmount = 0;
    TInt error = KErrNotReady;
    if (iEpgSession)
        {
        error = iEpgSession->GetVodClientImplL()->GetEcgAllListL(search, from, amount, totalAmount, iContentBriefDetailsArray);
        if (error != KErrNone)
            {                
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgAllListSizeReqL error %d", error);
            }
        }
    
    iTotalAmount = totalAmount;
    TInt count = iContentBriefDetailsArray.Count();
    IPTVLOGSTRING2_LOW_LEVEL("Item count = %d", count);
        
    // count + totalAmount
    dataSize = KIptvFour + KIptvFour;
    for (TInt i = 0; i < count; i++)
        {                        
        dataSize += iContentBriefDetailsArray[i]->CountExternalizeSize();
        }

    IPTVLOGSTRING2_LOW_LEVEL("Content brief details array data size = %d", dataSize);
    iDataSize = dataSize;

    // Send data size to client
    dataPtr.FillZ();
    RDesWriteStream writeStream;
    writeStream.Open(dataPtr);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(dataSize);
    CleanupStack::PopAndDestroy( &writeStream );

	aMessage.WriteL(0, dataPtr, 0);
    CleanupStack::PopAndDestroy(data);  
    aMessage.Complete(error);
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleEcgAllListDataReqL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvEpgMsgHandler::HandleGetEcgAllListDataReqL(const RMessage2& aMessage)
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetEcgAllListDataReqL");

    HBufC8* data = HBufC8::NewL(iDataSize);
    CleanupStack::PushL(data);
    TPtr8 dataPtr(data->Des());
    
    RDesWriteStream stream;
    stream.Open(dataPtr);
    CleanupClosePushL( stream );
    TUint32 count = iContentBriefDetailsArray.Count();
    stream.WriteUint32L(count);
    stream.WriteUint32L(iTotalAmount);
    for(TInt i = 0; i < count; i++)
        {
        iContentBriefDetailsArray[i]->ExternalizeL(stream);
        }
    CleanupStack::PopAndDestroy( &stream );
    aMessage.WriteL(0, dataPtr, 0);
    iTotalAmount = 0;
    CleanupStack::PopAndDestroy(data);
    iContentBriefDetailsArray.ResetAndDestroy();    
    aMessage.Complete(KErrNone);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetContentDetailsSizeReqL()
// 
// -----------------------------------------------------------------------------
//    
void CIptvEpgMsgHandler::HandleGetContentDetailsSizeReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetContentDetailsSizeReqL");

    TBuf8<KIptvSizeRequestSize> data;        
    aMessage.ReadL(0, data, 0);       
    RDesReadStream stream;
    stream.Open(data);       
    CleanupClosePushL( stream );
    TIptvContentId contentId = stream.ReadUint32L();     
    CleanupStack::PopAndDestroy( &stream );
    
    // Get Epg stuff from DB        
    delete iContentFullDetails;
    iContentFullDetails = NULL;        
    iContentFullDetails = CIptvVodContentContentFullDetails::NewL();
        
    TInt error = KErrNotReady;
    if (iEpgSession)
        {
        error = iEpgSession->GetVodClientImplL()->GetContentDetailsL(contentId, *iContentFullDetails);
        if (error != KErrNone)
            {                    
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetContentDetailsSizeReqL error %d", error);
            }
        }
    
    TUint32 dataSize = 0;
    if (iContentFullDetails)
        {
        dataSize = iContentFullDetails->CountExternalizeSize();
        IPTVLOGSTRING2_LOW_LEVEL("Content full details data size = %d", dataSize);
        iDataSize = dataSize;
        }
    
    // Send data size to client
    data.FillZ();
    RDesWriteStream writeStream;
    writeStream.Open(data);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(dataSize);
    CleanupStack::PopAndDestroy( &writeStream );

	aMessage.WriteL(0, data, 0);   
    aMessage.Complete(error);                       
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetContentDetailsDataReqL()
// 
// -----------------------------------------------------------------------------
//
void CIptvEpgMsgHandler::HandleGetContentDetailsDataReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetContentDetailsDataReqL");
    
    HBufC8* data = HBufC8::NewL(iDataSize);
    CleanupStack::PushL(data);
    TPtr8 dataPtr(data->Des());
    
    RDesWriteStream stream;
    stream.Open(dataPtr);
    CleanupClosePushL( stream );
    iContentFullDetails->ExternalizeL(stream);
    CleanupStack::PopAndDestroy( &stream );   
        
    aMessage.WriteL(0, dataPtr, 0);
    CleanupStack::PopAndDestroy(data);
    delete iContentFullDetails;
    iContentFullDetails = NULL;      
    aMessage.Complete(KErrNone);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleServiceId
// Get service ID from client and get CIptvEpgManagerImpl pointer from server.
// -----------------------------------------------------------------------------
//   
void CIptvEpgMsgHandler::HandleServiceIdL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleServiceIdL");

    TBuf8<KIptvSizeRequestSize> data;
    aMessage.ReadL( 0, data, 0);
    RDesReadStream stream;
    CleanupClosePushL( stream );
    stream.Open( data );       
    iServiceId = stream.ReadUint32L();
    IPTVLOGSTRING2_LOW_LEVEL("Service ID = %d", iServiceId);
    CleanupStack::PopAndDestroy( &stream );
                
    TInt error( KErrNone );
    CIptvEpgManagerImpl& epgManager = iServer.GetEpgManager();
    delete iEpgSession;
    iEpgSession = NULL;
    iEpgSession = epgManager.CreateSessionL( iServiceId, iVodMsgQueue, error );
    if ( iIapId > 0 && iEpgSession )
        {
        // IAP id has been set previously, Set it to the new EPG session also
        iEpgSession->SetIapL( iIapId );    
        }
    aMessage.Complete( error );    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetUpdateTimeL
// 
// -----------------------------------------------------------------------------
//   
void CIptvEpgMsgHandler::HandleGetUpdateTimeL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetUpdateTimeL");

    TTime updateTime;
    TInt error( KErrNotReady );
    if (iEpgSession)
        {
        error = iEpgSession->GetVodClientImplL()->GetUpdateTimeL(updateTime);
        if (error != KErrNone)
            {                    
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetUpdateTimeL error %d", error);
            }
        }
    
    TInt64 time = updateTime.Int64();    
    TUint32 lower = (TUint32) (0x00000000FFFFFFFF) & time;
    TUint32 upper = (TUint32) (0x00000000FFFFFFFF) & (time >> 32);

    HBufC8* data = HBufC8::NewL(KIptvVodGetUpdateTimeRequestSize);
    CleanupStack::PushL(data);
    TPtr8 dataPtr(data->Des());
    
    RDesWriteStream stream;
    stream.Open(dataPtr);    
    CleanupClosePushL( stream );
    stream.WriteUint32L(lower);
    stream.WriteUint32L(upper);    
    CleanupStack::PopAndDestroy( &stream );
    aMessage.WriteL(0, dataPtr, 0);    
    CleanupStack::PopAndDestroy(data);    
    aMessage.Complete(error);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleSetIapL
// 
// -----------------------------------------------------------------------------
//   
void CIptvEpgMsgHandler::HandleSetIapL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleSetIapL");
    if ( !iEpgSession )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgMsgHandler::HandleSetIapL: iEpgSession has not been created, complete aMessage with KErrNotReady" );
        aMessage.Complete( KErrNotReady );
        return;
        }

    TBuf8<KIptvSizeRequestSize> data;
    aMessage.ReadL(0, data, 0);       
    RDesReadStream stream;
    stream.Open(data);       
	CleanupClosePushL( stream );    
    iIapId = stream.ReadUint32L();    
	CleanupStack::PopAndDestroy( &stream ); // closes stream	
    iEpgSession->SetIapL( iIapId );
                    
    aMessage.Complete( KErrNone );  
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetCAListSizeReqL
// 
// -----------------------------------------------------------------------------
//   
void CIptvEpgMsgHandler::HandleGetCAListSizeReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetCAListSizeReqL");

    TBuf8<KIptvSizeRequestSize> data;        
    aMessage.ReadL(0, data, 0);       
    RDesReadStream stream;
    stream.Open(data);       
    CleanupClosePushL( stream );
    TUint32 contentId = stream.ReadUint32L();     
    CleanupStack::PopAndDestroy( &stream );
        
    // Get Epg stuff from DB
    TInt error = KErrNotReady;
    if (iEpgSession)
        {            
        error = iEpgSession->GetVodClientImplL()->GetContentAccessListL(contentId, iMediaContentArray);
        if (error != KErrNone)
            {
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetCAListSizeReqL error %d", error);            
            }       
        }
    else
        {                        
        IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetCAListSizeReqL Epg session is NULL");        
        }
   
    TInt count = iMediaContentArray.Count();
        
    // count in the beginning
    TUint32 dataSize = KIptvFour;
    for (TInt i = 0; i < count; i++)
        {                        
        dataSize += iMediaContentArray[i]->CountExternalizeSize();
        }

    IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgMsgHandler::Media content array data size = %d", dataSize);
    iDataSize = dataSize;

    // Send data size to client
    data.FillZ();
    RDesWriteStream writeStream;
    writeStream.Open(data);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(dataSize);
    CleanupStack::PopAndDestroy( &writeStream );

	aMessage.WriteL(0, data, 0);
    aMessage.Complete(error);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleGetCAListDataReqL
// 
// -----------------------------------------------------------------------------
//   
void CIptvEpgMsgHandler::HandleGetCAListDataReqL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleGetCAListDataReqL");

    HBufC8* data = HBufC8::NewL(iDataSize);
    CleanupStack::PushL(data);
    TPtr8 dataPtr(data->Des());
    
    RDesWriteStream stream;
    stream.Open(dataPtr);
    CleanupClosePushL( stream );
    TUint32 count = iMediaContentArray.Count();
    stream.WriteUint32L(count);    
    for (TInt i = 0; i < count; i++)
        {
        iMediaContentArray[i]->ExternalizeL(stream);
        }
    CleanupStack::PopAndDestroy( &stream );
    aMessage.WriteL(0, dataPtr, 0);    
    CleanupStack::PopAndDestroy(data);
    iMediaContentArray.ResetAndDestroy();    
    aMessage.Complete(KErrNone);    
    }

// ---------------------------------------------------------------------------
// Handle Reset Global Id request.
// ---------------------------------------------------------------------------
//
void CIptvEpgMsgHandler::HandleResetGlobalIdL(
    const RMessage2& aMessage )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "My Videos Mgr ## CIptvEpgMsgHandler::HandleResetGlobalIdL -- Enter" );

    // aMessage includes: CIptvMyVideosGlobalFileId, TUint32

    TInt dataSize = aMessage.GetDesLengthL( 0 );
    HBufC8* data = HBufC8::NewLC( dataSize );
    TPtr8 dataPtr( data->Des() );
    RDesReadStream stream;
    CIptvMyVideosGlobalFileId* fileId = CIptvMyVideosGlobalFileId::NewLC();
    TUint32 serviceId( 0 );

    aMessage.ReadL( 0, dataPtr, 0 );
    stream.Open( dataPtr );
    CleanupClosePushL( stream );
    fileId->InternalizeL( stream );
    serviceId = stream.ReadUint32L();
    CleanupStack::PopAndDestroy( &stream );

    // CreateSessionL leaves if service does not exist.
    CIptvEpgManagerImpl& epgManager = iServer.GetEpgManager();
    TInt temp = KErrNone;
    CIptvEpgSession* epgSession = epgManager.CreateSessionL(
        serviceId,
        NULL,
        temp,
        EFalse );

    CleanupStack::PushL( epgSession );
    epgSession->ResetGlobalId( *fileId );
    CleanupStack::PopAndDestroy( epgSession );

    CleanupStack::PopAndDestroy( fileId );
    CleanupStack::PopAndDestroy( data );
    aMessage.Complete( KErrNone );

    IPTVLOGSTRING_LOW_LEVEL(
        "My Videos Mgr ## CIptvEpgMsgHandler::HandleEnsureFreeSpaceReqL -- Exit" );
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::SetLastPositionL
// 
// -----------------------------------------------------------------------------
//   
void CIptvEpgMsgHandler::SetLastPositionL(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::SetLastPositionL");

    TBuf8<KIptvVodSendLastPositionSize> data;        
    aMessage.ReadL(0, data, 0);       
    RDesReadStream stream;
    stream.Open(data);       
    CleanupClosePushL( stream );
    TUint32 id = stream.ReadUint32L();
    TUint32 index = stream.ReadUint32L();
    TUint32 position = stream.ReadUint32L();
    CleanupStack::PopAndDestroy( &stream );
    TInt error( KErrNotReady );
    if ( iEpgSession )
        {
        error = iEpgSession->GetVodClientImplL()->SetLastPositionL(id, index, position);    
        }
    aMessage.Complete(error);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleSetMpxIdL
// 
// -----------------------------------------------------------------------------
//   
void CIptvEpgMsgHandler::HandleSetMpxIdL( const RMessage2& aMessage )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleSetMpxIdL");

    TBuf8<KIptvVodSendMpxIdSize> data;        
    aMessage.ReadL( 0, data, 0 );       
    RDesReadStream stream;
    stream.Open( data );       
    CleanupClosePushL( stream );
    
    TUint32 contentId = stream.ReadUint32L();
    TUint32 caIndex = stream.ReadUint32L();
    TUint32 mpxId = stream.ReadUint32L();
    CleanupStack::PopAndDestroy( &stream );
    TInt error( KErrNotReady );
    if ( iEpgSession )
        {
        error = iEpgSession->GetVodClientImplL()->SetMpxIdL( contentId, caIndex, mpxId );    
        }
    aMessage.Complete(error);    
    }

// -----------------------------------------------------------------------------
// CIptvEpgMsgHandler::HandleContentCheckUpdateL
// 
// -----------------------------------------------------------------------------
//
void CIptvEpgMsgHandler::HandleContentCheckUpdateL( const RMessage2& aMessage )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgMsgHandler::HandleContentCheckUpdateL");
    
    TInt error( KErrNotReady );
    TBool updateAvailable ( EFalse );
    
    if ( iEpgSession )
        {
        updateAvailable = iEpgSession->GetVodClientImplL()->IsVodUpdateNeededL();
        error = KErrNone;
        }

    HBufC8* data = HBufC8::NewL( KIptvVodGetUpdateTimeRequestSize );
    CleanupStack::PushL( data );
    TPtr8 dataPtr( data->Des() );
    
    RDesWriteStream stream;
    stream.Open( dataPtr );    
    CleanupClosePushL( stream );
    stream.WriteInt32L( updateAvailable );  
    CleanupStack::PopAndDestroy( &stream );
    aMessage.WriteL( 0, dataPtr, 0 );    
    CleanupStack::PopAndDestroy(data);    
    aMessage.Complete(error);    
    }
