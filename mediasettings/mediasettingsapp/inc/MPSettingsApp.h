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
* Description:   Declares main application class.*
*/


// Version : %version: 2 %



#ifndef MPSETTINGSAPP_H
#define MPSETTINGSAPP_H

// INCLUDES
#include    <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidMediaSettings = {0x10005A3F};

// CLASS DECLARATION

/**
* CMPSettingsApp application class.
* Provides factory to create concrete document object.
* 
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsApp : public CAknApplication
    {
    private:

        /**
        * From CApaApplication, creates CMPSettingsDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidMediaSettings).
        * @return The value of KUidMediaSettings.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

