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
#include <ganes/HgDoubleGraphicList.h>

#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include <thumbnailobjectsource.h>
#include <thumbnaildata.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <vcxhgmyvideosicons.mbg>
#include <myvideosindicator.h>
#include <vcxhgmyvideos.rsg>
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>
#include "IptvDebug.h"

#include "vcxhgmyvideoscategorymodelhandler.h"
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideoscollectionclient.h"
#include "vcxhgmyvideosthumbnailmanager.h"
#include "vcxhgmyvideoscenrepkeys.h"

const TInt KMyVideosTitleStringMaxLength = 64;
const TInt KMyVideosTitleUrlMaxLength    = 128;

_LIT( KVcxHgMyVideosMifFile, "\\resource\\apps\\vcxhgmyvideosicons.mif" );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryModelHandler* CVcxHgMyVideosCategoryModelHandler::NewL(
        CVcxHgMyVideosModel& aModel,
        CHgScroller& aScroller )
    {
    CVcxHgMyVideosCategoryModelHandler* self = 
        new (ELeave) CVcxHgMyVideosCategoryModelHandler( aModel, aScroller );

    return self;
    }


// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::CVcxHgMyVideosCategoryModelHandler()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryModelHandler::CVcxHgMyVideosCategoryModelHandler(
        CVcxHgMyVideosModel& aModel, CHgScroller& aScroller )
    : iModel( aModel ),
      iScroller( aScroller ),
      iTnManager( aModel.ThumbnailManager() ),
      iTnRequestId( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::~CVcxHgMyVideosCategoryModelHandler()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryModelHandler::~CVcxHgMyVideosCategoryModelHandler()
    {
    delete iCategoryList;
    delete iLastWatched;
    delete iVideoIndicator;
    
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

    iTnManager.AddObserverL( *this );
    
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

    iTnManager.RemoveObserver( *this );
    
    delete iLastWatched;
    iLastWatched = NULL;
    
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
HBufC* CVcxHgMyVideosCategoryModelHandler::GetCategoryNameLC( TInt aCategoryId )
    {
    HBufC* name = NULL;

    switch ( aCategoryId )
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
        case KCategoryIdLastWatched:
            name = StringLoader::LoadLC( R_VCXHGMYVIDEOS_LAST_WATCHED );
            break;            
        case KCategoryIdExtraItem1:
            // ExtraItem1 is always interpreted as Ovi Store
            name = StringLoader::LoadLC( R_VCXHGMYVIDEOS_OVI_STORE ); 
			break;
        case KCategoryIdExtraItem2:
        case KCategoryIdExtraItem3:
            {
            TInt key = 0;
            if ( aCategoryId == KCategoryIdExtraItem2 )
                {
                key = KCRVideoPlayerExtraItem2Title;
                }
            else if ( aCategoryId == KCategoryIdExtraItem3 )
                {
                key = KCRVideoPlayerExtraItem3Title;
                }

            TBuf<KMyVideosTitleStringMaxLength> titleString;
            TInt error = iModel.GetMyVideosCustomizationString( key, titleString );
            if ( error == KErrNone )
                {
                name = titleString.AllocLC();
                }
            else 
                {
                name = KNullDesC().AllocLC();
                }
            }
            break;
            
        default:
            {
            name = KNullDesC().AllocLC();
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # GetCategoryNameLC() Unexpected category!" );
            }
            break;
        }
    return name;    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::GetCategoryIconL()
// -----------------------------------------------------------------------------
// 
CGulIcon* CVcxHgMyVideosCategoryModelHandler::GetCategoryIconL( TInt aCategoryId )
    {
    IPTVLOGSTRING2_LOW_LEVEL( "CVcxHgMyVideosCategoryModelHandler::GetCategoryIconL() Enter! Category id = %d", aCategoryId );

    CGulIcon* icon( NULL );
    TInt bitmapId( 0 );
    TInt maskId( 0 );
    TInt key( KErrNotFound );
    TFileName iconFile( KNullDesC() );
    
    // Find icon file & bitmap IDs or cenrep keys
    switch ( aCategoryId )
        {
        case KVcxMvcCategoryIdDownloads:    // fall through
        case KVcxMvcCategoryIdAll:          // fall through
        case KVcxMvcCategoryIdOther:
            iconFile = KVcxHgMyVideosMifFile;
            bitmapId = EMbmVcxhgmyvideosiconsQgn_prop_download_thumbnail_video;
            maskId = EMbmVcxhgmyvideosiconsQgn_prop_download_thumbnail_video_mask;
            break;
        case KVcxMvcCategoryIdCaptured:
            iconFile = KVcxHgMyVideosMifFile;
            bitmapId = EMbmVcxhgmyvideosiconsQgn_prop_captured_thumbnail_video;
            maskId = EMbmVcxhgmyvideosiconsQgn_prop_captured_thumbnail_video_mask;
            break;
        case KCategoryIdLastWatched:
            // If no last watched, dont show default icon
            if ( iLastWatched )
                {
                iconFile = KVcxHgMyVideosMifFile;
                bitmapId = EMbmVcxhgmyvideosiconsQgn_prop_recent_thumbnail_video;
                maskId = EMbmVcxhgmyvideosiconsQgn_prop_recent_thumbnail_video_mask;
                }
            break;
        case KCategoryIdExtraItem1:
            // ExtraItem1 is always interpreted as Ovi Store
            iconFile = KVcxHgMyVideosMifFile;
            bitmapId = EMbmVcxhgmyvideosiconsQgn_prop_ovi_thumbnail_video;
            maskId = EMbmVcxhgmyvideosiconsQgn_prop_ovi_thumbnail_video_mask;
            break;
        case KCategoryIdExtraItem2:
            key = KCRVideoPlayerExtraItem2IconPath;
            break;
        case KCategoryIdExtraItem3:
            key = KCRVideoPlayerExtraItem3IconPath;
            break;
        default:
            {
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # GetCategoryIconLC() Unexpected category!" );
            }
            break;
        }

    if ( key != KErrNotFound ) 
        {
        // Extra item 2 or 3, get icon file from cenrep
        TInt error = iModel.GetMyVideosCustomizationString( key, iconFile );
        if ( error == KErrNone && iconFile.Length() > 0 )
            {
            IPTVLOGSTRING3_LOW_LEVEL( "CVcxHgMyVideosCategoryModelHandler::GetCategoryIconL() cenrep key %d iconFile %S", key, &iconFile );
            bitmapId = 0x4000;
            maskId = 0x4001;
            }
        else
            { // Use default service icon
            iconFile = KVcxHgMyVideosMifFile;
            bitmapId = EMbmVcxhgmyvideosiconsQgn_prop_service_thumbnail_video;
            maskId = EMbmVcxhgmyvideosiconsQgn_prop_service_thumbnail_video_mask;
            }
        }

    if ( bitmapId && iconFile.Length() > 0 )
        {
        // find mif file
        TFindFile findFile( iModel.FileServerSessionL() );
        TInt error = findFile.FindByDir( iconFile, KNullDesC );

        // Create icon if icon file was found
        if ( KErrNone == error )
            {
            TParse parse;
            parse.Set( findFile.File(), NULL, NULL );
            iconFile = parse.FullName();
            IPTVLOGSTRING3_LOW_LEVEL( "CVcxHgMyVideosCategoryModelHandler::GetCategoryIconLC() get %d from file %S", bitmapId, &iconFile );

            icon = CreateHgListIconL( iconFile, bitmapId, maskId );
            }
        else
            {
            IPTVLOGSTRING2_LOW_LEVEL( "CVcxHgMyVideosCategoryModelHandler::GetCategoryIconL() FindByDir() returned %d", error );
            }
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( "CVcxHgMyVideosCategoryModelHandler::GetCategoryIconLC() no icon file or id" );
        }

    return icon;    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryModelHandler::FormatCategorySecondRowLC()
// -----------------------------------------------------------------------------
// 
HBufC* CVcxHgMyVideosCategoryModelHandler::FormatCategorySecondRowLC( CMPXMedia& aMedia )
    {    
    if ( !aMedia.IsSupported( KMPXMediaGeneralType ) ||
          aMedia.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) != EMPXGroup )
        {
        if ( aMedia.IsSupported( KMPXMediaGeneralTitle ) )
            {
            return aMedia.ValueText( KMPXMediaGeneralTitle ).AllocLC(); 
            }
	    else
		    {
			return KNullDesC().AllocLC();
			}
        }
    
    HBufC* details = NULL;
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
        if ( newVideos == 1 )
            {
            details = StringLoader::LoadL( R_VCXHGMYVIDEOS_ONE_NEW_VIDEO );
            }
        else
            {
            details = StringLoader::LoadL( R_VCXHGMYVIDEOS_N_NEW_VIDEOS, newVideos );
            }
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
// CVcxHgMyVideosCategoryModelHandler::FormatCategorySecondRowLC()
// -----------------------------------------------------------------------------
// 
HBufC* CVcxHgMyVideosCategoryModelHandler::FormatCategorySecondRowLC( TInt aCategoryId )
    {
    HBufC* secondRow = NULL;
   
    switch ( aCategoryId )
        {
        case KVcxMvcCategoryIdAll:
        case KVcxMvcCategoryIdDownloads:
        case KVcxMvcCategoryIdTvRecordings:
        case KVcxMvcCategoryIdCaptured:
        case KVcxMvcCategoryIdOther:
        case KCategoryIdLastWatched:
            {
            CMPXMedia* media = GetCategoryDataL( aCategoryId );
            if ( media )
                {
                secondRow = FormatCategorySecondRowLC( *media );
                }
			else
			    {
				secondRow = KNullDesC().AllocLC();
				}
            }
            break;
        case KCategoryIdExtraItem1:
            // ExtraItem1 is always interpreted as Ovi Store
            secondRow = StringLoader::LoadLC( R_VCXHGMYVIDEOS_OVI_STORE_VIDEOS ); 
            break;
            
        case KCategoryIdExtraItem2:
        case KCategoryIdExtraItem3:
            {
            TInt key = 0;
            if ( aCategoryId == KCategoryIdExtraItem2 )
                {
                key = KCRVideoPlayerExtraItem2Text;
                }
            else if ( aCategoryId == KCategoryIdExtraItem3 )
                {
                key = KCRVideoPlayerExtraItem3Text;
                }
            
            TBuf<KMyVideosTitleStringMaxLength> detailsString;
            TInt error = iModel.GetMyVideosCustomizationString( key, detailsString );
            if ( error == KErrNone )
                {
                secondRow = detailsString.AllocLC();
                }
            else 
                {
                secondRow = KNullDesC().AllocLC();
                }
            break;
            }
        default:
            {
            secondRow = KNullDesC().AllocLC();
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # FormatCategorySecondRowLC() Unexpected category!" );
            break;
            }
        }
    return secondRow;
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
    TInt ret( KErrNotFound );
    
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
        CMPXMediaArray* aCategoryList, TBool aIsPartial )
    {
    IPTVLOGSTRING2_LOW_LEVEL( "MPX My Videos UI # NewCategoryListL() - Enter. IsPartial=%d", aIsPartial );

    delete iCategoryList;
    iCategoryList = aCategoryList;
    
    UpdateCategoryListL();
	
    if ( !aIsPartial )
        {
        // KVcxMessageMyVideosListComplete does not come, finish list fetching now
        CategoryListFetchingCompletedL();
        }
    
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
    
    FetchLastWatchedL();
    SetLastWatchedIndicatorL();
    
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
            {
            if ( !iLastWatched )
                {
                TInt id1( 0 );
                TInt ret = iModel.GetLastWatchedIdL( id1 );
                TMPXItemId id ( id1 );
                
                if (  ret == KErrNone &&
                      id1 == aMpxItemId.iId1 )
                    {
                    FetchLastWatchedL();
                    }
                }
            }
            break;
        case EMPXItemDeleted:
            {
            if ( iLastWatched && 
                 iLastWatched->IsSupported( KMPXMediaGeneralId ) &&
                 iLastWatched->Value<TMPXItemId>( KMPXMediaGeneralId )->iId1 == aMpxItemId.iId1 )
                {
                delete iLastWatched;
                iLastWatched = NULL;
                
                UpdateCategoryListItemL( ResolveCategoryArrayIndexById( KCategoryIdLastWatched ) );
                SetLastWatchedIndicatorL();
                }
            }
            break;
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
        
        HBufC* categoryName( NULL );     
        categoryName = GetCategoryNameLC( iCategoryIdArray[ aListIndex ] );
        listItem.SetTitleL( *categoryName );
        CleanupStack::PopAndDestroy( categoryName );

        HBufC* secondRow( NULL );
        secondRow = FormatCategorySecondRowLC( iCategoryIdArray[ aListIndex ] );
        listItem.SetTextL( *secondRow );
        CleanupStack::PopAndDestroy( secondRow );

        // Set icon for category
        listItem.SetIcon( GetCategoryIconL( iCategoryIdArray[ aListIndex ] ) );
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
    if ( aScrollerIndex >= 0 &&
         aScrollerIndex <= iCategoryIdArray.Count() )
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
    
    TInt error = KErrNone;
    TInt value = KErrNotFound;
    TInt categoryId = KErrNotFound;
    
    // Check that cenrep exists and has some valid data
    error = iModel.GetMyVideosCustomizationInt( KCRVideoPlayerItemLocation1, value );
    
    if ( error == KErrNone && value > EMyVideosListItemTypeEmpty )
        {
        IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # MakeCategoryIdArray() Getting category items from cenrep" ); 

        for ( TInt i=KCRVideoPlayerItemLocation1; i<=KCRVideoPlayerItemLocation7; i++ )    
            {
            error = iModel.GetMyVideosCustomizationInt( i , value );
            if ( error == KErrNone && value != EMyVideosListItemTypeEmpty )
                {
                switch ( value )
                    {
                    case EMyVideosListItemTypeLastWatched:
                        categoryId = KCategoryIdLastWatched;
                        break;        
                    case EMyVideosListItemTypeCapturedVideos:
                        categoryId = KVcxMvcCategoryIdCaptured;
                        break;
                    case EMyVideosListItemTypeOtherVideos:
                        categoryId = KVcxMvcCategoryIdOther;
                        break;                        
                    case EMyVideosListItemTypeExtra1:
                        categoryId = KCategoryIdExtraItem1;
                        break;                        
                    case EMyVideosListItemTypeExtra2:
                        categoryId = KCategoryIdExtraItem2;
                        break;                        
                    case EMyVideosListItemTypeExtra3:
                        categoryId = KCategoryIdExtraItem3;
                        break;
                    case EMyVideosListItemTypeEmpty:
                        categoryId = KErrNotFound;
                        break;
                    default:
                        categoryId = KErrNotFound;
                        break;
                    }
                
                if ( AcceptCategory( categoryId ) )
                    {
                    iCategoryIdArray.Append( categoryId );
                    }
                }
            }
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # MakeCategoryIdArray() Could not access CenRep!" ); 
    
        // If cenrep access fails then use these values as backup
	    iCategoryIdArray.Append( KCategoryIdLastWatched );
		
    	for(TInt i = 0; i < aCategoriesAvailable; i++)
        	{
	        if( AcceptCategory( i ) )
    	        {
        	    iCategoryIdArray.Append( i );        
            	}        
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
        case KVcxMvcCategoryIdCaptured:
        case KVcxMvcCategoryIdOther:
        case KCategoryIdLastWatched:
        case KCategoryIdExtraItem1:
        case KCategoryIdExtraItem2:
        case KCategoryIdExtraItem3:
            IPTVLOGSTRING2_LOW_LEVEL( 
                    "MPX My Videos UI # AcceptCategory() Accepted category: %d", aCategoryId );
            ret = ETrue;
            break;
        default:
            IPTVLOGSTRING2_LOW_LEVEL( 
                    "MPX My Videos UI # AcceptCategory() Unknown category: %d", aCategoryId );
            break;            
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoFetchingCompletedL()
// MPX Collection calls this callback when a single video has fetched.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::VideoFetchingCompletedL( CMPXMedia* aVideo )
    {
    TInt id( 0 );
    
    TInt ret = iModel.GetLastWatchedIdL( id );
                    
    if ( ret == KErrNone && 
         id && 
		 aVideo &&
         aVideo->IsSupported( KMPXMediaGeneralId ) &&
         aVideo->Value<TMPXItemId>( KMPXMediaGeneralId )->iId1 == id )
        {
        delete iLastWatched;
        iLastWatched = aVideo;
		
		LoadLastWatchedIconL();
		SetLastWatchedIndicatorL();
        }
    else 
		{
		delete aVideo;
		aVideo = NULL;
		}
    
    UpdateCategoryListItemL( ResolveCategoryArrayIndexById( KCategoryIdLastWatched ) );
    iScroller.RefreshScreen( ResolveCategoryArrayIndexById( KCategoryIdLastWatched ) );
    }

// -----------------------------------------------------------------------------
// MPX Collection calls this callback when category list items have been fetched.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::CategoryListFetchingCompletedL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CategoryListFetchingCompletedL()" ); 
    FetchLastWatchedL();
    SetLastWatchedIndicatorL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::GetCategoryDataL()
// 
// -----------------------------------------------------------------------------
// 
CMPXMedia* CVcxHgMyVideosCategoryModelHandler::GetCategoryDataL( TInt aCategoryId )
    {
    if ( aCategoryId == KCategoryIdLastWatched )
        {
        return iLastWatched;
        }
    
    if ( aCategoryId >= 0 && iCategoryList->Count() > aCategoryId )
        {
        return (*iCategoryList)[ aCategoryId ];
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::FetchLastWatchedL()
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::FetchLastWatchedL()
    {
    TInt id1( 0 );
    TInt ret = iModel.GetLastWatchedIdL( id1 );
    TMPXItemId id ( id1 );
    
    if (  id1 != 0 && 
          KErrNone == ret )
        {
        if ( !iLastWatched ||
		    ( iLastWatched &&
              iLastWatched->IsSupported( KMPXMediaGeneralId ) &&
              iLastWatched->Value<TMPXItemId>( KMPXMediaGeneralId )->iId1 != id1 ))
            {
            iModel.CollectionClient().FetchMpxMediaByMpxIdL( id );
            } 
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::PlayLastWatchedVidedoL()
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::PlayLastWatchedVidedoL()
    {
    if (  iLastWatched && iLastWatched->IsSupported( KMPXMediaGeneralId ) )
        {
        iModel.CollectionClient().PlayVideoL(
                    *iLastWatched->Value<TMPXItemId>( KMPXMediaGeneralId ) );

        iModel.SetAppState( CVcxHgMyVideosModel::EVcxMyVideosAppStatePlayer );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::LoadLastWatchedIconL()
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::LoadLastWatchedIconL()
    {
    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC( 
                    iLastWatched->ValueText( KMPXMediaGeneralUri ),
                    KNullDesC );
    
    iTnRequestId = iTnManager.GetL( *source );
    CleanupStack::PopAndDestroy( source );
	}

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::SetLastWatchedIndicatorL()
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::SetLastWatchedIndicatorL()
    {
    if ( KErrNotFound != ResolveCategoryArrayIndexById( KCategoryIdLastWatched ) ) 
        {
        CHgItem& lastWatchedItem = iScroller.ItemL(
                ResolveCategoryArrayIndexById( KCategoryIdLastWatched ) );
        if ( iLastWatched && VideoIndicatorL().IsIndicatorShown( *iLastWatched ) )
            {
            lastWatchedItem.SetFlags( CHgItem::EHgItemFlagsIconOverlayIndicator );
            }
        else
            {
            lastWatchedItem.ClearFlags( CHgItem::EHgItemFlagsIconOverlayIndicator );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoIndicatorL()
// 
// -----------------------------------------------------------------------------
// 
CMyVideosIndicator& CVcxHgMyVideosCategoryModelHandler::VideoIndicatorL()
    {
    if ( !iVideoIndicator )
        {
        iVideoIndicator = CMyVideosIndicator::NewL();
        }
    return *iVideoIndicator;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ThumbnailPreviewReady()
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::ThumbnailPreviewReady( 
                MThumbnailData& /*aThumbnail*/,
                TThumbnailRequestId /*aId*/ )
    {
    // No implementation
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ThumbnailReady()
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::ThumbnailReady( 
                TInt aError, 
                MThumbnailData& aThumbnail,
                TThumbnailRequestId aId )
    {
    if ( aId == iTnRequestId )
        {
        iTnRequestId = KErrNotFound;
        
        TRAP_IGNORE( ThumbnailReadyL( aError, aThumbnail, aId ) );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ThumbnailReady()
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::ThumbnailReadyL( 
                TInt aError, 
                MThumbnailData& aThumbnail,
                TThumbnailRequestId /*aId*/ )
    {
    TInt lastWatchedIndex = ResolveCategoryArrayIndexById( KCategoryIdLastWatched );
    if ( lastWatchedIndex != KErrNotFound )
        {
        CHgItem& listItem = iScroller.ItemL( lastWatchedIndex );
        CGulIcon* thumbnail( NULL );
        
        if ( !aError )
            {
            thumbnail = CGulIcon::NewL( aThumbnail.DetachBitmap() );
            CleanupStack::PushL( thumbnail );
            }
        else
            {
            TFileName iconFile( KVcxHgMyVideosMifFile );
            thumbnail = CreateHgListIconL( iconFile, 
                EMbmVcxhgmyvideosiconsQgn_prop_recent_thumbnail_video, 
                EMbmVcxhgmyvideosiconsQgn_prop_recent_thumbnail_video_mask );

            CleanupStack::PushL( thumbnail );
            }
        
        listItem.SetIcon( thumbnail ); // Takes ownership
        CleanupStack::Pop( thumbnail );
        
        iScroller.RefreshScreen( lastWatchedIndex );
        }
    }


// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::HandleExtraItemSelectionL()
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryModelHandler::HandleExtraItemSelectionL( 
        TInt categoryId,
        TVcxHgMyVideosCategoryItemType& aItemType,
        TUid& aUid,
        TDes& aUrl )
    {
    TInt uidKey = 0;
    TInt urlKey = 0;
   
    aItemType = TVcxHgMyVideosCategoryItemTypeUndefined;
    
    switch ( categoryId )
        {
        case KCategoryIdExtraItem1:
            {
            uidKey = KCRVideoPlayerExtraItem1Uid;
            urlKey = KCRVideoPlayerExtraItem1Url;                   
            break;
            }
        case KCategoryIdExtraItem2:
            {
            uidKey = KCRVideoPlayerExtraItem2Uid;
            urlKey = KCRVideoPlayerExtraItem2Url;                    
            break;
            }
        case KCategoryIdExtraItem3:
            {
            uidKey = KCRVideoPlayerExtraItem3Uid;
            urlKey = KCRVideoPlayerExtraItem3Url;                   
            break;
            }
        default:
            return;
        }
    
    TBuf<KMyVideosTitleUrlMaxLength> urlString;
    TInt uidInt( 0 ) ;
    
    // Try to get web link URL first.
    TInt error = iModel.GetMyVideosCustomizationString( urlKey, urlString );
    if ( error == KErrNone && urlString.Size() > 0 )
        {
        aUrl = urlString;
        aItemType = TVcxHgMyVideosCategoryItemTypeUrl;
        }
    else 
        {
        // If URL is not found try to fetch application UID.
        error = iModel.GetMyVideosCustomizationInt( uidKey, uidInt );
        if ( error == KErrNone && uidInt != 0 )
            {
            aUid = TUid::Uid( uidInt );
            aItemType = TVcxHgMyVideosCategoryItemTypeUid;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::CreateHgListIconL()
// 
// -----------------------------------------------------------------------------
// 
CGulIcon* CVcxHgMyVideosCategoryModelHandler::CreateHgListIconL(
        const TFileName& aFileName,
        TInt aBitmapId,
        TInt aMaskId )
    {
    IPTVLOGSTRING4_LOW_LEVEL( "CVcxHgMyVideosCategoryModelHandler::CreateHgListIconL() bitmap=%d mask=%d file %S", aBitmapId, aMaskId, &aFileName );
    
    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* mask( NULL );
    CGulIcon* icon( NULL );

    // Create default icon for Hg list.
    AknIconUtils::CreateIconLC(
            bitmap,
            mask,
            aFileName, 
            aBitmapId,
            aMaskId );

    TInt error( KErrNone );
    error = AknIconUtils::SetSize(
            bitmap, 
            CHgDoubleGraphicList::PreferredImageSize(), 
            EAspectRatioPreservedAndUnusedSpaceRemoved );
    User::LeaveIfError( error );

    error = AknIconUtils::SetSize(
            mask, 
            CHgDoubleGraphicList::PreferredImageSize(), 
            EAspectRatioPreservedAndUnusedSpaceRemoved );
    User::LeaveIfError( error );

    // Ownership of bitmap and mask is transferred to icon.
    icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2, bitmap ); // mask and bitmap

    IPTVLOGSTRING2_LOW_LEVEL( "CVcxHgMyVideosCategoryModelHandler::CreateHgListIconL() icon=0x%08x", icon );

    return icon;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::CreateEmptyHgListIconL()
// 
// -----------------------------------------------------------------------------
// 
CGulIcon* CVcxHgMyVideosCategoryModelHandler::CreateEmptyHgListIconL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxHgMyVideosCategoryModelHandler::CreateEmptyHgListIconL()" );
    CGulIcon* icon( NULL );
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;

    bitmap->Create( CHgDoubleGraphicList::PreferredImageSize(), ENone );
    CleanupStack::PushL( bitmap );
    icon = CGulIcon::NewL( bitmap );
    CleanupStack::Pop( bitmap );

    return icon;
    }
