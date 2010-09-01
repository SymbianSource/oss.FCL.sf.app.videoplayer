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
#include "CIptvMyVideosGlobalFolderId.h"

// CONSTANTS
const TInt KTUint32SizeInBytes = 4;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvMyVideosGlobalFolderId::CIptvMyVideosGlobalFolderId
// ---------------------------------------------------------
//
CIptvMyVideosGlobalFolderId::CIptvMyVideosGlobalFolderId()
 :  iDrive((TDriveNumber)0),
    iFolderId(0)    
    {
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFolderId::ConstructL
// ---------------------------------------------------------
//
void CIptvMyVideosGlobalFolderId::ConstructL()
    {    
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFolderId::NewL
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosGlobalFolderId* CIptvMyVideosGlobalFolderId::NewL()
    {
    CIptvMyVideosGlobalFolderId* self = CIptvMyVideosGlobalFolderId::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFolderId::NewLC
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosGlobalFolderId* CIptvMyVideosGlobalFolderId::NewLC()
    {
    CIptvMyVideosGlobalFolderId* self = new (ELeave) CIptvMyVideosGlobalFolderId();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFolderId::NewRootFolderL
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosGlobalFolderId* CIptvMyVideosGlobalFolderId::NewRootFolderL(TInt aDrive)
    {
    if ((aDrive != -1) && ((aDrive < EDriveA) || (aDrive > EDriveZ)))
        {
        User::Leave(KErrArgument);
        }

    CIptvMyVideosGlobalFolderId* self = CIptvMyVideosGlobalFolderId::NewL();
    if (aDrive != -1)
        {
        self->iDrive = aDrive;
        }
    else
        {
        self->iDrive = 0;
        }
    self->iFolderId = KIptvMyVideosFolderRootId;
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFolderId::~CIptvMyVideosGlobalFolderId
// ---------------------------------------------------------
//
CIptvMyVideosGlobalFolderId::~CIptvMyVideosGlobalFolderId()
    {    
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFolderId::ExternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosGlobalFolderId::ExternalizeL(RWriteStream& aStream) const
    {
    // See TDriveNumber, EDriveC and EDriveE allowed.
    aStream.WriteInt32L(iDrive);

    // Drive specific folder ID identifier.
    aStream.WriteUint32L(iFolderId);
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFolderId::InternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosGlobalFolderId::InternalizeL(RReadStream& aStream)
    {
    // See TDriveNumber, EDriveC and EDriveE allowed.
    iDrive = aStream.ReadInt32L();
   
    // Drive specific folder ID identifier.
    iFolderId = aStream.ReadUint32L();
    }

// ---------------------------------------------------------
// CIptvMyVideosGlobalFolderId::CountExternalizeSize
// ---------------------------------------------------------
//
EXPORT_C TUint32 CIptvMyVideosGlobalFolderId::CountExternalizeSize()
    {
    TUint32 size = 0;

    // See TDriveNumber, EDriveC and EDriveE allowed.
    size = KTUint32SizeInBytes +

    // Drive specific folder ID identifier.
    KTUint32SizeInBytes;
    
    return size;    
    }

// End of file
