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
* Description:   Base class for Media Settings views.*
*/


// Version : %version: 2 %




#ifndef MPSETTINGSBASEVIEW_H
#define MPSETTINGSBASEVIEW_H

//  INCLUDES
#include    <aknview.h>
    
// FORWARD DECLARATIONS
class CMPSettingsBaseContainer;
class CAknNavigationDecorator;
class CMPSettingsAppUi;
class MMPSettingsNaviPaneController;

// CLASS DECLARATION

/**
*  Base class for Media Settings views.
*
*  @lib MPSettBase.dll
*  @since 2.0
*/
class CMPSettingsBaseView : public CAknView
    {
    public:     // New data types
        enum KMPViewPanicCodes
            {
            EMPViewPanicNullPtr = 1
            };

    public:     // Constructors and destructor

        /**
        * C++ default constructor
        */
        IMPORT_C CMPSettingsBaseView();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMPSettingsBaseView();

    public:     // from CAknView

        /**
        * Handles client rect change.
        */
        IMPORT_C void HandleClientRectChange();

    public:     // New

        /**
        * This function is used to set the current item in the listbox.
        * @since 2.0
        * @param aIndex Current item.
        */
        IMPORT_C virtual void SetCurrentItem(TInt aIndex);

    protected:  // Functions from base classes

        /**
        * From CAknView
        */
        IMPORT_C void DoActivateL(const TVwsViewId& aPrevViewId,
                                  TUid aCustomMessageId,
                                  const TDesC8& aCustomMessage);

        /**
        * From CAknView
        */
        IMPORT_C void DoDeactivate();

        /**
        * From MEikMenuObserver
        */
        IMPORT_C void DynInitMenuPaneL(TInt aResourceId,
                                       CEikMenuPane* aMenuPane);        

    protected:  // New

        /**
        * Sets the correct navi pane for the view (eg. tabs, empty,
        *   string etc.)
        * @since 2.0
        */
        IMPORT_C virtual void SetNaviPaneL();

        /**
        * Called at the end of DoActivateL.
        * @since 2.0
        */
        IMPORT_C virtual void DynInitContainerL();

        /**
        * Creates new view specific container class. Pure virtual.
        * @since 2.0
        */
        virtual CMPSettingsBaseContainer* NewContainerL() = 0;

        /** 
        * Adds given resource text as MSK to CBA.
        *
        * @param aResourceId middle softkey label.
        * @param aCommandId command that should be performed when MSK
        *        is pressed.
        * @since S60 v3.2
        */
        IMPORT_C virtual void SetMiddleSoftKeyLabelL(
            const TInt aResourceId,
            const TInt aCommandId );

    protected:  // Data
        CMPSettingsBaseContainer* iContainer;
        CAknNavigationDecorator* iNaviPaneContext;
        MMPSettingsNaviPaneController* iNaviCntrl;

        TInt iCurrentItem;
        TInt iTopItemIndex;
    };

#endif      // MPSETTINGSBASEVIEW_H   
            
