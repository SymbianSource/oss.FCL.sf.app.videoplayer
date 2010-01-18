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




#ifndef C_VCXNSARRAY_H
#define C_VCXNSARRAY_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CVcxNsContent;
class CVcxNsCategory;

// CLASS DECLARATION

/**
 * CVcxNsContentList 
 * 
 * Array class for CVcxNsContent.
 * 
 * @lib vcxnsuiengine.lib
 */

class CVcxNsContentList : public CBase
    {
    
public:

    virtual ~CVcxNsContentList();
    
    /**
     * Content list
     */
    RPointerArray<CVcxNsContent> iArray; 
    
    /**
     * iServiceId
     */
    TUint32 iServiceId;
    
    /**
     * iCategoryId
     */
    TUint32 iCategoryId;
    
    };

/**
 * CVcxNsCategoryList 
 * 
 * Array class for CVcxNsCategory.
 * 
 * @lib vcxnsuiengine.lib
 */

class CVcxNsCategoryList : public CBase
    {
    
public:

    virtual ~CVcxNsCategoryList();
    
    /**
     * Content list
     */
    RPointerArray<CVcxNsCategory> iArray;
    
    /**
     * iServiceId
     */
    TUint32 iServiceId;
    
    };

#endif // C_VCXNSARRAY_H
