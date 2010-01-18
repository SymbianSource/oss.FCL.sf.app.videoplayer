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
* Description:    Identifies a download, stream object.*
*/





#ifndef TIPTVVODDLDOWNLOADID_H
#define TIPTVVODDLDOWNLOADID_H

//  INCLUDES
#include "MIptvStreamObject.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Identifies a VOD download.
 *
 * @lib IptvClientApi.lib
 */
class TIptvVodDlDownloadId : public MIptvStreamObject
    {

    public: // Functions from base classes

        /**
        * From MIptvStreamObject
        * @param aStream stream to read data from.
        */
        IMPORT_C void InternalizeL(RReadStream& aStream);

        /**
        * From MIptvStreamObject
        * @param aStream stream to write data to.
        */
        IMPORT_C void ExternalizeL(RWriteStream& aStream);

        /**
        * From MIptvStreamObject
        * @return returns the size of externalization.
        */
        IMPORT_C TUint32 CountExternalizeSize();
        
        /**
        * Copies values from aDownloadId to this object.
        * @param aDownloadId 
        */
        IMPORT_C void Set(TIptvVodDlDownloadId& aDownloadId);
        
    public:     // Data
    
        /**
        * ETrue if iServiceId and ContentId are valid.
        */
        TBool iServiceIdAndContentIdAreValid;
        
        /**
        * Service ID in Service Manager database.
        */
        TUint32 iServiceId;
        
        /**
        * Content ID in EPG database.
        */
        TUint32 iContentId;
        
        /**
        * ETrue if iFileId and iDriveId are valid.
        */
        TBool iFileIdAndDriveAreValid;
        
        /**
        * Identifies file together with iDriveId in My Videos database.
        */
        TUint32 iFileId;
        
        /**
        * Identifies file together with iFileId in My Videos database.
        */
        TUint32 iDrive;
    };

#endif      // TIPTVVODDLDOWNLOADID_H   
            
// End of File
