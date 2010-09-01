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
* Description:    HG VOD*
*/




#include <aknmessagequerydialog.h>
#include <StringLoader.h>
#include <vcxhgvodui.rsg>
#include <ganes/HgScroller.h>
#include <ganes/HgItem.h>
#include <ganes/HgDoubleGraphicList.h>

#include "vcxnsserviceproviderif.h"
#include "vcxhgvodservicelistmodelhandler.h"
#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodui.hrh"
#include "vcxhgvodutils.h"
#include "vcxhgvodthumbnailmanager.h"
#include "vcxnsuiengine.h"
#include "vcxnsservice.h"
#include "vcxhgvodservicelistimpl.h"
#include "IptvDebug.h"

TInt FindServiceIndex( TUint32 aId, RPointerArray<CVcxNsService>* aArray )
    {
    TInt index( KErrNotFound );
    for ( TInt i = 0; i < aArray->Count(); i++ )
        {
        if ( (*aArray)[i]->GetServiceId() == aId )
            {
            index = i;
            break;
            }
        }
    
    return index;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListModelHandler::~CVcxHgVodServiceListModelHandler()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceListModelHandler::~CVcxHgVodServiceListModelHandler()
    {
    if ( iServiceProvider )
        {
        iServiceProvider->DeRegisterObserver( this );
        }
    
    delete iTnManager;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceListModelHandler* CVcxHgVodServiceListModelHandler::NewL( 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodServiceListImpl& aServiceListImpl )
    {
    CVcxHgVodServiceListModelHandler* self = 
        CVcxHgVodServiceListModelHandler::NewLC(  
                aDataProviderIf, 
                aView,
                aScroller,
                aServiceListImpl );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListModelHandler::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceListModelHandler* CVcxHgVodServiceListModelHandler::NewLC(
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodServiceListImpl& aServiceListImpl )
    {
    CVcxHgVodServiceListModelHandler* self = 
        new (ELeave) CVcxHgVodServiceListModelHandler(  
                aDataProviderIf,
                aView,
                aScroller, 
                aServiceListImpl );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListModelHandler::CVcxHgVodServiceListModelHandler()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceListModelHandler::CVcxHgVodServiceListModelHandler( 
    CVcxNsUiEngine& aDataProviderIf, 
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodServiceListImpl& aServiceListImpl )
    : iDataProviderIf ( aDataProviderIf ),
      iScroller ( aScroller ),
      iView ( aView ),
      iServiceListImpl ( aServiceListImpl )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListModelHandler::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListModelHandler::ConstructL()
    {
    iServiceProvider = iDataProviderIf.GetServiceProviderL();
    iProgressHandler = iDataProviderIf.ProgressHandlerL();
    iTnManager = CVcxHgVodThumbnailManager::NewL( iView.EikonEnv().FsSession() );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::ServiceAdded( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::ServiceAdded");
    
    SetSelected( aIndex );

    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::ServiceAdded");
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::ServiceUpdated( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::ServiceUpdated");
    
    TRAP_IGNORE( UpdateListItemL( aIndex ) );
	            
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::ServiceUpdated");
    }

// -----------------------------------------------------------------------------
// Refresh view, update all services.
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::RefreshView()
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::RefreshView");
    
    if ( iDataProviderIf.VcAppState() == EStateServiceView )
        {
        TRAP_IGNORE( iServices = &(iServiceProvider->GetSubscribedServicesL()) );
        
        TRAP_IGNORE( RefreshViewL() );

        TRAP_IGNORE( iServiceListImpl.CheckSoftkeysL() );
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::RefreshView");
    }

// -----------------------------------------------------------------------------
// Application state change initialized by data provider.
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::HandleAppStateChangedL()
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::HandleAppStateChangedL");
    iView.ChangeListViewL();
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::HandleAppStateChangedL");
    }

// -----------------------------------------------------------------------------
// Open defined browser link
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::OpenBrowserLinkL( const TDesC& aUri )
    {
    iServiceListImpl.OpenEmbeddedBrowserL( aUri );
    }

// -----------------------------------------------------------------------------
// Open player for streaming ( no implementation )
// -----------------------------------------------------------------------------
// 
    void CVcxHgVodServiceListModelHandler::OpenStreamingLinkL( const TDesC& /*aUri*/ )
    {
    }

// -----------------------------------------------------------------------------
// Service removed
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::ServiceRemoved( 
    TInt aIndex, 
    RPointerArray<CVcxNsService>& aUpdatedServiceList )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::ServiceRemoved");
    
    if ( iScroller.ItemCount() > 0 
            && iScroller.ItemCount() > aIndex )
        {
        iScroller.RemoveItem( aIndex );
        iScroller.RefreshScreen( aIndex );
        }
    iServices = NULL;
    iServices = &aUpdatedServiceList;
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::ServiceRemoved");
    }

// -----------------------------------------------------------------------------
// Activate model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::DoActivateModelL( )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::DoActivateModelL");
    
    iServiceProvider->RegisterObserver( this );
    
    if ( iProgressHandler )
        {
        iProgressHandler->RegisterObserver( this );
        }
    
    iScroller.EnableScrollBufferL( *this, KScrollerBufferSize, KScrollerBufferTreshold ); 
    iScroller.Reset();
    RefreshView();
    
    iScroller.SetSelectedIndex( iHighlight );
    iScroller.ControlEnv()->WsSession().ClearAllRedrawStores();
    iScroller.MakeVisible( ETrue );
    iScroller.DrawDeferred();
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::DoActivateModelL");
    }

// -----------------------------------------------------------------------------
// Deactivate model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::DoDeActivateModel( )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::DoDeActivateModel");
    
    if ( iServiceProvider )
        {
        iServiceProvider->DeRegisterObserver( this );
        }

    if ( iProgressHandler )
        {
        iProgressHandler->DeRegisterObserver( this );
        }
    
    iTnManager->Reset();
    
    iHighlight = iScroller.SelectedIndex();
    iScroller.DisableScrollBuffer();
    iScroller.MakeVisible( EFalse );
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::DoDeActivateModel");
    }

// -----------------------------------------------------------------------------
// Get pointer to service provider 
// -----------------------------------------------------------------------------
// 
MVcxNsServiceProviderIf* CVcxHgVodServiceListModelHandler::ServiceProvider() 
    {
    return iServiceProvider;   
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
// 
TInt CVcxHgVodServiceListModelHandler::GetSelected() 
    {
    return iScroller.SelectedIndex();
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::SetSelected( TInt aIndex )
    {
    if( aIndex >= 0 && iScroller.ItemCount() > aIndex )
        {
        iScroller.SetSelectedIndex( aIndex ); 
        }
    }

// ---------------------------------------------------------
// CVcxHgVodServiceListModelHandler::IsEmptyModel
// ---------------------------------------------------------
//
TBool CVcxHgVodServiceListModelHandler::IsEmptyModel()
    {
    return iScroller.ItemCount() <= 0;
    }

// ---------------------------------------------------------
// CVcxHgVodServiceListModelHandler::GenerateServiceInfoTextLC
// ---------------------------------------------------------
//
HBufC* CVcxHgVodServiceListModelHandler::GenerateServiceInfoTextLC( 
    CVcxNsService::TServiceUpdateStatus aUpdateStatus,
    TTime aUpdateTime )
    {    
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::GenerateServiceInfoTextLC");
    HBufC* serviceInfo;
    
    if ( aUpdateStatus == CVcxNsService::EUpdateFailed )
        {
        serviceInfo = CVcxHgVodUtils::LoadStringFromResourceLC( R_VCXHG_FEED_UPDATE_FAILED );
        }
    else if ( aUpdateTime == 0 )
        {
        serviceInfo = KVcxNsSpaceString().AllocLC();
        }
    else
        {
        TBuf<KVcxNsMaxDateBufSize + KVcxNsMaxTimeBufSize + 1> datetimebuf;
        CVcxHgVodUtils::FormatDateAndTimeL( aUpdateTime, datetimebuf );

        serviceInfo = CVcxHgVodUtils::LoadStringFromResourceLC( R_VCXHG_LAST_UPDATE, datetimebuf );
        }
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::GenerateServiceInfoTextLC");
    return serviceInfo;    
    }

// -----------------------------------------------------------------------------
// Show selected feed's details
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::ShowFeedDetailsL( TInt aSelected )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::ShowFeedDetailsL");
    
    if ( aSelected >= 0 && aSelected < iServices->Count() )
        {
        if ( iServiceProvider->GetServiceData( aSelected )->GetDesc().Length() > 0 )
            {
            const TDesC& feedDesc = iServiceProvider->GetServiceData( aSelected )->GetDesc();
            HBufC* context = HBufC::NewLC( feedDesc.Length() );
            context->Des().Append( feedDesc );
            
            CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *context );
            dlg->ExecuteLD( R_VCXHGVOD_FEED_DETAILS_MESSAGE_QUERY );
            
            if ( context )
                {
                CleanupStack::PopAndDestroy( context );
                }
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::ShowFeedDetailsL");
    }

// -----------------------------------------------------------------------------
// Update progress
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::HandleFeedUpdateProgressL( 
    TInt aIndex, 
    TInt aProgress )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxHgVodServiceListModelHandler::HandleFeedUpdateProgressL");
    
    if ( aIndex < iScroller.ItemCount() 
          && aIndex < iServices->Count() 
          && aIndex >= 0 )
        {
        // need to add progress, when loc string is available. 
        // For now only remove the compiler warning
        aProgress = aProgress;
        HBufC* info = CVcxHgVodUtils::LoadStringFromResourceLC( R_VCXHG_UPDATING_FEED );
        
        if ( info->Length() > 0 )
            {
            CHgItem& item = iScroller.ItemL( aIndex );
            item.SetTextL( *info );
            iScroller.RefreshScreen( aIndex );
            }
        CleanupStack::PopAndDestroy( info );
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxHgVodServiceListModelHandler::HandleFeedUpdateProgressL");
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::MarkItem( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iScroller.ItemCount() )
        {
        TRAP_IGNORE( 
                {
                CHgItem& item = iScroller.ItemL( aIndex ); 
                item.SetFlags( CHgItem::EHgItemFlagMarked );
                iScroller.RefreshScreen( aIndex );
                } );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::UnmarkItem( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iScroller.ItemCount() )
        {
        TRAP_IGNORE( 
                {
                CHgItem& item = iScroller.ItemL( aIndex ); 
                item.ClearFlags( CHgItem::EHgItemFlagMarked );
                iScroller.RefreshScreen( aIndex );
                } );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::MoveItemL( TInt aSourceIndex, TInt aTargetIndex )
    {
    if ( aSourceIndex == aTargetIndex ||
         aSourceIndex < 0 || 
         aTargetIndex < 0 ||
         aSourceIndex >= iScroller.ItemCount() ||
         aTargetIndex >= iScroller.ItemCount() )
        {
        return;
        }
    
    CHgItem& oldItem = iScroller.ItemL( aSourceIndex );
    CGulIcon* newIcon = NULL;
    
    if ( oldItem.Icon() )
        {
        newIcon = CVcxHgVodUtils::DuplicateGulIconL( oldItem.Icon() );
        }
    
    CHgItem* newItem = CHgItem::NewL( oldItem.Flags(), newIcon, 
            oldItem.Title(), oldItem.Text() );
    
    newItem->SetTime( oldItem.Time() );

    iScroller.RemoveItem( aSourceIndex );
    iScroller.InsertItem( newItem, aTargetIndex );
    iScroller.DrawDeferred();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
TInt CVcxHgVodServiceListModelHandler::ItemCount()
    {
    return iScroller.ItemCount();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
TInt CVcxHgVodServiceListModelHandler::LastItemSelected()
    {
    return iScroller.SelectedIndex() == iScroller.ItemCount() - 1;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::MvtoThumbnailReady( 
    CGulIcon* aIcon,
    TInt64 aClientId )
    {
    // error ignored, as there is no error handling that can be done if this leaves
    TRAP_IGNORE( DoThumbnailReadyL( aIcon, aClientId ) );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::RefreshViewL()
    {
    TInt highlight = iScroller.SelectedIndex();
    
    iScroller.Reset();
    
    if ( iServices && iServices->Count() > 0 )
        {
        iScroller.ResizeL( iServices->Count() );
        
        if ( highlight < 0 )
            {
            highlight = 0;
            }
        else if ( highlight >= iScroller.ItemCount() )
            {
            highlight = iScroller.ItemCount() - 1;
            }

        iScroller.SetSelectedIndex( highlight );
        }
    
    iScroller.DrawDeferred();
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListModelHandler::Request( TInt aBufferStart, 
    TInt aBufferEnd, THgScrollDirection /*aDirection*/ )
    {
    for ( TInt i = aBufferStart; i <= aBufferEnd; i++ )
        {
        TRAP_IGNORE( UpdateListItemL( i ) );
        }
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListModelHandler::Release( TInt /*aBufferStart*/, 
    TInt /*aBufferEnd*/ )
    {
    // No implementation required.
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::UpdateListItemL( TInt aIndex )
    {
    if ( iDataProviderIf.VcAppState() == EStateServiceView &&
         aIndex >= 0 && iServices && iServices->Count() > aIndex &&
         iScroller.ItemCount() > aIndex )
        {
        CHgItem& item = iScroller.ItemL( aIndex ); 
        CVcxNsService& service = *(*iServices)[ aIndex ];
        
        HBufC* name = NULL;
        HBufC* info = NULL;

        CVcxNsService::TServiceUpdateStatus updateStatus = service.GetUpdateStatus();
    
        if ( service.GetName().CompareF( KVcxNsVideoDirectory ) == 0 )
            {
            name = CVcxHgVodUtils::LoadStringFromResourceLC( R_VCXHG_VOD_FEEDS_VIDEO_DIRECTORY );
            item.SetIcon( NULL );
            }
        else
            {
            name = service.GetName().AllocLC();

            const TDesC& iconPath = service.GetIconPath();

            if ( updateStatus != CVcxNsService::EUpdateOngoing || !( item.Icon() ) )
                {
                item.SetIcon( NULL );
                if ( iconPath.Length() > 0 )
                    {
                    iTnManager->AddThumbnailRequestL( *this, 
                                                      service.GetIconPath(), 
                                                      CHgDoubleGraphicList::PreferredImageSize(), 
                                                      service.GetServiceId() );
                    }
                }
            }
    
        item.SetTitleL( *name );
        CleanupStack::PopAndDestroy( name );
    
        if ( updateStatus == CVcxNsService::EUpdateOngoing )
            {
            TInt progress( 0 );
        
            if ( iProgressHandler )
                {
                progress = iProgressHandler->GetProgress( service.GetServiceId() );
                }
        
            // need to add progress here, when loc string is available.
            // For now just remove the compiler warning
            progress = progress;
            info = CVcxHgVodUtils::LoadStringFromResourceLC( R_VCXHG_UPDATING_FEED );
            }
        else
            {
            info = GenerateServiceInfoTextLC( updateStatus, service.GetUpdateTime() );
            }
    
        if ( info->Length() > 0 )
            {
            // Set detail ( 2nd line text )
            item.SetTextL( *info );
            }
        CleanupStack::PopAndDestroy( info );
        
        iScroller.RefreshScreen( aIndex );
        }
    
    iServiceListImpl.SetMskL( R_VCXHG_VOD_MSK_BUTTON_OPEN );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceListModelHandler::DoThumbnailReadyL( 
    CGulIcon* aIcon, 
    TInt64 aClientId )
    {
    CleanupStack::PushL( aIcon );
    TInt index = FindServiceIndex( (TUint32)aClientId, iServices );
    
    if ( index >= 0 && iScroller.ItemCount() > index )
        {
        iScroller.ItemL( index ).SetIcon( aIcon );
        iScroller.RefreshScreen( index );
        CleanupStack::Pop( aIcon );
        }
    else
        {
        // For some reason the scroller list was update after image conversion
        // started, and the icon is no longer needed.
        CleanupStack::PopAndDestroy( aIcon );
        }
    }
