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
* Description:    Contains CIptvVodDlDownloadEvent objects*
*/





#ifndef CIPTVVODDLDOWNLOADEVENTS_H
#define CIPTVVODDLDOWNLOADEVENTS_H

//  INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"
#include "MIptvStreamObject.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvVodDlDownloadEvent;

// CLASS DECLARATION

/**
*  Used for handling groups of CIptvVodDlDownloadEvent objects.
*
*  @lib IptvClientApi.lib
*/
class CIptvVodDlDownloadEvents : public CBase, public MIptvStreamObject
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvVodDlDownloadEvents* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvVodDlDownloadEvents();

    public: // New functions
        

    public: // Functions from base classes

        IMPORT_C void InternalizeL(RReadStream& aStream);
        IMPORT_C void ExternalizeL(RWriteStream& aStream);
        IMPORT_C TUint32 CountExternalizeSize();
        IMPORT_C void SetL(CIptvVodDlDownloadEvents& aEvents);
        IMPORT_C void DeleteEvents();
 
    private:

        /**
        * C++ default constructor.
        */
        CIptvVodDlDownloadEvents();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
    
        /**
        * Array of pointers to CIptvVodDlDownloadEvent objects.
        */
        RArray<CIptvVodDlDownloadEvent*> iEvents;    

    };

#endif      // CIPTVVODDLDOWNLOADEVENTS_H   
            
// End of File
