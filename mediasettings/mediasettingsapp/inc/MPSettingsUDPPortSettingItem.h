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
* Description:   MPSettingsUDPPortSettingItem.h*
*/


// Version : %version: 3 %




#ifndef CMPSETTINGSUDPPORTSETTINGITEM_H
#define CMPSETTINGSUDPPORTSETTINGITEM_H

//  INCLUDES
#include    <aknsettingitemlist.h>

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class CMPSettingsUDPPortSettingItem : public CAknIntegerEdwinSettingItem
    {
    public:  // Constructors and destructor
        

        /**
        * C++ default constructor.
        */
        CMPSettingsUDPPortSettingItem(TInt aIdentifier,
                                      TInt& aValue,
                                      TInt& aRefValue);
        
        /**
        * Destructor.
        */
        virtual ~CMPSettingsUDPPortSettingItem();

    public: // Functions from base classes

        /**
        * From CAknIntegerEdwinSettingItem
        */
        void EditItemL(TBool aCalledFromMenu);

    private:
        TInt& iRefValue;
    };

#endif      // CMPSETTINGSUDPPORTSETTINGITEM_H   
            
// End of File
