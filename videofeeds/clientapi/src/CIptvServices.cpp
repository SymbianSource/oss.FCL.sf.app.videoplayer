/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "IptvDebug.h"

#include "CIptvServices.h"
#include "CIptvService.h"

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
// CIptvServices::CIptvServices
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvServices::CIptvServices()
    {
    }

// -----------------------------------------------------------------------------
// CIptvServices::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvServices::ConstructL()
    {
    iServices.Reset();
    iService = CIptvService::NewL();
    }

// -----------------------------------------------------------------------------
// CIptvServices::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvServices* CIptvServices::NewL()
    {
    CIptvServices* self = new( ELeave ) CIptvServices;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
EXPORT_C CIptvServices::~CIptvServices()
    {
    TInt i;
    for ( i = iServices.Count() -1; i >= 0; i-- )
        {
        delete iServices[i];
        }
    iServices.Close();
    delete iService;
    }


// -----------------------------------------------------------------------------
// CIptvServices::GetServiceL
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvService* CIptvServices::GetServiceL(TUint32 aIndex)
    {
    if(aIndex > iServices.Count())
        {
        return NULL;
        }
    CIptvService* service = CIptvService::NewL();
    CleanupStack::PushL(service); //1->
    service->SetL(iServices[aIndex]->Des());
    CleanupStack::Pop(service); //<-1
    return service;
    }

// -----------------------------------------------------------------------------
// CIptvServices::GetServiceRefL
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvService& CIptvServices::GetServiceRefL(TUint32 aIndex)
    {
    if(aIndex > iServices.Count())
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServices::GetServiceL leaving, index out of bounds.");
        User::Leave(KErrNotFound);
        }
    iService->SetL(iServices[aIndex]->Des());
    return *iService;
    }

// -----------------------------------------------------------------------------
// CIptvServices::Service
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvService& CIptvServices::Service( TUint32 aIndex, TInt& aError )
    {
    if ( aIndex > iServices.Count() )
        {
        IPTVLOGSTRING_HIGH_LEVEL( "CIptvServices::Service index out of bounds." );
        aError = KErrNotFound;
        return *iService;
        }
    
    TRAP( aError, iService->SetL( iServices[aIndex]->Des() ) );
    return *iService;
    }

// -----------------------------------------------------------------------------
// CIptvServices::AddServiceL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvServices::AddL(CIptvService& aService)
    {
    HBufC8* serviceBin = aService.GetL();
    TInt err = iServices.Append(serviceBin);
    if(err != KErrNone)
        {
        delete serviceBin;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CIptvServices::Count
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvServices::Count()
    {
    return iServices.Count();
    }

// -----------------------------------------------------------------------------
// CIptvServices::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvServices::ExternalizeL(RDesWriteStream& aStream) const
    {
    TUint32 serviceCount = iServices.Count();
    aStream.WriteUint32L(serviceCount);
    TInt i;
    CIptvService* service = CIptvService::NewL();
    CleanupStack::PushL(service); // 1->
    for(i = 0; i < serviceCount; i++)
        {
        service->SetL(iServices[i]->Des());
        service->ExternalizeL(aStream);
        }
    CleanupStack::PopAndDestroy(service); // <-1
    }

// -----------------------------------------------------------------------------
// CIptvServices::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvServices::InternalizeL(RDesReadStream& aStream)
    {
    TUint32 serviceCount = aStream.ReadUint32L();
    TInt i, err;
    CIptvService* service = CIptvService::NewL();
    CleanupStack::PushL(service); // 1->
    for(i = 0; i < serviceCount; i++)
        {
        service->InternalizeL(aStream);
        HBufC8* serviceBin = service->GetL();
        err = iServices.Append(serviceBin);
        if(err != KErrNone)
            {
            delete serviceBin;
            }
        }
    CleanupStack::PopAndDestroy(service); // <-1
    }
    
// -----------------------------------------------------------------------------
// CIptvServices::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvServices::CountExternalizeSize()
    {
    TUint32 extSize = 4; //count field
    TInt i;
    for(i = 0; i < iServices.Count(); i++)
        {
        extSize += iServices[i]->Des().Length();
        }
    return extSize;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
