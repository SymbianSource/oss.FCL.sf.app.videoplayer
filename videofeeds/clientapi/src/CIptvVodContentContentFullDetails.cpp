/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
#include "IptvDebug.h"
#include "CIptvVodContentContentFullDetails.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvVodContentContentFullDetails::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvVodContentContentFullDetails::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentContentFullDetails::ConstructL");                  

    CIptvVodContentContentBriefDetails::ConstructL();    
    }

// ---------------------------------------------------------
// CIptvVodContentContentFullDetails::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CIptvVodContentContentFullDetails* CIptvVodContentContentFullDetails::NewL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentContentFullDetails::NewL");                  

    CIptvVodContentContentFullDetails* self = new(ELeave) CIptvVodContentContentFullDetails();
    CleanupStack::PushL(self);

    self->ConstructL(); 

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvVodContentContentFullDetails::~CIptvVodContentContentFullDetails
// Destructor
// -----------------------------------------------------------------------------
//
CIptvVodContentContentFullDetails::~CIptvVodContentContentFullDetails()    
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentContentFullDetails::~CIptvVodContentContentFullDetails");                  
    }

// ---------------------------------------------------------
// CIptvVodContentContentFullDetails::CIptvVodContentContentFullDetails
// C++ default constructor
// ---------------------------------------------------------
//
CIptvVodContentContentFullDetails::CIptvVodContentContentFullDetails()    
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentContentFullDetails::CIptvVodContentContentFullDetails");                              
    }
    
// ---------------------------------------------------------
// Externalize
// 
// ---------------------------------------------------------
//
EXPORT_C void CIptvVodContentContentFullDetails::ExternalizeL(RWriteStream& aStream) const
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentContentFullDetails::ExternalizeL");                              

    CIptvVodContentContentBriefDetails::ExternalizeL(aStream);
        
    // Content description string.
    CIptvUtil::WriteDesToStreamL(iDescription, aStream);
           
    //  DRM placeholder.
    aStream.WriteInt32L(iContentProtection);

    // Author
    CIptvUtil::WriteDesToStreamL(iAuthor, aStream);

    // Copyright
    CIptvUtil::WriteDesToStreamL(iCopyright, aStream);
    }

// ---------------------------------------------------------
// Internalize
// 
// ---------------------------------------------------------
//
EXPORT_C void CIptvVodContentContentFullDetails::InternalizeL(RReadStream& aStream)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentContentFullDetails::InternalizeL");                              

    CIptvVodContentContentBriefDetails::InternalizeL(aStream);
                
    // Content description string.
    CIptvUtil::ReadDesFromStreamL(iDescription, aStream);
                    
    //  DRM placeholder.
    iContentProtection = aStream.ReadInt32L();                        

    // Author
    CIptvUtil::ReadDesFromStreamL(iAuthor, aStream);

    // Copyright
    CIptvUtil::ReadDesFromStreamL(iCopyright, aStream);
    }

// ---------------------------------------------------------
// Count externalized size
// 
// ---------------------------------------------------------
//
EXPORT_C TUint32 CIptvVodContentContentFullDetails::CountExternalizeSize()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentContentFullDetails::CountExternalizeSize");                              

    TUint32 size = 0;

    size = CIptvVodContentContentBriefDetails::CountExternalizeSize() +
    
    // Content description string.
    2 + (iDescription.Length() * 2) +
        
    //  DRM placeholder.
    4 +

    // Author
    2 + (iAuthor.Length() * 2) +

    // Copyright
    2 + (iCopyright.Length() * 2);
            
    return size;
    }

// End of file.
