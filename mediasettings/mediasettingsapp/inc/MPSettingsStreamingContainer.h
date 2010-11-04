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
* Description:   Container class for Streaming settings list view.*
*/


// Version : %version: 3 %



#ifndef MPSETTINGSSTREAMINGCONTAINER_H
#define MPSETTINGSSTREAMINGCONTAINER_H

// INCLUDES
#include    "MPSettingsBaseContainer.h"
 
// FORWARD DECLARATIONS
class CEikTextListBox;
class CMPSettingsModelForROP;
class CMPSettingsStreamingSettingItemList;

// CLASS DECLARATION
/**
*  CMPSettingsStreamingContainer  container control class.
*  
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsStreamingContainer : public CMPSettingsBaseContainer
    {
    public: // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMPSettingsStreamingContainer(CMPSettingsModelForROP* aModel);

        /**
        * Destructor.
        */
        virtual ~CMPSettingsStreamingContainer();

    public: // Functions from base classes
            
        /**
        * From CoeControl
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
	    void FocusChanged(TDrawNow /*aDrawNow*/);
   
        /**
        * Edits current setting item.
        * 
        * @param aMenuOption
        */
        void EditCurrentItemFromMenuL(TBool aMenuOption);
	
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
        CMPSettingsModelForROP* iModel;
	
		/**
		 * Setting item list.
	     */
        CMPSettingsStreamingSettingItemList* iSettingList;
    
    };

#endif

// End of File
