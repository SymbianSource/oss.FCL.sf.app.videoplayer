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




#ifndef M_VCXNSAPPCATEGORYPROVIDERIF_H
#define M_VCXNSAPPCATEGORYPROVIDERIF_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class MVcxNsCategoryProviderObserver;
class CVcxNsCategory;

/**
* Class MVcxNsCategoryProviderIf 
* Interface for using UI engine's category provider
*
* @lib vcxnsuiengine.lib
*/
class MVcxNsCategoryProviderIf
    {

public:

    /**
     * RegisterObserver.
     * @param aObserver Observer
     */
    virtual void RegisterObserver( MVcxNsCategoryProviderObserver* aObserver ) = 0;

    /**
     * DeRegisterObserver.
     * @param aObserver Observer
     */
    virtual void DeRegisterObserver( MVcxNsCategoryProviderObserver* aObserver ) = 0;

    /**
     * Get categories for active service.
     * @param aHighlight On return, hightlight index for category list.
     * @return Category array. Ownership of categories remains in ui engine.
     */
    virtual RPointerArray<CVcxNsCategory>& GetCategoriesL( TInt& aHighlight ) = 0;

    /**
     * Get categories for active service.
     * @return Vod categorys.
     */
    virtual RPointerArray<CVcxNsCategory>& GetCategorysL() = 0;
    
    /**
     * Opens selected category.
     *
     * @param aIndex Index of the category to be opened.
     */
    virtual void OpenCategoryL( TInt aIndex ) = 0 ;
    
    /**
     * Return pointer to active category data
     * @return Pointer to active category data.
     *         NULL returned, if category data not found
     */
    virtual CVcxNsCategory* GetActiveCategoryDataL() = 0;
    
    /**
     * Return specified category data.
     * @param aServiceId  Service Id
     * @param aCategoryId Category Id
     * @return Pointer to category data.
     *         NULL returned, if specified category data not found
     */
    virtual CVcxNsCategory* GetCategoryDataL( TUint32 aServiceId, TUint32 aCategoryId  ) = 0;

    /**
     * Starts the category update.
     *
     * @return KErrNone or one of the system wide error codes.
     */
    virtual TInt RefreshCategorysL() = 0;
    
    /**
     * Save category highlight. 
     * @param aHighlight Highlight index to save.
     */
    virtual void SetCategoryHighlight( TInt aHighlight ) = 0;
    
    };

#endif // MVCXNSAPPCATEGORYPROVIDERIF_H
