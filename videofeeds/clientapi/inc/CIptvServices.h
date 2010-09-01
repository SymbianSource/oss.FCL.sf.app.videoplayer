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
* Description:    A class holding several CIptvService objects in a binary form.*
*/





#ifndef CIPTVSERVICES_H
#define CIPTVSERVICES_H

//  INCLUDES
#include <e32base.h>
#include <s32mem.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvService;

// CLASS DECLARATION

/**
* Holds several CIptvService objects in a binary form. This class is good for handling
* large number of CIptvService objects in a memory saving manner. Provides methods
* for adding a new service and referencing to them.
*/
class CIptvServices : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvServices* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvServices();

    public: // New functions
        
        /**
         * Creates CIptvService instance from aIndex position.
         * Notice that CIptvService object is constructed from binary data
         * and is somewhat CPU extensive. On the other hand, CIptvService
         * class uses lots of memory, so it's recommended not to
         * have too many of them loaded at the same time.
         * Caller is responsible for freeing the returned CIptvService pointer.
         *
         * @param aIndex Position in CIptvServices private services array.
         * @return       Pointer to CIptvService class, If service is not found, NULL is returned.
         */
        IMPORT_C CIptvService* GetServiceL(TUint32 aIndex);

        /**
         * Gets reference to CIptvService instance from aIndex position.
         * Notice that CIptvService object is constructed from binary data
         * and is somewhat CPU extensive. Memory allocation is not performed, so
         * this is ligher operation than "CIptvService* GetServiceL(TUint32 aIndex)"
         * -method. This method is good for quick browsing through services.
         * Changing this reference will not change CIptvServices binary data.
         *
         * @param aIndex Position in CIptvServices private services array.
         * @return       Reference to CIptvService class, if service is not found, leave occurs.
         */
        IMPORT_C CIptvService& GetServiceRefL(TUint32 aIndex);

        /**
         * Returns reference to CIptvService instance from aIndex position.
         * Notice that CIptvService object is constructed from binary data
         * and is somewhat CPU extensive. Memory allocation is not performed, so
         * this is ligher operation than "CIptvService* GetServiceL(TUint32 aIndex)"
         * -method. This method is good for quick browsing through services.
         * Changing this reference will not change CIptvServices binary data.
         *
         * @param aIndex Position in CIptvServices private services array.
         * @param aError Will contain KErrNone if service found and returned
         *               successfully. If != KErrNone, then return reference
         *               does not contain valid data.
         * @return       Reference to CIptvService class.
         */
        IMPORT_C CIptvService& CIptvServices::Service( TUint32 aIndex, TInt& aError );        
        
        /**
         * Adds a service.
         *
         * @param aService Service to add.
         * @return         System-wide error code.
         */        
        IMPORT_C TInt AddL(CIptvService& aService);

        /**
         * Service count hold by this object.
         *
         * @return Service count.
         */        
        IMPORT_C TInt Count();

        /**
         * Serializes this object from aStream.
         *
         * @param aStream Stream to read from.
         */        
        IMPORT_C void InternalizeL(RDesReadStream& aStream);

        /**
         * Serializes this object to aStream.
         *
         * @param aStream Stream to write to.
         */        
        IMPORT_C void ExternalizeL(RDesWriteStream& aStream) const;

        /**
         * Returns the size of the memoryblock needed for ExternalizeL call.
         *
         * @param Memory block size in bytes.
         */        
        IMPORT_C TUint32 CountExternalizeSize();
        
    private:

        /**
        * C++ default constructor.
        */
        CIptvServices();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
    
        /**
        * CIptvService classes in binary form.
        */
        RArray<HBufC8*> iServices;
        
        /**
        * Used for offering references to iServices array.
        */
        CIptvService*   iService;
         
    };

#endif      // CIPTVSERVICES_H
            
// End of File
