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
* Description:   CAknSettingItemList compliant setting item class for Advanced bandwidth settings.*
*/


// Version : %version: 8 %




// INCLUDE FILES
#include    <AknQueryDialog.h>
#include    <aknradiobuttonsettingpage.h>
#include    <eikenv.h>
#include    <MediaSettings.rsg>
#include    <StringLoader.h>
#include    <AknUtils.h>

#include    "MPSettingsAdvancedBwSettingItem.h"
#include	"mpxlog.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItem::CMPSettingsAdvancedBwSettingItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsAdvancedBwSettingItem::CMPSettingsAdvancedBwSettingItem(
                                                            TInt aIdentifier,
                                                            RArray<TInt>& aValueArray,
                                                            TInt& aValue,
                                                            TInt& aListValue,
                                                            TInt aQueryRes) :
    CAknEnumeratedTextPopupSettingItem(aIdentifier, aListValue),
    iValueArray(aValueArray),
    iValue(aValue),
    iQueryRes(aQueryRes)
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItem::CMPSettingsAdvancedBwSettingItem()");
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItem::~CMPSettingsAdvancedBwSettingItem
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsAdvancedBwSettingItem::~CMPSettingsAdvancedBwSettingItem()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItem::~CMPSettingsAdvancedBwSettingItem()");
	if (iSettingText) 
		{
		delete iSettingText;
		}

    if (iEnumTextArray)
        {
        iEnumTextArray->ResetAndDestroy();
        delete iEnumTextArray;
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItem::SettingTextL
// -----------------------------------------------------------------------------
//
const TDesC& CMPSettingsAdvancedBwSettingItem::SettingTextL()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItem::SettingTextL()");
    delete iSettingText;
    iSettingText = NULL;

    iSettingText = CreateValueTextL(iValue);

    return *iSettingText;
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItem::EditItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItem::EditItemL(TBool aCalledFromMenu)
    {
	MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwSettingItem::EditItemL(%d)"),aCalledFromMenu);
    CAknEnumeratedTextPopupSettingItem::EditItemL(aCalledFromMenu);

    TInt count = iValueArray.Count();

    for (TInt index = 0; index < count; ++index)
        {
        if (iValueArray[index] == iValue)
            {
            SetInternalValue(index);
            SetSelectedIndex(IndexFromValue(index));
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItem::CompleteConstructionL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItem::CompleteConstructionL()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItem::CompleteConstructionL()");
    __ASSERT_DEBUG(!iEnumTextArray, User::Leave(KErrAlreadyExists));

    TInt count = iValueArray.Count();
    iEnumTextArray = new(ELeave) CArrayPtrFlat<CAknEnumeratedText>(count + 1);
    CAknEnumeratedText* enumText = NULL;
    HBufC* text = NULL;

    for (TInt index = 0; index < count; ++index)
        {
        text = CreateValueTextL(iValueArray[index]);
        CleanupStack::PushL(text);
        enumText = new(ELeave) CAknEnumeratedText(index, text);
        CleanupStack::Pop(); // text
        text = NULL;
        CleanupStack::PushL(enumText);
        iEnumTextArray->AppendL(enumText);
        CleanupStack::Pop(); //enumText
        enumText = NULL;
        }

    // Append "User defined" to the end of the array
    text = StringLoader::LoadLC(R_MPSETT_USER_DEFINED);
    enumText = new(ELeave) CAknEnumeratedText(count, text);
    CleanupStack::Pop(); // text
    CleanupStack::PushL(enumText);
    iEnumTextArray->AppendL(enumText);
    CleanupStack::Pop(); //enumText                

    // poppedTextArray should be left empty! Magic number: array granularity
    CArrayPtr<HBufC>* poppedTextArray = new(ELeave) CArrayPtrFlat<HBufC>(1);
    // The ownership of both arrays is transferred to the base class.
    SetEnumeratedTextArrays(iEnumTextArray, poppedTextArray);
    iEnumTextArray = NULL;
    poppedTextArray = NULL;

    HandleTextArrayUpdateL();
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItem::HandleSettingPageEventL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItem::HandleSettingPageEventL(CAknSettingPage* aSettingPage,
                                                           TAknSettingPageEvent aEventType)
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItem::HandleSettingPageEventL()");
    if (aEventType != EEventSettingCancelled)
        {
        CEikListBox* listbox = static_cast<CAknRadioButtonSettingPage*>(aSettingPage)->ListBoxControl();
    
        TInt current = listbox->CurrentItemIndex();
        TInt count = listbox->Model()->NumberOfItems();
        
        if ( current == count - 1 )
            {
			// Search min and max values to use as limits.
            TInt min = KMaxTInt;
            TInt max = 0;
            for ( TInt i=0; i<iValueArray.Count(); i++ )
                {
                if ( iValueArray[i] < min )
                    {
                    min = iValueArray[i];
                    }
                if ( iValueArray[i] > max )
                    {
                    max = iValueArray[i];
                    }
                }
            
            ShowAdvancedBwQueryL( min, max );
            }
        else
            {
            iValue = iValueArray[current];
            }
        }

    CAknSettingItem::HandleSettingPageEventL(aSettingPage, aEventType);
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItem::ShowAdvancedBwQueryL
// -----------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwSettingItem::ShowAdvancedBwQueryL( TInt aMin, TInt aMax )
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwSettingItem::ShowAdvancedBwQueryL()");
    TReal value = static_cast<TReal>(iValue) / 1000;
    CAknFloatingPointQueryDialog* dlg = new (ELeave) CAknFloatingPointQueryDialog(value);

    dlg->PrepareLC( iQueryRes );
    dlg->SetMinimumAndMaximum( static_cast<TReal>(aMin) / 1000,
                               static_cast<TReal>(aMax) / 1000 );
    if ( dlg->RunLD() )
        {
        iValue = static_cast<TInt>(value * 1000);
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwSettingItem::CreateValueTextL
// -----------------------------------------------------------------------------
//
HBufC* CMPSettingsAdvancedBwSettingItem::CreateValueTextL(TInt aValue)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwSettingItem::CreateValueTextL(%d)"),aValue);
    TRealFormat format(10, 2);
    TBuf<10> valueString;
    HBufC* convert;
    TReal value = static_cast<TReal>(aValue) / 1000;

    if (value >= 1000)
        {
        User::LeaveIfError(valueString.Num(value/1000, format));
        convert = StringLoader::LoadL(R_MPSETT_ADVANCED_BW_VALUE_MBIT, valueString);
        }
    else
        {
        User::LeaveIfError(valueString.Num(value, format));
        convert = StringLoader::LoadL(R_MPSETT_ADVANCED_BW_VALUE, valueString);
        }

    TPtr tmp( convert->Des() );
    AknTextUtils::LanguageSpecificNumberConversion(tmp);
    MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwSettingItem::CreateValueTextL() ret (%S)"),&tmp);
    return convert;

    }

//  End of File  
