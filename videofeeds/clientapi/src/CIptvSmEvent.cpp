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
* Description:    Used for sending events from Service Manager to client*
*/





// INCLUDE FILES
#include "CIptvSmEvent.h"
#include <s32mem.h>
#include "IptvDebug.h"

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

// -----------------------------------------------------------------------------
// CIptvSmEvent::CIptvSmEvent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvSmEvent::CIptvSmEvent()
    {
    }

// -----------------------------------------------------------------------------
// CIptvSmEvent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvSmEvent::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CIptvSmEvent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvSmEvent* CIptvSmEvent::NewL()
    {
    CIptvSmEvent* self = new( ELeave ) CIptvSmEvent;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
EXPORT_C CIptvSmEvent::~CIptvSmEvent()
    {
    }

EXPORT_C void CIptvSmEvent::Destruct()
    {
    delete this;
    }

// -----------------------------------------------------------------------------
// CIptvSmEvent::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvSmEvent::InternalizeL(RReadStream& aStream)
    {
    iEvent     = static_cast<TEvent>(aStream.ReadInt32L());
    iServiceId = aStream.ReadUint32L();
    }

// -----------------------------------------------------------------------------
// CIptvSmEvent::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvSmEvent::ExternalizeL(RWriteStream& aStream)
    {
    aStream.WriteInt32L(static_cast<TInt32>(iEvent));
    aStream.WriteUint32L(iServiceId);
    }

// -----------------------------------------------------------------------------
// CIptvSmEvent::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvSmEvent::CountExternalizeSize()
    {
    return 8;
    }

// -----------------------------------------------------------------------------
// CIptvSmEvent::GetCopy
// -----------------------------------------------------------------------------
//
EXPORT_C MIptvEvent* CIptvSmEvent::GetCopyL()
    {    
    CIptvSmEvent* event = CIptvSmEvent::NewL();
    event->iEvent     = iEvent;
    event->iServiceId = iServiceId;
    
    return event;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
