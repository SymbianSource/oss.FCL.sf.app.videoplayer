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
* Description:    HG VOD category list implementation*
*/




#include <akntitle.h>
#include <aknViewAppUi.h>
#include <eikmenup.h> 
#include <StringLoader.h>
#include <ganes/HgScroller.h>
#include <vcxhgvodui.rsg>

#include <csxhelp/iptv.hlp.hrh>
#include "vcxhgvodcategorylistimpl.h"
#include "vcxhgvodcategorylistmodelhandler.h"
#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxnsserviceproviderif.h"
#include "vcxnscategoryproviderif.h"
#include "vcxnsuiengine.h"
#include "vcxnsservice.h"
#include "vcxhgvodui.hrh"
#include "vcxhgvodutils.h"
#include "vcxhgvodlistimplbase.h"
#include "vcxhgvodsearchimpl.h"

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListImpl::~CVcxHgVodCategoryListImpl()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodCategoryListImpl::~CVcxHgVodCategoryListImpl()
    {
    //always set model null to widget before destroying it
    delete iModelHandler;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgVodCategoryListImpl* CVcxHgVodCategoryListImpl::NewL(
    CHgScroller& aScroller, 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodCategoryListImpl* self = 
        CVcxHgVodCategoryListImpl::NewLC( aScroller, aDataProviderIf, aView );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListImpl::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodCategoryListImpl* CVcxHgVodCategoryListImpl::NewLC(
    CHgScroller& aScroller, 
    CVcxNsUiEngine& aDataProviderIf, 
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodCategoryListImpl* self = 
        new (ELeave) CVcxHgVodCategoryListImpl( aScroller, aDataProviderIf, aView );
    CleanupStack::PushL( self );
    self->ConstructL( aDataProviderIf, aView );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CVcxHgVodCategoryListImpl::DoActivateL( )
    {
    if ( iSearchImpl )
        {
        iSearchImpl->RegisterObserver();
        }
    
    SetMenuResourceId( R_VCXHG_VOD_CATEGORYVIEW_MENUBAR );
    SetEmptyListTextL( R_VCXHG_VOD_UPDATING_ITEMS );
    SetMskL( R_VCXHG_VOD_MSK_BUTTON_OPEN );
        
    iScroller.SetSelectionObserver( *this );
    iView.AppUi()->AddToStackL( &iScroller );
    iScroller.SetFocus( ETrue );
    
    iModelHandler->DoActivateModelL();

    UpdateTitlePaneL();

    //Set empty list text in case that is shown if list is empty
    if ( iScroller.ItemCount() == 0 )
        {
        SetEmptyListTextL( R_VCXHG_VOD_NO_DATA );
        }
    if( iModel.GetServiceProviderL()->GetActiveServiceData()->
            GetUpdateStatus() == CVcxNsService::EUpdateOngoing )
        {
        ShowInformationPopupL( ETrue, 0, 0 );
        }
    }    

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
void CVcxHgVodCategoryListImpl::DoDeactivate( )
    {
    //Make sure that MSK is updated when returning to view
    iCurrentMskResource = 0;
    //Always hide when deactivating view
    iModelHandler->DoDeActivateModel();
    iView.AppUi()->RemoveFromStack( &iScroller );
    iScroller.SetFocus( EFalse );
    
    TRAP_IGNORE( ShowInformationPopupL( EFalse, 0, 0 ) );
    
    if ( iSearchImpl )
        {
        iSearchImpl->DeRegisterObserver();
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListImpl::CVcxHgVodCategoryListImpl()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodCategoryListImpl::CVcxHgVodCategoryListImpl( 
    CHgScroller& aScroller,
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    : CVcxHgVodListImplBase( aView, aDataProviderIf, aScroller )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListImpl::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodCategoryListImpl::ConstructL( 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    iModelHandler = 
        CVcxHgVodCategoryListModelHandler::NewL( 
                aDataProviderIf, aView, iScroller, *this );

    //Initialize search implementation in base class.
    CVcxHgVodListImplBase::InitSearchImplL();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListImpl::HandleCommandL
// -----------------------------------------------------------------------------
//
void CVcxHgVodCategoryListImpl::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyOpen:
        case EVcxHgVodCategoryOpenCmd: 
            {
            if ( iModelHandler && iModelHandler->CategoryProvider() ) 
                {
                TInt selected = iScroller.SelectedIndex();
                iModelHandler->CategoryProvider()->OpenCategoryL( selected );
                }
            }
        break;
        case EVcxHgVodCategoryScheduleDownloadsCmd:
            {
            iView.OpenScheduleSettingsL();
            }
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
//  Activate usable menu items before showing menu
// -----------------------------------------------------------------------------
void CVcxHgVodCategoryListImpl::ActivateMenuPaneItemsL( 
    CEikMenuPane* /*aMenuPane*/,
    TInt /*aResourceId*/ )
    {
    // No implementation needed
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListImpl::HandleOpenL
// -----------------------------------------------------------------------------
//
void CVcxHgVodCategoryListImpl::HandleOpenL( TInt /*aIndex*/ )
    {
    iView.HandleCommandL( EVcxHgVodCategoryOpenCmd );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListImpl::GetHelpContext
// -----------------------------------------------------------------------------
//
void CVcxHgVodCategoryListImpl::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    const TUid KUidIptvMainApp = { 0x102750E2 };
    
    aContext.iMajor   = KUidIptvMainApp;
    aContext.iContext = KIPTV_HLP_VOD_CATEGORIES;
    }

// -----------------------------------------------------------------------------
// CVcxNsVodCategoryListImpl::UpdateTitlePaneL
// -----------------------------------------------------------------------------
//
void CVcxHgVodCategoryListImpl::UpdateTitlePaneL()
    {
    CVcxNsService* currentService = 
    iModel.GetServiceProviderL()->GetActiveServiceData();
    if ( currentService )
        {
        SetTitlePaneTextL( currentService->GetName() );
        }
    }
