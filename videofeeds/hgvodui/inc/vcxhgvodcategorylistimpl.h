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
* Description:    HG VOD category view functionality implementation*
*/




#ifndef VCXHGVODCATEGORYLISTIMPL_H
#define VCXHGVODCATEGORYLISTIMPL_H

#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodlistimplbase.h"

class CHgScroller;
class CVcxHgVodMainView;
class CVcxHgVodMainViewContainer;
class CVcxHgVodCategoryListModelHandler;
class CVcxNsUiEngine;

// CLASS DECLARATION
/**
 *  Category list implementation
 *  This class handles service view list functionality.
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
class CVcxHgVodCategoryListImpl : public CVcxHgVodListImplBase
    {

public:

    /**
     * Two-phased constructor.
     * @param aScroller UI component responsible of drawing the information
     * @param aDataProviderIf Reference to UI engine
     * @param aView Reference to main view
     * @return Pointer to newly created instance
     */
    static CVcxHgVodCategoryListImpl* NewL(
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
    static CVcxHgVodCategoryListImpl* NewLC( 
            CHgScroller& aScroller,
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView );

    /**
    * Destructor.
    */
    virtual ~CVcxHgVodCategoryListImpl();
    
    /**
     * Activate service model
     */
    void DoActivateL( );

    /**
     * Deactivate service model
     *
     */
    void DoDeactivate();

    /**
     * Handle user command
     */
    void HandleCommandL( TInt aCommand );
    
    /**
     * Activate menu pane items before displaying menu
     * 
     * @param aMenuPane Pointer to menu pane
     *
     */
    void ActivateMenuPaneItemsL( CEikMenuPane* aMenuPane, TInt aResourceId );

    /**
     * Called when item in list is opened.
     * 
     * @param aIndex Index of the item opened.
     */
    void HandleOpenL( TInt aIndex );
    
    /**
    * Get help context.
    *
    * @param aContext context
    */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

    /**
     * Update title pane text for category view.
     */
    void UpdateTitlePaneL();

private:    
    
    /**
     * Default constructor
     * @param aScroller UI component responsible of drawing the information
     * @param aDataProviderIf Ref to ui engine interface
     * @param aView Main view
     */
    CVcxHgVodCategoryListImpl( CHgScroller& aScroller,
                               CVcxNsUiEngine& aDataProviderIf,
                               CVcxHgVodMainView& aView );

    /**
     * 
     * @param aDataProviderIf
     */
    void ConstructL( 
            CVcxNsUiEngine& aDataProviderIf, 
            CVcxHgVodMainView& aView  );

private: // data

    /**
     * List widget model handler
     * Own.
     */
    CVcxHgVodCategoryListModelHandler* iModelHandler;
    };

#endif // VCXHGVODCATEGORYLISTIMPL_H
