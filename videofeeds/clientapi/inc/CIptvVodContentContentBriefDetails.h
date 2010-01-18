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
* Description:    Defines data class structure for the API*
*/




#ifndef CIPTVVODCONTENTCONTENTBRIEFDETAILS_H
#define CIPTVVODCONTENTCONTENTBRIEFDETAILS_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CIptvMyVideosGlobalFileId;

/**
*  VoD ECG content API
*
*  @lib IptvClientApi.lib
*  @since Engine 1.0
*/

// CLASS DECLARATION
class CIptvVodContentContentBriefDetails : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvVodContentContentBriefDetails* NewL();

        /**
         * Externalize.
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

        /**
         * Internalize.
         */
        IMPORT_C void InternalizeL(RReadStream& aStream);

        /**
         * Count externalized size.
         */
        IMPORT_C TUint32 CountExternalizeSize();

        /**
         * Set browser URL.
         */
        IMPORT_C void SetBrowserUrlL(const TDesC& aBrowserUrl);

        /**
         * Get browser URL.
         */
        IMPORT_C TPtrC GetBrowserUrlL();

        /**
         * Destructor.
         */
        virtual ~CIptvVodContentContentBriefDetails();
           
    protected:                  
                
        /**
        * C++ default constructor.
        */
        CIptvVodContentContentBriefDetails();
    
        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();
                  
    public:     // Data members

        /**        
        * Content ID indetifier.
        */
        TIptvContentId iContentId;

        /**        
        * Content name string.
        */
        TBuf<KIptvEpgContentNameMaxLength> iName;
                       
        /**        
        * Complete thumbnail load path.
        */
        TBuf<KIptvEpgContentIconFilePathMaxLength> iThumbnailPath;

        /**        
        * Length in time.
        */
        TUint32 iPlaytime;          

        /**        
        * Content download file size.
        */
        TUint32 iSize;       

        /**        
        * Global file id.
        */
        CIptvMyVideosGlobalFileId* iFileId;

        /**        
        * Preview global file id.
        */
        CIptvMyVideosGlobalFileId* iPreviewGlobalFileId;

        /**
        * Indicates when the item was published. In RSS 2.0 specification
        * <pubDate>
        */ 
        TTime iPubDate;
        
        /**        
        * Language
        */
        TBuf<KIptvCALanguageLength> iLanguage;

        /**        
         * Rating age.
         */
        TUint16 iRatingAge;

    private:        

        /**        
        * Content home page, if exists. In RSS 2.0 specification <link>
        */
        HBufC* iBrowserUrl;
    };

#endif  // CIPTVVODCONTENTCONTENTBRIEFDETAILS_H
