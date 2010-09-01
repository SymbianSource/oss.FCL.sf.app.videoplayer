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
#include <mpxcollectionobserver.h>

#include "CIptvUtil.h"

// FORWARD DECLARATIONS
class CIptvService;
class CIptvMediaContent;
class MMPXCollectionUtility;
class CIptvVodContentClient;
class CIptvVodDlDownloadEvent;

/**
* MVcxNsMpxClientObserver
* 
* class specifies the interface for CVcxNsScheduledDownloadMpxClient callback function.
* Client must inherit this and implement the virtual functions.
*/
class MVcxNsMpxClientObserver
    {
        
public: // New functions

    /**
    * This emulates old fashion dl client events
    */
    virtual void HandleVodDlDownloadEvent( CIptvVodDlDownloadEvent& aEvent ) = 0;
    
    };

/**
* Class CVcxNsScheduledDownloadMpxClient handles mpx collection client interactions.
*
* @lib iptvscheduleddownloadplugin.lib
*/
class CVcxNsScheduledDownloadMpxClient : public CBase,
                                         public MMPXCollectionObserver
    {

public:

    /**
     * Two-phased constructor. 
     */
    static CVcxNsScheduledDownloadMpxClient* NewL( MVcxNsMpxClientObserver& aObserver,
                                                   CIptvVodContentClient& aContentClient,
                                                   TUint32 aServiceId );

    /**
     * Destructor.
     */    
    virtual ~CVcxNsScheduledDownloadMpxClient();
    
    /**
     * Download content from service.
     * @param aIapId
     * @param aService   Service.
     * @param aContentId Content ID identifier.
     * @param aName      Content name.
     * @param aIndex     Content access index.
     * @param aDltype    Type of the requested download, currently only EIptvDownloadTypeImmediate
     *                   supported.
     * @return           Error code, indicates if download is started successfully. If return code is
     *                   EIptvDlNoError, then further download status is reported via events, on error
     *                   no further events will arrive.
     */
    TIptvDlError DownloadL(
        TUint32 aIapId,
        CIptvService* aService,
        TIptvContentId aContentId,
        const TDesC& aName );
    
    /**
     * Cancels download.
     * Operation includes clean up that removes files from file system,
     * deletes data related to FileId and releases FileId identifier.
     * @param aServiceId
     * @param aContentId
     * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
     *         KErrNotFound if download was not found from VOD DL Manager, file might still be in
     *         My Videos.
     */
    TInt CancelDownloadL(
        TUint32 aServiceId,
        TUint32 aContentId );    

    /**
     * Get the mpx media object by id.
     * @param aMpxIds
     */
    void GetMediaByMpxIdReqL(  RArray<TUint32>& aMpxIds  );
    
    /**
     *  Check media's download state and do routines if necessary.
     * 
     *  @param aMediaId media object id which to check.
     */
    void CheckMediaStateL( TUint32 aMediaId );

    /**
     * Append the given content to download list.
     * 
     * List takes ownership of given object.
     * 
     * @param aContent Content
     */
    void AppendToDownloadList( CIptvMediaContent* aContent );

    /**
     * Remove the given content from download list.
     * 
     * Pointed object is deleted.
     * 
     * @param aContent Content
     */
    void RemoveFromDownloadList( CIptvMediaContent* aContent );

    /**
     * Find a content by url from download list.
     */
    CIptvMediaContent* SearchFromDownloadList( const TDesC& aUri );

    /**
     * Find a content by mpx id from download list.
     */
    CIptvMediaContent* SearchFromDownloadList( const TUint32 aMpxId );
    
    /**
     * Find a content by service and content id from download list.
     */
    CIptvMediaContent* SearchFromDownloadList( const TUint32 aServiceId, 
                                               const TUint32 aContentId );

// From Base classes

    /**
    *  From MMPXCollectionObserver->MMPXCollectionMediaObserver
    *  Handle extended media properties
    *
    *  @param aMedia media 
    *  @param aError error code    
    */
    void HandleCollectionMediaL( const CMPXMedia& /*aMedia*/, 
                                 TInt /*aError*/ );

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
    void HandleCollectionMessage( CMPXMessage* /*aMsg*/, TInt /*aErr*/ ); 

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
    void HandleOpenL( const CMPXMedia& /*aEntries*/,
                      TInt /*aIndex*/, TBool /*aComplete*/, TInt /*aError*/ );

    /**
    *  From MMPXCollectionUtilityObserver.
    *  Handles the item being opened. Typically called
    *  when client has Open()'d an item. Client typically responds by
    *  'playing' the item via the playlist
    *
    *  @param aPlaylist collection playlist
    *  @param aError error code   
    */                         
    void HandleOpenL( const CMPXCollectionPlaylist& /*aPlaylist*/, TInt /*aError*/ );

    /**
    *  From MMPXCollectionUtilityObserver.
    *  Handle completion of a asynchronous command
    *  All clients should implement this callback
    *  @param aCommandResult result of the command, NULL if error
    *  @param aError error code    
    */
    void HandleCommandComplete( CMPXCommand* /*aCommandResult*/, TInt /*aError*/ );
    
    /**
     *  Marks given items as watched to MPX col.
     * 
     *  @param aArray media object array to be processed.
     */
    void MarkItemsAsWatchedL( CMPXMediaArray* aArray );
    
    /**
     *  Sets given items last play pos to MPX col.
     * 
     *  @param aArray media object array to be processed.
     */
    void WriteLastPlayPosL( CMPXMediaArray* aArray );

private:

    /**
     * Starts download of given item.
     * 
     * @param aAddress          Address of the downloaded content
     * @param aName             Name of the downloaded content
     * @param aIapId            Iap id for the download
     * @param aIsReadOnlyIap    Is the give iap read-only
     * @param aUserName         UserName of the downloaded content
     * @param aPassword         Password of the downloaded content
     * @return                  System-wide error code
     */
    TInt DoDownloadL( const TDesC& aAddress,
                      const TDesC& aName,
                      TUint32 aIapId,
                      TBool aIsReadOnlyIap,
                      const TDesC& aUserName,
                      const TDesC& aPassword );
    
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
     *  @param aMedia   Target media object, where to copy the values.
     */
    void CopyEcgDataToMpxColL( CIptvMediaContent *aContent, CMPXMedia *aMedia );
    
    /**
     *  Get CMPXMedia.
     * 
     *  @param aMediaId media object id, which to get.
     *  @return CMPXMedia object. Ownership not transferred.
     */
    CMPXMedia* GetMpxMediaFromList( TUint32 aMediaId );

    CVcxNsScheduledDownloadMpxClient(  MVcxNsMpxClientObserver& aObserver,
                                       CIptvVodContentClient& aContentClient,
                                       TUint32 aServiceId );

    void ConstructL();

private:
    
    /**
    * MMPXCollectionUtility* 
    */
    MMPXCollectionUtility* iCollectionUtility;
    
    /**
    * The currently / previously downloaded contents.
    */  
    RPointerArray<CIptvMediaContent> iDownloadedContents;

    /**
    * The downloaded content's mpx items.
    */  
    RPointerArray<CMPXMedia> iDownloadedMedias;
    
    /**
    * MVcxNsMpxClientObserver
    */
    MVcxNsMpxClientObserver& iObserver; 
    
    /**
    * CIptvVodContentClient
    */
    CIptvVodContentClient& iContentClient;
    
    /**
    * Current service id
    */
    TUint32 iServiceId;

    };

#endif // VCXNSDOWNLOADCLIENTHANDLER_H_
