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
* Description:  Implementation of CLWPlayerDocument
 *
*/

// Version : %version: 4 %


#include <apgwgnam.h>
#include <AiwGenericParam.h>
#include "lwplayerdocument.h"
#include "lwplayerappui.h"
#include "mpxvideo_debug.h"

// -----------------------------------------------------------------------------
//  CLWPlayerDocument::CLWPlayerDocument
//  C++ default constructor can NOT contain any code, that
//  might leave.
// -----------------------------------------------------------------------------
//
CLWPlayerDocument::CLWPlayerDocument(CEikApplication& aApp)
    : CAiwGenericParamConsumer(aApp)
{
    MPX_DEBUG(_L("CLWPlayerDocument::CLWPlayerDocument()"));
}

// -----------------------------------------------------------------------------
// CLWPlayerDocument::~CLWPlayerDocument
// Destructor
// Frees reserved resources
// -----------------------------------------------------------------------------
//
CLWPlayerDocument::~CLWPlayerDocument()
{
    MPX_DEBUG(_L("CLWPlayerDocument::~CLWPlayerDocument()"));
}

// -----------------------------------------------------------------------------
// CLWPlayerDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void
CLWPlayerDocument::ConstructL()
{
    MPX_DEBUG(_L("CLWPlayerDocument::ConstructL()"));
}

// -----------------------------------------------------------------------------
// CLWPlayerDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLWPlayerDocument* CLWPlayerDocument::NewL(CEikApplication& aApp)
{
    MPX_DEBUG(_L("CLWPlayerDocument::NewL()"));

    CLWPlayerDocument* self = new (ELeave) CLWPlayerDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// CLWPlayerDocument::CreateAppUiL()
// constructs CLWPlayerAppUi
// -----------------------------------------------------------------------------
//
CEikAppUi* CLWPlayerDocument::CreateAppUiL()
{
    MPX_DEBUG(_L("CLWPlayerDocument::CreateAppUiL()"));

    return new (ELeave) CLWPlayerAppUi;
}

// -----------------------------------------------------------------------------
// CLWPlayerDocument::UpdateTaskNameL()
// Hides the icon from the task list
// -----------------------------------------------------------------------------
//
void CLWPlayerDocument::UpdateTaskNameL(CApaWindowGroupName* aWgName)
{
    CEikDocument::UpdateTaskNameL(aWgName);
    aWgName->SetHidden(ETrue);
}

// End of File
