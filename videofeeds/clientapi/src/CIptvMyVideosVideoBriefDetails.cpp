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
#include "CIptvMyVideosVideoBriefDetails.h"

// CONSTANTS
const TInt KTUint32SizeInBytes = 4;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::CIptvMyVideosVideoBriefDetails
// ---------------------------------------------------------
//
CIptvMyVideosVideoBriefDetails::CIptvMyVideosVideoBriefDetails()
 :  iVideoFlags((TIptvMyVideosVideoFlags)0),
    iDownloadTime(2005,(TMonth)0,1,0,0,0,0),
    iContentId(0),
    iServiceId(0)
    {
    // Note: The initial value of iDownloadTime is used in comparisons on DB level.
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::ConstructL
// ---------------------------------------------------------
//
void CIptvMyVideosVideoBriefDetails::ConstructL()
    {
    iFileId = CIptvMyVideosGlobalFileId::NewL();

    iThumbnailPath = HBufC::NewL( 0 ); 
    iName          = HBufC::NewL( 0 );
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::NewL
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosVideoBriefDetails* CIptvMyVideosVideoBriefDetails::NewL()
    {
    CIptvMyVideosVideoBriefDetails* self = CIptvMyVideosVideoBriefDetails::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::NewLC
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosVideoBriefDetails* CIptvMyVideosVideoBriefDetails::NewLC()
    {
    CIptvMyVideosVideoBriefDetails* self = new (ELeave) CIptvMyVideosVideoBriefDetails();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::~CIptvMyVideosVideoBriefDetails
// ---------------------------------------------------------
//
CIptvMyVideosVideoBriefDetails::~CIptvMyVideosVideoBriefDetails()
    {
    delete iFileId;
    delete iThumbnailPath;
    delete iName;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::ExternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoBriefDetails::ExternalizeL(RWriteStream& aStream) const
    {
    // Global file ID identifier.
    iFileId->ExternalizeL(aStream);
    
    // Thumbnail path.
    CIptvUtil::WriteHBufCToStreamL( iThumbnailPath, aStream );

    // Video flags.
    aStream.WriteUint32L(static_cast<TUint32>(iVideoFlags));

    // Video name.
    CIptvUtil::WriteHBufCToStreamL( iName, aStream );
    
    // Download time.
    aStream.WriteInt32L(iDownloadTime.Year());
    aStream.WriteUint32L(static_cast<TUint32>(iDownloadTime.Month()));
    aStream.WriteInt32L(iDownloadTime.Day());
    aStream.WriteInt32L(iDownloadTime.Hour());
    aStream.WriteInt32L(iDownloadTime.Minute());
    aStream.WriteInt32L(iDownloadTime.Second());
    aStream.WriteInt32L(iDownloadTime.MicroSecond());
    
    // Content Id (from service provider data).
    aStream.WriteUint32L(iContentId);
    
    // Original service provider.
    aStream.WriteUint32L(iServiceId);

    // Age profile / rating age.
    aStream.WriteUint32L( iAgeProfile );

    // File size
    aStream.WriteInt32L( iFileSize );

    // Play time
    aStream.WriteReal32L( iPlayTime );
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::InternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoBriefDetails::InternalizeL(RReadStream& aStream)
    {
    // Global file ID identifier.
    iFileId->InternalizeL(aStream);

    // Thumbnail path.
    HBufC* temp = CIptvUtil::ReadHBufCFromStreamL( aStream );
    delete iThumbnailPath;
    iThumbnailPath = temp;
    
    // Video flags.
    iVideoFlags = static_cast<TIptvMyVideosVideoFlags>(aStream.ReadUint32L());
    
    // Video name.
    temp = CIptvUtil::ReadHBufCFromStreamL( aStream );
    delete iName;
    iName = temp;
    
    // Download time.
    iDownloadTime.SetYear(aStream.ReadInt32L());
    iDownloadTime.SetMonth(static_cast<TMonth>(aStream.ReadUint32L()));
    iDownloadTime.SetDay(aStream.ReadInt32L());
    iDownloadTime.SetHour(aStream.ReadInt32L());
    iDownloadTime.SetMinute(aStream.ReadInt32L());
    iDownloadTime.SetSecond(aStream.ReadInt32L());
    iDownloadTime.SetMicroSecond(aStream.ReadInt32L());

    // Content Id (from service provider data).
    iContentId = aStream.ReadUint32L();

    // Original service provider.
    iServiceId = aStream.ReadUint32L();

    // Age profile / rating age.
    iAgeProfile = aStream.ReadUint32L();

    // File size
    iFileSize = aStream.ReadInt32L();

    // Play time
    iPlayTime = aStream.ReadReal32L();
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::CountExternalizeSize
// ---------------------------------------------------------
//
EXPORT_C TUint32 CIptvMyVideosVideoBriefDetails::CountExternalizeSize()
    {
    TUint32 size = 0;
    
    // Global file ID identifier.
    size = (iFileId->CountExternalizeSize()) + 

    // Thumbnail path.
    KTUint32SizeInBytes + (iThumbnailPath->Length() * 2) +
    
    // Video flags.
    KTUint32SizeInBytes +

    // Video name.
    KTUint32SizeInBytes + (iName->Length() * 2) +

    // Download time.
    (7 * KTUint32SizeInBytes) +

    // Content Id.
    KTUint32SizeInBytes +

    // Original service provider.
    KTUint32SizeInBytes +

    // Age profile / rating age.
    KTUint32SizeInBytes +

    // File size
    sizeof(iFileSize) +

    // Play time
    sizeof(iPlayTime);

    return size;    
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::PlayTime
// ---------------------------------------------------------
//
EXPORT_C TReal32 CIptvMyVideosVideoBriefDetails::PlayTime()
    {
    return iPlayTime;
    }


// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::SetPlayTime
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoBriefDetails::SetPlayTime(
        TReal32 aPlayTime )
    {
    iPlayTime = aPlayTime;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::SetPlayTime
// ---------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosVideoBriefDetails::FileSize()
    {
    return iFileSize;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::SetPlayTime
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoBriefDetails::SetFileSize(
        TInt aSize )
    {
    iFileSize = aSize;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::SetThumbnailPathL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoBriefDetails::SetThumbnailPathL( const TDesC& aThumbnailPath )
    {
    if ( aThumbnailPath.Length() > KIptvEpgCategoryIconFilePathMaxLength )
        {
        User::Leave( KErrArgument );
        }
    
    HBufC* temp = aThumbnailPath.AllocL();
    delete iThumbnailPath;
    iThumbnailPath = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::ThumbnailPath
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CIptvMyVideosVideoBriefDetails::ThumbnailPath() const
    {
    if ( iThumbnailPath )
        {
        return *iThumbnailPath;
        }

    return KIptvEmptyDes;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::SetNameL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoBriefDetails::SetNameL( const TDesC& aName )
    {
    if ( aName.Length() > KIptvMyVideosNameMaxLength )
        {
        User::Leave( KErrArgument );
        }
            
    HBufC* temp = aName.AllocL();
    delete iName;
    iName = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoBriefDetails::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CIptvMyVideosVideoBriefDetails::Name() const
    {
    if ( iName )
        {
        return *iName;
        }

    return KIptvEmptyDes;
    }
    
// End of file.
