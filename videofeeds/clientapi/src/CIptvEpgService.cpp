/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include "CIptvEpgService.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CIptvEpgService::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CIptvEpgService::ConstructL()
    {
    iSearchUrl = HBufC::NewL( 0 );
    }

// --------------------------------------------------------------------------
// CIptvEpgService::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CIptvEpgService* CIptvEpgService::NewL()
    {
    CIptvEpgService* self = new(ELeave) CIptvEpgService();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// --------------------------------------------------------------------------
// CIptvEpgService::~CIptvEpgService
// Destructor
// --------------------------------------------------------------------------
//
CIptvEpgService::~CIptvEpgService()
    {
	delete iSearchUrl;
    }

// --------------------------------------------------------------------------
// CIptvEpgService::CIptvEpgService
// C++ default constructor
// --------------------------------------------------------------------------
//
CIptvEpgService::CIptvEpgService()
    {
    }

// --------------------------------------------------------------------------
// CIptvEpgService::SetSearchUrlL
// Search url setter
// --------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgService::SetSearchUrlL( const TDesC& aSearchUrl )
	{
	if( iSearchUrl )
		{
		delete iSearchUrl;
		iSearchUrl = NULL;
		}

	iSearchUrl = aSearchUrl.AllocL();
	}

// --------------------------------------------------------------------------
// CIptvEpgService::SearchUrl
// Search url getter
// --------------------------------------------------------------------------
//
EXPORT_C TPtrC CIptvEpgService::SearchUrl()
	{
	return *iSearchUrl;
	}
