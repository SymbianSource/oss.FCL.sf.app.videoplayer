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


// Version : %version: 3 %



#ifndef MPSETTINGSVIDEOCONTAINER_H
#define MPSETTINGSVIDEOCONTAINER_H

// INCLUDES
#include    "videoplayersettingsengine.h"
#include    "MPSettingsBaseContainer.h"

// FORWARD DECLARATIONS
class CAknSettingItemList;
class CMPSettingsModelForROP;   
class CVcxNsSettingsEngine;

// CLASS DECLARATION

/**
*  CMPSettingsVideoContainer  container control class.
*  
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsVideoContainer : public CMPSettingsBaseContainer
    {
    public: // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMPSettingsVideoContainer(CMPSettingsModelForROP* aModel);

        /**
        * Destructor.
        */
        virtual ~CMPSettingsVideoContainer();

    public: // New

        /**
        * Edits current setting item.
        * @since 2.0
        */
        void EditCurrentItemFromMenuL(TBool iMenuOption);
        
        /**
        * Check if specified item is currently selected.
		*
		* @param aIndex		Item index	
        */
        TBool IsCurrentItem( TInt aIndex );

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

    private:    // Functions from base classes

        /**
        * From CMPSettingsBaseContainer
        */
        TInt TitleResourceId();

    private:
        CAknSettingItemList* iSettingList;
		CMPSettingsModelForROP* iModel; 
		CVcxNsSettingsEngine* iVideoSettingsEngine;
    };

#endif

// End of File
