/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Video on Demand setting item class for dowload condition*
*/




#ifndef CVCXNSSCHEDULESETTINGSDLCONDITIONITEM_H
#define CVCXNSSCHEDULESETTINGSDLCONDITIONITEM_H

// INCLUDES
#include <aknsettingitemlist.h>

// CLASS DECLARATION
/**
*
*  Video on Demand setting item for download condition.
*
*/
class CVcxNsScheduleSettingsDlConditionItem : 
        public CAknEnumeratedTextPopupSettingItem
    {
    public: // constructors and destructor
    
        /**
        * C++ constructor.
        *
        * @param aIdentifier    Resource identifier.
        * @param aValue         The value to be manipulated. 
        * @param aWlanSupported WLAN supported
        * 
        */
        CVcxNsScheduleSettingsDlConditionItem( TInt aResourceId, 
                                                 TInt& aValue,
                                                 TBool aWlanSupported );
        /**
        * Destructor
        */
        virtual ~CVcxNsScheduleSettingsDlConditionItem();
       
    public: // From CAknSettingItem
       
        /**
        * Completes construction of this setting item.
        */
        void CompleteConstructionL();
       
    private: // data
    
        /**
        * Is WLAN supported
        */
        TBool iWlanSupported;
    };

#endif /*CVCXNSSCHEDULESETTINGSDLCONDITIONITEM_H*/

// End of File
