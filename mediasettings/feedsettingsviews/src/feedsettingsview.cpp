/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    CVcSettingsView view class definitions.*
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
#include <featmgr.h>
#include <hlplch.h>

#include <gstabhelper.h>
#include <videoplayeractivationmessage.h>

#include "IptvDebug.h"
#include <vcxnssettingsview.rsg>
#include "CIptvResourceLoader.h"
#include "vcxnsservicesettings.h"
#include "feedsettingsview.h"
#include "feedsettingsviewcontainer.h"

#include "videoplayersettingsengine.h"

#include <mediasettings.mbg>
#include "GSVideoPlugin.h"

#include "MPSettingsNaviPaneController.h"
#include "MPSettingsAppUi.h"
#include "MPSettingsConstants.h" 

// Consts
_LIT( KVcSettingsViewResource, "\\resource\\apps\\vcxnssettingsview." );
_LIT( KMSettingsFile, "mediasettings.mif" );

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVcxNsSettingsView::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsSettingsView* CVcxNsSettingsView::NewL( )
    {
    CVcxNsSettingsView* self = CVcxNsSettingsView::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsView::NewLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsSettingsView* CVcxNsSettingsView::NewLC( 
        CArrayPtrFlat<MGSTabbedView>* aTabViewArray )
    {
    CVcxNsSettingsView* self = new (ELeave) CVcxNsSettingsView( );
    CleanupStack::PushL( self );

    self->ConstructL( aTabViewArray );
    return self;
    }

// ---------------------------------------------------------
// CVcxNsSettingsView::~CVcxNsSettingsView()
// ---------------------------------------------------------
//
EXPORT_C CVcxNsSettingsView::~CVcxNsSettingsView()
    {
    iFs.Close();

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    
    delete iTabHelper;
    delete iResourceLoader;
    delete iSettingsEngine;
    delete iServiceSettings;
    
    if ( iViewUtility )
        {
        iViewUtility->Close();
        }
    }

// ---------------------------------------------------------
// CVcxNsSettingsView::CVcxNsSettingsView()
// ---------------------------------------------------------
//
EXPORT_C CVcxNsSettingsView::CVcxNsSettingsView( const TUid aViewId )
  : iViewId( aViewId ),
    iServiceSubscriptionMode( EFalse ),
    iVodcastAddFeedMode( EFalse ),
    iGSVideoPluginMode( EFalse )
    {
    }

// ---------------------------------------------------------
// CVcxNsSettingsView::ConstructL()
// ---------------------------------------------------------
//
EXPORT_C void CVcxNsSettingsView::ConstructL( 
        CArrayPtrFlat<MGSTabbedView>* aTabViewArray )
    {
    if ( aTabViewArray )
        {
        aTabViewArray->AppendL(this);
        iGSVideoPluginMode = ETrue;
        iViewArray = aTabViewArray;
        }
    
    if ( iGSVideoPluginMode )
        {
        iTabHelper = CGSTabHelper::NewL();
        }
    
    // Resources must be loaded before base construct.
    iResourceLoader = CIptvResourceLoader::NewL( *iCoeEnv );
    iResourceLoader->AddResourceL( KVcSettingsViewResource );

    BaseConstructL( R_VCSETTINGSVIEW_SETTINGSVIEW );

    User::LeaveIfError( iFs.Connect() );
    
    iSettingsEngine = CVcxNsSettingsEngine::NewL( );
    iServiceSettings = CVcxNsServiceSettings::NewL();
    iViewUtility = MMPXViewUtility::UtilityL();
    }

// ---------------------------------------------------------
// TUid CVcxNsSettingsView::Id()
// Return id of this view instance
// ---------------------------------------------------------
//
TUid CVcxNsSettingsView::Id() const
    {
    //return iViewId;
    return KMPSettFeedsViewId;
    }

// ---------------------------------------------------------
// CCVcxNsSettingsView::IsSettingsViewActive()
// ---------------------------------------------------------
//
TBool CVcxNsSettingsView::IsSettingsViewActive()
    {
    return iContainer != NULL;
    }

// ---------------------------------------------------------
// CCVcxNsSettingsView::HandleCommandL()
// Command handler
// ---------------------------------------------------------
//
EXPORT_C void CVcxNsSettingsView::HandleCommandL(TInt aCommand)
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "Settings view ## CVcxNsSettingsView::HandleCommandL(%d)", aCommand );

    switch ( aCommand )
        {
        // Settings view
        case EAknSoftkeyOpen:
        case EVcSettingsViewCmdOpen:
            break;

        case EVcSettingsServiceCmdSelect:
            if (iContainer)
                {
                iContainer->SaveServiceSelectionInformationL();
                }
            break;

        // Video service selection view
        case EVcSettingsServiceCmdMark:
        case EVcSettingsServiceCmdUnmark:
            if ( iContainer )
                {
                iContainer->HandleMarkCommandL( aCommand );
                }
            break;

        case EVcSettingsServiceCmdVodDetails:
        case EVcSettingsServiceCmdVodcastDetails:
        case EVcSettingsServiceCmdWebLinkDetails:
            if (iContainer)
                {
                iContainer->ShowVideoServiceDetailsL();
                }        
            break;

        case EVcSettingsServiceCmdVodEdit:
        case EVcSettingsServiceCmdVodcastEdit:
            HandleVodAddOrEditCommandL( EFalse );
            break;

        case EVcSettingsServiceCmdVodRemove:
        case EVcSettingsServiceCmdVodcastRemove:
        case EVcSettingsServiceCmdWebLinkRemove:
            if (iContainer)
                {
                iContainer->DeleteVideoServiceL();
                }
            break;

        case EVcSettingsServiceCmdVodAdd:
        case EVcSettingsServiceCmdVodcastAdd:
            HandleVodAddOrEditCommandL( ETrue );
            break;

        // Add/Edit service view
        case EVcSettingsViewAddServiceCmdChange:
        case EAknSoftkeyChange:
            if (iContainer)
                {
                iContainer->EditCurrentSettingItemListItemL(ETrue);
                }
            break;

        // Common
        case EVcSettingsViewCmdHelp:
            {
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
                                                     AppUi()->AppHelpContextL() );
                }
            }
            break;

        case EVcSettingsViewCmdExit:
            {
            if ( iContainer )
                {
                iContainer->SaveServiceSelectionInformationL();
                }
            AppUi()->HandleCommandL( EEikCmdExit );
            }
            break;

        // Softkeys
        case EAknSoftkeyExit:
            {
            AppUi()->HandleCommandL( EEikCmdExit );
            }
            break;

        case EAknSoftkeyBack:
            {
            HandleBackCommandL();
            }
            break;   
                    
        case EVcSettingsViewSoftkeyBack:
            break;

        case EVcSettingsViewSoftkeyMove:
        case EVcSettingsViewSoftkeyOk:
        case EVcSettingsViewSoftkeyCancel:
            break;

        default:
            AppUi()->HandleCommandL( aCommand );
            break;
        }

    if ( iContainer )
        {
        iContainer->CheckMsk();
        }
    }
    
 // ---------------------------------------------------------
// CVcxNsSettingsView::SetServiceSubscriptionMode()
// ---------------------------------------------------------   
void CVcxNsSettingsView::SetServiceSubscriptionMode()
    {
    iServiceSubscriptionMode = ETrue;
    }
    
// ---------------------------------------------------------
// CVcxNsSettingsView::SetVodcastAddFeedMode()
// ---------------------------------------------------------
//
void CVcxNsSettingsView::SetVodcastAddFeedMode()
    {
    iVodcastAddFeedMode = ETrue;
    }

// ---------------------------------------------------------
// CVcxNsSettingsView::HandleClientRectChange()
// ---------------------------------------------------------
//
void CVcxNsSettingsView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------
// CVcxNsSettingsView::GetButtonGroupContainer()
// ---------------------------------------------------------
//
CEikButtonGroupContainer* CVcxNsSettingsView::GetButtonGroupContainer()
    {
    return Cba();
    }

// ---------------------------------------------------------
// CVcxNsSettingsView::DoActivateL
// Activation of this view instance.
// ---------------------------------------------------------
//
EXPORT_C void CVcxNsSettingsView::DoActivateL( 
        const TVwsViewId& aPrevViewId, 
        TUid /*aCustomMessageId*/, 
        const TDesC8& aCustomMessage )
    {
    IPTVLOGSTRING_LOW_LEVEL("Settings view ## CVcxNsSettingsView::DoActivateL()");

    HandleActivationMessageL( aCustomMessage );

    if ( !iGSVideoPluginMode && !iServiceSubscriptionMode && !iVodcastAddFeedMode  )
        {
        iNaviCntrl = static_cast<CMPSettingsAppUi*>(AppUi());
        iNaviPaneContext = iNaviCntrl->MPTabGroup();
        }
    
    // Set correct navi pane
    SetNaviPaneL();
    
    iPrevViewId = aPrevViewId; // Save the previous view id 

    if ( !iContainer )
        {
        
        iContainer = new (ELeave) CVcxNsSettingsViewContainer( 
                                                  *AppUi(), 
                                                  *iSettingsEngine, 
                                                  *iServiceSettings,
                                                  this, 
                                                  iFs );
        
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );

        AppUi()->AddToStackL(*this, iContainer);
        }

    if ( iServiceSubscriptionMode )
        {
        iContainer->SwitchToServiceSelectionListL( CIptvService::EVod );
        }
    else if ( iVodcastAddFeedMode )
        {
        iContainer->LoadAddVodCastFeedL();
        iContainer->SwitchToAddOrEditServiceListL();
        }
    
   
    if ( iServiceSubscriptionMode || iVodcastAddFeedMode )
        {
        // If we come directly from Video Feeds, status pane has flat layout
        iPrevStatusPaneLayout = StatusPane()->CurrentLayoutResId();
        if ( iPrevStatusPaneLayout != R_AVKON_STATUS_PANE_LAYOUT_USUAL
                && iPrevStatusPaneLayout != R_AVKON_STATUS_PANE_LAYOUT_USUAL_EXT )
            {
            StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );        
            }
        }
    iContainer->MakeVisible( ETrue );

    CEikButtonGroupContainer* cba = Cba();
    if ( cba )
        {
        cba->DrawDeferred();
        }
    
    if ( iGSVideoPluginMode )
        {
        CGSVideoPlugin* parent = 
            static_cast<CGSVideoPlugin*> ( AppUi()->View( KGSVideoPluginUid ) );

        if ( parent )
            {
            iTabHelper->CreateTabGroupL( Id(), 
                        static_cast<CArrayPtrFlat<MGSTabbedView> *> (iViewArray) ); 
            }
        }
    iContainer->CheckMsk();
    }

// ---------------------------------------------------------
// CVcxNsSettingsView::DoDeactivate()
// Deactivation of this view instance.
// ---------------------------------------------------------
//
EXPORT_C void CVcxNsSettingsView::DoDeactivate()
    {
    if ( !iServiceSubscriptionMode )
        {
        // Save service selection when switching to another tab.
        TRAP_IGNORE( SaveServiceSelectionL() );
        }
        
    // Must restore status pane layout if going back to Video Feeds.
    // statuspane layout switch uses fonts, this makes sure that 
    // the fonts are still available in environment. This can happen
    // in cases where environment is already destroyed when this
    // is entered.
    if ( ( iServiceSubscriptionMode || iVodcastAddFeedMode ) && iPrevStatusPaneLayout )            
        {
        if ( iPrevStatusPaneLayout != StatusPane()->CurrentLayoutResId() &&
                iCoeEnv->NormalFont() )
            {
            TRAP_IGNORE( StatusPane()->SwitchLayoutL( iPrevStatusPaneLayout ) )
            }
        }
    
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );

        delete iContainer;
        iContainer = NULL;
        }
    
    if ( iTabHelper )
        {
        iTabHelper->RemoveTabGroup();
        }
    }

// ---------------------------------------------------------
// CVcxNsSettingsView::RoutedKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CVcxNsSettingsView::RoutedKeyEventL( const TKeyEvent &aKeyEvent,
                                                  TEventCode aType )
    {
    if (iContainer)
        {
        return iContainer->OfferKeyEventL(aKeyEvent, aType);
        }
    return EKeyWasConsumed;
    }

// ------------------------------------------------------------------------------
// CVcxNsSettingsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
// This function is called by the EIKON framework just before it displays
// a menu pane. Menu is adjusted according to Settings UI state.
// ------------------------------------------------------------------------------
//
EXPORT_C void CVcxNsSettingsView::DynInitMenuPaneL( TInt aResourceId,
                                                    CEikMenuPane* aMenuPane )
    {
    if ( !iContainer )
        {
        // This cleans up the code layout,
        // no need to check this later
        return;
        }
    
    switch ( aResourceId )
        {
        case R_VCSETTINGSVIEW_SETTINGSVIEW_MENU:
            {
            switch ( iContainer->GetActiveList() )
                {
                case EServiceSelection:
                    {
                    aMenuPane->SetItemDimmed( EVcSettingsViewCmdOpen, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsViewCmdChange, ETrue );
                                           
                    aMenuPane->SetItemDimmed(
                        EVcSettingsServiceCmdMark, 
                        !iContainer->ShowMarkForCurrentListItemL() );
                    aMenuPane->SetItemDimmed(
                        EVcSettingsServiceCmdUnmark, 
                        !iContainer->ShowUnmarkForCurrentListItemL() );
                    
                    switch ( iContainer->GetServiceSelectionServiceType() )
                        {
                        case CIptvService::EVod:
                            {
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodDetails, 
                                !iContainer->ShowMenuItemForCurrentListItemL(
                                     CVcxNsSettingsViewContainer::EConstAndInfo ) );
                            aMenuPane->SetItemDimmed(EVcSettingsServiceCmdVodEdit,
                                !iContainer->ShowMenuItemForCurrentListItemL( 
                                    CVcxNsSettingsViewContainer::EEditable ));
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodRemove, 
                                !iContainer->ShowMenuItemForCurrentListItemL(
                                     CVcxNsSettingsViewContainer::EEditable ) );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodAdd, EFalse );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastDetails, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastEdit, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastRemove, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastAdd, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdWebLinkDetails, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdWebLinkRemove, ETrue );
                            }
                            break;

                        case CIptvService::EVodCast:
                            {
                            aMenuPane->SetItemDimmed(EVcSettingsServiceCmdVodcastDetails,
                                !iContainer->ShowMenuItemForCurrentListItemL( 
                                     CVcxNsSettingsViewContainer::EConstAndInfo ));
                            aMenuPane->SetItemDimmed(EVcSettingsServiceCmdVodcastEdit,
                                !iContainer->ShowMenuItemForCurrentListItemL( 
                                     CVcxNsSettingsViewContainer::EEditable ));
                            aMenuPane->SetItemDimmed(EVcSettingsServiceCmdVodcastRemove,
                                !iContainer->ShowMenuItemForCurrentListItemL( 
                                     CVcxNsSettingsViewContainer::EEditable ));
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastAdd, EFalse );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodDetails, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodEdit, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodRemove, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodAdd, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdWebLinkDetails, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdWebLinkRemove, ETrue );
                            }
                            break;

                        case CIptvService::EBrowser:
                            {
                            aMenuPane->SetItemDimmed(EVcSettingsServiceCmdWebLinkDetails, 
                                                     !iContainer->ShowMenuItemForCurrentListItemL(
                                                         CVcxNsSettingsViewContainer::EConstAndInfo));
                            aMenuPane->SetItemDimmed(EVcSettingsServiceCmdWebLinkRemove, 
                                                     !iContainer->ShowMenuItemForCurrentListItemL(
                                                         CVcxNsSettingsViewContainer::EEditable));
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodDetails, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodEdit, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodRemove, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodAdd, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastDetails, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastEdit, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastRemove, ETrue );
                            aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastAdd, ETrue );
                            }
                            break;
                        
                        default:
                            break;
                        }

                    aMenuPane->SetItemDimmed(EVcSettingsViewAddServiceCmdChange, ETrue);

                    aMenuPane->SetItemDimmed(EVcSettingsViewCmdHelp, EFalse);
                    aMenuPane->SetItemDimmed(EVcSettingsViewCmdExit, EFalse);
                    }
                    break;
                    
                case EAddService:
                    {
                    aMenuPane->SetItemDimmed( EVcSettingsViewCmdOpen, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsViewCmdChange, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdMark, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdUnmark, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdMark, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdUnmark, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodDetails, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodEdit, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodRemove, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodAdd, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastDetails, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastEdit, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastRemove, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdVodcastAdd, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdWebLinkDetails, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsServiceCmdWebLinkRemove, ETrue );
                    aMenuPane->SetItemDimmed( EVcSettingsViewAddServiceCmdChange, EFalse );
                    aMenuPane->SetItemDimmed( EVcSettingsViewCmdHelp, EFalse );
                    aMenuPane->SetItemDimmed( EVcSettingsViewCmdExit, EFalse );
                    }
                    break;
                }
            }
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsView::HandleStatusPaneSizeChange
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsSettingsView::HandleStatusPaneSizeChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsView::LaunchedFromGsFw
// -----------------------------------------------------------------------------
//
TBool CVcxNsSettingsView::LaunchedFromGsFw()
    {
    //return Id().iUid == KVcxNsGeneralSettingsPluginUid ? ETrue : EFalse;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsView::Container
// -----------------------------------------------------------------------------
//
CVcxNsSettingsViewContainer* CVcxNsSettingsView::Container()
    {
    return iContainer;
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsView::CreateTabIconL
// -----------------------------------------------------------------------------
//
CGulIcon* CVcxNsSettingsView::CreateTabIconL()
    {
    CGulIcon* icon = NULL;
    TFileName fileName;
    LocateMifFileL( fileName );

    icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDDefault, 
        fileName,
        EMbmMediasettingsQgn_prop_set_mp_video_tab3,
        EMbmMediasettingsQgn_prop_set_mp_video_tab3_mask );

    return icon;
    }       

// -----------------------------------------------------------------------------
// CVcxNsSettingsView::LocateMifFileL
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsView::LocateMifFileL( TFileName& aFileName )
    {
    // Get the path & file name with the drive not specified.
    TParse parse;
    parse.Set( KMSettingsFile, &KDC_APP_BITMAP_DIR, NULL );
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

// ---------------------------------------------------------------------------
// CVcxNsSettingsView::SetNaviPaneL
// ---------------------------------------------------------------------------
//
void CVcxNsSettingsView::SetNaviPaneL( TBool aDisableNaviPane )
    {
    if ( !iGSVideoPluginMode && !iServiceSubscriptionMode && !iVodcastAddFeedMode )
        {
        if ( iNaviPaneContext && !aDisableNaviPane )
            {
            iNaviCntrl->NaviContainer()->PushL( *iNaviPaneContext );
            if ( iNaviPaneContext == iNaviCntrl->MPTabGroup() )
                {
                iNaviCntrl->UpdateTabIndex();
                }
            }
        else 
            {
            iNaviCntrl->NaviContainer()->PushDefaultL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxNsSettingsView::HandleActivationMessageL
// ---------------------------------------------------------------------------
//
void CVcxNsSettingsView::HandleActivationMessageL( const TDesC8& aCustomMessage )
    {
    if ( aCustomMessage.Length() == sizeof( TVideoPlayerActivationMessage ) )
        {
        TVideoPlayerActivationMessage params;
        TPckg<TVideoPlayerActivationMessage> paramsPckg( params );
        paramsPckg.Copy( aCustomMessage );
        
        if ( params.iMsgType == 
            TVideoPlayerActivationMessage::EOpenVCSettingsFeedSubscriptions )
            {
            SetServiceSubscriptionMode();
            }
        else if ( params.iMsgType == 
            TVideoPlayerActivationMessage::EOpenVCSettingsAddFeed )
            {
            SetVodcastAddFeedMode();
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxNsSettingsView::HandleBackCommandL
// ---------------------------------------------------------------------------
//
void CVcxNsSettingsView::HandleBackCommandL()
    {
    if ( iContainer )
        {
        switch ( iContainer->GetActiveList() )
            {
            case EServiceSelection:
                {
                HandleBackCommandForServiceSelectionL();
                }
                break;
                
            case EAddService:
                {
                HandleBackCommandForAddServiceL();
                }
                break;

            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxNsSettingsView::HandleBackCommandForServiceSelectionL
// ---------------------------------------------------------------------------
//
void CVcxNsSettingsView::HandleBackCommandForServiceSelectionL()
    {
    if ( iContainer )
        {
        if ( !iServiceSubscriptionMode )
            {
            // Normal case, Video player settings main view.
            iContainer->SaveServiceSelectionInformationL();
            
            if ( !iGSVideoPluginMode )
                {
                // Activate settings main view
                AppUi()->ActivateLocalViewL( KMPSettMainViewId );
                }
            else 
                {
                AppUi()->ActivateLocalViewL( KGSVideoPluginUid );
                }
            }
        else
            {
            // In Vodcast "feed subscription" mode we switch
            // right back to Vodcast when user closes this view.
    
            iContainer->SaveServiceSelectionInformationL();
            iContainer->FinishServiceSelectionList();

            iViewUtility->ActivatePreviousViewL();
            iServiceSubscriptionMode = EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxNsSettingsView::SaveServiceSelectionL()
// ---------------------------------------------------------------------------
//
void CVcxNsSettingsView::SaveServiceSelectionL()
    {
    if ( iContainer )
        {
        iContainer->SaveServiceSelectionInformationL();
        }
    }

// ---------------------------------------------------------------------------
// CVcxNsSettingsView::HandleBackCommandForAddServiceL
// ---------------------------------------------------------------------------
//
void CVcxNsSettingsView::HandleBackCommandForAddServiceL()
    {
    if ( iContainer )
        {
        if( !iVodcastAddFeedMode )
            {
            //Save new service and switch to service selection view.
            iContainer->SaveAddServiceInformationL();
            iContainer->SwitchToServiceSelectionListL();
            SetNaviPaneL();
            }
        else
            {
            // Save new service and switch back to VOD.
            iContainer->SaveAddServiceInformationL();
            iContainer->FinishServiceSelectionList();

           iViewUtility->ActivatePreviousViewL();
            iVodcastAddFeedMode = EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxNsSettingsView::HandleVodAddOrEditCommand
// ---------------------------------------------------------------------------
//
void CVcxNsSettingsView::HandleVodAddOrEditCommandL( TBool aIsAdding )
    {
    if ( iContainer )
        {
        iContainer->SaveServiceSelectionInformationL();
        iContainer->LoadAddServiceInformationL( aIsAdding ); // EFalse=Edit, ETrue=Add
        SetNaviPaneL( ETrue );
        iContainer->SwitchToAddOrEditServiceListL();
        }        
    }

