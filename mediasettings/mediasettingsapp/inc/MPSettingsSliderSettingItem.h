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
* Description:   Slider Settings for MPSettingsSliderSettingItem.h*
*/


// Version : %version: 3 %




#ifndef CMPSettingsSliderSettingItem_H
#define CMPSettingsSliderSettingItem_H

//  INCLUDES
#include    <aknsettingitemlist.h>

// CLASS DECLARATION

/**
* CMPSettingsSliderSettingItem class derived from CAknSliderSettingItem.
*
* Overwrites functions SettingTextL() and CreateBitmapL() in order to enable displaying
* textual current value instead of a bitmap in the settings item list. Should not be
* further derived.
*
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsSliderSettingItem : public CAknSliderSettingItem
    {
    public:

        /**
        * C++ default constructor.
        */
        CMPSettingsSliderSettingItem(TInt aIdentifier, TInt& aExternalSliderValue,
                                 TInt aResource, TBool aBareFigure = ETrue);

        /**
        * Destructor.
        */       
        virtual ~CMPSettingsSliderSettingItem();

    public:

        /**
        * From CAknSettingItem, obtains the current value's setting text.
        * @since 1.0
        * @return Reference to descriptor with the text.
        */
        const TDesC& SettingTextL();

    protected:

        /**
        * From CAknSliderSettingItem
        */
        CAknSettingPage* CreateSettingPageL();

        /**
        * From CAknSliderSettingItem
        */
        CFbsBitmap* CreateBitmapL();

    private:  // Data
        HBufC* iSettingText;
        TInt iResource;
        TBool iBareFigure;
    };

#endif      // CMPSettingsSliderSettingItem_H   
            
// End of File
    
