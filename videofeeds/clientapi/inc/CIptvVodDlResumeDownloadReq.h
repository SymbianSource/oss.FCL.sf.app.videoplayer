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





#ifndef CIPTVVODDLRESUMEDOWNLOADREQ_H
#define CIPTVVODDLRESUMEDOWNLOADREQ_H

//  INCLUDES
#include <e32base.h>
#include "MIptvStreamObject.h"
#include "CIptvUtil.h"
#include "TIptvVodDlDownloadId.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvMyVideosGlobalFileId;
class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
*
*  @lib IptvClientApi.lib
*/
class CIptvVodDlResumeDownloadReq : public CBase, public MIptvStreamObject
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvVodDlResumeDownloadReq* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvVodDlResumeDownloadReq();

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
        CIptvVodDlResumeDownloadReq();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
        TIptvVodDlDownloadId                      iDownloadId;
        TBool                                     iUseUserNameAndPassword;
        TBuf<KIptvSmPasswordsDbUserNameMaxLength> iUserName;
        TBuf<KIptvSmPasswordsDbPasswordMaxLength> iPassword;
        TUint32                                    iIapId;
        TBool                                     iUseIapId;        
    };

#endif      // CIPTVVODDLDOWNLOADLIST_H   
            
// End of File
