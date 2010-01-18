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




#include <eikenv.h>
#include <eikmenup.h>
#include <aknViewAppUi.h>
#include <StringLoader.h>
#include <SendUiConsts.h>
#include <vcxhgvodui.rsg>
#include <aknnotewrappers.h>
#include <ganes/HgScroller.h>
#include <csxhelp/iptv.hlp.hrh>

#include "vcxnsuiengine.h"
#include "vcxnscategory.h"
#include "vcxnscategoryproviderif.h"
#include "vcxnscontentproviderif.h"
#include "vcxnsservice.h"
#include "vcxnsserviceproviderif.h"

#include "vcxhgvodcontentlistimpl.h"
#include "vcxhgvodlistimplbase.h"

#include "vcxhgvodcontentlistimpl.h"
#include "vcxhgvodcontentlistmodelhandler.h"
#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodui.hrh"
#include "vcxhgvodutils.h"
#include "vcxhgvodsearchimpl.h"


// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::~CVcxHgVodContentListImpl()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodContentListImpl::~CVcxHgVodContentListImpl()
    {
    //always set model null to widget before destroying it
    delete iModelHandler;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgVodContentListImpl* CVcxHgVodContentListImpl::NewL(
    CHgScroller& aScroller, 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodContentListImpl* self = 
        CVcxHgVodContentListImpl::NewLC( 
                aScroller, aDataProviderIf, aView );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodContentListImpl* CVcxHgVodContentListImpl::NewLC(
    CHgScroller& aScroller, 
    CVcxNsUiEngine& aDataProviderIf, 
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodContentListImpl* self = 
        new (ELeave) CVcxHgVodContentListImpl( 
            aScroller, aDataProviderIf, aView );
    CleanupStack::PushL( self );
    self->ConstructL( aDataProviderIf, aView );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CVcxHgVodContentListImpl::DoActivateL( )
    {
    if ( iSearchImpl )
        {
        iSearchImpl->RegisterObserver();
        }
    
    SetMenuResourceId( R_VCXHG_VOD_CONTENTVIEW_MENUBAR );
    SetEmptyListTextL( R_VCXHG_VOD_UPDATING_ITEMS );
    //Set default context menu
    iView.MenuBar()->SetContextMenuTitleResourceId( 
        R_VCXHG_VOD_MSK_CONTENT_CONTEXT_MENUBAR );
    SetMskL( R_VCXHG_VOD_MSK_BUTTON_DO_NOTHING );
        
    iScroller.SetSelectionObserver( *this );
    iView.AppUi()->AddToStackL( &iScroller );
    iScroller.SetFocus( ETrue );
    
    iModelHandler->DoActivateModelL();
    UpdateTitlePaneL();

    CVcxNsService* currentService =
        iModel.GetServiceProviderL()->GetActiveServiceData();

    TUint32 activeCategory = iModel.ActiveCategory();

    if ( ( activeCategory == KVcxNsSearchCategoryId )
            && ( iScroller.ItemCount() == 0 ) )
        {
        // Set empty list text "no videos found" when opened search
        // category and it's empty. 
        SetEmptyListTextL( R_VCXHG_NO_VIDEOS_FOUND );
        }
    else if ( iScroller.ItemCount() == 0 &&
              currentService->GetUpdateTime() != 0 )
        {
        // Set empty list text "no videos" only if content list is empty
        // and service has been updated successfully at least once.
        SetEmptyListTextL( R_VCXHG_VOD_NO_DATA );
        }

    if( currentService &&
        currentService->GetUpdateStatus() == CVcxNsService::EUpdateOngoing )
        {
        ShowInformationPopupL( ETrue, 0, 0 );
        }
    }    

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
void CVcxHgVodContentListImpl::DoDeactivate( )
    {
    //Make sure that MSK is updated when returning to view
    iCurrentMskResource = 0;
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
// CVcxHgVodContentListImpl::CVcxHgVodContentListImpl()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodContentListImpl::CVcxHgVodContentListImpl( 
    CHgScroller& aScroller,
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    : CVcxHgVodListImplBase( aView, aDataProviderIf, aScroller ),
      iEmailAccountEnabled ( EFalse )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::ConstructL( 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    iModelHandler = 
        CVcxHgVodContentListModelHandler::NewL( 
                aDataProviderIf, aView, iScroller, *this );

    CVcxHgVodListImplBase::InitSearchImplL();
    
    iEmailAccountEnabled = CVcxHgVodUtils::IsDefaultSmtpAccountDefinedL();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::HandleCommandL
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::HandleCommandL( TInt aCommand )
    {
    TInt selected = iScroller.SelectedIndex();
    
    MVcxNsContentProviderIf* contentProvider = 
        iModelHandler->ContentProvider();
    
    switch ( aCommand )
        {
        case EVcxHgVodContentStartDownloadCmd:
            {
            if ( selected >= 0 ) 
                {
                contentProvider->StartDownloadL( selected );
                }
            }
            break;
        case EVcxHgVodContentResumeDownloadCmd:
            {
            if ( selected >= 0 )
                {
                contentProvider->ResumeDownloadL( selected );
                }
            }
            break;        
        case EVcxHgVodContentCancelDownloadCmd:
            {
            if ( selected >= 0 )
                {
                contentProvider->CancelDownloadL( selected );
                
                HBufC* text = StringLoader::LoadLC( R_VCXHGVOD_DOWNLOAD_CANCELED );
                CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
                note->ExecuteLD( *text );
                CleanupStack::PopAndDestroy( text );    
                }
            }
            break;
        case EVcxHgVodContentPlayCmd:
            {
            StartPlaybackL();
            }
            break;

        case EVcxHgVodContentNewSearchCmd:
            {
            if ( iSearchImpl )
                {
                iSearchImpl->StartSearchL( EFalse );
                }
            }
            break;
        case EVcxHgVodContentOpenLinkCmd:
            {
            if ( selected >= 0 )
                {
                OpenBrowserLinkL( selected );
                }
            }
            break;
        case EVcxHgVodContentDetailsCmd:
            {
            if ( selected >= 0 )
                {
                iModelHandler->ShowVideoDetailsL( selected );
                }
            }
            break;
        case EVcxHgVodContentSendCmd:
            {
            if ( !iEmailAccountEnabled )
                {
                HandleCommandL( EVcxHgVodContentSendViaMsgCmd );
                }
            else
                {
                // Switching to context sensitive menu.
                iView.MenuBar()->SetContextMenuTitleResourceId( R_VCXHG_VOD_SEND_CONTEXT_MENUBAR );
                iView.MenuBar()->TryDisplayContextMenuBarL();
                iView.MenuBar()->SetContextMenuTitleResourceId( R_VCXHG_VOD_MSK_CONTENT_CONTEXT_MENUBAR );
                
                // Switching back to normal options menu.
                iView.MenuBar()->SetMenuType( CEikMenuBar::EMenuOptions );
                iView.MenuBar()->SetMenuTitleResourceId( R_VCXHG_VOD_CONTENTVIEW_MENUBAR );
                }
            }
            break;
        case EVcxHgVodContentSendViaMsgCmd:
            {
            ShareLinkL( KSenduiMtmSmsUid, selected );
            }
            break;

        case EVcxHgVodContentSendViaEmailCmd:
            {
            ShareLinkL( KSenduiMtmSmtpUid, selected  );
            }
            break;
        case EVcxHgVodContentScheduleDownloadsCmd:
            {
            iView.OpenScheduleSettingsL();
            }
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// Activate usable menu items before showing menu
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::ActivateMenuPaneItemsL( 
    CEikMenuPane* aMenuPane,
    TInt aResourceId )
    {
    if ( aMenuPane )
        {
        if ( aResourceId == R_VCXHG_VOD_CONTENTVIEW_SHARE_LINK_SUBMENU )
            {
            if ( !iEmailAccountEnabled )
                {
                aMenuPane->SetItemDimmed( EVcxHgVodContentSendViaEmailCmd, ETrue );
                }
            }
        if  ( aResourceId == R_VCXHG_VOD_CONTENTVIEW_MENU 
                || aResourceId == R_VCXHG_VOD_CONTENTVIEW_CONTEXT_MENU )
            {
            TBool showPlay( EFalse );
            TBool showDownload( EFalse );
            TBool showResume ( EFalse );
            TBool showCancel( EFalse );
            TBool showOpenLink ( EFalse );
            TBool showDetails( EFalse );
            
            RPointerArray<CVcxNsContent>& contents = 
                iModelHandler->ContentProvider()->GetContentsL();
            TInt highlight = iScroller.SelectedIndex();
            CVcxNsContent::TVcxContentDlStatus dlStatus( CVcxNsContent::EVcxContentDlStateNotFound );
            TUint32 dltypes( 0 );
    
            if( highlight >=0 && highlight < contents.Count() )
                {
                dlStatus = ( contents[highlight] )->GetDlStatus();
                dltypes = ( contents[highlight] )->GetContentTypes();
                 
                if ( ( contents[highlight] )->GetBrowserUrl().Length() > 0 )
                    {
                    showOpenLink = ETrue; 
                    }
                
                if ( !iModelHandler->IsEmptyModel() 
                        && iModelHandler->ContentProvider()->HasVideoDetailsL( highlight ) )
                    {
                    showDetails = ETrue;
                    }
                
                if( dltypes & CVcxNsContent::EVcxContentTypeVideo )
                    {
                    switch ( dlStatus )
                        {
                        case CVcxNsContent::EVcxContentDlStateNone:
                        case CVcxNsContent::EVcxContentDlStateNotFound:
                            {
                            showDownload = ETrue; 
                            }
                            break;
                        case CVcxNsContent::EVcxContentDlStateDownloading:
                            {
                            showCancel = ETrue; 
                            showPlay = ETrue;
                            }
                            break;
                        case CVcxNsContent::EVcxContentDlStateFailed:
                        case CVcxNsContent::EVcxContentDlStatePaused:
                            {
                            showPlay = ETrue;
                            showResume = ETrue;
                            showCancel = ETrue;
                            }
                            break;
                        case CVcxNsContent::EVcxContentDlStateDownloaded:
                            {
                            showPlay = ETrue;
                            }
                            break;
                        case CVcxNsContent::EVcxContentDlStateCanceling:
                            break;
                        default:
                            break;
                        }
                    }
                
                // Play cmd is always visible, if there's stream.
                if( dltypes & CVcxNsContent::EVcxContentTypeStream )
                    {
                    showPlay = ETrue;
                    }
                }
            
            //For both menus:
            aMenuPane->SetItemDimmed( EVcxHgVodContentStartDownloadCmd, !showDownload );
            aMenuPane->SetItemDimmed( EVcxHgVodContentPlayCmd, !showPlay );
            aMenuPane->SetItemDimmed( EVcxHgVodContentResumeDownloadCmd, !showResume );
            aMenuPane->SetItemDimmed( EVcxHgVodContentCancelDownloadCmd, !showCancel );

            
            if  ( aResourceId == R_VCXHG_VOD_CONTENTVIEW_MENU ) 
                {
                aMenuPane->SetItemDimmed( EVcxHgVodContentDetailsCmd, !showDetails );
                aMenuPane->SetItemDimmed( EVcxHgVodContentOpenLinkCmd, !showOpenLink );
                
                if ( iModel.ActiveCategory() == KVcxNsSearchCategoryId )
                    {
                    aMenuPane->SetItemDimmed( EVcxHgVodContentNewSearchCmd, EFalse );
                    }
                else
                    {
                    aMenuPane->SetItemDimmed( EVcxHgVodContentNewSearchCmd, ETrue );
                    }
                
                if ( iModelHandler->IsEmptyModel() )
                    {
                    aMenuPane->SetItemDimmed( EVcxHgVodContentSendCmd, ETrue );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::StartPlaybackL
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::StartPlaybackL()
    {
    RPointerArray<CVcxNsContent>& contents = iModel.GetContentProviderL()->GetContentsL();

    TInt highlight = iScroller.SelectedIndex();

    if( highlight >= 0 && contents.Count() > highlight )
        {
        iView.PlayInternalVideoL( contents[highlight] );
        }
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::HandleSelectL( TInt /*aIndex*/ )
    {
    CheckMskL();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::AakashItemSelectedL
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::HandleOpenL( TInt /*aIndex*/ )
    {
    //This check must be done because ETypeSelect event is received
    //from antriksh even if context menu item defined as msk button.
    if ( iCurrentMskResource == R_VCXHG_VOD_MSK_BUTTON_CTXTMENU
          && ! iView.IsTouchSupported() )
        {
        return;
        }
    
    HandleSelectionL();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::MediaKeyEventL
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::MediaKeyEventL(
    TRemConCoreApiOperationId aOperationId, 
    TRemConCoreApiButtonAction /*aButtonAct*/ )
    {
    if ( iModel.VcAppState() == EStateContentView &&
         ( aOperationId == ERemConCoreApiPause ||
           aOperationId == ERemConCoreApiPlay ||
           aOperationId == ERemConCoreApiPausePlayFunction ) )
        {
        HandleCommandL( EVcxHgVodContentPlayCmd );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::UpdateTitlePaneL
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::UpdateTitlePaneL()
    {
    CVcxNsCategory* currentCategory =
        iModel.GetCategoryProviderL()->GetActiveCategoryDataL();
    
    if ( currentCategory )
        {
        if ( currentCategory->GetCategoryId() == KVcxNsSearchCategoryId )
            {
            HBufC* searchTitle = StringLoader::LoadLC( R_VCXHG_TITLE_VIDEO_SEARCH );
            SetTitlePaneTextL( *searchTitle );
            CleanupStack::PopAndDestroy( searchTitle );
            }
        else
            {
            if( currentCategory->GetName().Length() > 0 )
                {
                SetTitlePaneTextL( currentCategory->GetName());
                }
            else
                {
                SetTitlePaneTextFromResourceL( R_VCXHG_CATEGORIES_VIDEO_LIST );
                }
            }
        }
    else
        {
        CVcxNsService* currentService = 
            iModel.GetServiceProviderL()->GetActiveServiceData();
        
        if ( currentService )
            {
            SetTitlePaneTextL( currentService->GetName());
            }
        }
    }
// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::GetContentStatusDataL
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::ContentStatusDataL(
    CVcxNsContent::TVcxContentDlStatus &aDlStatus,
    TUint32 &aContentTypes )
    {
    RPointerArray<CVcxNsContent>& contents = 
        iModelHandler->ContentProvider()->GetContentsL();
    
    //Default values:
    aDlStatus = CVcxNsContent::EVcxContentDlStateNotFound;
    aContentTypes = 0;

    TInt highlight = iScroller.SelectedIndex();

    if( highlight >= 0 && highlight < contents.Count() )
        {
        aDlStatus = ( contents[highlight] )->GetDlStatus();
        aContentTypes = ( contents[highlight] )->GetContentTypes();
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::OpenBrowserLinkL
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::OpenBrowserLinkL( TInt aSelected )
    {
    if ( aSelected >= 0 && 
            iModelHandler && iModelHandler->ContentProvider())
        {
        RPointerArray<CVcxNsContent>& contents = 
            iModelHandler->ContentProvider()->GetContentsL();

        if( aSelected >= 0 && aSelected < contents.Count() )
            {
            OpenEmbeddedBrowserL( ( contents[aSelected] )->GetBrowserUrl() );
            iModel.SetVcAppState( EStateBrowser );
            }
        
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::ShareLinkL
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::ShareLinkL( 
    const TUid aMessageTypeModule,
    TInt aSelected )
    {
    RPointerArray<CVcxNsContent>& contents = 
        iModelHandler->ContentProvider()->GetContentsL();

    if( aSelected >= 0 && aSelected < contents.Count() )
        {
        TUint32 contentTypes( 0 );
        CVcxNsContent::TVcxNsContentAccessType caType = 
            CVcxNsContent::EVcxContentTypeNoType;
        
        contentTypes = ( contents[aSelected] )->GetContentTypes();
        if ( ( contentTypes & CVcxNsContent::EVcxContentTypeStream ) && 
             ( contents[aSelected] )->GetUrl(  
                   CVcxNsContent::EVcxContentTypeStream ).Length() > 0 )
            {
            caType = CVcxNsContent::EVcxContentTypeStream;
            }
        else if ( ( contentTypes & CVcxNsContent::EVcxContentTypeVideo ) && 
                ( contents[aSelected] )->GetUrl( 
                      CVcxNsContent::EVcxContentTypeVideo ).Length() > 0 )

            {
            caType = CVcxNsContent::EVcxContentTypeVideo;
            }
        
        if ( caType == CVcxNsContent::EVcxContentTypeVideo
                || caType == CVcxNsContent::EVcxContentTypeStream )
            {
            CVcxHgVodUtils::ShareLinkL( 
                ( contents[aSelected] )->GetUrl( caType ), 
                aMessageTypeModule,
                iView.EikonEnv() );
            
            iView.LayoutChanged();
            }
        }
    }

// ---------------------------------------------------------
// CVcxHgVodContentListImpl::CheckMskL
// ---------------------------------------------------------
void CVcxHgVodContentListImpl::CheckMskL()
    {
    TBool showPlay( EFalse );
    TBool showStartDl( EFalse );
    TBool showResumeDl( EFalse );
    
    if ( !iModelHandler->IsEmptyModel() )
        {
        CVcxNsContent::TVcxContentDlStatus dlStatus;
        TUint32 contentTypes;
        ContentStatusDataL( dlStatus, contentTypes );
        
        if( contentTypes & CVcxNsContent::EVcxContentTypeVideo )
            {
            switch ( dlStatus )
                {
                case CVcxNsContent::EVcxContentDlStateNone:
                case CVcxNsContent::EVcxContentDlStateNotFound:
                    {
                    showStartDl = ETrue;
                    }
                    break;
                case CVcxNsContent::EVcxContentDlStateDownloading:
                    {
                    showPlay = ETrue;
                    }        
                    break;
                case CVcxNsContent::EVcxContentDlStateCanceling:
                    {
                    // no handling 
                    }
                    break;
                case CVcxNsContent::EVcxContentDlStateFailed:
                case CVcxNsContent::EVcxContentDlStatePaused:
                    {
                    showResumeDl = ETrue;
                    //Check for progressive playback
                    }
                    break;
                case CVcxNsContent::EVcxContentDlStateDownloaded:
                    {
                    showPlay = ETrue;
                    }
                    break;
                default:
                    break;
                }
            }
        
        if( contentTypes & CVcxNsContent::EVcxContentTypeStream )
            {
            showPlay = ETrue;
            }
        }

    CheckMskL( showStartDl, showPlay, showResumeDl );
    }

// ---------------------------------------------------------
// CVcxHgVodContentListImpl::CheckMskL
// ---------------------------------------------------------
void CVcxHgVodContentListImpl::CheckMskL( 
    TBool aShowDownload,
    TBool aShowPlay,
    TBool aShowResume )
    {
    TInt resourceId = 0;
    TInt numberOfCommands = ( aShowDownload ? 1 : 0 ) +
                            ( aShowPlay ? 1 : 0 ) +
                            ( aShowResume ? 1 : 0 );

    if ( numberOfCommands == 0 )
        {
        resourceId = R_VCXHG_VOD_MSK_BUTTON_DO_NOTHING;
        }
    else if ( numberOfCommands == 1 )
        {
        if ( aShowDownload )
            {
            resourceId = R_VCXHG_VOD_MSK_BUTTON_DOWNLOAD;
            }
        else if ( aShowPlay )
            {
            resourceId = R_VCXHG_VOD_MSK_BUTTON_PLAY;
            }
        else if ( aShowResume )
            {
            resourceId = R_VCXHG_VOD_MSK_BUTTON_RESUME;
            }
        }
    else // numberOfCommands >= 2
        {
        resourceId = R_VCXHG_VOD_MSK_BUTTON_CTXTMENU;
        }

    SetMskL( resourceId );
    }

// -----------------------------------------------------------------------------
// Handle selection
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::HandleSelectionL()
    {
    CVcxNsContent::TVcxContentDlStatus dlStatus;
    TUint32 contentTypes;
    ContentStatusDataL( dlStatus, contentTypes );

    // Play if is or is being downloaded
    if ( contentTypes & CVcxNsContent::EVcxContentTypeVideo &&
         dlStatus != CVcxNsContent::EVcxContentDlStateNone  &&
         dlStatus != CVcxNsContent::EVcxContentDlStateNotFound )
        {
        HandleCommandL( EVcxHgVodContentPlayCmd );
        }
    // Play if has stream url
    else if ( contentTypes & CVcxNsContent::EVcxContentTypeStream )
        {
        HandleCommandL( EVcxHgVodContentPlayCmd );
        }
    // Dl only if no stream url nor being downloaded
    else if ( contentTypes & CVcxNsContent::EVcxContentTypeVideo )
        { 
        HandleCommandL( EVcxHgVodContentStartDownloadCmd );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListImpl::GetHelpContext
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListImpl::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    const TUid KUidIptvMainApp = { 0x102750E2 };
    
    aContext.iMajor   = KUidIptvMainApp;
    aContext.iContext = KIPTV_HLP_VOD;
    }
