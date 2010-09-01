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
* Description:    Encapsulates data which identifies VOD download.*
*/





// INCLUDE FILES
#include "TIptvVodDlDownloadId.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TIptvVodDlDownloadId::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TIptvVodDlDownloadId::InternalizeL(RReadStream& aStream)
    {
    iServiceIdAndContentIdAreValid = static_cast<TBool>(aStream.ReadUint8L());
    iServiceId                     = aStream.ReadUint32L();
    iContentId                     = aStream.ReadUint32L();
    iFileIdAndDriveAreValid        = static_cast<TBool>(aStream.ReadUint8L());
    iFileId                        = aStream.ReadUint32L();
    iDrive                         = aStream.ReadUint32L();
    }

// -----------------------------------------------------------------------------
// TIptvVodDlDownloadId::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void TIptvVodDlDownloadId::ExternalizeL(RWriteStream& aStream)
    {
    aStream.WriteUint8L(static_cast<TUint8>(iServiceIdAndContentIdAreValid));
    aStream.WriteUint32L(iServiceId);
    aStream.WriteUint32L(iContentId);
    aStream.WriteUint8L(static_cast<TUint8>(iFileIdAndDriveAreValid));
    aStream.WriteUint32L(iFileId);
    aStream.WriteUint32L(iDrive);
    }

// -----------------------------------------------------------------------------
// TIptvVodDlDownloadId::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TIptvVodDlDownloadId::CountExternalizeSize()
    {
    const TInt KExternalizeSize = 1+4+4+1+4+4;
    return KExternalizeSize; 
    }

// -----------------------------------------------------------------------------
// TIptvVodDlDownloadId::Set
// -----------------------------------------------------------------------------
//
EXPORT_C void TIptvVodDlDownloadId::Set(TIptvVodDlDownloadId& aDownloadId)
    {
    iServiceIdAndContentIdAreValid = aDownloadId.iServiceIdAndContentIdAreValid;
    iServiceId                     = aDownloadId.iServiceId;
    iContentId                     = aDownloadId.iContentId;
    iFileIdAndDriveAreValid        = aDownloadId.iFileIdAndDriveAreValid;
    iFileId                        = aDownloadId.iFileId;
    iDrive                         = aDownloadId.iDrive;    
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
