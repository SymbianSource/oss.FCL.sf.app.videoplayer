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


// Version : %version: 5 %

#ifndef VCXHGVODSERVICELISTIMPL_H
#define VCXHGVODSERVICELISTIMPL_H

#include "vcxhgvodlistimplbase.h"
#include "vcxhgvodmainviewcontainer.h"

class CHgScroller;
class CVcxHgVodMainView;
class CVcxHgVodMainViewContainer;
class CVcxHgVodServiceListModelHandler;
class CVcxNsUiEngine;


// CLASS DECLARATION
/**
 *  Service list implementation
 *  This class handles service view list functionality.
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
class CVcxHgVodServiceListImpl : public CVcxHgVodListImplBase 
    {

public:

    /**
     * Two-phased constructor.
     * @param aScroller UI component responsible of drawing the information
     * @param aDataProviderIf Reference to UI engine
     * @param aView Reference to main view
     * @return Pointer to newly created instance
     */
    static CVcxHgVodServiceListImpl* NewL(
            CHgScroller& aScroller, 
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView );
    
    /**
     * Two-phased constructor.
     * @param aScroller UI component responsible of drawing the information
     * @param aDataProviderIf Reference to UI engine
     * @param aView Reference to main view
     * @return Pointer to newly created instance
     */
    static CVcxHgVodServiceListImpl* NewLC( 
            CHgScroller& aScroller, 
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView );

    /**
    * Destructor.
    */
    virtual ~CVcxHgVodServiceListImpl();

    /**
     * Activate menu pane items before displaying menu
     * 
     * @param aMenuPane Pointer to menu pane
     *
     */
    void ActivateMenuPaneItemsL( CEikMenuPane* aMenuPane, TInt aResourceId );
    
public: //From CVcxHgVodListImplBase
    
    /**
     * Handle user command
     * @param aCommand Command to handle
     */
    void HandleCommandL( TInt aCommand );
    
    /**
    * Activate model
    * 
    */
    void DoActivateL();
    
    /**
    * Deactivate model
    * 
    */
    void DoDeactivate();
    
    /**
    * HandleKeyEventL 
    */
    TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    
    /**
     * Handles item opening. 
     */
    void HandleOpenL( TInt aIndex );
    
    /**
     * Handles selection changes.
     */
    void HandleSelectL( TInt aIndex );

    /**
    * CheckSoftkeysL 
    */
    void CheckSoftkeysL();

    /**
    * Get help context.
    *
    * @param aContext context
    */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

    /**
     * Update title pane text for services view.
     */
    void UpdateTitlePaneL();


public:
    
    /**
     * Refresh view contents 
     *
     */
    void Refresh();
    
private:    
    
    /**
     * Default constructor
     * @param aListWidget List widget
     * @param aView Main view
     */
    CVcxHgVodServiceListImpl( CHgScroller& aScroller,
                              CVcxNsUiEngine& aDataProviderIf,
                              CVcxHgVodMainView& aView );

    /**
     * 
     * @param aDataProviderIf
     * @param aAlfEnv
     */
    void ConstructL( CVcxNsUiEngine& aDataProviderIf );
    
    /**
     * Check if list item is removable.
     * @param aIndex
     * @return ETrue if feed can be removed,
     * otherwise EFalse
     */
    TBool FeedRemovable( TInt aIndex ); 
    
    /**
     * Remove feed if user accepts.
     * @param aIndex Indexfrom where feed is removed
     *
     */
    void RemoveFeedL( TInt aIndex );

    /**
     * Check if some feed has been selected for moving
     * @return ETrue if move operation is unfinished,
     *         otherwise EFalse.
     */
    TBool IsMoving() const;

    /**
     * Start moving feed.
     */
    void StartMoveL();

    /**
     * Stop moving. Move item to selected index.
     * @param aSave ETrue if item is moved to new position in list
     *              EFalse if move operation is cancelled.
     */
    void StopMovingL( TBool aSave );
    
    /**
     * Check, is there any video services.
     * @return Is there any video services,
     *         ETrue if there's at least one.
     */
    TBool HasVideoServicesL();

private: // data

    /**
     * List widget model handler
     * Own.
     */
    CVcxHgVodServiceListModelHandler* iModelHandler;
    
    /**
     * Flag that indicates is a service being moved.
     */
    TBool iMoving;

    /**
     * Source item index for move operation.
     */
    TInt iSource;

    /**
     * Target item index for move operation.
     */
    TInt iTarget;
    
    };

#endif // VCXHGVODSERVICELISTIMPL_H
