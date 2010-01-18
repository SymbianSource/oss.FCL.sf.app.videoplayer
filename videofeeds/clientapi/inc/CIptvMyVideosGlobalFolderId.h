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
* Description:    Defines data class structure for the API*
*/




#ifndef CIPTVMYVIDEOSGLOBALFOLDERID_H
#define CIPTVMYVIDEOSGLOBALFOLDERID_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"

class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
* Defines data class structure for the API.
*
* @lib IptvClientApi.dll
*/
class CIptvMyVideosGlobalFolderId : public CBase
    {
    public:
    
        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosGlobalFolderId object.
         */
        IMPORT_C static CIptvMyVideosGlobalFolderId* NewL();

        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosGlobalFolderId object.
         */
        IMPORT_C static CIptvMyVideosGlobalFolderId* NewLC();

        /**
         * Get object representing any of the root folders. These objects
         * are needed when copying/moving folders/files to root of some
         * drive. Method performs also basic functionality of NewL().
         *
         * @param aDrive Requested drive for root folder. Possible enumeration
         *               values are EDriveC and EDriveE. Leave empty or use -1
         *               to point to root of all drives (when for example just
         *               listing all folders and files).
         * @return New CIptvMyVideosGlobalFolderId object.
         */
        IMPORT_C static CIptvMyVideosGlobalFolderId* NewRootFolderL(TInt aDrive = -1);

        /**
         * Destructor.
         */
        virtual ~CIptvMyVideosGlobalFolderId();

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

    public: // Data.

        /**        
         * See TDriveNumber, EDriveC and EDriveE allowed.
         */
        TInt iDrive;  

        /**        
         * Drive specific folder ID identifier.
         */
        TIptvFolderId iFolderId; 

    protected:

        /**
         * C++ default constructor.
         */
        CIptvMyVideosGlobalFolderId();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();
    };

#endif // CIPTVMYVIDEOSGLOBALFOLDERID_H

// End of File
