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
* Description:    Class for handling mpx collection interactions*
*/




#ifndef VCXNSDOWNLOADCLIENTHANDLER_H_
#define VCXNSDOWNLOADCLIENTHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "CIptvUtil.h"
#include <mpxcollectionobserver.h>

#include "vcxnsdownloadupdater.h"

// FORWARD DECLARATIONS
class CVcxNsUiEngine;
class CVcxNsService;
class CVcxNsContent;
class MMPXCollectionUtility;
class CMPXMedia;

/**
* Class CVcxNsMpxCollectionClientHandler handles mpx collection client interactions.
*
* @lib vcxnsuiengine.lib
*/
class CVcxNsMpxCollectionClientHandler : public CBase,
                                         public MMPXCollectionObserver,
                                         public MVcxNsDownloadUpdaterObserver
    {

public:

    /**
     * Two-phased constructor. 
     * 
     * @param aUiEngine Reference to CVcxNsUiEngine.
     */
    static CVcxNsMpxCollectionClientHandler* NewL( CVcxNsUiEngine& aUiEngine );

    /**
     * Destructor.
     */    
    virtual ~CVcxNsMpxCollectionClientHandler();
    
    /**
     * Starts download of given item.
     * 
     * @param aService Service data for downloaded iten
     * @param aContent Content data for downloaded item
     * @return System-wide error code
     */
    TInt DownloadL( CVcxNsService& aService, CVcxNsContent& aContent );

    /**
     * Pauses download of given item.
     * 
     * @param aContent Content data for downloaded item
     * @return System-wide error code
     */
    TInt PauseDownloadL( CVcxNsContent& aContent );

    /**
     * Resume download of given item.
     * 
     * @param aContent Content data for downloaded item
     * @return System-wide error code
     */
    TInt ResumeDownloadL( CVcxNsContent& aContent );

    /**
     * Cancels download of given item.
     * 
     * @param aContent Content data for downloaded item
     * @return System-wide error code
     */
    TInt CancelDownloadL( CVcxNsContent& aContent );
    
    /**
     * Get mpx data from collection to resolve ongoing or completed downloads.
     */   
    void LoadMpxMediaObjectsL();
    
    /**
     * Fetch single mpx object from collection.
     */ 
    void LoadMpxMediaObjectL( TUint32 aMpxId );
    
    /**
     * Store the mpx id to content database.
     * @param aContentId Target content id
     * @param aCaIndex Target content index
     * @param aMpxId Mpx media id
     */
    void StoreMpxIdToEcgDbL( TUint32 aContentId, TUint32 aCaIndex, TUint32 aMpxId );

    /**
     * Get the mpx media object by id.
     * @param aMpxIds
     */
    void GetMediaByMpxIdReqL(  RArray<TUint32>& aMpxIds  );

    /**
     * Append the given content to download list.
     * 
     * @param aContent Content
     */
    void AppendToDownloadList( CVcxNsContent* aContent );

    /**
     * Remove the given content from download list.
     * 
     * @param aContent Content
     */
    void RemoveFromDownloadList( CVcxNsContent* aContent );

    /**
     * Reset whole download list.
     *
     * @param aResetCmdsFromContentItems  If ETrue, resets commands
     *                                    from content items.
     */
    void ResetDownloadList( TBool aResetCmdsFromContentItems );

    /**
     * Find a comntent by url from download list.
     * @param aUri
     * @return Found content item or NULL
     */
    CVcxNsContent* SearchFromDownloadList( const TDesC& aUri );

    /**
     * Find a content by mpx id from download list.
     * @param aMpxId
     * @return Found content item or NULL
     */
    CVcxNsContent* SearchFromDownloadList( const TUint32 aMpxId );
    
    /**
     * MarkVideoAsWatchedL
     * @param aMpxId
     */
    void MarkVideoAsWatchedL( const TUint32 aMpxId );
    
    /**
     * SetlastPlayPosL
     * @param aMpxId Media id
     * @param aPos Play position
     */
    void SetlastPlayPosL( const TUint32 aMpxId, TInt32 aPos );
    
    /**
     *  Gets media object details from collection SYNC
     * 
     *  @param aMpxId MPX Media ID, which to fetch.
     */
    void GetMediaDetailsSyncL( TUint32 aMpxId );

// From Base classes

    /**
    *  From MMPXCollectionObserver->MMPXCollectionMediaObserver
    *  Handle extended media properties
    *
    *  @param aMedia media 
    *  @param aError error code    
    */
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

    /** 
    *  From MMPXCollectionUtilityObserver.
    *  Handle collection message
    *  NOTE: only one of HandleCollectionMessage callback can be implemented
    * 
    *  @param aMsg collection message, ownership not transferred. 
    *         Please check aMsg is not NULL before using it. If aErr is not 
    *         KErrNone, plugin might still call back with more info in the aMsg.
    *  @param aErr system error code.
    */
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr ); 

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
                      TInt aIndex, TBool aComplete, TInt aError );

    /**
    *  From MMPXCollectionUtilityObserver.
    *  Handles the item being opened. Typically called
    *  when client has Open()'d an item. Client typically responds by
    *  'playing' the item via the playlist
    *
    *  @param aPlaylist collection playlist
    *  @param aError error code   
    */                         
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError );

    /**
    *  From MMPXCollectionUtilityObserver.
    *  Handle completion of a asynchronous command
    *  All clients should implement this callback
    *  @param aCommandResult result of the command, NULL if error
    *  @param aError error code    
    */
    void HandleCommandComplete( CMPXCommand* aCommandResult, TInt aError );
    
    /**
     * From MVcxNsDownloadUpdaterObserver
     * 
     * Notify, that the object with given mpx id has 
     * changed, and needs to be refreshed.
     */
    void UpdateVideoObject( TUint32 aMpxId );
    
    /**
     * From MVcxNsDownloadUpdaterObserver
     * 
     * Notifies that resume has started from beginning.
     * @param aMpxId MPX Media Id of the video object.
     */
    void ResumeStartedFromBeginningL( TUint32 aMpxId );

    /**
     *  Searches given array and marks the item identified by 
     *  iMarkAsWatchedId as watched.
     * 
     *  @param aArray media object array to be processed.
     */
    void SearchAndMarkItemAsWatchedL( CMPXMediaArray* aArray );
    
    /**
     *  Marks the given media as watched. 
     * 
     *  @param aMedia media object to be marked as watched.
     */
    void MarkItemAsWatchedL( CMPXMedia* aMedia );

    /**
     * Starts to play the given mpx media 
     * 
     * @param aMpxId Mpx Id
     */
    void PlayVideoL( TUint32 aMpxId );

private:

    /**
     * Starts download of given item.
     * 
     * @param aAddress          Downloaded content's address
     * @param aName             Downloaded content's name
     * @param aIapId            Download iap id
     * @param aIsReadOnlyIap    Is the given iap readonly
     * @param aUserName         Downloaded content's username
     * @param aPassword         Downloaded content's password
     * @param aTransactionId    Download request transaction id
     * @return System-wide error code
     */
    TInt DownloadL( const TDesC& aAddress,
                    const TDesC& aName,
                    TUint32 aIapId,
                    TBool aIsReadOnlyIap,
                    const TDesC& aUserName,
                    const TDesC& aPassword,
                    TUint32& aTransactionId );
    
    /**
     * Links the mpx media objects to contents.
     * 
     * @param aArray Mpx media array containing the data.
     */
    void LinkMediaObjectsL( CMPXMediaArray* aArray  );

    /**
     * Sets the mpx media object to downloaded content
     * item and refreshes the content view, if visible.
     * 
     * @param aMedia Mpx media containing the data.
     */
    void SearchAndAddMpxMediaL( CMPXMedia* aMedia );
    
    /** 
     * Calls HandleSingleCollectionMessageL() for every message.
     * 
     *  @param aMessage collection message, ownership not transferred. 
     *         Please check aMsg is not NULL before using it. If aErr is not 
     *         KErrNone, plugin might still call back with more info in the aMsg.
     *  @param aErr system error code.
     */
    void ExplodeCollectionMessagesL( CMPXMessage* aMsg, TInt aErr ); 

    /**
     * Handles one sinlgle message
     * 
     *  @param aMessage collection message, ownership not transferred. 
     *         Please check aMsg is not NULL before using it. If aErr is not 
     *         KErrNone, plugin might still call back with more info in the aMsg.
     *  @param aErr system error code.
     */
    void HandleSingleCollectionMessageL( CMPXMessage* aMsg, TInt aErr  );
    
    /**
     * HandleSingleCollectionMessage's implementation.
     * 
     *  Handle collection message
     *  NOTE: only one of HandleCollectionMessage callback can be implemented
     * 
     *  @param aMessage collection message, ownership not transferred. 
     *         Please check aMsg is not NULL before using it. If aErr is not 
     *         KErrNone, plugin might still call back with more info in the aMsg.
     *  @param aErr system error code.
     */
    void DoHandleSingleCollectionMessageL( CMPXMessage* aMsg, TInt aErr );
    
    /**
     *  Handle mpx item changed -collection message
     * 
     *  @param aMessage collection message, ownership not transferred. 
     *         Please check aMsg is not NULL before using it. If aErr is not 
     *         KErrNone, plugin might still call back with more info in the aMsg.
     *  @param aErr system error code.
     */
    void HandleMpxItemChangedL( CMPXMessage* aMsg, TInt aErr );
    
    
    /**
     *  Copy ecg data to mpx database
     * 
     *  @param aContent Source content, where copy values from.
     *  @param aMpxId   Target media object id.
     */
    void CopyEcgDataToMpxColL( CVcxNsContent *aContent, TUint32 aMpxId );
    
    
    /**
     *  Notify to ui client that a download has  
     *  failed and error note needs to be shown.
     * 
     *  @param aMedia media object, which download has failed.
     *  @param aContent Details of the video content which download failed.
     */
    void NotifyDownloadErrorL( CMPXMedia* aMedia, CVcxNsContent& aContent );

    /**
     *  Get VC error code out of S60 dmgr error codes.
     * 
     *  @param aMedia media object, which error code to get.
     *  @return VideoCenter specific error code.
     */
    TIptvDlError GetDownloadErrorL( CMPXMedia* aMedia );

    /**
     *  Writes given media attribute to mpx collection
     * 
     *  @param aAttribute  Attribute
     *  @param aValue      Value
     *  @param aMpxId      Mpx item id to write
     */
    void WriteAttrToColL( TMPXAttributeData aAttribute, 
                          TInt32 aValue, 
                          TUint32 aMpxId );
    
    /**
     *  Writes given media attribute to mpx collection
     * 
     *  @param aAttribute  Attribute
     *  @param aValue      Value
     *  @param aMpxId      Mpx item id to write
     */
    void WriteAttrToColL( TMPXAttributeData aAttribute, 
                          TReal32 aValue, 
                          TUint32 aMpxId );
    
    /**
     *  Writes changed media object attribute data to collection
     * 
     *  @param aMsg Media object containing changes, which to commit.
     */
    void WriteMsgToColL( CMPXMedia* aMsg );
    
    /**
     *  Gets media object details from collection
     * 
     *  @param aMpxId MPX Media ID, which to fetch.
     */
    void GetMediaDetailsL( TUint32 aMpxId );

    /**
     * Constructor.
     * 
     * @param aUiEngine Reference to CVcxNsUiEngine.
     */    
    CVcxNsMpxCollectionClientHandler( CVcxNsUiEngine& aUiEngine );

    void ConstructL();
    
    /**
    * Deletes download from Ui Engine. Called when item deleted event arrives from collection,
    * or when download cancel command completes (HandleCommandComplete).
    *
    * @param itemId Item to delete.
    */
    void DeleteDownload( TMPXItemId itemId );

private:

    /**
     * Reference to CVcxNsUiEngine.
     * Not own.
     */
    CVcxNsUiEngine& iUiEngine;
    
    /**
    * MMPXCollectionUtility* 
    */
    MMPXCollectionUtility* iCollectionUtility;
    
    /**
    * Id of most recent command sent to mpx collection.
    * Incremented after each cmd to keep 'em unique.
    */    
    TUint32 iLatestTransactionId;
    
    /**
    * The currently / previously downloaded contents.
    */  
    RPointerArray<CVcxNsContent> iDownloadedContents;
    
    /**
    * CVcxNsDownloadUpdater.
    * Own.
    */  
    CVcxNsDownloadUpdater* iDownloadUpdater;
 
    /**
    * Write pending play pos.
    */  
    TInt32 iWritePendingPlayPos;

    /**
     * Downloading has been resumed for these MPX IDs. Needed for showing
     * note "Resuming download not possible, download starts from beginning".
     */
    RArray<TUint32> iResumeArray;
    
    /**
     * Last fetched mpx media objects
     */
    RPointerArray<CMPXMedia> iLastFetchedMedias;
    
    /**
     * Wait for the async media fetch
     */
    CActiveSchedulerWait* iGetMediaDetailWait;
    
    /**
     * Id for the media that needs to be marked as watched.
     */
    TInt64 iMarkAsWatchedId;

    };

#endif // VCXNSDOWNLOADCLIENTHANDLER_H_
