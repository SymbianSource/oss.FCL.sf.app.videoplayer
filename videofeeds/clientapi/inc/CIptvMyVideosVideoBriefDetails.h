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
* Description:    Defines data class structure for the API.*
*/




#ifndef CIPTVMYVIDEOSVIDEOBRIEFDETAILS_H
#define CIPTVMYVIDEOSVIDEOBRIEFDETAILS_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include "CIptvMyVideosGlobalFileId.h"

class RWriteStream;
class RReadStream;

// DATA TYPES

typedef enum // Video flags
    {
    EIptvMyVideosVideoNew                = 0x00000001, // New video flag.
    EIptvMyVideosVideoScheduledDownload  = 0x00000002, // Scheduled flag, not downloaded yet.
    EIptvMyVideosVideoDownloading        = 0x00000004, // Downloading ongoing at the moment flag.
    EIptvMyVideosVideoDeleteWarning      = 0x00000008, // Deletion warning flag.
    EIptvMyVideosVideoProtected          = 0x00000010, // Protection flag.
    EIptvMyVideosVideoMarked             = 0x00000020, // Marked flag.
    EIptvMyVideosVideoDrmProtected       = 0x00000040, // DRM protection flag.
    EIptvMyVideosVideoMemoryCard         = 0x00000080, // Video located at the memorycard flag.
    EIptvMyVideosVideoDownloadFailed     = 0x00000100, // Download failed, but started flag.
    EIptvMyVideosVideoDownloadPaused     = 0x00000200, // Download paused.
    EIptvMyVideosVideoTypeRecording      = 0x00000400, // Video type is recording. // DUPLICATE in harvesterrtpmetadatarader.h : do not change this!
    EIptvMyVideosVideoIsRecording        = 0x00000800, // Video is being recorded. // DUPLICATE in harvesterrtpmetadatarader.h : do not change this!
    EIptvMyVideosVideoTypeMtp            = 0x00001000, // Deprecated.
    EIptvMyVideosVideoTypeDownload       = 0x00002000, // Video is originated from VC download.
    EIptvMyVideosVideoTypeGallery        = 0x00004000, // Video is originated from Gallery.
    EIptvMyVideosVideoTypeOther          = 0x00008000, // Video's origin is unknown.
    EIptvMyVideosVideoPreview            = 0x00010000, // Preview, not full clip.
    EIptvMyVideosVideoRecPartiallyFailed = 0x00020000, // Recording partially failed  // DUPLICATE in harvesterrtpmetadatarader.h : do not change this!
    EIptvMyVideosVideoRecFailed          = 0x00040000  // Recording failed  // DUPLICATE in harvesterrtpmetadatarader.h : do not change this!
    } TIptvMyVideosVideoFlags;
    

// CLASS DECLARATION
    
/**
* Defines data class structure for the API.
*
* @lib IptvClientApi.dll
*/
class CIptvMyVideosVideoBriefDetails : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosVideoBriefDetails object.
         */
        IMPORT_C static CIptvMyVideosVideoBriefDetails* NewL();

        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosVideoBriefDetails object.         
         */
        IMPORT_C static CIptvMyVideosVideoBriefDetails* NewLC();

        /**
         * Destructor.
         */
        virtual ~CIptvMyVideosVideoBriefDetails();

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
         * Return the file size of the video.
         *
         * @return The file size of the video.
         */
        IMPORT_C TInt FileSize();

        /**
         * Set the file size of the video.
         *
         * @param aSize The file size of the video.
         */
        IMPORT_C void SetFileSize( TInt aSize );

        /**
         * Get play time.
         *
         * @return The play time in seconds.
         */
        IMPORT_C TReal32 PlayTime();

        /**
         * Set play time.
         *
         * @param aPlayTime The play time in seconds.
         */
        IMPORT_C void SetPlayTime( TReal32 aPlayTime );

        /**
         * Set thumbnail path. KIptvEpgCategoryIconFilePathMaxLength is the
         * maximum length.
         *
         * @param aThumbnailPath ThumbnailPath
         */
        IMPORT_C void SetThumbnailPathL( const TDesC& aThumbnailPath );

        /**
         * Gets thumbnail path.
         *
         * @return tumbnail path.
         */
        IMPORT_C const TDesC& ThumbnailPath() const;

        /**
         * Set name. KIptvMyVideosNameMaxLength is the
         * maximum length.
         *
         * @param aName Name
         */
        IMPORT_C void SetNameL( const TDesC& aName );

        /**
         * Gets name.
         *
         * @return name.
         */
        IMPORT_C const TDesC& Name() const;

    public: // Data members

        /**        
         * Global file ID identifier.
         */
        CIptvMyVideosGlobalFileId* iFileId;        

        /**        
         * Video flags, see TIptvMyVideosVideoFlags.
         */
        TUint32 iVideoFlags;    

        /**        
         * Download time.
         */
        TDateTime iDownloadTime;  

        /**        
         * Original content ID (from service provider data).
         */
        TIptvContentId iContentId;

        /**        
         * Original service provider.
         */
        TIptvServiceId iServiceId;

        /**        
         * Age profile / rating age.
         */
        TUint32 iAgeProfile;

    protected:

        /**        
         * The size of the file.
         */
        TInt iFileSize;

        /**        
         * Video play time.
         */
        TReal32 iPlayTime;      

        /**        
         * Thumbnail path.
         */
        HBufC* iThumbnailPath; 

        /**        
         * Video name.
         */
        HBufC* iName;

    protected:

        /**
         * C++ default constructor.
         */
        CIptvMyVideosVideoBriefDetails();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();
    
    };

#endif // CIPTVMYVIDEOSVIDEOBRIEFDETAILS_H

// End of File
