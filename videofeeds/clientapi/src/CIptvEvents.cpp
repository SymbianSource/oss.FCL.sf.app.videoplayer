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
* Description:    Contains CIptvVodDlDownloadEvent objects*
*/





// INCLUDE FILES
#include "CIptvEvents.h"
#include "MIptvEvent.h"
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

// -----------------------------------------------------------------------------
// CIptvEvents::CIptvEvents
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvEvents::CIptvEvents(MIptvEvent* aEventProto)
: iEventProto(aEventProto)
    {
    }

// -----------------------------------------------------------------------------
// CIptvEvents::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvEvents::ConstructL()
    {
    iEvents.Reset();
    }

// -----------------------------------------------------------------------------
// CIptvEvents::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvEvents* CIptvEvents::NewL(MIptvEvent* aEventProto)
    {
    CIptvEvents* self = new( ELeave ) CIptvEvents(aEventProto);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
EXPORT_C CIptvEvents::~CIptvEvents()
    {
    DeleteEvents();
    iEventProto->Destruct();
    }


// -----------------------------------------------------------------------------
// CIptvEvents::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvEvents::InternalizeL(RReadStream& aStream)
    {
    TUint16 eventCount = aStream.ReadUint16L();
    TInt i;
    for(i = 0; i < eventCount; i++)
        {
        MIptvEvent* event = iEventProto->GetCopyL();
        CleanupStack::PushL(event); // 1->
        event->InternalizeL(aStream);
        iEvents.AppendL(event);
        CleanupStack::Pop(event); // <-1
        }
    }

// -----------------------------------------------------------------------------
// CIptvEvents::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvEvents::ExternalizeL(RWriteStream& aStream)
    {
    TInt eventCount = iEvents.Count();
    aStream.WriteUint16L(eventCount);
    TInt i;
    for(i = 0; i < eventCount; i++)
        {
        iEvents[i]->ExternalizeL(aStream);
        }
    }

// -----------------------------------------------------------------------------
// CIptvEvents::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvEvents::CountExternalizeSize()
    {
    TUint size = 0;
    size += 2;
    TInt i;
    for(i = 0; i < iEvents.Count(); i++)
        {
        size += iEvents[i]->CountExternalizeSize();
        }
    return size;
    }

// -----------------------------------------------------------------------------
// CIptvEvents::Set
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvEvents::SetL(CIptvEvents& aEvents)
    {
    DeleteEvents();
    
    //copy new
    TInt i;
    for(i = 0; i < aEvents.iEvents.Count(); i++)
        {
        MIptvEvent* event = aEvents.iEvents[i]->GetCopyL();
        CleanupStack::PushL(event); // 1->
        iEvents.AppendL(event);
        CleanupStack::Pop(event); // <-1 
        }    
    }

// -----------------------------------------------------------------------------
// CIptvEvents::DeleteEvents
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvEvents::DeleteEvents()
    {
    TInt i;
    for(i = 0; i < iEvents.Count(); i++)
        {
        iEvents[i]->Destruct(); //iEvents[i] is MIptvEvent type, MIptvEvent::Destruct() calls derived classes destructor
        } 
    iEvents.Close(); //delete pointers
    iEvents.Reset();
    }
       
// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File  
