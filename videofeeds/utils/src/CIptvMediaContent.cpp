/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Utilities to handle media content*
*/




#include <e32base.h>
#include "IptvDebug.h"
#include "CIptvMediaContent.h"

#ifdef _DEBUG
const TInt KIptvMaxDebugPrint = 100;
#endif

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CIptvMediaContent::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CIptvMediaContent* CIptvMediaContent::NewL()
    {
    CIptvMediaContent* self = new ( ELeave ) CIptvMediaContent();
    return self;
    }

// --------------------------------------------------------------------------
// CIptvMediaContent::~CIptvMediaContent
// Destructor
// --------------------------------------------------------------------------
//
CIptvMediaContent::~CIptvMediaContent()
    {
    delete iMediaContentUrl;
    }

// --------------------------------------------------------------------------
// CIptvMediaContent::CIptvMediaContent
// C++ default constructor
// --------------------------------------------------------------------------
//
CIptvMediaContent::CIptvMediaContent()
    {
    }

// --------------------------------------------------------------------------
// CIptvMediaContent::PrintL
// --------------------------------------------------------------------------
//
EXPORT_C void CIptvMediaContent::PrintL()
    {
#ifdef _DEBUG

    IPTVLOGSTRING_LOW_LEVEL( "CIptvMediaContent::PrintL" );
    
    TBuf<KIptvMaxDebugPrint> temp;
    TInt count = Min( KIptvMaxDebugPrint, iMediaContentUrl->Length() );
    temp.Copy( iMediaContentUrl->Mid( 0, count ) );

    IPTVLOGSTRING2_LOW_LEVEL("--- Url:           %S", &temp);

    switch ( iDownloadType )
        {
        case EIptvDownloadTypeStream:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "--- Download type: EIptvDownloadTypeStream" );
            break;
            }

        case EIptvDownloadTypeImmediate:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "--- Download type: EIptvDownloadTypeImmediate" );
            break;
            }

        case EIptvDownloadTypePostponed:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "--- Download type: EIptvDownloadTypePostponed" );
            break;
            }

        case EIptvDownloadTypeScheduled:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "--- Download type: EIptvDownloadTypeScheduled" );
            break;
            }

        default:
            {
            break;
            }
        }
   
    IPTVLOGSTRING2_LOW_LEVEL( "--- Filesize:      %d", iFileSize );
    IPTVLOGSTRING2_LOW_LEVEL( "--- Mime-type:     %S", &iMimeType );
    IPTVLOGSTRING2_LOW_LEVEL( "--- Medium:        %S", &iMedium );
    
    if ( iIsDefault )
        {
        IPTVLOGSTRING_LOW_LEVEL( "--- Is default:    ETrue" );
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( "--- Is default:    EFalse" );
        }                        

    IPTVLOGSTRING2_LOW_LEVEL( "--- Bitrate:       %d", iBitrate );
    IPTVLOGSTRING2_LOW_LEVEL( "--- Framerate:     %d", iFramerate );
    IPTVLOGSTRING2_LOW_LEVEL( "--- Samplingrate:  %d", iSamplingrate );
    IPTVLOGSTRING2_LOW_LEVEL( "--- Duration:      %d", iDuration );
    IPTVLOGSTRING2_LOW_LEVEL( "--- Height:        %d", iHeight );
    IPTVLOGSTRING2_LOW_LEVEL( "--- Width:         %d", iWidth );
    IPTVLOGSTRING2_LOW_LEVEL( "--- Language:      %S", &iLanguage );

    switch ( iExpression )
        {
        case EFull:
            {
            IPTVLOGSTRING_LOW_LEVEL("--- Expression:       EFull");            
            break;
            }

        case ESample:
            {
            IPTVLOGSTRING_LOW_LEVEL("--- Expression:       ESample");
            break;
            }

        case ENonStop:
            {
            IPTVLOGSTRING_LOW_LEVEL("--- Expression:       ENonStop");
            break;
            }

        default:
            {
            break;
            }
        }      

    IPTVLOGSTRING2_LOW_LEVEL( "--- Last position: %d", iLastPosition );
#endif
    }

// --------------------------------------------------------------------------
// Externalize
// --------------------------------------------------------------------------
//
EXPORT_C void CIptvMediaContent::ExternalizeL( RWriteStream& aStream ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvMediaContent::ExternalizeL" );

    // Content key
    aStream.WriteUint32L( iContentKey );

    // Index
    aStream.WriteUint32L( iIndex );

    // Download type
    aStream.WriteUint32L( iDownloadType );

    // File id (Part of global file id (1/2)
    aStream.WriteUint32L( iFileId );

    // Drive id (Part of global file id (2/2) 
    aStream.WriteUint32L( iDriveId );

    // Content URL (MRSS)
    if ( iMediaContentUrl )
        {
        CIptvUtil::WriteDesToStreamL( *iMediaContentUrl, aStream );
        }
    else
        {
        aStream.WriteUint32L( 0 );
        }    

    // File size (MRSS)
    aStream.WriteUint32L( iFileSize );

    // Mime-type (MRSS
    CIptvUtil::WriteDesToStreamL( iMimeType, aStream );

    // Medium (not used in client side)
    
    // iIsDefault (not used in client side)

    // Expression    
    aStream.WriteUint32L( iExpression );
    
    // Bitrate (not used in client side)

    // Framerate (not used in client side)

    // Samplingrate (not used in client side)
    
    // Duration
    aStream.WriteUint32L( iDuration );

    // Height (Not used in client side)

    // Width (Not used in client side)

    // Language
    CIptvUtil::WriteDesToStreamL( iLanguage, aStream );

    // Last position
    aStream.WriteUint32L( iLastPosition );
    }

// --------------------------------------------------------------------------
// Internalize
// --------------------------------------------------------------------------
//
EXPORT_C void CIptvMediaContent::InternalizeL( RReadStream& aStream )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvMediaContent::InternalizeL" );

    // Content key
    iContentKey = aStream.ReadUint32L();

     // Index
    iIndex = aStream.ReadUint32L();
    
    // Download type
    iDownloadType = aStream.ReadUint32L();

    // File id (Part of global file id (1/2)
    iFileId = aStream.ReadUint32L();

    // Drive id (Part of global file id (2/2) 
    iDriveId = aStream.ReadUint32L();

    // Content URL (MRSS)
    delete iMediaContentUrl;
    iMediaContentUrl = NULL;

    iMediaContentUrl = HBufC::NewL( KIptvEpgContentBrowserUrlMaxLength );
    TPtr16 p = iMediaContentUrl->Des();
    CIptvUtil::ReadDesFromStreamL( p, aStream );
    iMediaContentUrl =
        iMediaContentUrl->ReAllocL( iMediaContentUrl->Length() );

    // File size (MRSS)
    iFileSize = aStream.ReadUint32L();

    // Mime-type (MRSS)
    CIptvUtil::ReadDesFromStreamL( iMimeType, aStream );

    // Expression
    iExpression = aStream.ReadUint32L();
    
    // Duration
    iDuration = aStream.ReadUint32L();

    // Language
    CIptvUtil::ReadDesFromStreamL( iLanguage, aStream );

    // Last position
    iLastPosition = aStream.ReadUint32L();
    }

// --------------------------------------------------------------------------
// Count externalized size
// --------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvMediaContent::CountExternalizeSize()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvMediaContent::CountExternalizeSize" );
    
    TUint32 size = 0;
    
    size =
        // Content key
        4 +

        // Index
        4 +

        // Download type
        4 +

        // File id (Part of global file id (1/2)    
        4 +
    
        // Drive id (Part of global file id (2/2) 
        4 +
    
        // Content URL (MRSS)
        4 + ( iMediaContentUrl->Length() * 2 ) +
    
        // File size (MRSS)
        4 +

        // Mime-type (MRSS)
        4 + ( iMimeType.Length() * 2 ) +

        // Expression
        4 + 

        // Duration
        4 +
    
        // Language
        4 + ( iLanguage.Length() * 2 ) +

        // Last position
        4;

    return size;
    }

// --------------------------------------------------------------------------
// Set media content URL
// --------------------------------------------------------------------------
//
EXPORT_C void CIptvMediaContent::SetMediaContentUrlL(
    const TDesC& aMediaUrl )
    {
    delete iMediaContentUrl;
    iMediaContentUrl = NULL;

    TInt actualLength = Min( KIptvCAContentUrlLength, aMediaUrl.Length() );

    if ( actualLength > 0 )
        {
        iMediaContentUrl = HBufC::NewL( actualLength );
        TPtr16 ptr = iMediaContentUrl->Des();
        ptr.Copy( aMediaUrl.Mid( 0, actualLength ) );
        }
    }

// --------------------------------------------------------------------------
// Get media content URL
// --------------------------------------------------------------------------
//
EXPORT_C TPtrC CIptvMediaContent::GetMediaContentUrl() const
    {
    if ( iMediaContentUrl )
        {
        return iMediaContentUrl->Des();
        }

    return TPtrC();
    }
