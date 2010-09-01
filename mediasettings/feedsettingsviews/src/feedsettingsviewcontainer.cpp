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
* Description:    Container class for Setting view.*
*/




// INCLUDE FILES
#include <eikenv.h>
#include <eikapp.h>
#include <aknlists.h>
#include <barsread.h>
#include <AknIconArray.h>
#include <eikclbd.h>
#include <akntitle.h>
#include <aknmessagequerydialog.h>
#include <aknViewAppUi.h>
#include <aknsettingitemlist.h>

#include "IptvDebug.h"
#include <vcxnssettingsview.rsg>
#include "vcxnsservicesettings.h"

#include "videoplayersettingsengine.h"
#include "CIptvUtil.h"
#include <csxhelp/iptv.hlp.hrh>
#include "feedsettingsview.hrh"
#include "feedsettingsview.h"
#include "feedsettingsviewcontainer.h"
#include "feedsettingsviewaddservicelistbox.h"
#include "feedsettingsviewserviceselection.h"
#include "feedsettingsviewaddoreditservice.h"
#include "MPSettingsConstants.h" 

const TInt KIptvMskPosition = 3;
 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CVcSettingsViewContainer::CVcSettingsViewContainer()
// ---------------------------------------------------------
//
CVcxNsSettingsViewContainer::CVcxNsSettingsViewContainer( CAknViewAppUi& aAppUi,
                                                          CVcxNsSettingsEngine& aAppSettings,
                                                          CVcxNsServiceSettings& aServiceSettings,
                                                          CVcxNsSettingsView* aParentView,
                                                          RFs& aFs)
  : iAppUi(aAppUi),
    iAppSettings(aAppSettings),
    iServiceSettings(aServiceSettings),
    iParentView(aParentView),
    iFs(aFs),
    iActiveList((TSettingsViewActiveList)0)
    {
    __ASSERT_DEBUG(iParentView, User::Panic(KNullDesC, KErrBadHandle));
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::ConstructL()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    // Service selection list
    iServiceSelection = CVcxNsSettingsViewServiceSelection::NewL(this, iServiceSettings );

    // Add service list
    iAddOrEditService = CVcxNsSettingsViewAddOrEditService::NewL(this, iServiceSettings );
    
    SetRect(aRect);
    
    SwitchToServiceSelectionListL();
    ActivateL();
    }


// ---------------------------------------------------------
// CVcSettingsViewContainer::SetTitleL()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::SetTitleL( TInt aResourceId )
    {
    CEikStatusPane* sp = static_cast<CAknAppUi*>(iCoeEnv->AppUi())->StatusPane();

    CAknTitlePane* title = 
        static_cast<CAknTitlePane*>(sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));

    TResourceReader rReader;
    iCoeEnv->CreateResourceReaderLC(rReader, aResourceId );
    title->SetFromResourceL(rReader);
    CleanupStack::PopAndDestroy(); //rReader
    }

// ----------------------------------------------------
// CVcSettingsViewContainer::~CVcSettingsViewContainer()
// ----------------------------------------------------
//
CVcxNsSettingsViewContainer::~CVcxNsSettingsViewContainer()
    {
    delete iServiceSelection;
    delete iAddOrEditService;
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::AppUi()
// ---------------------------------------------------------
//
CAknViewAppUi* CVcxNsSettingsViewContainer::AppUi()
    {
    return &iAppUi;
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::SettingsView()
// ---------------------------------------------------------
//
CVcxNsSettingsView* CVcxNsSettingsViewContainer::SettingsView()
    {
    return iParentView;
    }
   
// ---------------------------------------------------------
// CVcSettingsViewContainer::SettingsView()
// ---------------------------------------------------------
//    
CVcxNsSettingsViewServiceSelection* CVcxNsSettingsViewContainer::ServiceSelection()
    {
    return iServiceSelection;
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::SwitchToServiceSelectionListL()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::SwitchToServiceSelectionListL()
    {
    iAddOrEditService->Deactivate();

    iServiceSelection->LoadL();
    iServiceSelection->ActivateL();

    iActiveList = EServiceSelection;
    DrawNow();    
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::SwitchToServiceSelectionListL()
// Special handler for direct activation of service subscriptions.
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::SwitchToServiceSelectionListL( 
        CIptvService::TServiceType aServiceType )
    {
    iAddOrEditService->Deactivate();

    iServiceSelection->SetServiceType( aServiceType );
    iServiceSelection->LoadL();
    iServiceSelection->ActivateL();
    iServiceSelection->CheckMsk( );

    iActiveList = EServiceSelection;
    DrawNow();    
    }
    
// ---------------------------------------------------------
// CVcSettingsViewContainer::FinishServiceSelectionList()
// Special handler for closing service subscriptions.
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::FinishServiceSelectionList()
    {
    iServiceSelection->Deactivate();
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::SwitchToAddOrEditServiceListL()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::SwitchToAddOrEditServiceListL()
    {
    iServiceSelection->Deactivate();
    iAddOrEditService->ActivateL();    
    
    iActiveList = EAddService;
    DrawNow();    
    }
    
// ---------------------------------------------------------
// CVcSettingsViewContainer::EditCurrentSettingItemListItemL()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::EditCurrentSettingItemListItemL( 
        TBool aCalledFromMenu )
    {
    if (iActiveList == EAddService)
        {
        iAddOrEditService->ListBox()->EditCurrentItemL();
        }

    if ( !aCalledFromMenu )
        {
        // Fixes not updating toggled setting items
        ComponentControl( 0 )->DrawDeferred();
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::ShowVideoServiceDetailsL()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::ShowVideoServiceDetailsL()
    {
    if ( iActiveList == EServiceSelection )
        {
        CIptvService* iptvService = iServiceSelection->GetCurrentServiceL();
        
        if ( iptvService )
            {
            CleanupStack::PushL(iptvService);
            CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
            TDes16& desc = iptvService->GetDesc();
            dlg->SetMessageText(desc);
            CleanupStack::PopAndDestroy(iptvService);

            dlg->ExecuteLD(R_VCSETTINGSVIEW_SERVICEDETAILS_QUERY);
            }
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::GetServiceSelectionServiceType()
// ---------------------------------------------------------
//    
CIptvService::TServiceType CVcxNsSettingsViewContainer::GetServiceSelectionServiceType()
    {
    return iServiceSelection->GetServiceType();
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::LoadAddServiceInformationL()
// iAppSettings -> UI
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::LoadAddServiceInformationL(TBool aAdding)
    {
    iAddOrEditService->LoadL(aAdding, iServiceSelection->GetServiceType());
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::SaveServiceSelectionInformationL()
// UI -> iAppSettings
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::SaveServiceSelectionInformationL()
    {
    iServiceSelection->SaveL();
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::SaveAddServiceInformationL()
// UI -> iAppSettings
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::SaveAddServiceInformationL()
    {
    iAddOrEditService->SaveL();
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::HandleMarkCommandL()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::HandleMarkCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EVcSettingsServiceCmdMark:
            {
            AknSelectionService::HandleMarkableListProcessCommandL(
                                     EAknCmdMark, 
                                     iServiceSelection->ListBox() );
            }
            break;

        case EVcSettingsServiceCmdUnmark:
            {
            AknSelectionService::HandleMarkableListProcessCommandL(
                                     EAknCmdUnmark, 
                                     iServiceSelection->ListBox() );
            }
            break;
        
        default:
            break;
        }
    }



// ---------------------------------------------------------
// CVcSettingsViewContainer::DeleteVideoServiceL()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::DeleteVideoServiceL()
    {
    if (iActiveList == EServiceSelection)
        {    
        iServiceSelection->DeleteCurrentServiceL();
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::ShowMarkForCurrentListItemL()
// ---------------------------------------------------------
//
TBool CVcxNsSettingsViewContainer::ShowMarkForCurrentListItemL()
    {
    return ((!IsCurrentListItemMarked()) && ShowMenuItemForCurrentListItemL(EConst));
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::ShowUnmarkForCurrentListItemL()
// ---------------------------------------------------------
//
TBool CVcxNsSettingsViewContainer::ShowUnmarkForCurrentListItemL()
    {
    return (IsCurrentListItemMarked() && ShowMenuItemForCurrentListItemL(EConst));
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::IsCurrentListItemMarked()
// ---------------------------------------------------------
//
TBool CVcxNsSettingsViewContainer::IsCurrentListItemMarked()
    {
    const CListBoxView::CSelectionIndexArray* indices = NULL;
    TInt                                      index   = KErrNotFound;

    if (iActiveList == EServiceSelection)
        {
        indices = iServiceSelection->ListBox()->SelectionIndexes();
        index   = iServiceSelection->ListBox()->CurrentItemIndex();

        for (TInt i = 0; i < indices->Count(); i++)
            {
            if (((*indices)[i]) == index)
                {
                return ETrue;
                }
            }
        }

    return EFalse;    
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::ShowMenuItemForCurrentListItemL()
// TMenuItemType: EGeneral, EConst, EConstAndInfo, EEditable
// ---------------------------------------------------------
//
TBool CVcxNsSettingsViewContainer::ShowMenuItemForCurrentListItemL(
                                    TMenuItemType aMenuItemType)
    {
    if (iActiveList == EServiceSelection)
        {
        if (iServiceSelection->ListBox()->CurrentItemIndex() >= 0)
            {
            switch (aMenuItemType)
                {
                case EGeneral:
                case EConst:
                default:
                    // No special checks.
                    break;

                case EConstAndInfo:
                    // For const items where info is needed, 
                    // we check that info is available.
                    {
                    CIptvService* iptvService = 
                        iServiceSelection->GetCurrentServiceL();

                    if (iptvService)
                        {
                        CleanupStack::PushL(iptvService);
                        TInt descLength = iptvService->GetDesc().Length();
                        CleanupStack::PopAndDestroy(iptvService);
                        
                        return descLength > 0;
                        }                    
                    }
                    break;

                case EEditable:
                    // For editable items, we need to check 
                    // that service is not read only.
                    {
                    if (iServiceSelection->IsCurrentServiceReadOnlyL())
                        {
                        return EFalse;
                        }                    
                    }
                    break;
                }

            return ETrue;
            }
        }

    return EFalse;     
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::SizeChanged()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::SizeChanged()
    {
    if (/*iSettingItem && iServiceType && */ iServiceSelection && iAddOrEditService )
        {
        iServiceSelection->ListBox()->SetRect(Rect());
        iAddOrEditService->ListBox()->SetRect(Rect());
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::CountComponentControls()
// ---------------------------------------------------------
//
TInt CVcxNsSettingsViewContainer::CountComponentControls() const
    {
    // iSettingItemList, iServiceType, iServiceSelection, 
    // iAddOrEditService or iChangePriorityList.
    return 1;
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::ComponentControl()
// ---------------------------------------------------------
//
CCoeControl* CVcxNsSettingsViewContainer::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            {
            if (iActiveList == EServiceSelection)
                {
                return iServiceSelection->ListBox();
                }
            else if (iActiveList == EAddService)
                {
                return iAddOrEditService->ListBox();
                }
            else
                {
                return NULL;    
                }
            }
        default:
            {
            return NULL;
            }
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::Draw()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::Draw(const TRect& /*aRect*/) const
    {
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::HandleResourceChange()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange(aType);

    iServiceSelection->ListBox()->HandleResourceChange( aType );
    iAddOrEditService->ListBox()->HandleResourceChange( aType );

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;

        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::OfferKeyEventL()
// ---------------------------------------------------------
//
TKeyResponse CVcxNsSettingsViewContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    
    if ( aKeyEvent.iCode == EKeyLeftArrow ||
         aKeyEvent.iCode == EKeyRightArrow)
         {
         return EKeyWasNotConsumed;
         }
    
    if ( iActiveList == EServiceSelection &&
         aType == EEventKey &&
         aKeyEvent.iCode == EKeyOK && 
         iServiceSelection )
        {
        SaveServiceSelectionInformationL();
        return iServiceSelection->ListBox()->OfferKeyEventL( aKeyEvent, aType );
        }

    if ( aType == EEventKeyUp )
        {        
        CheckMsk();
        }

    if ( iActiveList == EServiceSelection && iServiceSelection )
        {
        if ( aKeyEvent.iScanCode == EStdKeyBackspace &&  aType == EEventKeyDown )
            {
            if ( ! iServiceSelection->IsCurrentServiceReadOnlyL() )
                DeleteVideoServiceL();
            return EKeyWasConsumed;
            }
        
        return iServiceSelection->ListBox()->OfferKeyEventL( aKeyEvent, aType );
        }
    else if ( iActiveList == EAddService && iAddOrEditService )
        {
        return iAddOrEditService->ListBox()->OfferKeyEventL( aKeyEvent, aType );
        }

    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::GetHelpContext()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KIptvHelpContextUid;

    switch ( iActiveList )
        {
        case EServiceSelection:
            {
            if ( iServiceSelection->GetServiceType() == CIptvService::EVodCast )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "Settings view ## CVcSettingsViewContainer::GetHelpContext()\
                     KIPTV_HLP_SERVICE_SEL_VODCAST");
                aContext.iContext = KIPTV_HLP_SERVICE_SEL_VODCAST;
                }
            else if ( iServiceSelection->GetServiceType() == CIptvService::EVod )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "Settings view ## CVcSettingsViewContainer::GetHelpContext()\
                     KIPTV_HLP_SERVICE_SEL_VODCAST");
                aContext.iContext = KIPTV_HLP_SERVICE_SEL_VODCAST;
                }
            else if ( iServiceSelection->GetServiceType() == CIptvService::EBrowser )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "Settings view ## CVcSettingsViewContainer::GetHelpContext()\
                     KIPTV_HLP_SERVICE_SELECTION_WEB");
                aContext.iContext = KIPTV_HLP_SERVICE_SELECTION_WEB;
                }
            }
            break;

        case EAddService:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "Settings view ## CVcSettingsViewContainer::GetHelpContext()\
                 KIPTV_HLP_ADD_SERVICE");
            aContext.iContext = KIPTV_HLP_ADD_SERVICE;
            }
            break;

        default:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "Settings view ## CVcSettingsViewContainer::GetHelpContext()\
                KIPTV_HLP_SETTINGS(3)");
            aContext.iContext = KIPTV_HLP_SETTINGS;
            }
            break;
        }
    }
    
// ---------------------------------------------------------
// CVcSettingsViewContainer::LoadAddVodCastFeedL()
// ---------------------------------------------------------
//    
void CVcxNsSettingsViewContainer::LoadAddVodCastFeedL()
    {
    TBool adding( ETrue );
    iAddOrEditService->LoadL( adding, CIptvService::EVodCast );                         
    }

// ---------------------------------------------------------
// CVcxNsSettingsViewContainer::HandleListBoxEventL
// ---------------------------------------------------------
//
void CVcxNsSettingsViewContainer::HandleListBoxEventL( 
        CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    if ( aEventType == EEventEnterKeyPressed   ||
         aEventType == EEventItemSingleClicked ||
         aEventType == EEventItemDoubleClicked  )
        {
        switch ( iActiveList )
            {
            case EServiceSelection:
                // decide wether to handle select, mark or unmark cmd
                if( IsCurrentListItemMarked() )
                    {
                    // item already marked, unmark
                    HandleMarkCommandL( EVcSettingsServiceCmdUnmark );
                    break;
                    }
                else
                    {
                    // item unmarked, mark it
                    HandleMarkCommandL( EVcSettingsServiceCmdMark );
                    break;
                    }                
            default:
                break;
            }
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::ClearMsk()
// ---------------------------------------------------------
//    
void CVcxNsSettingsViewContainer::ClearMsk()
    {
    iCurrentMskResource = 0;                      
    }

// ---------------------------------------------------------
// CVcSettingsViewContainer::CheckMsk()
// ---------------------------------------------------------
//    
void CVcxNsSettingsViewContainer::CheckMsk()
    {
    if ( iActiveList == EAddService   )
        {            
        TInt resourceId( 0 );
        
        if ( ! iParentView ||
             ! iParentView->GetButtonGroupContainer() )
            {
            return;
            }

        resourceId = R_VCSETTINGSVIEW_MSK_BUTTON_CHANGE;
            
        if ( resourceId != iCurrentMskResource )
            {
            iCurrentMskResource = resourceId;
            
            CEikButtonGroupContainer* cba = iParentView->GetButtonGroupContainer();

            TRAPD( err, cba->SetCommandL( KIptvMskPosition, iCurrentMskResource ) );
            if ( err == KErrNone )
                {                            
                cba->DrawDeferred();            
                }
            }
        }
    else if ( iActiveList == EServiceSelection && iServiceSelection)
        {
        iServiceSelection->CheckMsk();
        }
    }

// ---------------------------------------------------------
// CVcxNsSettingsViewContainer::TitleResourceId
// ---------------------------------------------------------
//
TInt CVcxNsSettingsViewContainer::TitleResourceId()
    {
    return R_MPSETT_FEEDS_VIEW_TITLE;
    }

