/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Object to offer some common utility functions to Iptv engine testing*
*/




// INCLUDE FILES
#include <e32Base.h>
#include "CIptvTestVodContentAllDetails.h"
#include "CIptvVodContentClient.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvMediaContent.h"
#include "VCXTestLog.h"

// -----------------------------------------------------------------------------
// CIptvTestVodContentAllDetails::CIptvTestVodContentAllDetails
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvTestVodContentAllDetails::CIptvTestVodContentAllDetails()
    {
    }

// -----------------------------------------------------------------------------
// CIptvTestVodContentAllDetails::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvTestVodContentAllDetails::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTestVodContentAllDetails* CIptvTestVodContentAllDetails::NewL()
    {
    VCXLOGLO1(">>>CIptvTestVodContentAllDetails::NewL");
    CIptvTestVodContentAllDetails* self = new( ELeave ) CIptvTestVodContentAllDetails;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	VCXLOGLO1("<<<CIptvTestVodContentAllDetails::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::~CIptvTestVodContentAllDetails
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTestVodContentAllDetails::~CIptvTestVodContentAllDetails()
    {
    VCXLOGLO1(">>>CIptvTestVodContentAllDetails::~CIptvTestVodContentAllDetails");
    iMediaContents.ResetAndDestroy();
    delete iBriefDetails;
    iBriefDetails = NULL;
    delete iFullDetails;
    iFullDetails = NULL;
    VCXLOGLO1("<<<CIptvTestVodContentAllDetails::~CIptvTestVodContentAllDetails");
    }

//  End of File
