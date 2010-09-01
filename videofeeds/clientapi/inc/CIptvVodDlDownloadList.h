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
* Description: 
*
*/





#ifndef CIPTVVODDLDOWNLOADLIST_H
#define CIPTVVODDLDOWNLOADLIST_H

//  INCLUDES
#include <e32base.h>
#include "MIptvStreamObject.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvVodDlDownloadListItem;
class CIptvMyVideosGlobalFileId;
class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
*  Used with CIptvVodDlClient::GetDownloadListL method.
*
*  @lib IptvClientApi.lib
*/
class CIptvVodDlDownloadList : public CBase, public MIptvStreamObject
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvVodDlDownloadList* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvVodDlDownloadList();

    public: // New functions
        
        /**
        * Initializes this object from stream.
        * @param aStream stream to read data from
        */
        IMPORT_C void InternalizeL(RReadStream& aStream);

        /**
        * Writes this object to stream.
        * @param aStream stream to write data to.
        */
        IMPORT_C void ExternalizeL(RWriteStream& aStream);

        /**
        * Counts externalize size.
        */
        IMPORT_C TUint32 CountExternalizeSize();

        /**
         * Get pointer to list item.
         * Items which have iGlobalFileIdIsValid as EFalse are not returned. 
         * @param aGlobalFileId
         * @return returns pointer to list item, ownership does not move, caller MAY NOT free the pointer.
         */        
        IMPORT_C CIptvVodDlDownloadListItem* GetListItem(CIptvMyVideosGlobalFileId& aGlobalFileId);
        

        /**
         * Get pointer to list item by service id and content id.
         * @param aServiceId
         * @param aContentId
         * @return returns pointer to list item, ownership does not move, caller MAY NOT free the pointer.
         */
        IMPORT_C CIptvVodDlDownloadListItem* GetListItem(TUint32 aServiceId, TUint32 aContentId);

        /**
         * Delete item.
         * @param aGlobalFileId
         * @return System-wide error code.
         */
        IMPORT_C TInt Delete(CIptvMyVideosGlobalFileId& aGlobalFileId);
        
        /**
         * Delete item.
         * @param aServiceId
         * @param aContentId
         * @return System-wide error code.
         */                
        IMPORT_C TInt Delete(TUint32 aServiceId, TUint32 aContentId);
        
    private:

        /**
        * C++ default constructor.
        */
        CIptvVodDlDownloadList();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
            
        /**
        * Contains download items.
        */
        RArray<CIptvVodDlDownloadListItem*> iList;
            
    };

#endif      // CIPTVVODDLDOWNLOADLIST_H   
            
// End of File
