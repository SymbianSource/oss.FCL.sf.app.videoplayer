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
* Description:    IPC message.*
*/





// INCLUDE FILES
#include <s32mem.h>
#include "IptvDebug.h"
#include "CIptvUtil.h"

#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvVodDlResumeDownloadReq.h"

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
// CIptvVodDlResumeDownloadReq::CIptvVodDlResumeDownloadReq
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvVodDlResumeDownloadReq::CIptvVodDlResumeDownloadReq()
    {
    }

// -----------------------------------------------------------------------------
// CIptvVodDlResumeDownloadReq::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVodDlResumeDownloadReq::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CIptvVodDlResumeDownloadReq::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlResumeDownloadReq* CIptvVodDlResumeDownloadReq::NewL()
    {
    CIptvVodDlResumeDownloadReq* self = new( ELeave ) CIptvVodDlResumeDownloadReq;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
EXPORT_C CIptvVodDlResumeDownloadReq::~CIptvVodDlResumeDownloadReq()
    {
    }

// -----------------------------------------------------------------------------
// CIptvVodDlResumeDownloadReq::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlResumeDownloadReq::InternalizeL(RReadStream& aStream)
    {
    iDownloadId.InternalizeL(aStream);
    iUseUserNameAndPassword = static_cast<TBool>(aStream.ReadUint8L());
    CIptvUtil::ReadDesFromStreamL(iUserName, aStream);
    CIptvUtil::ReadDesFromStreamL(iPassword, aStream);
    iIapId    = aStream.ReadUint32L();
    iUseIapId = static_cast<TBool>(aStream.ReadUint8L());
    }

// -----------------------------------------------------------------------------
// CIptvVodDlResumeDownloadReq::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlResumeDownloadReq::ExternalizeL(RWriteStream& aStream)
    {
    iDownloadId.ExternalizeL(aStream);
    aStream.WriteUint8L(static_cast<TUint8>(iUseUserNameAndPassword));
    CIptvUtil::WriteDesToStreamL(iUserName, aStream);
    CIptvUtil::WriteDesToStreamL(iPassword, aStream);
    aStream.WriteUint32L(iIapId);
    aStream.WriteUint8L(static_cast<TUint8>(iUseIapId));
    }

// -----------------------------------------------------------------------------
// CIptvVodDlResumeDownloadReq::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvVodDlResumeDownloadReq::CountExternalizeSize()
    {
    TUint32 size = 0;
    size += iDownloadId.CountExternalizeSize();
    size += 1;
    size += (2 + (iUserName.Length()*2));
    size += (2 + (iPassword.Length()*2));
    size += 4;
    size += 1;
    return size;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
