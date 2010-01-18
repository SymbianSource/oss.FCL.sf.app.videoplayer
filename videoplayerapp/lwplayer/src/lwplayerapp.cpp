/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation of CLWPLayerApp
 *
*/

// Version : %version: 4 %


#include <eikstart.h>

#include "lwplayerapp.h"
#include "lwplayerdocument.h"
#include "lwpuids.hrh"
#include "mpxvideo_debug.h"


// -----------------------------------------------------------------------------
// CLWPlayerApp::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CLWPlayerApp::AppDllUid() const
{
    MPX_DEBUG(_L("CLWPlayerApp::AppDllUid()"));

    return KLWPLAYERUID;
}

// -----------------------------------------------------------------------------
// CLWPlayerApp::CreateDocumentL()
// Creates CLWPlayerDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CLWPlayerApp::CreateDocumentL()
{
    MPX_DEBUG(_L("CLWPlayerApp::CreateDocumentL()"));

    return CLWPlayerDocument::NewL( *this );
}

// ========================= OTHER EXPORTED FUNCTIONS ==========================
//
// -----------------------------------------------------------------------------
// NewApplication()
// Constructs CLWPlayerApp
// Returns: created application object
// -----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
{
    MPX_DEBUG(_L("CLWPlayerApp NewApplication()"));

    return new CLWPlayerApp;
}

// -----------------------------------------------------------------------------
// E32Main
// An entry point
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
{
    MPX_DEBUG(_L("CLWPlayerApp E32Main()"));

    return EikStart::RunApplication(NewApplication);
}

// End of File
