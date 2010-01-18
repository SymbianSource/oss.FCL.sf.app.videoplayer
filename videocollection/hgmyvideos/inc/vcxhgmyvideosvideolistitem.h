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
* Description:   CVcxHgMyVideosVideoListItem class definition*
*/




#ifndef VCXHGMYVIDEOSVIDEOLISTITEM_H
#define VCXHGMYVIDEOSVIDEOLISTITEM_H

// INCLUDE FILES
#include <coemain.h>
#include <mpxitemid.h>
#include "vcxmyvideosdefs.h"

// FORWARD DECLARATIONS
class CMPXMedia;

// CLASS DECLARATION

/**
 *
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosVideoListItem ) : public CBase
    {
    public:

        /**
         * Two-phased constructor.
		 *
         * @param aMPXMedia MPX Media object, ownership is transferred.
         * @return New object.
         */
        static CVcxHgMyVideosVideoListItem* NewL( CMPXMedia* aMPXMedia );

        /**
         * Two-phased constructor.
		 *
         * @param aMPXMedia MPX Media object, ownership is transferred.
         * @return New object. 
         */
        static CVcxHgMyVideosVideoListItem* NewLC( CMPXMedia* aMPXMedia );

        /**
		 * Compares aNewVideo and aVideoInArray objects by size.
		 * 
		 * @param aNewVideo Video to be added to the list. 
		 * @param aVideoInArray Video in array which is currently compared against aNewVideo.
		 * @return 1 if aNewVideo size is greater, -1 if smaller and 0 if equal.
		 */
		static TInt CompareBySize( const CVcxHgMyVideosVideoListItem& aNewVideo,
                                   const CVcxHgMyVideosVideoListItem& aVideoInArray );
        
		/**
		 * Compares aNewVideo and aVideoInArray objects by name.
		 * 
		 * @param aNewVideo Video to be added to the list. 
		 * @param aVideoInArray Video in array which is currently compared against aNewVideo.
		 * @return -1 if aVideoInArray has smaller value, 1 if greater and 0 if equal.
		 */
		static TInt CompareByName( const CVcxHgMyVideosVideoListItem& aNewVideo,
                                   const CVcxHgMyVideosVideoListItem& aVideoInArray );
        
        /**
		 * Compares aNewVideo and aVideoInArray objects by date.
		 * 
		 * @param aNewVideo Video to be added to the list. 
		 * @param aVideoInArray Video in array which is currently compared against aNewVideo.
		 * @return -1 if aVideoInArray has smaller value, 1 if greater and 0 if equal.
		 */
		static TInt CompareByDate( const CVcxHgMyVideosVideoListItem& aNewVideo,
                                   const CVcxHgMyVideosVideoListItem& aVideoInArray );
		
	    /**
         * Returns MPX Media object.
		 *
		 * @return MPX Media object.
         */
		CMPXMedia* Media();
		        
        /**
         * Gets item's download state.
         * 
         * @return Item's download state as TVcxMyVideosDownloadState structure.
         */
        TVcxMyVideosDownloadState DownloadState();
                
        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosVideoListItem();                      
        
    private: // Constructors

        /**
         * Default constructor.
		 *
		 * @param aMPXMedia MPX Media object, ownership is transferred.
         */
        CVcxHgMyVideosVideoListItem( CMPXMedia* aMPXMedia );

        /**
         * 2nd phase constructor.
         */
        void ConstructL();       
    
    private: // Data
        
        /**
		 * MPX media object.
         * Own.
		 */
        CMPXMedia*                      iMedia;
    };

#endif // VCXHGMYVIDEOSVIDEOLISTITEM_H
