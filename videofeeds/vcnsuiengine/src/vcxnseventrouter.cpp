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




#include <mpxmessage2.h>
#include <mpxcollectionutility.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionpath.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessage.h>
#include <mpxlog.h>
#include <mpxitemid.h>
#include <mpxmessagecontainerdefs.h>
#include <vcxmyvideosdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>

#include "vcxnseventrouter.h"
#include "vcxnsuiengine.h"
#include "CIptvSmEvent.h"
#include "CIptvUtil.h"
#include "vcxnsserviceprovider.h"
#include "vcxnscontentprovider.h"
#include "vcxnscategoryprovider.h"
#include "vcxnsservice.h"
#include "vcxnscontent.h"
#include "vcxnscategory.h"
#include "vcxnsmpxcollectionclienthandler.h"
#include "vcxnscontentsearchhandler.h"
#include "vcxnsupdateprogresshandler.h"

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::CVcxNsEventRouter()
// -----------------------------------------------------------------------------
//
CVcxNsEventRouter::CVcxNsEventRouter( CVcxNsUiEngine& aUiEngine )
    :iUiEngine( aUiEngine )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::~CVcxNsEventRouter()
// -----------------------------------------------------------------------------
//
CVcxNsEventRouter::~CVcxNsEventRouter( )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsEventRouter* CVcxNsEventRouter::NewL( CVcxNsUiEngine& aUiEngine )
    {
    CVcxNsEventRouter* self = new ( ELeave ) CVcxNsEventRouter( aUiEngine );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::HandleSmEvent()
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::HandleSmEvent( CIptvSmEvent& aEvent )
    {
    // check when to reset the data
    if( iUiEngine.ServiceProvider() )
         {
         iUiEngine.ServiceProvider()->RemoveServiceData();
         }

    RemoveServiceData( aEvent.iServiceId );
    
    if( iUiEngine.VcAppState() == EStateServiceView || 
        aEvent.iServiceId == iUiEngine.ActiveService() )
        {
        // Services can be added or deleted, so let's redraw all.
        UpdateActiveView();
        
        if ( aEvent.iEvent == CIptvSmEvent::EServiceAdded )
            {
            // Assume that the added is always first
            iUiEngine.ServiceProvider()->ServiceAdded( 0 );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::HandleEpgManagerMsgL()
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::HandleEpgManagerMsgL( 
    TInt aMsg, TInt aInfo, TIptvServiceId aServiceId )
    {
    CVcxNsUpdateProgressHandler* progressHandler = iUiEngine.GetProgressHandler();
    if ( progressHandler )
        {
        progressHandler->HandleUpdateStatusChangeL( aServiceId, aMsg, aInfo );
        }

    CVcxNsServiceProvider* serviceProvider = iUiEngine.ServiceProvider();
    User::LeaveIfNull( serviceProvider );
    
    switch( aMsg )
        {
        case KIptvVodUpdateNotStarted:
            {
            serviceProvider->RefreshNextPendingService();

            IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::HandleEpgManagerMsgL: KIptvVodUpdateNotStarted");
            }
            break;
        case KIptvVodUpdateStarted: //Update process started
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::HandleEpgManagerMsgL: KIptvVodUpdateStarted");
            iUpdatedService = aServiceId;
            serviceProvider->UpdateServiceUpdateStatusL( aServiceId, CVcxNsService::EUpdateOngoing );
            if ( iUiEngine.VcAppState() == EStateContentView  && 
                 iUiEngine.GetContentProviderL()->GetContentsL().Count() == 0 )
                {
                UpdateActiveView();
                }

            iThumbnailCount = 0;
            iThumbnailsDownloaded = 0;
            UpdateInfoPopupL();
            }
            break;

        case KIptvContentUpdateCompleted:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "CVcxNsEventRouter::HandleEpgManagerMsgL: KIptvContentUpdateCompleted");

            // Refresh the view before thumbnails are downloaded, if there was
            // no old data. Empty category view shouldn't be ever shown.
            
            RPointerArray< CVcxNsCategory > categories =
                iUiEngine.GetCategoryProviderL()->GetCategorysL();

            if( ( iUiEngine.VcAppState() == EStateContentView  &&
                    categories.Count() == 0 )
                || ( iUiEngine.VcAppState() == EStateCategoryView &&
                        categories.Count() == 0 )
                || ( iUiEngine.VcAppState() == EStateCategoryView &&
                        categories.Count() == 1 &&
                        categories[0]->GetCategoryId() == KIptvRssSearchCategoryId )
                || ( iUiEngine.VcAppState() == EStateCustomView &&
                     iUiEngine.ActiveService() == aServiceId ) )
                {
                if ( !( iUiEngine.VcAppState() == EStateCustomView ) )
                    {
                    CheckAppStateL( aServiceId );
                    }
                UpdateActiveView();
                }
          
            iThumbnailCount = 0;
            iThumbnailsDownloaded = 0;
            UpdateInfoPopupL();
            }
            break;

        case KIptvThumbnailCount:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "CVcxNsEventRouter::HandleEpgManagerMsgL: KIptvThumbnailCount");

            // Store the tbn count even if we're in wrong service.
            iThumbnailCount = aInfo;
            UpdateInfoPopupL();
            }
            break;

        case KIptvServiceThumbnailDownloaded:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "CVcxNsEventRouter::HandleEpgManagerMsgL: KIptvServiceThumbnailDownloaded");
            serviceProvider->ReloadThumbnailPathL( aServiceId );
            if( iUiEngine.VcAppState() == EStateServiceView )
                {
                serviceProvider->UpdateService( aServiceId );
                }
          
            iThumbnailsDownloaded++;
            UpdateInfoPopupL();
            }
            break;

        case KIptvContentThumbnailDownloaded:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "CVcxNsEventRouter::HandleEpgManagerMsgL: KIptvContentThumbnailDownloaded");
            //When downloading group thumbnails, 
            //aInfo includes service id information for grouped service
            CVcxNsService* serviceData = serviceProvider->GetServiceDataById( aServiceId );
            if ( serviceData && serviceData->Type() == CVcxNsService::EVcxNsMainGroup )
                {
                serviceProvider->ReloadThumbnailPathL( aInfo );
                }

            // Store the tbn count even if we're in wrong service.
            iThumbnailsDownloaded++;
            UpdateInfoPopupL();
            }
            break;

        case KIptvErrorEpgUpdateStopped:
            {
            //This event comes when plugin has stopped all activities for update
            //We can start updating next service as server is not busy with last update anymore.
            IPTVLOGSTRING_LOW_LEVEL(
                "CVcxNsEventRouter::HandleEpgManagerMsgL: KIptvErrorEpgUpdateStopped");
            serviceProvider->RefreshNextPendingService();
            }
            break;
        case KIptvErrorEpgUpdateSuccessed:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "CVcxNsEventRouter::HandleEpgManagerMsgL: KIptvErrorEpgUpdateSuccessed");
            HandleEpgUpdateSucceededL( aServiceId );
            }
            break;

        case KIptvErrorEpgUpdateFailed:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "CVcxNsEventRouter::HandleEpgManagerMsgL: KIptvErrorEpgUpdateFailed");
            HandleEpgUpdateFailedL( aInfo, aServiceId );
            }
            break;
        case KIptvErrorRssSearchNoIap: //fall trough
        case KIptvErrorRssSearchStarted: //fall through
        case KIptvErrorRssSearchFailed:  //fall through
        case KIptvErrorRssSearchSucceed: //fall through
        case KIptvRssSearchContentUpdateCompleted: //fall through
        case KIptvRssSearchThumbnailCount: //fall through
        case KIptvRssSearchContentThumbnailDownloaded:
            iUiEngine.GetContentSearchHandlerL()->HandleSearchMsgL( aMsg, aInfo );
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::HandleCollectionMessageL()
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::HandleCollectionMessageL( CMPXMessage* aMsg, TInt /*aErr*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::HandleCollectionMessageL IN");
    // what the err represents?
    
    if( !iUiEngine.ContentProvider() )
        {
        return;
        }
    
    TUid colId = aMsg->ValueTObjectL<TUid>(KMPXMessageCollectionId);
    TMPXChangeEventType type = aMsg->ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType);

    if( colId.iUid == KVcxUidMyVideosMpxCollection )
        {
        TMPXItemId itemId = aMsg->ValueTObjectL<TMPXItemId>( KMPXMessageMediaGeneralId );

        if ( type == EMPXItemModified )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::HandleCollectionMessageL EMPXItemModified");
            CVcxNsContent* content = iUiEngine.GetMpxCollectionClientHandlerL()->SearchFromDownloadList( itemId );            
            if( content )
                {
                iUiEngine.ContentProvider()->UpdateContent( content );
                }
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::HandleCollectionMessageL OUT");
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::UpdateActiveView
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::UpdateActiveView()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::UpdateActiveView");
    
    if( iUiEngine.VcAppState()== EStateServiceView 
            || iUiEngine.VcAppState()== EStateCustomView )
        {
        if ( iUiEngine.ServiceProvider() )
            {
            iUiEngine.ServiceProvider()->RefreshView();
            }
        }
    else if( iUiEngine.VcAppState()== EStateContentView )
        {
        if( iUiEngine.ContentProvider() )
            {
            iUiEngine.ContentProvider()->RefreshView();
            }
        }
    else if( iUiEngine.VcAppState()== EStateCategoryView )
        {
        if( iUiEngine.CategoryProvider() )
            {
            iUiEngine.CategoryProvider()->RefreshView();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::RemoveServiceData()
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::RemoveServiceData( TUint32 aServiceId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::RemoveServiceData");
    
    CVcxNsMpxCollectionClientHandler* collectionClient( NULL );
    
    TRAP_IGNORE( collectionClient = iUiEngine.GetMpxCollectionClientHandlerL() );
    
    if ( collectionClient && iUiEngine.ActiveService() == aServiceId )
        {
        collectionClient->ResetDownloadList( ETrue );
        }
    if ( iUiEngine.ContentProvider() )
        {
        iUiEngine.ContentProvider()->RemoveServiceData( aServiceId );
        }            
    if ( iUiEngine.CategoryProvider() )
        {
        iUiEngine.CategoryProvider()->RemoveServiceData( aServiceId );
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::UpdateInfoPopupL()
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::UpdateInfoPopupL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::UpdateInfoPopupL");
    CVcxNsServiceProvider* serviceProvider = iUiEngine.ServiceProvider();
    if ( !serviceProvider )
        {
        return;
        }

    CVcxNsService* service = serviceProvider->GetServiceDataById( iUpdatedService ); 

    CVcxNsService::TServiceUpdateStatus status ( CVcxNsService::ENotUpdating );
    
    if ( service )
        {
        status = service->GetUpdateStatus();
        }

    TBool updating = ( status == CVcxNsService::EUpdateOngoing ? ETrue : EFalse );

    if( iUiEngine.ActiveService() == iUpdatedService )
        {
        switch( iUiEngine.VcAppState() )
            {
            case EStateContentView:
                {
                if( iUiEngine.ContentProvider() )
                    {
                    iUiEngine.ContentProvider()->ShowUpdatingNoteL( 
                        updating ,iThumbnailCount, iThumbnailsDownloaded );
                    }
                break;
                }
            case EStateCategoryView:
                {
                if( iUiEngine.CategoryProvider() )
                    {
                    iUiEngine.CategoryProvider()->ShowUpdatingNoteL( 
                        updating, iThumbnailCount, iThumbnailsDownloaded );
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::HandleEpgUpdateFailedL()
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::HandleEpgUpdateFailedL( 
    TInt aError,
    TIptvServiceId aServiceId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::HandleEpgUpdateFailedL");
    TVcxNsAppState appState = iUiEngine.VcAppState();
    CVcxNsServiceProvider* serviceProvider = iUiEngine.ServiceProvider();
    
    if ( serviceProvider )
        {
        // Check if Updating manually failed, epg update still pending (EUpdatePending)
        serviceProvider->UpdateServiceUpdateStatusL( 
                aServiceId, CVcxNsService::EUpdateFailed );

        if( appState == EStateServiceView )
            {
            serviceProvider->UpdateService( aServiceId );
            }
        } 

    if ( appState == EStateCategoryView && iUiEngine.CategoryProvider() )
        {
        iUiEngine.CategoryProvider()->HandleErrorL( aError, aServiceId );
        }

    if ( appState == EStateContentView && iUiEngine.ContentProvider() )
        {
        iUiEngine.ContentProvider()->HandleErrorL( aError, ETrue, aServiceId );
        }
    
    if ( appState == EStateCustomView && serviceProvider )
        {
        serviceProvider->RefreshView();
        }

    iThumbnailCount = 0;
    iThumbnailsDownloaded = 0;
    UpdateInfoPopupL();
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::HandleEpgUpdateSucceededL()
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::HandleEpgUpdateSucceededL( TIptvServiceId aServiceId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::HandleEpgUpdateSucceededL");
    
    CheckAppStateL( aServiceId );

    iUiEngine.ServiceProvider()->UpdateServiceUpdateStatusL( 
            aServiceId, CVcxNsService::ENotUpdating );

    iUiEngine.ServiceProvider()->UpdateServiceInCacheL( aServiceId );        
    
    if( iUiEngine.VcAppState() == EStateServiceView )
        {
        // Update just the one item
        iUiEngine.ServiceProvider()->UpdateService( aServiceId );
        }
    else if( ( iUiEngine.VcAppState() == EStateContentView ||                            
          iUiEngine.VcAppState() == EStateCategoryView  ||
          iUiEngine.VcAppState() == EStateCustomView ) &&
          iUiEngine.ActiveService() == aServiceId )
        {
        // Update whole list
        UpdateActiveView();
        }

    iThumbnailCount = 0;
    iThumbnailsDownloaded = 0;
    UpdateInfoPopupL();
    }
                             
// -----------------------------------------------------------------------------
// Check, is the view switch needed
// Note that this removes old service data.
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::CheckAppStateL( TIptvServiceId aServiceId )
    {
    TUint32 activeServiceId = iUiEngine.ActiveService();
    
    if( aServiceId == activeServiceId &&
          iUiEngine.CategoryProvider() &&
        ( iUiEngine.VcAppState() == EStateContentView ||                            
          iUiEngine.VcAppState() == EStateCategoryView ) )
        {
        RPointerArray<CVcxNsCategory> oldCategories = 
            iUiEngine.CategoryProvider()->GetCategorysL();
        TInt oldCategoryCount = oldCategories.Count();
        TBool wasSingle = oldCategoryCount == 0 || 
                          ( oldCategoryCount == 1 && 
                            oldCategories[0]->GetCategoryId() != 
                            KIptvRssSearchCategoryId ) ? ETrue : EFalse ;

        RemoveServiceData( aServiceId );

        RPointerArray<CVcxNsCategory> newCategories = 
            iUiEngine.CategoryProvider()->GetCategorysL();
        TInt newCategoryCount = newCategories.Count();
        TBool isSingle = newCategoryCount == 0 || 
                          ( newCategoryCount == 1 && 
                            newCategories[0]->GetCategoryId() != 
                            KIptvRssSearchCategoryId ) ? ETrue : EFalse ;
        
        
        TVcxNsAppState oldState = iUiEngine.VcAppState();
        // check that the old category really exists
        TBool activeCategoryExists = 
                iUiEngine.CategoryProvider()->GetActiveCategoryDataL()
                    ? ETrue : EFalse;

        // Used only if switching to single category view.
        TUint32 newActiveCategory( newCategoryCount == 1 ?
                                    ( newCategories[0] )->GetCategoryId():
                                    KIptvVodContentCategoryRootId );

        // Categories are increased from single to multiple
        if ( wasSingle && !isSingle &&
            oldState == EStateContentView )
            {
            iUiEngine.PopAppStateStack(); // Back to service level
            iUiEngine.SetVcAppState( EStateCategoryView );
            iUiEngine.ContentProvider()->HandleAppStateChangedL();
            }
        // Categories kept single, check still if category has changed.
        else if ( wasSingle && isSingle &&
            oldState == EStateContentView )
            {
            iUiEngine.SetActiveCategory( newActiveCategory );
            }
        // Category count has dropped from multiple to single, in category view.
        else if ( !wasSingle && isSingle &&
                  oldState == EStateCategoryView )
            {
            iUiEngine.PopAppStateStack(); // Back to service level            
            iUiEngine.SetVcAppState( EStateContentView );
            iUiEngine.SetActiveCategory( newActiveCategory );
            iUiEngine.CategoryProvider()->HandleAppStateChangedL();
            }
        // Category count has dropped from multiple to single, in content view.
        else if ( !wasSingle && isSingle &&
                   oldState == EStateContentView )
            {
            iUiEngine.PopAppStateStack(); // Back to category level
            iUiEngine.PopAppStateStack(); // Back to service level
            iUiEngine.SetActiveCategory( newActiveCategory );
            iUiEngine.SetVcAppState( EStateContentView );
            iUiEngine.ContentProvider()->HandleAppStateChangedL();
            }
        // Category count has not changed from multiple to single.
        else if( !wasSingle && !isSingle &&
                  oldState == EStateContentView && 
                  !activeCategoryExists )
            {
            iUiEngine.HandleBack(); // Back to category level
            iUiEngine.ContentProvider()->HandleAppStateChangedL();
            }
        }
    else
        {
        RemoveServiceData( aServiceId );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsEventRouter::HandleDlErrorL()
// -----------------------------------------------------------------------------
//
void CVcxNsEventRouter::HandleDlErrorL( 
    TIptvDlError aError,
    CVcxNsContent& aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsEventRouter::HandleDlErrorL");
    
    TVcxNsAppState appState = iUiEngine.VcAppState();
    
    if ( appState == EStateContentView && iUiEngine.ContentProvider() )
        {
        iUiEngine.ContentProvider()->HandleDlErrorL( aError, aContent );
        }
    }
