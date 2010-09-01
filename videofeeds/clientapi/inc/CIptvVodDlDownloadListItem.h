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





#ifndef CIPTVVODDLDOWNLOADLISTITEM_H
#define CIPTVVODDLDOWNLOADLISTITEM_H

//  INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;
class CIptvMyVideosGlobalFileId;

// CLASS DECLARATION

/**
*  Used with CIptvVodDlDownloadList. Contains information about a single download in VOD DL manager.
*
*  @lib IptvClientApi.lib
*/
class CIptvVodDlDownloadListItem : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvVodDlDownloadListItem* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvVodDlDownloadListItem();

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
        * @param aStream stream to read data from
        */
        IMPORT_C TUint32 CountExternalizeSize();
        

    private:

        /**
        * C++ default constructor.
        */
        CIptvVodDlDownloadListItem();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data

        /**
        * Global file ID in My Videos database.
        */    
        CIptvMyVideosGlobalFileId* iGlobalFileId;

        /**
        * If ETrue, iGlobalFileId won't change anymore.
        */
        TBool                      iGlobalFileIdIsValid;

        /**
        * Identifies the service in the Service database.
        */    
        TUint32                    iServiceId;
        
        /**
        * Identifies content in EPG database.
        */    
        TUint32                    iContentId;

        /**
        * Content index in EPG database.
        */    
        TUint32                    iContentIndex;
        
        /**
        * Identifies accesspoint in CommsDB.
        */    
        TUint32                    iIapId;
        
        /**
        * Download type. See possible values from CIptvUtil.h.
        */    
        TIptvDownloadType          iDlType;
        
        /**
        * Download progress in percentages.
        */    
        TUint32                    iProgress;
        
        /**
        * Download state, see possible states from CIptvUtil.h
        */    
        TUint32                    iState;
        
        /**
        * In case of error (iState = KIptvVodDlDownloadFailed) contains error code.
        * See possible codes from CIptvUtil.h.
        */    
        TIptvDlError               iError;        

    };

#endif      // CIPTVVODDLDOWNLOADLISTITEM_H   
            
// End of File
