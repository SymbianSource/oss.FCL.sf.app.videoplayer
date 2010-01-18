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
* Description:    HG VOD service list implementation*
*/


// Version : %version: 19 %

#include <akntitle.h>
#include <aknViewAppUi.h>
#include <aknnotewrappers.h>
#include <eikmenup.h> 
#include <StringLoader.h>
#include <vcxhgvodui.rsg>
#include <eikbtgpc.h>
#include <ganes/HgScroller.h>

#include <csxhelp/iptv.hlp.hrh>
#include "vcxhgvodservicelistimpl.h"
#include "vcxhgvodservicelistmodelhandler.h"
#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxnsserviceproviderif.h"
#include "vcxnsservice.h"
#include "vcxhgvodui.hrh"
#include "vcxhgvodutils.h"
#include "vcxnsupdateprogresshandler.h"

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::~CVcxHgVodServiceListImpl()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceListImpl::~CVcxHgVodServiceListImpl()
    {
    //always set model null to widget before destroying it
    delete iModelHandler;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceListImpl* CVcxHgVodServiceListImpl::NewL(
    CHgScroller& aScroller, 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodServiceListImpl* self = 
        CVcxHgVodServiceListImpl::NewLC( 
                  aScroller, aDataProviderIf, aView );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceListImpl* CVcxHgVodServiceListImpl::NewLC(
    CHgScroller& aScroller, 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodServiceListImpl* self = 
        new (ELeave) CVcxHgVodServiceListImpl( 
            aScroller, aDataProviderIf, aView );
    CleanupStack::PushL( self );
    self->ConstructL( aDataProviderIf );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CVcxHgVodServiceListImpl::DoActivateL( )
    {
    // refresh menu
    SetMenuResourceId( R_VCXHG_VOD_SERVICEVIEW_MENUBAR );
    UpdateTitlePaneL();
    SetEmptyListTextL( R_VCXHG_VOD_OPENING_FEEDS );
    SetMskL( R_VCXHG_VOD_MSK_BUTTON_DO_NOTHING );
    
    iScroller.SetSelectionObserver( *this );
    iView.AppUi()->AddToStackL( &iScroller );
    iScroller.SetFocus( ETrue );

    iModelHandler->DoActivateModelL();
    }
// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
void CVcxHgVodServiceListImpl::DoDeactivate( )
    {
    //Make sure that MSK is updated when returning to view
    iCurrentMskResource = 0;
    iMoving = EFalse;
    iModelHandler->DoDeActivateModel();
    iView.AppUi()->RemoveFromStack( &iScroller );
    iScroller.SetFocus( EFalse );
    }

// -----------------------------------------------------------------------------
//  Activate usable menu items before showing menu
// -----------------------------------------------------------------------------
void CVcxHgVodServiceListImpl::ActivateMenuPaneItemsL( 
    CEikMenuPane* aMenuPane,
    TInt aResourceId )
    {
    if ( aMenuPane && 
         ( aResourceId == R_VCXHG_VOD_SERVICEVIEW_MENU ) )
        {
        TInt selected = iModelHandler->GetSelected();
        
        if ( !FeedRemovable( selected ) )
            {
            aMenuPane->SetItemDimmed( EVcxHgVodServiceRemoveCmd, ETrue );
            }
        
        aMenuPane->SetItemDimmed( EVcxHgVodServiceDetailsCmd, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgVodServiceManageAccountCmd, ETrue );
        
        if ( !iModelHandler->IsEmptyModel() )
            {
            if ( iModelHandler->ServiceProvider()->HasAccountManagementUri( selected ) )
                {
                aMenuPane->SetItemDimmed( EVcxHgVodServiceManageAccountCmd, EFalse );
                }
            
            if ( iModelHandler->ServiceProvider()->HasFeedDetails( selected ) )    
                {
                aMenuPane->SetItemDimmed( EVcxHgVodServiceDetailsCmd, EFalse );
                }
            }
        
        if( ! iModelHandler->IsEmptyModel() &&
            ! iModelHandler->LastItemSelected() )
            {
            aMenuPane->SetItemDimmed( EVcxHgVodServiceMoveCmd, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EVcxHgVodServiceMoveCmd, ETrue );
            }

        if ( !HasVideoServicesL() )
            {
            aMenuPane->SetItemDimmed( EVcxHgVodServiceRefreshAllCmd, ETrue );            
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::CVcxHgVodServiceListImpl()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodServiceListImpl::CVcxHgVodServiceListImpl( 
    CHgScroller& aScroller,
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    : CVcxHgVodListImplBase( aView, aDataProviderIf, aScroller )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListImpl::ConstructL( 
    CVcxNsUiEngine& aDataProviderIf )
    {
    iModelHandler = CVcxHgVodServiceListModelHandler::NewL( 
                    aDataProviderIf, iView, iScroller, *this );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::HandleCommandL
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListImpl::HandleCommandL( 
    TInt aCommand )
    {
    if ( !iModelHandler )
        {
        return;
        }

    TInt selected = iModelHandler->GetSelected();
    
    MVcxNsServiceProviderIf* serviceProvider = 
        iModelHandler->ServiceProvider();
    
    User::LeaveIfNull( serviceProvider );
    
    switch ( aCommand )
        {
        case EAknSoftkeyOpen:
        case EVcxHgVodServiceOpenCmd:
            {
            serviceProvider->OpenServiceL( selected );
            }
        break;
        
        case EVcxHgVodServiceRemoveCmd:
            {
            RemoveFeedL( selected );
            }
        break;
        
        case EVcxHgVodServiceRefreshAllCmd:
            {
            serviceProvider->RefreshAllServicesL();
            }
        break;
        
        case EVcxHgVodServiceManageAccountCmd:
            {
            serviceProvider->ManageAccountL( selected );
            }
            break;
        case EVcxHgVodServiceSubscriptionsCmd:
        case EVcxHgVodServiceAddManuallyCmd:
            {
            iView.OpenSettingsL( aCommand );
            }
        break;
        
        case EVcxHgVodServiceAddViaCatalogCmd:
            {
            serviceProvider->OpenVideoDirectoryL();
            iView.ChangeListViewL();
            }
        break;    
    
        case EVcxHgVodServiceDetailsCmd:
            {
            iModelHandler->ShowFeedDetailsL( selected );
            }
        break;
            
        case EVcxHgVodServiceMoveCmd:
            {
            StartMoveL();
            }
        break;

        case EVcxHgVodMovingSoftkeyOk:
            {
            StopMovingL( ETrue );
            }
        break;

        case EVcxHgVodMovingSoftkeyCancel:
            {
            StopMovingL( EFalse );
            }
        break;
        
        default: 
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::Refresh()
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListImpl::Refresh()
    {
    if ( iModelHandler )
        {
        iModelHandler->RefreshView();
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TKeyResponse CVcxHgVodServiceListImpl::HandleKeyEventL( 
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    if( aType == EEventKeyUp && iModelHandler && IsMoving() )
        {
        CheckSoftkeysL();
        }
     
    if ( iModelHandler &&
	     !IsMoving() &&
	     aType == EEventKeyDown &&
         aKeyEvent.iScanCode == EStdKeyBackspace && 
		 FeedRemovable( iModelHandler->GetSelected() ) )
        {
        RemoveFeedL( iModelHandler->GetSelected() );
        return EKeyWasConsumed;
        }
    
    return CVcxHgVodListImplBase::HandleKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CVcxHgVodServiceListImpl::HandleSelectL( TInt /*aIndex*/ )
    {
    if( iModelHandler && IsMoving() )
        {
        CheckSoftkeysL();
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::HandleOpenL
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListImpl::HandleOpenL( TInt /*aIndex*/ )
    {
    if( IsMoving() )
        {
        HandleCommandL( EVcxHgVodMovingSoftkeyOk );
        }
    else
        {
        iView.HandleCommandL( EVcxHgVodServiceOpenCmd );
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CVcxHgVodServiceListImpl::CheckSoftkeysL()
    {
    CEikButtonGroupContainer* cba = iView.ViewCba();

    if( IsMoving() )
        {
        if( !iModelHandler->IsEmptyModel() &&
            !iModelHandler->LastItemSelected() )
            {
            cba->SetCommandSetL( R_VCXHG_VOD_SERVICEVIEW_MOVING_SOFTKEYS );
            }
        else
            {
            cba->SetCommandSetL( R_VCXHG_VOD_SERVICEVIEW_MOVING_SOFTKEYS_OK_DISABLED );
            }     
        }
    else
        {
        cba->SetCommandSetL( R_VCXHG_VOD_SOFTKEYS_OPTIONS_BACK__OPEN );
        
        if( !iModelHandler->IsEmptyModel() )
            {
            SetMskL( R_VCXHG_VOD_MSK_BUTTON_OPEN );
            }
        else
            {
            SetMskL( R_VCXHG_VOD_MSK_BUTTON_DO_NOTHING );
            }
        }
    
    cba->DrawNow();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::FeedRemovable()
// -----------------------------------------------------------------------------
//
TBool CVcxHgVodServiceListImpl::FeedRemovable( TInt aIndex ) 
    {
    if ( iModelHandler && 
            iModelHandler->ServiceProvider() )
        {
        CVcxNsService* service = 
            iModelHandler->ServiceProvider()->GetServiceData( aIndex );

        if ( service
               && ( service->Type() == CVcxNsService::EVcxNsEditable 
                  || service->Type() == CVcxNsService::EVcxNsMainGroup )
               && ( service->GetUpdateStatus() == CVcxNsService::ENotUpdating 
                  || service->GetUpdateStatus() == CVcxNsService::EUpdateFailed ) ) 
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::RemoveService
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListImpl::RemoveFeedL( TInt aIndex )
    {
    if ( iModelHandler && 
            iModelHandler->ServiceProvider() )
        {
        CVcxNsService* service = 
            iModelHandler->ServiceProvider()->GetServiceData( aIndex );
    
        if ( service )
            {
            //Get texts for the message query
            HBufC* dlgText = StringLoader::LoadLC( R_VCXHG_REMOVE_FEED_QUERY_TXT, 
                                                   service->GetName() );
            
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            TInt result = dlg->ExecuteLD( R_VCXHG_VOD_REMOVE_FEED_QUERY, 
                                         *dlgText ); 
            
            CleanupStack::PopAndDestroy( dlgText );
            
            if ( result == EAknSoftkeyYes || result == EAknSoftkeyOk )
                {
                iModelHandler->ServiceProvider()->RemoveServiceL( aIndex );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::IsMoving
// -----------------------------------------------------------------------------
TBool CVcxHgVodServiceListImpl::IsMoving() const
    {
    return iMoving;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::StartMoveL
// -----------------------------------------------------------------------------
void CVcxHgVodServiceListImpl::StartMoveL()
    {
    if ( !iMoving )
        {
        iMoving = ETrue;

        CheckSoftkeysL();

        iSource = iModelHandler->GetSelected();

        iModelHandler->MarkItem( iSource );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::StopMovingL
// -----------------------------------------------------------------------------
void CVcxHgVodServiceListImpl::StopMovingL( TBool aSave )
    {
    if ( !iMoving )
        {
        return;
        }
    
    iMoving = EFalse;

    CheckSoftkeysL();
    
    iModelHandler->UnmarkItem( iSource );

    iTarget = iModelHandler->GetSelected();

    if ( aSave &&
         iTarget != iSource &&
         iSource >= 0 && 
         iTarget >= 0 && 
         iSource < iModelHandler->ItemCount() && 
         iTarget < iModelHandler->ItemCount() && 
         ! iModelHandler->LastItemSelected() )
        {
        if( KErrNone == iModel.GetServiceProviderL()->MoveServiceL( iSource, iTarget ) )
            {
            iModelHandler->MoveItemL( iSource, iTarget );                
            }
        }
    
    if( !aSave )
        {
        iModelHandler->SetSelected( iSource ); 
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::GetHelpContext
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListImpl::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    const TUid KUidIptvMainApp = { 0x102750E2 };
    
    aContext.iMajor   = KUidIptvMainApp;
    aContext.iContext = KIPTV_HLP_VODCAST_MAIN;
    }

// -----------------------------------------------------------------------------
// CVcxNsVodServiceListImpl::UpdateTitlePaneL
// -----------------------------------------------------------------------------
//
void CVcxHgVodServiceListImpl::UpdateTitlePaneL()
    {
    SetTitlePaneTextFromResourceL( R_VCXHG_VIDEO_FEEDS_TITLE );
    }

// -----------------------------------------------------------------------------
// CVcxNsVodServiceListImpl::HasVideoServicesL
// -----------------------------------------------------------------------------
//
TBool CVcxHgVodServiceListImpl::HasVideoServicesL()
    {
    TBool found (EFalse);
    
    if ( iModelHandler && 
            iModelHandler->ServiceProvider() )
        {
        RPointerArray<CVcxNsService>& services = 
		    iModelHandler->ServiceProvider()->GetSubscribedServicesL();
        
        for ( TInt i = 0; i < services.Count(); i++ )
            {
            if ( services[i]->Type() != CVcxNsService::EVcxNsVideoDirectory )
                {
                found = ETrue;
                break;
                }
            }
        }
    
    return found;
    }
