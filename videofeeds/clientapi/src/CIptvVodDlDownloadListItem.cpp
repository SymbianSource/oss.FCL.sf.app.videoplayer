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
* Description:    Contains information of ongoing download*
*/





// INCLUDE FILES
#include <s32mem.h>
#include "CIptvUtil.h"
#include "IptvDebug.h"

#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvVodDlDownloadListItem.h"

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
// CIptvVodDlDownloadListItem::CIptvVodDlDownloadListItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvVodDlDownloadListItem::CIptvVodDlDownloadListItem()
    {
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadListItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVodDlDownloadListItem::ConstructL()
    {
    iGlobalFileId = CIptvMyVideosGlobalFileId::NewL();
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadListItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlDownloadListItem* CIptvVodDlDownloadListItem::NewL()
    {
    CIptvVodDlDownloadListItem* self = new( ELeave ) CIptvVodDlDownloadListItem;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
EXPORT_C CIptvVodDlDownloadListItem::~CIptvVodDlDownloadListItem()
    {
    delete iGlobalFileId;    
    }


// -----------------------------------------------------------------------------
// CIptvVodDlDownloadListItem::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadListItem::InternalizeL(RReadStream& aStream)
    {
    iGlobalFileId->InternalizeL(aStream);
    iGlobalFileIdIsValid   = static_cast<TBool>(aStream.ReadUint8L());
    iServiceId             = aStream.ReadUint32L();
    iContentId             = aStream.ReadUint32L();
    iContentIndex          = aStream.ReadUint32L();
    iIapId                 = aStream.ReadUint32L();
    iDlType                = static_cast<TIptvDownloadType>(aStream.ReadUint8L());
    iProgress              = aStream.ReadUint32L();
    iState                 = aStream.ReadUint32L();
    iError                 = static_cast<TIptvDlError>(aStream.ReadUint32L());
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadListItem::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadListItem::ExternalizeL(RWriteStream& aStream)
    {
    iGlobalFileId->ExternalizeL(aStream);
    aStream.WriteUint8L(static_cast<TUint8>(iGlobalFileIdIsValid));
    aStream.WriteUint32L(iServiceId);
    aStream.WriteUint32L(iContentId);
    aStream.WriteUint32L(iContentIndex);
    aStream.WriteUint32L(iIapId);
    aStream.WriteUint8L(static_cast<TUint8>(iDlType));
    aStream.WriteUint32L(iProgress);
    aStream.WriteUint32L(iState);
    aStream.WriteUint32L(iError);
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadListItem::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvVodDlDownloadListItem::CountExternalizeSize()
    {
    TUint32 size = 0;
    size = iGlobalFileId->CountExternalizeSize();
    size += 1; //TBool   iGlobalFileIdIsValid
    size += 4; //TUint32 iServiceId
    size += 4; //TUint32 iContentId  
    size += 4; //TUint32 iContentIndex
    size += 4; //TUint32 iIapId
    size += 1; //TIptvDownloadType iDlType (TUint8)
    size += 4; //TUint32 iProgress
    size += 4; //TUint32 iState
    size += 4; //TIptvDlError iError
    return size;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
