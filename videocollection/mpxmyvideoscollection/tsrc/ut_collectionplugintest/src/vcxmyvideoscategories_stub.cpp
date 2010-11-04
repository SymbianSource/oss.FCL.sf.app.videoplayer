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
* Description:    Categories related functionality*
*/




// INCLUDE FILES
#include <mpxmediageneraldefs.h>
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideoscategories_stub.h"
#include "vcxmyvideoscollectionutil.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCategories* CVcxMyVideosCategories::NewL(
    CVcxMyVideosCollectionPlugin& aCollection )
    {
    CVcxMyVideosCategories* self = new (ELeave) CVcxMyVideosCategories( aCollection );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCategories::~CVcxMyVideosCategories()
    {
    delete iList;
    iModifiedCategories.Close();
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCategories::CVcxMyVideosCategories( CVcxMyVideosCollectionPlugin& aCollection )
: iCollection( aCollection )
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::ConstructL ()
    {
    }
    
// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCategories::CreateCategoryListL
// ----------------------------------------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosCategories::CreateCategoryListL()
    {
    return NULL;
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCategories::AppendCategoryToArrayL
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosCategories::AppendCategoryToArrayL( const TDesC& /*aTitle*/, TMPXItemId /*aId*/,
        CMPXMediaArray& /*aCategoryArray*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoriesL
// Called by iCollection when fetching the video list from mds incrementally.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateCategoriesL( CMPXMedia& /*aVideoList*/,
        TInt /*aNewItemsStartIndex*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateVideosCountL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateVideosCountL( CMPXMediaArray& /*aCategoryArray*/,
    RArray<TInt>& /*aVideosIncrements*/, RArray<TInt>& /*aNewVideosIncrements*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateVideosCountL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateVideosCountL( CMPXMedia& /*aCategory*/,
    TInt /*aIncrement*/, TMPXAttributeData /*aCountAttribute*/, TInt /*aEventInfo*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideosAddedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideosAddedL( RArray<TUint32>& /*aIds*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideosRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideosRemovedL( RArray<TUint32>& /*aIds*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoAddedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoAddedL( TUint32 /*aMdsId*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoAddedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoAddedL( CMPXMedia& /*aVideo*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoRemovedL( TUint32 /*aMdsId*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoRemovedL( CMPXMedia& /*aVideo*/ )
    {
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideosAddedOrRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideosAddedOrRemovedL(
        RArray<TUint32>& /*aIds*/, TBool /*aAdded*/ )
    {
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoAddedOrRemovedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoAddedOrRemovedL( TUint32 /*aMdsId*/, TBool /*aAdded*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::VideoAddedOrRemovedL
// All remove&add calls eventually come here.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::VideoAddedOrRemovedL( CMPXMedia& /*aVideo*/, TBool /*aAdded*/ )
    {
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoryL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateCategoryL( CMPXMedia& /*aCategory*/, TInt /*aCategoryIndex*/,
        CMPXMedia& /*aVideo*/, TBool /*aAdded*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::AddToTUint32AttributeL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::AddToTUint32AttributeL( CMPXMedia& /*aCategory*/, TInt /*aAddition*/,
        TMPXAttributeData /*aAttribute*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::CategoryIndex
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCategories::CategoryIndex( TInt /*aOrigin*/ )
    {
    return 0;
    }     

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::NewVideoFlagChangedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::NewVideoFlagChangedL( TUint32 /*aOldFlag*/, TUint32 /*aNewFlag*/,
        TUint8 /*aOrigin*/, TBool& /*aModified*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::ResetVideoCountersL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::ResetVideoCountersL()
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoriesNewVideoNamesL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateCategoriesNewVideoNamesL()
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoryNewVideoNameAndDateL
// ----------------------------------------------------------------------------
//    
void CVcxMyVideosCategories::UpdateCategoryNewVideoNameAndDateL( TInt /*aOrigin*/, TBool& /*aModified*/,
        RArray<TUint32>& /*aIgnoredIds*/ )
    {   
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::CalculateNewVideoNameL
// ----------------------------------------------------------------------------
//    
const TDesC& CVcxMyVideosCategories::CalculateNewVideoNameL( CMPXMedia& /*aVideoList*/,
        TInt /*aOrigin*/, RArray<TUint32>& /*aIgnoredIds*/, CMPXMedia*& /*aNewVideo*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::UpdateCategoryNewVideoNameAndDateL
// Called when video is updated in a way that it affects category new
// video name.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::UpdateCategoryNewVideoNameAndDateL( CMPXMedia& /*aVideo*/,
        TBool& /*aModified*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::GenerateCategoryModifiedEventL
// Called when video is updated in a way that it modifies categories in a way
// that event is sent.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::GenerateCategoryModifiedEventL( CMPXMedia& /*aMedia*/,
        TBool /*aFlushMessageList*/, TInt32 /*aExtraInfo*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::OriginChangedL
// Called when videos origin is changed (Video moves from one category to other).
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::OriginChangedL( CMPXMedia& /*aVideo*/, TUint8 /*aPrevOrigin*/,
        TUint8 /*aNewOrigin*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::ResetModifiedCategoriesArray
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::ResetModifiedCategoriesArray()
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCategories::AddEventsFromModifiedCategoriesArrayL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCategories::AddEventsFromModifiedCategoriesArrayL()
    {
    }

// End of file

