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
#include "vcxnscategoryproviderif.h"
#include "vcxhgvodservicegrouplistimpl.h"
#include "vcxhgvodservicegrouplistmodelhandler.h"
#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodui.hrh"
#include "vcxhgvodutils.h"
#include "vcxnsuiengine.h"
#include "vcxnsservice.h"
#include "vcxnscategory.h"
#include "vcxhgvodthumbnailmanager.h"

TInt FindServiceGroupIndex( TUint32 aId, RPointerArray<CVcxNsService>* aArray )
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
CVcxHgVodServiceGroupListModelHandler::~CVcxHgVodServiceGroupListModelHandler()
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
CVcxHgVodServiceGroupListModelHandler* CVcxHgVodServiceGroupListModelHandler::NewL( 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodServiceGroupListImpl& aServiceGroupListImpl )
    {
    CVcxHgVodServiceGroupListModelHandler* self = 
        CVcxHgVodServiceGroupListModelHandler::NewLC(  
                aDataProviderIf, 
                aView,
                aScroller,
                aServiceGroupListImpl );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListModelHandler::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceGroupListModelHandler* CVcxHgVodServiceGroupListModelHandler::NewLC(
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodServiceGroupListImpl& aServiceGroupListImpl )
    {
    CVcxHgVodServiceGroupListModelHandler* self = 
        new (ELeave) CVcxHgVodServiceGroupListModelHandler(  
                aDataProviderIf,
                aView,
                aScroller, 
                aServiceGroupListImpl );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListModelHandler::CVcxHgVodServiceGroupListModelHandler()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceGroupListModelHandler::CVcxHgVodServiceGroupListModelHandler( 
    CVcxNsUiEngine& aDataProviderIf, 
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodServiceGroupListImpl& aServiceGroupListImpl )
    : iDataProviderIf ( aDataProviderIf ),
      iScroller ( aScroller ),
      iView ( aView ),
      iListImpl ( aServiceGroupListImpl )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListModelHandler::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceGroupListModelHandler::ConstructL()
    {
    iServiceProvider = iDataProviderIf.GetServiceProviderL();
    iTnManager = CVcxHgVodThumbnailManager::NewL( iView.EikonEnv().FsSession() );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::ServiceAdded( TInt /*aIndex*/ )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::ServiceUpdated( TInt aIndex )
    {
    TRAP_IGNORE( UpdateListItemL( aIndex ) );
    }

// -----------------------------------------------------------------------------
// Refresh view, update all services.
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::RefreshView()
    {
    if ( iDataProviderIf.VcAppState() == EStateCustomView )
        {
        TRAPD( err, iGroupServices = &(iServiceProvider->GetActiveGroupListL()) );
        
        if ( err == KErrNone )
            {
            TInt highlight = iScroller.SelectedIndex();
            
            TRAP_IGNORE( RefreshViewL() );
            
            if ( iScroller.ItemCount() > 0 )
                {
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
            }
        }
    }

// -----------------------------------------------------------------------------
// Handle service removal (no implementation)
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::ServiceRemoved( 
    TInt /*aIndex*/, 
    RPointerArray<CVcxNsService>& /*aUpdatedServiceList*/ )
    {
    }

// -----------------------------------------------------------------------------
// Application state change initialized by data provider.
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::HandleAppStateChangedL()
    {
    iView.ChangeListViewL();
    }

// -----------------------------------------------------------------------------
// Open defined browser link
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::OpenBrowserLinkL( const TDesC& aUri )
    {
    iListImpl.OpenEmbeddedBrowserL( aUri );
    }

// -----------------------------------------------------------------------------
// open streaming link
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::OpenStreamingLinkL( const TDesC& aUri )
    {
    iView.PlayStreamL( aUri );
    }

// -----------------------------------------------------------------------------
// Activate model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::DoActivateModelL( )
    {
    iServiceProvider->RegisterObserver( this );
    
    iGroupServices = &(iServiceProvider->GetActiveGroupListL());
    iScroller.EnableScrollBufferL( *this, KScrollerBufferSize, KScrollerBufferTreshold );
    iScroller.Reset();
    RefreshViewL();
    
    iScroller.SetSelectedIndex( iHighlight );
    iScroller.ControlEnv()->WsSession().ClearAllRedrawStores();
    iScroller.MakeVisible( ETrue );
    iScroller.DrawDeferred();
    }

// -----------------------------------------------------------------------------
// Deactivate model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::DoDeActivateModel( )
    {
    if ( iServiceProvider )
        {
        iServiceProvider->DeRegisterObserver( this );
        }
    
    iTnManager->Reset();
    
    iHighlight = iScroller.SelectedIndex();
    iScroller.DisableScrollBuffer();
    iScroller.MakeVisible( EFalse );
    }

// -----------------------------------------------------------------------------
// Get highlight index
// -----------------------------------------------------------------------------
// 
TInt CVcxHgVodServiceGroupListModelHandler::GetSelected() 
    {
    return iScroller.SelectedIndex();
    }

// ---------------------------------------------------------
// CVcxHgVodServiceGroupListModelHandler::IsEmptyModel
// ---------------------------------------------------------
//
TBool CVcxHgVodServiceGroupListModelHandler::IsEmptyModel()
    {
    return iScroller.ItemCount() <= 0;
    }

// ---------------------------------------------------------
// CVcxHgVodServiceGroupListModelHandler::GenerateInfoTextLC
// ---------------------------------------------------------
//
HBufC* CVcxHgVodServiceGroupListModelHandler::GenerateInfoTextLC( 
    CVcxNsService& aGroupService )
    {
    if ( aGroupService.Type() == CVcxNsService::EVcxNsGroupedSearch )
        {
        MVcxNsCategoryProviderIf* categoryProvider = 
            iDataProviderIf.GetCategoryProviderL();
        
        CVcxNsCategory* category = NULL;
        if ( categoryProvider )
            {
            category = categoryProvider->GetCategoryDataL( 
                           aGroupService.GetServiceId(), 
                           KIptvRssSearchCategoryId );
            }
        
        TInt count = 0;
        if ( category ) 
            {
            count = category->GetVideoCount();
            }

        if ( count == 1 )
            {
            return CVcxHgVodUtils::LoadStringFromResourceLC( 
                    R_VCXHG_SEARCH_VIDEOS_FOUND_ONE );
            }
        else if ( count > 1 ) 
            {
            return CVcxHgVodUtils::LoadStringFromResourceLC( 
                    R_VCXHG_SEARCH_VIDEOS_FOUND, count );            
            }
        }
    
    return aGroupService.GetDesc().AllocLC();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
TInt CVcxHgVodServiceGroupListModelHandler::ItemCount()
    {
    return iScroller.ItemCount();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::MvtoThumbnailReady( 
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
void CVcxHgVodServiceGroupListModelHandler::RefreshViewL()
    {
    iScroller.Reset();
    
    if ( iGroupServices && iGroupServices->Count() > 0 )
        {
        iScroller.ResizeL( iGroupServices->Count() );
        }
    else
        {
        CVcxNsService* service = iServiceProvider->GetActiveServiceData();
        if ( service && 
             service->Type() == CVcxNsService::EVcxNsMainGroup && 
             service->GetUpdateStatus() == CVcxNsService::EUpdateFailed )
            {
            //Does this need empty list text? (not specified in ui spec)
            //There's no text available, so if update failed, return 
            //to video feeds view. 
            iView.HandleCommandL( EAknSoftkeyBack );
            }

        iListImpl.CheckSoftkeysL();
        }
    
    iScroller.DrawDeferred();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::UpdateListItemL( TInt aIndex ) 
    
    {
    if ( iDataProviderIf.VcAppState() == EStateCustomView &&
         aIndex >= 0 && aIndex < iScroller.ItemCount() && 
         iGroupServices && iGroupServices->Count() > aIndex )    
        {
        CHgItem& item = iScroller.ItemL( aIndex ); 
        CVcxNsService& service = *(*iGroupServices)[aIndex];
    
        item.SetTitleL( service.GetName() );
    
        HBufC* info = GenerateInfoTextLC( service );
    
        item.SetTextL( *info );
        CleanupStack::PopAndDestroy( info );
    
        item.SetIcon( NULL );
    
        const TDesC& iconPath = service.GetIconPath();
        if ( iconPath.Length() > 0 )
            {
            iTnManager->AddThumbnailRequestL( *this,
                                              service.GetIconPath(), 
                                              CHgDoubleGraphicList::PreferredImageSize(), 
                                              service.GetServiceId() );
            }
        iScroller.RefreshScreen( aIndex );
        }
    
    iListImpl.SetMskL( R_VCXHG_VOD_MSK_BUTTON_OPEN );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodServiceGroupListModelHandler::DoThumbnailReadyL( 
    CGulIcon* aIcon, 
    TInt64 aClientId )
    {
    CleanupStack::PushL( aIcon );
    TInt index = FindServiceGroupIndex( (TUint32)aClientId, iGroupServices );
    
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

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceGroupListModelHandler::Request( TInt aBufferStart, 
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
void CVcxHgVodServiceGroupListModelHandler::Release( TInt /*aBufferStart*/, 
    TInt /*aBufferEnd*/ )
    {
    // No implementation required.
    }
