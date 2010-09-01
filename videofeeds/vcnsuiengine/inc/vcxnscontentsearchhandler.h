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




#ifndef VCXNSCONTENTSEARCHHANDLER_H
#define VCXNSCONTENTSEARCHHANDLER_H

// INCLUDES
#include <e32base.h>

#include "vcxnscontentsearchif.h"


// FORWARD DECLARATIONS
class CVcxNsUiEngine;
class MVcxNsContentSearchObserver;
// CLASS DECLARATION

/**
* Class CVcxNsCategoryProvider handles application categorys.
*
* @lib vcxnsuiengine.lib
*/
class CVcxNsContentSearchHandler: public CBase,
                                  public MVcxNsContentSearchIf   
    {

public:

    /**
     * Constructor
     * @param aUiEngine Ui engine
     */
    static CVcxNsContentSearchHandler* NewL( CVcxNsUiEngine& aUiEngine );

    virtual ~CVcxNsContentSearchHandler();

public: 
    
    /**
     * Start new search. This is only for UI engines internal use. 
     * When search category is opened and there's no contents,
     * we can start new search. 
     */
    void StartNewSearchL( );
    
public: //From MVcxNsContentSearchIf

    /**
     * RegisterObserver.
     * @param aObserver Observer
     */
    void RegisterObserver( MVcxNsContentSearchObserver* aObserver );

    /**
     * DeRegisterObserver.
     * @param aObserver Observer
     */
    void DeRegisterObserver( MVcxNsContentSearchObserver* aObserver );
    
    /**
     * Starts the search with given keyword.
     * @param aSearchString Search string
     */
    void SearchL( const TDesC& aSearchString );
    
    /**
     * Cancel ongoing search.
     */
    void CancelSearchL( );
    
    /**
     * Handle search related status messages.
     * @param aStatus Search status.
     * @param aInfo Information parameter.
     */
    void HandleSearchMsgL( TInt aStatus, TInt aInfo );
    
    /**
     * Handle search cancelled by user.
     */
    void SearchCancelled();

private:
     
    /**
     * Default constructor
     * @param aUiEngine Ui engine
     */
    CVcxNsContentSearchHandler( CVcxNsUiEngine& aUiEngine );
    
    /**
     * Update category video count.
     */
    void UpdateCategoryVideoCountL();

    /**
     * Shows or hides the popup about thumbnail update.
     * @param aVisible Show or hide pop-up.
     */
    void ShowInfoPopupL( TBool aVisible );

// Data

    /**
     * Array of search observers. Not Own.
     */
    RPointerArray<MVcxNsContentSearchObserver> iSearchObservers;
    
    /**
     * UI engine.
     */
    CVcxNsUiEngine& iUiEngine;

    /**
     * Total count of thumbnails in the search result.
     */
    TInt iThumbnailCount;
    
    /**
     * Count of thumbnails that has already been downloaded.
     */
    TInt iThumbnailsDownloaded;

    };

#endif // VCXNSCONTENTSEARCHHANDLER_H
