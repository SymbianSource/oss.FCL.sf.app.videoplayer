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




#ifndef CIPTVMYVIDEOSFOLDER_H
#define CIPTVMYVIDEOSFOLDER_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include "CIptvMyVideosGlobalFolderId.h"

class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
* Defines data class structure for the API.
*
* @lib IptvClientApi.dll
*/
class CIptvMyVideosFolder : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosFolder object.
         */
        IMPORT_C static CIptvMyVideosFolder* NewL();

        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosFolder object.
         */
        IMPORT_C static CIptvMyVideosFolder* NewLC();
    
        /**
         * Destructor.
         */
        virtual ~CIptvMyVideosFolder();

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
         * Global folder ID identifier.
         */
        CIptvMyVideosGlobalFolderId* iFolderId;

        /**        
         * Folder name.
         */
        TBuf<KIptvMyVideosFolderNameMaxLength> iName;

    protected:
           
        /**
         * C++ default constructor.
         */
        CIptvMyVideosFolder();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();
    };

#endif // CIPTVMYVIDEOSFOLDER_H

// End of File
