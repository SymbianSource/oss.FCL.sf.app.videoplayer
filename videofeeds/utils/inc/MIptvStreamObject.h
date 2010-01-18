/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __MIPTVSTREAMOBJECT_H__
#define __MIPTVSTREAMOBJECT_H__

#include <e32base.h>
#include <s32mem.h>

class RReadStream;
class RWriteStream;

// CLASS DECLARATION

/**
* This class specifies the interface for iptv stream object.
*
* @lib IptvUtil.lib
*/
class MIptvStreamObject
    {
        
    public: // New functions

        /**
        * Serializes this object from aStream.
        *
        * @param aStream 
        */
        virtual void InternalizeL(RReadStream& aStream) = 0;  

        /**
        * Serializes this object to aStream.
        *
        * @param aStream
        */
        virtual void ExternalizeL(RWriteStream& aStream) = 0;  

        /**
        * Returns the size required for externalization buffer in 8-bit
        * bytes.
        */
        virtual TUint32 CountExternalizeSize() = 0;

    public:
        /**
        * Can be used to identify the stream object.
        * This variable is not serialized in InternalizeL and ExternalizeL.
        */
        TInt iObjId;  
    };

#endif
