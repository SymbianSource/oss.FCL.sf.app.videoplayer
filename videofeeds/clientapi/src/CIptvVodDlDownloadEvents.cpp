/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CIptvVodDlDownloadEvents.h"
#include "CIptvVodDlDownloadEvent.h"
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
// CIptvVodDlDownloadEvents::CIptvVodDlDownloadEvents
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvVodDlDownloadEvents::CIptvVodDlDownloadEvents()
    {
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvents::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVodDlDownloadEvents::ConstructL()
    {
    iEvents.Reset();
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvents::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlDownloadEvents* CIptvVodDlDownloadEvents::NewL()
    {
    CIptvVodDlDownloadEvents* self = new( ELeave ) CIptvVodDlDownloadEvents;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
EXPORT_C CIptvVodDlDownloadEvents::~CIptvVodDlDownloadEvents()
    {
    DeleteEvents();
    }


// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvents::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadEvents::InternalizeL(RReadStream& aStream)
    {
    TUint16 eventCount = aStream.ReadUint16L();
    TInt i;
    for(i = 0; i < eventCount; i++)
        {
        CIptvVodDlDownloadEvent* event = CIptvVodDlDownloadEvent::NewL();
        CleanupStack::PushL(event); // 1->
        event->InternalizeL(aStream);
        iEvents.AppendL(event);
        CleanupStack::Pop(event); // <-1
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvents::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadEvents::ExternalizeL(RWriteStream& aStream)
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
// CIptvVodDlDownloadEvents::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvVodDlDownloadEvents::CountExternalizeSize()
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
// CIptvVodDlDownloadEvents::Set
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvVodDlDownloadEvents::SetL(CIptvVodDlDownloadEvents& aEvents)
    {
    DeleteEvents();
    
    //copy new
    TInt i;
    for(i = 0; i < aEvents.iEvents.Count(); i++)
        {
        CIptvVodDlDownloadEvent* event = CIptvVodDlDownloadEvent::NewL();
        CleanupStack::PushL(event); // 1->
        event->Set(*(aEvents.iEvents[i]));
        iEvents.AppendL(event);
        CleanupStack::Pop(event); // <-1 
        }    
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvents::DeleteEvents
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadEvents::DeleteEvents()
    {
    TInt i;
    for(i = 0; i < iEvents.Count(); i++)
        {
        delete iEvents[i]; //delete data
        iEvents[i] = NULL;
        } 
    iEvents.Close(); //delete pointers
    iEvents.Reset();
    }
       
// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File  
