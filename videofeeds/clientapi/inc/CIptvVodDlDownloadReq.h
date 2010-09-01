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
* Description:    Data object sent in download request.*
*/




#ifndef CIPTVVODDLDOWNLOADREQ_H
#define CIPTVVODDLDOWNLOADREQ_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include <s32mem.h>
#include "MIptvStreamObject.h"

// CLASS DECLARATION
/**
 * Used for IPC communication. Encapsulates download request parameters.
 */
class CIptvVodDlDownloadReq : public CBase, public MIptvStreamObject
    {
    public:
        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvVodDlDownloadReq* NewL();

        /**
         * Read data and construct this item from it.
         * @param 
         */
        IMPORT_C void InternalizeL(RReadStream& aStream);


        /**
         * Write internal data to binary format which can be sent over IPC.
         * @param 
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream);

        /**
         * Counts size of the externalize result. User can call this to find out
         * how much data will be received from ExternalizeL.
         */
        IMPORT_C TUint32 CountExternalizeSize();

        /**
         * Destructor.
         */
        ~CIptvVodDlDownloadReq();
                                
    private: // New functions           
        /**
        * C++ default constructor.
        */
        CIptvVodDlDownloadReq();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();
                  
    public:     // Data members

        /**        
        * Service provider ID identifier.
        */
        TIptvServiceId    iServiceId; 
        
        /**        
        *  Content ID.
        */
        TIptvContentId    iContentId;

        TUint32           iIapId;
        TUint32           iIndex;

        /**        
        * Download type flags: EDlTypeStream|Immediate.. 
        */
        TIptvDownloadType iDlType;   

    };

#endif  // CIPTVVODDLDOWNLOADREQ.H

// End of File
