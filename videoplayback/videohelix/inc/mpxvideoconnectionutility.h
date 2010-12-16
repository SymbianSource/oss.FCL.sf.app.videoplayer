/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Access Point handling logic for streaming use cases.
*
*/


#ifndef _MPXVIDEOCONNECTIONUTILITY_H
#define _MPXVIDEOCONNECTIONUTILITY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
NONSHARABLE_CLASS( MpxVideoConnectionUtility )
{  
    public: // New functions
        
        /**
         * Check if accesspoint given is packet data (gprs, egprs) type
         * 
         * @return Access point id to be used for streaming or KUseDefaultIap if 
         * access point from MediaSettings should be used. Returns given access point id
         * if no change is needed.
         */
        static TInt CheckAccessPointL(TInt aApId);
        
        /**
        * Try to start default connection.
        * Does not report error if connection start fails
        * @return void
        */
        static void StartDefaultConnectionL();
         
};

#endif      _MPXVIDEOCONNECTIONUTILITY_H
            
// End of File
