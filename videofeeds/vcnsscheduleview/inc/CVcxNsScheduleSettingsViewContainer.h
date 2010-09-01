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
* Description:    Video on Demand schedule settings view container class.*
*/




#ifndef CVCXNSSCHEDULESETTINGSVIEWCONTAINER_H
#define CVCXNSSCHEDULESETTINGSVIEWCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include "CIptvUtil.h"

// FORWARD DECLARATIONS
class CVcxNsServiceSettings;
class CVcxNsScheduleSettingsSettingItem;
class CVcxNsScheduleSettingsList;
class CVcxNsScheduleSettingsView;
class CVcxNsAppUi;

// CLASS DECLARATION
/**
*  VoD schedule settings view container class. 
*
*/
class CVcxNsScheduleSettingsViewContainer : public CCoeControl
    {
    
public: // constructors and destructor

    /**
    * Default C++ constructor
    *
    * @param aSettings Reference to service settings
    * @param aServiceId Id of the service for which this container
    *                    belongs to
    */
    CVcxNsScheduleSettingsViewContainer( CVcxNsServiceSettings& aSettings,
                                           TIptvServiceId aServiceId );

    /**
    * EPOC default constructor.
    *
    * @param aRect Frame rectangle for container        
    */
    void ConstructL( const TRect& aRect );
    
    /**
    * Destructor.
    */ 
    virtual ~CVcxNsScheduleSettingsViewContainer();

public: // From CCoeControl
    
    /**
    * Key event handler. 
    * 
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                 TEventCode aType );

public: // New methods

    /** 
    * Handle the selected event.
    *
    * @param aCommand The command id invoked
    */     
    void HandleChangeSelectedSettingItemL( TInt aCommand );

    /** 
    * Saves settings
    * 
    */     
    void SaveSettingsL();

    /** 
    * Check if scheduling is active i.e. download condition is
    * not set to manual and at least one download time is selected. 
    *
    * @return ETrue if scheduling is active, otherwise EFalse.
    */   
    TBool IsSchedulingActive();
    
private: // From CCoeControl

    /**
    * Called by framework when help is launched. Returns currently
    * active view's help content.
    *
    * @param aContext On return, help context.
    */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

    /**
    * Called when container size changes.
    */
    void SizeChanged();  
        
    /**
    * Returns count of controls in container.
    */
    TInt CountComponentControls() const;

    /**
    * Returns a specific control in container.
    */
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
    * Handle global resource change.
    */
    void HandleResourceChange( TInt aType );

    /**
    * Draws container.
    */
    void Draw( const TRect& aRect ) const;

private: // Data
    
    /**
    * Setting item 
    * Own.
    */   
    CVcxNsScheduleSettingsSettingItem* iSettingItem;
    
    /**
    * Settings 
    * Not own.
    */ 
    CVcxNsServiceSettings& iSettings;
    
    /**        
    * Service id 
    */
    TIptvServiceId iServiceId;
    
    };

#endif /*CVCXNSSCHEDULESETTINGSVIEWCONTAINER_H_*/
