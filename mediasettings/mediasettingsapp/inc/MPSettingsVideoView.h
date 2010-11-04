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
* Description:   Declares view for application.*
*/


// Version : %version: 3 %



#ifndef MPSETTINGSVIDEOVIEW_H
#define MPSETTINGSVIDEOVIEW_H

// INCLUDES
#include    "MPSettingsBaseView.h"

// FORWARD DECLARATIONS
class CMPSettingsModelForROP;

// CLASS DECLARATION

/**
*  CMPSettingsVideoView view class.
* 
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsVideoView : public CMPSettingsBaseView
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPSettingsVideoView* NewLC(CMPSettingsModelForROP* aModel );

        /**
        * Destructor.
        */
        virtual ~CMPSettingsVideoView();

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
        * From MEikMenuObserver
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane ); 

    protected: // From CMPSettingsBaseView

        /**
        * Called at the end of DoActivateL.
        */
        void DynInitContainerL();

        /**
        * Creates new view specific container class.
        */
        CMPSettingsBaseContainer* NewContainerL();

    private:

        /**
        * C++ default constructor.
        */
        CMPSettingsVideoView(CMPSettingsModelForROP* aModel);

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private: //data

        CMPSettingsModelForROP* iModel;

    };

#endif      // MPSETTINGSVIDEOVIEW_H

// End of File
