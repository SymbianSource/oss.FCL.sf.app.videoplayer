/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/




// INCLUDE FILES
#include "TIptvIap.h"
#include "CIptvUtil.h"

// ============================ MEMBER FUNCTIONS ===============================

EXPORT_C TIptvIap::TIptvIap()
    : iValueExists( EFalse )
    , iId( 0 )
    , iPriority( 0 )
    {
    }
    
EXPORT_C TIptvIap::~TIptvIap()
    {
    }

EXPORT_C void TIptvIap::ExternalizeL(RDesWriteStream& aStream) const
    {
    aStream.WriteUint32L(iId);
    aStream.WriteUint16L(iPriority);
    }

EXPORT_C void TIptvIap::InternalizeL(RDesReadStream& aStream)
    {
    iId       = aStream.ReadUint32L();
    iPriority = aStream.ReadUint16L();
    }
    
EXPORT_C TUint32 TIptvIap::CountExternalizeSize()
    {
    return 4+2;
    }
