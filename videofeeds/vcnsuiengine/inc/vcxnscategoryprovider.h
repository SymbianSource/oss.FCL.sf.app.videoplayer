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




#ifndef C_VCXNSCATEGORYPROVIDER_H
#define C_VCXNSCATEGORYPROVIDER_H

// INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"
#include "vcxnscategoryproviderif.h"
#include "vcxnscategoryproviderobserver.h"

// FORWARD DECLARATIONS
class CVcxNsCategory;
class CVcxNsCategoryList;
class MVcxNsCategoryProviderObserver;
class CVcxNsUiEngine;

// CLASS DECLARATION

/**
* Class CVcxNsCategoryProvider handles application categorys.
*
* @lib vcxnsuiengine.lib
*/
class CVcxNsCategoryProvider : public CBase,
                               public MVcxNsCategoryProviderIf   
    {

public:
    
    /**
     * Constructor
     * 
     * @param aUiEngine Parent ui engine ptr.
     */
    static CVcxNsCategoryProvider* NewL( CVcxNsUiEngine& aUiEngine );

    /**
     * Destructor
     */
    virtual ~CVcxNsCategoryProvider();

    /**
     * RegisterObserver.
     * 
     * @param aObserver Observer
     */
    void RegisterObserver( MVcxNsCategoryProviderObserver* aObserver );

    /**
     * DeRegisterObserver.
     * 
     * @param aObserver Observer
     */
    void DeRegisterObserver( MVcxNsCategoryProviderObserver* aObserver );

    /**
     * Get categories for active service.
     * @param aHighlight On return, hightlight index for category list.
     * @return Category array. Ownership of categories remains in ui engine.
     */
    RPointerArray<CVcxNsCategory>& GetCategoriesL( TInt& aHighlight );
    
    /**
     * Get categories for active service.
     * @return Vod categorys.
     */
    RPointerArray<CVcxNsCategory>& GetCategorysL( );

    /**
     * Opens selected category.
     *
     * @param aIndex Index of the category to be opened.
     */
    void OpenCategoryL( TInt aIndex );
      
    /**
     * Return pointer to active category data
     * @return Pointer to active category data.
     *         NULL returned, if category data not found
     */
    CVcxNsCategory* GetActiveCategoryDataL();
    
    /**
     * Return specified category data.
     * @param aServiceId  Service Id
     * @param aCategoryId Category Id
     * @return Pointer to category data.
     *         NULL returned, if specified category data not found
     */
    CVcxNsCategory* GetCategoryDataL( TUint32 aServiceId, TUint32 aCategoryId );

    /**
     * Get highlight index for content list. 
     * @return Current highlight index in content list.
     */
    TInt GetCurrentHighlightL();
    
    /**
     * Set highlight index for content list. 
     * @param aIndex Index to save
     */
    void SetCurrentHighlightL( TInt aIndex );
    
    /**
     * Update active category data.
     * This method is needed when count
     * of videos in category changes (search).
     * 
     * @param aCategory Category data to updat  
     * 
     */
    void UpdateActiveCategoryDataL( CVcxNsCategory& aCategory );
    
    /**
     * Reset the categories with given service id.
     *
     * @param aServiceId Service id
     */
    void RemoveServiceData( TUint32 aServiceId );

    /**
     * Starts the category update.
     *
     * @return KErrNone or one of the system wide error codes.
     */
    TInt RefreshCategorysL();

// From MVcxNsCategoryProviderObserver

    /**
     * Category update notification.
     *
     * @param aIndex Updated item's index
     */
    void CategoryUpdated( TInt aIndex );
    
    /**
     * View needs to be refreshed.
     */
    void RefreshView();

    /**
     * Application state needs to be updated.
     */
    void HandleAppStateChangedL();

    /**
     * Epg update is ongoing, show the notification.
     * When downloading and parsing the xml, both parameters are 0.
     * 
     * @param aShow If ETrue, show the note, if EFalse, hide it.
     * @param aDownloadedTbns count of downloaded thumbnails.
     * @param aTotalTbns Total count of thumbnails to be downloaded.
     */
    void ShowUpdatingNoteL( TBool aShow, TInt aDownloadedTbns, TInt aTotalTbns );
    
    /**
     * Handle error. 
     *
     * @param aError Error code.
     * @param aServiceId Service ID 
     */
    void HandleErrorL( TInt aError, TUint32 aServiceId );
    
    /**
     * Save category highlight. 
     * @param aHighlight Highlight index to save.
     */
    void SetCategoryHighlight( TInt aHighlight );

private:
    
    /**
     * Default constructor.
     * @param aUiEngine
     */
    CVcxNsCategoryProvider( CVcxNsUiEngine& aUiEngine );

private:
    
    /**
     * Array of category event observers. Not Own.
     */
    RPointerArray<MVcxNsCategoryProviderObserver> iCategoryObservers;
    
    /**
     * UI engine.
     */
    CVcxNsUiEngine& iUiEngine;

    /**
     * Array of category lists.
     */
    RPointerArray<CVcxNsCategoryList> iCategoryListList;

    };

#endif // C_VCXNSCATEGORYPROVIDER_H
