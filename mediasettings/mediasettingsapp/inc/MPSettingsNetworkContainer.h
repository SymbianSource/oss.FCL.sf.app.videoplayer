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
* Description:   Declares container control for application.*
*/


// Version : %version: 2 %



#ifndef MPSETTINGSNETWORKCONTAINER_H
#define MPSETTINGSNETWORKCONTAINER_H

// INCLUDES
#include    "MPSettingsBaseContainer.h"
   
// FORWARD DECLARATIONS
class CMPSettingsNetworkSettingItemList;
class CMPSettingsModelForROP;   

// CLASS DECLARATION

/**
*  CMPSettingsNetworkContainer container control class.
*  
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsNetworkContainer : public CMPSettingsBaseContainer
    {
    public: // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMPSettingsNetworkContainer(CMPSettingsModelForROP* aModel);

        /**
        * Destructor.
        */
        virtual ~CMPSettingsNetworkContainer();

    public: // New

        /**
        * Edits current setting item.
        * @since 2.0
        */
        void EditCurrentItemFromMenuL();

    public: // Functions from base classes
            
        /**
        * From CoeControl
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
		void FocusChanged(TDrawNow /*aDrawNow*/);
  		void SizeChanged();
  		void HandleResourceChange(TInt aType);
  
    protected: // Functions from base classes 

        /**
        * From CMPSettingsBaseContainer
        */
        void ConstructComponentControlsL();

    private:  // Functions from base classes

        /**
        * From CMPSettingsBaseContainer
        */
        TInt TitleResourceId();

    private:
        CMPSettingsNetworkSettingItemList* iSettingList;
		CMPSettingsModelForROP* iModel;      
    };

#endif // MPSETTINGSNETWORKCONTAINER_H

// End of File
