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
* Description:    Contains CIptvEvent objects*
*/





#ifndef CIPTVEVENTS_H
#define CIPTVEVENTS_H

//  INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"
#include "MIptvStreamObject.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class MIptvEvent;

// CLASS DECLARATION

/**
 * MIptvStreamObject derived class, contains group of MIptvEvent objects.
 * This class is used to convey events over IPC.
 */
class CIptvEvents : public CBase, public MIptvStreamObject
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aEventProto this is used to construct new event objects.
        */
        IMPORT_C static CIptvEvents* NewL(MIptvEvent* aEventProto);
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvEvents();
        
    public:

        //from MIptvStreamObject
        IMPORT_C void InternalizeL(RReadStream& aStream);
        IMPORT_C void ExternalizeL(RWriteStream& aStream);
        IMPORT_C TUint32 CountExternalizeSize();
        
        /**
        * Copies data from aEvents to this object.
        *
        * @param aEvents Data to be copied from.
        */
        IMPORT_C void SetL(CIptvEvents& aEvents);
        
        /**
        * Deletes all events from this object.
        */ 
        IMPORT_C void DeleteEvents();
 
    private:

        /**
        * C++ default constructor.
        */
        CIptvEvents(MIptvEvent* aEventProto);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:
    
        /**
        * Array of pointers to events.
        */
        RArray<MIptvEvent*> iEvents;
        
    private:
        /**
        * Pointer to event object which is used to get function implementations for
        * MIptvEvent interface methods. This defines the used event type. Owned.
        */
        MIptvEvent* iEventProto;    

    };

#endif      // CIPTVEVENTS_H   
            
// End of File
