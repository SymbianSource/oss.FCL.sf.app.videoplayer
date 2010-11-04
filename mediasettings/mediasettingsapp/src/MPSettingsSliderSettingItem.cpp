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
* Description:   CAknSettingItemList compliant setting item class for slider settings.*
*/


// Version : %version: 6 %




// INCLUDE FILES
#include    <aknslider.h>
#include    <StringLoader.h>
#include    <aknslidersettingpage.h>
#include    <MediaSettings.rsg>
#include    <barsread.h>

#include    "MPSettingsSliderSettingItem.h"
#include	"mpxlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsSliderSettingItem::CMPSettingsSliderSettingItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsSliderSettingItem::CMPSettingsSliderSettingItem(TInt aIdentifier,
    TInt& aExternalSliderValue, TInt aResource, TBool aBareFigure) :
        CAknSliderSettingItem(aIdentifier, aExternalSliderValue),
        iResource(aResource),
        iBareFigure(aBareFigure)
    {
    MPX_FUNC("#MS# CMPSettingsSliderSettingItem::CMPSettingsSliderSettingItem()");
    }

// -----------------------------------------------------------------------------
// CMPSettingsSliderSettingItem::~CMPSettingsSliderSettingItem
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsSliderSettingItem::~CMPSettingsSliderSettingItem()
    {
    MPX_FUNC("#MS# CMPSettingsSliderSettingItem::~CMPSettingsSliderSettingItem()");
	if (iSettingText) 
		{
		delete iSettingText;
		}
    }

// -----------------------------------------------------------------------------
// CMPSettingsSliderSettingItem::SettingTextL
// -----------------------------------------------------------------------------
//
const TDesC& CMPSettingsSliderSettingItem::SettingTextL()
    {
    delete iSettingText;
    iSettingText = NULL;

    if (iBareFigure)
        {
        iSettingText = CAknSlider::CreateValueTextInHBufCL(InternalSliderValue(),
                                                           iResource);
        }
    else 
        {
        iSettingText = StringLoader::LoadL(iResource, InternalSliderValue());
        }

	TPtr text = iSettingText->Des();
   	AknTextUtils::LanguageSpecificNumberConversion(text);
	MPX_DEBUG2(_L("#MS# CMPSettingsSliderSettingItem::SettingTextL(%S)"),&text);
    return *iSettingText;
    }

// -----------------------------------------------------------------------------
// CMPSettingsSliderSettingItem::CreateSettingPageL
// -----------------------------------------------------------------------------
//
CAknSettingPage* CMPSettingsSliderSettingItem::CreateSettingPageL()
    {
    MPX_FUNC("#MS# CMPSettingsSliderSettingItem::CreateSettingPageL()");
    CAknSliderSettingPage* settingPage = 
        static_cast<CAknSliderSettingPage*>(CAknSliderSettingItem::CreateSettingPageL());
    CleanupStack::PushL(settingPage);

    if (iBareFigure && AknTextUtils::DigitModeQuery())
        {
        settingPage->ConstructL();
        CAknSlider* slider = settingPage->SliderControl();
        
        // Read unnecessary SLIDER resource struct members
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC(reader, iResource);

        for (TInt ii = 0; ii < 6; ++ii)
            {
            reader.ReadInt16(); // layout, minvalue, maxvalue, step, valuetype, decimalplaces
            }
        
        HBufC* buf = reader.ReadHBufCL(); // singular_valuelabel
        delete buf;
        buf = NULL;
        
        // Read minlabel from resource file
        buf = reader.ReadHBufCL(); // minlabel
        CleanupStack::PushL(buf);
        TPtr ptr = buf->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(ptr);
        MPX_DEBUG2(_L("#MS# CMPSettingsSliderSettingItem::CreateSettingPageL() min(%S)"),&ptr);
        slider->SetMinimumTextL(*buf);
        CleanupStack::PopAndDestroy(); // buf
        buf = NULL;
        
        // Read maxlabel from resource file
        buf = reader.ReadHBufCL(); // maxlabel
        CleanupStack::PushL(buf);
        ptr.Set(buf->Des());
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(ptr);
        MPX_DEBUG2(_L("#MS# CMPSettingsSliderSettingItem::CreateSettingPageL() max(%S)"),&ptr);
        slider->SetMaximumTextL(*buf);
        CleanupStack::PopAndDestroy(2); // buf & reader
        }

    CleanupStack::Pop(); // settingPage
    return settingPage;
    }

// -----------------------------------------------------------------------------
// CMPSettingsSliderSettingItem::CreateBitmapL
// -----------------------------------------------------------------------------
//
CFbsBitmap* CMPSettingsSliderSettingItem::CreateBitmapL()
    {
    MPX_FUNC("#MS# CMPSettingsSliderSettingItem::CreateBitmapL()");
    return NULL;
    }

//  End of File  
