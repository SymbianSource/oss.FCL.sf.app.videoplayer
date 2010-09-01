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





#ifndef TIPTVGETUSEDIAPREQ_H
#define TIPTVGETUSEDIAPREQ_H

//  INCLUDES
#include "CIptvUtil.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MIptvNetworkSelectionObserver;

// CLASS DECLARATION

/**
 * Container for GetUsedIapReqL request.
 * See header for CIptvNetworkSelection::GetUsedIapReqL -method for more information.
 */
class TIptvGetUsedIapReq
    {

    public:     // Data

        /**
         * Client fills.
         */
        MIptvNetworkSelectionObserver* iNsObserver;
        
        /**
         * Client fills.
         */
        TUint32                        iServiceId;

        /**
         * CIptvNetworkSelection fills.
         */
        TUint32                       iIapId;

        /**
         * CIptvNetworkSelection fills.
         */
        TBuf<KIptvNsIapNameMaxLength> iIapName;

        /**
         * CIptvNetworkSelection fills.
         */
        TUint8                        iConnectionPermission;
        
        /**
        * CIptvNetworkSelection fills.
        */
        TUint8                           iWLANWhenGPRS;

        /**
         * CIptvNetworkSelection fills.
         */
        TUint8                        iRespStatus;
                    
    };

#endif      // TIPTVGETUSEDIAPREQ_H   
