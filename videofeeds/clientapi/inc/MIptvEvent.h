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
* Description:    Event object interface.*
*/





#ifndef CIPTVEVENT_H
#define CIPTVEVENT_H

//  INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"
#include "MIptvStreamObject.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class RDesReadStream;
class RDesWriteStream;

/**
* Interface class which defines common functions for event objects.
* This class is used when sending events from CIptvEventGenerator to CIptvEventListener.
*/
class MIptvEvent : public MIptvStreamObject
    {        
    public: // New functions
        
        //From MIptvStreamObject
        virtual void InternalizeL(RReadStream& aStream) = 0;
        virtual void ExternalizeL(RWriteStream& aStream) = 0;
        virtual TUint32 CountExternalizeSize() = 0;

        /**
        * Constructs copy of this object and returns pointer to it.
        * Ownership moves to caller.
        */
        virtual MIptvEvent* GetCopyL() = 0;

        /**
        * CIptvEvents needs to destruct MIptvEvent type class
        */
        virtual void Destruct() = 0;
        
    };

#endif      // CIPTVEVENT_H   
            
// End of File
