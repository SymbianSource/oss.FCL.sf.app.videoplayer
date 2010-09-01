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
* Description:    Container class for Setting view.*
*/




#ifndef VCSETTINGSVIEWSETTINGSVIEWCONTAINER_H
#define VCSETTINGSVIEWSETTINGSVIEWCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eikfrlb.h>
#include <AknIconArray.h> 

#include "CIptvService.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CVcxNsSettingsView;
class CVcxNsSettingsViewSettingsListbox;
class CAknSingleGraphicStyleListBox;
class CVcxNsSettingsViewAddServiceListbox;
class CAknSingleNumberStyleListBox;
class CVcxNsSettingsEngine;
class CVcxNsSettingsViewSettingItem;
class CVcxNsSettingsViewServiceType;
class CVcxNsSettingsViewServiceSelection;
class CVcxNsSettingsViewAddOrEditService;
class CVcSettingsViewChangePriority;
class CVcxNsServiceSettings;

class CAknViewAppUi;

enum TSettingsViewActiveList
    {
    /*
    ESettingItem = 1,
    EConnectionSettings,
    EServiceType,
    */
    EServiceSelection = 1,
    EAddService
    };

// CLASS DECLARATION

/**
 * Container class for Setting view.
 *
 *  @lib vcsettingsview.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CVcxNsSettingsViewContainer ) : public CCoeControl, public MEikListBoxObserver
    {
public: // Enumerations.

    enum TMenuItemType
        {
        EGeneral = 1,
        EConst,
        EConstAndInfo,
        EEditable
        };

public: // Construction and destruction.

    /**
     * @param aAppUi       Reference to application's App Ui.
     * @param aAppSettings Reference to UI Engine's AppSettings.
     * @param aParentView  Pointer to parent view.
     * @param aFs          Reference to RFs session.
     */
    CVcxNsSettingsViewContainer( CAknViewAppUi& aAppUi,
                                 CVcxNsSettingsEngine& aAppSettings,
            CVcxNsServiceSettings& aServiceSettings,
                                 CVcxNsSettingsView* aParentView,
                                 RFs& aFs);

    /**
     * @param aRect Frame rectangle for container.
     */
    void ConstructL(const TRect& aRect);

    ~CVcxNsSettingsViewContainer();

    /**
     * From CCoeControl
     * Called by framework when help is launched. Returns currently
     * active view's help content.
     * @param aContext On return, help context.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

public:

    /**
     * Returns pointer to App Ui.
     *
     * @return Pointer to App Ui.
     */
    CAknViewAppUi* AppUi();

    /**
     * Returns pointer to Settings view.
     *
     * @return Pointer to Settings view.
     */
    CVcxNsSettingsView* SettingsView();

    /**
     * Returns pointer to Service Selection.
     *
     * @return Pointer to Service Selection.
     */
    CVcxNsSettingsViewServiceSelection* ServiceSelection();

    /**
     * Switches "service selection list" visible.
     */
    void SwitchToServiceSelectionListL();

    /**
     * Switches "service selection list" of type Vod visible.
     *
     * @param aServiceType Type of service
     */
    void SwitchToServiceSelectionListL( CIptvService::TServiceType aServiceType );

    /**
     * Closes "service selection list".
     */
    void FinishServiceSelectionList();

    /**
     * Sets title for this view.
     *
     * @param aResourceId   Title resource id.
     */
    void SetTitleL( TInt aResourceId );
    
    /**
     * Switches "add/edit service list" visible.
     */
    void SwitchToAddOrEditServiceListL();

    /**
     * Returns type of currently active list.
     *
     * @return One of TSettingsViewActiveList enumerations.
     */
    TSettingsViewActiveList GetActiveList()
        {return(iActiveList);}

    /**
     * Deletes currently active video service.
     * Functional only in "service selection list".
     */
    void DeleteVideoServiceL();

    /**
     * Returns current service type of "service selection list".
     *
     * @return Service type as CIptvService::TServiceType enumeration.
     */
    CIptvService::TServiceType GetServiceSelectionServiceType();

    /**
     * Loads/updates data to "add/edit service list".
     *
     * @param aAdding ETrue if list will be used for adding,
     *                EFalse if it will be used for editing.
     */
    void LoadAddServiceInformationL(TBool aAdding);

    /**
     * Saves data from "add/edit service list".
     */
    void SaveAddServiceInformationL();

    /**
     * Saves data (=selections) from "service selection list".
     */
    void SaveServiceSelectionInformationL();

    /**
     * Opens current setting item for editing.
     *
     * @param aCalledFromMenu ETrue if edit request comes from menu.
     */
    void EditCurrentSettingItemListItemL(TBool aCalledFromMenu);

    /**
     * Opens dialog with video service details.
     * Functional only in "service selection list".
     */
    void ShowVideoServiceDetailsL();

    /**
     * Handles mark commands received from menu.
     * Functional only in "service selection list".
     *
     * @param aCommand Mark command.
     */
    void HandleMarkCommandL(TInt aCommand);

    /**
     * Key event handler. 
     * Note: Changed to public for key routing.
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    
    /**
     * Returns true if "mark" menu item can be shown for current list item.
     * Functional only in "service selection list".
     *
     * @return ETrue if "mark" menu item can be shown.
     */
    TBool ShowMarkForCurrentListItemL();

    /**
     * Returns true if "unmark" menu item can be shown for current list item.
     * Functional only in "service selection list".
     *
     * @return ETrue if "unmark" menu item can be shown.
     */
    TBool ShowUnmarkForCurrentListItemL();

    /**
     * Returns true if menu item of given type can be shown for current list item.
     * Functional only in "service selection list".
     *
     * @param aMenuItemType Menu item type as TMenuItemType enumeration.
     * @return ETrue if given menu item can be shown.
     */
    TBool ShowMenuItemForCurrentListItemL(TMenuItemType aMenuItemType);

    /**
     * Load Vodcast data to "Add feed" view. Used when add feed called directly 
     * from Vodcast view.
     */
    void LoadAddVodCastFeedL();

    /**
     * From MEikListBoxObserver
     * Event message handler to be used for the service type, 
     * selection and add/edit Listboxes.
     *
     * @param *aListBox not used in this context
     * @param aEventType A code for the event.
     */
    void HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType);
    
private:

    /**
     * Returns true if current list item is marked.
     * Functional only in "service selection list".
     */
    TBool IsCurrentListItemMarked();

    /**
     * From CoeControl. Called when container size changes.
     */
    void SizeChanged();

    /**
     * From CoeControl. Returns count of controls in container.
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl. Returns a specific control in container.
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

    /**
     * From CCoeControl. Handle global resource change.
     */
    void HandleResourceChange(TInt aType);

    /**
     * From CCoeControl. Draws container.
     */
    void Draw(const TRect& aRect) const;

    /**
     * 
     */
    TInt TitleResourceId();    
    
public:
    /**
     * Zeroes interal msk tracker. DOES NOT ACTUALLY CLEAR MSK!
     */
    void ClearMsk();

    /**
     * Check if the msk need update and updates if needed
     */
    void CheckMsk();

private: // Data

    /**
     * App UI
     */
    CAknViewAppUi& iAppUi;
    
    /**
     * reference to Settings engine
     */
    CVcxNsSettingsEngine& iAppSettings;

    /**
     * UI engine's service settings provider
     */
    CVcxNsServiceSettings& iServiceSettings;
    
    /**
     * Parent View
     */
    CVcxNsSettingsView* iParentView;

    /**
     * iFs
     */
    RFs& iFs;
    
    /**
     * Currently active setting item list
     */
    TSettingsViewActiveList iActiveList;
    
    /**
     * Current MSK resource id
     */
    TInt iCurrentMskResource;
    
    /**
     * Service selection setting item
     */
    CVcxNsSettingsViewServiceSelection* iServiceSelection;
    
    /**
     * Add or edit service setting item
     */
    CVcxNsSettingsViewAddOrEditService* iAddOrEditService;

    };

#endif // VCSETTINGSVIEWSETTINGSVIEWCONTAINER_H

