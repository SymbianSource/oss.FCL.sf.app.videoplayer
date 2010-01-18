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
* Description: 
*
*/




#ifndef MIPTVEPGVODCALLBACK_H
#define MIPTVEPGVODCALLBACK_H

#include <e32std.h>

// FORWARD DECLARATIONS
class CIptvEpgCategory;
class CIptvEpgContent;
class CIptvMediaContent;

// CLASS DECLARATION

enum TCategorySearchField
    {
    ECategoryName = 0,
    ECategoryId,
    ECategoryIconFileId
    };

enum TContentSearchField
    {
    EContentName = 0,
    EContentId, 
    EContentUrl,
    EContentIconFileId,
    EContentFileId
    };

enum TContentDecision
    {
    EContentAdd = 0,
    EContentUpdate,
    EContentLeave
    };

class MIptvEpgVodCallback
    {
    public:
        /**
         * Plugin started.
         *
         * @param none
         */
        virtual void StartedL() = 0;              
        
        /**
         * ECG content update from XML completed. Thumbnails are not downloaded yet.
         *
         * @param 
         */
        virtual void ContentUpdateCompletedL() = 0;

        /**
         * Plugin finished.
         *
         * @param aUpdateInterval       Update interval defines how long EPG data is valid.         
         */
        virtual TInt FinishedL(TTime& aUpdateInterval) = 0;

        /**
         * Add new category to database.         
         *
         * @param aCategory             Reference to CIptvEpgCategory object.
         * @param aCategoryKey          New category key value is inserted to aCategoryKey
         * @return                      KErrNone if successful, otherwise another of the system-wide error codes.
         */
        virtual TInt AddCategoryL(CIptvEpgCategory& aCategory, TUint32& aCategoryKey) = 0;

        /**
         * Add new content to given category.
         *
         * @param aContent          Reference to CIptvEpgContent object.         
         * @param aMediaContents    Pointer array containing media content objects. There can be n media content's for one 
         *                          content. 
         * @param aContentKey       New content key value is inserted to aContentKey       
         * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
         */
        virtual TInt AddContentL(CIptvEpgContent& aContent, RPointerArray<CIptvMediaContent> aMediaContents, TUint32& aContentKey) = 0;

        /**
         * Update service thumbnail.
         *         
         * @param aIconFilePath     Full path to service thumbnail in local filesystem.
         * @param aGrouped          if true updates also grouped item, otherwise not.
         * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
         */
        virtual TInt UpdateServiceThumbnailL( TDesC& aIconFilePath, const TBool aGrouped ) = 0;

        /**               
         * Add content to category.
         *
         * @param aCategoryKey      Category key.        
         * @param aContentKey       Content key. 
         */    
        virtual TInt AddRelationL(TUint32& aCategoryKey, TUint32& aContentKey) = 0;

        /**               
         * Error
         *
         * @param aError            Error code.         
         */    
        virtual void ErrorL(TInt aError, TInt aInfo) = 0;      

        /**
         * Update category icon.
         *
         * @param aCategoryKey      Category key.
         * @param aIconFileId       Icon file id to search from the database.         
         * @param aIconFilePath     Full path to icon file or URL + filename.
         * @param aIsUrl            If aIconFilePath is URL aIsUrl == ETrue, if aIconFilePath is path
         *                          to local filesystem aIsUrl == EFalse.
         * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
         */
        virtual TInt UpdateCategoryIconL(TUint32& aCategoryKey, TUint32& aIconFileId, TDesC& aIconFilePath, TBool aIsUrl) = 0;

        /**
         * Update content icon.
         *
         * @param aCategoryKey      Content key.
         * @param aIconFileId       Icon file id to search from the database.         
         * @param aIconFilePath     Full path to icon file or URL + filename.
         * @param aIsUrl            If aIconFilePath is URL aIsUrl == ETrue, if aIconFilePath is path
         *                          to local filesystem aIsUrl == EFalse.
         * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
         */
        virtual TInt UpdateContentIconL(TUint32& aContentKey, TUint32& aIconFileId, TDesC& aIconFilePath, TBool aIsUrl) = 0;
    };

#endif  // MIPTVEPGVODCALLBACK_H
