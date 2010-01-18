/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Image conversion from file to CFbsBitmap in memory*
*/




#ifndef __CIPTV_LIVE_UI_IMAGEHANDLER_H__
#define __CIPTV_LIVE_UI_IMAGEHANDLER_H__

// INCLUDES
#include <AknsItemID.h>
// FORWARD DECLARATIONS
class CImageDecoder;
class MIptvLiveUIImageObserver;
class CAknIconArray;

// CLASS DECLARATION
/**
* Live TV Image handler offers interface to convert images from file 
* (in JPEG or some other format) to Symbian in-memory representation 
* (CFbsBitmap class).
*
*  @since
*/
class CIptvLiveUIImageHandler : public CActive
    {
            
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvLiveUIImageHandler* NewL( MIptvLiveUIImageObserver& aObserver );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvLiveUIImageHandler();

    public: // New functions
        
        /**
		* Starts converting image from file into in-memory CFbsBitmap. 
		* The observer is notified with MIptvLiveUIImageObserver::ImageReadyL()
		* when the conversion is complete.
		* @param aFilePath Image file path.
		* @return None.
		*/
		IMPORT_C void LoadImageL( const TDesC& aFilePath );

		
		
        /**
		* Loads the constant icons (record, remind, rights_forbid etc.)
		* @param aIcons, icon array that the icon will be appended to.
		* @param aIconIndex, index of the icon
		* @param aMaskIndex, index of the icon mask
		* @param aIconSize Icon size.
		* @param aMifFileName MIF file name.
		* @param aInsert if ETrue insert in to beginning of array, if EFalse
		* append to the end of array.
		* @since 3.0
		*/		
		IMPORT_C static void LoadIconL( CAknIconArray* aIcons, const TInt aIconIndex, 
                               const TInt aMaskIndex, const TSize& aIconSize,
                               const TDesC& aMifFileName, TBool aInsert );		
		
        /**
		* Loads the constant color-customized icons (record, remind, rights_forbid etc.)
		* @param aIcons, icon array that the icon will be appended to.
		* @param aItemId, item ID of the masked bitmap to be created
		* @param aIconIndex, index of the icon
		* @param aMaskIndex, index of the icon mask
		* @param aIconSize Icon size.
		* @param aMifFileName MIF file name.
		* @param aInsert if ETrue insert in to beginning of array, if EFalse
		* append to the end of array.
		* @since 5.0
		*/		
		IMPORT_C static void LoadIconL( CAknIconArray* aIcons, const TAknsItemID aItemId,
		                        const TInt aIconIndex, const TInt aMaskIndex, const TSize& aIconSize,
                                const TDesC& aMifFileName, TBool aInsert );		

    protected: // From CActive

        /**
        * From CActive.
		* Handles an active object’s request completion event.
        */
        void RunL();

        /**
        * From CActive.
		* Implements cancellation of an outstanding request.
        * This function is called as part of the active object’s Cancel().
        */
        void DoCancel();
        
        /**
        * From CActive.
		* Handles a leave occurring in the request completion 
        * event handler RunL().
        */
        TInt RunError( TInt aError );

    private: // Constructors

        /**
        * C++ default constructor.
        */
        CIptvLiveUIImageHandler( MIptvLiveUIImageObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

	private: // Private methods
		
		/**
		* Resets iBitmap to empty bitmap.
		*/
		void ResetBitmap();

        
    private: // Data

		/**
		* Live TV Client observer.
		*/
		MIptvLiveUIImageObserver* iObserver;
		
		/**
		* File server session.
		*/
		RFs iFs;

		/**
		* Image decoder.
		*/
		CImageDecoder* iImageDecoder;
		
		/**
		* Image bitmap.
		*/
		CFbsBitmap* iBitmap;
        
    };

#endif      // __CIPTV_LIVE_UI_IMAGEHANDLER_H__
// End of File
