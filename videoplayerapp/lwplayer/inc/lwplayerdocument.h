/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares document for LWPLayer application
 *
*/

// Version : %version: 3 %



#ifndef _LWPLAYERDOCUMENT_H_
#define _LWPLAYERDOCUMENT_H_

#include <GenericParamConsumer.h>

//
//  FORWARD DECLARATIONS
//
class CEikAppUi;
class CApaWindowGroupName;

/*
 *  CLWPlayerDocument application class.
 */

class CLWPlayerDocument : public CAiwGenericParamConsumer
{
    public:
        /*
         * Two-phased constructor.
         */
        static CLWPlayerDocument* NewL(CEikApplication& aApp);

        /*
         * Destructor.
         */
        virtual ~CLWPlayerDocument();

    private:
        /*
         * EPOC default constructor.
         */
        CLWPlayerDocument(CEikApplication& aApp);

        /*
         * Two-phased constructor.
         */
        void ConstructL();

        /*
         * From CEikDocument, create CLWPlayerAppUi "App UI" object.
         */
        CEikAppUi* CreateAppUiL();

        /*
         * From CEikDocument, used to hide icon from task list
         */
        void UpdateTaskNameL(CApaWindowGroupName* aWgName);
};

#endif

// End of File
