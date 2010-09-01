/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for Add Service list in Settings UI.*
*/




#ifndef VCXNSSETTINGSVIEWADDSERVICELISTBOX_H
#define VCXNSSETTINGSVIEWADDSERVICELISTBOX_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CVcxNsSettingsViewContainer;

// CLASS DECLARATION

/**
 * The settings list for "add/edit service list". Used by class 
 * CVcSettingsViewAddOrEditService. Part of Settings UI.
 *
 *  @lib vcsettingsview.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CVcxNsSettingsViewAddServiceListbox ) : public CAknSettingItemList
    {
public:

    /**
     * Constructor.
     *
     * @param aParent      Parent container.
     * @param aServiceName Reference to data member where service name is stored.
     * @param aAddress     Reference to data member where service address is stored.
     * @param aPluginType  Reference to data member where service type is stored.
     */
    CVcxNsSettingsViewAddServiceListbox(CVcxNsSettingsViewContainer* aParent,
            TDes& aServiceName,
            TDes& aAddress,
            TInt& aPluginType );

    /**
     * Updates the internal references to data.
     * Set input data to variables before calling this method.
     *
     * @param aAdding      ETrue if adding a service, EFalse if editing.
     * @param aServiceName Reference to data member where service name is stored.
     * @param aAddress     Reference to data member where service address is stored.
     * @param aServiceType Service type.
     * @param aPluginType  Reference to data member where plugin type is stored.
     */
    void LoadAddServiceInformationL(TBool aAdding,
            TDes& aServiceName,
            TDes& aAddress,
            CIptvService::TServiceType aServiceType, 
            TInt& aPluginType );

    /*
     * Copies data from UI (this list) to referenced data variables.
     */
    void SaveAddServiceInformationL();

    /**
     * Is list shown for adding new item or for editing old.
     *
     * @return ETrue if adding a service, EFalse if editing.
     */
    TBool IsAdding();

    /**
     * Creates SettingItem. Overridden from CAknSettingItemList
     *
     * @param identifier Index numbre of the setting item to create.
     * @return pointer to new CAknSettingItem object.
     */
    CAknSettingItem* CreateSettingItemL(TInt identifier);

    /**
     * Opens editor for current setting item.
     */
    void EditCurrentItemL();

public:
    /**
     * From CCoeControl. 
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );

    /**
     * Event handling section, from MEikListBoxObserver.
     */
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aListBoxEvent );

private:

    /**
     * From CCoeControl. Called by framework when the size is changed.
     */
    void SizeChanged();

private: // Data

    /**
     * Parent control container
     */
    CVcxNsSettingsViewContainer* iParent;

    /**
     * ServiceName setting item
     */
    CAknTextSettingItem* iServiceNameItem;
    
    /**
     * Address setting item
     */
    CAknTextSettingItem* iAddressItem;

    /**
     * Service type
     */
    CIptvService::TServiceType iServiceType;
    
    /**
     * Plugin type item
     */
    CAknEnumeratedTextPopupSettingItem* iPluginTypeItem;

    /**
     * Is adding ongoing
     */
    TBool iAdding;
    
    /**
     * Service Name
     */
    TDes& iServiceName;
    
    /**
     * Address
     */
    TDes& iAddress;

    /**
     * Service plugin type
     */
    TInt& iServicePluginType;
    };

#endif // VCXNSSETTINGSVIEWADDSERVICELISTBOX_H
