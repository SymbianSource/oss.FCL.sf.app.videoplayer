/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Utilities to handle media content.*
*/




#ifndef CIPTVMEDIACONTENT_H
#define CIPTVMEDIACONTENT_H

#include <e32std.h>
#include <s32strm.h>
#include "CIptvUtil.h"
#include "IptvDebug.h"

const TInt KIptvMediumMaxLength = 10;

/**
 * Utility class for handling media content.
 *
 * @lib IptvUtil.lib
 * @since Video Center 2.0
 */
class CIptvMediaContent : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvMediaContent* NewL();

        /**
         * Destructor.
         */
        virtual ~CIptvMediaContent();

        /**
         * Print
         */
        IMPORT_C void PrintL();

        /**
         * Externalize.
         */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

        /**
         * Internalize.
         */
        IMPORT_C void InternalizeL( RReadStream& aStream );

        /**
         * Count externalized size.
         */
        IMPORT_C TUint32 CountExternalizeSize();

        /**
         * Set media URL.
         */
        IMPORT_C void SetMediaContentUrlL( const TDesC& aBrowserUrl );

        /**
         * Get media URL.
         */
        IMPORT_C TPtrC GetMediaContentUrl() const;
                                
    private: // New functions

        /**
         * C++ default constructor.
         */
        CIptvMediaContent();

    public: // Data members

        /**
         * Expression types
         */ 
        enum TExpression
            {
            EFull = 0,
            ESample,
            ENonStop
            };

        /**
         * Content key (same as content id)
         */
        TUint32 iContentKey;

        /**
         * Index (This index identifies content access objects which
         * belong to some content defined by iContentKey)
         */
        TUint32 iIndex;

        /**
         * Download type.
         */
        TUint32 iDownloadType;

        /**
         * File id (Part of global file id (1/2)
         */
        TUint32 iFileId;

        /**
         * Drive id (Part of global file id (2/2) 
         */
        TUint32 iDriveId;
      
        /**
         * File size (MRSS)
         */ 
        TUint32 iFileSize; 

        /**
         * Mime-type (MRSS)
         */
        TBuf<KIptvCAMimeTypeLength> iMimeType;   

        /**
         * Medium (MRSS) (Not used in client side)
         * Medium is the type of object
         * (image | audio | video | document | executable).
         * While this attribute can at times seem redundant if type is supplied,
         * it is included because it simplifies decision making on the reader
         * side, as well as flushes out any ambiguities between MIME type and 
         * object type. It is an optional attribute.
         */
        TBuf<KIptvMediumMaxLength> iMedium;

        /**
         * Is default. (MRSS) (Not used in client side)
         * isDefault determines if this is the default object that should be
         * used for the <media:group>. There should only be one default object
         * per <media:group>. It is an optional attribute.
         */
        TBool iIsDefault;

        /**
         * Expression. (MRSS)
         * Expression determines if the object is a sample or the full version
         * of the object, or even if it is a continuous stream
         * (sample | full | nonstop). Default value is 'full'.
         * It is an optional attribute
         */
        TUint32 iExpression;

        /**
         * Bitrate. (MRSS) (Not used in client side)
         * Bitrate is the kilobits per second rate of media.
         * It is an optional attribute
         */
        TUint32 iBitrate;

        /**
         * Framerate. (MRSS) (Not used in client side)
         * Framerate is the number of frames per second for the media object.
         * It is an optional attribute
         */
        TUint32 iFramerate;

        /**
         * Samplingrate (MRSS) (Not used in client side)
         * Samplingrate is the number of samples per second taken to create
         * the media object. It is expressed in thousands of samples
         * per second (kHz). It is an optional attribute.
         */
        TUint32 iSamplingrate;

        /**
         * Duration. (MRSS)
         * Duration is the number of seconds the media object plays.
         * It is an optional attribute.
         */
        TUint32 iDuration;  

        /**
         * Height. (MRSS) (Not used in client side)
         * Height is the height of the media object. It is an optional attribute.
         */
        TUint32 iHeight;

        /**
         * Width. (MRSS) (Not used in client side)
         * Width is the width of the media object. It is an optional attribute.
         */
        TUint32 iWidth;

        /**
         * Language. (MRSS)
         * Lang is the primary language encapsulated in the media object.
         * Language codes possible are detailed in 
         * RFC 3066. This attribute is used similar to the xml:lang attribute
         * detailed in the XML 1.0 Specification (Third Edition).
         * It is an optional attribute.
         */
        TBuf<KIptvCALanguageLength> iLanguage;

        /**
         * Last viewing position of this video.
         */
        TUint32 iLastPosition;

    private:        

        /**        
         * Content URL (MRSS)
         * Own.
         */
        HBufC* iMediaContentUrl;
        
    };

#endif // CIPTVMEDIACONTENT_H
