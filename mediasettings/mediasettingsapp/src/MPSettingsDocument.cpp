/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Document class required by AVKON application framework.*
*/


// Version : %version: 4 %



// INCLUDE FILES
#include    <ecom.h>

#include    "MPSettingsDocument.h"
#include    "MPSettingsAppUi.h"
#include    "mpsettingsmodel.h"
#include    "MPSettEngPluginImplementationUIDs.hrh"
#include	"mpxlog.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsDocument::CMPSettingsDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsDocument::CMPSettingsDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsDocument::CMPSettingsDocument()"));
    }

// -----------------------------------------------------------------------------
// CMPSettingsDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPSettingsDocument::ConstructL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsDocument::ConstructL()"));
    const TUid KSettingsModelForROPUid = {KMPSettEngImplUidROP};
    iRopSettingsModel = CMPSettingsModel::NewL(KSettingsModelForROPUid);
    }

// -----------------------------------------------------------------------------
// CMPSettingsDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPSettingsDocument* CMPSettingsDocument::NewL(
        CEikApplication& aApp)     // CMPSettingsApp reference
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsDocument::NewL()"));
    CMPSettingsDocument* self = new(ELeave) CMPSettingsDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CMPSettingsDocument::~CMPSettingsDocument
// Destructor.
// -----------------------------------------------------------------------------
//
CMPSettingsDocument::~CMPSettingsDocument()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsDocument::~CMPSettingsDocument()"));
	if (iRopSettingsModel) 
		{
		delete iRopSettingsModel;
		}

    REComSession::FinalClose();
    }
    
// ----------------------------------------------------
// CMPSettingsDocument::SettingsModel
// ----------------------------------------------------
//
CMPSettingsModel* CMPSettingsDocument::SettingsModel()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsDocument::SettingsModel()"));
    return iRopSettingsModel;
    }

// ----------------------------------------------------
// CMPSettingsDocument::CreateAppUiL()
// constructs CMPSettingsAppUi
// ----------------------------------------------------
//
CEikAppUi* CMPSettingsDocument::CreateAppUiL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsDocument::CreateAppUiL()"));
    return new(ELeave) CMPSettingsAppUi;
    }

// End of File  
