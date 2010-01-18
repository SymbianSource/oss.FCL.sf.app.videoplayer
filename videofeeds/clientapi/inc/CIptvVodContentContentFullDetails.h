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




#ifndef CIPTVVODCONTENTCONTENTFULLDETAILS_H
#define CIPTVVODCONTENTCONTENTFULLDETAILS_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include "CIptvVodContentContentBriefDetails.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
*  VoD ECG content API
*
*  @lib IptvClientApi.lib
*  @since Engine 1.0
*/

// CLASS DECLARATION
class CIptvVodContentContentFullDetails : public CIptvVodContentContentBriefDetails
    {
    public:
        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvVodContentContentFullDetails* NewL();

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
         * Destructor.
         */
        ~CIptvVodContentContentFullDetails();
                                
    private: // New functions           
        /**
        * C++ default constructor.
        */
        CIptvVodContentContentFullDetails();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();
                  
    public:     // Data members
                
        // Data members added to Brief details
                        
        /**        
        * Content description string.
        */
        TBuf<KIptvEpgContentDescriptionMaxLength> iDescription;       
                      
        /**        
        * DRM placeholder.
        */
        TInt iContentProtection; 
    
        /**        
        * Author.
        */        
        TBuf<KIptvEpgContentAuthorLength> iAuthor;

        /**        
        * Copyright.
        */                
        TBuf<KIptvEpgContentCopyrightLength> iCopyright;
    };

#endif  // CIPTVVodContentContentFullDetails.H
