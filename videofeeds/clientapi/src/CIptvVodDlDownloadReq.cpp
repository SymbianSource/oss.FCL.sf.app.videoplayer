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
* Description:    Used to request download from VOD DL MGR*
*/




// INCLUDE FILES
#include <e32base.h>
#include "IptvDebug.h"

#include "IptvClientServerCommon.h"
#include "CIptvVodDlDownloadReq.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvVodDlDownloadReq::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvVodDlDownloadReq::ConstructL()
    {
    iObjId = EIptvEngineVodDlDownloadReq;
    }

// ---------------------------------------------------------
// CIptvVodDlDownloadReq::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CIptvVodDlDownloadReq* CIptvVodDlDownloadReq::NewL()
    {
    CIptvVodDlDownloadReq* self = new(ELeave) CIptvVodDlDownloadReq();
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvVodDlDownloadReq::~CIptvVodDlDownloadReq
// Destructor
// -----------------------------------------------------------------------------
//
CIptvVodDlDownloadReq::~CIptvVodDlDownloadReq()
    {
    }

// ---------------------------------------------------------
// CIptvVodDlDownloadReq::CIptvVodDlDownloadReq
// C++ default constructor
// ---------------------------------------------------------
//
CIptvVodDlDownloadReq::CIptvVodDlDownloadReq()
    {
    }

// ---------------------------------------------------------
// CIptvVodDlDownloadReq::InternalizeL
// ---------------------------------------------------------
//  
EXPORT_C void CIptvVodDlDownloadReq::InternalizeL(RReadStream& aStream)
    {
    iServiceId = aStream.ReadInt32L();            
    iContentId = aStream.ReadInt32L();
    iIndex     = aStream.ReadInt32L();
    iIapId     = aStream.ReadInt32L();
    iDlType    = static_cast<TIptvDownloadType>(aStream.ReadInt32L());    
    }

// ---------------------------------------------------------
// CIptvVodDlDownloadReq::ExternalizeL
// ---------------------------------------------------------
//    
EXPORT_C void CIptvVodDlDownloadReq::ExternalizeL(RWriteStream& aStream)
    {
    aStream.WriteInt32L(iServiceId);
    aStream.WriteInt32L(iContentId);
    aStream.WriteInt32L(iIndex);
    aStream.WriteInt32L(iIapId);    
    aStream.WriteInt32L(iDlType);
    }

// ---------------------------------------------------------
// CIptvVodDlDownloadReq::CountExternalizeSizeL
// In bytes.
// ---------------------------------------------------------
//    
EXPORT_C TUint32 CIptvVodDlDownloadReq::CountExternalizeSize()
    {
    return  
           4 +                                       /* iServiceId    */
           4 +                                       /* iContentId    */ 
           4 +                                       /* iIndex        */
           4 +                                       /* iIapId        */
           4;                                        /* iDlType       */
    }
