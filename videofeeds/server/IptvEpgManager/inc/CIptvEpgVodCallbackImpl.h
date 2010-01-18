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




#ifndef CIPTVEPGVODCALLBACKIMPL_H
#define CIPTVEPGVODCALLBACKIMPL_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include "MIptvEpgVodCallback.h"

// FORWARD DECLARATIONS
class CIptvEpgDb;
class CIptvEpgSession;
class CIptvMediaContent;
class CIptvEpgVodMsqQueue;

// CLASS DECLARATION
class CIptvEpgVodCallbackImpl : public CBase, public MIptvEpgVodCallback
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        static CIptvEpgVodCallbackImpl* NewL(CIptvEpgDb& aVodEpgDb, CIptvEpgVodMsqQueue* aMsgQueue, CIptvEpgSession& aSession);

        /**
         * Destructor.
         */
        ~CIptvEpgVodCallbackImpl();
        
        /**
         * Is plugin running.
         */
        TBool IsPluginRunning() { return iPluginRunning; };
                         
    public: // From MIptvEpgVodCallback interface

         /**
         * Plugin started.
         *
         * @param none
         */
        void StartedL();
   
        /**
         * ECG content update from XML completed. Thumbnails are not downloaded yet.
         *
         * @param 
         */
        void ContentUpdateCompletedL();

        /**
         * Plugin finished.
         *
         * @param aUpdateInterval       Update interval defines how long EPG data is valid.         
         */
        TInt FinishedL(TTime& aUpdateInterval);

        /**
         * Add new category to database.         
         *
         * @param aCategory             Reference to CIptvEpgCategory object.
         * @param aCategoryKey          New category key value is inserted to aCategoryKey
         * @return                      KErrNone if successful, otherwise another of the system-wide error codes.
         */
        TInt AddCategoryL(CIptvEpgCategory& aCategory, TUint32& aCategoryKey);

        /**
         * Add new content to given category.
         *
         * @param aContent          Reference to CIptvEpgContent object.         
         * @param aMediaContents    Pointer array containing media content objects. There can be n media content's for one 
         *                          content. 
         * @param aContentKey       New content key value is inserted to aContentKey       
         * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
         */
        TInt AddContentL(CIptvEpgContent& aContent, RPointerArray<CIptvMediaContent> aMediaContents, TUint32& aContentKey);

        /**
         * Update service thumbnail.
         *         
         * @param aIconFilePath     Full path to service thumbnail in local filesystem.
         * @param aGrouped          if true updates also grouped item, otherwise not.
         * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
         */
        TInt UpdateServiceThumbnailL( TDesC& aIconFilePath, const TBool aGrouped );

        /**               
         * Add content to category.
         *
         * @param aCategoryKey      Category key.        
         * @param aContentKey       Content key. 
         */    
        TInt AddRelationL(TUint32& aCategoryKey, TUint32& aContentKey);

        /**               
         * Error
         *
         * @param aError            Error code.         
         */    
        void ErrorL(TInt aError, TInt aInfo);      

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
        TInt UpdateCategoryIconL(TUint32& aCategoryKey, TUint32& aIconFileId, TDesC& aIconFilePath, TBool aIsUrl);

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
        TInt UpdateContentIconL(TUint32& aContentKey, TUint32& aIconFileId, TDesC& aIconFilePath, TBool aIsUrl);

    private:    // New functions           
        /**
        * C++ default constructor.
        */
        CIptvEpgVodCallbackImpl(CIptvEpgDb& aVodEpgDb, CIptvEpgVodMsqQueue* aMsgQueue, CIptvEpgSession& aSession);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();
                                                
    private:    // Data members                
        CIptvEpgDb& iVodEpgDb;        
        CIptvEpgVodMsqQueue* iMsgQueue;
        TBool iPluginRunning;                
        CIptvEpgSession& iSession;
    };

#endif  // CIPTVEPGVODCALLBACKIMPL_H

// End of File
