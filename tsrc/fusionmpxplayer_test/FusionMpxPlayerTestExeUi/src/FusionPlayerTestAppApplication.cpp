/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "FusionPlayerTestAppDocument.h"
#include "FusionPlayerTestAppApplication.h"

// local constants
static const TUid KUidFusionPlayerTestAppApp = {0x101FB3E3};

CApaDocument* CFusionPlayerTestAppApplication::CreateDocumentL()
    {
    // Create an FusionPlayerTestApp document, and return a pointer to it
    CApaDocument* document = CFusionPlayerTestAppDocument::NewL(*this);
    return document;
    }

TUid CFusionPlayerTestAppApplication::AppDllUid() const
    {
    // Return the UID for the FusionPlayerTestApp application
    return KUidFusionPlayerTestAppApp;
    }
