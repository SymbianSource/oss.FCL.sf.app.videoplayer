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
* Description:    Implementation of My Videos MPX Collection Plugin interface*
*/




#ifndef VCXMYVIDEOSCOLLECTIONPLUGIN_H
#define VCXMYVIDEOSCOLLECTIONPLUGIN_H

// INCLUDES

#include <mpxcollectionplugin.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>

#include "vcxmyvideosactivetask.h"

#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideosdownloadutil.h"

// FORWARD DECLARATIONS
class CVcxMyVideosMdsDb;
class CMPXResource;
class CMPXMediaArray;
class CVcxMyVideosDownloadUtil;
class CVcxMyVideosVideoCache;
class CVcxMyVideosCategories;
class CVcxMyVideosMessageList;
class CVcxMyVideosAsyncFileOperations;
class CVcxMyVideosOpenHandler;

// CONSTANTS

// CLASS DECLARATION

/**
* Plug-in, provides access to My Videos collection.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosCollectionPlugin) :
    public CMPXCollectionPlugin,
    public MVcxMyVideosMdsDbObserver,
    public MVcxMyVideosActiveTaskObserver,
    public MVcxMyVideosDownloadUtilObserver
    {    
    public: // Constructors and destructor

        friend class CVcxMyVideosOpenHandler;
        friend class CVcxMyVideosVideoCache;
        
        /**
        * Two-phased constructor
        * @param aInitParams: initialization parameters
        * @return object constructed
        */
        static CVcxMyVideosCollectionPlugin* NewL (TAny* aInitParams);

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosCollectionPlugin();

    public: // from base classes
        /**
        * From CMPXCollectionPlugin
        * Navigates to the given path.  This method is serviced
        * asynchronously and HandleOpenL is called upon completion
        * @param aPath a path
        * @param aAttrs attributes requested
        * @param aFilter filter to apply or NULL if none
        */
        void OpenL(const CMPXCollectionPath& aPath,
                   const TArray<TMPXAttribute>& aAttrs,
                   CMPXFilter* aFilter);
        
        /**
        * From CMPXCollectionPlugin
        * Get the extended properties of the item referred to by the
        * specified collection path.  This method is serviced asynchronously and
        * HandleMediaL is called upon completion
        * @param aPath a collection path referring to the item to retrieve the
        *               extended properties for
        * @param aAttr: array of attributes requested
        * @param aCaps platsec capabilities of client requesting media; plug-in
        *        should also verify its process capabilities (not yet supported)
        * @aParam aSpecs specifications for attributes (not yet supported)
        */
        void MediaL(const CMPXCollectionPath& aPath,
                    const TArray<TMPXAttribute>& aAttrs,
                    const TArray<TCapability>& aCaps,
                    CMPXAttributeSpecs* aSpecs);

        /**
        * From CMPXCollectionPlugin
        * Cancel outstanding request.
        */
        void CancelRequest();

        /**
        * From CMPXCollectionPlugin
        * Executes the given CMPXCommand command on the collection
        * @param aCmd a command
        */
        void CommandL(CMPXCommand& aCmd);

        /**
        * From CMPXCollectionPlugin
        *  Find the items matching the media specifications.  This method is serviced
        *  asynchronously and HandleFindAllL is called upon completion
        *  @param aCriteria Properties of the item that is to be found
        *  @param aAttrs Requested attributes to return
        */
        void FindAllL(const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs);

        /**
        *  From CMPXCollectionPlugin
        *  Find the items matching the media specifications
        *  @param aCriteria Properties of the item that is to be found
        *  @param aAttrs Requested attributes to return
        *  @return results of the search. Method abandons ownership of results.
        */
        CMPXMedia* FindAllSyncL(const CMPXMedia& aCriteria,
                                const TArray<TMPXAttribute>& aAttrs);

        /**
        * Get the list of supported capabilities
        * @return TCollectionCapability, bitmask of supported capabilities
        */
        TCollectionCapability GetCapabilities();

        /**
        * DEPRECATED
        * From CMPXCollectionPlugin
        * Executes the given TMPXCollectionCommand command on the collection
        * @param aCmd a command
        * @param aArg an argument
        */
        void CommandL(TMPXCollectionCommand /*aCmd*/, TInt /*aArg = 0*/) {}

        /**
        * DEPRECATED
        * From CMPXCollectionPlugin
        * Adds an episode to the collection
        * @param aMedia Properties of the item
        */
        void AddL(const CMPXMedia& /*aMedia*/) {}

        /**
        * DEPRECATED
        * From CMPXCollectionPlugin
        * Remove an item(s) from the collection database using the given path
        * Note that the selection indices are hidden within the path
        * @param aPath: path to the item to be removed
        *
        */
        void RemoveL(const CMPXCollectionPath& /*aPath*/) {}

        /**
        *  DEPRECATED
        *  Remove item(s) from the collection database using the given media properties
        *  @param aMedia Properties of the item(s) to be removed. May cantain a URI
        *                 or metadata. All item(s) matching the specified properties
        *                 will be removed.
        */
        void RemoveL(const CMPXMedia& /*aMedia*/) {}

        /**
        *  DEPRECATED
        *  Sets/updates the specified media for an item in the collection.  Media
        *  is identified by specifying an ID or URI.
        *  @param aMedia: new property values for the item
        */
        void SetL(const CMPXMedia& /*aMedia*/) {}

        /**
        * From MMPXDbActiveTaskObserver
        */
        TBool HandleStepL();

        /**
        * From MMPXDbActiveTaskObserver
        */
        void HandleOperationCompleted( TInt aErr );

        /**
        * From MVcxMyVideosMdsDbObserver.
        */
        void HandleMyVideosDbEvent( TMPXChangeEventType aEvent,
                RArray<TUint32>& aId );

        /**
        * Leaving version of HandleMyVideosDbEvent.
        */
        void DoHandleMyVideosDbEventL( TMPXChangeEventType aEvent,
                RArray<TUint32>& aId );
                
        /**
        * From MVcxMyVideosMdsDbObserver.
        * Handler function for list fetching events. This callback is called as a response
        * to CreateVideoList( aSync = EFalse ).
        *
        * @param aVideoList           List of videos fetched from MDS.
        * @param aNewItemsStartIndex  Start position for the new items added.
        * @param aComplete            ETrue if list is complete and no more items are expected,
        *                             EFalse if there are more to come.
        */
        void HandleCreateVideoListResp( CMPXMedia* aVideoList, TInt aNewItemsStartIndex,
                TBool aComplete );

        /**
        * From MVcxMyVideosMdsDbObserver. Called when media is removed or inserted.
        */
        void HandleObjectPresentNotification();

        /**
        * Leaving version of HandleObjectPresentNotification.
        */
        void DoHandleObjectPresentNotificationL();
                
        /**
        * From CVcxMyVideosDownloadUtilObserver.
        */
        void HandleDlEvent( TVcxMyVideosDownloadState aState,
                TUint32 aDownloadId, TInt aProgress, TInt64 aDownloaded,
                TInt32 aError, TInt32 aGlobalError );

        /**
        * Leaving version of HandleDlEvent.
        */
        void DoHandleDlEventL( TVcxMyVideosDownloadState aState,
                TUint32 aDownloadId, TInt aProgress, TInt64 aDownloaded,
                TInt32 aError, TInt32 aGlobalError );
 
    private:
        /**
        * Constructor
        */
        CVcxMyVideosCollectionPlugin();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL ();

        /**
        * Synchronizes set of videos in cache with Download Manager.
        * Download related attributes are set in cache if corresponding
        * download is found.
        *
        * @param aItemsInCache  Array of MDS IDs. 
        */
        void SyncWithDownloadsL( RArray<TUint32>& aItemsInCache );

        /**
        * Synchronizes video with Download Manager.
        *
        * @aVideo       Video to synchronize.
        * @aEventAdded  Set to ETrue if event was added to iMessageList, does not change the
        *               value otherwise.
        */
        void SyncVideoWithDownloadsL( CMPXMedia& aVideo, TBool& aEventAdded,
                TBool aAddEvent = ETrue );

        /**
        * Copies download attributes from aDownload to aVideo.
        *
        * @param aVideo        Video in cache.
        * @param aDownload     Corresponding download in Download Manager.
        * @param aSendMpxEvent ETrue if MPX item has been modified and caller should
        *                      send the modification event.
        */
        void SyncVideoAndDownloadL(
                CMPXMedia& aVideo,
                RHttpDownload& aDownload,
                TBool& aSendMpxEvent );
                        
        /**
        * Copies KVcxMediaMyVideosTransactionId field from aRequest to aResp.
        */
        void SetTransactionIdL( CMPXMedia& aRequest, CMPXMedia& aResp );
               
        /**
        * Generates and sends resp message for Move,Copy and Delete operations.
        * Does nothing if there is some other operation going on.
        * This function is called when operation is interrupted from a leave or
        * cancel request.
        * Leaves with KErrNotFound if no operation was in progress.
        *
        * @param aErr  Error code which is set for non handled items in the resp array.
        */
        void CancelOperationL( TInt aErr );
 
        /**
        * Returns reference to iAsyncFileOperations member.
        *
        * @return reference to iAsyncFileOperations.
        */        
        CVcxMyVideosAsyncFileOperations& AsyncFileOperationsL();
        
        /**
        * Called by iCache when it notices that current video list fetching has to
        * be done again from the scratch. This is called only when there is video list
        * fetching ongoing.
        */
        void RestartVideoListFetchingL();


        /**
        * Sets download ID to 0 and download state to none. Updates MDS and sends
        * modified event.
        *
        * @param aDownloadId  Download ID of the MPX item.
        */
        void SetDownloadIdToZeroL( TUint aDownloadId );


#ifdef _DEBUG
        /**
        * Returns download state as a descriptor.
        */
        const TDesC& DownloadState( TUint8 aDlState );
#endif

    public:

        /**
        * Adds video to MDS and cache.
        *
        * @param aVideo video to add, KMPXMediaGeneralId will be set to new value.
        */
        void AddVideoToMdsAndCacheL( CMPXMedia& aVideo );

        /**
        * Sets video parameters in iCache and MDS from aVideo.
        *
        * @param aVideo  Media used to copy parameters from.
        */
        void SetVideoL( CMPXMedia& aVideo );

        /**
        * Sends My Videos message to clients. Transaction ID is copied from
        * aCommand if it is available.
        *
        * @param aMessageId Message id.
        * @param aCommand   Command which has caused the message, optional.
        *                   Ownership does not move.
        */
        void SendMyVideosMessageL( TUint32 aMessageId, CMPXCommand* aCommand = NULL );

        /**
        * Synchronizes video list with Download Manager.
        *
        * @param aVideoList  Video list to synchronize.
        * @param aStartPos   Items before aStartPos index are left out from synchronizing.
        * @param aSendEvents If ETrue and modifications are done to the list, then
        *                    MPX events are sent.
        */
        void SyncVideoListWithDownloadsL( CMPXMedia& aVideoList, 
                TBool aSendEvents, TInt aStartPos = 0 );
        
        /**
        * Returns reference to iDownloadUtil. If iDownloadUtil is NULL then
        * it is created.
        */
        CVcxMyVideosDownloadUtil& DownloadUtilL();

        /**
        * Sends messages to clients. If udeb version, adds messageid
        * parameter to the message for debugging purposes. May or may not
        * contain media list as parameter.
        *
        * @param aMessages  Messages to send.
        */
        void SendMessages( CMPXMessage& aMessages );

        /**
        * Creates iCategories if its not created yet and returns reference to it.
        *
        * @return Reference to iCategories member.
        */
        CVcxMyVideosCategories& CategoriesL();
        
        /**
        * Notifies to mediator listeners that a download has completed.
        *
        * @param aVideo Downloaded video.
        */
        void NotifyDownloadCompletedL( CMPXMedia& aVideo );

        /**
        * Notifies to mediator listeners that count of new videos has decreased.
        * 
        * @param aVideo Removed or watched video.  
        */
        void NotifyNewVideosCountDecreasedL( CMPXMedia& aVideo );
        
    public:
        /**
        * MDS database handler, own.
        */
        CVcxMyVideosMdsDb* iMyVideosMdsDb;

        /**
        * MDS items are cached here for fast access.
        * Own.
        */
        CVcxMyVideosVideoCache* iCache;

        /**
        * Messages are grouped here for sending.
        */        
        CVcxMyVideosMessageList* iMessageList;

        /**
        * File server session.
        */
        RFs iFs;

        /**
        * Executes command in several small steps.
        */
        CVcxMyVideosActiveTask* iActiveTask;
                
    private:

        /**
        * Contains category list, accessed through CategoriesL() function.
        * Own.
        */
        CVcxMyVideosCategories* iCategories;
                            
        /**
        * Download util, own.
        */
        CVcxMyVideosDownloadUtil* iDownloadUtil;
                                                                        
        /**
        * Target drive for Move operation.
        */
        TInt iTargetDrive;
        
        /**
        * Performs multiple Move,Copy and Delete operations step by step. Own.
        */
        CVcxMyVideosAsyncFileOperations* iAsyncFileOperations;
        
        /**
        * Contains collection Open operation related functionality.
        */
        CVcxMyVideosOpenHandler* iOpenHandler;
        
        /**
        * Used for clearing orphan downloads only once.
        */
        TBool iOrphanDownloadsCleared;

                        
#ifdef _DEBUG
        /**
        * Used for debugging that client receives all events.
        */
        TInt iTotalMessagesSent;
        TInt iMessagesInArraySent;
#endif

    };

#endif   // VCXMYVIDEOSCOLLECTIONPLUGIN_H


