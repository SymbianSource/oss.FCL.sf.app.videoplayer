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
* Description:   Setting item list class for Video settings list view.*
*/


// Version : %version: 20 %




// INCLUDE FILES
#include <eikappui.h>
#include <MediaSettings.rsg>
#include <StringLoader.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmapplicationsettingsui.h>

#include "MediaPlayerVariant.hrh" 
#include "MediaPlayerPrivateCRKeys.h"
#include "mediasettings.hrh"
#include "MPSettingsVideoSettingItemList.h"
#include "MPSettingsSliderSettingItem.h"
#include "MPSettingsModelForROP.h"
#include "MPSettingsAppUi.h" 
#include "videoplayersettingsengine.h"
#include "videoplayercenrepkeys.h"
#include "mpxlog.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::CMPSettingsVideoSettingItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsVideoSettingItemList::CMPSettingsVideoSettingItemList(
        CMPSettingsModelForROP* aModel, 
        CVcxNsSettingsEngine& aVideoSettingsEngine) 
    : iModel( aModel ), 
      iVideoSettingsEngine(aVideoSettingsEngine), 
      iRockerKeysSupported( EFalse )
    {
    CMPSettingsAppUi* AppUi =
            static_cast<CMPSettingsAppUi*> (CCoeEnv::Static()->AppUi());
    iLandscapeSupportedByDevice = AppUi->OrientationCanBeChanged();
    
    TBool rockerSupported = EFalse;
    TRAP_IGNORE( rockerSupported = iModel->IsRockerKeysSupportedL() );
    if ( rockerSupported )
        {
        iRockerKeysSupported = ETrue;
        }
    MPX_DEBUG3(_L("#MS# CMPSettingsVideoSettingItemList::CMPSettingsVideoSettingItemList() iLandscapeSupportedByDevice(%d) iRockerKeysSupported(%d) "),iLandscapeSupportedByDevice,iRockerKeysSupported );
    }
    
// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::~CMPSettingsVideoSettingItemList
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsVideoSettingItemList::~CMPSettingsVideoSettingItemList()
    {   
    MPX_DEBUG1(_L("#MS# CMPSettingsVideoSettingItemList::~CMPSettingsVideoSettingItemList()"));
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::EditItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::EditItemL(TInt aIndex, TBool aCalledFromMenu)
    {
    MPX_DEBUG3(_L("#MS# CMPSettingsVideoSettingItemList::EditItemL(%d,%d)"),aIndex,aCalledFromMenu);

    TInt itemIndex = aIndex;
    TBool storeModifiedSetting( ETrue );
    
    CAknSettingItem* settingItem = SettingItemArray()->At( itemIndex );
    TInt identifier = settingItem->Identifier();
    
    // Load current value from shared data
    switch ( identifier )
         {
         case EMPSettVideoContrastSettingId:
            {
            CAknSettingItemList::EditItemL( itemIndex, aCalledFromMenu );
            break;
            }

         default:
            break;
         }
    
    if ( storeModifiedSetting )
        {
         // Store the modified setting
        switch ( itemIndex )
            {
            case EVcSettingsViewParentControlIndex:
            case EVcSettingsViewMemoryItemIndex:
            case EVcSettingsViewThumbnailsIndex:
                break;
                
            default:
                {
                // Store setting value to settings model
                settingItem->StoreL();
                StoreSettingL( identifier );
                }
                break;
            }
        }
    }

// --------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::HandleListBoxEventL()
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::HandleListBoxEventL(
    CEikListBox* aListBox,
    TListBoxEvent aListBoxEvent )
    {
    if ( aListBoxEvent == EEventEnterKeyPressed   ||
         aListBoxEvent == EEventItemSingleClicked ||
         aListBoxEvent == EEventItemDoubleClicked )
        {
        TInt itemIndex = SettingItemArray()->ItemIndexFromVisibleIndex(
            aListBox->CurrentItemIndex() );

        CAknSettingItemList::HandleListBoxEventL( aListBox, aListBoxEvent );
        }
    }

// --------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::EditCurrentItemL()
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::EditCurrentItemL( TBool aCalledFromMenu )
    {
    TInt itemIndex = SettingItemArray()->ItemIndexFromVisibleIndex(
            ListBox()->CurrentItemIndex() );
   
    EditItemL( itemIndex, aCalledFromMenu );
    }

// --------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::IsCurrentItem()
// --------------------------------------------------------------------------
//
TBool CMPSettingsVideoSettingItemList::IsCurrentItem( TInt aIndex )
    {
    TBool retValue( EFalse ); 
    TInt currentIndex = SettingItemArray()->ItemIndexFromVisibleIndex(
            ListBox()->CurrentItemIndex() );
   
    CAknSettingItem* settingItem = SettingItemArray()->At( currentIndex );
    TInt identifier = settingItem->Identifier();
    if ( aIndex == identifier )
        {
        retValue = ETrue;
        }
    
    return retValue;
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::LoadSettingsL
// -----------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::LoadSettingsL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsVideoSettingItemList::LoadSettingsL()"));
    // Get setting values from settings model
    LoadAllL();

    CAknSettingItemList::LoadSettingsL();
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::CreateSettingItemL
// -----------------------------------------------------------------------------
//
CAknSettingItem* CMPSettingsVideoSettingItemList::CreateSettingItemL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsVideoSettingItemList::CreateSettingItemL(%d)"),aSettingId);
    CAknSettingItem* settingItem = NULL;

    switch (aSettingId)
        {
        case EMPSettVideoContrastSettingId:
            {
            settingItem = new (ELeave) CMPSettingsSliderSettingItem(
                aSettingId, iVideoContrast,
                R_MPSETT_CONTRAST_SLIDER_LIST_VALUE, EFalse);
            if ( ! VideoContrastIsSupportedL() )
			    {
                settingItem->SetHidden( ETrue );
				}
            break;
            }
        default:
            break;
        }

    return settingItem;   
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::LoadSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::LoadSettingL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsVideoSettingItemList::LoadSettingL(%d)"),aSettingId);
    switch (aSettingId)
        {
        case EMPSettVideoContrastSettingId:
            iModel->GetVideoContrast(iVideoContrast);
            break;
            
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::StoreSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::StoreSettingL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsVideoSettingItemList::StoreSettingL(%d)"),aSettingId);
    switch (aSettingId)
        {
        case EMPSettVideoContrastSettingId:
            iModel->SetVideoContrast(iVideoContrast);
            break;

        default:
            break;
        }

    iModel->StoreSettingsL();
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::LoadAllL
// -----------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::LoadAllL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsVideoSettingItemList::LoadAllL()"));
    iModel->LoadSettingsL(EConfigUser);
    LoadSettingL(EMPSettVideoContrastSettingId);
    }

// --------------------------------------------------------------------------
// Check if video contrast is supported.
// --------------------------------------------------------------------------
//
TBool CMPSettingsVideoSettingItemList::VideoContrastIsSupportedL()
    {
    // As video contrast is only item in video settings list, 
    // we can assume here that contrast is supported, because
    // it has been already checked in AppUi/GsPlugin level.
    return ETrue;
    
    }

//  End of File  
