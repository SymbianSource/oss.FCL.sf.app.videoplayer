/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Content information.*
*/




#ifndef CIPTVEPGCONTENT_H
#define CIPTVEPGCONTENT_H

#include <e32std.h>
#include "CIptvUtil.h"

/**
 * Content information data class.
 *
 * @lib IptvUtil.lib
 * @since Video Center 1.0
 */
class CIptvEpgContent : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvEpgContent* NewL();

        /**
         * Destructor.
         */
        virtual ~CIptvEpgContent();
                                
    private: // New functions  

        /**
         * C++ default constructor.
         */
        CIptvEpgContent();

        /**
         * By default Symbian 2nd phase constructor is private.
         * @param 
         */
        void ConstructL();
                  
    public: // Data members

        /**        
         * Unique key for database indexing.
         */
        TUint32 iKey;

        /**        
         * Content Id from service provider.
         */
        TBuf<KIptvIdMaxLength> iId;            

        /**        
         * Content name.
         */
        TBuf<KIptvEpgContentNameMaxLength> iName;
                      
        /**
         * Content protection.
         */ 
        TUint32 iContentProtection;

        /**        
         * Icon file id.
         */
        TUint32 iIconFileId;
        
        /**
         * If iIsIconFilePathUrl == ETrue,
         *     iIconFilePath is URL where icon can be downloaded.
         * If iIsIconFilePathUrl == EFalse,
         *     iIconFilePath is icons path to local filesystem.     
         */
        TBool iIsIconFilePathUrl;

        /**        
         * Icon file path, see iUrl.
         */
        TBuf<KIptvEpgContentIconFilePathMaxLength> iIconFilePath;   
                        
        /**        
         * Content file id.
         */        
        TUint32 iContentFileId;
                
        /**        
         * Description.
         */
        TBuf<KIptvEpgContentDescriptionMaxLength> iDescription;
        
        /**
         * Indicates when the item was published. In RSS 2.0 specification
         * <pubDate>
         */ 
        TTime iPubDate;

        /**        
         * Content home page, if exists. In RSS 2.0 specification <link>
         */
        TBuf<KIptvEpgContentBrowserUrlMaxLength> iBrowserUrl;

        /**        
         * Order number.
         */
        TInt iOrder;

        /**        
         * Author. In RSS 2.0 specification <author>
         */
        TBuf<KIptvEpgContentAuthorLength> iAuthor;

        /**        
         * Copyright. In Yahoo MRSS <media:copyright>
         */
        TBuf<KIptvEpgContentCopyrightLength> iCopyright;

        /**        
         * Size.
         */
        TUint32 iSize;        

        /**        
         * Duration.
         */
        TUint32 iDuration;

        /**        
         * Language
         */
        TBuf<KIptvCALanguageLength> iLanguage;

        /**
         * Rating age.
         * This parameter defines in years the age limit.
         * 0 means unlimited.
         * 18 means adult.
         */
        TUint16 iRatingAge;
    };

#endif // CIPTVEPGCONTENT_H
