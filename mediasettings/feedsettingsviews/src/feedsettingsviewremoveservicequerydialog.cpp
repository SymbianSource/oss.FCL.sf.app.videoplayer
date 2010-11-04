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
* Description:    Remove service query dialog.*
*/




// INCLUDE FILES
#include <e32base.h>
#include "IptvDebug.h"
#include "feedsettingsviewremoveservicequerydialog.h"

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------
// CVcSettingsViewRemoveServiceQueryDialog::CVcSettingsViewRemoveServiceQueryDialog
// C++ default constructor
// ---------------------------------------------------------
//
CVcxNsSettingsViewRemoveServiceQueryDialog::CVcxNsSettingsViewRemoveServiceQueryDialog( )  
    {
    IPTVLOGSTRING_LOW_LEVEL(
    "CVcSettingsViewRemoveServiceQueryDialog::CVcSettingsViewRemoveServiceQueryDialog");
    }

// ---------------------------------------------------------
// CVcSettingsViewRemoveServiceQueryDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CVcxNsSettingsViewRemoveServiceQueryDialog::ConstructL( const TDesC& aMessageText )
    {    
    IPTVLOGSTRING_LOW_LEVEL("CVcSettingsViewRemoveServiceQueryDialog::ConstructL");
    SetMessageTextL( aMessageText );        
    }

// ---------------------------------------------------------
// CVcSettingsViewRemoveServiceQueryDialog::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CVcxNsSettingsViewRemoveServiceQueryDialog* CVcxNsSettingsViewRemoveServiceQueryDialog::NewL( 
                                                            const TDesC& aMessageText )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcSettingsViewRemoveServiceQueryDialog::NewL");        

    CVcxNsSettingsViewRemoveServiceQueryDialog* self = 
    new(ELeave) CVcxNsSettingsViewRemoveServiceQueryDialog(  );
    CleanupStack::PushL(self);
    self->ConstructL(aMessageText);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CVcSettingsViewRemoveServiceQueryDialog::~CVcSettingsViewRemoveServiceQueryDialog
// Destructor
// -----------------------------------------------------------------------------
//
CVcxNsSettingsViewRemoveServiceQueryDialog::~CVcxNsSettingsViewRemoveServiceQueryDialog()
    {
    IPTVLOGSTRING_LOW_LEVEL(
    "CVcSettingsViewRemoveServiceQueryDialog::~CVcSettingsViewRemoveServiceQueryDialog");                
    }
    
// ---------------------------------------------------------
// CVcSettingsViewRemoveServiceQueryDialog::~CVcSettingsViewRemoveServiceQueryDialog
// Destructor
// -----------------------------------------------------------------------------
//
TKeyResponse CVcxNsSettingsViewRemoveServiceQueryDialog::OfferKeyEventL(
                                                        const TKeyEvent& aKeyEvent, 
                                                        TEventCode aCode )
    {
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcSettingsViewRemoveServiceQueryDialog::OfferKeyEventL aKeyEvent.iCode: %d",
        aKeyEvent.iCode);
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcSettingsViewRemoveServiceQueryDialog::OfferKeyEventL aKeyEvent.iScanCode: %d",
        aKeyEvent.iScanCode);
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcSettingsViewRemoveServiceQueryDialog::OfferKeyEventL aCode: %d", aCode);
    
    return CAknMessageQueryDialog::OfferKeyEventL( aKeyEvent, aCode );       
    }

// ---------------------------------------------------------
// CVcSettingsViewRemoveServiceQueryDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CVcxNsSettingsViewRemoveServiceQueryDialog::OkToExitL( TInt /*aButtonId*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL(
       "CVcSettingsViewRemoveServiceQueryDialog::OkToExitL() returning ETrue.");
    return ETrue;
    }    

// ---------------------------------------------------------
// CVcSettingsViewRemoveServiceQueryDialog::OkToExitL
// -----------------------------------------------------------------------------
// 
TBool CVcxNsSettingsViewRemoveServiceQueryDialog::NeedToDismissQueryL( const TKeyEvent& aKeyEvent )
    {
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcSettingsViewRemoveServiceQueryDialog::NeedToDismissQueryL aKeyEvent.iCode: %d",
        aKeyEvent.iCode );
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcSettingsViewRemoveServiceQueryDialog::NeedToDismissQueryL aKeyEvent.iScanCode: %d",
        aKeyEvent.iScanCode );    
    return CAknMessageQueryDialog::NeedToDismissQueryL( aKeyEvent );
    }


// ---------------------------------------------------------
// CVcxNsSettingsViewRemoveServiceQueryDialog::ProcessCommandL
// -----------------------------------------------------------------------------
// 
void CVcxNsSettingsViewRemoveServiceQueryDialog::ProcessCommandL( TInt aCommandId )    
    {
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcSettingsViewRemoveServiceQueryDialog::ProcessCommandL aCommandId %d", 
        aCommandId );
    
    CAknMessageQueryDialog::ProcessCommandL( aCommandId );
    }
