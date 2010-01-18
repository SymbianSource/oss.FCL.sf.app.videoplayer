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




#ifndef C_VCXNSCONTENTHARVESTERMPXCLIENT_H
#define C_VCXNSCONTENTHARVESTERMPXCLIENT_H

#include <e32base.h>
#include <mpxcollectionobserver.h>
#include <MediatorEventConsumer.h>

class MMPXCollectionUtility;
class CVcxNsChPublishableData;
class CVcxNsChPublisher;
class CVcxNsChMCategoryListener;
class CVcxNsContentHarvesterPlugin;

/**
 *  CVcxNsContentHarvesterMpxClient
 * 
 *  @lib vcxnscontentharvesterplugin.dll
 *  @since S60 5.0
 */
class CVcxNsContentHarvesterMpxClient : public CBase,
                                        public MMPXCollectionObserver,
                                        public MMediatorEventObserver
    {

public:


    static CVcxNsContentHarvesterMpxClient* NewL( CVcxNsChPublisher* aPublisher,
                                                  CVcxNsChPublishableData* aPublishableData,
                                                  CVcxNsContentHarvesterPlugin* aPlugin );

    virtual ~CVcxNsContentHarvesterMpxClient();
    
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
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,
                      TInt aError );

    /**
     * From MMPXCollectionUtilityObserver.
     * Handle completion of a asynchronous command
     *
     * @param aCommandResult result of the command, NULL if error
     * @param aError error code    
     */
    void HandleCommandComplete( CMPXCommand* aCommandResult, 
                                TInt aError );
    
    /**
     * From MMediatorEventObserver
     * 
     * Catches the events
     */
    void MediatorEventL( TUid aDomain,
                         TUid aCategory, 
                         TInt aEventId, 
                         const TDesC8& aData );

private:

    enum TVcxNsCollectionOpenStatus
        {
        EVcxNsCollectionNotOpen = 0,
        EVcxNsCollectionOpening,
        EVcxNsCollectionOpened            
        };

    CVcxNsContentHarvesterMpxClient( CVcxNsChPublisher* aPublisher,
                                     CVcxNsChPublishableData* aPublishableData,
                                     CVcxNsContentHarvesterPlugin* aPlugin );

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
     * Handles item addition message, called by HandleCollectionMessage().
     * 
     * @param aMessage collection message, ownership not transferred. 
     *        Please check aMsg is not NULL before using it. If aErr is not 
     *        KErrNone, plugin might still call back with more info in the aMsg.
     * @param aErr system error code. 
     */
    void HandleMyVideosItemsAppendedL( CMPXMessage* aMessage, TInt aError );        
    
    /**
     * Handles item changed message, called by HandleCollectionMessage().
     * 
     * @param aMessage collection message, ownership not transferred. 
     *        Please check aMsg is not NULL before using it. If aErr is not 
     *        KErrNone, plugin might still call back with more info in the aMsg.
     * @param aErr system error code. 
     */
    void HandleMyVideosItemsChangedL( CMPXMessage* aMessage, TInt aError ); 
    
    /**
     * Handles item deleted or modified message, called by HandleMyVideosItemsChangedL().
     * 
     * @param aMessage collection message, ownership not transferred. 
     *        Please check aMsg is not NULL before using it. If aErr is not 
     *        KErrNone, plugin might still call back with more info in the aMsg.
     */
    void HandleContentModifiedL( CMPXMessage* aMessage ); 
    
    /**
     * OpenCollectionL
     */
    void OpenCollectionL();
    
    /**
     * CloseCollection
     */
    void CloseCollection();
    
    /**
     * Returns the current level of collection.
     */
    TInt CollectionLevelL();
    
    /**
     * UpdateCategoryL
     */
    void UpdateCategoryL();
    
    /**
     * SearchAndStoreCategoryL
     */
    void SearchAndStoreCategoryL( CMPXMediaArray* categoryArray );
    
    /**
     * UpdateNewVideoCount
     */
    void UpdateNewVideoCountL();

private: // data

    /**
     * Instance of MPX collection util interface.
     */
    MMPXCollectionUtility* iCollectionUtility;
    
    /**
     * Observed category
     */
    CMPXMedia* iObservedCategory;
    
    /**
     * Variable for storing collection opening status.
     */
    TVcxNsCollectionOpenStatus iCollectionOpenStatus;
    
    /**
     * CVcxNsChPublisher. Not owned.
     */
    CVcxNsChPublisher* iPublisher;
    
    /**
     * CVcxNsChPublishableData. Not owned.
     */
    CVcxNsChPublishableData* iPublishableData;
    
    /**
     * Mediator helper object for registering and subscribing 
     * mpx collection related mediator events
     */
    CVcxNsChMCategoryListener* iMCategoryListener;
    
    /**
     * Mediator helper object for registering and subscribing 
     * action handler related mediator events
     */
    CVcxNsChMCategoryListener* iAhCategoryListener;
    
    /**
     * iOngoingDownloads.
     */
    TBool iOngoingDownloads;
    
    /**
     * iTvVideoSuiteLoaded.
     */
    TBool iTvVideoSuiteLoaded;
    
    /**
     * Parent Plugin. Not own.
     */
    CVcxNsContentHarvesterPlugin* iPlugin;

    };

#endif // C_VCXNSCONTENTHARVESTERMPXCLIENT_H
