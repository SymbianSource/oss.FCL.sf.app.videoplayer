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
* Description:   Application class required by AVKON application framework.*
*/


// Version : %version: 2 %



// INCLUDE FILES
#include    "MPSettingsApp.h"
#include    "MPSettingsDocument.h"
#include	"mpxlog.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMPSettingsApp::AppDllUid
// Returns application UID
// ---------------------------------------------------------
//
TUid CMPSettingsApp::AppDllUid() const
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsApp::AppDllUid()"));
    return KUidMediaSettings;
    }

   
// ---------------------------------------------------------
// CMPSettingsApp::CreateDocumentL
// Creates CMPSettingsDocument object
// ---------------------------------------------------------
//
CApaDocument* CMPSettingsApp::CreateDocumentL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsApp::CreateDocumentL()"));
    return CMPSettingsDocument::NewL(*this);
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CMPSettingsApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// End of File  

