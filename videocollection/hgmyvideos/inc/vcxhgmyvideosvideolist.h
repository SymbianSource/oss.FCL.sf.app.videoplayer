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




#ifndef VCXHGMYVIDEOSVIDEOLIST_H
#define VCXHGMYVIDEOSVIDEOLIST_H

// INCLUDE FILES
#include <coemain.h>
#include <e32cmn.h>
#include <vcxmyvideosdefs.h>

// FORWARD DECLARATIONS
class CMPXMedia;
class CMPXMediaArray;
class CVcxHgMyVideosVideoListItem;
class TMPXItemId;

// CLASS DECLARATION

/**
 * Class for providing video list.
 * 
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosVideoList ) :
        public CBase
    {
    public: // Constructing and destructing.

        /**
         * Two-phased constructor.
         * 
         * @return New object.
         */
        static CVcxHgMyVideosVideoList* NewL();

        /**
         * Two-phased constructor.
         * 
         * @return New object. 
         */
        static CVcxHgMyVideosVideoList* NewLC();

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosVideoList();

    public: // Data changes.
        
        /**
         * Removes all items from the video list.
         */
        void RemoveVideoList();
        
        /**
         * Replaces video list with new one.
         * 
         * @param aVideoList Reference to new video list.
         */
        void ReplaceVideoListL( CMPXMediaArray& aVideoList );
                
    public: // Data fetching and status information.
               
        /**
         * Returns count of video items.
         * 
         * @return Count of items.
         */        
        TInt VideoCount();

        /**
         * Gets video's download state based on list index.
         * 
         * @param aIndex Index to video list.
         * @return Video's download state as TVcxMyVideosDownloadState structure.
         */
        TVcxMyVideosDownloadState VideoDownloadState( TInt aIndex );
        
		/**
		 * Returns a list of item indexes which downloading
		 * state is different than EVcxMyVideosDlStateNone.
		 * 
		 * @return List of item indexes.
		 */
		void GetOngoingDownloads( RArray<TInt>& aDownloads );
		
        /**
         * Removes MPX Media object from video array.
         * 
         * @param aMpxItemId MPX Item Id.
         * @return Removed index number if succeeded, otherwise KErrNotFound.
         */
        TInt RemoveVideo( TMPXItemId aMpxItemId );
        
        /**
         * Resorts the video list. The list can be sorted by date, 
		 * size or name.
         *
         * @param aSortingOrder Sorting order used.
         */
        void ResortVideoListL( const TVcxMyVideosSortingOrder& aSortingOrder );
        
        /**
         * Adds video to video list to the correct position, 
         * according to sorting order. Ownership of video 
         * is transferred to video list.
         *
         * @param aVideo Video to add.
         * @param aSortingOrder Sorting order used.
         */
        void AddToCorrectPlaceL( CMPXMedia* aVideo,
                TVcxMyVideosSortingOrder aSortingOrder );
        
        /**
         * Returns MPX Media object based on index.
         * 
         * @param aIndex Index to video list.
         * @return MPX Media object or NULL.
         */
        CMPXMedia* MPXMedia( TInt aIndex );
        
        /**
         * Returns MPX Media object based on MPX Item Id.
         * 
         * @pararm aMpxItemId MPX Item Id.
         * @return MPX Media object or NULL.
         */
        CMPXMedia* MPXMediaByMPXItemId( TMPXItemId aMpxItemId );
        
        /**
         * Returns correct list index based on MPX Item Id.
         * 
         * @param aMpxItemId MPX Item Id.
         * @return List index.
         */
        TInt IndexByMPXItemId( TMPXItemId aMpxItemId );
        
        /**
         * Translates array index to MPX item id by reading the value from array.
         *
         * @param aArrayIndex Index in array.
         * @return MPX item id
         */
        TMPXItemId ArrayIndexToMpxItemIdL( TInt aArrayIndex );
        
        /**
         * Returns MPX Media object based on URI.
         * 
         * @param aUri Video's URI.
         * @return MPX Media object or NULL.
         */        
        CMPXMedia* MPXMediaByUri( const TDesC& aUri );

    private:

        /**
         * Constructor.
         */
        CVcxHgMyVideosVideoList();
                
    private:

        /**
         * Array of videos.
         * Own.
         */
        RPointerArray<CVcxHgMyVideosVideoListItem> iVideoList;
    };

#endif // VCXHGMYVIDEOSVIDEOLIST_H
