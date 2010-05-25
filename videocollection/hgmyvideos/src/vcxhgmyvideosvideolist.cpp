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
* Description:      Class for providing video list.*
*/




// INCLUDE FILES
#include <collate.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include "vcxhgmyvideosvideolist.h"
#include "vcxhgmyvideosvideolistitem.h"
#include "IptvDebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoList* CVcxHgMyVideosVideoList::NewL()
    {
    CVcxHgMyVideosVideoList* self = 
        CVcxHgMyVideosVideoList::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoList* CVcxHgMyVideosVideoList::NewLC()
    {
    CVcxHgMyVideosVideoList* self = 
        new (ELeave) CVcxHgMyVideosVideoList();
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::CVcxHgMyVideosVideoList()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoList::CVcxHgMyVideosVideoList()
    {
    // No implementation required.
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::~CVcxHgMyVideosVideoList()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoList::~CVcxHgMyVideosVideoList()
    {
    RemoveVideoList();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::RemoveVideoList()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoList::RemoveVideoList()
    {
    iVideoList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::ReplaceVideoListL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoList::ReplaceVideoListL( CMPXMediaArray& aVideoList )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoList::ReplaceVideoListL() - Enter" );
    
    // Removes the all videos from array.
    RemoveVideoList();
           
    TInt count = aVideoList.Count();
    iVideoList.ReserveL( count );
    for ( TInt i = 0; i < count; i++ )
        {
        CMPXMedia* media = CMPXMedia::NewL( *( aVideoList[i] ) ); 
        if ( media )
            {
            CVcxHgMyVideosVideoListItem* item = CVcxHgMyVideosVideoListItem::NewLC( media );
            iVideoList.AppendL( item );
            CleanupStack::Pop( item );
            }
        }        
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoList::ReplaceVideoListL() - Exit" );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoList::HasSameItemsL( const CMPXMediaArray& aVideoList )
    {
    TInt count( iVideoList.Count() );
    if ( aVideoList.Count() != count || count == 0 )
        {
        return EFalse;
        }

    TBool isSame( ETrue );
    CMPXMedia* oldMedia = NULL;
    CMPXMedia* newMedia = NULL;
    for ( TInt i = 0; i < count; i++ )
        {
        oldMedia = iVideoList[i]->Media();
        newMedia = aVideoList[i];
        if ( oldMedia && oldMedia->IsSupported( KMPXMediaGeneralId )
                && newMedia && newMedia->IsSupported( KMPXMediaGeneralId ) )
            {
            if ( oldMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) != 
                    newMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) )
                {
                isSame = EFalse;
                break;
                }
            }
        }
    return isSame;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::VideoCount()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoList::VideoCount()
    {
    return iVideoList.Count();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::RemoveVideo()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoList::RemoveVideo( TMPXItemId aMpxItemId )
    {
    TInt index = IndexByMPXItemId( aMpxItemId );
    
    if ( index != KErrNotFound )
        {
        delete iVideoList[index];
        iVideoList.Remove( index );
        }
    return index;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::MPXMedia()
// -----------------------------------------------------------------------------
//
CMPXMedia* CVcxHgMyVideosVideoList::MPXMedia( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iVideoList.Count() )
        {
        return iVideoList[aIndex]->Media();
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::MPXMediaByUri()
// -----------------------------------------------------------------------------
//
CMPXMedia* CVcxHgMyVideosVideoList::MPXMediaByUri( const TDesC& aUri )
    {
    CMPXMedia* media = NULL;
    TInt count( iVideoList.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        media = iVideoList[i]->Media();
            
        if ( media && media->IsSupported( KMPXMediaGeneralUri ) )
            {
            if ( aUri.CompareF( media->ValueText( KMPXMediaGeneralUri ) ) == 0 )
                {
                return media;
                }
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::MPXMediaByMPXItemId()
// -----------------------------------------------------------------------------
//
CMPXMedia* CVcxHgMyVideosVideoList::MPXMediaByMPXItemId( TMPXItemId aMpxItemId )
    {
    CMPXMedia* media = NULL;
    TInt count( iVideoList.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        media = iVideoList[i]->Media();

        if ( media && media->IsSupported( KMPXMediaGeneralId ) )
            {         
            if ( aMpxItemId == media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) )
                {
                return media;
                }
            }
        }        
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::ArrayIndexToMpxItemIdL()
// -----------------------------------------------------------------------------
//
TMPXItemId CVcxHgMyVideosVideoList::ArrayIndexToMpxItemIdL( TInt aArrayIndex )
    {
    if ( aArrayIndex < 0 || aArrayIndex >= iVideoList.Count() )
        {
        User::Leave( KErrArgument );
        }

    return iVideoList[aArrayIndex]->Media()->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::IndexByMPXItemId()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoList::IndexByMPXItemId( TMPXItemId aMpxItemId )
    {
    CMPXMedia* media = NULL;
    TInt count( iVideoList.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        media = iVideoList[i]->Media();

        if ( media && media->IsSupported( KMPXMediaGeneralId ) )
            {           
            if ( aMpxItemId == media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) )
                {
                return i;
                }
            }
        }        
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::AddToCorrectPlaceL()
// ----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoList::AddToCorrectPlaceL( CMPXMedia* aVideo,
        TVcxMyVideosSortingOrder aSortingOrder )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoList::AddToCorrectPlaceL() - Enter" );
    
    if ( aVideo )
        {        
        // Ownership of aVideo is transferred.
        CVcxHgMyVideosVideoListItem* video = CVcxHgMyVideosVideoListItem::NewL( aVideo );
        CleanupStack::PushL( video );
		
		TLinearOrder<CVcxHgMyVideosVideoListItem> sortOrder( 
            CVcxHgMyVideosVideoListItem::CompareByDate ); 
            
        switch( aSortingOrder )
            {
            case EVcxMyVideosSortingName:
                {
                sortOrder = CVcxHgMyVideosVideoListItem::CompareByName;
                }
                break;
                
            case EVcxMyVideosSortingSize:
                {
                sortOrder = CVcxHgMyVideosVideoListItem::CompareBySize;
                }
                break;
                
            case EVcxMyVideosSortingCreationDate:
                {
                sortOrder = CVcxHgMyVideosVideoListItem::CompareByDate;
                }
                break;
                
            case EVcxMyVideosSortingModified:
            case EVcxMyVideosSortingId:
            default:
                {
                CleanupStack::PopAndDestroy( video );
                IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoList::AddToCorrectPlaceL() - LEAVE: Invalid sort order" );
                User::Leave( KErrNotSupported );
                }
                break;
            }
                
        // Ownership of video is transferred.
        iVideoList.InsertInOrderAllowRepeatsL( video, sortOrder );
        
        CleanupStack::Pop( video );
        }
    
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoList::AddToCorrectPlaceL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoList::ResortVideoListL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoList::ResortVideoListL( const TVcxMyVideosSortingOrder& aSortingOrder )
    {    
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoList::ResortVideoListL() - Enter" );
    
    TLinearOrder<CVcxHgMyVideosVideoListItem> sortOrder( 
        CVcxHgMyVideosVideoListItem::CompareByDate );
    
    switch ( aSortingOrder )
        {
        case EVcxMyVideosSortingName:
            {
            sortOrder = CVcxHgMyVideosVideoListItem::CompareByName;
            }
            break;
            
        case EVcxMyVideosSortingSize:
            {
            sortOrder = CVcxHgMyVideosVideoListItem::CompareBySize;
            }
            break;
            
        case EVcxMyVideosSortingCreationDate:
            {
            sortOrder = CVcxHgMyVideosVideoListItem::CompareByDate;
            }
            break;
            
        case EVcxMyVideosSortingModified:
        case EVcxMyVideosSortingId:
        default:
            {
            IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoList::ResortVideoListL() - LEAVE: Invalid sort order" );
            User::Leave( KErrNotSupported );
            }
            break;
        }
    iVideoList.Sort( sortOrder );   
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoList::ResortVideoListL() - Exit" );
    }
