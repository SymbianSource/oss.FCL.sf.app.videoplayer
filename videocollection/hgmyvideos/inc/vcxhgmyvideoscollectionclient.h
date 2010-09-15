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
* Description:      Client class for My Videos MPX Collection.*
*/




#ifndef VCXHGMYVIDEOSCOLLECTIONCLIENT_H
#define VCXHGMYVIDEOSCOLLECTIONCLIENT_H

// INCLUDE FILES
#include <coemain.h>
#include <mpxcollectionobserver.h>

// FORWARD DECLARATIONS
class MMPXCollectionUtility;
class MVcxHgMyVideosCategoryModelObserver;
class MVcxHgMyVideosVideoModelObserver;
class CVcxHgMyVideosDownloadClient;

// CLASS DECLARATION

/**
 * Client class for My Videos MPX Collection.
 *
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosCollectionClient ) : public CBase,
                                                      public MMPXCollectionObserver
    {
    public:
        
        enum TVcxHgMyVideosCollectionCommand
            {
            EVcxHgMyVideosCollectionCommandNone = 0,
            EVcxHgMyVideosCollectionCommandMultiDelete_Starting,
            EVcxHgMyVideosCollectionCommandMultiDelete_Started,
            EVcxHgMyVideosCollectionCommandMultiDelete_Finished,
            EVcxHgMyVideosCollectionCommandCancelDelete_Requested,
            EVcxHgMyVideosCollectionCommandCancelDelete_Completed,
            EVcxHgMyVideosCollectionCommandResort,
            EVcxHgMyVideosCollectionCommandMoveCopy_Starting,
            EVcxHgMyVideosCollectionCommandMoveCopy_Started,
            EVcxHgMyVideosCollectionCommandMoveCopy_Finished,
            EVcxHgMyVideosCollectionCommandCancelMoveCopy_Requested,
            EVcxHgMyVideosCollectionCommandCancelMoveCopy_Completed,
            EVcxHgMyVideosCollectionCommandVideoDetails_Requested
            };
        
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @return New object.
         */
        static CVcxHgMyVideosCollectionClient* NewL();

        /**
         * Two-phased constructor.
         *
         * @return New object.
         */
        static CVcxHgMyVideosCollectionClient* NewLC();

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosCollectionClient();

    public:

        /**
         * Returns Download client.
         *
         * @return Reference to Download client.
         */
        CVcxHgMyVideosDownloadClient& DownloadClient();
        
        /**
         * Returns true if object has pending command for MPX Collection Utility.
         * 
         * @return ETrue if command is pending.
         */
        TBool HasPendingCommand();        
        
        /**
         * Called to set category data observer. Can be called with NULL to
         * reset the observer.
         * 
         * @param aCategoryModelObserver Observer.
         */
        void SetCategoryModelObserver(
                MVcxHgMyVideosCategoryModelObserver* aCategoryModelObserver ); 

        /**
         * Called to set video data observer. Can be called with NULL to
         * reset the observer.
         * 
         * @param aVideoModelObserver Observer.
         */
        void SetVideoModelObserver(
                MVcxHgMyVideosVideoModelObserver* aVideoModelObserver );
        
        /**
         * When called, starts fetching category list. Category list is returned
         * through callback interface MVcxHgMyVideosCategoryModelObserver.
         */
        void GetCategoryListL();        
        
        /**
         * When called, starts fetching video list. Video list is returned
         * through callback interface MVcxHgMyVideosVideoModelObserver.
         * 
         * @param aIndex Index of video list to fetch.
         */
        void GetVideoListL( TInt aIndex );
                
        /**
         * Translates array index to MPX item id by reading the value from array.
         *
         * @param aArrayIndex Index in array.
         * @param aArray Array of MPX Media objects.
         * @return MPX item id
         */
        TMPXItemId ArrayIndexToMpxItemIdL( TInt aArrayIndex, CMPXMediaArray* aArray );
 
        /**
         * Deletes an array of videos.
         * 
         * @param aMediasToDelete Array of MPX Media objects to delete.
         */
        void DeleteVideosL( CMPXMediaArray* aMediasToDelete );

        /**
         * Cancels ongoing deletion of videos. See DeleteVideosL().
         */
        void CancelDeleteVideosL();

        /**
         * Moves or copies array of videos.
         * 
         * @param mediasToMoveOrCopy Array of MPX Media objects to move/copy.
         * @param aTargetDrive Target drive.
         * @param aCopy ETrue of copy operation, EFalse if move.
         */
        void MoveOrCopyVideosL( CMPXMediaArray* mediasToMoveOrCopy,
                                TInt aTargetDrive,
                                TBool aCopy );
        
        /**
         * Cancels ongoing move/copy operation of videos. See MoveOrCopyVideosL().
         */
        void CancelMoveOrCopyVideosL();

        /**
         * Play a video with given MPX item id.
         *
         * @param aMpxItemId MPX item id
         */
        void PlayVideoL( TMPXItemId aMpxItemId );
       
        /**
         * Fetches MPX Media object based on MPX ID. Fetched MPX Media object
         * is returned through callback interface MVcxHgMyVideosVideoModelObserver.
         * 
         * @param aMpxId MPX ID.
         */
        void FetchMpxMediaByMpxIdL( TMPXItemId& aMpxId );
        
        /**
         * Gets all details for the selected MPX Media object.
		 * Result will be returned in HandleCommandComplete().
         * 
         * @param aMpxId MPX ID.
         */
        void GetVideoDetailsL( TMPXItemId& aMpxId );
                
        /**
         * Sets new general flags to given MPX Media object.
         * 
         * @param aMpxItemId MpxId of media object for which flags are set.
         * @param aFlags New flags to be set.         
         */
        void SetFlagsL( TMPXItemId aMpxItemId, TUint32 aFlags );

    public: // From MMPXCollectionObserver

        /**
         * From MMPXCollectionObserver->MMPXCollectionMediaObserver
         * Handle extended media properties
         *
         * @param aMedia media 
         * @param aError error code    
         */
        void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

        /** 
         * From MMPXCollectionUtilityObserver.
         * Handle collection message
         * NOTE: only one of HandleCollectionMessage callback can be implemented
         * 
         * @param aMessage collection message, ownership not transferred. 
         *        Please check aMsg is not NULL before using it. If aErr is not 
         *        KErrNone, plugin might still call back with more info in the aMsg.
         * @param aErr system error code.
         */
        void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr ); 

        /**
         * From MMPXCollectionUtilityObserver.
         * Handles the collection entries being opened. Typically called
         * when client has Open()'d a folder
         *
         * @param aEntries collection entries opened
         * @param aIndex focused entry
         * @param aComplete ETrue no more entries. EFalse more entries
         *                  expected
         * @param aError error code   
         */
        void HandleOpenL( const CMPXMedia& aEntries,
                          TInt aIndex,
                          TBool aComplete,
                          TInt aError );
    
        /**
         * From MMPXCollectionUtilityObserver.
         * Handles the item being opened. Typically called
         * when client has Open()'d an item. Client typically responds by
         * 'playing' the item via the playlist
         *
         * @param aPlaylist collection playlist
         * @param aError error code   
         */                         
        void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError );

        /**
         * From MMPXCollectionUtilityObserver.
         * Handle completion of a asynchronous command
         *
         * @param aCommandResult result of the command, NULL if error
         * @param aError error code    
         */
        void HandleCommandComplete( CMPXCommand* aCommandResult, TInt aError );

    protected: // Constructors

        /**
         * Default C++ constructor. 
         */
        CVcxHgMyVideosCollectionClient();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();

        /** 
         * Called by HandleCollectionMessage(). Handles collection messages.
         * 
         * @param aMessage collection message.
         * @param aError system error code.
         */
        void HandleCollectionMessageL( CMPXMessage* aMessage, TInt aError );

        /** 
         * Called by HandleCollectionMessageL(). Handles single collection message.
         * 
         * @param aMessage collection message.
         * @param aError system error code.
         */
        void HandleSingleCollectionMessageL( CMPXMessage* aMessage, TInt aError );
        
        /**
         * Returns the current level of collection.
         */
        TInt CollectionLevelL();
        
        /**
         * Handles item addition message, called by HandleSingleCollectionMessageL().
         * 
         * @param aMessage collection message.
         * @param aError system error code. 
         */
        void HandleMyVideosItemsAppendedL( CMPXMessage* aMessage, TInt aError );        
        
        /**
         * Handles item changed message, called by HandleSingleCollectionMessageL().
         * 
         * @param aMessage collection message.
         * @param aError system error code. 
         */
        void HandleMyVideosItemsChangedL( CMPXMessage* aMessage, TInt aError );        
        
        /**
         * Handles delete related messages, called by HandleSingleCollectionMessageL().
         * 
         * @param aMessage collection message.
         * @param aError system error code. 
         */
        void HandleMyVideosDeleteMessageL( CMPXMessage* aMessage, TInt aError );        

        /**
         * Handles move or copy related messages, called by HandleSingleCollectionMessageL().
         * 
         * @param aMessage collection message.
         * @param aError system error code. 
         */
        void HandleMyVideosMoveOrCopyMessageL( CMPXMessage* aMessage, TInt aError );        

        /**
         * Handles response message when MPX Media object requested.
         *
         * @param aMessage collection message.
         * @param aError system error code.
         */
        void HandleGetMediasByMpxIdRespL( CMPXMessage* aMessage, TInt aError );
        
        /**
         * Handles list complete message, called by HandleSingleCollectionMessageL().
         * 
         * @param aMessage collection message.
         * @param aError system error code. 
         */
        void HandleMyVideosListCompleteL( CMPXMessage* aMessage, TInt aError );
        
        /**
         * Handles response message when video details requested.
         * 
         * @param aMessage collection message.
         * @param aError system error code.
         */
        void HandleGetVideoDetailsRespL( CMPXMessage* aMessage, TInt aError );
        
        /**
         * Gets current collection path category level index
         * 
         * @return Index or KErrNotFound, if current path is not in video level
         */
        TInt CategoryIndexL();

    protected:
        
        enum TVcxHgCollectionOpenStatus
            {
            EVcxHgCollectionNotOpen = 0,
            EVcxHgCollectionOpening,
            EVcxHgCollectionOpened            
            };
        
    protected:

        /**
         * Pointer to MPX Collection utility.
         * Own.
         */
        MMPXCollectionUtility* iCollectionUtility;
        
        /**
         * Pointer to Download client.
         * Own.
         */
        CVcxHgMyVideosDownloadClient* iDownloadClient;
                
        /**
         * Pointer to observer of category model.
         * Not own.
         */
        MVcxHgMyVideosCategoryModelObserver* iCategoryModelObserver;
        
        /**
         * Pointer to observer of video model.
         * Not own.
         */
        MVcxHgMyVideosVideoModelObserver* iVideoModelObserver;
        
        /**
         * Variable for storing My Videos collection opening status.
         */
        TVcxHgCollectionOpenStatus iCollectionOpenStatus;
        
        /**
         * Variable for storing pending collection command.
         */
        TVcxHgMyVideosCollectionCommand iPendingCommand;

		/**
		 * Variable for storing current collection level.
		 */
        TInt iCollectionLevel;
        
#if defined(_DEBUG) && IPTV_LOGGING_METHOD != 0
    private:
        /**
         * Debug only: Prints list of videos to debug output.
         */
        void DebugPrintVideoListL( CMPXMediaArray* aVideoArray );
#endif // _DEBUG && IPTV_LOGGING_METHOD        

    };

#endif // VCXHGMYVIDEOSCOLLECTIONCLIENT_H
