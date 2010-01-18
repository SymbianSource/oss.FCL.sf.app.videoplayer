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


#include "FusionPlayerTestAppUi.h"
#include "FusionPlayerTestAppDocument.h"
#include <mpxviewutility.h>

// implementation is purely framework code. Document class not customised
// in this application

CFusionPlayerTestAppDocument* CFusionPlayerTestAppDocument::NewL(CEikApplication& aApp)
    {
    CFusionPlayerTestAppDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CFusionPlayerTestAppDocument* CFusionPlayerTestAppDocument::NewLC(CEikApplication& aApp)
    {
    CFusionPlayerTestAppDocument* self = new (ELeave) CFusionPlayerTestAppDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CFusionPlayerTestAppDocument::ConstructL()
    {
    // Get view utility. View utility must be created before AppUi.
    iViewUtility = MMPXViewUtility::UtilityL();
    }

CFusionPlayerTestAppDocument::CFusionPlayerTestAppDocument(CEikApplication& aApp) : CAknDocument(aApp)
    {
    }

CFusionPlayerTestAppDocument::~CFusionPlayerTestAppDocument()
    {
    if ( iViewUtility )
        {
        iViewUtility->Close();
        }    
    }

CEikAppUi* CFusionPlayerTestAppDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it
    CEikAppUi* appUi = new (ELeave) CFusionPlayerTestAppAppUi;
    return appUi;
    }

