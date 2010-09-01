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
* Description:   M-class for NaviPane related callbacks to appui from view classes.*
*/


// Version : %version: 2 %




#ifndef MMPSETTINGSNAVIPANECONTROLLER_H
#define MMPSETTINGSNAVIPANECONTROLLER_H

// FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CAknNavigationDecorator;

// CLASS DECLARATION

/**
*  
*
*  @lib MediaSettings.app
*  @since 2.0
*/
class MMPSettingsNaviPaneController
    {
    public: // New functions
        
        /**
        * Returns pointer to MP's Navi pane object.
        * @return CAknNavigationControlContainer.
        */
        virtual CAknNavigationControlContainer* NaviContainer() const = 0;
    
        /**
        * Returns pointer to MP's tab group object.
        * @return CAknNavigationDecorator.
        */
        virtual CAknNavigationDecorator* MPTabGroup() const = 0;

        /**
        * Updates tab group according to active view.
        */
        virtual void UpdateTabIndex() = 0;

    };

#endif      // MMPSETTINGSNAVIPANECONTROLLER_H   
            
// End of File
