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
* Description:    Defines data class structure for the API*
*/




// INCLUDE FILES
#include <e32base.h>
#include <s32strm.h>
#include <f32file.h>
#include "CIptvMyVideosGlobalFileId.h"

// CONSTANTS
const TInt KTUint32SizeInBytes = 4;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvMyVideosGlobalFileId::CIptvMyVideosGlobalFileId
// ---------------------------------------------------------
//
CIptvMyVideosGlobalFileId::CIptvMyVideosGlobalFileId()
 :  iDrive((TDriveNumber)0),
    iFileId(0)    
    {
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFileId::ConstructL
// ---------------------------------------------------------
//
void CIptvMyVideosGlobalFileId::ConstructL()
    {    
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFileId::NewL
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosGlobalFileId* CIptvMyVideosGlobalFileId::NewL()
    {
    CIptvMyVideosGlobalFileId* self = CIptvMyVideosGlobalFileId::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFileId::NewLC
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosGlobalFileId* CIptvMyVideosGlobalFileId::NewLC()
    {
    CIptvMyVideosGlobalFileId* self = new(ELeave) CIptvMyVideosGlobalFileId();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFileId::~CIptvMyVideosGlobalFileId
// ---------------------------------------------------------
//
CIptvMyVideosGlobalFileId::~CIptvMyVideosGlobalFileId()
    {    
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFileId::ExternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosGlobalFileId::ExternalizeL(RWriteStream& aStream)
    {
    // See TDriveNumber, EDriveC and EDriveE allowed.
    aStream.WriteInt32L(iDrive);

    // Drive specific file ID identifier.
    aStream.WriteUint32L(iFileId);
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFileId::InternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosGlobalFileId::InternalizeL(RReadStream& aStream)
    {
    // See TDriveNumber, EDriveC and EDriveE allowed.
    iDrive = aStream.ReadInt32L();

    // Drive specific file ID identifier.
    iFileId = aStream.ReadUint32L();
    }

// ---------------------------------------------------------
// CountExternalizeSize
// ---------------------------------------------------------
//
EXPORT_C TUint32 CIptvMyVideosGlobalFileId::CountExternalizeSize()
    {
    TUint32 size = 0;

    // See TDriveNumber, EDriveC and EDriveE allowed.
    size = KTUint32SizeInBytes +
    
    // Drive specific file ID identifier.
    KTUint32SizeInBytes;

    return size;    
    }
    
// End of file. 
