/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    HG VOD Feed view implementation*
*/


// Version : %version: tr1ido#REL6_12 %

#ifndef VCXHGVODMAINVIEW_H
#define VCXHGVODMAINVIEW_H

#include <aknview.h>
#include <videoplayeractivationmessage.h>
#include "vcxviewmessageutility.h"

class CAknTitlePane;
class CAknNavigationControlContainer;
class CEikonEnv;
class CAknsBasicBackgroundControlContext;
class CGulIcon;
class CRepository;
class CVcxHgVodMainViewContainer;
class CIptvResourceLoader;
class MEikMenuObserver;
class CVcxNsPlayerWrapper;
class CVcxNsUiEngine;
class MMPXViewUtility;
class CVcxNsContent;
class CVcxHgVodServiceListImpl;
class CVcxHgVodCategoryListImpl;
class CVcxHgVodContentListImpl;
class CVcxHgVodServiceGroupListImpl;
class CVcxHgVodListImplBase;
class CVcxHgVodTelephonyClient;
class TViaPlayerCustomMessage;
class CHgDoubleGraphicList;
class CHgDoubleTextList;

// CLASS DECLARATION
/**
 *  Main view
 *  
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
class CVcxHgVodMainView : public CAknView,
                          public MVcxViewMessageObserver
    {

public:
    
    //list type
    enum TVcxHgVodListType
        {
        EVcxNsServiceList,
        EVcxNsCategoryList,
        EVcxNsContentList,
        EVcxNsCustomList
        };
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CVcxHgVodMainView* NewL( );
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CVcxHgVodMainView* NewLC( );

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CVcxHgVodMainView();
    
    /** 
     * Accessor for the application's AppUi.
     *
     * @return Pointer to AppUi.
     */
    CAknViewAppUi* AppUi() const { return CAknView::AppUi(); }
    
    /**
    * Get pointer to title pane
    * @return Pointer to CAknTitlePane
    */
    CAknTitlePane* TitlePaneL();
    
    /**
    * Get pointer to navi pane
    * @return Pointer to CAknNavigationControlContainer
    */
    CAknNavigationControlContainer* NaviPaneL();

    /**
     * Accessor for player wrapper.
     * 
     * @return Pointer to player wrapper.
     */
    CVcxNsPlayerWrapper* PlayerWrapper();

    /**
     * Command handling function
     * @param aCommand ID of the command to respond to 
     */
    void HandleCommandL( TInt aCommand );
    
    /**
     * Accessor for view container
     * 
     */
    CVcxHgVodMainViewContainer* Container();

    /**
     * Handles key events from container.
     * 
     * @param aKeyEvent Key event.
     * @param aType Key event type.
     * @return Response, tells if key was handled or not.
     */
    TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );    

    /**
     * Create list widget and related classes 
     * @param aType List type
     */
    void CreateListL( TVcxHgVodListType aType );
    
    /**
    * Change list inside main view. 
    * No actual view change, just switch to different list type.
    */
    void ChangeListViewL();
    
    /**
    * Open settings view via mpx view utility
    * @param aCommand Command for the state where settings is opened
    */
    void OpenSettingsL( TInt aCommand );
    
    /**
    * Open schedule settings for the service.
    */
    void OpenScheduleSettingsL();

    /**
    * Get pointer to active list in view
    * @return Pointer to active list. 
    */
    CVcxHgVodListImplBase* ActiveList();
    
    /**
     * PlayInternalVideoL
     * 
     * @param aContent Content that contains video data.
     */
    void PlayInternalVideoL( CVcxNsContent* aContent );
    
    /**
     * Play stream
     * @param aUrl Url
     */
    void PlayStreamL( const TDesC& aUrl );

    /**
     * From MVcxViewMessageObserver
     * Handles a custom view message.
     * @param aMessageUid    UID for custom message
     * @param aCustomMessage Message contents
     */
    void HandleCustomViewMessageL(
            const TUid aMessageUid, 
            const TDesC8 &aCustomMessage );
    
    /**
     * Called by list impelementation(s) when layout is changed.
     */
    void LayoutChanged();
    
    /**
     * Return eikon env.
     * 
     * @return Reference to eikon env
     */
    CEikonEnv& EikonEnv();
    
    /**
     * Return view's cba.
     */
    CEikButtonGroupContainer* ViewCba(); 
    
    /**
     * Return touch support flag.
     * ETrue if touch supported.
     */
    TBool IsTouchSupported();
        
private:
    
    /**
     * Set active list 
     * @param aList 
     */
    void SetActiveList( CVcxHgVodListImplBase* aList );
    
    /**
     * From CAknView returns Uid of View
     * @return TUid uid of the view
     */
    TUid Id() const;

    /**
     * From CAknView activate the view
     *
     * @since S60 v3.0
     * @param aPrevViewId
     * @param aCustomMessageId
     * @param aCustomMessage
     */
    void DoActivateL( const TVwsViewId& aPrevViewId,
                      TUid  aCustomMessageId,
                      const TDesC8& aCustomMessage );

    /**
     * From CAknView deactivate the view (free resources)
     *
     * @since S60 v3.0
     */
    void DoDeactivate();
	
    /**
     * Default constructor
     *
     */
    CVcxHgVodMainView( );

    /**
     * Constructor
     *
     */
    void ConstructL();
    
    //From MEikMenuObserver

    /**
    * Menu item handling method
    * @param aResourceId Menu pane resource id
    * @param aMenuPane Pointer to menu pane
    */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
    
    /**
     * Supplies objects to Mop childs.
     * @param aId Type of object to supply.
     * @return Type UID pointer containing supplied object, or @c NULL type UID
     *   pointer if given type is not found.
     */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
    
    /**
     * Handles the change of status pane.
     */
    void HandleStatusPaneSizeChange();

    /**
     * Handles the foreground event.
     * @param aForeground Is foreground state gained or not.
     */
    void HandleForegroundEventL( TBool aForeground );
    
    /**
     * Checks parental control for a video, and asks lock code.
     *
     * @leave KErrPermissionDenied If not allowed to play.
     * @param aAgeProfile Age profile for the video to be checked.
     */
    void CheckParentalControlL( TUint32 aAgeProfile );
            
    /*
     * Process custom activation message.
     * 
     * @param aCustomMessage The message.
     */
    void ProcessCustomMessageL( const TDesC8& aCustomMessage );
    
    /*
     * Handles custom activation message.
     */ 
    void HandleCustomViewMessageL();
    
    /**
     * Locates the mif file searching from y: to a: and then z: last
     *
     * @param aFileName Full path to mif file.
     */
    void LocateMifFileL( TFileName& aFileName );
        
private: // data

    /**
     * View id
     */
    TUid iViewId; 
    
    /**
    * Flag for touch support
    */
    TBool iTouchSupport;
    
    /**
     * View container
     * Own.
     */
    CVcxHgVodMainViewContainer* iContainer;
    
    /**
    * Resource loader
    * Own.
    */        
    CIptvResourceLoader* iResourceLoader;
    
    /**
     * Background context.
     * Own.
     */
    CAknsBasicBackgroundControlContext* iBgContext;
    
    /**
     * Ui engine.
     * Own.
     */
    CVcxNsUiEngine* iModel;
    
    /**
     * Service list view implementation
     * Own. 
     */
    CVcxHgVodServiceListImpl* iServiceList;

    /**
     * Category list view implementation
     * Own. 
     */
    CVcxHgVodCategoryListImpl* iCategoryList;
    
    /**
     * Content list view implementation
     * Own. 
     */
    CVcxHgVodContentListImpl* iContentList;

    /**
     * Service group list view implementation
     * Own. 
     */
    CVcxHgVodServiceGroupListImpl* iServiceGroupList;
    
    /**
     * Content list view implementation
     * Own. 
     */
    CVcxHgVodListImplBase* iActiveList;
    
    /**
    * Mpx view Utility
    */
    MMPXViewUtility* iViewUtility;
    
    /**
     * Scroller for double graphics hg list.
     * Own.
     */
    CHgDoubleGraphicList* iGraphicScroller;

    /**
     * Scroller for double text hg list.
     * Own.
     */
    CHgDoubleTextList* iTextScroller;
    
    /**
     * Default icon for services.
     * Own.
     */
    CGulIcon* iServiceDefaultIcon;
    
    /**
     * Default icon for content.
     * Own.
     */
    CGulIcon* iContentDefaultIcon;
    
    /**
     * Telephony client for lock code queries
     * Own.
     */
    CVcxHgVodTelephonyClient* iTelephonyClient;
    
    /**
     * Central repository access needed for parental control setting.
     * Own.
     */
    CRepository* iCenRep;
    
    };

#endif // VCXHGVODMAINVIEW_H
