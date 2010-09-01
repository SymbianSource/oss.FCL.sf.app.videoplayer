/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Content information.*
*/




#include <e32base.h>
#include "CIptvEpgContent.h"

// ---------------------------------------------------------
// CIptvEpgContent::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvEpgContent::ConstructL()
    {    
    }

// ---------------------------------------------------------
// CIptvEpgContent::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CIptvEpgContent* CIptvEpgContent::NewL()
    {
    CIptvEpgContent* self = new (ELeave) CIptvEpgContent();
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CIptvEpgContent::~CIptvEpgContent
// Destructor
// ---------------------------------------------------------
//
CIptvEpgContent::~CIptvEpgContent()
    {    
    }

// ---------------------------------------------------------
// CIptvEpgContent::CIptvEpgContent
// C++ default constructor
// ---------------------------------------------------------
//
CIptvEpgContent::CIptvEpgContent()
    {
    }
