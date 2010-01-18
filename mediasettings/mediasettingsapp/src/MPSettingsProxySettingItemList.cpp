/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


// Version : %version: 3 %




// INCLUDE FILES
#include    <eikappui.h>
#include    <akntextsettingpage.h>
#include    <barsread.h>

#include    "mediasettings.hrh"
#include    "MPSettingsProxySettingItemList.h"
#include    "MPSettingsModelForROP.h"
#include	"mpxlog.h"

// CONSTANTS
const   TInt    KMPSettProxyHostListItemId  =   1;
const   TInt    KMPSettProxyPortListItemId  =   2;
const   TInt    KMPSettProxyHostMaxLength = 1000;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::CMPSettingsProxySettingItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsProxySettingItemList::CMPSettingsProxySettingItemList(CMPSettingsModelForROP* aModel) 
	: iModel( aModel), iProxyHostPtr(NULL, 0)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsProxySettingItemList::CMPSettingsProxySettingItemList()"));
    }

// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CMPSettingsProxySettingItemList::ConstructFromResourceL(TInt aResourceId) 
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsProxySettingItemList::ConstructFromResourceL(0x%X)"),aResourceId);
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader, aResourceId);
	ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader
    }

// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CMPSettingsProxySettingItemList::ConstructFromResourceL(TResourceReader& aReader) 
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsProxySettingItemList::ConstructFromResourceL()"));
    LoadAllL();
    CAknSettingItemList::ConstructFromResourceL(aReader);
    SetItemVisibilityL();
    }
  
// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::~CMPSettingsProxySettingItemList
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsProxySettingItemList::~CMPSettingsProxySettingItemList()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsProxySettingItemList::~CMPSettingsProxySettingItemList()"));
	if (iProxyHost) 
		{
		delete iProxyHost;
		}
    }

// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::EditItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsProxySettingItemList::EditItemL(TInt aIndex, TBool aCalledFromMenu)
    {
    MPX_DEBUG3(_L("#MS# CMPSettingsProxySettingItemList::EditItemL(%d,%d)"),aIndex,aCalledFromMenu);
    CAknSettingItem* settingItem = SettingItemArray()->At(aIndex);
    TInt identifier = settingItem->Identifier();

    // Load current value from settings model
//    LoadSettingL(identifier);
//    settingItem->LoadL();

    // Call base call's method
    CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);

    // Store setting value to settings model
    settingItem->StoreL();   
    StoreSettingL(identifier);

    // Hide/unhide Proxy port & Proxy host settings according to Proxy mode.
    if (identifier == EMPSettProxyModeSettingId)
        {
        SetItemVisibilityL();
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::CreateSettingItemL
// -----------------------------------------------------------------------------
//
CAknSettingItem* CMPSettingsProxySettingItemList::CreateSettingItemL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsProxySettingItemList::CreateSettingItemL(%d)"),aSettingId);
    __ASSERT_DEBUG(iProxyHost, User::Leave(KErrAbort));

    CAknSettingItem* settingItem  = NULL;

    switch (aSettingId)
        {
        case EMPSettProxyModeSettingId:
            settingItem = new(ELeave) CAknBinaryPopupSettingItem(aSettingId, iProxyMode);
            break;
        case EMPSettProxyHostSettingId:
            {
            settingItem = new(ELeave) CAknTextSettingItem(aSettingId, iProxyHostPtr);
            settingItem->SetSettingPageFlags(CAknTextSettingPage::EZeroLengthAllowed);
            break;
            }
        case EMPSettProxyPortSettingId:
            settingItem = new(ELeave) CAknIntegerEdwinSettingItem(aSettingId, iProxyPort);
            break;            
        default:
            break;
        }

    return settingItem;   
    }

// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::SetItemVisibilityL
// -----------------------------------------------------------------------------
//
void CMPSettingsProxySettingItemList::SetItemVisibilityL()
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsProxySettingItemList::SetItemVisibilityL() iProxyMode(%d)"),iProxyMode);
    if (iProxyMode)
        {
        SettingItemArray()->At(KMPSettProxyHostListItemId)->SetHidden(EFalse);
        SettingItemArray()->At(KMPSettProxyPortListItemId)->SetHidden(EFalse);
        }
    else
       {
       SettingItemArray()->At(KMPSettProxyHostListItemId)->SetHidden(ETrue);
       SettingItemArray()->At(KMPSettProxyPortListItemId)->SetHidden(ETrue);
       }

    HandleChangeInItemArrayOrVisibilityL();
    }

// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::LoadAllL
// -----------------------------------------------------------------------------
//
void CMPSettingsProxySettingItemList::LoadAllL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsProxySettingItemList::LoadAllL()"));
    iModel->LoadSettingsL(EConfigUser);
    LoadSettingL(EMPSettProxyModeSettingId);
    LoadSettingL(EMPSettProxyHostSettingId);
    LoadSettingL(EMPSettProxyPortSettingId);
    }

// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::LoadSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsProxySettingItemList::LoadSettingL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsProxySettingItemList::LoadSettingL(%d)"),aSettingId);
    switch (aSettingId)
        {
        case EMPSettProxyModeSettingId:
            iModel->GetProxyMode(iProxyMode);
            break;
        case EMPSettProxyHostSettingId:
            {
            if (!iProxyHost)
                {
                iProxyHost = HBufC::NewL(KMPSettProxyHostMaxLength);
                iProxyHostPtr.Set(iProxyHost->Des());
                }

            iModel->GetProxyHostName(iProxyHostPtr);
            break;
            }
        case EMPSettProxyPortSettingId:
            iModel->GetProxyPort(iProxyPort);
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsProxySettingItemList::StoreSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsProxySettingItemList::StoreSettingL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsProxySettingItemList::StoreSettingL(%d)"),aSettingId);
    switch (aSettingId)
        {
        case EMPSettProxyModeSettingId:
            iModel->SetProxyMode(iProxyMode);
            break;
        case EMPSettProxyHostSettingId:
            iModel->SetProxyHostNameL(*iProxyHost);
            break;
        case EMPSettProxyPortSettingId:
            iModel->SetProxyPort(iProxyPort);
            break;
        default:
            break;
        }

    iModel->StoreSettingsL();
    }


//  End of File  
