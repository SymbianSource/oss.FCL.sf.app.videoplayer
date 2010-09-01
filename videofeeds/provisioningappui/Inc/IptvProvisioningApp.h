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




#ifndef CIPTVPROVISIONINGAPP_H
#define CIPTVPROVISIONINGAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
const TUid KUidIptvProvisioningAppUid = { 0x10281F1A };

// CLASS DECLARATION
/**
*	Application class definition
*/
class CIptvProvisioningApp : public CAknApplication
    {
    private: // from CApaApplication
        /**
        * Create CIptvProvisioningDocument document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * Return CIptvProvisioningApp uid.
        */
        TUid AppDllUid() const;
    };

#endif // CIPTVPROVISIONINGAPP_H
            
// End of File

