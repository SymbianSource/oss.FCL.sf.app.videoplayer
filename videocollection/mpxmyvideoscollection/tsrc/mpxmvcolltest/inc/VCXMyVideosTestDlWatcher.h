/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description*
*/



#ifndef VCXMYVIDEOSTESTDLWATCHER_H
#define VCXMYVIDEOSTESTDLWATCHER_H

//  INCLUDES
#include <f32file.h>
#include <vcxmyvideosdefs.h>

#include "MVCXMyVideosCollectionPluginTesterObserver.h"

// CONSTANTS
const TInt KVCXTestProgressUpdateInterval( 10 );

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// FORWARD DECLARATIONS
class CVCXTestStatsKeeper;

// CLASS DECLARATION

/**
*  CVCXMyVideosTestDownload test class
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class CVCXMyVideosTestDownload : public CBase
{
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CVCXMyVideosTestDownload* NewL();

        /**
        * Destructor.
        */
        virtual ~CVCXMyVideosTestDownload();

    public: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CVCXMyVideosTestDownload();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

    public: // Data
        HBufC* iTitle;

        HBufC* iUrl;

        HBufC* iPath;

        HBufC* iUserName;

        HBufC* iPassword;

        TBool iSyncCall;

        TInt iIapId;

        TInt iState;

        TInt iProgress;

        TUint32 iServiceId;

        TUint32 iContentId;

        TInt iDownloadError;

        TInt iGlobalError;

        TInt iMpxId;

        TUint32 iDownloadId;

        TInt iInformed;

        // This is set during download check.
        TBool iExists;

        // Set when download is marked to be canceled.
        TBool iCanceled;

        TBool iWaitingResume;
        
        TBool iWaitingPause;
};

/**
*  CVCXMyVideosTestDlWatcher test class
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class CVCXMyVideosTestDlWatcher : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVCXMyVideosTestDlWatcher* NewL( MVCXMyVideosCollectionPluginTesterObserver* aObserver, CVCXTestStatsKeeper* aStatsKeeper  );

        /**
        * Destructor.
        */
        virtual ~CVCXMyVideosTestDlWatcher();

    public: // New functions

        /**
         * Creates an empty download item.
         */
        void CreateDownloadL( TInt aIapId, TInt aServiceId, TInt aContentId, const TDesC& aUrl, TBool aSync, const TDesC& aUserName, const TDesC& aPassword );

        /**
         * Called when downloads starts. Updates the attributes of download.
         */
        void StartDownloadL( const TDesC& aUrl, const TDesC& aPath, TInt aDownloadId, TInt aMpxId, const TDesC& aTitle, TVcxMyVideosDownloadState aState, TInt aProgress );

        /**
         * Updates download state.
         */
        void UpdateDownloadStateL( TInt aMpxId, TInt aDownloadId, TVcxMyVideosDownloadState aState, TInt aProgress, TInt aDownloadError, TInt aGlobalError );

        /**
         * Updates download progress.
         */
        void UpdateDownloadProgressL( TInt aMpxId, TInt aDownloadId, TInt aProgress );

        /**
         * Cancels a download.
         */
        void CancelDownloadL( TInt aServiceId, TInt aContentId, const TDesC& aUrl );

        void SetDownloadResumedFlagL( TInt aMpxId, TInt aDownloadId );
        
        void SetDownloadPausedFlagL( TInt aMpxId, TInt /* aDownloadId */ );

        /**
         * Returns DL with specified mpx ID.
         */
        CVCXMyVideosTestDownload* GetDownloadByMpxId( TInt aMpxId );

        /**
         * Returns DL with specified download ID.
         */
        CVCXMyVideosTestDownload* GetDownloadByDownloadId( TInt aDownloadId );


        /**
         * Returns DL with specified parameters.
         */
        CVCXMyVideosTestDownload* GetDownload( TInt aServiceId, TInt aContentId, const TDesC& aUrl );

        /**
         * Returns DL with specified URL. If there's several DLs with same URL then DL with state none is preferred.
         */
        CVCXMyVideosTestDownload* GetDownload( const TDesC& aUrl );

        /**
         * Returns DL at specified index.
         */
        CVCXMyVideosTestDownload* GetDownloadByIndex( TInt aIndex );

        TInt GetDownloadCount();

        TInt GetFailedDownloadCount();

        void CleanupDownloadsL( TInt aServiceId, TInt aContentId, const TDesC& aUrl );

        /**
         * Prints download information to debug output.
         */
        void PrintDownloads();

        /**
         * Prints URL to debug output.
         */
        void PrintUrl( const TDesC& aPre, const TDesC& aUrl );

        /**
         * Call before updating all the downloads. Upon update downloads are checked for validity.
         */
        void PrepareCheck();

        /**
         * Call after updating the downloads. If there's any inconsistencies with downloads the test scripter
         * will be signaled.
         */
        void FinishCheckL();

        /**
         * Disables check for missing downloads.
         */
        void DisableDownloadCheck();

        /**
         * Removes all download entries.
         */
        void Reset();

    private: // New functions

        const TDesC& GetStateDesc( TVcxMyVideosDownloadState aState );

    private:

        /**
        * C++ default constructor.
        */
        CVCXMyVideosTestDlWatcher( CVCXTestStatsKeeper* aStatsKeeper );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MVCXMyVideosCollectionPluginTesterObserver* aObserver );
        
    private:    // Data

        MVCXMyVideosCollectionPluginTesterObserver* iObserver;

        RPointerArray<CVCXMyVideosTestDownload> iDownloads;

        TBool iDlCheckActive;
        TBool iDlCheckDisabled;

        RFs iFs;

        TBool iMpxCollectionRefreshing;
        
        CVCXTestStatsKeeper* iStats;
    };

#endif      // VCXMYVIDEOSTESTDLWATCHER_H

// End of File
