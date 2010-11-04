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
* Description:   Container class for Main view.*
*/


// Version : %version: 3 %




#ifndef CMPSETTINGSMAINCONTAINER_H
#define CMPSETTINGSMAINCONTAINER_H

// INCLUDES
#include    "MPSettingsBaseContainer.h"
   
// FORWARD DECLARATIONS
class CEikTextListBox;
class CAknIconArray;

// CLASS DECLARATION

/**
*  CMPSettingsMainContainer container control class.
*
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsMainContainer : public CMPSettingsBaseContainer
    {
    public: // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMPSettingsMainContainer();

        /**
        * Destructor.
        */
        virtual ~CMPSettingsMainContainer();

    private:    // Functions from base classes
            
        /**
        * From CoeControl
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
	void FocusChanged(TDrawNow /*aDrawNow*/);

        /**
        * From CMPSettingsBaseContainer
        */
        void ConstructComponentControlsL();

        /**
        * From CMPSettingsBaseContainer
        */
        TInt TitleResourceId();

        /**
        * From CoeControl
        */
        void HandleResourceChange( TInt aType );

    private: // New functions

        /**
        * Contructs icon array for listbox
        */
        void MakeIconArrayL();

        /**
        * Load a possibly skinned icon (with mask) and append it to an 
        * icon array.
        * @since 2.6
        * @param aArray    Icon array to add the icon to
        * @param aID       Skin ID for the icon
        * @param aMbmFile  Full path to MBM file which contains the default icon
        * @param aBitmapId Id for the default icon in the MBM file
        * @param aMaskId   Id for the default icon mask in the MBM file
        */
        static void AppendIconToArrayL(
            CAknIconArray* aArray,
            const TAknsItemID& aID,
            const TDesC& aMbmFile,
            TInt aBitmapId,
            TInt aMaskId );
        
        /**
        * Handles leave in HandleResourceChange method 
        *
        * @param aError     Error code
        */
        void HandleResourceChangeErrorL( TInt aError );
        
    };
        
#endif      // CMPSETTINGSMAINCONTAINER_H   
            
// End of File


