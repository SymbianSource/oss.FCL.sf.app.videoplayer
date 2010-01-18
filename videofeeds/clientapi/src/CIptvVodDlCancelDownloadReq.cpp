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
* Description:    Used to request download cancellation from VOD DL MGR*
*/




// INCLUDE FILES
#include <e32base.h>
#include "IptvDebug.h"

#include "IptvClientServerCommon.h"
#include "CIptvVodDlCancelDownloadReq.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvVodDlDownloadReq::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvVodDlCancelDownloadReq::ConstructL()
    {
    iObjId = EIptvEngineVodDlCancelDownloadReq;
    }

// ---------------------------------------------------------
// CIptvVodDlCancelDownloadReq::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CIptvVodDlCancelDownloadReq* CIptvVodDlCancelDownloadReq::NewL()
    {
    CIptvVodDlCancelDownloadReq* self = new(ELeave) CIptvVodDlCancelDownloadReq();
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvVodDlCancelDownloadReq::~CIptvVodDlCancelDownloadReq
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlCancelDownloadReq::~CIptvVodDlCancelDownloadReq()
    {
    }

// ---------------------------------------------------------
// CIptvVodDlCancelDownloadReq::CIptvVodDlCancelDownloadReq
// C++ default constructor
// ---------------------------------------------------------
//
CIptvVodDlCancelDownloadReq::CIptvVodDlCancelDownloadReq()
    {
    }

// ---------------------------------------------------------
// CIptvVodDlCancelDownloadReq::InternalizeL
// ---------------------------------------------------------
//  
EXPORT_C void CIptvVodDlCancelDownloadReq::InternalizeL(RReadStream& aStream)
    {
    iDownloadId.InternalizeL(aStream);
    }

// ---------------------------------------------------------
// CIptvVodDlCancelDownloadReq::ExternalizeL
// ---------------------------------------------------------
//    
EXPORT_C void CIptvVodDlCancelDownloadReq::ExternalizeL(RWriteStream& aStream)
    {
    iDownloadId.ExternalizeL(aStream);
    }

// ---------------------------------------------------------
// CIptvVodDlCancelDownloadReq::CountExternalizeSizeL
// In bytes.
// ---------------------------------------------------------
//    
EXPORT_C TUint32 CIptvVodDlCancelDownloadReq::CountExternalizeSize()
    {
    return iDownloadId.CountExternalizeSize();
    }

// ---------------------------------------------------------
// CIptvVodDlCancelDownloadReq::Set
// ---------------------------------------------------------
//    
EXPORT_C void CIptvVodDlCancelDownloadReq::Set(TIptvVodDlDownloadId& aDownloadId)
    {
    iDownloadId.Set(aDownloadId);
    }
