/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    ?Description*
*/





#ifndef MSERVICEMANAGEROBSERVER_H
#define MSERVICEMANAGEROBSERVER_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CIptvSmEvent;

/**
*  Observer class for CIptvServiceManager.
*/
class MIptvServiceManagerObserver
    {
    
    public: // New functions
        
        /**
         * CIptvServiceManager calls this to inform about events.
         *
         * @param aEvent Event sent by Service Manager.
         */
        virtual void HandleSmEvent(CIptvSmEvent& aEvent) = 0;

    public: // Functions from base classes

    };

#endif      // MSERVICEMANAGEROBSERVER_H   
            
// End of File
