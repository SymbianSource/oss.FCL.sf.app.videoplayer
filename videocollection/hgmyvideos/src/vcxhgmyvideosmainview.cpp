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


// Version : %version: TB92_33.1.1 %

// INCLUDE FILES
#include <bldvariant.hrh>
#include <aknViewAppUi.h>
#include <avkon.hrh>
#include <StringLoader.h>
#include <coeaui.h>
#include <aknnotewrappers.h>
#include <hlplch.h>

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
        CHgDoubleTextList* hgCategoryList = 
            CHgDoubleTextList::NewL( ClientRect(), 0 );
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
    UpdateLayout(); 
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
                ActivateVideoListL( highlight );
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
        case EVcxHgMyVideosCmdResumeDownload:
            {
            if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
                {
                VideoListL()->ResumeDownloadL();
                }
            }
            break;
        case EVcxHgMyVideosCmdCancelDownload:
            {
            if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
                {
                VideoListL()->CancelDownloadL();
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
            VideoListL()->HandleSendL();
            break;
            }        
        case EVcxHgMyVideosCmdMark:
        case EVcxHgMyVideosCmdUnmark:
        case EVcxHgMyVideosCmdMarkAll:
        case EVcxHgMyVideosCmdUnmarkAll:
        case EVcxHgMyVideosCmdToggleMultipleMarking:        
            {
            VideoListL()->HandleMarkCommandL( aCommand );
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
    
    // Clear key performs either 'Delete' or 'Cancel Download'. As Cancel Download
    // cannot be performed if there are selections, selections always cause Delete.
    // When there is no selections, video's status dictates the operation.
    if ( aKeyEvent.iScanCode == EStdKeyBackspace && aType == EEventKey )
        {
        if ( iModel->AppState() == CVcxHgMyVideosModel::EVcxMyVideosAppStateVideoIdle )
            {
            RArray<TInt> markedVideos;
            CleanupClosePushL( markedVideos );
            
            VideoListL()->VideoModel().MarkedVideosL( markedVideos );
            TInt count = markedVideos.Count();
			CleanupStack::PopAndDestroy( &markedVideos );
			
            if ( ( count > 0 ) ||
                 ( VideoListL()->VideoModel().VideoDownloadState( VideoListL()->Highlight() )
                   == EVcxMyVideosDlStateNone ) )
                {
                HandleCommandL( EVcxHgMyVideosCmdDelete );
                }
            else
                {
                HandleCommandL( EVcxHgMyVideosCmdCancelDownload );
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
    TBool showMoveAndCopySubmenu = EFalse;
    TBool showCopy = EFalse;
    TBool showMove = EFalse;

    if ( aResourceId == R_VCXHGMYVIDEOS_MAINVIEW_MENU )
        {
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdDelete, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdResumeDownload, ETrue );
        aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdCancelDownload, ETrue );
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
            VideoListL()->ShowMoveAndCopyMenuItemsL( showMoveAndCopySubmenu, showCopy, showMove );
            
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdPlay, EFalse );
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdCopy, !showCopy );
            aMenuPane->SetItemDimmed( EVcxHgMyVideosCmdMove, !showMove );
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
        rskToSet = R_VCXHGMYVIDEOS_RSK_BACK;
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
