/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for following new video and service count and update*
*/




// INCLUDE FILES

#include "IptvDebug.h"
#include "IptvLiveDataStructures.h" // CleanupResetAndDestroyPushL

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
 #include <aspaclienthelper.h>
 #include <ipvideo/IptvActivationMessage.h>
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

#include "IptvServer.pan"
#include "iptvactivespaceupdater.h"
#include "iptvactivespacepskeys.h"
#include "CIptvMyVideosGlobalFolderId.h"
#include "CIptvMyVideosVideoBriefDetails.h"
#include "CIptvMyVideosVideoFullDetails.h"


#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

static void CleanupArray( TAny* item )
    {
    ((RPointerArray<CActiveSpaceDataBase>*) item )->ResetAndDestroy();
    }

const TUid KIptvMain = {0x102750E2};
const TInt KIptvCountBufferLength = 255;

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// Constructor.
// --------------------------------------------------------------------------
//
CIptvActiveSpaceUpdater::CIptvActiveSpaceUpdater()
    {
    }

// --------------------------------------------------------------------------
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CIptvActiveSpaceUpdater* CIptvActiveSpaceUpdater::NewL()
    {
    return new ( ELeave ) CIptvActiveSpaceUpdater();
    }

#if !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__) 

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CIptvActiveSpaceUpdater::~CIptvActiveSpaceUpdater()
    {
    if ( iServiceManager )
        {
        iServiceManager->DeRegisterObserver( this );
        }

    iVodServices.Reset();
    iLiveTvServices.Reset();

    iVodCastCountProperty.Close();
    iLiveTVServiceCountProperty.Close();

    RProperty::Delete( KPSUidIptvServer, KIptvAsVodCastFeedCount );
    RProperty::Delete( KPSUidIptvServer, KIptvLiveTVServiceCount );

    }

// --------------------------------------------------------------------------
// Initialises Active Space updater with current data from My Videos Manager
// and Service Manager.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::InitialiseL(
        MIptvMyVideos* /*aMyVideos*/,      
        CIptvServiceManager* aServiceManager )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::InitialiseL() - Enter");
   
    iServiceManager = aServiceManager;

    TInt ret;

    ret = RProperty::Define( KPSUidIptvServer, KIptvAsVodCastFeedCount, RProperty::EInt );
    User::LeaveIfError( ret == KErrAlreadyExists ? KErrNone : ret );
    
    ret = RProperty::Define( KPSUidIptvServer, KIptvLiveTVServiceCount, RProperty::EInt );
       User::LeaveIfError( ret == KErrAlreadyExists ? KErrNone : ret );

    User::LeaveIfError(
        iVodCastCountProperty.Attach( KPSUidIptvServer, KIptvAsVodCastFeedCount ) );
    
    User::LeaveIfError(
        iLiveTVServiceCountProperty.Attach( KPSUidIptvServer, KIptvLiveTVServiceCount ) );

    iServiceManager->RegisterObserver( this );

  
    GetVodAndLiveTVServicesCountL();

    IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::InitialiseL() - Exit");
    }

// --------------------------------------------------------------------------
// Gets count of vod and live tv services.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::GetVodAndLiveTVServicesCountL()
    {
    CIptvServices* allServices = NULL;
    CIptvService::TServiceType type;

    iVodServices.Reset();
    iLiveTvServices.Reset();

    TRAPD( err, allServices = iServiceManager->GetServicesL(
               CIptvServiceManagementClient::ESelectedServices,
               CIptvServiceManagementClient::EDisplayOrderDescending,
               0 ) ); // 0 = use latest version.

    if ( err == KErrNone )
        {
        CleanupStack::PushL( allServices );

        // Go through all services.
        for ( TInt i = 0; i < allServices->Count(); i++ )
            {
            CIptvService* service = allServices->GetServiceL(i);
            CleanupStack::PushL( service );

            type = service->GetType();

            if ( type == CIptvService::EVod ||
                 type == CIptvService::EVodCast ||
                 type == CIptvService::EServiceGroup )
                {
                iVodServices.AppendL( service->GetId() );
                }
            else if( type == CIptvService::ELiveTv )
                {
                iLiveTvServices.AppendL( service->GetId() );
                }

            CleanupStack::PopAndDestroy( service );
            }

        CleanupStack::PopAndDestroy( allServices );
        
        // update values to PS
        iVodCastCountProperty.Set( iVodServices.Count() );
        iLiveTVServiceCountProperty.Set( iLiveTvServices.Count() );
        
        }
    }

// --------------------------------------------------------------------------
// Asynchronous callback function signaling service changes.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::HandleSmEvent( CIptvSmEvent& aEvent )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::HandleSmEvent()");
    // Transfer service selection modified - message to either service added
    // or service deleted -message.
    if ( aEvent.iEvent == CIptvSmEvent::EServiceSelectionModified )
        {
        if ( aEvent.iService )
            {
            if ( aEvent.iService->GetFlags() & CIptvService::ESelected )
                {
                aEvent.iEvent = CIptvSmEvent::EServiceAdded;
                }
            else  
                {
                aEvent.iEvent = CIptvSmEvent::EServiceDeleted;
                }
            }
        }
        
    TRAP_IGNORE( HandleSmEventL( aEvent ) );
    }

// --------------------------------------------------------------------------
// Handler for service changes.
// --------------------------------------------------------------------------
//    
void CIptvActiveSpaceUpdater::HandleSmEventL( CIptvSmEvent& aEvent )    
    {

    // Handle service added and service deleted -messages.
    switch ( aEvent.iEvent )
        {
        case CIptvSmEvent::EServiceAdded:
            {
            __ASSERT_DEBUG( 
                aEvent.iService, 
                User::Panic( KIptvServer, EIptvEngineServerBadState ) );

            if ( aEvent.iService )
                {
                HandleServiceAddedUpdateL( aEvent.iService );
                }
            }
            break;

        case CIptvSmEvent::EServiceDeleted:
            {
            HandleServiceDeletedUpdate( aEvent.iServiceId );
            }            
            break;

        case CIptvSmEvent::EServiceSelectionModified:
            {
            IPTVLOGSTRING_HIGH_LEVEL(
                "CIptvActiveSpaceUpdater : EServiceSelectionModified not handled!");

            __ASSERT_DEBUG( 
                0, 
                User::Panic( KIptvServer, EIptvEngineServerBadState ) );            
            }
            break;

        case CIptvSmEvent::EServiceModified:
        case CIptvSmEvent::EServiceScheduleModified:
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// updates either vod service or live tv service count
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::HandleServiceAddedUpdateL( CIptvService* aService )
    {   
    CIptvService::TServiceType type( aService->GetType() );
    TUint32 id( aService->GetId() );
    
    if ( ( type == CIptvService::EVod       ||
           type == CIptvService::EVodCast ) &&
         ( aService->GetFlags() & CIptvService::ESelected ) )
        {
        if( iVodServices.Find( id ) == KErrNotFound )
            {
            iVodServices.AppendL( id );        
            iVodCastCountProperty.Set( iVodServices.Count() );
            }
        return;
        }
    
    if( type == CIptvService::ELiveTv  &&
        ( aService->GetFlags() & CIptvService::ESelected ) )
        {
        if( iLiveTvServices.Find( id ) == KErrNotFound )
            {
            iLiveTvServices.AppendL( id ); 
            iLiveTVServiceCountProperty.Set( iLiveTvServices.Count() );
            }
        return;
        }    
    }

// --------------------------------------------------------------------------
// updates either vod service or live tv service count
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::HandleServiceDeletedUpdate( TUint32& aServiceId )
    {
    TInt index( 0 );
    for ( index = 0; index < iVodServices.Count(); index++ )
        {
        if ( aServiceId == iVodServices[index] )
            {
            iVodServices.Remove( index );
            iVodCastCountProperty.Set( iVodServices.Count() );
            return;
            }            
        }
    
    for ( index = 0; index < iVodServices.Count(); index++ )
        {
        if ( aServiceId == iVodServices[index] )
            {
            iLiveTvServices.Remove(index);
            iVodCastCountProperty.Set( iLiveTvServices.Count() );
            return;
            }            
        }    
    }


#else // !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__) 

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CIptvActiveSpaceUpdater::~CIptvActiveSpaceUpdater()
    {
    if ( iServiceManager )
        {
        iServiceManager->DeRegisterObserver( this );
        }

    
    iNewVideos.Reset();  
    iServices.Reset();

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    delete iActiveSpaceHelper;
    REComSession::FinalClose();
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    iNewVideoCountProperty.Close();
    iVodCastCountProperty.Close();
    
    RProperty::Delete( KPSUidIptvServer, KIptvAsNewVideoCount );
    RProperty::Delete( KPSUidIptvServer, KIptvAsVodCastFeedCount );
    
    delete iNewestVideoName;
    delete iNewestVideoThumbnailPath;  
    }

// --------------------------------------------------------------------------
// Called whenever service count changes. Updates Active Space.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::UpdateServiceCountToSpaceL()
    {
    // Publish the new count
    iVodCastCountProperty.Set( iServices.Count() );

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    IPTVLOGSTRING2_HIGH_LEVEL(
        "CIptvActiveSpaceUpdater::UpdateServiceCountToSpaceL(%d)", iServices.Count());

    if( !iActiveSpaceHelper )
        return;                 // Active Space not found
    	
    CActiveSpaceBase* activeSpace = iActiveSpaceHelper->NewActiveSpaceL();
    CleanupStack::PushL( activeSpace );
        
    RPointerArray<CActiveSpaceDataBase> dataArray;
    TCleanupItem cleanup( CleanupArray, &dataArray );
    CleanupStack::PushL( cleanup );
    TActiveSpaceDataProperties searchProperties;
    searchProperties.iGenre = EActiveSpaceGenreWatch;
    searchProperties.iCategory = EActiveSpaceCategory4th;
    
    activeSpace->FindDataL( searchProperties, EActiveSpaceSortCriteriaNotDefined, 
        dataArray );
        
    if( dataArray.Count() )
        {
        // Child entry to modify
        RPointerArray<CActiveSpaceDataBase> dataArray2;
        TCleanupItem cleanup( CleanupArray, &dataArray2 );
        CleanupStack::PushL( cleanup );
        TActiveSpaceDataProperties properties = searchProperties;
        properties.iCategory = EActiveSpaceCategory1st;
   
        activeSpace->FindDataL( properties, EActiveSpaceSortCriteriaNotDefined, 
            dataArray2, dataArray[0]->Id() );
 
        if( dataArray2.Count() )                        
            {
            // Update child entry name with string and count
            switch( iServices.Count() )
                {
                case 0:
                    {
                    dataArray2[0]->SetName( EVideoTileNoFeeds );
                    break;
                    }
                case 1:
                    {
                    dataArray2[0]->SetName( EVideoTile1Feed );
                    break;
                    }
                default:
                    {
                    dataArray2[0]->SetName( EVideoTileFeeds );
                    
                    TBuf<KIptvCountBufferLength> countBuf;
                    countBuf.Num( iServices.Count() );
                    dataArray2[0]->SetNameArgumentL( 0, countBuf );
                    break;
                    }
                }
            activeSpace->UpdateDataL( dataArray2[0], 
                dataArray2[0]->Id() );
            }
        CleanupStack::PopAndDestroy( &dataArray2 );
        }
    else
        {
        // No entry found, create new one
	    IPTVLOGSTRING_LOW_LEVEL(
	        "CIptvActiveSpaceUpdater::UpdateServiceCountToSpaceL() - New entry");
       
        // Create item to 4th title
        TActiveSpaceDataProperties properties;
	    properties.iGenre = EActiveSpaceGenreWatch;
	    properties.iSource = EActiveSpaceSourceLocal;
	    properties.iOwner = EActiveSpaceOwnerCommercial;
	    properties.iFlags = EActiveSpaceFlagNone;    
	    properties.iCategory = EActiveSpaceCategory4th;
	
	    // Create parent    
	    CActiveSpaceDataBase* parentData = iActiveSpaceHelper->NewActiveSpaceDataL();
	    CleanupStack::PushL( parentData );
	    IPTVLOGSTRING_LOW_LEVEL(
	        "CIptvActiveSpaceUpdater::UpdateServiceCountToSpaceL() - Parent Created");
	    parentData->SetParent( CActiveSpaceDataBase::EActiveSpaceRoot );		
	    parentData->SetName( EVideoTileInternetVideos );
	    parentData->SetProperties( properties );
	
	    TInt parentId = activeSpace->AddDataL( parentData,
	                                           CActiveSpaceDataBase::EActiveSpaceRoot );
	    IPTVLOGSTRING_LOW_LEVEL(
	        "CIptvActiveSpaceUpdater::UpdateServiceCountToSpaceL() - Parent data added");
	
	    if( parentId > 0 )          // No error
	        {    
            // Create child
            CActiveSpaceDataBase* childData = iActiveSpaceHelper->NewActiveSpaceDataL();
	        CleanupStack::PushL( childData );  
	        childData->SetParent( parentId );
	        properties.iCategory = EActiveSpaceCategory1st;
	        childData->SetProperties( properties );
	        
	        switch( iServices.Count() )
                {
                case 0:
                    {
                    childData->SetName( EVideoTileNoFeeds );
                    break;
                    }
                case 1:
                    {
                    childData->SetName( EVideoTile1Feed );
                    break;
                    }
                default:
                    {
                    childData->SetName( EVideoTileFeeds );
                    
                    TBuf<KIptvCountBufferLength> countBuf;
                    countBuf.Num( iServices.Count() );
                    childData->SetNameArgumentL( 0, countBuf );
                    break;
                    }
                }
	        	
	        TInt childId = activeSpace->AddDataL( childData, parentId );	    
	        IPTVLOGSTRING_LOW_LEVEL(
	            "CIptvActiveSpaceUpdater::UpdateServiceCountToSpaceL() - Child data added");    
	
            CASpaAppLauncherActionDataBase* videoData = 
                iActiveSpaceHelper->NewAppLauncherActionDataL();
	        CleanupStack::PushL( videoData ); 	

	        TUid videoCenterAppUid = KIptvMain;
	        TUid vodPlayerUid = TUid::Uid( KIptvVodStartPlayer );
	
            TIptvActivationMessage params;
		    params.iMsgType = TIptvActivationMessage::EOpenInternetVideos;
		    params.iMsgSender = TIptvActivationMessage::EActiveSpace;
		    params.iServiceId = 0;
		    params.iActive = 1;
		    params.iSingle = ETrue;
		    params.iFileId = 0;
		    params.iDrive = 0;
		    params.iFullPath = KNullDesC;
		    TPckg<TIptvActivationMessage> paramsPckg( params );
	
	        videoData->SetApplicationUid( videoCenterAppUid );
	        videoData->SetApaLaunchingMethod( CASpaAppLauncherActionDataBase::
                EApaLaunchMethodStartApaMessageWithTail );               
	        videoData->SetAdditionalDataL( paramsPckg );                 
	        videoData->SetMessageUid( vodPlayerUid );	
	    
	        const TInt KCASpaAppLauncherPlugin = 0x10282E5F;
	        TUid AppLauncherPlugin( TUid::Uid( KCASpaAppLauncherPlugin ) );

	        CActiveSpaceActionBase* videoAction = iActiveSpaceHelper->NewActiveSpaceActionL();
	        CleanupStack::PushL( videoAction );

	        videoAction->SetType( EActiveSpaceActionTypeLaunchApplication );		
	        videoAction->SetActionHandlerUid( TUid::Uid( KCASpaAppLauncherPlugin ) );	
	        videoAction->SetKeys( EActiveSpaceActionKeySelect );
	        videoAction->SetDataL( videoData->GetBufferL() );
		
	        IPTVLOGSTRING_LOW_LEVEL(
	            "CIptvActiveSpaceUpdater::UpdateServiceCountToSpaceL() - Action object created");
										
	        TInt actionId = activeSpace->AddActionL( videoAction );
	        activeSpace->BindActionToDataL( parentId, actionId );
	        IPTVLOGSTRING_LOW_LEVEL(
	            "CIptvActiveSpaceUpdater::UpdateServiceCountToSpaceL() - Action binded");
	
	        CleanupStack::PopAndDestroy( videoAction );
	        CleanupStack::PopAndDestroy( videoData );                        
	        CleanupStack::PopAndDestroy( childData );
	        }
	    CleanupStack::PopAndDestroy( parentData );	   
        }
                        
    CleanupStack::PopAndDestroy( &dataArray );
    CleanupStack::PopAndDestroy( activeSpace );

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    }

// --------------------------------------------------------------------------
// Called whenever new video count changes. Updates Active Space.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::UpdateVideoCountToSpaceL()
    {
    // Publish the new count
    iNewVideoCountProperty.Set( iNewVideos.Count() );
    
    // Publish video name and path
    if( iNewestVideoName->Length() )
        {
        iNewestVideoNameProperty.Set( *iNewestVideoName );
        }
    else
        {
        iNewestVideoNameProperty.Set( KNullDesC );
        }
    if( iNewestVideoThumbnailPath->Length() > 1 )
        {
        iNewestVideoThumbnailPathProperty.Set( *iNewestVideoThumbnailPath );
        }
    else
        {
        iNewestVideoThumbnailPathProperty.Set( KNullDesC );
        }

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    IPTVLOGSTRING2_HIGH_LEVEL(
        "CIptvActiveSpaceUpdater::UpdateVideoCountToSpaceL(%d)", iNewVideos.Count());
    
    if( !iActiveSpaceHelper )
        return;                 // Active Space not found
    	
    CActiveSpaceBase* activeSpace = iActiveSpaceHelper->NewActiveSpaceL();
    CleanupStack::PushL( activeSpace );
        
    RPointerArray<CActiveSpaceDataBase> dataArray;
    TCleanupItem cleanup( CleanupArray, &dataArray );
    CleanupStack::PushL( cleanup );
    TActiveSpaceDataProperties searchProperties;
    searchProperties.iGenre = EActiveSpaceGenreWatch;
    searchProperties.iCategory = EActiveSpaceCategory3rd;
    
    activeSpace->FindDataL( searchProperties, EActiveSpaceSortCriteriaNotDefined, 
        dataArray );
        
    if( dataArray.Count() )
        {
        // Child entry to modify
        RPointerArray<CActiveSpaceDataBase> dataArray2;
        TCleanupItem cleanup( CleanupArray, &dataArray2 );
        CleanupStack::PushL( cleanup );
        TActiveSpaceDataProperties properties = searchProperties;
        properties.iCategory = EActiveSpaceCategory1st;
   
        activeSpace->FindDataL( properties, EActiveSpaceSortCriteriaNotDefined, 
            dataArray2, dataArray[0]->Id() );
 
        if( dataArray2.Count() )                        
            {
            // Update child entry name with string and count
            switch( iNewVideos.Count() )
                {
                case 0:
                    {
                    dataArray2[0]->SetName( EVideoTileNoNewVids );
                    break;
                    }
                case 1:
                    {
                    dataArray2[0]->SetName( EVideoTile1NewVid );
                    break;
                    }
                default :
                    {
                    dataArray2[0]->SetName( EVideoTileNewVids );
                    
                    TBuf<KIptvCountBufferLength> countBuf;
                    countBuf.Num( iNewVideos.Count() );
                    dataArray2[0]->SetNameArgumentL( 0, countBuf );
                    break;
                    }
                }
         
            activeSpace->UpdateDataL( dataArray2[0], 
                dataArray2[0]->Id() );
            } 
        CleanupStack::PopAndDestroy( &dataArray2 );
        }
    else
        {
        // No entry found, create new one
	    IPTVLOGSTRING_LOW_LEVEL(
	        "CIptvActiveSpaceUpdater::UpdateVideoCountToSpaceL() - New entry");
       
        // Create item to 3rd category
        TActiveSpaceDataProperties properties;
	    properties.iGenre = EActiveSpaceGenreWatch;
	    properties.iSource = EActiveSpaceSourceLocal;
	    properties.iOwner = EActiveSpaceOwnerCommercial;
	    properties.iFlags = EActiveSpaceFlagNone;    
	    properties.iCategory = EActiveSpaceCategory3rd;
	
	    // Create parent    
	    CActiveSpaceDataBase* parentData = iActiveSpaceHelper->NewActiveSpaceDataL();
	    CleanupStack::PushL( parentData );
	    IPTVLOGSTRING_LOW_LEVEL(
	        "CIptvActiveSpaceUpdater::UpdateVideoCountToSpaceL() - Parent created");
	    parentData->SetParent( CActiveSpaceDataBase::EActiveSpaceRoot );		
	    parentData->SetName( EVideoTileWatchList );
	    parentData->SetProperties( properties );
	
	    TInt parentId = activeSpace->AddDataL( parentData, 
	                                           CActiveSpaceDataBase::EActiveSpaceRoot );
	    IPTVLOGSTRING_LOW_LEVEL(
	        "CIptvActiveSpaceUpdater::UpdateVideoCountToSpaceL() - Parent data added");
	
	    if( parentId > 0 )          // No error
	        {    
            // Create child
            CActiveSpaceDataBase* childData = iActiveSpaceHelper->NewActiveSpaceDataL();
	        CleanupStack::PushL( childData );  
	        childData->SetParent( parentId );
	        properties.iCategory = EActiveSpaceCategory1st;
	        childData->SetProperties( properties );
	        
	        switch( iNewVideos.Count() )
                {
                case 0:
                    {
                    childData->SetName( EVideoTileNoNewVids );
                    break;
                    }
                case 1:
                    {
                    childData->SetName( EVideoTile1NewVid );
                    break;
                    }
                default:
                    {
                    childData->SetName( EVideoTileNewVids );
                    
                    TBuf<KIptvCountBufferLength> countBuf;
                    countBuf.Num( iNewVideos.Count() );
                    childData->SetNameArgumentL( 0, countBuf );
                    break;
                    }
                }
	        	
	        TInt childId = activeSpace->AddDataL( childData, parentId );	    
	        IPTVLOGSTRING_LOW_LEVEL(
	            "CIptvActiveSpaceUpdater::UpdateVideoCountToSpaceL() - Child data added");
	
            CASpaAppLauncherActionDataBase* videoData = 
                iActiveSpaceHelper->NewAppLauncherActionDataL();
	        CleanupStack::PushL( videoData ); 	

	        TUid videoCenterAppUid = KIptvMain;
	        TUid vodPlayerUid = TUid::Uid( KIptvVodStartPlayer );
	
            TIptvActivationMessage params;
		    params.iMsgType = TIptvActivationMessage::EOpenVideoStorage;
		    params.iMsgSender = TIptvActivationMessage::EActiveSpace;
		    params.iServiceId = 0;
		    params.iActive = 1;
		    params.iSingle = ETrue;
		    params.iFileId = 0;
		    params.iDrive = 0;
		    params.iFullPath = KNullDesC;
		    TPckg<TIptvActivationMessage> paramsPckg( params );
	
	        videoData->SetApplicationUid( videoCenterAppUid );
	        videoData->SetApaLaunchingMethod( CASpaAppLauncherActionDataBase::
                EApaLaunchMethodStartApaMessageWithTail );               
	        videoData->SetAdditionalDataL( paramsPckg );                 
	        videoData->SetMessageUid( vodPlayerUid );	
	    
	        const TInt KCASpaAppLauncherPlugin = 0x10282E5F;
	        TUid AppLauncherPlugin( TUid::Uid( KCASpaAppLauncherPlugin ) );

	        CActiveSpaceActionBase* videoAction = iActiveSpaceHelper->NewActiveSpaceActionL();
	        CleanupStack::PushL( videoAction );

	        videoAction->SetType( EActiveSpaceActionTypeLaunchApplication );		
	        videoAction->SetActionHandlerUid( TUid::Uid( KCASpaAppLauncherPlugin ) );	
	        videoAction->SetKeys( EActiveSpaceActionKeySelect );
	        videoAction->SetDataL( videoData->GetBufferL() );
		
	        IPTVLOGSTRING_LOW_LEVEL(
	            "CIptvActiveSpaceUpdater::UpdateVideoCountToSpaceL() - Action object rreated");
										
	        TInt actionId = activeSpace->AddActionL( videoAction );
	        activeSpace->BindActionToDataL( parentId, actionId );
	        IPTVLOGSTRING_LOW_LEVEL(
	            "CIptvActiveSpaceUpdater::UpdateVideoCountToSpaceL() - Action binded");
	
	        CleanupStack::PopAndDestroy( videoAction );
	        CleanupStack::PopAndDestroy( videoData );                        
	        CleanupStack::PopAndDestroy( childData );    
            }
        CleanupStack::PopAndDestroy( parentData );	   
        }                   
    CleanupStack::PopAndDestroy( &dataArray );
    CleanupStack::PopAndDestroy( activeSpace );

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    }

// --------------------------------------------------------------------------
// Initialises Active Space updater with current data from My Videos Manager
// and Service Manager.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::InitialiseL(
        MIptvMyVideos* aMyVideos,     
        CIptvServiceManager* aServiceManager )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::InitialiseL() - Enter");

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
 	TRAPD( err, iActiveSpaceHelper = CActiveSpaceHelper::NewL() );
 	if( err == KErrNotFound )
 	    {
 	    iActiveSpaceHelper = NULL;      // Active Space not found
 	    }
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    iMyVideos       = aMyVideos;
   
    iServiceManager = aServiceManager;

    TInt ret;
    
    ret = RProperty::Define( KPSUidIptvServer, KIptvAsNewVideoCount, RProperty::EInt );
    User::LeaveIfError( ret == KErrAlreadyExists ? KErrNone : ret );

    ret = RProperty::Define( KPSUidIptvServer, KIptvAsVodCastFeedCount, RProperty::EInt );
    User::LeaveIfError( ret == KErrAlreadyExists ? KErrNone : ret );
    
    ret = RProperty::Define( KPSUidIptvServer, KIptvNewestVideoName, RProperty::ELargeText );
    User::LeaveIfError( ret == KErrAlreadyExists ? KErrNone : ret );
    
    ret = RProperty::Define( KPSUidIptvServer, KIptvNewestVideoThumbnailPath, RProperty::ELargeText );
    User::LeaveIfError( ret == KErrAlreadyExists ? KErrNone : ret );
   
    User::LeaveIfError( 
        iNewVideoCountProperty.Attach( KPSUidIptvServer, KIptvAsNewVideoCount ) );
    User::LeaveIfError(
        iVodCastCountProperty.Attach( KPSUidIptvServer, KIptvAsVodCastFeedCount ) );
    
    User::LeaveIfError( 
        iNewestVideoNameProperty.Attach( KPSUidIptvServer, KIptvNewestVideoName ) );
    User::LeaveIfError(
        iNewestVideoThumbnailPathProperty.Attach( KPSUidIptvServer, KIptvNewestVideoThumbnailPath ) );

    iServiceManager->RegisterObserver( this );

  
    iNewestVideoName  = HBufC::NewL( NULL );
    iNewestVideoThumbnailPath = HBufC::NewL( NULL );

    GetVideosCountL( ETrue );
  
    GetVodcastServicesCountL();

    IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::InitialiseL() - Exit");
    }

// --------------------------------------------------------------------------
// Gets list of all videos and updates new video count.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::GetVideosCountL( TBool aForceUpdate )
    {
    CIptvMyVideosGlobalFolderId* rootFolder =
        CIptvMyVideosGlobalFolderId::NewRootFolderL();
    CleanupStack::PushL( rootFolder );
    TUint32 temp;
    RPointerArray<CIptvMyVideosVideoBriefDetails> videoList;
    CleanupResetAndDestroyPushL( videoList );

    TInt oldVideosCount = iNewVideos.Count();
    iNewVideos.Reset();

    if ( iMyVideos->GetVideoListL( *rootFolder, 0, 0, temp, videoList ) 
         == KErrNone )
        {
        for ( TInt i = 0; i < videoList.Count(); i++ )
            {
            if ( videoList[i]->iVideoFlags & EIptvMyVideosVideoNew )
                {
                TNewVideoInformation video;
                video.iDrive  = videoList[i]->iFileId->iDrive;
                video.iFileId = videoList[i]->iFileId->iFileId;
                iNewVideos.AppendL( video );
                }
            }

        if ( aForceUpdate ||
             ( oldVideosCount != iNewVideos.Count() ) )
            {
            GetNewestVideoL( videoList );
            UpdateVideoCountToSpaceL();
            }
        }

    CleanupStack::PopAndDestroy(); // videoList
    CleanupStack::PopAndDestroy( rootFolder );
    }

// --------------------------------------------------------------------------
// Gets list of Vodcast services and updates service count.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::GetVodcastServicesCountL()
    {
    CIptvServices* allServices = NULL;
    CIptvService::TServiceType type;

    iServices.Reset();

    TRAPD( err, allServices = iServiceManager->GetServicesL(
               CIptvServiceManagementClient::ESelectedServices,
               CIptvServiceManagementClient::EDisplayOrderDescending,
               0 ) ); // 0 = use latest version.

    if ( err == KErrNone )
        {
        CleanupStack::PushL( allServices );

        // Go through all services.
        for ( TInt i = 0; i < allServices->Count(); i++ )
            {
            CIptvService* service = allServices->GetServiceL(i);
            CleanupStack::PushL( service );

            type = service->GetType();

            if ( type == CIptvService::EVod ||
                 type == CIptvService::EVodCast ||
                 type == CIptvService::EServiceGroup )
                {
  	            iServices.AppendL( service->GetId() );
                }

            CleanupStack::PopAndDestroy( service );
            }

        CleanupStack::PopAndDestroy( allServices );

        UpdateServiceCountToSpaceL();
        }
    }

// --------------------------------------------------------------------------
// Gets list of all videos and updates newest video name and thumbnail path.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::GetNewestVideoL( 
    RPointerArray<CIptvMyVideosVideoBriefDetails>& aVideoList )
    {
    TDateTime nDateTime( 0, TMonth( 0 ), 0, 0, 0, 0, 0 );
    TTime newest( nDateTime );
    iNewestVideoName->Des().Zero();
    iNewestVideoThumbnailPath->Des().Zero();
                
    for( TInt i = 0; i < aVideoList.Count(); i++ )
        {
        if ( aVideoList[i]->iVideoFlags & EIptvMyVideosVideoNew )
            {
            TTime downloadTime( aVideoList[i]->iDownloadTime );
            if( downloadTime.Int64() > newest.Int64() )
                {
                SetNewestVideoL( *aVideoList[i] );

                newest = downloadTime;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Create list for newest video search.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::GetNewestVideoListL()
    {
    CIptvMyVideosGlobalFolderId* rootFolder =
        CIptvMyVideosGlobalFolderId::NewRootFolderL();
    CleanupStack::PushL( rootFolder );
    TUint32 temp;
    RPointerArray<CIptvMyVideosVideoBriefDetails> videoList;
    CleanupResetAndDestroyPushL( videoList );

    if( iMyVideos->GetVideoListL( *rootFolder, 0, 0, temp, videoList ) 
         == KErrNone )
        {
        GetNewestVideoL( videoList );
        }
    CleanupStack::PopAndDestroy(); // videoList
    CleanupStack::PopAndDestroy( rootFolder );
    }
    
// --------------------------------------------------------------------------
// Set newest video name and thumbnail path.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::SetNewestVideoL( CIptvMyVideosVideoBriefDetails& aDetails )
    {
    delete iNewestVideoName;
    iNewestVideoName  = NULL;
    iNewestVideoName  = HBufC::NewL( aDetails.Name().Length() );
    *iNewestVideoName = aDetails.Name();
     
    delete iNewestVideoThumbnailPath;
    iNewestVideoThumbnailPath = NULL;
    iNewestVideoThumbnailPath = HBufC::NewL( aDetails.ThumbnailPath().Length() );
    *iNewestVideoThumbnailPath = aDetails.ThumbnailPath();
    }

// --------------------------------------------------------------------------
// Asynchronous callback function signaling service changes.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::HandleSmEvent( CIptvSmEvent& aEvent )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::HandleSmEvent()");

    TRAP_IGNORE( HandleSmEventL( aEvent ) );
    }

// --------------------------------------------------------------------------
// Handler for service changes.
// --------------------------------------------------------------------------
//    
void CIptvActiveSpaceUpdater::HandleSmEventL( CIptvSmEvent& aEvent )    
    {
    // Transfer service selection modified - message to either service added
    // or service deleted -message.
    if ( aEvent.iEvent == CIptvSmEvent::EServiceSelectionModified )
        {
        if ( aEvent.iService &&
             ( aEvent.iService->GetFlags() & CIptvService::ESelected ) )
            {
            aEvent.iEvent = CIptvSmEvent::EServiceAdded;
            }
        else if ( ! ( aEvent.iService->GetFlags() & CIptvService::ESelected ) )
            {
            aEvent.iEvent = CIptvSmEvent::EServiceDeleted;
            }
        }

    // Handle service added and service deleted -messages.
    switch ( aEvent.iEvent )
        {
        case CIptvSmEvent::EServiceAdded:
            {
            __ASSERT_DEBUG( 
                aEvent.iService, 
                User::Panic( KIptvServer, EIptvEngineServerBadState ) );

            if ( aEvent.iService )
                {
                TBool add    = EFalse;
                TBool exists = EFalse;
                
                if ( ( aEvent.iService->GetType() == CIptvService::EVod ||
                       aEvent.iService->GetType() == CIptvService::EVodCast ||
                       aEvent.iService->GetType() == CIptvService::EServiceGroup )
                     && ( aEvent.iService->GetFlags() & CIptvService::ESelected ) )
                    {
                    add = ETrue;
                    }

                for ( TInt i = 0; i < iServices.Count(); i++ )
                    {
                    if ( aEvent.iServiceId == iServices[i] )
                        {
                        exists = ETrue;
                        break;
                        }
                    }

                if ( add && ! exists )
                    {
                    iServices.AppendL( aEvent.iServiceId );
                    
                    UpdateServiceCountToSpaceL();
                    }
                }
            }
            break;

        case CIptvSmEvent::EServiceDeleted:
            {
            for ( TInt i = 0; i < iServices.Count(); i++ )
                {
                if ( aEvent.iServiceId == iServices[i] )
                    {
                    iServices.Remove(i);

                    UpdateServiceCountToSpaceL();

                    return;
                    }
                }            
            }
            break;

        case CIptvSmEvent::EServiceSelectionModified:
            {
            IPTVLOGSTRING_HIGH_LEVEL(
                "CIptvActiveSpaceUpdater : EServiceSelectionModified not handled!");

            __ASSERT_DEBUG( 
                0, 
                User::Panic( KIptvServer, EIptvEngineServerBadState ) );            
            }
            break;

        case CIptvSmEvent::EServiceModified:
        case CIptvSmEvent::EServiceScheduleModified:
        default:
            {
            }
            break;
        }
    }

// --------------------------------------------------------------------------
// Asynchronous callback function signaling video changes.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::ContentsUpdated(
        TIptvContentUpdatedEvent aEvent )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::ContentsUpdated()");

    TRAP_IGNORE( ContentsUpdatedL( aEvent ) );
    }

// --------------------------------------------------------------------------
// Handler for video changes.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::ContentsUpdatedL(
        TIptvContentUpdatedEvent aEvent )
    {
    switch ( aEvent.iEventType )
        {
        case EAdded:
        case EUpdated:
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::ContentsUpdatedL() EAdded/EUpdated");

            CIptvMyVideosVideoFullDetails* details = 
                CIptvMyVideosVideoFullDetails::NewLC();
            CIptvMyVideosGlobalFileId* fileId =
                CIptvMyVideosGlobalFileId::NewLC();
            TBool isNew  = EFalse;
            TBool exists = EFalse;
            TInt  index  = KErrNotFound;

            fileId->iDrive  = aEvent.iDrive;
            fileId->iFileId = aEvent.iFileId;

            if ( iMyVideos->GetVideoDetailsL( *fileId, *details )
                 == KErrNone )
                {
                if ( details->iVideoFlags & EIptvMyVideosVideoNew )
                    {
                    isNew = ETrue;
                    }

                for ( TInt i = 0; i < iNewVideos.Count(); i++ )
                    {
                    if ( iNewVideos[i].iDrive == aEvent.iDrive &&
                         iNewVideos[i].iFileId == aEvent.iFileId )
                        {
                        exists = ETrue;
                        index = i;
                        break;
                        }
                    }

                if ( isNew && ! exists )
                    {
                    IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::ContentsUpdatedL() new flag added.");

                    TNewVideoInformation video;
                    video.iDrive  = aEvent.iDrive;
                    video.iFileId = aEvent.iFileId;
                    iNewVideos.AppendL( video );
                    
                    SetNewestVideoL( *details );
                    UpdateVideoCountToSpaceL();                    
                    }
                else if ( ! isNew && exists )
                    {
                    IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::ContentsUpdatedL() new flag removed.");

                    iNewVideos.Remove( index );
                    
                    GetNewestVideoListL();
                    UpdateVideoCountToSpaceL();
                    }
                }

            CleanupStack::PopAndDestroy( fileId );
            CleanupStack::PopAndDestroy( details );
            }
            break;

        case ERemoved:
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::ContentsUpdatedL() ERemoved");

            for ( TInt i = 0; i < iNewVideos.Count(); i++ )
                {
                if ( iNewVideos[i].iDrive == aEvent.iDrive &&
                     iNewVideos[i].iFileId == aEvent.iFileId )
                    {
                    iNewVideos.Remove(i);
                    
                    GetNewestVideoListL();
                    UpdateVideoCountToSpaceL();
                    return;
                    }
                }
            }
            break;        

        case EContentUpdated:
        default:
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvActiveSpaceUpdater::ContentsUpdatedL() EContentUpdated");
            
            GetVideosCountL( EFalse );
            }
            break;        
        }
    }

// --------------------------------------------------------------------------
// Asynchronous response callback function to video GetTotalVideoLenght().
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::TotalVideoLenghtResponse(
    TIptvPlayTime /*aTotalPlayTime*/,
    TIptvFileSize /*aTotalFileSize*/ )
    {
    }

// --------------------------------------------------------------------------
// Asynchronous response callback function to copy or move.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::CopyOrMoveResponse(
    TInt /*aServerStatus*/,
    TInt /*aOperationStatus*/,
    TInt /*aFailed*/ )
    {
    }

// --------------------------------------------------------------------------
// Asynchronous response callback function to delete.
// --------------------------------------------------------------------------
//
void CIptvActiveSpaceUpdater::DeleteResponse( TInt /*aStatus*/ )
    {
    }

#endif // !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__) 
