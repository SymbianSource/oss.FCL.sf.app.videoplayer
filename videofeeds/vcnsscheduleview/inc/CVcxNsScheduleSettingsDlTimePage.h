/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Video on Demand download time setting selection page*
*/




#ifndef CVCXNSSCHEDULESETTINGSDLTIMEPAGE_H
#define CVCXNSSCHEDULESETTINGSDLTIMEPAGE_H

// INCLUDES
#include <akncheckboxsettingpage.h>

// CLASS DECLARATION
/**
* 
*  Download time selection setting page.
*
*/
class CVcxNsScheduleSettingsDlTimePage : public CAknCheckBoxSettingPage
    {
    public:  // Constructors and destructor
        
        /**
        * Constructor.
        *
        * @param aResourceID    Resource ID.
        * @param aItemArray     Item array for this settings page.
        */       
        CVcxNsScheduleSettingsDlTimePage( TInt aResourceID,
                                            CSelectionItemList* aItemArray);
        /**
        * Destructor.
        */ 
        virtual ~CVcxNsScheduleSettingsDlTimePage();
    
    public: // From CAknSettingPage.
    
    	/**
        * Handle key event
        *
        */ 
    	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
    	                             TEventCode aType );
   
    public: // New methods
    	
    	/**
        * Sets correct softkeys for setting page.
        */ 
    	void SetCbaL();
    	
    private:
        
        /**
        * Reference to associated selection item list. 
        */
        CSelectionItemList& iItemArray;
        
    };

#endif /* CVCXNSSCHEDULESETTINGSDLTIMEPAGE_H */

// End of File
