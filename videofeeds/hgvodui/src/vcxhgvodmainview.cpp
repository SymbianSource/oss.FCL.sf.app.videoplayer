/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   HG VOD Main view implementation*
*/

// Version : %version: REL6_40 %

#include <aknview.h>
#include <aknViewAppUi.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <gulicon.h>

#include <eikenv.h>
#include <AknsBasicBackgroundControlContext.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <akntoolbar.h>
#include <centralrepository.h>
#include <ganes/HgDoubleGraphicList.h>
#include <ganes/HgDoubleTextList.h>
#include <data_caging_path_literals.hrh>

#include <mpxviewutility.h>

#include <vcxhgvodui.rsg>
#include <vcxhgvoddefaulticons.mbg>
#include "vcxnsuiengine.h"
#include <hlplch.h>
#include "vcxnscontentproviderif.h"

#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodservicelistimpl.h"
#include "vcxhgvodcategorylistimpl.h"
#include "vcxhgvodcontentlistimpl.h"
#include "vcxhgvodservicegrouplistimpl.h"
#include "vcxhgvodlistimplbase.h"
#include "vcxhgvodutils.h"
#include "vcxhgvodtelephonyclient.h"
#include "vcxhgvodui.hrh"
#include "CIptvResourceLoader.h"
#include "vcxnssettingsplugin.hrh"

#include <avkon.rsg>
#include <videoplayeractivationmessage.h>

#include "vcxhgvodutils.h"
#include "vcxviewmessageutility.h"
#include "vcxnsserviceproviderif.h"
#include "vcxnsservice.h"
#include "vcxnsscheduleplugin.hrh"
#include "mpxvideoplayercustomviewmsgconsts.h"

_LIT( KVodViewResFile, "\\resource\\apps\\vcxhgvodui." );
_LIT( KVodDefaultIconsMifFile, "vcxhgvoddefaulticons.mif" );

const TUint32 KVcxNsFeedsViewUid = 0x102750DB;
const TInt KVcxNsCenRepUid = 0x102750E2;
const TInt KVcxNsCenRepParentControlKey = 0xC;

void ScaleGulIconL( CGulIcon* aIcon, const TSize aSize )
    {
    User::LeaveIfError( AknIconUtils::SetSize( aIcon->Bitmap(), aSize, 
            EAspectRatioPreservedAndUnusedSpaceRemoved ) );
    User::LeaveIfError( AknIconUtils::SetSize( aIcon->Mask(), aSize, 
            EAspectRatioPreservedAndUnusedSpaceRemoved ) );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodMainView::~CVcxHgVodMainView()
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxHgVodMainView::~CVcxHgVodMainView()
    {
    if ( iActiveList )
        {
        iActiveList->DoDeactivate();
        }
    
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    
    delete iServiceList;
    delete iCategoryList;
    delete iContentList;
    delete iServiceGroupList;
    delete iResourceLoader;   
    delete iBgContext;

    if ( iViewUtility )
        {
        iViewUtility->Close();
        }    

    if( iModel )
        {
        iModel->ResetStates();
        iModel->DecreaseReferenceCount();
        }
    
    delete iGraphicScroller;
    delete iTextScroller;
    
    delete iContentDefaultIcon;
    delete iServiceDefaultIcon;
    
    delete iCenRep;
    delete iTelephonyClient;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodMainView::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxHgVodMainView* CVcxHgVodMainView::NewL( )
    {
    CVcxHgVodMainView* self = CVcxHgVodMainView::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodMainView::NewLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxHgVodMainView* CVcxHgVodMainView::NewLC( )
    {
    CVcxHgVodMainView* self = new (ELeave) CVcxHgVodMainView( );
      CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//  Return view id
// -----------------------------------------------------------------------------
TUid CVcxHgVodMainView::Id() const
    {
    return TUid::Uid( KVcxNsFeedsViewUid );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CVcxHgVodMainView::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/,
    TUid  /*aCustomMessageId*/,
    const TDesC8& aCustomMessage )
    {
    // ###
    if ( aCustomMessage.Length() > KVideoPlayerActivationMessageMinLength )
        {
        ProcessCustomMessageL( aCustomMessage );
        }
    else
        {
        // Normal startup, set UI engine's first state    
        if ( iModel->VcAppState() == EStateNoState )
            {
            iModel->SetVcAppState( EStateServiceView );
            }
        
        // Back from player, pop ui engine's state.
        else if ( iModel->VcAppState() == EStatePlayback )
            {
            TBool consumed = iModel->HandleBack();
            
            // If player was strated directly, exit the app.
            if ( !consumed )
                {
                AppUi()->HandleCommandL( EAknSoftkeyBack );
                }
            }
        }

    if ( !iContainer )
        {
        iContainer = 
            new (ELeave) CVcxHgVodMainViewContainer( AppUi(), *this );
        iContainer->ConstructL( ClientRect() );
        iContainer->SetMopParent( this );
        }

    iContainer->MakeVisible( ETrue );
    AppUi()->AddToStackL( *this, iContainer );


    if ( IsTouchSupported() )
        {
        CAknToolbar* fixedToolbar( NULL );
        fixedToolbar = AppUi()->CurrentFixedToolbar();
        
        if ( fixedToolbar )
            {
            fixedToolbar->SetToolbarVisibility( ETrue );    
            }
        }

    // Activate correct list view
    if( iModel->VcAppState() == EStateServiceView ||
        iModel->VcAppState() == EStateCategoryView ||
        iModel->VcAppState() == EStateContentView ||
        iModel->VcAppState() == EStateCustomView )
        {
        // Activate correct list view
        ChangeListViewL();
        }

    CVcxViewMessageUtility::InstanceL()->AddObserverL( *this );
        
    HandleStatusPaneSizeChange(); // make sure that all rects are correct.
    }    

// -----------------------------------------------------------------------------
//  Deactivating of view
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::DoDeactivate( )
    {
    if ( iActiveList )
        {
        iActiveList->DoDeactivate();
        SetActiveList( NULL );
        }
    
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    
    TRAP_IGNORE( CVcxViewMessageUtility::InstanceL()->RemoveObserver( *this ) );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodMainView::HandleCommandL
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::HandleCommandL( TInt aCommand )
    {
    if ( iActiveList )
        {
        //Active list handles command first
        iActiveList->HandleCommandL( aCommand );
        }
    
    //Handle cases, that need list view changes
    switch ( aCommand )
        {
        case EAknSoftkeyOpen:
        case EVcxHgVodServiceOpenCmd:
        case EVcxHgVodCategoryOpenCmd:
        case EVcxHgVodServiceGroupOpenCmd:
            {
            ChangeListViewL();
            }
            break;
            
        case KVcxCustomViewMessageCmd:
            {
            HandleCustomViewMessageL();
            }
            break;

        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                             AppUi()->AppHelpContextL() );
            }
            break;

        case EAknSoftkeyBack:
            {
            // Playback state may remain even if we're really in content
            // view when playback starting is canceled, because there may
            // not be any view transitions.
            if ( iModel->VcAppState() == EStatePlayback )
                {
                iModel->HandleBack();
                }
            
            if ( iServiceList && iModel->VcAppState() == EStateBrowser )
                {
                iServiceList->CloseEmbeddedBrowser();
                }            
            
            TBool consumed  = iModel->HandleBack();

            ChangeListViewL();

            if ( !consumed )
                {
                AppUi()->HandleCommandL( aCommand );
                }
            }
            break;

        case EAknCmdExit:
        case EEikCmdExit:
            {
            if ( iActiveList )
                {
                iActiveList->DoDeactivate();
                iActiveList = NULL;
                }
            
            iModel->SetVcAppState( EStateNoState );
            
            AppUi()->HandleCommandL( aCommand );
            }
            break;

        default:
            {
            AppUi()->HandleCommandL( aCommand );
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodMainView::CVcxHgVodMainView()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodMainView::CVcxHgVodMainView( )
  : iViewId( TUid::Uid( EIptvVodFeedView ) ),
    iTouchSupport ( EFalse )
    {
    // None
    }


// -----------------------------------------------------------------------------
// CVcxHgVodMainView::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::ConstructL()
    {
    iResourceLoader = CIptvResourceLoader::NewL( *iEikonEnv );

    if ( iResourceLoader )
        {
        iResourceLoader->AddResourceL( KVodViewResFile );
        }

    BaseConstructL( R_VCXHG_VOD_MAIN_VIEW );
    
    iModel = CVcxNsUiEngine::InstanceL();

    // Create background context.
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
                                                           ClientRect(),
                                                           ETrue );

    if ( AknLayoutUtils::PenEnabled() )
        {
        iTouchSupport = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVcxHgVodListImplBase* CVcxHgVodMainView::ActiveList( )
    {
    return iActiveList; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::SetActiveList( CVcxHgVodListImplBase* aList )
    {
    iActiveList = aList; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVcxNsPlayerWrapper* CVcxHgVodMainView::PlayerWrapper()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVcxHgVodMainViewContainer* CVcxHgVodMainView::Container()
    {
    return iContainer;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TKeyResponse CVcxHgVodMainView::HandleKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    // Add checks for application state.
    
    TKeyResponse response( EKeyWasNotConsumed );
    
    if( iActiveList && MenuBar()->ItemSpecificCommandsEnabled() )
        {
        if ( ( aKeyEvent.iScanCode == EStdKeyEnter || aKeyEvent.iScanCode == EStdKeyDevice3 ) && 
             aType == EEventKey && 
             aKeyEvent.iRepeats == 0 )
            {
            iActiveList->HandleOpenL( KErrNotFound ); // index not needed or used in list impl.
            response = EKeyWasConsumed;
            }
        else
            {
            response = iActiveList->HandleKeyEventL( aKeyEvent, aType );            
            }
        }

    return response;    
    }

// -----------------------------------------------------------------------------
// Menu handling
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::DynInitMenuPaneL( 
    TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    if ( iActiveList )
        {
        iActiveList->ActivateMenuPaneItemsL( aMenuPane, aResourceId );
        }
    }

// -----------------------------------------------------------------------------
// Handle custom view message
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::HandleCustomViewMessageL(
    const TUid /*aMessageUid*/, 
    const TDesC8& aCustomMessage )
    {
    if ( aCustomMessage.Length() > KVideoPlayerActivationMessageMinLength )
        {
        ProcessCustomMessageL( aCustomMessage );
        }
    }

// -----------------------------------------------------------------------------
// Get pointer to title pane
// -----------------------------------------------------------------------------
//
CAknTitlePane* CVcxHgVodMainView::TitlePaneL()
    {
    return static_cast<CAknTitlePane*>( StatusPane()->
            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    }

// -----------------------------------------------------------------------------
// Get pointer to navi pane
// -----------------------------------------------------------------------------
//
CAknNavigationControlContainer* CVcxHgVodMainView::NaviPaneL()
    {
    return static_cast<CAknNavigationControlContainer*>( StatusPane()->
            ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    }

// ---------------------------------------------------------
// Create list
// ---------------------------------------------------------
//
void CVcxHgVodMainView::CreateListL( 
    TVcxHgVodListType aType )
    {
    if ( !iTextScroller && aType == EVcxNsCategoryList )
        {
        iTextScroller = CHgDoubleTextList::NewL( ClientRect(), 0 );
        }
    else if ( !iGraphicScroller )
        {
        iGraphicScroller = CHgDoubleGraphicList::NewL( ClientRect(), 0 );
        
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        
        TFileName fileName;
        LocateMifFileL( fileName );
        
        if ( !iContentDefaultIcon ) // paranoia
            {
            iContentDefaultIcon = AknsUtils::CreateGulIconL( skin, 
                    KAknsIIDQgnPropMtvProgClip, 
					fileName, 
                    EMbmVcxhgvoddefaulticonsQgn_prop_mtv_prog_clip, 
                    EMbmVcxhgvoddefaulticonsQgn_prop_mtv_prog_clip_mask );
            
            ScaleGulIconL( iContentDefaultIcon, 
                    CHgDoubleGraphicList::PreferredImageSize() );
            }
        
        if ( !iServiceDefaultIcon ) // paranoia
            {
            iServiceDefaultIcon = AknsUtils::CreateGulIconL( skin, 
                    KAknsIIDQgnPropMtvProgClip, 
                    fileName, 
                    EMbmVcxhgvoddefaulticonsQgn_prop_mtv_vodcast_feed, 
                    EMbmVcxhgvoddefaulticonsQgn_prop_mtv_vodcast_feed_mask );
            
            ScaleGulIconL( iServiceDefaultIcon, 
                    CHgDoubleGraphicList::PreferredImageSize() );
            }
        }
    
    if ( aType == EVcxNsServiceList && !iServiceList )
        {
        iServiceList = CVcxHgVodServiceListImpl::NewL( 
                *iGraphicScroller, *iModel, *this );
        }
    else if ( ( aType == EVcxNsCategoryList ) && !iCategoryList )
        {
        iCategoryList = CVcxHgVodCategoryListImpl::NewL( 
                *iTextScroller, *iModel, *this ); 
        }
    else if ( ( aType == EVcxNsContentList ) && !iContentList )
        {
        iContentList = CVcxHgVodContentListImpl::NewL( 
                 *iGraphicScroller, *iModel, *this );
        }
    else if ( ( aType == EVcxNsCustomList ) && !iServiceGroupList )
        {
        iServiceGroupList = CVcxHgVodServiceGroupListImpl::NewL(
                *iGraphicScroller, *iModel, *this );
        }
    }

// ---------------------------------------------------------
// Activate list according to current state
// ---------------------------------------------------------
//
void CVcxHgVodMainView::ChangeListViewL()
    {
    //Get application state from ui engine
    TVcxNsAppState newState = iModel->VcAppState();    
    
    if ( iActiveList )
        {
        iActiveList->DoDeactivate();
        SetActiveList( NULL );
        }
        
    if ( newState == EStateNoState )
        {
        // return, model not ready or application is closing.
        return;
        }
    
    
    if ( newState == EStateServiceView )
        {
        CreateListL( EVcxNsServiceList );

        iGraphicScroller->SetDefaultIconL( CVcxHgVodUtils::DuplicateGulIconL( iServiceDefaultIcon ) );
        
        iServiceList->DoActivateL();
        SetActiveList( iServiceList );
        }
    else if ( newState == EStateCategoryView )
        {
        CreateListL( EVcxNsCategoryList );
        iCategoryList->DoActivateL();
        SetActiveList( iCategoryList );
        }
    else if ( newState == EStateContentView )
        {
        CreateListL( EVcxNsContentList );

        iGraphicScroller->SetDefaultIconL( CVcxHgVodUtils::DuplicateGulIconL( iContentDefaultIcon ) );

        iContentList->DoActivateL();
        SetActiveList( iContentList );
        }
    else if ( newState == EStateCustomView )
        {
        CreateListL( EVcxNsCustomList );

        iGraphicScroller->SetDefaultIconL( CVcxHgVodUtils::DuplicateGulIconL( iServiceDefaultIcon ) );

        iServiceGroupList->DoActivateL();
        SetActiveList( iServiceGroupList );
        }
    }

// ---------------------------------------------------------
// Activate settings view
// ---------------------------------------------------------
//
void CVcxHgVodMainView::OpenSettingsL( TInt aCommand )
    {
    // ###
	TVideoPlayerActivationMessage params;
    
    if ( aCommand == EVcxHgVodServiceSubscriptionsCmd )
        {
        params.iMsgType = 
        TVideoPlayerActivationMessage::EOpenVCSettingsFeedSubscriptions;
        }
    else if ( aCommand == EVcxHgVodServiceAddManuallyCmd )
        {
        params.iMsgType = 
        TVideoPlayerActivationMessage::EOpenVCSettingsAddFeed;
        }

    TPckg<TVideoPlayerActivationMessage> paramsPckg( params );
    HBufC* customMsg = HBufC::NewLC( paramsPckg.Length() );
    customMsg->Des().Copy( paramsPckg );
    
    if ( !iViewUtility )
        {
        iViewUtility = MMPXViewUtility::UtilityL();
        }
    
    iViewUtility->ActivateViewL( 
        TUid::Uid ( KVcxNsSettingsPluginSettingsViewTypeId ), customMsg );
    
    CleanupStack::PopAndDestroy( customMsg );
    }

// ---------------------------------------------------------
// Activate schedule settings view
// ---------------------------------------------------------
//
void CVcxHgVodMainView::OpenScheduleSettingsL()
    {
    CVcxNsService* service = iModel->GetServiceProviderL()->GetActiveServiceData();

    if ( !service )
        {
	    return;
        }
    
	TUint32 id = service->GetServiceId();
    TPckg<TUint32> paramsPckg( id );
    HBufC* customMsg = HBufC::NewLC( paramsPckg.Length() );
    customMsg->Des().Copy( paramsPckg );
    
    if ( !iViewUtility )
        {
        iViewUtility = MMPXViewUtility::UtilityL();
        }
    
    iViewUtility->ActivateViewL( 
        TUid::Uid ( KVcxNsSchedulePluginScheduleViewTypeId ), customMsg );
    
    CleanupStack::PopAndDestroy( customMsg );
    }

// ---------------------------------------------------------
// PlayInternalVideoL
// ---------------------------------------------------------
//
void CVcxHgVodMainView::PlayInternalVideoL( CVcxNsContent* aContent )
    {
    if ( aContent )
        {
	    if ( aContent->GetContentTypes() & CVcxNsContent::EVcxContentTypeVideo &&
                aContent->GetDlStatus() == CVcxNsContent::EVcxContentDlStateDownloading ||
                aContent->GetDlStatus() == CVcxNsContent::EVcxContentDlStateFailed ||
                aContent->GetDlStatus() == CVcxNsContent::EVcxContentDlStatePaused || 
                aContent->GetDlStatus() == CVcxNsContent::EVcxContentDlStateDownloaded ) 
	        {
	        iModel->PlayVideoL( aContent );
	        iModel->VideoPlaybackStartingL( aContent );
	        iModel->SetVcAppState( EStatePlayback );
	        }
	    else if ( aContent->GetContentTypes() & CVcxNsContent::EVcxContentTypeStream &&
	              aContent->GetUrl( CVcxNsContent::EVcxContentTypeStream ).Length() > 0 ) 
            {
            iModel->PlayStreamL( aContent );
            iModel->VideoPlaybackStartingL( aContent );
            iModel->SetVcAppState( EStatePlayback );
            }
        }
    }


// ---------------------------------------------------------
// Stream video
// ---------------------------------------------------------
//
void CVcxHgVodMainView::PlayStreamL( const TDesC& aUrl )
    {
    AppUi()->OpenFileL( aUrl );
    iModel->SetVcAppState( EStatePlayback );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
TTypeUid::Ptr CVcxHgVodMainView::MopSupplyObject( TTypeUid aId )
    {   
    if ( iBgContext && aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }  
    return CAknView::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::HandleStatusPaneSizeChange()
    {
    // This function uses fonts from CCoeEnv, so we cannot  
    // execute function if the environment is already destroyed. This can
	// happen e.g. when End key has pressed and application is closing.
    if ( iCoeEnv->NormalFont() )
        {
        TRect cr = ClientRect();
        
        TRect containerRect;
        if ( !AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, containerRect ) )
            {
            containerRect = cr;
            }
        
        if ( iBgContext )
            {
            iBgContext->SetRect( containerRect );
            }
    
        if ( iContainer )
            {
            iContainer->SetRect( containerRect );
            }
        
        if ( iGraphicScroller )
            {
            TRAP_IGNORE( iGraphicScroller->InitScreenL( cr ) );
            }
        
        if ( iTextScroller )
            {
            TRAP_IGNORE( iTextScroller->InitScreenL( cr ) );
            }
        
        if ( iActiveList )
            {
            iActiveList->HandleSizeChange();
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::HandleForegroundEventL( TBool aForeground )
    {
    CAknView::HandleForegroundEventL( aForeground );
    if ( iActiveList )
        {
        iActiveList->HandleForegroundEventL( aForeground );
        }
    }

// ---------------------------------------------------------
// Called by list implementation(s) when layout is changed.
// ---------------------------------------------------------
//
void CVcxHgVodMainView::LayoutChanged()
    {
    HandleStatusPaneSizeChange();
    }

// ---------------------------------------------------------
// Get eikon env
// ---------------------------------------------------------
//
CEikonEnv& CVcxHgVodMainView::EikonEnv()
    {
    return *iEikonEnv;
    }

// ---------------------------------------------------------
// Get Cba
// ---------------------------------------------------------
//
CEikButtonGroupContainer* CVcxHgVodMainView::ViewCba()
    {
    return Cba();
    }

// ---------------------------------------------------------
// Touch support
// ---------------------------------------------------------
//
TBool CVcxHgVodMainView::IsTouchSupported()
    {
    return iTouchSupport;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CVcxHgVodMainView::ProcessCustomMessageL( const TDesC8& aCustomMessage )
    {    
    TVideoPlayerActivationMessage params;
    TPckg<TVideoPlayerActivationMessage> paramsPckg( params );
    paramsPckg.Copy( aCustomMessage );
    
    // Message coming from note,
    // open to specified service
    if ( params.iMsgType == TVideoPlayerActivationMessage::ELaunchServiceById ) 
        {
        // Close the embedded browser if opened from here:
        TVcxNsAppState appState = iModel->VcAppState();
        if ( appState == EStateBrowser )
            {
            // Step the state back
            iModel->HandleBack();
            appState = iModel->VcAppState();
            
            if ( iServiceList && appState == EStateServiceView  )
                {
                iServiceList->CloseEmbeddedBrowser();
                }
            }
        
        // Reset previous states.
        iModel->ResetStates();

        // Set state, where to return from content view
        iModel->SetVcAppState( EStateServiceView );

        // Open the actual service:
        iModel->GetServiceProviderL()->OpenServiceL( params.iServiceId );
        
        // Switch to correct view
        ChangeListViewL();
        }

    // Message coming from note or suite,
    // launch last watched video
    else if ( params.iMsgType == TVideoPlayerActivationMessage::ELaunchVideoToPlayer )
        {
		iModel->PlayLastWatchedL();
        
		iModel->SetVcAppState( EStatePlayback );
        
        // Switch to correct view
        ChangeListViewL();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::CheckParentalControlL( TUint32 aAgeProfile )
    {
    TInt parentControlSetting( 0 );
    TInt error( KErrNone );
    if ( !iCenRep )
        {
        TRAP( error, iCenRep = CRepository::NewL( TUid::Uid( KVcxNsCenRepUid ) ) )
        }
    if ( error == KErrNone )
        {
        error = iCenRep->Get( KVcxNsCenRepParentControlKey, parentControlSetting );
        if ( error == KErrNone && parentControlSetting != 0 )
            {
            // Parental control has been set
            if ( static_cast<TInt>( aAgeProfile ) >= parentControlSetting )
                {
                // Must ask lock code to allow playing
                if ( !iTelephonyClient )
                    {
                    iTelephonyClient = new( ELeave ) CVcxHgVodTelephonyClient();
                    }
                if ( !iTelephonyClient->CheckLockCodeL() )
                    {
                    User::Leave( KErrPermissionDenied );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodMainView::HandleCustomViewMessageL
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::HandleCustomViewMessageL()
    {
    TAny* ptr = UserSvr::DllTls( KVcxCustomViewMessagePtr );
    
    if ( ptr )
        {
        HBufC8* bufptr = ( HBufC8* ) ptr;
        
        if ( bufptr->Length() > KVideoPlayerActivationMessageMinLength )
            {
            ProcessCustomMessageL( *bufptr );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodMainView::LocateMifFileL
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainView::LocateMifFileL( TFileName& aFileName )
    {
    // Get the path & file name with the drive not specified.
    TParse parse;
    parse.Set( KVodDefaultIconsMifFile, &KDC_APP_BITMAP_DIR, NULL );
    TPtrC mifFile = parse.FullName();
   
    // This is done to ensure upgraded file is used first.
    TFindFile find( iCoeEnv->FsSession() );
    TInt err = find.FindByDir( mifFile, KNullDesC );
    
    if ( err )
        {
        User::Leave( KErrNotFound );
        }
    else
        {
        aFileName.Append( find.File() );
        }
    }

