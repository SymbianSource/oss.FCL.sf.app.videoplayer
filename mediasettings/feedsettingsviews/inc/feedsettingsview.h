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
* Description:    CVcxNsSettingsView view class definitions.*
*/




#ifndef VCXNSSETTINGSVIEW_H
#define VCXNSSETTINGSVIEW_H

// INCLUDES
#include <akncontext.h>
#include <AknsUtils.h>
#include <gsplugininterface.h>
#include <gstabbedview.h>

#include "feedsettingsview.hrh"

// FORWARD DECLARATIONS
class CVcxNsSettingsViewContainer;
class CIptvResourceLoader;
class CVcxNsSettingsEngine;
class CVcxNsServiceSettings;
class MMPXViewUtility;

class MGSTabbedView;
class CGSTabHelper;
class CAknNavigationDecorator;
class MMPSettingsNaviPaneController;

// CONSTS
const TInt32 KVcxNsSettingsViewUid = 0x20016B99;

// CLASS DECLARATION

/**
 *  CVcxNsSettingsView view class definitions.
 *
 *  @lib vcsettingsview.lib
 *  @since S60 5.0
 */
class CVcxNsSettingsView  : public CGSPluginInterface, 
                            public MGSTabbedView
    {
public:
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CVcxNsSettingsView* NewL( );
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CVcxNsSettingsView* NewLC( CArrayPtrFlat<MGSTabbedView>* aTabViewArray = NULL);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CVcxNsSettingsView( );

protected:
    /**
     * Constructor.
     */
    IMPORT_C CVcxNsSettingsView( const TUid aViewId = TUid::Uid( KVcxNsSettingsViewUid ) );

    /**
     * EPOC default constructor.
     */
    IMPORT_C void ConstructL( CArrayPtrFlat<MGSTabbedView>* aTabViewArray );
    
public:

    /**
    * MGSTabbedView
    */       
    CGulIcon* CreateTabIconL();
    
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
     * here because this view has overall control of the Settings UI.
     */
    TKeyResponse RoutedKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType );

    /**
     * Returns the button group container. Can be used for changing softkeys.
     *
     * @return Button group container.
     */
    CEikButtonGroupContainer* GetButtonGroupContainer( );

    /**
     * Sets "service subscription" mode on. When Settings UI is activated
     * in this mode, it goes directly to service subscriptions view.
     */
    void SetServiceSubscriptionMode();

    /**
     * Sets "Vodcast add feed subscription" mode on. When this is activated
     * it goes directly Add feed and returns to Vodcast view.
     */
    void SetVodcastAddFeedMode();

    /**
     * Checks if Settings UI is currently being displayed.
     *
     * @return ETrue if Settings UI is active.
     */
    TBool IsSettingsViewActive();

    /**
     * From CGSPluginInterface
     * GetCaptionL
     * Dummy implementation, implemented in settings plugin.
     */
    virtual void GetCaptionL( TDes& /*aCaption*/ ) const {};

    /**
     * Is the view launhed from general settings framework?
     * @return Is it launched from gs fw.
     */
    TBool LaunchedFromGsFw( );
   
    /**
     * Returns pointer to container
     * 
     * @return Pointer to container.
     */
    CVcxNsSettingsViewContainer* Container();

    /**
     * Sets navi pane.
     * 
     * @param aDisableNaviPane  If ETrue navi pane is disabled.
     */
    void SetNaviPaneL( TBool aDisableNaviPane = EFalse );
    
protected:

    /**
     * Handles activation message.
     * 
     * @param aCustomMessage    Custom message
     */
    void HandleActivationMessageL( const TDesC8& aCustomMessage );
    
    /**
     * Handle Back command.
     */
    void HandleBackCommandL();
    
    /**
     * Handle Back command service selection.
     */
    void HandleBackCommandForServiceSelectionL();
    
    /**
     *  Handle Back command for add service view
     */
    void HandleBackCommandForAddServiceL();
    
    /**
     * Handle Add/Edit VoD service command.
     * 
     * @param aIsAdding EFalse for edit, ETrue for add.
     */
    void HandleVodAddOrEditCommandL( TBool aIsAdding );

    /**
     * Saves service selection.
     * 
     */
    void SaveServiceSelectionL();
    
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
     * Adjusts the visible items in menu pane accordding to Settings UI state.
     *
     * @param aResourceId Resource ID identifying the menu pane to initialise.
     * @param aMenuPane   The in-memory representation of the menu pane.
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
     * Handles status pane size changes.
     */
    IMPORT_C void HandleStatusPaneSizeChange();
    
    /**
    * Locates the mif file searching from y: to a: and then z: last
	*
    * @param aFileName Full path to mif file.
    */
    void LocateMifFileL( TFileName& aFileName );

private:

    /**
     * This view's ID
     */
    TUid iViewId;
    
    /**
     * Settings control container
     */
    CVcxNsSettingsViewContainer* iContainer;
    
    /**
     * Resource loader
     */
    CIptvResourceLoader* iResourceLoader;

    /**
     * Previously active view's ID
     */
    TVwsViewId iPrevViewId;

    /**
     * Status pane layout from previously active view.
     */
    TInt iPrevStatusPaneLayout;

    /**
     * iFs
     */
    RFs iFs;
    
    /**
     * Settings engine
     */
    CVcxNsSettingsEngine* iSettingsEngine;

    /**
     * UI engine's service setting if.
     */
    CVcxNsServiceSettings* iServiceSettings;
    
    /**
     * Is settings activated in ServiceSubscriptionMode
     */
    TBool iServiceSubscriptionMode;
    
    /**
     * Is settings activated in VodcastAddFeedMode
     */
    TBool iVodcastAddFeedMode;

    /**
     * Is settings activated from GSVideoPlugin
     */
    TBool iGSVideoPluginMode;
    
    /**
    * Mpx view Utility
    */
    MMPXViewUtility* iViewUtility;

    /**
    * General Settings tab helper
    */
    CGSTabHelper* iTabHelper;
    
    /**
     * Navi pane context.
     * Not own.
     */
    CAknNavigationDecorator* iNaviPaneContext;
        
    /**
     * Navi pane controller.
     * Not own.
     */
    MMPSettingsNaviPaneController* iNaviCntrl;
    
    /*
     * View array.
     * Not own.
     */
    CArrayPtrFlat<MGSTabbedView>* iViewArray; 
        
    };

#endif // VCXNSSETTINGSVIEW_H
