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
* Description: 
*
*/




#ifndef MVCXNSCONTENTSEARCHOBSERVER_H
#define MVCXNSCONTENTSEARCHOBSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 *  MVcxNsContentSearchObserver
 * 
 *  @lib vcxnsuiengine.lib
 */
class MVcxNsContentSearchObserver
    {
   
public:

    /**
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
    virtual void StartSearchL( TBool aUseCurrentQuery ) = 0;
    
    /**
     * New search started.
     * This informs that search string has been sent to server and
     * search prosess has started.
     *
     */
    virtual void SearchStartedL() = 0;   

    /**
     * Handle search result.
     * This informs that search has been finished.
     * 
     * @param aSucceeded Informs the search result. 
     *        ETrue if search has been finished successfully. 
     *        EFalse if search has failed.
     * 
     */
    virtual void HandleSearchResultL( TBool aSucceeded ) = 0;

    };

#endif // MVCXNSCONTENTSEARCHOBSERVER_H
