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



#ifndef MPSETTINGSSTREAMINGVIEW_H
#define MPSETTINGSSTREAMINGVIEW_H

// INCLUDES
#include    <eikclb.h> //MEikListBoxObserver
#include    "MPSettingsBaseView.h" //CMPSettingsBaseView

// CLASS DECLARATION
class CMPSettingsModelForROP;

/**
*  CMPSettingsStreamingView view class.
* 
*/
class CMPSettingsStreamingView : public CMPSettingsBaseView 
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPSettingsStreamingView* NewLC(CMPSettingsModelForROP* aModel );

        /**
        * Destructor.
        */
        virtual ~CMPSettingsStreamingView();

    public: // Functions from base classes
        
        /**
        * From CAknView
        */
        TUid Id() const;

        /**
        * From MEikMenuObserver
        */
        void HandleCommandL(TInt aCommand);

    protected: // from MEikListBoxObserver

 
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
        CMPSettingsStreamingView(CMPSettingsModelForROP* aModel );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private: // New 

        /**
        * Handles listbox selection.
        */
		CMPSettingsModelForROP* iModel;

    };

#endif

// End of File
