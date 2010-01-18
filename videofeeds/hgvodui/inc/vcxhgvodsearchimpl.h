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
* Description:    HG VOD content search related ui implementation*
*/




#ifndef VCXHGVODSEARCHIMPL_H
#define VCXHGVODSEARCHIMPL_H

#include <AknProgressDialog.h>   //Search progress
#include "vcxnscontentsearchif.h"
#include "vcxnscontentsearchobserver.h"

class CVcxNsUiEngine;
class CAknWaitDialog;
class CVcxHgVodMainView;

// CLASS DECLARATION
/**
 *  Content search ui implementation.
 *  This class handles search related functionality 
 *  in ui side. 
 *  For example search dialogs have been implemented here. 
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CVcxHgVodSearchImpl ) : public CBase, 
                                           public MProgressDialogCallback,
                                           public MVcxNsContentSearchObserver
    {

public:

    /**
     * Two-phased constructor.
     * @param aDataProviderIf Reference to UI engine
     * @return Pointer to newly created instance
     */
    static CVcxHgVodSearchImpl* NewL( 
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView );    
    /**
     * Two-phased constructor.
     * @param aDataProviderIf Reference to UI engine
     * @return Pointer to newly created instance
     */
    static CVcxHgVodSearchImpl* NewLC( 
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView );
    
    /**
    * Destructor.
    */
    virtual ~CVcxHgVodSearchImpl();
    
    void RegisterObserver();
    
    void DeRegisterObserver();

public:    

    /**
     * Show search query for user input. 
     * 
     * @param aShowPrevious If ETrue, stored old search query is copied 
     *                      to search query and displayed on data query
     *                      field as a default value. 
     *                      If EFalse, data query field is empty.
     */
    void ShowSearchQueryL( TBool aShowPrevious );
    
    /**
    * Display search wait note 
    */
    void DisplaySearchWaitNoteL();

    /**
    * Close search wait note 
    */
    void CloseSearchWaitNote();

public:
    
    /**
    * From MProgressDialogCallback
    * Called when wait dialog is dismissed.
    *
    * @param aButtonId 
    */
    void DialogDismissedL( TInt aButtonId );
    
public:
    
    /**
     * From MVcxNsContentSearchObserver
     * Handle search result.
     * This informs that search has been finished.
     * 
     * @param aSucceeded Informs the search result. 
     *        ETrue if search has been finished successfully. 
     *        EFalse if search has failed.
     */
    void HandleSearchResultL( TBool aSucceeded ); 
    
    /**
     * From MVcxNsContentSearchObserver
     * New search started.
     * This informs that search string has been sent to server and
     * search prosess has started.
     * 
     */
    void SearchStartedL();

    /**
     * From MVcxNsContentSearchObserver
     * Start search.
     * This informs that search can be started.
     * @param aUseCurrentQuery 
     *        ETrue, if IAP has been selected for search 
     *        and search can be started again with current query. 
     *        EFalse if new search query can be started. 
     *        Note: EFalse is always used when search category is opened from
     *        category view and there's no previous search results available.
     *
     */
    void StartSearchL( TBool aUseCurrentQuery );
    
private:    
    
    /**
     * Default constructor
     * @param aDataProviderIf Reference to ui engine interface
     * @param aView Reference to view
     */
    CVcxHgVodSearchImpl( 
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView );

    /**
     * 
     * @param aDataProviderIf
     */
    void ConstructL( );
    
private: // data

    /**
     * Reference to ui engine interface 
     */
    CVcxNsUiEngine& iUiEngine;

    /**
     * Reference to view. 
     */
    CVcxHgVodMainView& iView;
    
    /**
     * Current search query.
     * Own.
     */
    HBufC* iCurrentSearchQuery;
    
    /**
    * Wait note during video search
    */
    CAknWaitDialog* iSearchWaitDialog;
    
    /**
     * Pointer to search interface in ui engine. 
     * Not own.
     */
    MVcxNsContentSearchIf* iSearchHandler;
    };


#endif // VCXHGVODSEARCHIMPL_H
