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



#ifndef MPSETTINGSADVANCEDBWCONTAINER_H
#define MPSETTINGSADVANCEDBWCONTAINER_H

// INCLUDES
#include    "MPSettingsBaseContainer.h"
   
// FORWARD DECLARATIONS
class CMPSettingsAdvancedBwSettingItemList;
class CMPSettingsModelForROP;   

// CLASS DECLARATION

/**
*  CMPSettingsAdvancedBwContainer container control class.
*  
*  @lib MediaSettings.app
*  @since 2.1
*/
class CMPSettingsAdvancedBwContainer : public CMPSettingsBaseContainer
    {
    public: // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMPSettingsAdvancedBwContainer(CMPSettingsModelForROP* aModel);

        /**
        * Destructor.
        */
        virtual ~CMPSettingsAdvancedBwContainer();

    public: // New

        /**
        * Edits current setting item.
        * @since 2.1
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
        CMPSettingsAdvancedBwSettingItemList* iSettingList;
        CMPSettingsModelForROP* iModel;
      
    };

#endif // MPSETTINGSADVANCEDBWCONTAINER_H

// End of File
