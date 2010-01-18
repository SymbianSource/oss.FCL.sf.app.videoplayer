/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "TIptvUint32StreamObject.h"
#include <s32mem.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

EXPORT_C TIptvUint32StreamObject::TIptvUint32StreamObject()
    : iData(0)
    {
    }
    
EXPORT_C TIptvUint32StreamObject::~TIptvUint32StreamObject()
    {
    }

// -----------------------------------------------------------------------------
// TIptvUint32StreamObject::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TIptvUint32StreamObject::ExternalizeL(RWriteStream& aStream)
    {
    aStream.WriteUint32L(iData);
    }

// -----------------------------------------------------------------------------
// TIptvUint32StreamObject::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TIptvUint32StreamObject::InternalizeL(RReadStream& aStream)
    {
    iData = aStream.ReadUint32L();
    }

// -----------------------------------------------------------------------------
// TIptvUint32StreamObject::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TIptvUint32StreamObject::CountExternalizeSize()
    {
    return 4;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
