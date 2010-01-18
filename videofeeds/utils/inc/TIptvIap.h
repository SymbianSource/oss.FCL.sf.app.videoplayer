/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    header file for TIptvIap class*
*/





#ifndef TIPTVIAP_H
#define TIPTVIAP_H

#include <s32mem.h>

// CLASS DECLARATION
/**
* Container class for storing IAP ID and priority.
*/
class TIptvIap
    {
    public:
    
        /**
        * Constructor, clears member variables to zero.
        */
        IMPORT_C TIptvIap();

        /**
        * Destructor.
        */
        IMPORT_C ~TIptvIap();
        
        /**
         * Externalizes this object to aStream.
         * @param aStream
         */
        IMPORT_C void ExternalizeL(RDesWriteStream& aStream) const;
        
        /**
         * Internalizes this object from aStream.
         * @param aStream
         */
        IMPORT_C void InternalizeL(RDesReadStream& aStream);

        /**
         * Counts the memory block needed for the externalize call.
         * @return Memory size (in bytes) needed for the externalize.
         */
        IMPORT_C TUint32 CountExternalizeSize();
    
    public:
    
        /**
        * Used to indicate that iId and iPriority have valid values. This variable is not serialized
        * in ExternalizeL or InternalizeL.
        */
        TBool   iValueExists;

        /**
        * IAP ID in CommsDb.
        */
        TUint32 iId;

        /**
        * Priority, small number indicates high priority, 0 being the highest.
        */        
        TUint16 iPriority;
    };

#endif
