/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    CVcxNsScheduleView view class definitions.*
*/




#ifndef VCXNSSCHEDULEVIEW_H
#define VCXNSSCHEDULEVIEW_H

// INCLUDES
#include <akncontext.h>
#include <AknsUtils.h>
#include <aknview.h>

#include "CIptvUtil.h"

// FORWARD DECLARATIONS
class CVcxNsScheduleSettingsViewContainer;
class CVcxNsScheduleViewApPage;
class CIptvResourceLoader;
class CVcxNsSettingsEngine;
class CVcxNsServiceSettings;
class MMPXViewUtility;

// CONSTS
const TInt32 KVcxNsScheduleViewUid = 0x2001B2B0;

// CLASS DECLARATION

/**
 *  CVcxNsScheduleView view class definitions.
 *
 *  @lib vcscheduleview.lib
 *  @since S60 5.0
 */
class CVcxNsScheduleView : public CAknView
    {
public:
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CVcxNsScheduleView* NewL( );
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CVcxNsScheduleView* NewLC( );

    /**
     * Destructor.
     */
    virtual ~CVcxNsScheduleView( );

protected:

    /**
     * Constructor.
     */
    IMPORT_C CVcxNsScheduleView( const TUid aViewId = TUid::Uid( KVcxNsScheduleViewUid ) );

    /**
     * EPOC default constructor.
     */
    IMPORT_C void ConstructL();
    
public:

    /**
     * From CAknView.
     */
    TUid Id( ) const;

    /**
     * From CAknView.
     *
     * @param aCommand Command for view.
     */
    IMPORT_C void HandleCommandL(TInt aCommand);

    /**
     * Handles client rectangle changes.
     */
    IMPORT_C void HandleClientRectChange();

    /**
     * Handles routed key events. Other components might route their key events
     * here because this view has overall control of the Schedule UI.
     */
    TKeyResponse RoutedKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType );

    /**
     * Returns the button group container. Can be used for changing softkeys.
     *
     * @return Button group container.
     */
    CEikButtonGroupContainer* GetButtonGroupContainer( );

    /**
     * Checks if Schedule UI is currently being displayed.
     *
     * @return ETrue if Schedule UI is active.
     */
    TBool IsScheduleViewActive();

public: // New methods
    
    /** 
    * Handle the selected event.
    *
    * @param aCommand the command id invoked
    * @return ETrue if the command was handled, EFalse if not
    */     
    TBool HandleChangeSelectedSettingItemL( TInt aCommand );

    /** 
    * Check IAP for scheduled download.
    * Checks if access point is available for scheduled download. 
    * If no IAP is available, user is asked to select one.
    *
    * @return ETrue if IAP was already available or user selected 
    *         valid IAP. Otherwise EFalse.
    */     
    TBool CheckScheduledDownloadIapL();

private: // New methods

    /**
     * Launch Snap selection query.
     *
     * @param aUsedSnapId Snap selection
     * @return ETrue if user selected valid Snap. Otherwise EFalse.
     */
    TBool LaunchSnapSelectionL( TInt& aUsedSnapId ) const;

protected:

    /**
     * From CAknView
     * Called when view is activated.
     * @param aPrevViewId
     * @param aCustomMessageId
     * @param aCustomMessage
     */
    IMPORT_C virtual void DoActivateL( const TVwsViewId& aPrevViewId,
                              TUid aCustomMessageId, 
                              const TDesC8& aCustomMessage );

    /**
     * From CAknView 
     * Called when view is deactivated.
     */
    IMPORT_C virtual void DoDeactivate();

    /**
     * Adjusts the visible items in menu pane accordding to Schedule UI state.
     *
     * @param aResourceId Resource ID identifying the menu pane to initialise.
     * @param aMenuPane   The in-memory representation of the menu pane.
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
     * Handles status pane size changes.
     */
    IMPORT_C void HandleStatusPaneSizeChange();

private:

    /**
     * This view's ID
     */
    TUid iViewId;
    
    /**
     * Schedule control container
     */
    CVcxNsScheduleSettingsViewContainer* iContainer;
    
    /**
     * Resource loader
     */
    CIptvResourceLoader* iResourceLoader;

    /**
     * Previously active view's ID
     */
    TVwsViewId iPrevViewId;

    /**
     * UI engine's service setting if.
     */
    CVcxNsServiceSettings* iServiceSettings;

    /**
    * Mpx view Utility
    */
    MMPXViewUtility* iViewUtility;

    };

#endif // VCXNSSCHEDULEVIEW_H
