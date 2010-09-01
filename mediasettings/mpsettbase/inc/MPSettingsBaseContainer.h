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
* Description:   Base class for all Media Settings containers.*
*/


// Version : %version: 2 %



#ifndef MPSETTINGSBASECONTAINER_H
#define MPSETTINGSBASECONTAINER_H

// INCLUDES
#include    <coeccntx.h>
#include    <eikclb.h>

// CLASS DECLARATION

/**
*  CMPSettingsBaseContainer container class
*
*  @lib MPSettBase.dll
*  @since 2.0
*/
class CMPSettingsBaseContainer : public CCoeControl
{
    public: // constructors and destructor

        /*
        * C++ constructor.
        */
        IMPORT_C CMPSettingsBaseContainer();
      
        /**
        * Constructs all common parts of the container.
        * @param aRect Listbox control's rect.
        */
        IMPORT_C void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMPSettingsBaseContainer();

    public:  // Functions from base classes

        /**
        * From CCoeControl.
        */
        IMPORT_C void SizeChanged();

        /**
        * From CCoeControl 
        */
        IMPORT_C void HandleResourceChange(TInt aType);
        
		/**
        * From CCoeControl.
        */
        IMPORT_C TInt CountComponentControls() const;

        /**
        * From CCoeControl.
        */
        IMPORT_C CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl.
        */
        IMPORT_C TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                             TEventCode aType);

    public:   // New
        /**
        * Provides access to listbox control.
        * @since 2.0
        * @return pointer to listbox control.
        */
        IMPORT_C virtual CEikTextListBox* ListBox();

        /**
        * Sets selected item in the listbox.
        * @param aIndex Index of the item.
        */
        IMPORT_C void SetSelectedItem( TInt aIndex );


    protected:  // New functions
        /**
        * Constructs component controls owned by the child class.
        *   Pure virtual.
        * @since 2.0
        */
        virtual void ConstructComponentControlsL() = 0;

    private:  // New functions
        /**
        * Returns title resource id.
        * @since 2.0
        * @return title resource id.
        */
        virtual TInt TitleResourceId() = 0;

    protected: // Data
        CCoeControl* iComponentControl; // Not owned
        CEikTextListBox* iListBox; // Not owned
};

#endif //MPSETTINGSBASECONTAINER_H

// End of File
