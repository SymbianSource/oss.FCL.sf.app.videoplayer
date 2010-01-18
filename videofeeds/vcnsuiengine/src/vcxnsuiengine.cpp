/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#include "IptvDebug.h"
#include <e32svr.h>
#include <coemain.h>
#include <eikappui.h>
#include <ipvideo/vcxconnectionutility.h>
#include <videoplayercustommessage.h>
#include <MediatorEventProvider.h>

#include "vcxnsuiengine.h"
#include "iptvlastwatchedapi.h"
#include "vcxnsmediatorids.h"
#include "vcxnsservice.h"
#include "vcxnsserviceprovider.h"
#include "vcxnsserviceproviderif.h"
#include "vcxnscontent.h"
#include "vcxnscontentaccess.h"
#include "vcxnscontentprovider.h"
#include "vcxnscontentproviderif.h"
#include "vcxnscategory.h"
#include "vcxnscategoryprovider.h"
#include "vcxnscategoryproviderif.h"
#include "vcxnsserviceclienthandler.h"
#include "vcxnscontentclienthandler.h"
#include "vcxnsmpxcollectionclienthandler.h"
#include "vcxnseventrouter.h"
#include "vcxnscontentsearchif.h"
#include "vcxnscontentsearchhandler.h"
#include "vcxnsupdateprogressif.h"
#include "vcxnsupdateprogresshandler.h"
#include "vcxnsstreaminghandler.h"

const TInt KVcxNsUiEngineInstance = 0x2000B43D;

// Version : %version: tr1ido#REL6_54 %

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::CVcxNsUiEngine()
// -----------------------------------------------------------------------------
//
CVcxNsUiEngine::CVcxNsUiEngine() :
    iApplicationState( EStateNoState ),
    iActiveServiceGroup( KMaxTUint )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::~CVcxNsUiEngine()
// -----------------------------------------------------------------------------
//
CVcxNsUiEngine::~CVcxNsUiEngine()
    {
    iAppStateStack.Close();
    
    delete iStreamingHandler;
    delete iServiceProvider;
    delete iContentProvider;
    delete iCategoryProvider;

    delete iServiceClientHandler;
    delete iContentClientHandler;
    delete iMpxCollectionClientHandler;
    delete iContentSearchHandler;

    if ( iConnUtil )
        {
        iConnUtil->DecreaseReferenceCount();
        }
    
    delete iEventRouter;
    delete iProgressHandler;
    delete iLastWatchedApi;
    delete iMediatorEventProvider;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxNsUiEngine::ConstructL()
    {
    iEventRouter = CVcxNsEventRouter::NewL( *this );
    iLastWatchedApi = CIptvLastWatchedApi::NewL();
    iMediatorEventProvider = CMediatorEventProvider::NewL();
    
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsUiEngine::ConstructL()");
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::InstanceL()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsUiEngine* CVcxNsUiEngine::InstanceL()
    {
    CVcxNsUiEngine* self = NULL;
    TAny* instance = UserSvr::DllTls( KVcxNsUiEngineInstance );

    if ( !instance )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CVcxNsUiEngine::InstanceL(): Creating new instance of singleton." );
        self = new( ELeave ) CVcxNsUiEngine;
        CleanupStack::PushL( self );
        self->ConstructL();
        User::LeaveIfError( UserSvr::DllSetTls( KVcxNsUiEngineInstance, self ) );
        CleanupStack::Pop( self );
        IPTVLOGSTRING2_LOW_LEVEL( "CVcxNsUiEngine::InstanceL(): Instance created (0x%x).", (TUint32)self );
        }
    else
        {
        self = reinterpret_cast<CVcxNsUiEngine*>(instance );
        IPTVLOGSTRING2_LOW_LEVEL( "CVcxNsUiEngine::InstanceL(): returning old instance (0x%x).", (TUint32)self );
        }

    self->IncreaseReferenceCount();

    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::IncreaseReferenceCount()
// -----------------------------------------------------------------------------
//
void CVcxNsUiEngine::IncreaseReferenceCount()
    {
    iReferenceCount++;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::DecreaseReferenceCount()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsUiEngine::DecreaseReferenceCount()
    {
    iReferenceCount--;

    if ( iReferenceCount == 0 )
        {
        DeleteInstance( this );
        }
    }

// ---------------------------------------------------------------------------
// CVcxNsUiEngine::DeleteInstance()
// ---------------------------------------------------------------------------
//
void CVcxNsUiEngine::DeleteInstance( CVcxNsUiEngine* aThis )
    {
    TAny* singleton = UserSvr::DllTls( KVcxNsUiEngineInstance );

    if ( singleton != aThis )
        {
        IPTVLOGSTRING3_LOW_LEVEL( "CVcxNsUiEngine::DeleteInstance(): ERROR: POINTER STORED IN THREAD LOCAL STORAGE DOES NOT MATCH WITH THE ONE FOR DELETION! (TLS = 0x%x != instance = 0x%x)", singleton, aThis );
        _LIT( KPanic, "NSUIENGINE" );
        User::Panic( KPanic, 0);
        }

    UserSvr::DllFreeTls( KVcxNsUiEngineInstance );

    delete aThis;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetServiceProviderL()
// -----------------------------------------------------------------------------
//
EXPORT_C MVcxNsServiceProviderIf* CVcxNsUiEngine::GetServiceProviderL()
    {
    IPTVLOGSTRING_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetServiceProviderL() -- Enter");

    if ( !iServiceProvider )
        {
        iServiceProvider = CVcxNsServiceProvider::NewL( *this );
        }

    IPTVLOGSTRING2_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetServiceProviderL() -- Returning: %d", (TInt) iServiceProvider);

    return iServiceProvider;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetContentProviderL()
// -----------------------------------------------------------------------------
//
EXPORT_C MVcxNsContentProviderIf* CVcxNsUiEngine::GetContentProviderL()
    {
    IPTVLOGSTRING_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetContentProviderL() -- Enter");

    if ( !iContentProvider )
        {
        iContentProvider = CVcxNsContentProvider::NewL( *this );
        }

    IPTVLOGSTRING2_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetContentProviderL() -- Returning: %d", (TInt) iContentProvider);

    return iContentProvider;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetCategoryProviderL()
// -----------------------------------------------------------------------------
//
EXPORT_C MVcxNsCategoryProviderIf* CVcxNsUiEngine::GetCategoryProviderL()
    {
    IPTVLOGSTRING_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetCategoryProviderL() -- Enter");

    if ( !iCategoryProvider )
        {
        iCategoryProvider = CVcxNsCategoryProvider::NewL( *this );
        }

    IPTVLOGSTRING2_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetCategoryProviderL() -- Returning: %d", (TInt) iCategoryProvider);

    return iCategoryProvider;
    }
// -----------------------------------------------------------------------------
// CVcxNsUiEngine::ContentSearchHandlerL()
// -----------------------------------------------------------------------------
//
EXPORT_C MVcxNsContentSearchIf* CVcxNsUiEngine::ContentSearchHandlerL()
    {
    IPTVLOGSTRING_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::ContentSearchIfL() -- Enter");

    if ( !iContentSearchHandler )
        {
        iContentSearchHandler = CVcxNsContentSearchHandler::NewL( *this );
        }

    IPTVLOGSTRING2_LOW_LEVEL(
            "UI Engine ## CVcxNsUiEngine::ContentSearchIfL() -- Returning: %d", (TInt) iContentSearchHandler );

    return iContentSearchHandler;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C MVcxNsUpdateProgressIf* CVcxNsUiEngine::ProgressHandlerL()
    {
    IPTVLOGSTRING_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::ProgressHandlerL() -- Enter");

    if ( !iProgressHandler )
        {
        iProgressHandler = CVcxNsUpdateProgressHandler::NewL( *this );
        }

    IPTVLOGSTRING2_LOW_LEVEL(
            "UI Engine ## CVcxNsUiEngine::ProgressHandlerL(): Returning: %d", 
            (TInt) iProgressHandler );

    return iProgressHandler;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::VcAppState()
// -----------------------------------------------------------------------------
//
EXPORT_C TVcxNsAppState CVcxNsUiEngine::VcAppState()
    {
#ifdef _DEBUG
    switch ( iApplicationState )
        {
        case EStateServiceView:
            IPTVLOGSTRING_LOW_LEVEL(
                    "CVcxNsUiEngine::VcAppState state = EStateServiceView");
            break;
        case EStateCategoryView:
            IPTVLOGSTRING_LOW_LEVEL(
                    "CVcxNsUiEngine::VcAppState state = EStateCategoryView" );
            break;
        case EStateContentView:
            IPTVLOGSTRING_LOW_LEVEL(
                    "CVcxNsUiEngine::VcAppState state = EStateContentView" );
            break;
        case EStateBrowser:
            IPTVLOGSTRING_LOW_LEVEL(
                    "CVcxNsUiEngine::VcAppState state = EStateBrowser" );
            break;
        default:
            IPTVLOGSTRING2_LOW_LEVEL( "CVcxNsUiEngine::VcAppState state = %d", iApplicationState );
            break;
        }
#endif
    return iApplicationState;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::SetVcAppState()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsUiEngine::SetVcAppState( TVcxNsAppState aAppState )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsUiEngine::SetVcAppState state = %d", aAppState);
    
    if ( iApplicationState != aAppState )
        {
        iAppStateStack.Append( aAppState );
        iApplicationState = aAppState;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::HandleBack()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsUiEngine::HandleBack()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsUiEngine::HandleBack");

    TVcxNsAppState lastState ( EStateNoState );
    TInt count = iAppStateStack.Count();
    
    if ( count > 0 )
        {
        lastState = iAppStateStack[count-1];
        }
    
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsUiEngine::HandleBack: current state = %d", lastState );
    
    PopAppStateStack();
    
    count = iAppStateStack.Count();

    if ( count > 0 )
        {
        TVcxNsAppState newState = iAppStateStack[ count-1 ];
        iApplicationState = newState;
        
        if ( iApplicationState == EStateCustomView 
             && ( lastState == EStateCategoryView || lastState == EStateContentView ) )
            {
            SetActiveService( ActiveServiceGroup() );
            SetActiveServiceGroup( KMaxTUint );
            }
        
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsUiEngine::HandleBack: new state = %d", newState );
        return ETrue;
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsUiEngine::HandleBack: state stack is empty" );
        iApplicationState = EStateNoState;
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::ResetStates()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsUiEngine::ResetStates()
    {
    iAppStateStack.Reset();
    iApplicationState = EStateNoState;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::ActiveCategory()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsUiEngine::ActiveCategory()
    {
    return iActiveCategory;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetAvailableIapL()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsUiEngine::GetAvailableIapL()
    {
    TUint32 iap( 0 );

    if ( ! iConnUtil )
        {
        iConnUtil = CVcxConnectionUtility::InstanceL();
        }

    if ( iConnUtil )
        {
        iConnUtil->GetIap( iap, EFalse );
        }

    return iap;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetAvailableWapIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsUiEngine::GetAvailableWapIdL()
    {
    TUint32 wap( 0 );
    TUint32 iap = GetAvailableIapL();
    if ( iap != 0 && iConnUtil )
        {
        wap = iConnUtil->WapIdFromIapIdL( iap );
        }
    
    return wap;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::PlayVideoL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsUiEngine::PlayVideoL( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsUiEngine::PlayVideoL");
    
    if( aContent )
         {
         TUint32 typeFlags = aContent->GetContentTypes();
         
         if( ( typeFlags & CVcxNsContent::EVcxContentTypeVideo )  &&
                aContent->GetMpxId() )
             {
             GetMpxCollectionClientHandlerL()->PlayVideoL( aContent->GetMpxId() );
             }
         }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsUiEngine::PlayVideoL");
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::PlayStreamL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsUiEngine::PlayStreamL( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsUiEngine::PlayStreamL");
    
    if( aContent )
        {
        TUint32 typeFlags = aContent->GetContentTypes();
         
        if( typeFlags & CVcxNsContent::EVcxContentTypeStream ) 
            {
            CEikAppUi* appui = (CEikAppUi*) CCoeEnv::Static()->AppUi();
            
            User::LeaveIfNull( appui );
            
            appui->OpenFileL( aContent->GetUrl( CVcxNsContent::EVcxContentTypeStream ) );

		    if ( !iStreamingHandler )
		       {
		       iStreamingHandler = CVcxNsStreamingHandler::NewL( *this );
		       }
    
		    iStreamingHandler->HandleOpenStreamL( aContent );
		    }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsUiEngine::PlayStreamL");
    }
// -----------------------------------------------------------------------------
// CVcxNsUiEngine::PlayLastWatchedL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsUiEngine::PlayLastWatchedL()
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsUiEngine::PlayLastWatchedL");
    
    CVcxNsContent* content = LastWatchedToContentL();
    
    if ( !content )
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PushL( content );
    
    TUint32 typeFlags = content->GetContentTypes();
     
    if( typeFlags & CVcxNsContent::EVcxContentTypeVideo ) 
        {
        GetMpxCollectionClientHandlerL()->GetMediaDetailsSyncL( content->GetMpxId() );
        
        PlayVideoL( content );
        }
    else if( typeFlags & CVcxNsContent::EVcxContentTypeStream ) 
        {
        PlayStreamL( content );
        }

    CleanupStack::PopAndDestroy( content );
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsUiEngine::PlayLastWatchedL");
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsUiEngine::VideoPlaybackStartingL( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsUiEngine::VideoPlaybackStartingL");
    
    WriteLastWatchedL( aContent );
    RaisePlayEventL( aContent );

    TUint32 mpxId = aContent->GetMpxId( CVcxNsContent::EVcxContentTypeVideo );
    
    if( mpxId )
        {
        // This also loads the played object media data to cache while playing.
        // Sometimes MDS has the item locked and the new video flag setting fails
        // We don't want to prevent playback starting in that case -> trap ignore.
        TRAP_IGNORE( GetMpxCollectionClientHandlerL()->MarkVideoAsWatchedL( mpxId ));
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsUiEngine::VideoPlaybackStartingL");
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsUiEngine::RaisePlayEventL( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsUiEngine::RaisePlayEventL");
    
    TBool isLocal ( aContent->GetContentTypes() & CVcxNsContent::EVcxContentTypeVideo ); 
    
    TVideoPlayerCustomMessage msg;

    if ( isLocal )
        {
        msg.iContent = aContent->GetLocalFilepath( CVcxNsContent::EVcxContentTypeVideo );
        }
    else 
        {
        msg.iContent = aContent->GetUrl( CVcxNsContent::EVcxContentTypeStream );
        }
    
    msg.iIcon = aContent->GetIconPath();
    msg.iName = aContent->GetName();
    
    TPckg<TVideoPlayerCustomMessage> videoInfoPckg( msg );
    
    iMediatorEventProvider->RaiseEvent( KVcxNsVideoCenterMediatorDomain,
                                        KVcxNsPlayerGeneralCategory,
                                        KVcxNsMediatorEventPlayerLastWatch,
                                        TVersion( KVcxNsMediatorPlayerVersion, 0, 0 ),
                                        videoInfoPckg );
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsUiEngine::RaisePlayEventL");
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsUiEngine::WriteLastWatchedL( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsUiEngine::WriteLastWatchedL");
    
    CIptvLastWatchedData* lwData = CIptvLastWatchedData::NewL();
    CleanupStack::PushL( lwData );

    TUint32 mpxId = aContent->GetMpxId( CVcxNsContent::EVcxContentTypeVideo );
    TBool isLocal ( aContent->GetContentTypes() & CVcxNsContent::EVcxContentTypeVideo ); 
    
    if( mpxId && isLocal )
        {
        lwData->SetMpxId( mpxId );
        }
    
    if ( isLocal )
        {
        lwData->SetUriL( aContent->GetLocalFilepath( CVcxNsContent::EVcxContentTypeVideo ) );
        }
    else 
        {
        lwData->SetUriL( aContent->GetUrl( CVcxNsContent::EVcxContentTypeStream ) );
        }
    
    lwData->SetIconPathL( aContent->GetIconPath() );
    lwData->SetLaunchType( CIptvLastWatchedData::EUndefined ); 
    lwData->SetServiceId( aContent->GetServiceId() );
    lwData->SetContentId( aContent->GetContentId() );
    lwData->SetNameL( aContent->GetName() );
    lwData->SetParentalControl( aContent->GetAgeProfile() );
    
    iLastWatchedApi->SetLastWatchedDataL( *lwData );
    CleanupStack::PopAndDestroy( lwData );
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsUiEngine::WriteLastWatchedL");
    }

// -----------------------------------------------------------------------------
// VcxNsUiEngine::LastWatchedToContentL
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsUiEngine::LastWatchedToContentL()
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsUiEngine::LastWatchedToContentL");
    
    CIptvLastWatchedData* lwData = CIptvLastWatchedData::NewL();
    CleanupStack::PushL( lwData );
    iLastWatchedApi->GetLastWatchedDataL( *lwData );
    
    CVcxNsContent* content = NULL;
    
    if ( lwData->MpxId() != KIdUndefined )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsUiEngine::LastWatchedToContentL lw = video");
        
        content = CVcxNsContent::NewL();
        CleanupStack::PushL( content );
        
        CVcxNsContentAccess* ca =  CVcxNsContentAccess::NewL();
        ca->iType = CVcxNsContent::EVcxContentTypeVideo;
        
        content->AddContentAccess( ca );
        content->SetMpxId( lwData->MpxId(), CVcxNsContent::EVcxContentTypeVideo );
        
        CleanupStack::Pop( content );
        }
    else if ( lwData->Uri().Length() > 0 )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsUiEngine::LastWatchedToContentL lw = stream");
        
        GetContentProviderL();
        
        CVcxNsContent* contentInCache = ContentProvider()->GetContentL( lwData->ServiceId(),
                                                                        lwData->ContentId() );
        if ( contentInCache )
            {
            content = CVcxNsContent::NewL( *contentInCache );
            }
        else
            {
            content = CVcxNsContent::NewL();
            CleanupStack::PushL( content );
            CVcxNsContentAccess* ca = CVcxNsContentAccess::NewL();
            CleanupStack::PushL( ca );
            ca->SetUrlL( lwData->Uri() );
            ca->iType = CVcxNsContent::EVcxContentTypeStream;
            CleanupStack::Pop( ca );
            content->AddContentAccess( ca );
            CleanupStack::Pop( content );
            }
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsUiEngine::LastWatchedToContentL no last watched ");
        }
    
    CleanupStack::PopAndDestroy( lwData );
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsUiEngine::LastWatchedToContentL");
    
    return content;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxNsUiEngine::PopAppStateStack()
    {
    TInt count = iAppStateStack.Count();

    if( count > 0 )
        {
        //remove current state
        iAppStateStack.Remove( count-1 );
        iAppStateStack.Compress();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CVcxNsUiEngine::MnpwoLastPlaybackPosition( const TDesC& /*aContentUri*/ )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxNsUiEngine::MnpwoSetLastPlaybackPosition( const TDesC& /*aContentUri*/, TInt /*aPos*/ )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetServiceClientHandlerL()
// -----------------------------------------------------------------------------
//
CVcxNsServiceClientHandler* CVcxNsUiEngine::GetServiceClientHandlerL()
    {
    IPTVLOGSTRING_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetServiceClientHandlerL() -- Enter");

    if ( !iServiceClientHandler )
        {
        iServiceClientHandler = CVcxNsServiceClientHandler::NewL( *this );
        }

    IPTVLOGSTRING2_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetServiceClientHandlerL() -- Returning: %d", (TInt) iServiceClientHandler );

    return iServiceClientHandler;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetContentClientHandlerL()
// -----------------------------------------------------------------------------
//
CVcxNsContentClientHandler* CVcxNsUiEngine::GetContentClientHandlerL()
    {
    IPTVLOGSTRING_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetContentClientHandlerL() -- Enter");

    if ( ! iContentClientHandler )
        {
        if ( ! iConnUtil )
            {
            iConnUtil = CVcxConnectionUtility::InstanceL();
            }

        iContentClientHandler = CVcxNsContentClientHandler::NewL( *this, *iConnUtil );
        }

    IPTVLOGSTRING2_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetContentClientHandlerL() -- Returning: %d", (TInt) iContentClientHandler );

     return iContentClientHandler;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetMpxCollectionClientHandler()
// -----------------------------------------------------------------------------
//
CVcxNsMpxCollectionClientHandler* CVcxNsUiEngine::GetMpxCollectionClientHandlerL()
    {
    IPTVLOGSTRING2_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetMpxCollectionClientHandler() -- Returning: %d", (TInt) iContentClientHandler );

    if ( ! iMpxCollectionClientHandler )
        {
        iMpxCollectionClientHandler = CVcxNsMpxCollectionClientHandler::NewL( *this );
        }

    return iMpxCollectionClientHandler;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetContentSearchHandlerL()
// -----------------------------------------------------------------------------
//
CVcxNsContentSearchHandler* CVcxNsUiEngine::GetContentSearchHandlerL()
    {
    IPTVLOGSTRING2_LOW_LEVEL("UI Engine ## CVcxNsUiEngine::GetContentSearchHandlerL() -- Returning: %d", 
            (TInt)iContentSearchHandler );

    return iContentSearchHandler;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetEventRouter()
// -----------------------------------------------------------------------------
//
CVcxNsEventRouter* CVcxNsUiEngine::GetEventRouter()
    {
    return iEventRouter;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::GetUpdateProgressHandler
// -----------------------------------------------------------------------------
//
CVcxNsUpdateProgressHandler* CVcxNsUiEngine::GetProgressHandler()
    {
    IPTVLOGSTRING2_LOW_LEVEL(
            "UI Engine ## CVcxNsUiEngine::GetProgressHandler() -- Returning: %d", 
            (TInt)iProgressHandler );
    return iProgressHandler;
    }

 // -----------------------------------------------------------------------------
 // CVcxNsUiEngine::ActiveService()
 // -----------------------------------------------------------------------------
 //
TUint32 CVcxNsUiEngine::ActiveService()
    {
    return iActiveService;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::SetActiveService()
// -----------------------------------------------------------------------------
//
void CVcxNsUiEngine::SetActiveService( TUint32 aServiceId )
    {
    iActiveService = aServiceId;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::ActiveServiceGroup()
// -----------------------------------------------------------------------------
//
TUint32 CVcxNsUiEngine::ActiveServiceGroup()
   {
   return iActiveServiceGroup;
   }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::SetActiveServiceGroup()
// -----------------------------------------------------------------------------
//
void CVcxNsUiEngine::SetActiveServiceGroup( TUint32 aServiceId )
   {
   iActiveServiceGroup = aServiceId;
   }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::SetActiveCategory()
// -----------------------------------------------------------------------------
//
void CVcxNsUiEngine::SetActiveCategory( TUint32 aCategoryId )
    {
    iActiveCategory = aCategoryId;
    }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::ServiceProvider()
// -----------------------------------------------------------------------------
//
CVcxNsServiceProvider* CVcxNsUiEngine::ServiceProvider()
     {
     return iServiceProvider;
     }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::ContentProvider()
// -----------------------------------------------------------------------------
//
CVcxNsContentProvider* CVcxNsUiEngine::ContentProvider()
     {
     return iContentProvider;
     }

// -----------------------------------------------------------------------------
// CVcxNsUiEngine::CategoryProvider()
// -----------------------------------------------------------------------------
//
CVcxNsCategoryProvider* CVcxNsUiEngine::CategoryProvider()
     {
     return iCategoryProvider;
     }
