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



#ifndef VCXMYVIDEOSCOLLECTIONPLUGINTESTER_H
#define VCXMYVIDEOSCOLLECTIONPLUGINTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <mpxcollectionobserver.h>
#include "MVCXMyVideosCollectionPluginTesterObserver.h"
#include <mpxmediabase.h>
#include <f32file.h>
#include "MIptvTestTimerObserver.h"
#include "VCXMyVideosTestTransactions.h"

// CONSTANTS
const TUint KOpenCollectionActionId =        10000001;
const TUint KOpenCollectionLevelActionId =   10000002;
const TUint KRefreshCollectionActionId =     10000003;
const TUint KGetMediaFullDetailsActionId =   10000004;
const TUint KSideloadVideoActionId =         10000005;
const TUint KRemoveMediaActionId =           10000006;

// MACROS

// FORWARD DECLARATIONS
class MMPXCollectionUtility;
class CVCXTestCommon;
class CVCXMyVideosTestDlWatcher;
class CIptvTestActiveWait;
class CVCXMyVideosTestDownload;
class CIptvTestTimer;
class CVCXTestStatsKeeper;

// DATA TYPES

// CLASS DECLARATION

/**
*  CVCXMyVideosCollectionPluginTester test class
*
*  @lib ?library
*  @since ?Series60_version
*/
class CVCXMyVideosCollectionPluginTester : public CBase, 
                                           public MMPXCollectionObserver,
                                           public MIptvTestTimerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVCXMyVideosCollectionPluginTester* NewL( MVCXMyVideosCollectionPluginTesterObserver* aObserver,
                CVCXTestCommon* aTestCommon, CVCXTestStatsKeeper* aStatsKeeper );

        /**
        * Destructor.
        */
        virtual ~CVCXMyVideosCollectionPluginTester();

    public: // New functions

        /**
         * Opens MPX collection.
         * Observer gets message when action is finished.
         */
        void OpenCollectionL( TUint32 aCollectionUid );

        /**
         * Opens MPX collection level.
         * Observer gets message when action is finished.
         */
        void OpenLevelL( TInt aIndex );

        /**
         * Gets medias by their MPX IDs. A collection must be opened before calling this.
         * The returned medias are handled as when opening collection.
         * @param aStartIndex, start index in iMediaArray, first media to get.
         * @param aEndIndex, end index in iMediaArray, last media to get.
         */
        void GetMediasByMpxIdL( TInt aStartIndex, TInt aEndIndex, TBool aSync );

        /**
         * Opens MPX collection level.
         * Observer gets message when action is finished.
         */
        void RefreshContentsL();

        /**
         * Closes MPX collection level or collection.
         * Observer gets message when action is finished.
         */
        void CloseLevelL();

        /**
         * Gets item from MPX and prints details for it. Copy of the media is 
         * stored to member variable and can be fetched with GetLastFullMedia. 
         * Observer gets message when action is finished.
         */
        void GetMediaFullDetailsL( TInt aDrive, TInt aIndex );
        
        /**
         * Gets item from MPX and prints details for it. Copy of the media is 
         * stored to member variable and can be fetched with GetLastFullMedia. 
         * Observer gets message when action is finished.
         */
        void GetMediaFullDetailsByMpxIdL( TInt aDrive, TInt aIndex, TBool aSync );
        
        /**
         * Gets the full details for all of the medias and prints them to debug output. 
         */
        void GetAllMediaFullDetailsL();

        /**
         * Prints details for items in the current MPX level and
         * updates the downloads. This is done when collection or collection
         * level is opened.
         */
        void ProcessCurrentEntriesL();

        /**
         * Prints MPX media object properties.
         */
        void PrintMediasL( CMPXMediaArray* aMediaArray, TBool aCheckChanges, const TDesC& aTitle );
        
        /**
         * Prints MPX media object properties to debug output.
         */
        void PrintMPXMediaL( const CMPXMedia& aMedia, TBool aPrintAllDetails );
        
        /**
         * Returns count of items of the open MPX level.
         */
        TInt GetMediaCount();

        /**
         * Adds media to the collection.
         * Observer gets message when action is finished.
         */
        void AddMediaL( CMPXMedia* aMedia, TBool aSync );

        /**
         * Updates media in the collection.
         * Observer gets message when action is finished.
         */
        void SetMediaL( CMPXMedia* aMedia, TBool aSync );

        /**
         * Removes media item with command KMPXCommandIdCollectionRemoveMedia to MV MPX plugin.
         */
        void RemoveMediaL( TInt aDrive, TInt aIndex, TBool aSync );
        
        /**
         * Removes media item with command KMPXCommandIdCollectionRemoveMedia to MV MPX plugin.
         */
        void RemoveMediaByMpxIdL( TInt aMpxId, TBool aSync );

        /**
         * Deletes files of the current media entries.
         */
        void DeleteAllMediaFilesL();

        /**
         * Deletes a file of a media in current entries.
         */
        void DeleteFileOfMediaL( TInt aDrive, TInt aIndex );

        /**
         * Gets MPX media from current entries. Contains only brief details of the item.
         */
        CMPXMedia* GetMediaL( TInt aDrive, TInt aIndex );

        /**
         * Moves multiple medias to specified drive.
         * 
         * @param aSourceDrive, -1 is all drives 
         * @param aStartIndex
         * @param aEndIndex 
         * @param aDestDrive
         * @param aSync
         */
        void MoveMediasL(  TInt aSourceDrive, TInt aStartIndex, TInt aEndIndex, TInt aDestDrive, TBool aSync );

        /**
         * Cancels move or copy request.
         */
        void CancelMoveOrCopyL( TBool aSync );

        /**
         * Copies multiple medias to specified drive.
         * 
         * @param aSourceDrive, -1 is all drives 
         * @param aStartIndex
         * @param aEndIndex 
         * @param aDestDrive
         * @param aSync
         */
        void CopyMediasL(  TInt aSourceDrive, TInt aStartIndex, TInt aEndIndex, TInt aDestDrive, TBool aSync );
        
        /**
         * Deletes multiple medias on specified drive.
         * 
         * @param aSourceDrive, -1 is all drives 
         * @param aStartIndex
         * @param aEndIndex 
         * @param aSync
         */
        void DeleteMediasL(  TInt aSourceDrive, TInt aStartIndex, TInt aEndIndex, TBool aSync );
        
        void DeleteMediasByMpxIdsL( CMPXMediaArray* aMedias, TBool aSync );        
        
        void DeleteMediaByMpxIdL( TMPXItemId& aMpxId, TBool aSync );

        /**
         * Cancels move request.
         */
        void CancelDeleteL( TBool aSync );
        
        /**
         * Use to toggle automatic refresh of contents when open collection changes.
         */
        void SetAutomaticRefresh( TBool aValue );
        
        /**
         * Returns true if refresh is ongoing.
         */
        TBool IsRefreshing();
        
        /**
         * Use to disable some of the logging.
         */
        void SetQuietMode( TBool aValue );

        /**
         * Starts MPX My Videos download.
         * @param aTitle
         * @param aIapId
         * @param aServiceId
         * @param aContentId
         * @param aUrl
         * @param aSync
         * @param aUserName
         * @param aPassword
         * @param aMedia
         */
        void StartDownloadL( const TDesC& aTitle, TInt aIapId, TInt aServiceId, TInt aContentId,
                const TDesC& aUrl, TBool aSync, const TDesC& aUserName, const TDesC& aPassword,
                CMPXMedia* aMedia );
        
        /**
         * Resumes MPX My Videos download.
         * If download is not found from cache then fake download ID is used.
         * 
         * @param aTitle
         * @param aIapId
         * @param aServiceId
         * @param aContentId
         * @param aUrl
         * @param aSync
         * @param aUserName
         * @param aPassword
         * @param aMedia
         */
        void ResumeDownloadL( const TDesC& aTitle, TInt aIapId, TInt aServiceId, TInt aContentId,
                const TDesC& aUrl, TBool aSync, const TDesC& aUserName, const TDesC& aPassword,
                CMPXMedia* aMedia );        

        /**
         * Resumes MPX My Videos downloads.
         */
        void ResumeAllDownloadsL();
        
        /**
         * Cancels MPX My Videos download.
         */
        void CancelDownloadL( CVCXMyVideosTestDownload* aDownload, TBool aSync );

        /**
         * Cancels MPX My Videos download.
         */
        void CancelDownloadL( TInt aMpxId, TInt aDownloadId, const TPtrC& aDownloadPath, TBool aSync );

        /**
         * Pauses MPX My Videos download.
         */
        void PauseDownloadL( TInt aServiceId, TInt aContentId, const TDesC& aUrl, TBool aSync );

        /**
         * Pauses MPX My Videos download.
         */
        void PauseDownloadL( const TDesC& aUrl, TBool aSync  );

        /**
         * Returns instance of the download watcher.
         */
        CVCXMyVideosTestDlWatcher* GetDownloadWatcher();

        /**
         * Returns number of active downloads.
         */
        TInt GetActiveDownloadCountL();

        /**
         * Sets automatic resuming of paused download.
         */
        void SetAutoResume( TBool aValue );

        /**
         * Returns the index of current collection level.
         */
        TInt GetCurrentLevel();
        
        /**
         * Returns the currently opened level index (category).
         */
        TInt GetCurrentOpenLevelIndex();

        /**
         * Returns the media from last call to GetMediaDetailsL.
         */
        CMPXMedia* GetLastFullMedia();

        /**
         * Gets index of media in collection side array.  
         */
        TInt GetMediaIndexInCollectionL( TInt aDrive, TInt aIndex );
        
        /**
         * Returns the video count on specified drive. 
         */
        TInt GetVideoCountForDrive( TInt aDrive );
        
        /**
         * Returns all medias from collection side.
         */
        const CMPXMediaArray* GetAllCollectionMedia();

        /**
         * Ensures that files are not in use.
         */
        void EnsureMediaFilesAreNotInUseL();
        
        /**
         * Creates media array with following rules:
         * If startIndex is > 0 and < endIndex then these files will be selected from specified drive.
         * If startIndex is -1 then invalid IDs will be used and end index is count.
         * If startIndex is -2 then duplicate MPX IDs are requested starting from 0.
         * If startIndex is -3 then only every second MPX item is used in request and starting from 0.
         * If startIndex > endIndex then empty array is returned. 
         *
         * @param aDriveFilter, -1 is all files. This is ignored for all other rules but the first.
         *                      When drive is specified selection starts from index 0. 
         * @param aStartIndex index at current entries.
         * @param aEndIndex, index at current entries.
         */
        CMPXMediaArray* SelectMediasL( TInt aDriveFilter, TInt aStartIndex, TInt aEndIndex );
        
    private:
        /**
         * Updates the downloads using current MPX entries.
         */
        void UpdateDownloadsL( TBool aQuietMode );
        
        /** 
         * Creates a MPX command.
         * 
         * @param aCommandGeneralId
         * @param aMyVideosCommandId ( this is used only if aCommandGeneralId is My Videos collection specific.    
         * @param aAsync
         */
        CMPXCommand* CreateMpxCommandLC( TInt aCommandGeneralId, TInt aMyVideosCommandId, TBool aAsync );
            
        /**
         * Ensures that result medias array has all the medias of requested array. 
         */
        void CheckRequestMediaArrayL( CMPXMediaArray& aRequestResultMedias, RArray<TInt32>& aRequestedMediaIds );

        /**
         * Creates copies of medias in iCollectionEntries to iMediaArray and old media array is moved to 
         * iOldMediaArray.
         */
        void UpdateOwnedMediaArrayL();
        
        /**
         * Compares iMediaArray to iOldMediaArray and returns copies if any of the medias are different.
         */
        CMPXMediaArray* GetChangedMedias( const CMPXMediaArray* aNewMedias );        
        
        /**
         * Returns media from the aMedias array for specified mpx id.
         */
        CMPXMedia* GetMediaByMpxId( CMPXMediaArray* aMedias, TMPXItemId& aMpxId );
        
        /**
         * Sets status of refresh.
         */
        void SetRefreshStatus( TBool aRefreshingCollection );
        
    public: // Functions from base classes

        /**
        *  From MMPXCollectionObserver->MMPXCollectionMediaObserver
        *  Handle extended media properties
        *
        *  @param aMedia media
        *  @param aError error code
        */
        virtual void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

        /**
        *  From MMPXCollectionUtilityObserver.
        *  Handle collection message
        *  NOTE: only one of HandleCollectionMessage callback can be implemented
        *
        *  @param aMessage collection message, ownership not transferred.
        *         Please check aMsg is not NULL before using it. If aErr is not
        *         KErrNone, plugin might still call back with more info in the aMsg.
        *  @param aErr system error code.
        */
        void HandleCollectionMessage( CMPXMessage* aMsg, TInt /*aErr*/ );

        void HandleSingleCollectionMessageL( CMPXMessage* aMessage );

        /**
        *  From MMPXCollectionUtilityObserver.
        *  Handles the collection entries being opened. Typically called
        *  when client has Open()'d a folder
        *
        *  @param aEntries collection entries opened
        *  @param aIndex focused entry
        *  @param aComplete ETrue no more entries. EFalse more entries
        *                   expected
        *  @param aError error code
        */
        void HandleOpenL( const CMPXMedia& aEntries,
                          TInt aIndex,TBool aComplete,TInt aError );

        /**
        *  From MMPXCollectionUtilityObserver.
        *  Handles the item being opened. Typically called
        *  when client has Open()'d an item. Client typically responds by
        *  'playing' the item via the playlist
        *
        *  @param aPlaylist collection playlist
        *  @param aError error code
        */
        void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,TInt aError );

        /**
        *  From MMPXCollectionUtilityObserver.
        *  Handle completion of a asynchronous command
        *  @param aCommandResult result of the command, NULL if error
        *  @param aError error code
        */
        void HandleCommandComplete( CMPXCommand* aCommandResult,
                                    TInt aError);

        /**
        * From MIptvTestTimerObserver Handles timer completion
        * @since
        * @param aTimerId
        * @param aError
        */
        void TimerComplete(TInt aTimerId, TInt aError);
        
    private:

        /**
        * C++ default constructor.
        */
        CVCXMyVideosCollectionPluginTester( MVCXMyVideosCollectionPluginTesterObserver* aObserver, 
                CVCXTestCommon* aTestCommon, CVCXTestStatsKeeper* aStatsKeeper );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        // MVCXMyVideosCollectionPluginTesterObserver
        MVCXMyVideosCollectionPluginTesterObserver* iObserver;

        // UID of currently open collection.
        TUid iCollectionUid;

        // The index of last level which was opened.
        TInt iCurrentOpenedLevelIndex;
        
        HBufC* iCurrentLevelName;

        // MMPXCollectionUtility.
        MMPXCollectionUtility* iCollectionUtility;

        // Entries from collection.
        CMPXMedia* iCollectionEntries;

        // Media array of videos on collection side. 
        CMPXMediaArray* iCollectionMediaArray;
        
        // Media array of videos tests uses.
        CMPXMediaArray* iMediaArray;

        // Media array of previous videos.
        CMPXMediaArray* iOldMediaArray;

        // Count of current medias in iMediaArray.
        TUint32 iMediaCount;

        // CVCXTestCommon.
        CVCXTestCommon* iTestCommon;

        // RFs.
        RFs iFs;

        // Refresh is already ongoing.
        TBool iRefreshingCollection;

        // Member to keep track of downnloads.
        CVCXMyVideosTestDlWatcher* iDlWatcher;

        // Active wait
        CIptvTestActiveWait* iActiveWait;

        // Refresh contents of open collection when they change
        TBool iAutomaticContentRefresh;

        // Tester is waiting for item change.
        TBool iWaitingForItemChange;

        // Autoresume downloads
        TBool iAutoResume;

        // Downloads do not start updating before we get DL related commands from script.
        TBool iUpdateDownloads;

        TUint32 iCurrentMyVideosMessageId;

        RArray<TInt32> iRequestedMediaIds;
        
        // Full details of media got from GetMediaDetailsL
        CMPXMedia* iFullMedia;

        TBool iGettingFullDetailsForAllMedia;

        // Item ids which have been deleted in change event.
        RArray<TUint32> iDeletedItemIds;

        // Item ids which have been inserted in change event.
        RArray<TUint32> iInsertedItemIds;
        
        CVCXMyVideosTestTransactions* iTransactions;
        
        TBool iQuietMode;
        
        // Canceling Move, Copy or Delete cause commmand completion with error KErrCancel  
        TBool iCancelRequested;
        
        // Update downloads only after first one is started.
        TBool iDownloadsStarted;
        
        TInt iActionCount;
        
        // Download progress update timer.
        CIptvTestTimer* iProgressTimer;
        
        TInt iVideosOnRomCount;
        
        // Variable to follow what kind of collection opening is going on.  
        TInt iCurrentActionId; 
        
        // Variable to follow where action ending should be handled. 
        TBool iCurrentActionHasResponse;
        
        // Class to hold robustness and performance data.
        CVCXTestStatsKeeper* iStats;
    };

#endif      // VCXMYVIDEOSCOLLECTIONPLUGINTESTER_H

// End of File
