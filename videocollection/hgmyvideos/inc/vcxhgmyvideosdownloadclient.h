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
* Description:   CVcxHgMyVideosDownloadClient class definition*
*/




#ifndef VCXHGMYVIDEOSDOWNLOADCLIENT_H
#define VCXHGMYVIDEOSDOWNLOADCLIENT_H

// INCLUDE FILES
#include <coemain.h>
#include "vcxhgmyvideoscollectionclient.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * 
 *
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosDownloadClient ) : public CBase
    {
    public:
        
        enum TVcxHgMyVideosDownloadCommand
            {
            EVcxHgMyVideosDownloadCommandNone   = 0,
            EVcxHgMyVideosDownloadCommandResume = 1,
            EVcxHgMyVideosDownloadCommandCancel = 2            
            };
        
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @param aCollectionClient Reference to MPX Collection client.
         * @param aCollectionUtility Reference to MPX Collection Utility.
         * @return New object.
         */
        static CVcxHgMyVideosDownloadClient* NewL(
                   CVcxHgMyVideosCollectionClient& aCollectionClient,
                   MMPXCollectionUtility& aCollectionUtility);

        /**
         * Two-phased constructor.
         *
         * @param aCollectionClient Reference to MPX Collection client.
         * @param aCollectionUtility Reference to MPX Collection Utility.
         * @return New object.
         */
        static CVcxHgMyVideosDownloadClient* NewLC(
                   CVcxHgMyVideosCollectionClient& aCollectionClient,
                   MMPXCollectionUtility& aCollectionUtility);

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosDownloadClient();

    public:
        
        /**
         * Resumes a paused or failed video download.
         * 
         * @param aMpxMedia Reference to MPX media object to resume.
         */
        void ResumeDownloadL( const CMPXMedia& aMpxMedia );
        
        /**
         * Cancels an ongoing video download.
         * 
         * @param aMpxMedia Reference to MPX media object to cancel.
         */
        void CancelDownloadL( const CMPXMedia& aMpxMedia );

        /**
         * Returns true if object has pending command for MPX Collection Utility.
         * 
         * @return ETrue if command is pending.
         */
        TBool HasPendingCommand();
        
        /**
         * Handles completion of MPX Collection Utility command.
         * 
         * @param aError Error code about command completion.
         */
        void CommandCompleteL( TInt aError );
        
    protected: // Constructors

        /**
         * Default C++ constructor. 
         *
         * @param aCollectionClient Reference to MPX Collection client.
         * @param aCollectionUtility Reference to MPX Collection Utility.
         */
        CVcxHgMyVideosDownloadClient(
            CVcxHgMyVideosCollectionClient& aCollectionClient,
            MMPXCollectionUtility& aCollectionUtility);

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();
        
    protected:

		/**
		 * Reference to MPX Collection client.
		 * Not own.
		 */
		CVcxHgMyVideosCollectionClient& iCollectionClient;

		/**
		 * Reference to MPX Collection Utility.
		 * Not own.
		 */
		MMPXCollectionUtility& iCollectionUtility;
		
		/**
		 * Variable for storing pending download command.
		 */
		TVcxHgMyVideosDownloadCommand iPendingCommand;
    };

#endif // VCXHGMYVIDEOSDOWNLOADCLIENT_H
