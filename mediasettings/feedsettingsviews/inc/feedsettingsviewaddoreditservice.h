/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for "Add/edit service list" in Settings UI.*
*/




#ifndef VCXNSSETTINGSVIEWADDOREDITSERVICE_H
#define VCXNSSETTINGSVIEWADDOREDITSERVICE_H

// INCLUDES
#include "feedsettingsviewcontainer.h"

// CLASS DECLARATION
/**
 * Class for "Add/edit service list" in Settings UI.
 *
 *  @lib vcsettingsview.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CVcxNsSettingsViewAddOrEditService ) : public CBase
    {
public: // Constructors and desctructor.

    /**
     * Dynamic construction.
     *
     * @param aParent      Pointer to parent container.
     * @param aServiceSettings Reference to Service Settings in UI Engine.
     * @return New class object.
     */
    static CVcxNsSettingsViewAddOrEditService* NewL(
            CVcxNsSettingsViewContainer* aParent,
            CVcxNsServiceSettings& aServiceSettings );

    /**
     * Destructor.
     */
    virtual ~CVcxNsSettingsViewAddOrEditService();

public:

    /**
     * Activates "add/edit service list".
     */
    void ActivateL();

    /**
     * Deactivates "add/edit service list".
     */
    void Deactivate();

    /**
     * Loads data to "add/edit service list".
     *
     * @param aAdding      ETrue if adding a service, EFalse if editing.
     * @param aServiceType Type of service as CIptvService::TServiceType enumeration.
     */
    void LoadL( TBool aAdding, CIptvService::TServiceType aServiceType );

    /**
     * Saves data in "add/edit service list".
     */
    void SaveL();

    /**
     * Returns pointer to "add/edit service" listbox. No ownership transfer.
     *
     * @return Pointer to listbox.
     */
    CVcxNsSettingsViewAddServiceListbox* ListBox();

private:

    /** 
     * Constructor.
     *
     * @param aParent      Pointer to parent container.
     * @param aServiceSettings Reference to Service Settings in UI Engine.
     */
    CVcxNsSettingsViewAddOrEditService(
            CVcxNsSettingsViewContainer* aParent,
            CVcxNsServiceSettings& aServiceSettings );

    /**
     * EPOC constructor.
     */
    void ConstructL();

private: // Data

    /**
     * Parent control container
     */
    CVcxNsSettingsViewContainer* iParent;
    
    /**
     * UI engine's application settings provider
     */   
    CVcxNsServiceSettings& iServiceSettings;
    
    /**
     * List box control
     */    
    CVcxNsSettingsViewAddServiceListbox* iListbox;

    /**
     * Service type
     */
    CIptvService::TServiceType iServiceType;
    
    /**
     * Service name
     */
    TBuf<KIptvSmServicesDbNameMaxLength> iAddServiceName;
    
    /**
     * Service address
     */
    TBuf<KIptvSmServicesDbAddressMaxLength> iAddServiceAddress;
    
    /**
     * Service's old address
     */
    TBuf<KIptvSmServicesDbAddressMaxLength> iPreviousAddress;

    /**
     * Service plugin type
     */
    TInt iAddServicePluginType;

    };

#endif // VCXNSSETTINGSVIEWADDOREDITSERVICE_H
