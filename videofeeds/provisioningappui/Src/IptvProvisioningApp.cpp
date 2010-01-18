/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/




// INCLUDE FILES
#include "IptvProvisioningApp.h"
#include "IptvProvisioningDocument.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CIptvProvisioningApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------------------------

TUid CIptvProvisioningApp::AppDllUid() const
    {
    return KUidIptvProvisioningAppUid;
    }

// ---------------------------------------------------------------------------
// CIptvProvisioningApp::CreateDocumentL()
// Creates CIptvProvisioningDocument object
// ---------------------------------------------------------------------------

CApaDocument* CIptvProvisioningApp::CreateDocumentL()
    {
    return new( ELeave ) CIptvProvisioningDocument( *this );
    }

// ===================== OTHER EXPORTED FUNCTIONS ============================

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CIptvProvisioningApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }


// End of File

