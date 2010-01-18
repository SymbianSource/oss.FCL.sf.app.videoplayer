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
* Description:   CAknSettingItemList compliant setting item class for Advanced bandwidth settings.*
*/


// Version : %version: 3 %




#ifndef CMPSETTADVANCEDBWSETTINGSITEM_H
#define CMPSETTADVANCEDBWSETTINGSITEM_H

//  INCLUDES
#include    <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CEikonEnv;

// CLASS DECLARATION

/**
* CMPSettingsAdvancedBwSettingItem class derived from 
* CAknEnumeratedTextPopupSettingItem.
*  CAknSettingItemList compliant setting item class for
*  Advanced bandwidth settings.
*
*  @lib MediaSettings.app
*  @since 2.1
*/
class CMPSettingsAdvancedBwSettingItem : public CAknEnumeratedTextPopupSettingItem
    {
    public:

        /**
        * C++ default constructor.
        * @param aIdentifier Setting item id.
        * @param aValueArray Array containing fixed bw values for the list.
        * @param aValue Reference to current bw value in bits per sec.
        * @param aListValue Reference to Setting page's list value.
        * @param aQueryRes Bw query resource.
        */
        CMPSettingsAdvancedBwSettingItem(TInt aIdentifier,
                                         RArray<TInt>& aValueArray,
                                         TInt& aValue,
                                         TInt& aListValue,
                                         TInt aQueryRes);

        /**
        * Destructor.
        */       
        virtual ~CMPSettingsAdvancedBwSettingItem();

    public: // From base classes

        /**
        * From MAknSettingPageObserver
        * @param aSettingPage Pointer to setting page which has caused this callback.
        * @param aEventType Occurred setting page event type.
        */
        void HandleSettingPageEventL(CAknSettingPage* aSettingPage,
                                     TAknSettingPageEvent aEventType );

        /**
        * From CAknSliderSettingItem
        * @return Setting item text (current value).
        */
        const TDesC& SettingTextL();

        /**
        * From CAknSliderSettingItem
        * This launches the setting page for popup list setting page 
        * @param aCalledFromMenu - ignored in this class
        */
        void EditItemL(TBool aCalledFromMenu);

    private: // From base classes

        /**
        * From CAknSettingItem
        */
        void CompleteConstructionL();

    private:  // New

        /**
        * Displays bandwidth number query.
        */
        void ShowAdvancedBwQueryL();

        /**
        * Creates setting value text from aValue.
        */
        HBufC* CreateValueTextL(TInt aValue);

    private:    // Data
        HBufC* iSettingText;
        RArray<TInt>& iValueArray;
        TInt& iValue;
        TInt iQueryRes;
        CEikonEnv* iEnv;

        CArrayPtr<CAknEnumeratedText>* iEnumTextArray;
    };

#endif      // CMPSETTADVANCEDBWSETTINGSITEM_H   
            
// End of File
