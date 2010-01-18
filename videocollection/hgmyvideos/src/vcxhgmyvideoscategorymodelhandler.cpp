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
* Description:      UI level handler for category data.*
*/




// INCLUDE FILES
#include <StringLoader.h>

#include <mpxmediageneraldefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>

#include <ganes/HgScroller.h>
#include <ganes/HgItem.h>

#include <vcxhgmyvideos.rsg>
#include <vcxmyvideosdefs.h>
#include "IptvDebug.h"

#include "vcxhgmyvideoscategorymodelhandler.h"
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideoscollectionclient.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::CVcxHgMyVideosCategoryModelHandler()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryModelHandler::CVcxHgMyVideosCategoryModelHandler(
        CVcxHgMyVideosModel& aModel, CHgScroller& aScroller )
    : iModel( aModel ),
      iScroller( aScroller )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::~CVcxHgMyVideosCategoryModelHandler()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryModelHandler::~CVcxHgMyVideosCategoryModelHandler()
    {
    delete iCategoryList;
    iCategoryIdArray.Reset();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::DoModelActivateL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryModelHandler::DoModelActivateL()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosCategoryModelHandler::DoModelActivateL() - Enter" );

    iModel.CollectionClient().SetCategoryModelObserver( this );
    
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosCategoryModelHandler::DoModelActivateL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::DoModelDeactivate()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryModelHandler::DoModelDeactivate()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosCategoryModelHandler::DoModelDeactivate() - Enter" );

    iModel.CollectionClient().SetCategoryModelObserver( NULL );
    
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosCategoryModelHandler::DoModelDeactivate() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::GetCategoryListL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryModelHandler::GetCategoryListL()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosCategoryModelHandler::GetCategoryListL() - Enter" );
    
    iModel.CollectionClient().GetCategoryListL();
    
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosCategoryModelHandler::GetCategoryListL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::Highlight()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosCategoryModelHandler::Highlight()
    {
    return iScroller.SelectedIndex();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::GetCategoryNameLC()
// -----------------------------------------------------------------------------
// 
HBufC* CVcxHgMyVideosCategoryModelHandler::GetCategoryNameLC( TInt aIndex )
    {
    HBufC* name = NULL;

    switch ( aIndex )
        {
        case KVcxMvcCategoryIdAll:
            name = StringLoader::LoadLC( R_VCXHGMYVIDEOS_STORAGE_ALL_LIST );
            break;
        case KVcxMvcCategoryIdDownloads:
            name = StringLoader::LoadLC( R_VCXHGMYVIDEOS_STORAGE_DOWNLOADS_LIST );
            break;        
        case KVcxMvcCategoryIdCaptured:
            name = StringLoader::LoadLC( R_VCXHGMYVIDEOS_STORAGE_CAPTURED_LIST );
            break;            
        case KVcxMvcCategoryIdOther:
            name = StringLoader::LoadLC( R_VCXHGMYVIDEOS_STORAGE_OTHER_LIST );
            break;            
        default:
            {
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # GetCategoryNameLC() Unexpected category!" );
            }
            break;
        }
    return name;    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::FormatCategorySecondRowLC()
// -----------------------------------------------------------------------------
// 
HBufC* CVcxHgMyVideosCategoryModelHandler::FormatCategorySecondRowLC( CMPXMedia& aMedia )
    {
    HBufC* details = NULL;
    HBufC* newVideoName = NULL;
    TInt newVideos = 0;
    TInt videos = 0;

    if ( aMedia.IsSupported( KVcxMediaMyVideosCategoryNewItemCount ) )
        {
        newVideos = *( aMedia.Value<TInt>( KVcxMediaMyVideosCategoryNewItemCount ) );
        }
    if ( aMedia.IsSupported( KVcxMediaMyVideosCategoryItemCount ) )
        {
        videos = *( aMedia.Value<TInt>( KVcxMediaMyVideosCategoryItemCount ) );
        }

    if ( newVideos > 0 )
        {
        if ( aMedia.IsSupported( KVcxMediaMyVideosCategoryNewItemName ) )
            {
            newVideoName = aMedia.ValueText( KVcxMediaMyVideosCategoryNewItemName ).AllocLC();
            }
        else
            {
            newVideoName = KNullDesC().AllocLC();
            }

        if ( newVideos == 1 )
            {
            details = StringLoader::LoadL( R_VCXHGMYVIDEOS_ONE_NEW_VIDEO, *newVideoName );
            }
        else
            {
            CDesCArrayFlat* arrayForStrings = new ( ELeave ) CDesCArrayFlat( 1 );
            CleanupStack::PushL( arrayForStrings );
            arrayForStrings->AppendL( *newVideoName );
            
            CArrayFixFlat<TInt>* arrayForInts = new ( ELeave ) CArrayFixFlat<TInt>( 3 );
            CleanupStack::PushL( arrayForInts );
            arrayForInts->AppendL( newVideos );
            
            details = StringLoader::LoadL( R_VCXHGMYVIDEOS_N_NEW_VIDEOS,
                                           *arrayForStrings,
                                           *arrayForInts );
            
            CleanupStack::PopAndDestroy( arrayForInts );
            CleanupStack::PopAndDestroy( arrayForStrings );
            }
        
        CleanupStack::PopAndDestroy( newVideoName );
        CleanupStack::PushL( details );
        }
    else
        {
        if ( videos == 0 )
            {
            details = StringLoader::LoadLC( R_VCXHGMYVIDEOS_NO_VIDEOS_IN_CATEGORY );
            }
        else if ( videos == 1 )
            {
            details = StringLoader::LoadLC( R_VCXHGMYVIDEOS_ONE_VIDEO_IN_CATEGORY );            
            }
        else
            {
            details = StringLoader::LoadLC( R_VCXHGMYVIDEOS_VIDEOS_IN_CATEGORY, videos );
            }        
        }

    return details;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::UiIndexFromMpxItemId()
// -----------------------------------------------------------------------------
// 
TInt CVcxHgMyVideosCategoryModelHandler::UiIndexFromMpxItemId( TMPXItemId& aMpxItemId )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # UiIndexFromMpxItemId() - Enter" );
    
    TInt index ( KErrNotFound );
    
    if ( iCategoryList )
        {
        CMPXMedia* media = NULL;
        TMPXItemId mpxItemId;

        for ( TInt i = 0; i < iCategoryList->Count(); i++ )
            {
            media = (*iCategoryList)[i];

            if ( media && media->IsSupported( KMPXMediaGeneralId ) )
                {
                mpxItemId = *media->Value<TMPXItemId>( KMPXMediaGeneralId );
            
                if ( mpxItemId == aMpxItemId )
                    {
                    index = ResolveCategoryArrayIndexById( i );                                        
                    break;
                    }
                }
            }
        }
    
    IPTVLOGSTRING2_LOW_LEVEL( "MPX My Videos UI # UiIndexFromMpxItemId(), index = %d - Exit", index );
    return index;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::ResolveCategoryArrayIndexById()
// -----------------------------------------------------------------------------
// 
TInt CVcxHgMyVideosCategoryModelHandler::ResolveCategoryArrayIndexById( TInt aCategoryId )
    {
    TInt ret( 0 );
    for(TInt i = 0; i < iCategoryIdArray.Count(); i++ )
        {
        if( iCategoryIdArray[ i ] == aCategoryId )
            {
            ret = i;
            break;
            }
        }  
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::NewCategoryListL()
// MPX Collection calls this callback when new category list is available.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::NewCategoryListL( 
        CMPXMediaArray* aCategoryList )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # NewCategoryListL() - Enter" );

    delete iCategoryList;
    iCategoryList = aCategoryList;
    
    UpdateCategoryListL();
    
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # NewCategoryListL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::CategoryListModifiedL()
// MPX Collection calls this callback when the category list is modified.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::CategoryListModifiedL()
    { 
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CategoryListModifiedL() - Enter" );
    
    UpdateCategoryListL();
        
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CategoryListModifiedL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::CategoryModifiedL()
// MPX Collection calls this callback when single category is modified.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::CategoryModifiedL( TMPXChangeEventType aEventType, 
                                                            TMPXItemId& aMpxItemId )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # CategoryModifiedL(event=%d)", aEventType );

    switch ( aEventType )
        {
        case EMPXItemModified:
            {
            TInt uiIndex = UiIndexFromMpxItemId( aMpxItemId );
            UpdateCategoryListItemL( uiIndex );
            iScroller.RefreshScreen( uiIndex );
            }
            break;
            
        case EMPXItemInserted:
        case EMPXItemDeleted:
        default:
            {
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # CategoryModifiedL() Unexpected event!" );
            }
            break;
        } 
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::UpdateCategoryListL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryModelHandler::UpdateCategoryListL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # UpdateCategoryListL() - Enter" );
    
    if ( iCategoryList )
        {        
        TInt categoryCount = iCategoryList->Count();
        
        MakeCategoryIdArray( categoryCount );
        
        IPTVLOGSTRING2_LOW_LEVEL( 
                "MPX My Videos UI # UpdateCategoryListL() - count = %d", categoryCount );
        
        if ( iCategoryIdArray.Count() != iScroller.ItemCount() )
            {
            iScroller.ResizeL( iCategoryIdArray.Count() );
            }
            
        if ( iCategoryIdArray.Count() > 0 )
            {                    
            for ( TInt i = 0; i < iCategoryIdArray.Count(); i++ )
                {
                UpdateCategoryListItemL( i );
                }
            if ( iScroller.SelectedIndex() < 0 || 
                    iScroller.SelectedIndex() >= iScroller.ItemCount() )
                {
                iScroller.SetSelectedIndex( 0 );    
                }           
            }        
        
        // Refresh the whole list.
        iScroller.DrawDeferred();
        }
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # UpdateCategoryListL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::UpdateCategoryListItemL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::UpdateCategoryListItemL( TInt aListIndex )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
            "MPX My Videos UI # UpdateCategoryListItemL (index=%d)", aListIndex );
    
    if( aListIndex >= 0 && 
        aListIndex < iScroller.ItemCount() &&
        aListIndex < iCategoryIdArray.Count() )
        {
        CHgItem& listItem = iScroller.ItemL( aListIndex );
        
        CMPXMedia* media( NULL );        
        media = (*iCategoryList)[ iCategoryIdArray[ aListIndex ] ];
        
        HBufC* categoryName( NULL );     
        categoryName = GetCategoryNameLC( iCategoryIdArray[ aListIndex ] );
        
        if ( categoryName && media )
            {
            // Insert the category name into first row of list item.
            listItem.SetTitleL( *categoryName );
            CleanupStack::PopAndDestroy( categoryName );
                    
            // Insert video count into second row of list item.
            HBufC* secondRow( NULL );
            secondRow = FormatCategorySecondRowLC( *media );
            listItem.SetTextL( *secondRow );
            CleanupStack::PopAndDestroy( secondRow );            
            }
        else
            {
            // If category name or media was not found then 
            // the whole list item should be removed.
            RemoveCategoryListItem( aListIndex );
            }
        }
        
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # UpdateCategoryListItemL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::RemoveCategoryListItem()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryModelHandler::RemoveCategoryListItem( TInt aListIndex )
    {
    if ( aListIndex >= 0 && aListIndex < iScroller.ItemCount() )
        {
        iScroller.RemoveItem( aListIndex );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::ResolveCategoryId()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosCategoryModelHandler::ResolveCategoryId( TInt aScrollerIndex )
    {
    TInt ret( -1 );
    if ( aScrollerIndex <= iCategoryIdArray.Count() )
        {
        ret = iCategoryIdArray[ aScrollerIndex ];
        }  
    return ret;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::MakeCategoryIdArray()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryModelHandler::MakeCategoryIdArray( TInt aCategoriesAvailable )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
            "MPX My Videos UI # MakeCategoryArray (number of categories=%d)", aCategoriesAvailable );
    
    iCategoryIdArray.Reset();    
    
    for(TInt i = 0; i < aCategoriesAvailable; i++)
        {
        if( AcceptCategory( i ) )
            {
            iCategoryIdArray.Append( i );        
            }        
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::AcceptCategory()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosCategoryModelHandler::AcceptCategory( TInt aCategoryId )
    {
    TBool ret(EFalse);
    switch ( aCategoryId )
        {
        case KVcxMvcCategoryIdAll:
        case KVcxMvcCategoryIdDownloads:
        case KVcxMvcCategoryIdCaptured:
        case KVcxMvcCategoryIdOther:
            IPTVLOGSTRING2_LOW_LEVEL( 
                    "MPX My Videos UI # AcceptCategory() Accepted category: %d", aCategoryId );
            ret = ETrue;
            break;
        default:
            IPTVLOGSTRING2_LOW_LEVEL( 
                    "MPX My Videos UI # AcceptCategory() Uknowns category: %d", aCategoryId );
            break;            
        }
    
    return ret;
    }
