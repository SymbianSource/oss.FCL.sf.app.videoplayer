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
* Description:    HG VOD service group view functionality implementation*
*/




#ifndef VCXHGVODSERVICEGROUPLISTIMPL_H_
#define VCXHGVODSERVICEGROUPLISTIMPL_H_

#include "vcxhgvodlistimplbase.h"
#include "vcxhgvodmainviewcontainer.h"

class CHgScroller;
class CVcxHgVodMainView;
class CVcxHgVodMainViewContainer;
class CVcxHgVodServiceGroupListModelHandler;
class CVcxNsUiEngine;

// CLASS DECLARATION
/**
 *  Service group list implementation
 *  This class handles service group view list functionality.
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CVcxHgVodServiceGroupListImpl ): public CVcxHgVodListImplBase 
    {

public:

    /**
     * Two-phased constructor.
     * @param aScroller UI component responsible of drawing the information
     * @param aDataProviderIf Reference to UI engine
     * @param aView Reference to main view
     * @return Pointer to newly created instance
     */
    static CVcxHgVodServiceGroupListImpl* NewL(
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
    static CVcxHgVodServiceGroupListImpl* NewLC( 
            CHgScroller& aScroller, 
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView );

    /**
    * Destructor.
    */
    virtual ~CVcxHgVodServiceGroupListImpl();

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
     * @param aCommand 
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
     * Handles item opening. 
     */
    void HandleOpenL( TInt aIndex );
    
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
     * Update title pane text for service group view.
     */
    void UpdateTitlePaneL();

private:    
    
    /**
     * Default constructor
     * @param aScroller UI component responsible of drawing the information
     * @param aView Main view
     */
    CVcxHgVodServiceGroupListImpl( CHgScroller& aScroller,
                              CVcxNsUiEngine& aDataProviderIf,
                              CVcxHgVodMainView& aView );

    /**
     * 
     * @param aDataProviderIf
     * @param aScroller UI component responsible of drawing the information
     */
    void ConstructL( CVcxNsUiEngine& aDataProviderIf );
    
private: // data

    /**
     * List widget model handler
     * Own.
     */
    CVcxHgVodServiceGroupListModelHandler* iModelHandler;
    };

#endif /*VCXHGVODSERVICEGROUPLISTIMPL_H_*/
