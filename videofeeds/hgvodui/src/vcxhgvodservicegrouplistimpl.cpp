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
* Description:    HG VOD service group list implementation*
*/




#include <akntitle.h>
#include <aknViewAppUi.h>
#include <aknnotewrappers.h>
#include <eikmenup.h> 
#include <StringLoader.h>
#include <vcxhgvodui.rsg>
#include <eikbtgpc.h>
#include <ganes/HgScroller.h>

#include <csxhelp/iptv.hlp.hrh>
#include "vcxhgvodsearchimpl.h"
#include "vcxnsserviceproviderif.h"
#include "vcxhgvodservicegrouplistimpl.h"
#include "vcxhgvodservicegrouplistmodelhandler.h"
#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxnsservice.h"
#include "vcxhgvodui.hrh"
#include "vcxhgvodutils.h"
#include "vcxnsuiengine.h"

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListImpl::~CVcxHgVodServiceGroupListImpl()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceGroupListImpl::~CVcxHgVodServiceGroupListImpl()
    {
    //always set model null to widget before destroying it
    delete iModelHandler;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceGroupListImpl* CVcxHgVodServiceGroupListImpl::NewL(
    CHgScroller& aScroller, 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodServiceGroupListImpl* self = 
        CVcxHgVodServiceGroupListImpl::NewLC( 
                  aScroller, aDataProviderIf, aView );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListImpl::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceGroupListImpl* CVcxHgVodServiceGroupListImpl::NewLC(
    CHgScroller& aScroller, 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodServiceGroupListImpl* self = 
        new (ELeave) CVcxHgVodServiceGroupListImpl( 
            aScroller, aDataProviderIf, aView );
    CleanupStack::PushL( self );
    self->ConstructL( aDataProviderIf );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CVcxHgVodServiceGroupListImpl::DoActivateL( )
    {
    if ( iSearchImpl )
        {
        iSearchImpl->RegisterObserver();
        }

    // refresh menu
    SetMenuResourceId( R_VCXHG_VOD_SERVICEGROUPVIEW_MENUBAR );
    SetEmptyListTextL( R_VCXHG_VOD_OPENING_FEEDS );
    SetMskL( R_VCXHG_VOD_MSK_BUTTON_DO_NOTHING );
    
    UpdateTitlePaneL();

    iScroller.SetSelectionObserver( *this );
    iView.AppUi()->AddToStackL( &iScroller );
    iScroller.SetFocus( ETrue );
    
    iModelHandler->DoActivateModelL();
    }
// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
void CVcxHgVodServiceGroupListImpl::DoDeactivate( )
    {
    //Make sure that MSK is updated when returning to view
    iCurrentMskResource = 0;
    iModelHandler->DoDeActivateModel();
    iView.AppUi()->RemoveFromStack( &iScroller );
    iScroller.SetFocus( EFalse );
    
    if ( iSearchImpl )
        {
        iSearchImpl->DeRegisterObserver();
        }
    }

// -----------------------------------------------------------------------------
//  Activate usable menu items before showing menu
// -----------------------------------------------------------------------------
void CVcxHgVodServiceGroupListImpl::ActivateMenuPaneItemsL( 
    CEikMenuPane* aMenuPane,
    TInt aResourceId )
    {
    if ( aMenuPane &&
         ( aResourceId == R_VCXHG_VOD_SERVICEGROUPVIEW_MENU ) )
        {
        if ( iModelHandler && iModelHandler->IsEmptyModel() )
            {
            aMenuPane->SetItemDimmed( EVcxHgVodServiceGroupOpenCmd, ETrue );
            } 
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListImpl::CVcxHgVodServiceGroupListImpl()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceGroupListImpl::CVcxHgVodServiceGroupListImpl( 
    CHgScroller& aScroller,
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    : CVcxHgVodListImplBase( aView, aDataProviderIf, aScroller )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListImpl::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceGroupListImpl::ConstructL( 
    CVcxNsUiEngine& aDataProviderIf )
    {
    iModelHandler = CVcxHgVodServiceGroupListModelHandler::NewL( 
                    aDataProviderIf, iView, iScroller, *this );
    
    //Initialize search implementation in base class.
    CVcxHgVodListImplBase::InitSearchImplL();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListImpl::HandleCommandL
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceGroupListImpl::HandleCommandL( 
    TInt aCommand )
    {
    TInt selected = iModelHandler->GetSelected();
    
    MVcxNsServiceProviderIf* serviceProvider = 
       iModel.GetServiceProviderL();
    
    switch ( aCommand )
        {
        case EAknSoftkeyOpen:
        case EVcxHgVodServiceGroupOpenCmd:
            {
            serviceProvider->OpenServiceL( selected );
            }
        break;
        
        default: 
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListImpl::HandleOpenL
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceGroupListImpl::HandleOpenL( TInt /*aIndex*/ )
    {
    iView.HandleCommandL( EVcxHgVodServiceGroupOpenCmd );
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CVcxHgVodServiceGroupListImpl::CheckSoftkeysL()
    {
    CEikButtonGroupContainer* cba = iView.ViewCba();

    cba->SetCommandSetL( R_VCXHG_VOD_SOFTKEYS_OPTIONS_BACK__OPEN );
    
    if( !iModelHandler->IsEmptyModel() )
        {
        SetMskL( R_VCXHG_VOD_MSK_BUTTON_OPEN );
        }
    else
        {
        SetMskL( R_VCXHG_VOD_MSK_BUTTON_DO_NOTHING );
        }
    
    cba->DrawNow();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceGroupListImpl::GetHelpContext
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceGroupListImpl::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    const TUid KUidIptvMainApp = { 0x102750E2 };
    
    aContext.iMajor   = KUidIptvMainApp;
    aContext.iContext = KIPTV_HLP_VOD_GROUPING;
    }

// -----------------------------------------------------------------------------
// CVcxNsVodServiceGroupListImpl::UpdateTitlePaneL
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceGroupListImpl::UpdateTitlePaneL()
    {
    CVcxNsService* currentService = 
        iModel.GetServiceProviderL()->GetActiveServiceData();
    if ( currentService )
        {
        SetTitlePaneTextL( currentService->GetName() );
        }   
    }
