/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares Application for LWPlayer.
 *
*/

// Version : %version: 3 %



#ifndef _LWPLAYERAPP_H_
#define _LWPLAYERAPP_H_

#include <aknapp.h>

/*
 * CLWPlayerApp application class.
 * Provides factory to create concrete document object.
 * 
 */

class CLWPlayerApp : public CAknApplication
{
    private:
        /*
         * From CApaApplication, creates CLWPlayerDocument document object.
         * @return A pointer to the created document object.
         */
        CApaDocument* CreateDocumentL();

        /*
         * From CApaApplication, returns application's UID (KUidLWPlayer).
         * @return The value of KUidLWPlayer.
         */
        TUid AppDllUid() const;
};

#endif // _LWPLAYERAPP_H_

// End of File
