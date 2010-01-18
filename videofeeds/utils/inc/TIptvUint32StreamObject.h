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
* Description:    This class was defined to be able to use RIptvClientSession.cpp*
*/





#ifndef TIPTVUINT32STREAMOBJECT_H
#define TIPTVUINT32STREAMOBJECT_H

//  INCLUDES
#include "MIptvStreamObject.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class RWriteStream;
class RReadStream;

/**
*  @lib IptvUtil.lib
*/
class TIptvUint32StreamObject : public MIptvStreamObject
    {
        
    public: // Functions from base classes

        IMPORT_C TIptvUint32StreamObject();
        IMPORT_C ~TIptvUint32StreamObject();

        /**
        * From MIptvStreamObject
        * @param aStream Stream to write.
        */
        IMPORT_C void ExternalizeL(RWriteStream& aStream);

        /**
        * From MIptvStreamObject
        * @param aStream Stream to read from.
        */
        IMPORT_C void InternalizeL(RReadStream& aStream);

        /**
        * From MIptvStreamObject
        */
        IMPORT_C TUint32 CountExternalizeSize();
        
    public:     // Data
        TUint32 iData;
    };

#endif      // TIPTVUINT32STREAMOBJECT_H   
            
// End of File
