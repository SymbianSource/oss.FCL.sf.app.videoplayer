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
* Description:   Declares UI class for application.*
*/


// Version : %version: 5 %



#ifndef MPSETTINGSAPPUI_H
#define MPSETTINGSAPPUI_H

// INCLUDES
#include    <eikapp.h>
#include    <eikdoc.h>
#include    <e32std.h>
#include    <coeccntx.h>
#include    <aknViewAppUi.h>
#include    <akntabgrp.h>
#include    <aknnavide.h>
#include    <AknTabObserver.h>


#include    "MPSettingsNaviPaneController.h"

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CMPSettingsMainView;

// CLASS DECLARATION
class CMPSettingsModel;
class CMPMediaPlayerSettings;
class CMPSettingsModelForROP;

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
* 
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsAppUi : public CAknViewAppUi,
                         public MAknTabObserver,
                         public MMPSettingsNaviPaneController
    {
    public: // // Constructors and destructor

        /*
        * C++ constructor.
        */
        CMPSettingsAppUi();

        /**
        * Symbian OS 2nd phase constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        virtual ~CMPSettingsAppUi();
        
    public: // Functions from base classes

        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, takes care of resource handling.
        * @param aType resource to be handled
        */
        void HandleResourceChangeL( TInt aType );

        /**
        * From MAknTabObserver, takes care of tab handling.
        * @param aIndex tab to be handled
        */
        void TabChangedL(TInt aIndex); 

        /**
        * From MMPSettingsNaviPaneController, returns pointer
        * to MP's Navi pane object.
        * @return CAknNavigationControlContainer.
        */
        CAknNavigationControlContainer* NaviContainer() const;
    
        /**
        * From MMPSettingsNaviPaneController, returns pointer
        * to MP's tab group object.
        * @return CAknNavigationDecorator.
        */
        CAknNavigationDecorator* MPTabGroup() const;

        /**
        * From MMPSettingsNaviPaneController, updates tab group
        * according to active view.
        */
        void UpdateTabIndex();


    private:

        /**
        * From MEikMenuObserver, dynamically initialises menu pane.
        * @param aResourceId Resource ID identifying the menu pane
        *   to initialise.
        * @param aMenuPane Pointer to the menu pane.
        */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

            /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private: // new functions

        /**
        * Create tab to tabgroup with possibly skinned icon (with mask)
        * @param aID       Skin ID for the icon
        * @param aMbmFile  Full path to MBM file which contains the default icon
        * @param aBitmapId Id for the default icon in the MBM file
        * @param aMaskId   Id for the default icon mask in the MBM file
        * @param aTabId    Id for tab
        */
        void AddTabSettingsL(
            const TAknsItemID& aID,
            const TDesC& aMbmFile,
            TInt aBitmapId,
            TInt aMaskId,
            TInt aTabId );

        /**
        * Create navigation pane tabgroup 
        */
        void AddTabSettingsGroupL();

    private: //Data
        CAknNavigationControlContainer* iNaviPane;
        CAknTabGroup*                   iTabGroup; 
        CAknNavigationDecorator*        iDecoratedTabGroup; // owned

        CArrayFix<TUid>*                iViewIds;
        CMPSettingsMainView*            iMainView;
        RImplInfoPtrArray               iImplInfoArray;

        CMPSettingsModel* iModel;
        CMPSettingsModelForROP* iRopModel;
        
		TBool	iConstructAsGsPlugin;
    };

#endif

// End of File
