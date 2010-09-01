/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Category information holder*
*/




#include <e32base.h>
#include "CIptvEpgCategory.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CIptvEpgCategory* CIptvEpgCategory::NewL()
    {
    CIptvEpgCategory* self = new( ELeave ) CIptvEpgCategory();
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CIptvEpgCategory::~CIptvEpgCategory()
    {
    }

// --------------------------------------------------------------------------
// C++ default constructor
// --------------------------------------------------------------------------
//
CIptvEpgCategory::CIptvEpgCategory()
    {
    }
