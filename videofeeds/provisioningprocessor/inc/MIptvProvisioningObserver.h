/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef MIPTVPROVISIONINGOBSERVER_H
#define MIPTVPROVISIONINGOBSERVER_H

// INCLUDES

// FORWARD DECLARATIONS

// CLASS DECLARATION

const TInt KIptvProvisioningTooManyServicesDefinedInXml = 0;
const TInt KIptvProvisioningSucceeded = 1;
const TInt KIptvProvisioningFileNotFound = 2;

class MIptvProvisioningObserver
    {
    public:
      
        /**                
         * Notification from provisioning processor. 
         *
         * @param aNotification Message from provisioning processor.         
         */
        virtual void Notify(TInt aNotification) = 0;
    };

#endif  // MIPTVPROVISIONINGOBSERVER_H

// End of File
