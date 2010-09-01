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




#ifndef CIPTVMYVIDEOSVIDEOFULLDETAILS_H
#define CIPTVMYVIDEOSVIDEOFULLDETAILS_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include "CIptvMyVideosVideoBriefDetails.h"

class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
* Defines data class structure for the API.
*
* @lib IptvClientApi.dll
*/
class CIptvMyVideosVideoFullDetails : public CIptvMyVideosVideoBriefDetails
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosVideoFullDetails object.
         */
        IMPORT_C static CIptvMyVideosVideoFullDetails* NewL();

        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosVideoFullDetails object.
         */
        IMPORT_C static CIptvMyVideosVideoFullDetails* NewLC();

        /**
         * Destructor.
         */
        virtual ~CIptvMyVideosVideoFullDetails();

        /**
         * Externalize.
         *
         * @param aStream Stream to externalize object to.
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

        /**
         * Internalize.
         *
         * @param aStream Stream to internalize object from.
         */
        IMPORT_C void InternalizeL(RReadStream& aStream);

        /**
         * Count externalized size.
         *
         * @return Externalized size.
         */
        IMPORT_C TUint32 CountExternalizeSize();

        /**
         * Get last play point.
         *
         * @return The play point in seconds.
         */
        IMPORT_C TReal32 LastPlayPoint();

        /**
         * Set last play point.
         *
         * @param aLastPlayPoint The play point in seconds.
         */
        IMPORT_C void SetLastPlayPoint( TReal32 aLastPlayPoint );

        /**
         * Get the MDS origin.
         *
         * @return The the MDS origin.
         */
        IMPORT_C TInt MDSOrigin();

        /**
         * Set the MDS origin.
         *
         * @param aMDSOrigin The the MDS origin.
         */
        IMPORT_C void SetMDSOrigin( TInt aMDSOrigin );

        /**
         * Get the MDS origin. Please notice that the mime type can be up to
         * 64k in MDS.
         *
         * @return The the MDS origin.
         */
        IMPORT_C const TDesC& MimeType() const;

        /**
         * Set the MDS origin.
         *
         * @param aMDSOrigin The the MDS origin.
         */
        IMPORT_C void SetMimeTypeL( const TDesC& aMimeType );


        /**
         * Set video path.
         *
         * @param aVideoPath Video path, KIptvMaxPath is the maximum length.
         */
        IMPORT_C void SetVideoPathL( const TDesC& aVideoPath );

        /**
         * Get video path.
         *
         * @return Video path.
         */        
        IMPORT_C const TDesC& VideoPath() const;

        /**
         * Set description.
         *
         * @param aDescription Description, KIptvEpgContentDescriptionMaxLength
         *                     is the maximum length.
         */
        IMPORT_C void SetDescriptionL( const TDesC& aDescription );

        /**
         * Get description.
         *
         * @return description.
         */        
        IMPORT_C const TDesC& Description() const;

        /**
         * Set audio language.
         *
         * @param aAudioLanguage Audio language, KIptvMyVideosVideoMaxAudioLanguageLen
         *                       is the maximum length.
         */
        IMPORT_C void SetAudioLanguageL( const TDesC& aAudioLanguage );

        /**
         * Get audio language.
         *
         * @return Audio language.
         */        
        IMPORT_C const TDesC& AudioLanguage() const;

        /**
         * Set author.
         *
         * @param aAuthor Author, KIptvMyVideosVideoMaxAuthorLength
         *                is the maximum length.
         */
        IMPORT_C void SetAuthorL( const TDesC& aAuthor );

        /**
         * Get author.
         *
         * @return Author.
         */        
        IMPORT_C const TDesC& Author() const;

        /**
         * Set copyright.
         *
         * @param aCopyright Copyright, KIptvMyVideosVideoMaxCopyrightLength
         *                   is the maximum length.
         */
        IMPORT_C void SetCopyrightL( const TDesC& aCopyright );

        /**
         * Get copyright.
         *
         * @return Copyright.
         */        
        IMPORT_C const TDesC& Copyright() const;

        /**
         * Set file source name.
         *
         * @param aFileSourceName File source name, KIptvMyVideosVideoMaxFileSourceNameLength
         *                        is the maximum length.
         */
        IMPORT_C void SetFileSourceNameL( const TDesC& aFileSourceName );

        /**
         * Get file source name.
         *
         * @return File source name.
         */        
        IMPORT_C const TDesC& FileSourceName() const;

    protected: 

        /**
         * C++ default constructor.
         */
        CIptvMyVideosVideoFullDetails();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();

    private:
    
        /**        
         * Video path in phone file system.
         */
        HBufC* iVideoPath;     

        /**        
         * Video description.
         */
        HBufC* iDescription;

        /**        
         * Video audio language.
         */
        HBufC* iAudioLanguage; 

        /**
         * Video author.
         */
        HBufC* iAuthor;

        /**
         * Video copyright.
         */
        HBufC* iCopyright;

        /**
         * File source name.
         */
        HBufC* iFileSourceName;

    protected:

        /**        
         * Last play point of the video.
         */
        TReal32 iLastPlayPoint;

        /**        
         * The MDS origin of the object.
         */
        TInt iMDSOrigin;

        /**
         * Mime type.
         */
        HBufC* iMimeType;

    };

#endif  // CIPTVMYVIDEOSVIDEOFULLDETAILS_H

// End of File
