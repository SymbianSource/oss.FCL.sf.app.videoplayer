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
* Description:   Declares view for application*
*/


// Version : %version: 2 %



#ifndef MPSETTINGSPROXYVIEW_H
#define MPSETTINGSPROXYVIEW_H

// INCLUDES
#include    "MPSettingsBaseView.h"

// CLASS DECLARATION
class CMPSettingsModelForROP;
/**
*  CMPSettingsProxyView view class.
* 
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsProxyView : public CMPSettingsBaseView
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPSettingsProxyView* NewLC(CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin);

        /**
        * Destructor.
        */
        virtual ~CMPSettingsProxyView();

    public: // Functions from base classes
        
        /**
        * From CAknView
        */
        TUid Id() const;

        /**
        * From MEikMenuObserver
        */
        void HandleCommandL(TInt aCommand);

        /**
        * Sets Navipane text
        * @since 3.1
        */
        void SetNaviPaneL();
        
    protected: // From CMPSettingsBaseView

        /**
        * Creates new view specific container class.
        * @return void
        */
        CMPSettingsBaseContainer* NewContainerL();

    private:

        /**
        * C++ default constructor.
        */
        CMPSettingsProxyView(CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin);
    
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private: //Data
    
        CMPSettingsModelForROP* iModel;
        TBool   iGsPlugin;

    };

#endif      // MPSETTINGSPROXYVIEW_H

// End of File
