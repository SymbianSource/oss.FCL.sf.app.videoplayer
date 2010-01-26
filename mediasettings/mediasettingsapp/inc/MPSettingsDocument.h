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
* Description:   Document class required by AVKON application framework.*
*/


// Version : %version: 5 %



#ifndef MPSETTINGSDOCUMENT_H
#define MPSETTINGSDOCUMENT_H

// INCLUDES
#include    <AknDoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;
class CMPSettingsModel;
class CMPMediaPlayerSettings;

// CLASS DECLARATION

/**
*  CMPSettingsDocument application class.
*
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CMPSettingsDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CMPSettingsDocument();

    public: // New functions

        /**
        * Returns a MMF controller specific settings model.
        * @since 2.0
        * @return MMF controller specific settings model.
        */
        CMPSettingsModel* SettingsModel();
 

    private:

        /**
        * C++ default constructor.
        */
        CMPSettingsDocument(CEikApplication& aApp);

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CMPSettingsAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();

    private: // Data
        CMPSettingsModel* iRopSettingsModel;
 
     };

#endif

// End of File

