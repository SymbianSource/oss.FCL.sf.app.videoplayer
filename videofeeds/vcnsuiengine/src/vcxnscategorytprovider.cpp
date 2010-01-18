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




// INCLUDE FILES
#include <bldvariant.hrh>
#include "CIptvUtil.h"
#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvVodContentClient.h"
#include "MIptvVodContentClientObserver.h"
#include "TIptvRssSearchQuery.h"
#include "vcxnscategoryproviderobserver.h"
#include "vcxnscontentclienthandler.h"
#include "vcxnscontentsearchhandler.h"
#include "vcxnscategoryprovider.h"
#include "vcxnsserviceprovider.h"
#include "vcxnsuiengine.h"
#include "vcxnscategory.h"
#include "vcxnsdatalist.h"

// CONSTS
const TInt KMaxCategoryHistory = 50; 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
CVcxNsCategoryProvider::CVcxNsCategoryProvider( CVcxNsUiEngine& aUiEngine ) : 
    iUiEngine( aUiEngine )
    {
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CVcxNsCategoryProvider* CVcxNsCategoryProvider::NewL( CVcxNsUiEngine& aUiEngine )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsCategoryProvider::NewL()");
    return new (ELeave) CVcxNsCategoryProvider( aUiEngine );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVcxNsCategoryProvider::~CVcxNsCategoryProvider()
    {
    iCategoryObservers.Reset();
    iCategoryListList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// Get categories from active service
// -----------------------------------------------------------------------------
//
RPointerArray<CVcxNsCategory>& CVcxNsCategoryProvider::GetCategoriesL( TInt& aHighlight )
    {
    if ( iUiEngine.ServiceProvider() )
        {
        aHighlight = iUiEngine.ServiceProvider()->GetCurrentHighlight();
        }
    else
        {
        aHighlight = 0;
        }

    return GetCategorysL(); 
    }

// -----------------------------------------------------------------------------
// Get categories from active service
// -----------------------------------------------------------------------------
//
RPointerArray<CVcxNsCategory>& CVcxNsCategoryProvider::GetCategorysL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsCategoryProvider::GetCategorysL");

    for( TInt k = 0; k < iCategoryListList.Count(); k++ )
        {
        if( (iCategoryListList[k])->iServiceId == iUiEngine.ActiveService() )
            {
            // Sort the data by use order
            CVcxNsCategoryList* list = iCategoryListList[k];
            if( k != iCategoryListList.Count()-1 )
                {
                iCategoryListList.Remove( k );
                iCategoryListList.Compress();
                iCategoryListList.Append( list );
                }
            return list->iArray;
            }
        }
    
    // Get the category data from DB
    RPointerArray<CIptvVodContentCategoryBriefDetails> catList;
    
    TInt error = iUiEngine.GetContentClientHandlerL()->
                     GetVodContentClientL( iUiEngine.ActiveService() )->
                         GetEcgCategoryListL( KIptvVodContentCategoryRootId, catList );
    
    
    // If cache is already full, remove the oldest data.
    if( iCategoryListList.Count() >= KMaxCategoryHistory )
        {
        delete iCategoryListList[0];
        iCategoryListList.Remove(0);
        iCategoryListList.Compress();
        }
    
    // Append the new data to cache
    CVcxNsCategoryList* entry = new (ELeave) CVcxNsCategoryList;
    entry->iServiceId = iUiEngine.ActiveService();
    iCategoryListList.AppendL( entry );
    RPointerArray<CVcxNsCategory>& categoryList = entry->iArray;
    
   // Translate data classes     
    if ( error == KErrNone )
        {
        CVcxNsCategory* category = NULL;
        
        for( TInt i = 0; i < catList.Count(); i++ )
            {
            category = CVcxNsCategory::NewL();
            CleanupStack::PushL( category );

            category->SetNameL( (catList[i])->iName );
            category->SetCategoryId( (catList[i])->iCategoryId );
            category->SetVideoCount( (catList[i])->iContentCount );
            
            if( category->GetCategoryId() == KIptvRssSearchCategoryId )
                {
                // Search is always first.
                categoryList.Insert( category, 0 ); 
                CleanupStack::Pop( category );                
                }
            else
                {
                categoryList.AppendL( category );
                CleanupStack::Pop( category );                
                }
            }
        }
    catList.ResetAndDestroy();

    return categoryList;
    }

// -----------------------------------------------------------------------------
// Open category
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::OpenCategoryL( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsCategoryProvider::OpenCategoryL");

    RPointerArray<CVcxNsCategory>* categoryList = &GetCategorysL();
    
    if( aIndex >= 0 && aIndex < (*categoryList).Count() )
        {
        CVcxNsCategory* categoryData = (*categoryList)[aIndex];

        if ( ( categoryData->GetCategoryId() == KIptvRssSearchCategoryId )
              && ( categoryData->GetVideoCount() <= 0 ) )
            {
            //Start new search
            iUiEngine.GetContentSearchHandlerL()->StartNewSearchL();
            }
        else
            {
            iUiEngine.SetActiveCategory ( categoryData->GetCategoryId() ); 
            iUiEngine.SetVcAppState( EStateContentView );
            }
        }
    }

// -----------------------------------------------------------------------------
// Get active category data
// -----------------------------------------------------------------------------
//
CVcxNsCategory* CVcxNsCategoryProvider::GetActiveCategoryDataL( )
    {
    TUint32 activeCategory = iUiEngine.ActiveCategory();
    
    //Get list of categories from active service
    RPointerArray<CVcxNsCategory>* categoryList = &GetCategorysL();
    
    for ( TInt i = 0; i < (*categoryList).Count(); i++ )
        {
        if ( (*categoryList)[i]->GetCategoryId() == activeCategory )
            {
            return (*categoryList)[i];
            }
        }

    return NULL;
    }


// -----------------------------------------------------------------------------
// Get category data
// -----------------------------------------------------------------------------
//
CVcxNsCategory* CVcxNsCategoryProvider::GetCategoryDataL( 
    TUint32 aServiceId, 
    TUint32 aCategoryId )
    {
    if ( aServiceId == iUiEngine.ActiveService() )
        {
        //Get list of categories from active service
        RPointerArray<CVcxNsCategory>* categoryList = &GetCategorysL();
        
        for ( TInt i = 0; i < (*categoryList).Count(); i++ )
            {
            if ( (*categoryList)[i]->GetCategoryId() == aCategoryId )
                {
                return (*categoryList)[i];
                }
            }

        return NULL;
        }
    else
        {
        // Check, if category data exists
        for( TInt k = 0; k < iCategoryListList.Count(); k++ )
            {
            if( ( iCategoryListList[k])->iServiceId == aServiceId )
                {
                // Sort the data by use order
                CVcxNsCategoryList* list = iCategoryListList[k];
                if ( list )
                    {
                    for ( TInt ind = 0; ind < list->iArray.Count(); ind++ )
                        {
                        if ( (list->iArray[ind])->GetCategoryId() == aCategoryId )
                            {
                            return list->iArray[ind];
                            }
                        }
                    }
                }
            }

        // GetCategorysL() includes same kind of implementation that follows,
        // Check and modify to one method
        
        // Get the category data from DB
        RPointerArray<CIptvVodContentCategoryBriefDetails> catList;
        
        TInt error = iUiEngine.GetContentClientHandlerL()->
                         GetVodContentClientL( aServiceId )->
                             GetEcgCategoryListL( KIptvVodContentCategoryRootId, catList );
        
        // If cache is already full, remove the oldest data.
        if( iCategoryListList.Count() >= KMaxCategoryHistory )
            {
            delete iCategoryListList[0];
            iCategoryListList.Remove(0);
            iCategoryListList.Compress();
            }
        
        // Append the new data to cache
        CVcxNsCategoryList* entry = new (ELeave) CVcxNsCategoryList;
        entry->iServiceId = aServiceId;
        iCategoryListList.AppendL( entry );
        RPointerArray<CVcxNsCategory>& categoryList = entry->iArray;
        
        CVcxNsCategory* toReturn = NULL;
       // Translate data classes     
        if ( error == KErrNone )
            {
            CVcxNsCategory* category = NULL;
            
            for( TInt i = 0; i < catList.Count(); i++ )
                {
                category = CVcxNsCategory::NewL();
                CleanupStack::PushL( category );

                category->SetNameL( (catList[i])->iName );
                category->SetCategoryId( (catList[i])->iCategoryId );
                category->SetVideoCount( (catList[i])->iContentCount );
                
                if( category->GetCategoryId() == KIptvRssSearchCategoryId )
                    {
                    // Search is always first.
                    categoryList.Insert( category, 0 ); 
                    CleanupStack::Pop( category );                
                    }
                else
                    {
                    categoryList.AppendL( category );
                    CleanupStack::Pop( category );                
                    }
                
                if ( category->GetCategoryId() == aCategoryId )
                    {
                    toReturn = category;
                    }
                }
            }
        catList.ResetAndDestroy();
        return toReturn;
        }

    }

// -----------------------------------------------------------------------------
// CVcxNsCategoryProvider::GetCurrentHighlight
// -----------------------------------------------------------------------------
//
TInt CVcxNsCategoryProvider::GetCurrentHighlightL()
    {
    CVcxNsCategory* category = GetActiveCategoryDataL();
    if ( category )
        {
        return category->HighlightContentIndex();
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CVcxNsCategoryProvider::SetCurrentHighlight
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::SetCurrentHighlightL( TInt aIndex )
    {
    CVcxNsCategory* category = GetActiveCategoryDataL();
    if ( category )
        {
        return category->SetHighlightContentIndex( aIndex );
        }
    }


// -----------------------------------------------------------------------------
// Update active category data
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::UpdateActiveCategoryDataL( 
   CVcxNsCategory& aCategory )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsCategoryProvider::UpdateActiveCategoryDataL");

    TUint32 activeCategory = iUiEngine.ActiveCategory();
    
    if ( activeCategory == aCategory.GetCategoryId() )
        {
        //Get list of categories from active service
        RPointerArray<CVcxNsCategory>* categoryList = &GetCategorysL();
    
        for ( TInt i = 0; i < (*categoryList).Count(); i++ )
            {
            if ( (*categoryList)[i]->GetCategoryId() == activeCategory )
                {
                (*categoryList)[i]->SetVideoCount( aCategory.GetVideoCount() );
                }
            }
        }
    }
// -----------------------------------------------------------------------------
// Remove category data from specified service
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::RemoveServiceData( TUint32 aServiceId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsCategoryProvider::RemoveServiceData");
    for( TInt k = 0; k < iCategoryListList.Count(); k++ )
        {
        if( (iCategoryListList[k])->iServiceId == aServiceId )
            {
            delete (iCategoryListList[k]);
            iCategoryListList.Remove( k );
            iCategoryListList.Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// Update active service
// -----------------------------------------------------------------------------
//
TInt CVcxNsCategoryProvider::RefreshCategorysL()
    {
    return iUiEngine.GetContentClientHandlerL()->UpdateEcgL( iUiEngine.ActiveService() );
    }

// -----------------------------------------------------------------------------
// Register observer
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::RegisterObserver( 
    MVcxNsCategoryProviderObserver* aObserver )
    {
    for ( TInt i = 0; i < iCategoryObservers.Count(); i++ )
        {
        if( iCategoryObservers[i] == aObserver )
            {
            return;
            }
        }
    
    iCategoryObservers.Append(aObserver);
    }

// -----------------------------------------------------------------------------
// Unregister observer
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::DeRegisterObserver( 
    MVcxNsCategoryProviderObserver* aObserver )
    {
    for ( TInt i = 0; i < iCategoryObservers.Count(); i++ )
        {
        if( iCategoryObservers[i] == aObserver )
            {
            iCategoryObservers.Remove( i );
            iCategoryObservers.Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsCategoryProvider::CategoryUpdated()
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::CategoryUpdated( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsCategoryProvider::CategoryUpdated");
    for ( TInt i = 0; i < iCategoryObservers.Count(); i++ )
        {
        if( iCategoryObservers[i] )
            {
            iCategoryObservers[i]->CategoryUpdated( aIndex );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsCategoryProvider::RefreshView()
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::RefreshView()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsCategoryProvider::RefreshView");
    for ( TInt i = 0; i < iCategoryObservers.Count(); i++ )
        {
        if( iCategoryObservers[i] )
            {
            iCategoryObservers[i]->RefreshView();
            }
        }   
    }

// -----------------------------------------------------------------------------
// CVcxNsCategoryProvider::HandleAppStateChangedL()
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::HandleAppStateChangedL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsCategoryProvider::HandleAppStateChangedL");
    for ( TInt i = 0; i < iCategoryObservers.Count(); i++ )
        {
        if( iCategoryObservers[i] )
            {
            iCategoryObservers[i]->HandleAppStateChangedL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsCategoryProvider::ShowUpdatingNoteL()
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::ShowUpdatingNoteL( TBool aShow, TInt aDownloadedTbns, TInt aTotalTbns )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsCategoryProvider::ShowUpdatingNote");
    for ( TInt i = 0; i < iCategoryObservers.Count(); i++ )
        {
        if( iCategoryObservers[i] )
            {
            iCategoryObservers[i]->ShowUpdatingNoteL( aShow, aDownloadedTbns, aTotalTbns );
            }
        }       
    }

// -----------------------------------------------------------------------------
// CVcxNsCategoryProvider::HandleErrorL()
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::HandleErrorL( 
    TInt aError,
    TUint32 aServiceId )
    {
    for ( TInt i = 0; i < iCategoryObservers.Count(); i++ )
        {
        if( iCategoryObservers[i] && 
              aServiceId == iUiEngine.ActiveService() )
            {
            iCategoryObservers[i]->HandleUpdateErrorL( aError );
            }
        }
    }

// -----------------------------------------------------------------------------
// Set highlighted category index for active service
// -----------------------------------------------------------------------------
//
void CVcxNsCategoryProvider::SetCategoryHighlight( TInt aHighlight )
    {
    if ( iUiEngine.ServiceProvider() )
        {
        if ( aHighlight >= 0 )
            {
            iUiEngine.ServiceProvider()->SetCurrentHighlight( aHighlight );
            }
        else
            {
            iUiEngine.ServiceProvider()->SetCurrentHighlight( 0 );
            }
        }
    }
