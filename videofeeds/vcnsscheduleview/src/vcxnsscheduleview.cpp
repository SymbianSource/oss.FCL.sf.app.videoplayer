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
* Description:    CVcNsScheduleView view class definitions.*
*/




// INCLUDE FILES
#include <aknViewAppUi.h>
#include <akntitle.h>
#include <AknUtils.h>
#include <avkon.hrh>
#include <AknGlobalNote.h>
#include <AknIconUtils.h> 
#include <StringLoader.h>
#include <akntitle.h>
#include <barsread.h>
#include <mpxviewutility.h>
#include <cmapplicationsettingsui.h>
#include <centralrepository.h>
#include <AknQueryDialog.h>

#include <vcxnsscheduleview.rsg>
#include <videoplayeractivationmessage.h>

#include "IptvDebug.h"
#include "CIptvResourceLoader.h"
#include "vcxnsservicesettings.h"
#include "videoplayersettingsengine.h"
#include "vcxnsscheduleview.h"
#include "vcxnsscheduleview.hrh"
#include "CVcxNsScheduleSettingsViewContainer.h"


// Consts
_LIT( KVcScheduleViewResource, "\\resource\\apps\\vcxnsscheduleview." );

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVcxNsScheduleView::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsScheduleView* CVcxNsScheduleView::NewL( )
    {
    CVcxNsScheduleView* self = CVcxNsScheduleView::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleView::NewLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsScheduleView* CVcxNsScheduleView::NewLC( )
    {
    CVcxNsScheduleView* self = new (ELeave) CVcxNsScheduleView( );
      CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CVcxNsScheduleView::~CVcxNsScheduleView()
// ---------------------------------------------------------
//
EXPORT_C CVcxNsScheduleView::~CVcxNsScheduleView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }

    delete iResourceLoader;
    delete iServiceSettings;
    
    if ( iViewUtility )
        {
        iViewUtility->Close();
        }
    }

// ---------------------------------------------------------
// CVcxNsScheduleView::CVcxNsScheduleView()
// ---------------------------------------------------------
//
EXPORT_C CVcxNsScheduleView::CVcxNsScheduleView( const TUid aViewId )
  : iViewId( aViewId )
    {
    }

// ---------------------------------------------------------
// CVcxNsScheduleView::ConstructL()
// ---------------------------------------------------------
//
EXPORT_C void CVcxNsScheduleView::ConstructL( )
    {
    // Resources must be loaded before base construct.
    iResourceLoader = CIptvResourceLoader::NewL( *iCoeEnv );
    iResourceLoader->AddResourceL( KVcScheduleViewResource );

    BaseConstructL( R_VCXNS_SCHEDULE_SCHEDULESETTINGSVIEW );
    
    iServiceSettings = CVcxNsServiceSettings::NewL();
    
    iViewUtility = MMPXViewUtility::UtilityL();
    }

// ---------------------------------------------------------
// TUid CVcxNsScheduleView::Id()
// Return id of this view instance
// ---------------------------------------------------------
//
TUid CVcxNsScheduleView::Id() const
    {
    return iViewId;
    }

// ---------------------------------------------------------
// CCVcxNsScheduleView::IsScheduleViewActive()
// ---------------------------------------------------------
//
TBool CVcxNsScheduleView::IsScheduleViewActive()
    {
    return iContainer != NULL;
    }

// ---------------------------------------------------------
// CCVcxNsScheduleView::HandleCommandL()
// Command handler
// ---------------------------------------------------------
//
EXPORT_C void CVcxNsScheduleView::HandleCommandL( TInt aCommand )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "Schedule view ## CVcxNsScheduleView::HandleCommandL(%d)", aCommand );

    switch ( aCommand )
        {
        case EAknSoftkeyOpen:
        case EAknSoftkeyChange:
        case EIptvScheduleSettingsCmdChange:
            if ( iContainer )
                {
                iContainer->HandleChangeSelectedSettingItemL( aCommand );
                }
            break;

        case EIptvScheduleSettingsCmdHelp:
            {
            AppUi()->HandleCommandL( EAknCmdHelp );
            }
            break;

        case EAknSoftkeyBack: 
        case EAknSoftkeyExit:
        case EIptvScheduleSettingsCmdExit:
            {
            AppUi()->HandleCommandL( EAknCmdExit );
            }
            break;

        case EAknSoftkeyDone:
            {
            TBool scheduleViewExitOk = ETrue;
            if ( iContainer && iContainer->IsSchedulingActive() )
                {
                scheduleViewExitOk = CheckScheduledDownloadIapL();
                }
            
            if ( scheduleViewExitOk )
                {
                if ( iContainer )
                    {
                    iContainer->SaveSettingsL();
                    }

                iViewUtility->ActivatePreviousViewL();
                }
            }
            break;

        default:
            AppUi()->HandleCommandL( aCommand );
            break;
        }
    }
    
// ---------------------------------------------------------
// CVcxNsScheduleView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CVcxNsScheduleView::HandleClientRectChange()
    {
    if (iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    }

// ---------------------------------------------------------
// CVcxNsScheduleView::GetButtonGroupContainer()
// ---------------------------------------------------------
//
CEikButtonGroupContainer* CVcxNsScheduleView::GetButtonGroupContainer()
    {
    return Cba();
    }

// ---------------------------------------------------------
// CVcxNsScheduleView::DoActivateL
// Activation of this view instance.
// ---------------------------------------------------------
//
EXPORT_C void CVcxNsScheduleView::DoActivateL( 
    const TVwsViewId& aPrevViewId, 
    TUid /*aCustomMessageId*/, 
    const TDesC8& aCustomMessage )
    {
    IPTVLOGSTRING_LOW_LEVEL("Schedule view ## CVcxNsScheduleView::DoActivateL()");
 
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>( StatusPane()->
            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    HBufC* title = StringLoader::LoadLC( R_VCXNS_SCHEDULE_SCHEDULE_DOWNLOADS );
    titlePane->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );

    iPrevViewId = aPrevViewId; 

    TUint32 serviceId( 0 );
    
    if ( aCustomMessage.Length() == sizeof( TUint32 ) )
        { 
        TPckg<TUint32> pkg ( serviceId ) ;
        pkg.Copy( aCustomMessage );
        }
    else 
        {
        User::Leave( KErrNotFound );
        }

    if ( !iContainer )
        {
        iContainer = new (ELeave) CVcxNsScheduleSettingsViewContainer( 
                                                  *iServiceSettings, 
                                                   serviceId );
        
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );

        AppUi()->AddToStackL(*this, iContainer);
        }
    
    iContainer->MakeVisible( ETrue );

    CEikButtonGroupContainer* cba = Cba();
    
    if ( cba )
        {
        cba->DrawDeferred();
        }

    }

// ---------------------------------------------------------
// CVcxNsScheduleView::DoDeactivate()
// Deactivation of this view instance.
// ---------------------------------------------------------
//
EXPORT_C void CVcxNsScheduleView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );

        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------
// CVcxNsScheduleView::RoutedKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CVcxNsScheduleView::RoutedKeyEventL( const TKeyEvent &aKeyEvent,
                                               TEventCode aType )
    {
    if (iContainer)
        {
        return iContainer->OfferKeyEventL(aKeyEvent, aType);
        }
    return EKeyWasConsumed;
    }

// ------------------------------------------------------------------------------
// CVcxNsScheduleView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
// This function is called by the EIKON framework just before it displays
// a menu pane. Menu is adjusted according to Schedule UI state.
// ------------------------------------------------------------------------------
//
EXPORT_C void CVcxNsScheduleView::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    if ( !iContainer )
        {
        return;
        }
    
    switch ( aResourceId )
        {
        case R_VCXNS_SCHEDULE_SCHEDULESETTINGSVIEW_OPTIONS_MENU:

            {
            aMenuPane->SetItemDimmed( EIptvScheduleSettingsCmdChange, EFalse );
            // implement help
            aMenuPane->SetItemDimmed( EIptvScheduleSettingsCmdHelp, ETrue );
            aMenuPane->SetItemDimmed( EIptvScheduleSettingsCmdExit, EFalse );
            }
            break;
        default: 
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleView::CheckScheduledDownloadIapL()
// -----------------------------------------------------------------------------
//
TBool CVcxNsScheduleView::CheckScheduledDownloadIapL()
    {
    TBool exitScheduledDownloadView( ETrue );

	TInt snapId( 0 );
	CRepository* cenRep = CRepository::NewLC( KIptvAlrCenRepUid );
	// In the case of an error we should anyway terminate query not to jam ui.
	if ( KErrNone == cenRep->Get( KIptvCenRepUsedSnapIdKey, snapId ) )
		{
        if ( KIptvCenRepAPModeAlwaysAsk == snapId )
            {
            HBufC* dlgText = 
                StringLoader::LoadLC( R_VCXNS_SCHEDULE_DEFINE_SCHEDULED_DESTINATION );
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            dlg->ExecuteLD( R_VCXNS_SCHEDULE_CONFIRMATION_QUERY, *dlgText );
            CleanupStack::PopAndDestroy( dlgText );

            if ( LaunchSnapSelectionL( snapId ) )
                {
                // Selected Snap is stored to cenrep.
                cenRep->Set( KIptvCenRepUsedSnapIdKey, snapId );
                }
            else
                {
                // User has cancelled Snap selection.
                // Scheduled download is not possible.
                exitScheduledDownloadView = EFalse;
                }
            }
        }

	CleanupStack::PopAndDestroy( cenRep );

    return exitScheduledDownloadView;
    }

// --------------------------------------------------------------------------
// CVcxNsScheduleView::LaunchSnapSelectionL()
// --------------------------------------------------------------------------
//
TBool CVcxNsScheduleView::LaunchSnapSelectionL( TInt& aUsedSnapId ) const
    {
    TBool set( EFalse );

    CCmApplicationSettingsUi* settingsUi = CCmApplicationSettingsUi::NewL();
    CleanupStack::PushL ( settingsUi );
    
    TCmSettingSelection selection; 
    selection.iId = aUsedSnapId;
    selection.iResult = CMManager::EDestination;
    
    TUint listItems = CMManager::EShowDestinations;
    TBearerFilterArray filters;
    
    TBool retVal = settingsUi->RunApplicationSettingsL( selection, listItems, filters );

    if ( retVal && ( CMManager::EDestination == selection.iResult ) )
        {
        aUsedSnapId = selection.iId;
        set = ETrue;
        }
    
    CleanupStack::PopAndDestroy ( settingsUi );
    
    return set;
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleView::HandleStatusPaneSizeChange
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsScheduleView::HandleStatusPaneSizeChange()
    {
    }
