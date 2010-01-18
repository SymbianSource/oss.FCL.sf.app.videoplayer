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
#include <e32std.h>
#include <s32strm.h>
#include "CIptvMyVideosVideoFullDetails.h"

// CONSTANTS
const TInt KTUint32SizeInBytes = 4;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::CIptvMyVideosVideoFullDetails
// ---------------------------------------------------------
//
CIptvMyVideosVideoFullDetails::CIptvMyVideosVideoFullDetails()
 :  iLastPlayPoint(0.0)
    {
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::ConstructL
// ---------------------------------------------------------
//
void CIptvMyVideosVideoFullDetails::ConstructL()
    {
    CIptvMyVideosVideoBriefDetails::ConstructL();
    iVideoPath      = HBufC::NewL(0);
    iDescription    = HBufC::NewL(0);
    iAudioLanguage  = HBufC::NewL(0);
    iMimeType       = HBufC::NewL(0);
    iAuthor         = HBufC::NewL(0);
    iCopyright      = HBufC::NewL(0);
    iFileSourceName = HBufC::NewL(0);
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::NewL
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosVideoFullDetails* CIptvMyVideosVideoFullDetails::NewL()
    {
    CIptvMyVideosVideoFullDetails* self = CIptvMyVideosVideoFullDetails::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::NewLC
// ---------------------------------------------------------
//
EXPORT_C CIptvMyVideosVideoFullDetails* CIptvMyVideosVideoFullDetails::NewLC()
    {
    CIptvMyVideosVideoFullDetails* self = new (ELeave) CIptvMyVideosVideoFullDetails();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::~CIptvMyVideosVideoFullDetails
// ---------------------------------------------------------
//
CIptvMyVideosVideoFullDetails::~CIptvMyVideosVideoFullDetails()
    {
    delete iVideoPath;
    delete iDescription;
    delete iAudioLanguage;
    delete iMimeType;
    delete iAuthor;
    delete iCopyright;
    delete iFileSourceName;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::ExternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::ExternalizeL(RWriteStream& aStream) const
    {
    // Externalize base class members.
    CIptvMyVideosVideoBriefDetails::ExternalizeL(aStream);

    // Video path in phone file system.
    CIptvUtil::WriteHBufCToStreamL( iVideoPath, aStream );

    // Video description.
    CIptvUtil::WriteHBufCToStreamL( iDescription, aStream );

    // Video audio language.
    CIptvUtil::WriteHBufCToStreamL( iAudioLanguage, aStream);

    // Last play point of the video.
    aStream.WriteReal32L(iLastPlayPoint);

    // Mime type.
    CIptvUtil::WriteHBufCToStreamL( iMimeType, aStream );

    // Video author.
    CIptvUtil::WriteHBufCToStreamL( iAuthor, aStream );

    // Video copyright.
    CIptvUtil::WriteHBufCToStreamL( iCopyright, aStream );

    // File source name.
    CIptvUtil::WriteHBufCToStreamL( iFileSourceName, aStream );
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::InternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::InternalizeL(RReadStream& aStream)
    {
    // Internalize base class members.
    CIptvMyVideosVideoBriefDetails::InternalizeL(aStream);

    // Video path in phone file system.
    HBufC* temp = CIptvUtil::ReadHBufCFromStreamL( aStream );
    delete iVideoPath;
    iVideoPath = temp;

    // Video description.
    temp = CIptvUtil::ReadHBufCFromStreamL( aStream );
    delete iDescription;
    iDescription = temp;
 
    // Video audio language.
    temp = CIptvUtil::ReadHBufCFromStreamL( aStream );
    delete iAudioLanguage;
    iAudioLanguage = temp;
    
    // Last play point of the video.
    iLastPlayPoint = aStream.ReadReal32L();

    // Mime type.
    temp = CIptvUtil::ReadHBufCFromStreamL( aStream );
    delete iMimeType;
    iMimeType = temp;    

    // Video author.
    temp = CIptvUtil::ReadHBufCFromStreamL( aStream );
    delete iAuthor;
    iAuthor = temp;
        
    // Video copyright.
    temp = CIptvUtil::ReadHBufCFromStreamL( aStream );
    delete iCopyright;
    iCopyright = temp;
    
    // File source name.
    temp = CIptvUtil::ReadHBufCFromStreamL( aStream );
    delete iFileSourceName;
    iFileSourceName = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::CountExternalizeSize
// ---------------------------------------------------------
//
EXPORT_C TUint32 CIptvMyVideosVideoFullDetails::CountExternalizeSize()
    {
    TUint32 size = 0;
    
    // Get the externalized size of base class members.
    size = (CIptvMyVideosVideoBriefDetails::CountExternalizeSize()) + 
    
    // Video path in phone file system.
    KTUint32SizeInBytes + (iVideoPath->Length() * 2) +
    
    // Video description.
    KTUint32SizeInBytes + (iDescription->Length() * 2) + 
    
    // Video audio language.
    KTUint32SizeInBytes + (iAudioLanguage->Length() * 2) + 
    
    // Last play point of the video.
    sizeof(TReal32) +

    // Mime type.
    KTUint32SizeInBytes + (iMimeType->Length() * 2) +

    // Video author.
    KTUint32SizeInBytes + (iAuthor->Length() * 2) + 

    // Video copyright.
    KTUint32SizeInBytes + (iCopyright->Length() * 2) +
    
    // File source name.
    KTUint32SizeInBytes + (iFileSourceName->Length() * 2);

    return size;    
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::LastPlayPoint
// ---------------------------------------------------------
//
EXPORT_C TReal32
        CIptvMyVideosVideoFullDetails::LastPlayPoint()
    {
    return iLastPlayPoint;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::SetLastPlayPoint
// ---------------------------------------------------------
//
EXPORT_C void
        CIptvMyVideosVideoFullDetails::SetLastPlayPoint(
                TReal32 aLastPlayPoint )
    {
    iLastPlayPoint = aLastPlayPoint;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::MDSorigin
// ---------------------------------------------------------
//
EXPORT_C TInt
        CIptvMyVideosVideoFullDetails::MDSOrigin()
    {
    return iMDSOrigin;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::SetMDSorigin
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::SetMDSOrigin(
        TInt aMDSOrigin )
    {
    iMDSOrigin = aMDSOrigin;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::MimeType
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CIptvMyVideosVideoFullDetails::MimeType() const
    {
    return *iMimeType;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::SetMimeType
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::SetMimeTypeL(
        const TDesC& aMimeType )
    {
    HBufC* temp = aMimeType.AllocL();

    delete iMimeType;
    iMimeType = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::SetVideoPathL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::SetVideoPathL( const TDesC& aVideoPath )
    {
    if ( aVideoPath.Length() > KIptvMaxPath )
        {
        User::Leave( KErrArgument );
        }
        
    HBufC* temp = aVideoPath.AllocL();
    delete iVideoPath;
    iVideoPath = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::VideoPath
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CIptvMyVideosVideoFullDetails::VideoPath() const
    {
    return *iVideoPath;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::SetDescriptionL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::SetDescriptionL( const TDesC& aDescription )
    {
    if ( aDescription.Length() > KIptvEpgContentDescriptionMaxLength )
        {
        User::Leave( KErrArgument );
        }
        
    HBufC* temp = aDescription.AllocL();
    delete iDescription;
    iDescription = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::Description
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CIptvMyVideosVideoFullDetails::Description() const
    {
    return *iDescription;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::SetAudioLanguageL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::SetAudioLanguageL( const TDesC& aAudioLanguage )
    {
    if ( aAudioLanguage.Length() > KIptvMyVideosVideoMaxAudioLanguageLen )
        {
        User::Leave( KErrArgument );
        }
        
    HBufC* temp = aAudioLanguage.AllocL();
    delete iAudioLanguage;
    iAudioLanguage = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::AudioLanguage
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CIptvMyVideosVideoFullDetails::AudioLanguage() const
    {
    return *iAudioLanguage;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::SetAuthorL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::SetAuthorL( const TDesC& aAuthor )
    {
    if ( aAuthor.Length() > KIptvMyVideosVideoMaxAuthorLength )
        {
        User::Leave( KErrArgument );
        }
        
    HBufC* temp = aAuthor.AllocL();
    delete iAuthor;
    iAuthor = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::Author
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CIptvMyVideosVideoFullDetails::Author() const
    {
    return *iAuthor;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::SetCopyrightL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::SetCopyrightL( const TDesC& aCopyright )
    {
    if ( aCopyright.Length() > KIptvMyVideosVideoMaxCopyrightLength )
        {
        User::Leave( KErrArgument );
        }
        
    HBufC* temp = aCopyright.AllocL();
    delete iCopyright;
    iCopyright = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::Copyright
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CIptvMyVideosVideoFullDetails::Copyright() const
    {
    return *iCopyright;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::SetFileSourceNameL
// ---------------------------------------------------------
//
EXPORT_C void CIptvMyVideosVideoFullDetails::SetFileSourceNameL( const TDesC& aFileSourceName )
    {
    if ( aFileSourceName.Length() > KIptvMyVideosVideoMaxFileSourceNameLength )
        {
        User::Leave( KErrArgument );
        }
        
    HBufC* temp = aFileSourceName.AllocL();
    delete iFileSourceName;
    iFileSourceName = temp;
    }

// ---------------------------------------------------------
// CIptvMyVideosVideoFullDetails::FileSourceName
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CIptvMyVideosVideoFullDetails::FileSourceName() const
    {
    return *iFileSourceName;
    }

// End of file.
