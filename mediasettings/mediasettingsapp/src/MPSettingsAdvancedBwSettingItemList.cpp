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
* Description:   Setting item list class for Advanced bandwidth setting list view.*
*/


// Version : %version: 5 %




// INCLUDE FILES
#include    <eikappui.h>
#include    <MediaSettings.rsg>
#include    <barsread.h>
#include    <featmgr.h>

#include    "mediasettings.hrh"
#include    "MPSettingsAdvancedBwSettingItemList.h"
#include    "MPSettingsAdvancedBwSettingItem.h"
#include    "MPSettingsModelForROP.h"
#include	"mpxlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::CMPSettingsAdvancedBwSettingItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsAdvancedBwSettingItemList::CMPSettingsAdvancedBwSettingItemList(CMPSettingsModelForROP* aModel) 
	: iModel( aModel)
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItemList::CMPSettingsAdvancedBwSettingItemList()");
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItemList::ConstructFromResourceL(TInt aResourceId) 
    {
	MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwSettingItemList::ConstructFromResourceL(0x%X)"),aResourceId);
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader, aResourceId);
	ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItemList::ConstructFromResourceL(TResourceReader& aReader) 
    {
	MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItemList::ConstructFromResourceL()");
    LoadAllL();

    iModel->GetMaxBwPresetsL(iGRPSMaxPresets, EBearerGPRS);    
    iModel->GetMaxBwPresetsL(iEGRPSMaxPresets, EBearerEGPRS);
    iModel->GetMaxBwPresetsL(iWCDMAMaxPresets, EBearerWCDMA);
    iModel->GetMaxBwPresetsL(iWLANMaxPresets, EBearerWLAN);
    iModel->GetMaxBwPresetsL(iHSDPAMaxPresets, EBearerHSDPA);

    CalculateListPosition(iGPRSBw, iGPRSListValue, iGRPSMaxPresets);
    CalculateListPosition(iEGPRSBw, iEGPRSListValue, iEGRPSMaxPresets);
    CalculateListPosition(iWCDMABw, iWCDMAListValue, iWCDMAMaxPresets);
    CalculateListPosition(iWLANBw, iWLANListValue, iWLANMaxPresets);
    CalculateListPosition(iHSDPABw, iHSDPAListValue, iHSDPAMaxPresets);

    CAknSettingItemList::ConstructFromResourceL(aReader);
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::~CMPSettingsAdvancedBwSettingItemList
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsAdvancedBwSettingItemList::~CMPSettingsAdvancedBwSettingItemList()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItemList::~CMPSettingsAdvancedBwSettingItemList()");
    iGRPSMaxPresets.Close();
    iEGRPSMaxPresets.Close();
    iWCDMAMaxPresets.Close();
    iWLANMaxPresets.Close();
    iHSDPAMaxPresets.Close();
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::EditItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItemList::EditItemL(TInt aIndex, TBool aCalledFromMenu)
    {
	MPX_DEBUG3(_L("#MS# CMPSettingsAdvancedBwSettingItemList::EditItemL(%d,%d)"),aIndex,aCalledFromMenu);
    // Call base class' method
    CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);

    // Store setting value to settings model
    CAknSettingItem* settingItem = SettingItemArray()->At(aIndex);
    TInt identifier = settingItem->Identifier();

    settingItem->StoreL();   
    StoreSettingL(identifier);
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::CreateSettingItemL
// -----------------------------------------------------------------------------
//
CAknSettingItem* CMPSettingsAdvancedBwSettingItemList::CreateSettingItemL(TInt aSettingId)
    {   
	MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwSettingItemList::CreateSettingItemL(%d)"),aSettingId);
    CAknSettingItem* settingItem = NULL;

    switch (aSettingId)
        {
        case EMPSettGPRSBwSettingId:
            settingItem = new(ELeave) CMPSettingsAdvancedBwSettingItem(aSettingId,
                iGRPSMaxPresets, iGPRSBw, iGPRSListValue, R_MPSETT_GPRS_BW_QUERY);
            break;
        case EMPSettEGPRSBwSettingId:
            if (FeatureManager::FeatureSupported(KFeatureIdEdgeKnowledge))
                {
                settingItem = new(ELeave) CMPSettingsAdvancedBwSettingItem(aSettingId,
                    iEGRPSMaxPresets, iEGPRSBw, iEGPRSListValue, R_MPSETT_EGPRS_BW_QUERY);
                }
            break;
        case EMPSettWCDMABwSettingId:
            if (FeatureManager::FeatureSupported(KFeatureIdProtocolWcdma)) 
                {
                settingItem = new(ELeave) CMPSettingsAdvancedBwSettingItem(aSettingId,
                    iWCDMAMaxPresets, iWCDMABw, iWCDMAListValue, R_MPSETT_WCDMA_BW_QUERY);
                }
            break;
        case EMPSettWLANBwSettingId:
            if (FeatureManager::FeatureSupported(KFeatureIdProtocolWlan)) 
                {
                settingItem = new(ELeave) CMPSettingsAdvancedBwSettingItem(aSettingId,
                    iWLANMaxPresets, iWLANBw, iWLANListValue, R_MPSETT_WLAN_BW_QUERY);
                }
            break;
        case EMPSettHSDPABwSettingId:
            if (FeatureManager::FeatureSupported(KFeatureIdHsxpaSupport)) 
                {
                settingItem = new(ELeave) CMPSettingsAdvancedBwSettingItem(aSettingId,
                    iHSDPAMaxPresets, iHSDPABw, iHSDPAListValue, R_MPSETT_HSDPA_BW_QUERY);
                }
            break;
        default:
            break;
        }

    return settingItem;   
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::LoadAllL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItemList::LoadAllL()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItemList::LoadAllL()");
    iModel->LoadSettingsL(EConfigUser);
    LoadSettingL(EMPSettGPRSBwSettingId);
    LoadSettingL(EMPSettEGPRSBwSettingId);
    LoadSettingL(EMPSettWCDMABwSettingId);
    LoadSettingL(EMPSettWLANBwSettingId);
    LoadSettingL(EMPSettHSDPABwSettingId);
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::LoadSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItemList::LoadSettingL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwSettingItemList::LoadSettingL(%d)"),aSettingId);
    switch (aSettingId)
        {
        case EMPSettGPRSBwSettingId:
            iModel->GetMaxBandwidth(iGPRSBw, EBearerGPRS);
            break;
        case EMPSettEGPRSBwSettingId:
            iModel->GetMaxBandwidth(iEGPRSBw, EBearerEGPRS);
            break;
        case EMPSettWCDMABwSettingId:
            iModel->GetMaxBandwidth(iWCDMABw, EBearerWCDMA);
            break;
        case EMPSettWLANBwSettingId:
            iModel->GetMaxBandwidth(iWLANBw, EBearerWLAN);
            break;
        case EMPSettHSDPABwSettingId:
            iModel->GetMaxBandwidth(iHSDPABw, EBearerHSDPA);
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::StoreSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItemList::StoreSettingL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwSettingItemList::StoreSettingL(%d)"),aSettingId);

    switch (aSettingId)
        {
        case EMPSettGPRSBwSettingId:
            {
            iModel->SetMaxBandwidth(iGPRSBw, EBearerGPRS);
            break;
            }
        case EMPSettEGPRSBwSettingId:
            {
            iModel->SetMaxBandwidth(iEGPRSBw, EBearerEGPRS);
            break;
            }
        case EMPSettWCDMABwSettingId:
            {
            iModel->SetMaxBandwidth(iWCDMABw, EBearerWCDMA);
            break;
            }
        case EMPSettWLANBwSettingId:
            {
            iModel->SetMaxBandwidth(iWLANBw, EBearerWLAN);
            break;
            }
        case EMPSettHSDPABwSettingId:
            {
            iModel->SetMaxBandwidth(iHSDPABw, EBearerHSDPA);
            break;
            }
        default:
            break;
        }
    
    iModel->StoreSettingsL();
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItemList::CalculateListPosition
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItemList::CalculateListPosition(TInt aValue,
                                                                 TInt& aListValue,
                                                                 RArray<TInt>& aArray)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwSettingItemList::CalculateListPosition(%d)"),aValue);
    TInt count = aArray.Count();
    aListValue = count;

    for (TInt index = 0; index < count; ++index)
        {
        if (aArray[index] == aValue)
            {
            aListValue = index;
            break;
            }
        }
    }

//  End of File  
