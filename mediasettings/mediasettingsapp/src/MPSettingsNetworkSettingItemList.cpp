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
* Description:   Setting item list class for Network setting list view.*
*/


// Version : %version: 9 %




// INCLUDE FILES
#include    <eikappui.h>
#include    <MediaSettings.rsg>
#include    <barsread.h>

#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmapplicationsettingsui.h>

#include "mediasettings.hrh"
#include "MPSettingsNetworkSettingItemList.h"
#include "MPSettingsSliderSettingItem.h"
#include "MPSettingsUDPPortSettingItem.h"
#include "MPSettingsModelForROP.h"
#include "mpxlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::CMPSettingsNetworkSettingItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsNetworkSettingItemList::CMPSettingsNetworkSettingItemList(CMPSettingsModelForROP* aModel) 
	: iModel( aModel)
    {
	MPX_FUNC("#MS# CMPSettingsNetworkSettingItemList::CMPSettingsNetworkSettingItemList()");
    }
    
// -----------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CMPSettingsNetworkSettingItemList::ConstructFromResourceL(TInt aResourceId) 
    {
	MPX_DEBUG2(_L("#MS# CMPSettingsNetworkSettingItemList::ConstructFromResourceL(0x%X)"),aResourceId);
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader, aResourceId);
	ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader
    }

// -----------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CMPSettingsNetworkSettingItemList::ConstructFromResourceL(TResourceReader& aReader) 
    {
    MPX_FUNC("#MS# CMPSettingsNetworkSettingItemList::ConstructFromResourceL()");
    LoadAllL();
    CAknSettingItemList::ConstructFromResourceL(aReader);
    }

// -----------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::~CMPSettingsNetworkSettingItemList
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsNetworkSettingItemList::~CMPSettingsNetworkSettingItemList()
    {  
    MPX_FUNC("#MS# CMPSettingsNetworkSettingItemList::~CMPSettingsNetworkSettingItemList()");
    }

// -----------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::EditItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsNetworkSettingItemList::EditItemL(TInt aIndex, TBool aCalledFromMenu)
    {
    MPX_DEBUG3(_L("#MS# CMPSettingsNetworkSettingItemList::EditItemL(%d,%d)"),aIndex,aCalledFromMenu);
    CAknSettingItem* settingItem = SettingItemArray()->At(aIndex);
    TInt identifier = settingItem->Identifier();

    if ( identifier == EMPSettDefaultAPSettingId )
        {
        LaunchConnectionSelectionL();
        }
    else
        {
        // Call base call's method
        CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
        }

    // Store setting value to settings model
    settingItem->StoreL();   
    StoreSettingL(identifier);
    }

// -----------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::CreateSettingItemL
// -----------------------------------------------------------------------------
//
CAknSettingItem* CMPSettingsNetworkSettingItemList::CreateSettingItemL(TInt aSettingId)
    {   
    MPX_DEBUG2(_L("#MS# CMPSettingsNetworkSettingItemList::CreateSettingItemL(%d)"),aSettingId);
    CAknSettingItem* settingItem = NULL;

    switch (aSettingId)
        {
        case EMPSettDefaultAPSettingId:        
            iSettingConnectionItem = new (ELeave) CAknTextSettingItem( aSettingId, iSelectedApName);
            settingItem = iSettingConnectionItem;
            break;
        
        /*case EMPSettConnTimeoutSettingId:
            settingItem = new(ELeave) CMPSettingsSliderSettingItem(aSettingId, iConnTimeout,
                                        R_MPSETT_TIMEOUT_SLIDER);
            break;
        case EMPSettServerTimeoutSettingId:
            settingItem = new(ELeave) CMPSettingsSliderSettingItem(aSettingId, iSrvTimeout,
                                        R_MPSETT_TIMEOUT_SLIDER);
            break;*/
        case EMPSettMinUDPPortSettingId:
            settingItem = new(ELeave) CMPSettingsUDPPortSettingItem(aSettingId, iMinUDPPort, iMaxUDPPort);
            break;            
        case EMPSettMaxUDPPortSettingId:
            settingItem = new(ELeave) CMPSettingsUDPPortSettingItem(aSettingId, iMaxUDPPort, iMinUDPPort);
            break;            
        default:
            break;
        }

    return settingItem;   
    }

// -----------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::LoadAllL
// -----------------------------------------------------------------------------
//
void CMPSettingsNetworkSettingItemList::LoadAllL()
    {
    MPX_FUNC("#MS# CMPSettingsNetworkSettingItemList::LoadAllL()");
    iModel->LoadSettingsL(EConfigUser);
    LoadSettingL(EMPSettDefaultAPSettingId);
    LoadSettingL(EMPSettConnTimeoutSettingId);
    LoadSettingL(EMPSettServerTimeoutSettingId);
    LoadSettingL(EMPSettMinUDPPortSettingId);
    LoadSettingL(EMPSettMaxUDPPortSettingId);
    }

// -----------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::LoadSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsNetworkSettingItemList::LoadSettingL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsNetworkSettingItemList::LoadSettingL(%d)"),aSettingId);
    switch (aSettingId)
        {
        case EMPSettDefaultAPSettingId:
            {
            iModel->GetDefaultAp( iDefaultAP );

            if ( iDefaultAP != 0 ) 
                {
                RCmManagerExt cmManager;
                CleanupClosePushL( cmManager );
                cmManager.OpenL();
        
                HBufC* tmpBuf = cmManager.GetConnectionMethodInfoStringL( iDefaultAP, CMManager::ECmName);
                if( tmpBuf && tmpBuf->Length() < KMaxApNameLength )
                    {
                    iSelectedApName = *tmpBuf;
                    }
                CleanupStack::PopAndDestroy( &cmManager );
                }
            }
            break;
        case EMPSettConnTimeoutSettingId:
            iModel->GetConnectionTimeout(iConnTimeout);
            break;
        case EMPSettServerTimeoutSettingId:
            iModel->GetServerTimeout(iSrvTimeout);
            break;
        case EMPSettMinUDPPortSettingId:
            iModel->GetMinUDPPort(iMinUDPPort);
            break;
        case EMPSettMaxUDPPortSettingId:
            iModel->GetMaxUDPPort(iMaxUDPPort);
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::StoreSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsNetworkSettingItemList::StoreSettingL(TInt aSettingId)
    {
	MPX_DEBUG2(_L("#MS# CMPSettingsNetworkSettingItemList::StoreSettingL(%d)"),aSettingId);
    switch (aSettingId)
        {
        case EMPSettDefaultAPSettingId:
            iModel->SetDefaultAp(iDefaultAP);
            break;
        case EMPSettConnTimeoutSettingId:
            iModel->SetConnectionTimeout(iConnTimeout);
            break;
        case EMPSettServerTimeoutSettingId:
            iModel->SetServerTimeout(iSrvTimeout);
            break;
        case EMPSettMinUDPPortSettingId:
            iModel->SetMinUDPPort(iMinUDPPort);
            break;
        case EMPSettMaxUDPPortSettingId:
            iModel->SetMaxUDPPort(iMaxUDPPort);
            break;
        default:
            break;
        }
    
    iModel->StoreSettingsL();
    }

// --------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::HandleListBoxEventL()
// --------------------------------------------------------------------------
//
void CMPSettingsNetworkSettingItemList::HandleListBoxEventL(
    CEikListBox* aListBox,
    TListBoxEvent aListBoxEvent )
    {
    if ( ( aListBoxEvent == EEventEnterKeyPressed ) ||
	     ( aListBoxEvent == EEventItemSingleClicked ) ||
         ( aListBoxEvent == EEventItemDoubleClicked ) )
        {
        CAknSettingItemList::HandleListBoxEventL( aListBox, aListBoxEvent );
        }
    }

// --------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::LaunchConnectionSelectionL()
// --------------------------------------------------------------------------
//
void CMPSettingsNetworkSettingItemList::LaunchConnectionSelectionL()
    {
    MPX_FUNC("#MS# CMPSettingsNetworkSettingItemList::LaunchConnectionSelectionL()");
    
    CCmApplicationSettingsUi* settingsUi = CCmApplicationSettingsUi::NewL();
    CleanupStack::PushL ( settingsUi );
    
    TCmSettingSelection selection; 
    selection.iId = 0;
    selection.iResult = CMManager::EConnectionMethod;
    
    TUint listItems = CMManager::EShowConnectionMethods;
    
    TBearerFilterArray filters;
    
    TBool retVal = settingsUi->RunApplicationSettingsL( selection, listItems, filters );

    if ( retVal )
        {
        switch ( selection.iResult )
            {
            case CMManager::EDestination:
            case CMManager::EAlwaysAsk:
                break;

            case CMManager::EConnectionMethod:
                {
                iDefaultAP = selection.iId;
                }
                break;

            default:
                break;
            }
        
        UpdateSelectedConnectionNameL( );
        
        if ( iSettingConnectionItem )
            {
            iSettingConnectionItem->LoadL ( );
            iSettingConnectionItem->UpdateListBoxTextL ( );
            }
        DrawDeferred();
        }
    
    CleanupStack::PopAndDestroy ( settingsUi );
    }

// --------------------------------------------------------------------------
// CMPSettingsNetworkSettingItemList::UpdateSelectedConnectionNameL
// --------------------------------------------------------------------------
//
void CMPSettingsNetworkSettingItemList::UpdateSelectedConnectionNameL()
    {
    MPX_FUNC("#MS# CMPSettingsNetworkSettingItemList::UpdateSelectedConnectionNameL()");
    
    RCmManagerExt cmManager;
    RCmConnectionMethodExt connMethod;

    CleanupClosePushL( cmManager );
    CleanupClosePushL( connMethod );
    
    cmManager.OpenL();

    // This leaves if conn method is not found.
    TRAPD( leave, connMethod = cmManager.ConnectionMethodL( iDefaultAP ) );
    if ( KErrNone == leave )
        {
        // HBufC* tmpBuf = destination.NameLC();
        HBufC* tmpBuf = cmManager.GetConnectionMethodInfoStringL( iDefaultAP, CMManager::ECmName);

        if( tmpBuf && tmpBuf->Length() < KMaxApNameLength )
            {
            iSelectedApName = *tmpBuf;
            }
        }

    CleanupStack::PopAndDestroy( &connMethod );
    CleanupStack::PopAndDestroy( &cmManager );
    }
//  End of File  
