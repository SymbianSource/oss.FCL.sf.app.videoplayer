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
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvMyVideosGlobalFileId.h"

// CONSTANTS
const TInt KTUint32SizeInBytes = 4;
const TInt KTUint16SizeInBytes = 2;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvVodContentContentBriefDetails::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvVodContentContentBriefDetails::ConstructL()
    {
    iFileId = CIptvMyVideosGlobalFileId::NewL();
    iPreviewGlobalFileId = CIptvMyVideosGlobalFileId::NewL();
    }

// ---------------------------------------------------------
// CIptvVodContentContentBriefDetails::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CIptvVodContentContentBriefDetails* CIptvVodContentContentBriefDetails::NewL()
    {
    CIptvVodContentContentBriefDetails* self = new(ELeave) CIptvVodContentContentBriefDetails();
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvVodContentContentBriefDetails::~CIptvVodContentContentBriefDetails
// Destructor
// -----------------------------------------------------------------------------
//
CIptvVodContentContentBriefDetails::~CIptvVodContentContentBriefDetails()
    {    
    delete iFileId;    
    delete iPreviewGlobalFileId;
    delete iBrowserUrl;
    }

// ---------------------------------------------------------
// CIptvVodContentContentBriefDetails::CIptvVodContentContentBriefDetails
// C++ default constructor
// ---------------------------------------------------------
//
CIptvVodContentContentBriefDetails::CIptvVodContentContentBriefDetails()
    {
    }

// ---------------------------------------------------------
// Set browser URL
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvVodContentContentBriefDetails::SetBrowserUrlL(const TDesC& aBrowserUrl)
    {
    if (aBrowserUrl.Length() > 0)
        {
        delete iBrowserUrl;
        iBrowserUrl = NULL;
        iBrowserUrl = HBufC::NewL(aBrowserUrl.Length());
        TPtr16 p = iBrowserUrl->Des();
        p.Copy(aBrowserUrl);    
        }
    }

// ---------------------------------------------------------
// Get browser URL
// 
// -----------------------------------------------------------------------------
//  
EXPORT_C TPtrC CIptvVodContentContentBriefDetails::GetBrowserUrlL()
    {
    if (iBrowserUrl)
        {
        return iBrowserUrl->Des();
        }
          
    return TPtrC();
    }

// ---------------------------------------------------------
// Externalize
// 
// ---------------------------------------------------------
//
EXPORT_C void CIptvVodContentContentBriefDetails::ExternalizeL(RWriteStream& aStream) const
    {
    // Global file id
    iFileId->ExternalizeL(aStream);

    // Preview global file id
    iPreviewGlobalFileId->ExternalizeL(aStream);

    // Content ID indetifier.
    aStream.WriteUint32L(iContentId);
    
    // Content name
    CIptvUtil::WriteDesToStreamL(iName, aStream);
        
    // Complete thumbnail load path.
    CIptvUtil::WriteDesToStreamL(iThumbnailPath, aStream);

    // PubDate
    TInt64 time = iPubDate.Int64();    
    TUint32 lower = (0x00000000FFFFFFFFULL) & time;
    TUint32 upper = (0x00000000FFFFFFFFULL) & (time >> 32);
    aStream.WriteUint32L(lower);
    aStream.WriteUint32L(upper);

    // Browser url
    if (iBrowserUrl)
        {
        CIptvUtil::WriteDesToStreamL(*iBrowserUrl, aStream);
        }
    else
        {
        aStream.WriteUint16L(0);
        }    

    // Size
    aStream.WriteUint32L(iSize);    

    // Duration (playtime)
    aStream.WriteUint32L(iPlaytime);    

    // Language
    CIptvUtil::WriteDesToStreamL(iLanguage, aStream);

    // Rating age.
    aStream.WriteUint16L( iRatingAge );
    }

// ---------------------------------------------------------
// Internalize
// 
// ---------------------------------------------------------
//
EXPORT_C void CIptvVodContentContentBriefDetails::InternalizeL(RReadStream& aStream)
    {
    // Global file id
    iFileId->InternalizeL(aStream);

    // Preview global file id
    iPreviewGlobalFileId->InternalizeL(aStream);

    // Content ID indetifier.
    iContentId = aStream.ReadUint32L();

    // Content name
    CIptvUtil::ReadDesFromStreamL(iName, aStream);
    
    // Thumbnail path
    CIptvUtil::ReadDesFromStreamL(iThumbnailPath, aStream);

    // PubDate
    TUint32 lower = aStream.ReadUint32L();
    TUint32 upper = aStream.ReadUint32L();
    TInt64 time = 0;
    time = lower;    
    TInt64 longUpper = (TInt64) upper;
    longUpper = longUpper << 32;        
    longUpper &= (0xFFFFFFFF00000000ULL);
    time |= longUpper;
    iPubDate = time;

    // Browser url
    delete iBrowserUrl;
    iBrowserUrl = NULL;
    iBrowserUrl = HBufC::NewL(KIptvEpgContentBrowserUrlMaxLength);
    TPtr16 p = iBrowserUrl->Des();
    CIptvUtil::ReadDesFromStreamL(p, aStream);
    iBrowserUrl = iBrowserUrl->ReAllocL(iBrowserUrl->Length());

    // Size
    iSize = aStream.ReadUint32L();

    // Duration (playtime)
    iPlaytime = aStream.ReadUint32L();

    // Language
    CIptvUtil::ReadDesFromStreamL(iLanguage, aStream);

    // Rating age.
    iRatingAge = aStream.ReadUint16L();
    }

// ---------------------------------------------------------
// Count externalized size
// 
// ---------------------------------------------------------
//
EXPORT_C TUint32 CIptvVodContentContentBriefDetails::CountExternalizeSize()
    {    
    TUint32 size           = 0;
    TUint32 browserUrlSize = 0;
    
    if (iBrowserUrl)
        {
        browserUrlSize = (iBrowserUrl->Length() * 2);
        }

    // Global file id
    size = iFileId->CountExternalizeSize() +

    // Preview global file id
    iPreviewGlobalFileId->CountExternalizeSize() +

    // Content ID indetifier.
    KTUint32SizeInBytes +    

    // Content name
    KTUint16SizeInBytes + (iName.Length() * 2) +
    
    // Thumbnail path
    KTUint16SizeInBytes + (iThumbnailPath.Length() * 2) +

    // pubDate
    ( KTUint32SizeInBytes * 2 ) +

    // Browser url
    KTUint16SizeInBytes + browserUrlSize +

    // Size
    KTUint32SizeInBytes +

    // Duration (playtime)
    KTUint32SizeInBytes +

    // Language
    KTUint16SizeInBytes + (iLanguage.Length() * 2) +

    // Rating age.
    KTUint16SizeInBytes;
    
    return size;
    }

// End of file.
