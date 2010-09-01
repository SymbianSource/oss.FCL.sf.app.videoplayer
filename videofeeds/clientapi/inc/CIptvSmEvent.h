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
* Description:    Service Manager event object*
*/





#ifndef CIPTVSMEVENT_H
#define CIPTVSMEVENT_H

//  INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"
#include "MIptvStreamObject.h"

#include "MIptvEvent.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class RDesReadStream;
class RDesWriteStream;
class CIptvService;

/**
*  Used for sending events from Service Manager (on server side) to client.
*/
class CIptvSmEvent : public CBase, public MIptvEvent
    {
    public:
    
        /**
        * Events, used in iEvent member variable.
        */
        enum TEvent
            {                
            /**
            * This is sent when a service is added to the database.
            */                
            EServiceAdded,

            /**
            * This is sent when the service address has been modified.
            */
            EServiceModified,

            /**
            * This is sent when the scheduled download settings have been changed.
            */
            EServiceScheduleModified,

            /**
            * This is sent when the service selection has been modified.
            */
            EServiceSelectionModified,

            /**
            * This is sent when the service is deleted from the database.
            */
            EServiceDeleted
            
            };
            
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvSmEvent* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~CIptvSmEvent();

    public: // New functions
        
        //from MIptvEvent
        IMPORT_C void InternalizeL(RReadStream& aStream);
        IMPORT_C void ExternalizeL(RWriteStream& aStream);
        IMPORT_C TUint32 CountExternalizeSize();
        IMPORT_C MIptvEvent* GetCopyL();
        IMPORT_C void Destruct();
        
    private:

        /**
        * C++ default constructor.
        */
        CIptvSmEvent();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:
    
        /**
        * Identifies the event.
        */
        TEvent  iEvent;
        
        /**
        * Identifies the service which caused the event.
        */
        TUint32 iServiceId;

        /**
        * The service which was added, changed or deleted. This is never
        * serialized and is valid only when the event is being sent. Not own.
        */
        CIptvService* iService;
    };

#endif      // CIPTVSMEVENT_H   
            
// End of File
