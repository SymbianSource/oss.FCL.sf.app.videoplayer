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
* Description:      My Videos UI main view.*
*/


// Version : %version: 47 %

// INCLUDE FILES
#include <bldvariant.hrh>
#include <aknViewAppUi.h>
#include <avkon.hrh>
#include <StringLoader.h>
#include <coeaui.h>
#include <aknnotewrappers.h>
#include <hlplch.h>
#include <apgcli.h>     // RApaLsSession
#include <apacmdln.h>   // CApaCommandLine

#include <AknsBasicBackgroundControlContext.h>
#include <ganes/HgDoubleTextList.h>
#include <ganes/HgDoubleGraphicList.h>

#include "IptvDebug.h"
#include "CIptvResourceLoader.h"
#include <vcxhgmyvideos.rsg>

#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideosmainview.h"
#include "vcxhgmyvideosmainviewcontainer.h"
#include "vcxhgmyvideoscategorylistimpl.h"
#include "vcxhgmyvideosvideolistimpl.h"
#include "vcxhgmyvideosvideomodelhandler.h"
#include "vcxhgmyvideos.hrh"
#include "vcxhgmyvideosmemorystatus.h"
#include "vcxhgmyvideosvideodetailsdialog.h"
#include "vcxhgmyvideosupnpinterface.h"

// CONSTANTS
_LIT( KVcxHgMyVideosMainViewResFile, "\\resource\\apps\\vcxhgmyvideos." );

const TUint32 KVcxHgMyVideosViewUid = 0x20021191;
const TUid KMediaSettingsAppUid = { 0x10005A3F };
const TUid KBrowserAppUid       = { 0x10008D39 };

const TInt KVcxHgMyVideosUrlMaxLen = 1024;

_LIT( KBrowserProtocol, "4 " );

// URL to Ovi Store videos. If Ovi Store Launcher is not installed, this
// address will be opened to web browser.
_LIT( KOviVideoStoreURL, "http://lr.ovi.mobi/store/video" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::CVcxHgMyVideosMainView()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosMainView::CVcxHgMyVideosMainView()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # ----------- MPX My Videos UI starting -----------" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxHgMyVideosMainView* CVcxHgMyVideosMainView::NewL()
    {
    CVcxHgMyVideosMainView* self = 
        CVcxHgMyVideosMainView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::NewLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxHgMyVideosMainView* CVcxHgMyVideosMainView::NewLC()
    {
    CVcxHgMyVideosMainView* self = 
        new (ELeave) CVcxHgMyVideosMainView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosMainView::ConstructL: Entered." );
    
    iResourceLoader = CIptvResourceLoader::NewL( *iCoeEnv );
    iResourceLoader->AddResourceL( KVcxHgMyVideosMainViewResFile );

    BaseConstructL( R_VCXHGMYVIDEOS_MAINVIEW );
    
    // Create component model.
    iModel = CVcxHgMyVideosModel::NewL();

    // Create background context.
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
                                                           ClientRect(),
                                                           ETrue );
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosMainView::ConstructL: Exited." );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::~CVcxHgMyVideosMainView()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxHgMyVideosMainView::~CVcxHgMyVideosMainView()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # ----------- MPX My Videos UI stopping -----------" );

    delete iCategoryList;
    delete iVideoList;    
    delete iModel;
    delete iBgContext;
    delete iResourceLoader;
    
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::CategoryListL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryListImpl* CVcxHgMyVideosMainView::CategoryListL()
    {
    if ( !iCategoryList )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # CVcxHgMyVideosMainView::CategoryListL: Creating category list." );
        // Create category list control.
        CHgDoubleGraphicList* hgCategoryList = 
            CHgDoubleGraphicList::NewL( ClientRect(), 0 );
        CleanupStack::PushL( hgCategoryList );
      
        // Create list implementations. 
        // Ownership of list control is transferred. 
        iCategoryList = CVcxHgMyVideosCategoryListImpl::NewL( *iModel,
                                                              *this,
                                                              *hgCategoryList );
        CleanupStack::Pop( hgCategoryList );
        }
    
    return iCategoryList;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::VideoListL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListImpl* CVcxHgMyVideosMainView::VideoListL()
    {
    if ( !iVideoList )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # CVcxHgMyVideosMainView::VideoListL: Creating video list." );
        
        // Create video list control.
        CHgDoubleGraphicList* hgVideoList = 
            CHgDoubleGraphicList::NewL( ClientRect(), 0 );
        CleanupStack::PushL( hgVideoList );
        
        // Create list implementation. 
        // Ownership of list control is transferred.        
        iVideoList = CVcxHgMyVideosVideoListImpl::NewL( *iModel,
                                                        *this,
                                                        *hgVideoList );
        CleanupStack::Pop( hgVideoList );
        }
    
    return iVideoList;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::Cba()
// -----------------------------------------------------------------------------
//
CEikButtonGroupContainer* CVcxHgMyVideosMainView::Cba()
    {
    return CAknView::Cba();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::Id()
// -----------------------------------------------------------------------------
// 
TUid CVcxHgMyVideosMainView::Id() const
	{
	return TUid::Uid( KVcxHgMyVideosViewUid ); 
	}

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::MopSupplyObject()
// -----------------------------------------------------------------------------
// 
TTypeUid::Ptr CVcxHgMyVideosMainView::MopSupplyObject( TTypeUid aId )
    {   
    if ( iBgContext && aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }  
    return CAknView::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::HandleStatusPaneSizeChange()
// 
// Called by the framework when the application status pane
// size is changed.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::HandleStatusPaneSizeChange()
    {
    if ( iContainer ) // If view is active
        {
        UpdateLayout(); 
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::TitlePaneL()
// -----------------------------------------------------------------------------
// 
CAknTitlePane* CVcxHgMyVideosMainView::TitlePaneL()
    {
    CAknTitlePane*  titlePane  = static_cast<CAknTitlePane*>(
            StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    return titlePane;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::DoActivateL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosMainView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                          TUid /*aCustomMessageId*/,
                                          const TDesC8& /*aCustomMessage*/ )
    {  
    if ( ! iContainer )
        {
        iContainer = CVcxHgMyVideosMainViewContainer::NewL( *this );
        AppUi()->AddToStackL( *this, iContainer );
        }
    
    CVcxHgMyVideosModel::TVcxMyVideosAppState appState = iModel->AppState();
    if ( ( appState == CVcxHgMyVideosModel::EVcxMyVideosAppStateUnknown ) ||
         ( appState == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle ) ||
         ( appState == CVcxHgMyVideosModel::EVcxMyVideosAppStatePlayer &&
           iModel->PreviousAppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle ) ) 
        {
        ActivateCategoryListL();
        }
    else
        {
        ActivateVideoListL( KErrNotFound ); // Try to activate old list. 
        }
    UpdateLayout();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::DoDeactivate()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosMainView::DoDeactivate()
    {
    if ( iCategoryList )
        {
        TRAP_IGNORE( CategoryListL()->DoListDeactivate() );
        }
    
    if ( iVideoList )
        {
        TRAP_IGNORE( VideoListL()->DoListDeactivate() );
        }

    if ( iContainer )
		{
		AppUi()->RemoveFromStack( iContainer );
		delete iContainer;
		iContainer = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::ActivateCategoryListL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosMainView::ActivateCategoryListL()
    {
    iModel->SetAppState( CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryBusy );
    
    CategoryListL()->DoListActivateL();
    
    if ( iVideoList )
        {
        VideoListL()->DoListDeactivate();
        }
    
    iModel->SetAppState( CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle );
    DynInitMskL();
    DynInitRskL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::ActivateVideoListL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosMainView::ActivateVideoListL( TInt aIndex )
    {  
    iModel->SetAppState( CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoBusy );
        
    VideoListL()->DoListActivateL( aIndex );
    
    if ( iCategoryList )
        {   
        CategoryListL()->DoListDeactivate();
        }
    
    iModel->SetAppState( CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle );
    DynInitMskL();
    DynInitRskL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainView::HandleCommandL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyCancel:
            {
            HandleCancelL();
            break;
            }
        case EVcxHgMyVideosCmdPlay:
            {
            VideoListL()->PlayCurrentVideoL();
            }
            break;
        case EVcxHgMyVideosCmdOpen:
        case EAknSoftkeyOpen: // MSK
            {
            TInt highlight = CategoryListL()->Highlight();
            
            if ( highlight >= 0 )
                {
                CategoryListL()->HandleOpenL( highlight );
                }
            }
            break;
        case EVcxHgMyVideosCmdDelete:
            {
            if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
                {
                VideoListL()->DeleteVideosL();
                }
            }
            break;
        case EVcxHgMyVideosCmdVideoDetails:
            {
            VideoListL()->VideoModel().ShowVideoDetailsDialogL();
            }
            break;
        case EVcxHgMyVideosCmdShowViaHomenet:
            {            
            if ( ! VideoListL()->UPnPInterface().IsStarted() )
                {
                 if ( VideoListL()->UPnPInterface().Start() != KErrNone )
                    {
                    break;
                    }
                }
            VideoListL()->PlayCurrentVideoL();
            }
            break;
        case EVcxHgMyVideosCmdStopShowViaHomenet:
            {            
            VideoListL()->UPnPInterface().Stop();
            }
            break;
        case EVcxHgMyVideosCmdMemoryStatus:
            {
            iMemoryStatus->ShowMemoryStatusDialogL( *iModel );
            }
            break;
        case EVcxHgMyVideosCmdSortByDate:
        case EVcxHgMyVideosCmdSortByName:
        case EVcxHgMyVideosCmdSortBySize:
            {
            if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
                {
                VideoListL()->HandleSortCommandL( aCommand );
                }
            }
            break;
        case EVcxHgMyVideosCmdCopy:
        case EVcxHgMyVideosCmdMove:
            {
            if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
                {
                VideoListL()->HandleMoveOrCopyCommandL( aCommand == EVcxHgMyVideosCmdCopy ?
                                                        ETrue : EFalse );
                }
            }
            break;
        case EVcxHgMyVideosCmdSend:
            {
            Cba()->DrawDeferred();
            VideoListL()->HandleSendL();
            break;
            }        
        case EVcxHgMyVideosCmdMark:
        case EVcxHgMyVideosCmdUnmark:
        case EVcxHgMyVideosCmdMarkAll:
        case EVcxHgMyVideosCmdUnmarkAll:
        case EVcxHgMyVideosCmdToggleMultipleMarking:
        case EVcxHgMyVideosCmdMarkContext:
            {
            VideoListL()->HandleMarkCommandL( aCommand );
            }
            break;
        case EVcxHgMyVideosCmdOpenSettings:
            {
            // Open MediaSettings application
            LaunchAppL( KMediaSettingsAppUid ); 
            }
            break;
        case EVcxHgMyVideosCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                                 AppUi()->AppHelpContextL() );
            }
            break;
        case EAknSoftkeySelect: // Select button / MSK.
            {
            if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle )
                {
                HandleCommandL( EVcxHgMyVideosCmdOpen );
                }
            else if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
                {
                RArray<TInt> markedVideos;
                CleanupClosePushL( markedVideos );
                
                VideoListL()->VideoModel().MarkedVideosL( markedVideos );
                if ( markedVideos.Count() <= 0 )
                    {
                    HandleCommandL( EVcxHgMyVideosCmdPlay );
                    }
                CleanupStack::PopAndDestroy( &markedVideos );
                }
            }
            break;
        case EAknSoftkeyBack:
            {
            if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
                {
                VideoListL()->HandleMarkCommandL( EVcxHgMyVideosCmdUnmarkAll );
                ActivateCategoryListL();
                }
            else
                {
                AppUi()->HandleCommandL( aCommand );
                }
            }
            break;
        case EVcxHgMyVideosCmdExit:
        case EAknSoftkeyExit:
        case EEikCmdExit:
        case EAknCmdExit:
            {
            // Pass command to app ui for handling.
            AppUi()->HandleCommandL( EAknCmdExit );
            }
            break;
        case EVcxHgMyVideosCmdNothing:
        default:
            {
#ifdef RD_VIDEO_AS_RINGING_TONE            
            // The command could be an AIW command,  offer it
            // to AIW command handler
            VideoListL()->TryHandleAiwCommandL( aCommand );
#endif
            }
            break;
        }

    DynInitMskL();
    DynInitRskL();
    }

// ------------------------------------------------------------------------------
// CVcxHgMyVideosMainView::HandleKeyEventL()
// ------------------------------------------------------------------------------
//
TKeyResponse CVcxHgMyVideosMainView::HandleKeyEventL( const TKeyEvent& aKeyEvent, 
                                                      TEventCode aType )
    {
    TKeyResponse response( EKeyWasNotConsumed );
    
    // Clear key performs 'Delete'.
    if ( aKeyEvent.iScanCode == EStdKeyBackspace && aType == EEventKey )
        {
        if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
            {
            RArray<TInt> markedVideos;
            CleanupClosePushL( markedVideos );
            
            VideoListL()->VideoModel().MarkedVideosL( markedVideos );
            TInt count = markedVideos.Count();
			CleanupStack::PopAndDestroy( &markedVideos );
			
            if ( count > 0 )
                {
                HandleCommandL( EVcxHgMyVideosCmdDelete );
                }
            
			response = EKeyWasConsumed;
            }
        }
    
    // Enter key should work the same way as selection key.
    else if ( ( aKeyEvent.iScanCode == EStdKeyEnter || aKeyEvent.iScanCode == EStdKeyDevice3 )
              && aType == EEventKey )
        {
        HandleCommandL( EAknSoftkeySelect );
        response = EKeyWasConsumed;
        }
    
    return response;
    }

// ------------------------------------------------------------------------------
// CVcxHgMyVideosMainView::UpdateLayout()
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::UpdateLayout()
    {
    TRect rect( ClientRect() );
    
    if ( iVideoList )
        {
        iVideoList->UpdateLayout( rect );
        }
    if ( iCategoryList )
        {
        iCategoryList->UpdateLayout( rect );
        }
    if ( iBgContext )
        {
        iBgContext->SetRect( rect );
        }
    }
	
// ------------------------------------------------------------------------------
// CVcxHgMyVideosMainView::DynInitMenuPaneL()
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::DynInitMenuPaneL( TInt aResourceId,
                                               CEikMenuPane* aMenuPane )
    {
  
    if ( aResourceId == R_VCXHGMYVIDEOS_MAINVIEW_MENU )
        {
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdDelete, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdSortSubMenu, ETrue );        
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdVideoDetails, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdStopShowViaHomenet, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdShowViaHomenet, ETrue );
#ifdef RD_VIDEO_AS_RINGING_TONE
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdAiwAssign, ETrue );
#endif
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdOpen, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdPlay, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdMemoryStatus, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdCopy, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdMove, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdMarkUnmarkSubMenu, ETrue);
                
        if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle )
            {                    
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdOpen, EFalse );    
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdMemoryStatus, EFalse );
            }
        else if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
            {
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdPlay, EFalse );
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdMemoryStatus, EFalse );
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdSortSubMenu, EFalse );

            // Video list handles initialisation of menu items specific to it.
            VideoListL()->DynInitMenuPaneL( aResourceId, aMenuPane );
            }
        
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdHelp, EFalse );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdExit, EFalse );
        }
    else if ( aResourceId == R_VCXHGMYVIDEOS_SORT_SUBMENU )
        {
        if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle )
            {
            return;
            }

        switch ( iModel->VideolistSortOrderL() )
            {
            case EVcxMyVideosSortingCreationDate:
                {
                aMenuPane->SetItemButtonState( EVcxHgMyVideosCmdSortByDate,
                                               EEikMenuItemSymbolOn );
                break;
                }                    
            case EVcxMyVideosSortingName:
                {
                aMenuPane->SetItemButtonState( EVcxHgMyVideosCmdSortByName,
                                               EEikMenuItemSymbolOn );
                break;
                }                    
            case EVcxMyVideosSortingSize:
                {
                aMenuPane->SetItemButtonState( EVcxHgMyVideosCmdSortBySize,
                                               EEikMenuItemSymbolOn );
                break;
                }                    
            default:
                {
                break;
                }
            }
        }   
#ifdef RD_VIDEO_AS_RINGING_TONE
    else if ( aResourceId == R_VCXHGMYVIDEOS_USE_AS_SUBMENU )
        {
        if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle )
            {
            return;
            }

        // Let video list handle AIW menu
        VideoListL()->DynInitMenuPaneL( aResourceId, aMenuPane );
        }
#endif
    else if ( aResourceId == R_VCXHGMYVIDEOS_MARK_SUBMENU )
        {
        if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle )
            {
            return;
            }
        TBool showMarkSubmenu = EFalse;
        TBool showStartMarking = EFalse;
        TBool showEndmarking = EFalse;
        TBool showMarkAll = EFalse;
        TBool showUnmarkAll = EFalse;

        VideoListL()->ShowMarkMenuItemsL(
                showMarkSubmenu,
                showStartMarking,
                showEndmarking,
                showMarkAll,
                showUnmarkAll );

#ifndef RD_VIDEO_AS_RINGING_TONE
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdMarkContext, ! showStartMarking  );
#endif
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdToggleMultipleMarking, ! showStartMarking );        
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdMarkAll, ! showMarkAll );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdUnmarkAll, ! showUnmarkAll );
        }
    }

// ------------------------------------------------------------------------------
// CVcxHgMyVideosMainView::DynInitMskL()
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::DynInitMskL()
    {
    TInt mskToSet( 0 );

    if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryBusy ||
         iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle )
        {
        mskToSet = R_VCXHGMYVIDEOS_MSK_OPEN;
        }
    else if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
        {
        mskToSet = VideoListL()->GetMskResourceL();
        }
    else
        {
        mskToSet = R_VCXHGMYVIDEOS_MSK_NOTHING;
        }

    if ( mskToSet != iCurrentMskResource )
        {
        iCurrentMskResource = mskToSet;

        if ( Cba() )
            {
            Cba()->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                                iCurrentMskResource );
            Cba()->DrawDeferred();
            }
        }
    }

// ------------------------------------------------------------------------------
// CVcxHgMyVideosMainView::DynInitRskL()
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::DynInitRskL()
    {
    TInt rskToSet( 0 );

    if ( iVideoList && iVideoList->IsMarking() )
        {
        rskToSet = R_VCXHGMYVIDEOS_RSK_CANCEL;
        }    
    else
        {
        if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryIdle ||
             iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateCategoryBusy )
             {
             rskToSet = R_VCXHGMYVIDEOS_RSK_EXIT;
             }
         else 
             {     
             rskToSet = R_VCXHGMYVIDEOS_RSK_BACK;
             }
        }

    if ( rskToSet != iCurrentRskResource )
        {
        iCurrentRskResource = rskToSet;

        if ( Cba() )
            {
            Cba()->SetCommandL( CEikButtonGroupContainer::ERightSoftkeyPosition,
                                iCurrentRskResource );
            Cba()->DrawDeferred();
            }
        }
    }

// ------------------------------------------------------------------------------
// CVcxHgMyVideosMainView::HandleCancelL()
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::HandleCancelL()
    {
    if ( VideoListL()->IsMarking() )
        {
        VideoListL()->HandleMarkCommandL( EVcxHgMyVideosCmdStopMarking );
        }    
    }

// ------------------------------------------------------------------------------
// CVcxHgMyVideosMainView::LaunchAppL()
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::LaunchAppL( const TUid aAppUid, const TDesC& aMsg )
    {
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL<RWsSession>( wsSession );
    TApaTaskList taskList( wsSession );
    TApaTask task = taskList.FindApp( aAppUid );
    TInt msgLen = aMsg.Length();
       
    if ( task.Exists() )
        {
        if ( msgLen > 0 )
            {
            // Send message
            HBufC8* param8 = HBufC8::NewLC( msgLen );
            param8->Des().Append( aMsg );
            task.SendMessage( TUid::Uid( 0 ), *param8 );
            CleanupStack::PopAndDestroy( param8 );
            }
        else
            {
            task.BringToForeground();
            }
        }
    else // Task didn't exist
        {
        RApaLsSession appArcSession;
        User::LeaveIfError( appArcSession.Connect() );
        CleanupClosePushL<RApaLsSession>( appArcSession );
    
        if ( msgLen > 0 )
            {
            TThreadId id;
            appArcSession.StartDocument( aMsg, aAppUid , id ); 
            }
        else
            {
            TApaAppInfo appInfo;
            TInt retVal = appArcSession.GetAppInfo( appInfo, aAppUid );
            if ( retVal == KErrNone )
                {
                CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
                cmdLine->SetExecutableNameL( appInfo.iFullName );
                cmdLine->SetCommandL( EApaCommandRun );
                User::LeaveIfError( appArcSession.StartApp( *cmdLine ) );
                CleanupStack::PopAndDestroy( cmdLine );
                }
            else
                {
                IPTVLOGSTRING_LOW_LEVEL( 
                        "MPX My Videos UI # CVcxHgMyVideosMainView::LaunchAppL - Application not found!" );
                }
            }
        CleanupStack::PopAndDestroy(); // appArcSession
        }
    CleanupStack::PopAndDestroy(); // wsSession
    }

// ------------------------------------------------------------------------------
// CVcxHgMyVideosMainView::LaunchBrowserL()
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::LaunchBrowserL( const TDesC& aUrl )
    {
    HBufC* param = HBufC::NewLC( KVcxHgMyVideosUrlMaxLen );
    param->Des().Copy( KBrowserProtocol() );
    param->Des().Append( aUrl );
    
    LaunchAppL( KBrowserAppUid, *param );
    
    CleanupStack::PopAndDestroy( param );
    }

// ------------------------------------------------------------------------------
// CVcxHgMyVideosMainView::LaunchOviStoreL()
// ------------------------------------------------------------------------------
//
void CVcxHgMyVideosMainView::LaunchOviStoreL( const TUid aAppUid, const TDesC& aArgs )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxHgMyVideosMainView::LaunchOviStoreL() enter" );
    RApaLsSession appArcSession;
    User::LeaveIfError( appArcSession.Connect() );
    CleanupClosePushL<RApaLsSession>( appArcSession );

    TApaAppInfo appInfo;
    TInt err = appArcSession.GetAppInfo( appInfo, aAppUid );
    IPTVLOGSTRING2_LOW_LEVEL( "CVcxHgMyVideosMainView::LaunchOviStoreL() GetAppInfo() error = %d", err );

    if ( KErrNone == err )
        {
        IPTVLOGSTRING3_LOW_LEVEL( "CVcxHgMyVideosMainView::LaunchOviStoreL() launch '%S' with arguments '%S'", &appInfo.iFullName, &aArgs );
        RProcess process;
        User::LeaveIfError( process.Create( appInfo.iFullName, aArgs ) );
        process.Resume();
        process.Close();
        }
    else
        {
        // Launcher not found, use browser until launcher is available
        IPTVLOGSTRING_LOW_LEVEL( "CVcxHgMyVideosMainView::LaunchOviStoreL() launcher not found, launch browser" );
        LaunchBrowserL( KOviVideoStoreURL );
        }

    CleanupStack::PopAndDestroy( &appArcSession );
    }
