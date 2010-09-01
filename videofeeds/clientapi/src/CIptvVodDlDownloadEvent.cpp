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
* Description:    used for sending download events from server to client*
*/





// INCLUDE FILES
#include <s32mem.h>
#include "IptvDebug.h"

#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvVodDlDownloadEvent.h"

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
// CIptvVodDlDownloadEvent::CIptvVodDlDownloadEvent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvVodDlDownloadEvent::CIptvVodDlDownloadEvent()
    {
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVodDlDownloadEvent::ConstructL()
    {
    iGlobalFileId = CIptvMyVideosGlobalFileId::NewL();
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlDownloadEvent* CIptvVodDlDownloadEvent::NewL()
    {
    CIptvVodDlDownloadEvent* self = new( ELeave ) CIptvVodDlDownloadEvent;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
EXPORT_C CIptvVodDlDownloadEvent::~CIptvVodDlDownloadEvent()
    {
    delete iGlobalFileId;
    }

EXPORT_C void CIptvVodDlDownloadEvent::Destruct()
    {
    delete this;
    }
    
// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvent::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadEvent::InternalizeL(RReadStream& aStream)
    {
    iEvent       = static_cast<TEvent>(aStream.ReadUint8L());
    iGlobalFileIdIsValid = aStream.ReadUint8L();
    iGlobalFileId->InternalizeL(aStream);
    iServiceIdAndContentIdAreValid = aStream.ReadUint8L();
    iServiceId    = aStream.ReadUint32L();
    iContentId    = aStream.ReadUint32L();
    iContentIndex = aStream.ReadUint32L();
    iProgress     = aStream.ReadUint8L();
    iState        = aStream.ReadUint32L();
    iError        = static_cast<TIptvDlError>(aStream.ReadUint32L());
    iGlobalError  = aStream.ReadInt32L(); 
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvent::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadEvent::ExternalizeL(RWriteStream& aStream)
    {
    aStream.WriteUint8L(static_cast<TUint8>(iEvent));
    aStream.WriteUint8L(iGlobalFileIdIsValid);
    iGlobalFileId->ExternalizeL(aStream);
    aStream.WriteUint8L(iServiceIdAndContentIdAreValid);
    aStream.WriteUint32L(iServiceId);
    aStream.WriteUint32L(iContentId);
    aStream.WriteUint32L(iContentIndex);
    aStream.WriteUint8L(iProgress);
    aStream.WriteUint32L(iState);
    aStream.WriteUint32L(static_cast<TUint32>(iError));
    aStream.WriteInt32L(iGlobalError);
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvent::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvVodDlDownloadEvent::CountExternalizeSize()
    {
    return 1 + //TUint8  iEvent
           1 + //TUint8 iGlobalFileIdIsValid
           iGlobalFileId->CountExternalizeSize() +
           1 + //TUint8 iServiceIdAndContentIdAreValid
           4 + //TUint32 iServiceId
           4 + //TUint32 iContentId
           4 + //TUint32 iContentIndex
           1 + //TUint8  iProgress
           4 + //TUint32 iState
           4 + //TUint32 iError
           4;  //TInt    iGlobalError
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvent::GetCopy
// -----------------------------------------------------------------------------
//
EXPORT_C MIptvEvent* CIptvVodDlDownloadEvent::GetCopyL()
    {    
    CIptvVodDlDownloadEvent* event = CIptvVodDlDownloadEvent::NewL();

    event->iEvent                         = iEvent;
    event->iGlobalFileIdIsValid           = iGlobalFileIdIsValid;
    event->iGlobalFileId->iFileId         = iGlobalFileId->iFileId;
    event->iGlobalFileId->iDrive          = iGlobalFileId->iDrive;
    event->iServiceIdAndContentIdAreValid = iServiceIdAndContentIdAreValid;
    event->iServiceId                     = iServiceId;
    event->iContentId                     = iContentId;  
    event->iContentIndex                  = iContentIndex;
    event->iProgress                      = iProgress;
    event->iState                         = iState;
    event->iError                         = iError;
    event->iGlobalError                   = iGlobalError;
    
    return event;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadEvent::Set
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadEvent::Set(CIptvVodDlDownloadEvent& aEvent)
    {
    iEvent                  = aEvent.iEvent;
    iGlobalFileIdIsValid    = aEvent.iGlobalFileIdIsValid;
    iGlobalFileId->iFileId  = aEvent.iGlobalFileId->iFileId;
    iGlobalFileId->iDrive   = aEvent.iGlobalFileId->iDrive;
    iServiceIdAndContentIdAreValid = aEvent.iServiceIdAndContentIdAreValid;
    iServiceId              = aEvent.iServiceId;
    iContentId              = aEvent.iContentId; 
    iContentIndex           = aEvent.iContentIndex;
    iProgress               = aEvent.iProgress;
    iState                  = aEvent.iState;
    iError                  = aEvent.iError;
    iGlobalError            = aEvent.iGlobalError;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
