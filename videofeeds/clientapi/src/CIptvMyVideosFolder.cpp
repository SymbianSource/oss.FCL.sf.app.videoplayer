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
#include "CIptvMyVideosFolder.h"

// CONSTANTS
const TInt KTUint16SizeInBytes = 2;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvMyVideosFolder::CIptvMyVideosFolder
// ---------------------------------------------------------
//
CIptvMyVideosFolder::CIptvMyVideosFolder()
    {
    }

// ---------------------------------------------------------
// CIptvMyVideosFolder::ConstructL
// ---------------------------------------------------------
//
void CIptvMyVideosFolder::ConstructL()
    {
    iFolderId = CIptvMyVideosGlobalFolderId::NewL();
    }

// ---------------------------------------------------------
// CIptvMyVideosFolder::NewL
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosFolder* CIptvMyVideosFolder::NewL()
    {
    CIptvMyVideosFolder* self = CIptvMyVideosFolder::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosFolder::NewLC
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosFolder* CIptvMyVideosFolder::NewLC()
    {
    CIptvMyVideosFolder* self = new (ELeave) CIptvMyVideosFolder();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosFolder::~CIptvMyVideosFolder
// ---------------------------------------------------------
//
CIptvMyVideosFolder::~CIptvMyVideosFolder()
    {
    delete iFolderId;
    }

// ---------------------------------------------------------
// CIptvMyVideosFolder::ExternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosFolder::ExternalizeL(RWriteStream& aStream) const
    {
    // Global folder ID identifier.
    iFolderId->ExternalizeL(aStream);

    // Folder name.
    CIptvUtil::WriteDesToStreamL(iName, aStream);
    }

// ---------------------------------------------------------
// CIptvMyVideosFolder::InternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosFolder::InternalizeL(RReadStream& aStream)
    {
    // Global folder ID identifier.
    iFolderId->InternalizeL(aStream);

    // Folder name.
    CIptvUtil::ReadDesFromStreamL(iName, aStream);
    }

// ---------------------------------------------------------
// CIptvMyVideosFolder::CountExternalizeSize
// ---------------------------------------------------------
//
EXPORT_C TUint32 CIptvMyVideosFolder::CountExternalizeSize()
    {
    TUint32 size = 0;
    
    // Global folder ID identifier.
    size = (iFolderId->CountExternalizeSize()) + 

    // Folder name.
    KTUint16SizeInBytes + (iName.Length() * 2);
    
    return size;    
    }
    
// End of file.
