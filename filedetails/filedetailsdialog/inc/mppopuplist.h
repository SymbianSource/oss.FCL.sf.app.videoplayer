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


// Version : %version: 6 %



#ifndef __CMPPopupList_H__
#define __CMPPopupList_H__

//  INCLUDES
#include <aknPopup.h>   // CAknPopupList
#include <eiklbx.h>
#include <f32file.h>


// FORWARD DECLARATIONS
class CEikListBox;


// CLASS DECLARATION

NONSHARABLE_CLASS( CMPPopupList ): public CAknPopupList
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CMPPopupList* NewL( CEikListBox* aListBox, 
                                   const TDesC& aFilePath,
                                   TBool aDrmProtected,
                                   TBool aReadingDCFOnly );

        /**
         * Two-phased constructor.
         */
        static CMPPopupList* NewL( CEikListBox* aListBox,
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                   RFile64& aFile,
#else
                                   RFile& aFile,
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                   TBool aDrmProtected,
                                   TBool aReadingDCFOnly );

        /**
        * Destructor.
        */
        virtual ~CMPPopupList();

    public: // From CAknPopupList
    	
		/**
		 * Processes events from the softkeys. 
		 * @param aCommandId Event Id from the soft-key.
		 */
        void ProcessCommandL(TInt aCommandId);    
    	   	
		/**
		 * Handle key press events 
		 */
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aEventType );

        /**
         * Handle key press events 
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType);

    public: // new

        /**
         * Re-construct DRM softkeys for playlist
         * @since 9.1
         */
        void ReConstructDrmSoftkeysL( TBool aDrmProtected );
        
    private:
    	
        /**
         * Construct
         * @since 9.1
         */
        void ConstructL( CEikListBox* aListBox, 
					     const TDesC& aFilePath,
					     TBool aDrmProtected,
					     TBool aReadingDCFOnly );

        /**
         * Construct
         * @since 9.1
         */
        void ConstructL( CEikListBox* aListBox,
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                         RFile64& aFile,
#else
                         RFile& aFile,
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                         TBool aDrmProtected,
                         TBool aReadingDCFOnly );

        /**
         * Launch Details View of DRM component 
         * @since 9.1
         */
        void LaunchDrmInfoL();
            	
        /**
         * Update softkeys according to DRM or non-DRM status
         * @since 9.1
         */
        void UpdateSoftkeysL(); 
                
    private: // data
    	
		HBufC* iFilePath;
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        RFile64 iFileHandle;
#else
        RFile iFileHandle;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		TBool iDrmProtected;
		TBool iReadingDCFOnly;
		CEikListBox* iListBox; // not own

    };

#endif // __CMPPopupList_H__
            
// End of File
