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
* Description:   Common interface for ECOM varied plug-in views.*
*/


// Version : %version: 3 %




#ifndef CMPSETTINGSPLUGINVIEW_H
#define CMPSETTINGSPLUGINVIEW_H

// INCLUDES
#include    <e32std.h>
#include    <e32base.h>
#include    <implementationinformation.h>

#include    "MPSettingsBaseView.h"

// CLASS DECLARATION

/**
*  CMPSettingsPluginView
*  Common API for ECOM varied plug-in views.
*
*  @lib MPSettBase.dll
*  @since 2.0
*/
class CMPSettingsPluginView : public CMPSettingsBaseView
    {
    public: // Constructors and destructor

       /**
        * Constructor.
        */
        IMPORT_C static CMPSettingsPluginView* NewL(TUid aUid);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMPSettingsPluginView();

    public: // New

        /**
        * Provides an implementation information array.
        * @since 2.0
        * @param aImplInfoArray Array which is populated with the implementation
        *   information of the available implementations.
        */
        IMPORT_C static void ListImplementationsL(RImplInfoPtrArray& aImplInfoArray);

        /**
        * Returns ETrue if the view should be opened from Main view's list.
        * @return ETrue:  The view appears in Main view's list.
        *         EFalse: The view does not appear in Main view's list (case: subview).
        */
        virtual TBool OpenedFromMainView() = 0;

    protected:
        TUid iDtor_ID_Key;
    };

#endif      // CMPSETTINGSPLUGINVIEW_H   
            
// End of File
