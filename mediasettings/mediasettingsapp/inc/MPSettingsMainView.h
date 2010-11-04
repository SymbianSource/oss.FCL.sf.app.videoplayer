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
* Description:  MPSettingsMainView.h*
*/


// Version : %version: 2 %




#ifndef MPSETTINGSMAINVIEW_H
#define MPSETTINGSMAINVIEW_H

// INCLUDES
#include    <eikclb.h> //MEikListBoxObserver
#include    "MPSettingsBaseView.h" //CMPSettingsBaseView
#include    "MPSettingsModelForROP.h"

// CLASS DECLARATION

/**
*   CMPSettingsMainView view class.
*
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsMainView : public CMPSettingsBaseView,
                            public MEikListBoxObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMPSettingsMainView* NewLC(CArrayFix<TUid>* aViewIds, CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin);

        /**
        * Destructor.
        */
        virtual ~CMPSettingsMainView();

    public: // Functions from base classes
        
        /**
        * From CAknView
        */
        TUid Id() const;

        /**
        * From MEikMenuObserver
        */
        void HandleCommandL(TInt aCommand);

    public: // New

        /**
        * Returns the index of the active folder.
        * @return 0: video
        *         1: streaming
        */
        TInt ActiveFolder();

        /**
        * Sets the index of the active folder.
        * @param aCurrentItem Index of current item.
        */
        void SetActiveFolder(TInt aCurrentItem);

    protected: // Functions from base classes

        /**
        * From MEikListBoxObserver
        */
        void HandleListBoxEventL(CEikListBox* aListBox,
                                 TListBoxEvent aEventType);

        /**
        * From CMPSettingsBaseView
        */
        void DynInitContainerL();

        /**
        * From CMPSettingsBaseView
        */
        CMPSettingsBaseContainer* NewContainerL();

    private:

        /**
        * C++ default constructor.
        */
        CMPSettingsMainView(CArrayFix<TUid>* aViewIds, CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin);

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private: // New 

        /**
        * Handles listbox selection.
        */
        virtual void HandleListBoxSelectionL();

    private: // Data
        CArrayFix<TUid>* iViewIds;
        
        CMPSettingsModelForROP* iModel;
		TBool	iGsPlugin;

    };

#endif      // MPSETTINGSMAINVIEW_H   
            
// End of File
