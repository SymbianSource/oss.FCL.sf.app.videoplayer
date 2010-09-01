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
* Description:    HG VOD service view functionality implementation*
*/



#ifndef VCXHGVODLISTIMPLBASE_H_
#define VCXHGVODLISTIMPLBASE_H_

#include <AknServerApp.h>
#include <e32base.h>
#include <ganes/HgSelectionObserverIface.h>

class CHgScroller;
class CEikMenuPane;
class CBrowserLauncher;
class CVcxHgVodMainView;
class CVcxNsUiEngine;
class CVcxNsContent;
class CVcxHgVodInformationPopup;
class CVcxHgVodSearchImpl;

// CLASS DECLARATION
/**
 *  List implementation base class
 *  
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CVcxHgVodListImplBase ): public CBase, 
                                            public MAknServerAppExitObserver,
                                            public MHgSelectionObserver
    {

public:

    /**
     * Default constructor
     * @param aView
     * @param aModel
     * @param aScroller Ownership is transferred.
     */
    CVcxHgVodListImplBase( 
        CVcxHgVodMainView& aView, 
        CVcxNsUiEngine& aModel,
        CHgScroller& aScroller );
    
    /** 
     * Standard destructor.
     */ 
    virtual ~CVcxHgVodListImplBase();

    /**
     * Activate menu pane items before displaying menu
     * 
     * @param aMenuPane Pointer to menu pane
     * @param aResourceId 
     */
    virtual void ActivateMenuPaneItemsL( 
            CEikMenuPane* aMenuPane,
            TInt aResourceId ) = 0;

    /**
     * Handle user command
     * @param aCommand 
     */
    virtual void HandleCommandL( TInt aCommand ) = 0;
    
    /**
    * Activate list
    * 
    */
    virtual void DoActivateL() = 0;
    
    /**
    * Deactivate list
    * 
    */
    virtual void DoDeactivate() = 0;
    
    /**
     * Accessor for view container
     * 
     */
    void OpenEmbeddedBrowserL( const TDesC& aUri );
    
    /**
    * Close embedded browser.
    */
    void CloseEmbeddedBrowser();

    /**
    * From MAknServerAppExitObserver
    * Handle the exit of a connected server app.
    * This implementation provides Series 60 default behavior
    * for handling of the EAknCmdExit exit code. Derived classes
    * should base-call this implementation if they override this
    * function.
    */
    void HandleServerAppExit( TInt aReason ); 
    
    /**
     * Set title pane text.
     * @param aTitle Text to be shown in title pane
     */
    void SetTitlePaneTextL( const TDesC& aTitle );
    
    /**
     * Set title pane text from resources.
     * @param aResourceId Text resource id
     */
    void SetTitlePaneTextFromResourceL( TInt aResourceId );

    /**
     * Set empty list widget text
     * @param aResourceId Text resource id
     */
    void SetEmptyListTextL( TInt aResourceId );
    
    /**
     * Set menu’s resource ID
     * @param aResourceId Menu resource id
     */
    void SetMenuResourceId( TInt aResourceId );
    
    /**
    * Show information popup  
    *
    */      
    void ShowInformationPopupL( TBool aShow, TInt aDownloadedTbns, TInt aTotalTbns );
    
    /**
     * Layout/orientation has been changed.
     */
    virtual void HandleSizeChange();
    
    /**
     * Handles the foreground event.
     * @param aForeground Is foreground state gained or not.
     */
    void HandleForegroundEventL( TBool aForeground );
    
    /**
    * Get pointer to search implementation.
    * 
    * @return Pointer to search implementation 
    */              
    CVcxHgVodSearchImpl* SearchImpl();

    /**
    * Show update error note.
    * 
    * @param aError Error code
    */              
    void ShowUpdateErrorNoteL( TInt aError );
    
    /**
    * Show authentication dialog.
    * 
    * @param aEpgUpdateFailed ETrue if handling service authentication,
    *                         EFalse if handling download authentication 
    * @param aContent Pointer to content data in case of download authentication
    * Leaves with KErrCancel if user cancelled authentication query.
    */       
    void HandleAuthenticationL( 
            TBool aEpgUpdateFailed,
            CVcxNsContent* aContent = NULL );
    
    /**
    * Show error note 
    *
    * @param aErrorText Text in error dialog
    * @param aWaiting If ETrue, waiting dialog, EFalse by default.
    */
    void ShowErrorNoteL( const TDesC& aErrorText, TBool aWaiting = EFalse );
    
    /**
    * Set msk resource.
    *  
    */
    void SetMskL( TInt aResourceId );
    
    /**
    * Sets scroller to have the keyboard focus.
    * 
    * @param aFocus ETrue sets the control as having keyboard focus, 
    *               EFalse sets it as not having keyboard focus.
    */
    void SetFocus( TBool aFocus );
    
    /**
     * HandleKeyEventL
     */
    virtual TKeyResponse HandleKeyEventL( const TKeyEvent& aEvent, TEventCode aType );

    /**
    * Get help context.
    *
    * @param aContext context
    */
    virtual void GetHelpContext( TCoeHelpContext& aContext ) const = 0;
    
    /**
     * Update title pane
     */
    virtual void UpdateTitlePaneL() = 0;

// from MHgSelectionObserver
    
    /**
     * From MHgSelectionObserver
     * Handle selection of list item.
     * 
     * @param aIndex Selected item index.
     */
    virtual void HandleSelectL( TInt aIndex );

    /**
     * From MHgSelectionObserver
     * Handle opening of list item.
     * 
     * @param aIndex Opened item index.
     */
    virtual void HandleOpenL( TInt aIndex );

protected:
    
    /**
    * Initialize search implementation.
    * If does not exist, create. 
    */              
    void InitSearchImplL();
    
private:

    /**
    * Show information popup  
    *
    */      
    void ShowInformationPopupL();
    
    /**
    * Set text to information popup
    *
    * @param aInformationText Text for popup
    */      
    void SetInfoPopupTextL( const TDesC& aInformationText );
    
    /**
    * Hide information popup
    *
    */              
    void HideInformationPopup( );
    
protected:    

    /*
    * Reference to view 
    */
    CVcxHgVodMainView& iView;
    
    /*
    * Reference to ui engine interface 
    */
    CVcxNsUiEngine& iModel;
    
    /**
     * Reference to the UI component responsible of drawing the information
     */
    CHgScroller& iScroller;
    
    /**
     * Pointer to search implementation 
     * Own.
     */
    CVcxHgVodSearchImpl* iSearchImpl;
    
    /**
     * MSK resource currently in use
     *  
     */
    TInt iCurrentMskResource;

private:
    
    /*
    * Launcher that handles opening of web link in embedded browser
    * Own.
    */
    CBrowserLauncher* iLauncher;
    
    /**
     * Info popup.
     * Own. 
     */
    CVcxHgVodInformationPopup* iInformationPopup;
    
    };

#endif /*VCXHGVODLISTIMPLBASE_H_*/
