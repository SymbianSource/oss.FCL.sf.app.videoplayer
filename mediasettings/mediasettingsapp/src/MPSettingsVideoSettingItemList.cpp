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


// Version : %version: 19 %




// INCLUDE FILES
#include <eikappui.h>
#include <MediaSettings.rsg>
#include <StringLoader.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <AknCommonDialogsDynMem.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
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


const TInt KParentControlAdult( 18 );
const TInt KParentControlMinAge( 4 );
const TInt KParentControlIndexShift( 3 );
const TInt KParentControlMaxIndex( 15 );
const TInt KParentControlSelections( 16 );
const TInt KParentControlOff( 0 );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::CMPSettingsVideoSettingItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsVideoSettingItemList::CMPSettingsVideoSettingItemList(
        CMPSettingsModelForROP* aModel, CVcxNsSettingsEngine& aVideoSettingsEngine) :
    iModel(aModel), iVideoSettingsEngine(aVideoSettingsEngine), iRockerKeysSupported(EFalse)
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
    delete iDriveMonitor;
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
        
         case EVcSettingsViewParentControlItem:     
            {
            EditParentControlItemL( itemIndex, aCalledFromMenu );
            break;
            }
        
         case EVcSettingsViewMemoryItem:
            {
            EditPreferredMemoryItemL( itemIndex, aCalledFromMenu );
            break;    
            }
          
         case EVcSettingsViewThumbnails:    
            {
            EditThumbnailItemL( itemIndex, aCalledFromMenu );
            break;
            }
         case EVcSettingsviewIadUpdateItem:     
            {
            // Not editable
            storeModifiedSetting = EFalse;
            }
            break;
         
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

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::EditThumbnailItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::EditThumbnailItemL( TInt itemIndex, TBool aCalledFromMenu )
    {
    if ( aCalledFromMenu )
        {
        CAknSettingItemList::EditItemL( itemIndex, aCalledFromMenu);
        (*SettingItemArray())[itemIndex]->StoreL();
        }
    else
        {
        if (iSettingThumbnails)
            {
            iSettingThumbnails = 0;
            }
        else
            {
            iSettingThumbnails = 1;
            }
        (*SettingItemArray())[itemIndex]->LoadL();
        (*SettingItemArray())[itemIndex]->UpdateListBoxTextL();
        }
    
        iVideoSettingsEngine.SetVideoOnDemandThumbnails(
                        iSettingThumbnails);
        DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::EditParentControlItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::EditParentControlItemL( 
    TInt itemIndex, 
    TBool aCalledFromMenu )
    {
    if (!iLockCodeAccepted)
        {
        iLockCodeAccepted = iVideoSettingsEngine.CheckLockCodeL();
        }
    if (iLockCodeAccepted)
        {
        // can edit setting
        iParentControlIndex = GetCurrentParentControl();
        CAknSettingItemList::EditItemL( itemIndex, aCalledFromMenu );
        (*SettingItemArray())[itemIndex]->StoreL();
        }
    
    SetCurrentParentControl( iParentControlIndex );
    iParentControlIndex = GetCurrentParentControl();
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::EditPreferredMemoryItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::EditPreferredMemoryItemL( 
        TInt itemIndex, 
        TBool 
#ifndef RD_MULTIPLE_DRIVE_ENABLERS
        aCalledFromMenu 
#endif 
        )
    {
    TInt drive;
    iVideoSettingsEngine.GetUsedMemoryL(drive);
    iSettingMemory = iDriveMonitor->ArrayPos(drive);
    if (iSettingMemory == KErrNotFound)
        {
        iSettingMemory = 0;
        }

#ifndef RD_MULTIPLE_DRIVE_ENABLERS
    const TInt KThree( 3 );
    if ( iDriveMonitor->iAvailableDrives.Count() < KThree && !aCalledFromMenu )
        {
        if ( iSettingMemory == 1 )
            {
            iSettingMemory = 0;
            }
        else
            {
            iSettingMemory = 1;
            }
        }
    else
        {
        CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
        ( *SettingItemArray() )[aIndex]->StoreL();
        }
#else

    TBool driveSelected(EFalse);

    TDriveNumber selectedMem;
    TInt includedMedias = 0;

    if (iDriveMonitor->PhoneMemoryDrive() != KErrNotFound)
        {
        includedMedias = AknCommonDialogsDynMem::EMemoryTypePhone;
        }

    includedMedias |= AknCommonDialogsDynMem::EMemoryTypeMMCExternal
            | AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage;

    CAknMemorySelectionDialogMultiDrive* dlg =
            CAknMemorySelectionDialogMultiDrive::NewL(
                    ECFDDialogTypeSelect,
                    R_MTV_MEMORY_SELECTION_DIALOG, EFalse,
                    includedMedias);
    CleanupStack::PushL(dlg);
    driveSelected = dlg->ExecuteL(selectedMem);
    CleanupStack::PopAndDestroy(dlg);

    if (driveSelected)
        {
        iSettingMemory = iDriveMonitor->ArrayPos(selectedMem);
        }
       
#endif
   
    if ( iSettingMemory != KErrNotFound )
        {
        (*SettingItemArray())[itemIndex]->LoadL();
        (*SettingItemArray())[itemIndex]->UpdateListBoxTextL();
   
        iVideoSettingsEngine.SetUsedMemory( 
                iDriveMonitor->iAvailableDrives[iSettingMemory].iDrive );
        }
    DrawDeferred();
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
    
    FinishConstructionL();
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
        case EVcSettingsViewParentControlItem:
            {
            iParentControlIndex = GetCurrentParentControl();

            iSettingParentControlItem
                    = new (ELeave) CAknEnumeratedTextPopupSettingItem(
                            aSettingId, iParentControlIndex);
            return iSettingParentControlItem;
            }

        case EVcSettingsViewMemoryItem:
            {
            if (!iDriveMonitor)
                {
                iDriveMonitor = CVideoSettingsDriveMonitor::NewL();
                iDriveMonitor->RegisterObserverL(this);
                }

            TInt drive;
            iVideoSettingsEngine.GetUsedMemoryL(drive);
            iSettingMemory = iDriveMonitor->ArrayPos(drive);
            if (iSettingMemory == KErrNotFound)
                {
                iSettingMemory = 0;
                }

            iSettingMemoryItem
                    = new (ELeave) CAknEnumeratedTextPopupSettingItem(
                            aSettingId, iSettingMemory);

            return iSettingMemoryItem;
            }

        case EVcSettingsviewIadUpdateItem:
            {
            // Set text value to empty. The version number is displayed by
            // changing the string of empty item in UpdateVersionNumberTextL().
            iSettingVersionText = KNullDesC();
            iSettingIadUpdateItem = new (ELeave) CAknTextSettingItem(
                    aSettingId, iSettingVersionText);
            return iSettingIadUpdateItem;
            }
       
        case EVcSettingsViewThumbnails:
            {
            iVideoSettingsEngine.GetVideoOnDemandThumbnails(iSettingThumbnails);
            iSettingThumbnailsItem
                    = new (ELeave) CAknEnumeratedTextPopupSettingItem(
                            aSettingId, iSettingThumbnails);
            return iSettingThumbnailsItem;
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
// CMPSettingsVideoSettingItemList::FinishConstructionL()
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::FinishConstructionL()
    {
    UpdateMemoryItemTextL();
    UpdateVersionNumberTextL();
    UpdateParentControlTextL();
    }

// --------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::UpdateVersionNumberTextL()
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::UpdateVersionNumberTextL()
    {
    Swi::RSisRegistrySession sisRegistrySession;

    if ( sisRegistrySession.Connect() == KErrNone )
        {
        CleanupClosePushL( sisRegistrySession );

        if ( sisRegistrySession.IsInstalledL( KSisUid ) )
            {
            Swi::RSisRegistryEntry sisRegistryEntry;
            TInt error = sisRegistryEntry.Open( sisRegistrySession, KSisUid );
            if ( error == KErrNone )
                {
                CleanupClosePushL( sisRegistryEntry );
                TVersion vcVersion( sisRegistryEntry.VersionL() );
                CleanupStack::PopAndDestroy( &sisRegistryEntry );

                HBufC* versionString = HBufC::NewLC( KMaxVersionNumberLength );
                TPtr versionPtr( versionString->Des() );
                versionPtr.Format(
                        KVersionNumberFormat,
                        vcVersion.iMajor,
                        vcVersion.iMinor,
                        vcVersion.iBuild );
                iSettingIadUpdateItem->SetEmptyItemTextL( *versionString );
                CleanupStack::PopAndDestroy( versionString );
                iSettingIadUpdateItem->UpdateListBoxTextL();
                }
            }
        CleanupStack::PopAndDestroy( &sisRegistrySession );
        }
    }

// --------------------------------------------------------------------------
// Build dynamic listbox for parental control selection.
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::UpdateParentControlTextL()
    {
    // Set up the dynamic texts for this setting item. Choices are:
    //
    // 0:    Parent control not in use.
    // 1-15: Selected minimum age 4-18.

    CArrayPtrFlat<CAknEnumeratedText>* enumeratedTextArray =
        new ( ELeave ) CArrayPtrFlat<CAknEnumeratedText>(
            KParentControlSelections );
    CleanupStack::PushL( enumeratedTextArray );
    CArrayPtr<HBufC>* poppedUpTextArray =
        new ( ELeave ) CArrayPtrFlat<HBufC>( KParentControlSelections );
    CleanupStack::PushL( poppedUpTextArray );

    HBufC* parentControlStrOff1 = NULL;
    HBufC* parentControlStrOff2 = NULL;

    parentControlStrOff1 =
        StringLoader::LoadLC( R_IPTV_PARENTAL_CONTROL_OFF );
    parentControlStrOff2 =
        StringLoader::LoadLC( R_IPTV_PARENTAL_CONTROL_OFF );

    CAknEnumeratedText* enumeratedTextOff =
        new ( ELeave ) CAknEnumeratedText( 0, parentControlStrOff1 );
    CleanupStack::PushL( enumeratedTextOff );
    enumeratedTextArray->AppendL( enumeratedTextOff );
    poppedUpTextArray->AppendL( parentControlStrOff2 );
    CleanupStack::Pop( enumeratedTextOff );
    CleanupStack::Pop( parentControlStrOff2 );
    CleanupStack::Pop( parentControlStrOff1 );

    for ( TInt age = KParentControlMinAge; age <= KParentControlAdult; age++ )
        {
        HBufC* parentControlStr1 = NULL;
        HBufC* parentControlStr2 = NULL;

        parentControlStr1 =
            StringLoader::LoadLC( R_IPTV_PARENTAL_CONTROL_N_YEARS, age );
        parentControlStr2 =
            StringLoader::LoadLC( R_IPTV_PARENTAL_CONTROL_N_YEARS, age );

        CAknEnumeratedText* enumeratedText =
            new ( ELeave ) CAknEnumeratedText(
                age - KParentControlIndexShift, parentControlStr1 );
        CleanupStack::PushL( enumeratedText );
        enumeratedTextArray->AppendL( enumeratedText );
        poppedUpTextArray->AppendL( parentControlStr2 );
        CleanupStack::Pop( enumeratedText );

        CleanupStack::Pop( parentControlStr2 );
        CleanupStack::Pop( parentControlStr1 );
        }

    CleanupStack::Pop( poppedUpTextArray );
    CleanupStack::Pop( enumeratedTextArray );

    iSettingParentControlItem->SetEnumeratedTextArrays(
        enumeratedTextArray,
        poppedUpTextArray );
    iSettingParentControlItem->HandleTextArrayUpdateL();
    iSettingParentControlItem->UpdateListBoxTextL();
    }


// --------------------------------------------------------------------------
// Fetch current parent control age and return corresponding index.
// --------------------------------------------------------------------------
//
TInt CMPSettingsVideoSettingItemList::GetCurrentParentControl()
    {
    TInt parentControlAge( 0 );
    TInt parentControlIndex( 0 );

    if ( !iVideoSettingsEngine.GetParentControl( parentControlAge ) )
        {
        parentControlAge = KIptvCenRepParentControlKeyDefault;
        }

    // Fix age as index.
    if ( KParentControlAdult < parentControlAge )
        {
        parentControlIndex = KParentControlMaxIndex;
        }
    else if ( KParentControlMinAge <= parentControlAge )
        {
        parentControlIndex = parentControlAge - KParentControlIndexShift;
        }
    else
        {
        parentControlIndex = KParentControlOff;
        }

    return parentControlIndex;
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::HandleDriveMonitorEvent
// -----------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::HandleDriveMonitorEvent( 
        TIptvDriveMonitorEvent& /*aEvent*/ )
    {
    TRAP_IGNORE( UpdateMemoryItemTextL() );       
    }

// --------------------------------------------------------------------------
// CVcSettingsViewSettingsListbox::UpdateMemoryItemTextL()
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::UpdateMemoryItemTextL()
    {
    MPX_FUNC(
        "CMPSettingsVideoSettingItemList::UpdateMemoryItemTextL() start");

    if ( !iDriveMonitor )
        {
        iDriveMonitor = CVideoSettingsDriveMonitor::NewL();
        iDriveMonitor->RegisterObserverL( this );
        }

    CArrayPtrFlat<CAknEnumeratedText>* enumeratedTextArray =
        new ( ELeave ) CArrayPtrFlat<CAknEnumeratedText>( 2 );
    CleanupStack::PushL( enumeratedTextArray ); // 1->
    
    CArrayPtr<HBufC>*  poppedUpTextArray  =
        new ( ELeave ) CArrayPtrFlat<HBufC>( 2 );
    CleanupStack::PushL( poppedUpTextArray ); // 2->

    TInt i;
    TInt listIndex = 0;
    for ( i = 0; i < iDriveMonitor->iAvailableDrives.Count(); i++ )
        {
        if ( (iDriveMonitor->iAvailableDrives[i].iFlags &
                        TIptvDriveInfo::ERemovable) )
            {
            /*
             * MMC
             */
            if ( (iDriveMonitor->iAvailableDrives[i].iFlags & 
                            TIptvDriveInfo::EMediaNotPresent) )
                {
                //removable media not present -> Memory card
                MPX_DEBUG1(_L("CMPSettingsVideoSettingItemList:: media not present, adding memory card to the list"));
                
                AddMemoryCardDesToArraysL( listIndex++, 
                                           *enumeratedTextArray, 
                                           *poppedUpTextArray );
                }
            else
                {
                if ( (iDriveMonitor->iAvailableDrives[i].iFlags & 
                                TIptvDriveInfo::ELocked) )
                    {
                    //removable media present and locked -> Locked memory card
                    MPX_DEBUG1(_L("CMPSettingsVideoSettingItemList:: Adding locked memory card to the list"));
                    
                    AddLockedMemoryCardDesToArraysL( listIndex++, 
                                                     *enumeratedTextArray, 
                                                     *poppedUpTextArray );
                    }
                else
                    {
                    //removable media present and not locked
                    if ( iDriveMonitor->iAvailableDrives[i].iVolumeName.Length() > 0 )
                        {
                        //removable media present, not locked and name text exits-> Volume name
                        MPX_DEBUG1(_L("CMPSettingsVideoSettingItemList:: adding volume name to the list"));
                        HBufC* volumeName = 
                            HBufC::NewL( iDriveMonitor->iAvailableDrives[i].iVolumeName.Length() );
                        CleanupStack::PushL( volumeName ); // 3->
                        volumeName->Des() = iDriveMonitor->iAvailableDrives[i].iVolumeName;
                        AddDesToArraysL( listIndex++, 
                                         volumeName, 
                                         *enumeratedTextArray, 
                                         *poppedUpTextArray );
                        CleanupStack::Pop( volumeName ); // <-3
                        }
                    else
                        {
                        //removable media present, not locked and label text does not exit-> Memory card
                        MPX_DEBUG1(_L("CMPSettingsVideoSettingItemList:: volume name not defined, adding memory card to the list") );
                        AddMemoryCardDesToArraysL( listIndex++, 
                                                   *enumeratedTextArray,
                                                   *poppedUpTextArray );
                        }
                    }
                }
            }
        else
            {
            /*
             * Fixed drive
             */
            if ( iDriveMonitor->iAvailableDrives[i].iDrive == 
                    iDriveMonitor->PhoneMemoryDrive() )
                {
                //2 = c-drive -> Phone Memory
                MPX_DEBUG1(_L("CMPSettingsVideoSettingItemList:: adding Phone memory to the list"));
                AddPhoneMemoryDesToArraysL( listIndex++, 
                                            *enumeratedTextArray, 
                                            *poppedUpTextArray );
                }
            else
                {
                //->Hard disk
                MPX_DEBUG1(_L("CMPSettingsVideoSettingItemList:: adding Hard disk to the list"));
                AddHardDiskDesToArraysL( listIndex++, 
                                         *enumeratedTextArray, 
                                         *poppedUpTextArray );
                }
            }
        }

    iSettingMemoryItem->SetEnumeratedTextArrays(
        enumeratedTextArray, poppedUpTextArray );
    iSettingMemoryItem->HandleTextArrayUpdateL();
    iSettingMemoryItem->UpdateListBoxTextL();

    CleanupStack::Pop( poppedUpTextArray );   // <-2
    CleanupStack::Pop( enumeratedTextArray ); // <-1     
	
    DrawDeferred();
    }

// --------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::AddPhoneMemoryDesToArraysL()
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::AddPhoneMemoryDesToArraysL( TInt aIndex,
                              CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                              CArrayPtr<HBufC>&  aPoppedUpTextArray )
    {
    HBufC* phoneMemoryDes = StringLoader::LoadLC( R_IPTV_SETTING_PHONE_MEMORY ); // 1->
    AddDesToArraysL( aIndex, phoneMemoryDes, aEnumeratedTextArray, aPoppedUpTextArray );
    CleanupStack::Pop( phoneMemoryDes ); // <-1
    }

// --------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::AddMemoryCardDesToArraysL()
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::AddMemoryCardDesToArraysL( TInt aIndex,
                            CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                            CArrayPtr<HBufC>&  aPoppedUpTextArray )
    {
    HBufC* memoryCardDes = StringLoader::LoadLC( R_IPTV_SETTING_MEMORY_CARD ); // 1->
    AddDesToArraysL( aIndex, memoryCardDes, aEnumeratedTextArray, aPoppedUpTextArray );
    CleanupStack::Pop( memoryCardDes ); // <-1
    }

// --------------------------------------------------------------------------
// CVcSettingsViewSettingsListbox::AddLockedMemoryCardDesToArraysL()
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::AddLockedMemoryCardDesToArraysL( TInt aIndex,
                            CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                            CArrayPtr<HBufC>&  aPoppedUpTextArray )
    {
    HBufC* lockedMemoryCardDes = StringLoader::LoadLC( R_IPTV_LOCKED_MEMORY_CARD ); // 1->
    AddDesToArraysL( aIndex, lockedMemoryCardDes, aEnumeratedTextArray, aPoppedUpTextArray);
    CleanupStack::Pop( lockedMemoryCardDes ); // <-1
    }

// --------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::AddHardDiskDesToArraysL()
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::AddHardDiskDesToArraysL( TInt aIndex,
                            CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                            CArrayPtr<HBufC>&  aPoppedUpTextArray )
    {
    HBufC* hardDiskDes = StringLoader::LoadLC( R_IPTV_SETTING_MASS_MEMORY ); // 1->
    AddDesToArraysL( aIndex, hardDiskDes, aEnumeratedTextArray, aPoppedUpTextArray);
    CleanupStack::Pop( hardDiskDes ); // <-1
    }

// --------------------------------------------------------------------------
// CMPSettingsVideoSettingItemList::AddDesToArraysL()
// NOTE: Codescanner whines about passing non const HBufC,
// but that's needed for CAknEnumeratedText construction.
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::AddDesToArraysL( TInt aIndex, HBufC* aText,
                            CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                            CArrayPtr<HBufC>&  aPoppedUpTextArray )
    {
    CAknEnumeratedText* enumeratedText =
        new ( ELeave ) CAknEnumeratedText( aIndex, aText );
    CleanupStack::PushL( enumeratedText ); //1->
    aEnumeratedTextArray.AppendL( enumeratedText );
    CleanupStack::Pop( enumeratedText ); // <-1
    
    HBufC* textCopy = HBufC::NewL( aText->Des().Length() );
    CleanupStack::PushL( textCopy ); // 1->
    textCopy->Des() = aText->Des();
    
    aPoppedUpTextArray.AppendL( textCopy );
    CleanupStack::Pop( textCopy ); // <-1
    }

// --------------------------------------------------------------------------
// Convert and save parent control index as age.
// --------------------------------------------------------------------------
//
void CMPSettingsVideoSettingItemList::SetCurrentParentControl(
    TInt aParentControlIndex )
    {
    TInt parentControlAge( 0 );

    // Fix index as age.
    if ( KParentControlMaxIndex < aParentControlIndex )
        {
        parentControlAge = KParentControlAdult;
        }
    else if ( 1 <= aParentControlIndex )
        {
        parentControlAge = aParentControlIndex + KParentControlIndexShift;
        }
    else
        {
        parentControlAge = 0;
        }

    iVideoSettingsEngine.SetParentControl( parentControlAge );
    }

// --------------------------------------------------------------------------
// Check if video contrast is supported.
// --------------------------------------------------------------------------
//
TBool CMPSettingsVideoSettingItemList::VideoContrastIsSupportedL()
    {
    TInt flags;

    CRepository* repository = CRepository::NewL( KCRUidMediaPlayerFeatures );
    repository->Get( KMPLocalVariation, flags );
    delete repository;

    if ( flags & KMediaPlayerVideoContrast )
        {
        return ETrue;
        }
    
    return EFalse;
    }

//  End of File  
