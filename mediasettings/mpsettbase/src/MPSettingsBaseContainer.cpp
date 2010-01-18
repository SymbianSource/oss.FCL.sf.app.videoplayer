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
* Description:   Base class for MediaSettings view containers.*
*/


// Version : %version: 2 %




// INCLUDE FILES
#include    <aknappui.h>
#include    <akntitle.h>
#include    <barsread.h>
#include    <eikspane.h>
#include    <aknlists.h> 
#include    "MPSettingsBaseContainer.h" 
#include	"mpxlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsBaseContainer::CMPSettingsBaseContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPSettingsBaseContainer::CMPSettingsBaseContainer()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsBaseContainer::CMPSettingsBaseContainer()"));
    }

// -----------------------------------------------------------------------------
// CMPSettingsBaseContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseContainer::ConstructL(const TRect& aRect)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsBaseContainer::ConstructL()"));
    CEikStatusPane* sp = static_cast<CAknAppUi*>(iCoeEnv->AppUi())->StatusPane();

    CAknTitlePane* title = static_cast<CAknTitlePane*>(sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));

    TResourceReader rReader;
    iCoeEnv->CreateResourceReaderLC(rReader, TitleResourceId());
    title->SetFromResourceL(rReader);
    CleanupStack::PopAndDestroy(); //rReader
      
    CreateWindowL(); // Makes the control a window-owning control
 
    // Construct and initialize control(s)
    ConstructComponentControlsL();

    SetRect(aRect);
    ActivateL();
    }
   
// -----------------------------------------------------------------------------
// CMPSettingsBaseContainer::~CMPSettingsBaseContainer
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMPSettingsBaseContainer::~CMPSettingsBaseContainer()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsBaseContainer::~CMPSettingsBaseContainer()"));
    }

// -----------------------------------------------------------------------------
// CMPSettingsBaseContainer::ListBox
// -----------------------------------------------------------------------------
//
EXPORT_C CEikTextListBox* CMPSettingsBaseContainer::ListBox()
    {
    return iListBox;
    }

// ---------------------------------------------------------------------------
// CMPSettingsBaseContainer::SizeChanged()
// Called by framwork when the view size is changed
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseContainer::SizeChanged()
    {
    if (iComponentControl)
        {
        iComponentControl->SetRect(Rect());
        }
    }

// ---------------------------------------------------------------------------
// CMPSettingsBaseContainer::CountComponentControls() const
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CMPSettingsBaseContainer::CountComponentControls() const
    {
    return 1; //magic
    }

// ---------------------------------------------------------------------------
// CMPSettingsBaseContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CMPSettingsBaseContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return iComponentControl;
    }

// ---------------------------------------------------------
// CMPSettingsBaseContainer::HandleResourceChange
// Handles a resource relative event 
// ---------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseContainer::HandleResourceChange( TInt aType )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsBaseContainer::HandleResourceChange(0x%X)"),aType);
    if ( aType == KEikDynamicLayoutVariantSwitch ) //Handle change in layout orientation
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
		DrawNow();
		}
    CCoeControl::HandleResourceChange( aType );    
    }

// ---------------------------------------------------------------------------
// CMPSettingsBaseContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
// Called when a key is pressed.
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse  CMPSettingsBaseContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    MPX_DEBUG4(_L("#MS# CMPSettingsBaseContainer::OfferKeyEventL() iCode(0x%X) iScanCode(0x%X) aType(0x%X)"),
				 aKeyEvent.iCode,aKeyEvent.iScanCode,aType);
    TKeyResponse response = EKeyWasNotConsumed;

    if (iComponentControl &&
        aKeyEvent.iCode != EKeyLeftArrow &&
        aKeyEvent.iCode != EKeyRightArrow)
        {
        response = iComponentControl->OfferKeyEventL(aKeyEvent, aType); 
        }

    return response;
    }

// -----------------------------------------------------------------------------
// CMPSettingsBaseContainer::SetSelectedItem()
//
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseContainer::SetSelectedItem( TInt aIndex )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsBaseContainer::SetSelectedItem(%d)"),aIndex);
    if( iListBox && aIndex >= 0 )
        {
        iListBox->SetCurrentItemIndexAndDraw( aIndex );
        }
    }

//  End of File
