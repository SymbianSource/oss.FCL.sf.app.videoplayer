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




#ifndef CIPTVMYVIDEOSGLOBALFILEID_H
#define CIPTVMYVIDEOSGLOBALFILEID_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include "MIptvStreamObject.h"

class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
* Defines data class structure for the API.
*
* @lib IptvClientApi.dll
*/
class CIptvMyVideosGlobalFileId : public CBase, public MIptvStreamObject
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosGlobalFileId object.
         */
        IMPORT_C static CIptvMyVideosGlobalFileId* NewL();

        /**
         * Two-phased constructor.
         *
         * @return New CIptvMyVideosGlobalFileId object.         
         */
        IMPORT_C static CIptvMyVideosGlobalFileId* NewLC();

        /**
         * Destructor.
         */
        virtual ~CIptvMyVideosGlobalFileId();

        /**
         * Externalize.
         *
         * @param aStream Stream to externalize object to.         
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream);

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
        * Drive specific file ID identifier.
        */
        TIptvFileId iFileId; 

    protected:

        /**
        * C++ default constructor.
        */
        CIptvMyVideosGlobalFileId();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
    };

#endif // CIPTVMYVIDEOSGLOBALFILEID_H

// End of File
