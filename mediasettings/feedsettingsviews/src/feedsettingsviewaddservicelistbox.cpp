/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for "Add service list" in Settings UI.*
*/




// INCLUDE FILES
#include <akntextsettingpage.h>
#include <aknViewAppUi.h>

#include "CIptvService.h"
#include "feedsettingsviewaddservicelistbox.h"
#include "feedsettingsviewcontainer.h"
#include "feedsettingsview.hrh"

// CONSTANTS
_LIT(KCompulsoryChar, "*");

// ========================= MEMBER FUNCTIONS ==================================

// ------------------------------------------------------------------------------
// CVcSettingsViewAddServiceListbox::CVcSettingsViewAddServiceListbox()
// ------------------------------------------------------------------------------
//
CVcxNsSettingsViewAddServiceListbox::CVcxNsSettingsViewAddServiceListbox(
                                               CVcxNsSettingsViewContainer* aParent,
                                               TDes& aServiceName,
                                               TDes& aAddress,
                                               TInt& aPluginType )
  : iParent(aParent),
    iAdding(ETrue),
    iServiceName(aServiceName),
    iAddress(aAddress),
    iServicePluginType(aPluginType)
    {
    __ASSERT_DEBUG(iParent, User::Panic(KNullDesC, KErrBadHandle));
    }

// ----------------------------------------------------------------------------
// CVcSettingsViewAddServiceListbox::CreateSettingItemL()
// ----------------------------------------------------------------------------
//
CAknSettingItem* CVcxNsSettingsViewAddServiceListbox::CreateSettingItemL( 
                                                             TInt aIdentifier )
    {
    switch (aIdentifier)
        {
        case EVcSettingsViewAddServiceServiceNameItem:
            {
            iServiceNameItem = new (ELeave) CAknTextSettingItem( aIdentifier,
                                                                 iServiceName );
            return iServiceNameItem;
            }

        case EVcSettingsViewAddServiceAddressItem:
            {
            iAddressItem = new (ELeave) CAknTextSettingItem(aIdentifier, iAddress);
            iAddressItem->SetCompulsoryIndTextL(KCompulsoryChar);
            return iAddressItem;
            }

        case EVcSettingsViewAddServiceServiceTypeItem:
            {
            iPluginTypeItem = 
                new (ELeave) CAknEnumeratedTextPopupSettingItem( aIdentifier, 
                                                                 iServicePluginType );
            return iPluginTypeItem;
            }

        default:
            {
            return NULL;
            }
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewAddServiceListbox::EditCurrentItemL()
// ---------------------------------------------------------
//
void CVcxNsSettingsViewAddServiceListbox::EditCurrentItemL()
    {
    TInt itemIndex = ListBox()->CurrentItemIndex();

    if (itemIndex >= 0)
        {
        iParent->AppUi()->SetOrientationL( 
                              CAknAppUiBase::EAppUiOrientationUnspecified );

        EditItemL(itemIndex, EFalse);
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewAddServiceListbox::LoadAddServiceInformationL()
// iAppSettings -> UI
// ---------------------------------------------------------
//
void CVcxNsSettingsViewAddServiceListbox::LoadAddServiceInformationL(
                                           TBool aAdding,
                                           TDes& aServiceName,
                                           TDes& aAddress,
                                           CIptvService::TServiceType aServiceType,
                                           TInt& aPluginType )
    {
    iAdding            = aAdding;
    iServiceName       = aServiceName;
    iAddress           = aAddress;
    iServiceType       = aServiceType;
    iServicePluginType = aPluginType;

    LoadSettingsL();

    if (iPluginTypeItem)
        {
        if (aServiceType != CIptvService::ELiveTv)
            {
            iPluginTypeItem->SetHidden(ETrue);
            }
        else
            {
            iPluginTypeItem->SetHidden(EFalse);
            }
        HandleChangeInItemArrayOrVisibilityL();    
        iPluginTypeItem->UpdateListBoxTextL();
        }

    if (iServiceNameItem && iAddressItem)
        {
        iServiceNameItem->UpdateListBoxTextL();
        iAddressItem->SetCompulsoryIndTextL(KCompulsoryChar);
        iAddressItem->UpdateListBoxTextL();
        }
    }

// ---------------------------------------------------------
// CVcSettingsViewAddServiceListbox::SaveAddServiceInformationL()
// UI -> iAppSettings
// ---------------------------------------------------------
//
void CVcxNsSettingsViewAddServiceListbox::SaveAddServiceInformationL()
    {
    StoreSettingsL();
    }

// ---------------------------------------------------------
// CVcSettingsViewAddServiceListbox::IsAdding()
// ---------------------------------------------------------
//
TBool CVcxNsSettingsViewAddServiceListbox::IsAdding()
    {
    return iAdding;
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewAddServiceListbox::SizeChanged()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewAddServiceListbox::SizeChanged()
    {
    CAknSettingItemList::SizeChanged();

    if (ListBox()) 
        {
        ListBox()->SetRect(Rect());
        }
    }
// -----------------------------------------------------------------------------
// CVcSettingsViewAddServiceListbox::OfferKeyEventL()
// -----------------------------------------------------------------------------
//
TKeyResponse CVcxNsSettingsViewAddServiceListbox::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    return CAknSettingItemList::OfferKeyEventL(aKeyEvent, aType);
    }


// --------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::HandleListBoxEventL()
// --------------------------------------------------------------------------
//
void CVcxNsSettingsViewAddServiceListbox::HandleListBoxEventL(
        CEikListBox* aListBox,
        TListBoxEvent aListBoxEvent )
    {
    if ( aListBoxEvent == EEventEnterKeyPressed   ||
         aListBoxEvent == EEventItemSingleClicked ||
         aListBoxEvent == EEventItemDoubleClicked  )
        {
        CAknSettingItemList::HandleListBoxEventL( aListBox, aListBoxEvent );
        if ( iParent )
		    {
		    iParent->DrawDeferred();
		    }	
        }
    }

