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




#ifndef C_VCXNSVODCATEGORY_H
#define C_VCXNSVODCATEGORY_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 * Category data class.
 * Holds category information.
 * 
 * @lib vcxnsuiengine.lib
 */
class CVcxNsCategory : public CBase
    {
public:

    /**
     * Constructor.
     * @return Pointer to created CVcxNsCategory object. 
     */
    static CVcxNsCategory* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CVcxNsCategory();
    
    /**
     * Get category name.
     * @return Category name.
     */
    IMPORT_C const TDesC& GetName();

    /**
     * Get category id
     * @return Category id
     */
    IMPORT_C TUint32 GetCategoryId();
 
    /**
     * Get category's video count.
     * @return Video count.
     */
    IMPORT_C TInt GetVideoCount();
    
    /**
     * Set category name.
     * @param aName Category name
     */
    void SetNameL( const TDesC& aName );
    
    /**
     * Set category id for category.
     * @param aCategoryId Category id.
     */
    void SetCategoryId( const TUint32 aCategoryId );
    
    /**
     * Set video count for category.
     * @param aVideoCount Video count.
     */
    void SetVideoCount( const TInt aVideoCount );
    
    /**
     * Set video count for category.
     * @param aIndex Content list highlight index
     */
    void SetHighlightContentIndex( TInt aIndex );
    
    /**
     * Get content list index for highlighting 
     * item in content list in category. 
     * @return Content list index. If not set, always 0 by default.
     */
    TInt HighlightContentIndex();

private:

    /**
     * Default constructor
     */
    CVcxNsCategory( );

private:

    /**
     * Category name.
     * Own.
     */
    HBufC* iName;

    /**
     * Category id.
     */
    TUint32 iCategoryId;
    
    /**
     * Video count in category.
     */    
    TInt iVideoCount;
    
    /**
     * Content index for category in content list.
     */ 
    TInt iContentIndex;
    
    };

#endif // C_VCXNSVODCATEGORY_H
