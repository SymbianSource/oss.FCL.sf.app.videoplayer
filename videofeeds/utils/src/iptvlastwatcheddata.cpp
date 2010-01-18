/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Contains Video Center's last watched item's data.*
*/



#include "IptvDebug.h"
#include "CIptvUtil.h"
#include <s32mem.h>

#include "iptvlastwatcheddata.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CIptvLastWatchedData::CIptvLastWatchedData() :
    iServiceId( KIdUndefined ),
    iContentId( KIdUndefined ),
    iMpxId( KIdUndefined )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CIptvLastWatchedData* CIptvLastWatchedData::NewL()
    {
    return new( ELeave ) CIptvLastWatchedData;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CIptvLastWatchedData::~CIptvLastWatchedData()
    {
    delete iName;
    delete iUri;
    delete iIconPath;
    delete iMimeType;
    delete iParameters;
    }

// ---------------------------------------------------------------------------
// Construct class from binary stream
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvLastWatchedData::InternalizeL( RReadStream& aStream )
    {
    iAppUid.iUid      = aStream.ReadUint32L();
    iViewUid.iUid     = aStream.ReadUint32L();
    iParameterId.iUid = aStream.ReadUint32L();

    iLaunchType       = static_cast<TLaunchType>( aStream.ReadUint32L() );

    delete iName;
    iName = NULL;
    iName = HBufC::NewL( KIptvMyVideosVideoNameMaxLength );
    TPtr  pName       = iName->Des();
    CIptvUtil::ReadDesFromStreamL( pName, aStream );
    iName->ReAlloc( iName->Length() );

    delete iUri;
    iUri = NULL;
    iUri = HBufC::NewL( KIptvLastWatchedUriMaxLength );
    TPtr  pUri        = iUri->Des();
    CIptvUtil::ReadDesFromStreamL( pUri, aStream );
    iUri->ReAlloc( iUri->Length() );

    delete iIconPath;
    iIconPath = NULL;
    iIconPath = HBufC::NewL( KIptvMaxPath );
    TPtr  pIconPath   = iIconPath->Des();
    CIptvUtil::ReadDesFromStreamL( pIconPath, aStream );
    iIconPath->ReAlloc( iIconPath->Length() );

    delete iMimeType;
    iMimeType = NULL;
    iMimeType = HBufC::NewL( KIptvMaxPath );
    TPtr  pMimeType   = iMimeType->Des();
    CIptvUtil::ReadDesFromStreamL( pMimeType, aStream );
    iMimeType->ReAlloc( iMimeType->Length() );

    iServiceId        = aStream.ReadInt32L();
    iContentId        = aStream.ReadInt32L();
    iContentIndex     = aStream.ReadInt32L();

    iLastVideoPlayPoint = aStream.ReadUint32L();
    iParentalControl  = aStream.ReadUint32L();

    delete iParameters;
    iParameters       = NULL;
    iParameters       = HBufC8::NewL( KIptvLastWatchedStartparamMaxLength );
    TPtr8 pParameters = iParameters->Des();
    CIptvUtil::ReadDes8FromStreamL( pParameters, aStream );
    iParameters->ReAlloc( iParameters->Length() );

#if !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)

    TInt offset = aStream.Source()->TellL( MStreamBuf::ERead ).Offset();
    TInt size = aStream.Source()->SizeL();

    // Check if iContentType is in stream (file). If we are reading old file format,
    // we have already reached the EOF, but in new file format there's iContentType.
    if ( ( offset + 4 ) <= size )
        {
        iContentType = aStream.ReadInt32L();
        }

    offset = aStream.Source()->TellL( MStreamBuf::ERead ).Offset();

    // Check if iMpxId is in stream (file). If we are reading old file format,
    // we have already reached the EOF, but in new file format there's iMpxId.
    if ( ( offset + 4 ) <= size )
        {
        iMpxId = aStream.ReadInt32L();
        }

#endif // !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)    
    }

// ---------------------------------------------------------------------------
// Write class to binary stream
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvLastWatchedData::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteUint32L( iAppUid.iUid );
    aStream.WriteUint32L( iViewUid.iUid );
    aStream.WriteUint32L( iParameterId.iUid );
    aStream.WriteUint32L( iLaunchType );

    CIptvUtil::WriteDesToStreamL( Name(), aStream );
    CIptvUtil::WriteDesToStreamL( Uri(), aStream );
    CIptvUtil::WriteDesToStreamL( IconPath(), aStream );
    CIptvUtil::WriteDesToStreamL( MimeType(), aStream );

    aStream.WriteInt32L( iServiceId );
    aStream.WriteInt32L( iContentId );
    aStream.WriteInt32L( iContentIndex );
    aStream.WriteUint32L( iLastVideoPlayPoint );
    aStream.WriteUint32L( iParentalControl );

    CIptvUtil::WriteDes8ToStreamL( Parameters(), aStream );

#if !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)

    aStream.WriteInt32L( iContentType );

    aStream.WriteInt32L( iMpxId );
    
#endif // !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetNameL( const TDesC& aName )
    {
    if ( aName.Length() > KIptvMyVideosVideoNameMaxLength )
        {
        return KErrArgument;
        }

    delete iName;
    iName = NULL;
    iName = aName.AllocL();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetUriL( const TDesC& aUri )
    {
    if ( aUri.Length() > KIptvLastWatchedUriMaxLength )
        {
        return KErrArgument;
        }

    delete iUri;
    iUri = NULL;
    iUri = aUri.AllocL();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetIconPathL( const TDesC& aIconPath )
    {
    if ( aIconPath.Length() > KIptvMaxPath )
        {
        return KErrArgument;
        }

    delete iIconPath;
    iIconPath = NULL;
    iIconPath = aIconPath.AllocL();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetMimeTypeL( const TDesC& aMimeType )
    {
    if ( aMimeType.Length() > KIptvMyVideosVideoMaxMimeTypeLength )
        {
        return KErrArgument;
        }

    delete iMimeType;
    iMimeType = NULL;
    iMimeType = aMimeType.AllocL();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetServiceId(
    const TInt32 aServiceId )
    {
    iServiceId = aServiceId;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetContentId(
    const TIptvContentId aContentId )
    {
    iContentId = aContentId;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetContentIndex(
    const TInt32 aContentIndex )
    {
    iContentIndex = aContentIndex;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetLastVideoPlayPoint(
    const TUint32 aLastVideoPlayPoint )
    {
    iLastVideoPlayPoint = aLastVideoPlayPoint;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetParentalControl(
    const TUint32 aParentalControl )
    {
    iParentalControl = aParentalControl;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetParametersL( const TDesC8& aParameters )
    {
    if ( aParameters.Length() > KIptvLastWatchedStartparamMaxLength )
        {
        return KErrArgument;
        }

    delete iParameters;
    iParameters = NULL;
    iParameters = aParameters.AllocL();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvLastWatchedData::SetAppUid( const TUid& aAppUid )
    {
    iAppUid = aAppUid;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvLastWatchedData::SetViewUid( const TUid& aViewUid )
    {
    iViewUid = aViewUid;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvLastWatchedData::SetParameterId( const TUid& aParameterId )
    {
    iParameterId = aParameterId;
    }

// ---------------------------------------------------------------------------
// Setter
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvLastWatchedData::SetLaunchType( TLaunchType aLaunchType )
    {
    iLaunchType = aLaunchType;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIptvLastWatchedData::Name() const
    {
    if ( iName )
        {
        return *iName;
        }
    else
        {
        return KNullDesC();
        }
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIptvLastWatchedData::Uri() const
    {
    if ( iUri )
        {
        return *iUri;
        }
    else
        {
        return KNullDesC();
        }
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIptvLastWatchedData::IconPath() const
    {
    if ( iIconPath )
        {
        return *iIconPath;
        }
    else
        {
        return KNullDesC();
        }
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIptvLastWatchedData::MimeType() const
    {
    if ( iMimeType )
        {
        return *iMimeType;
        }
    else
        {
        return KNullDesC();
        }
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt32 CIptvLastWatchedData::ServiceId() const
    {
    return iServiceId;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C TIptvContentId CIptvLastWatchedData::ContentId() const
    {
    return iContentId;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C TInt32 CIptvLastWatchedData::ContentIndex() const
    {
    return iContentIndex;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvLastWatchedData::LastVideoPlayPoint() const
    {
    return iLastVideoPlayPoint;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvLastWatchedData::ParentalControl() const
    {
    return iParentalControl;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CIptvLastWatchedData::Parameters() const
    {
    if ( iParameters )
        {
        return *iParameters;
        }
    else
        {
        return KNullDesC8();
        }
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CIptvLastWatchedData::AppUid() const
    {
    return iAppUid;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CIptvLastWatchedData::ViewUid() const
    {
    return iViewUid;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CIptvLastWatchedData::ParameterId() const
    {
    return iParameterId;
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C CIptvLastWatchedData::TLaunchType CIptvLastWatchedData::LaunchType()
    const
    {
    return iLaunchType;
    }

// -----------------------------------------------------------------------------
// CIptvLastWatchedData::CountExternalizeSize()
// Return externalize size in 8bit bytes.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvLastWatchedData::CountExternalizeSize() const
    {
    TUint32 size =
            2 +  ( Name().Length() * 2 ) +     // HBufC* iName
            2 +  ( Uri().Length() * 2 ) +      // HBufC* iUri
            2 +  ( IconPath().Length() * 2 ) + // HBufC* iIconPath
            2 +  ( MimeType().Length() * 2 ) + // HBufC* iMimeType
            4 +                                // TInt32 iServiceId;
            4 +                                // TIptvContentId iContentId;
            4 +                                // TInt32 ContentIndex;
            4 +                                // TUint32 iLastVideoPlayPoint;
            4 +                                // TUint32 iParentalControl;
            2 +  ( Parameters().Length() ) +   // HBufC8* iParameters
            4 +                                // TUid iAppUid
            4 +                                // TUid iViewUid
            4 +                                // TUid iParameterId
            4;                                 // TLaunchType iLaunchType
    
#if !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)

    size += 4;                                 // TUint32 iContentType
    size += 4;                                 // TUint32 iMpxId
    
#endif // !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
    
    return size;
    }

// -----------------------------------------------------------------------------
// !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetContentType( const TInt aContentType )    
    {
    iContentType = aContentType;
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::ContentType() const
    {
    return iContentType;
    }

// -----------------------------------------------------------------------------
// !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::SetMpxId( const TInt aMpxId )    
    {
    iMpxId = aMpxId;
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedData::MpxId() const
    {
    return iMpxId;
    }
