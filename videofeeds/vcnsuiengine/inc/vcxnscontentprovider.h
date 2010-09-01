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




#ifndef C_VCXNSCONTENTPROVIDER_H
#define C_VCXNSCONTENTPROVIDER_H

// INCLUDES
#include <e32base.h>
#include <mpxitemid.h>
#include "CIptvUtil.h" 

#include "vcxnscontentproviderif.h"
#include "vcxnscontentproviderobserver.h"
#include "vcxnscontent.h"

// FORWARD DECLARATIONS
class MVcxNsContentProviderObserver;
class CVcxNsUiEngine;
class CVcxNsContentList;
class CIptvMediaContent;
class CIptvVodContentContentBriefDetails;

// CLASS DECLARATION

/**
* Class CVcxNsContentProvider handles application contents.
*
* @lib vcxnsuiengine.lib
*/
class CVcxNsContentProvider : public CBase,
                              public MVcxNsContentProviderIf
    { 

public:

    /**
     * Constructor.
     * @param aUiEngine Ui engine
     */
    static CVcxNsContentProvider* NewL( CVcxNsUiEngine& aUiEngine );

    virtual ~CVcxNsContentProvider();

    /**
     * RegisterObserver.
     * @param aObserver Observer
     */
    void RegisterObserver( MVcxNsContentProviderObserver* aObserver );

    /**
     * DeRegisterObserver.
     * @param aObserver Observer
     */
    void DeRegisterObserver( MVcxNsContentProviderObserver* aObserver );

    /**
     * Get contents with highlighted position
     * @param aHighlight On return, highlight index for content list
     * @return Array of contents.
     */
    RPointerArray<CVcxNsContent>& GetContentsL( TInt& aHighlight );
    
    /**
     * Gets contents.
     *
     * @return Vod contents
     */
    RPointerArray<CVcxNsContent>& GetContentsL();
    
    /**
     * Reset the contents with given service id.
     *
     * @param aServiceId Service id
     */
    void RemoveServiceData( TUint32 aServiceId );

    /**
     * Reset the contents of specified category in service.
     * Note: This is usually done only for search category when
     * search results are cleared before new search.
     *
     * @param aServiceId Service id
     * @param aCategoryId Category id
     */
    void RemoveCategoryData( TUint32 aServiceId, TUint32 aCategoryId );
    
    /**
     * Handle the back command by choosing new application state.
     * @return Was the back event consumed. If EFalse, 
     *         the back event should be handled by caller.     
     */
    TBool HandleBack();
    
    /**
     * Starts the content update.
     *
     * @return KErrNone or one of the system wide error codes.
     */
    TInt RefreshContentsL();

    /**
     * Starts download of given content item.
     * @param aIndex Index of the content to be downloaded
     * @return KErrNone or one of the system wide error codes.
     */
    TInt StartDownloadL( TInt aIndex );
      
    /**
     * Pause download of given content item.
     * 
     * @param aIndex Index of the content item.
     * @return KErrNone or one of the system wide error codes.
     */
    TInt PauseDownloadL( TInt aIndex );
    
    /**
     * Resume download of given content item.
     * 
     * @param aIndex Index of the content item.
     * @return KErrNone or one of the system wide error codes.
     */
    TInt ResumeDownloadL( TInt aIndex );
    
    /**
     * Resume download of given content item.
     * 
     * @param aContent Content details
     * @return KErrNone or one of the system wide error codes.
     */
    TInt ResumeDownloadL( CVcxNsContent& aContent );
    
    /**
     * Cancels downloading of given content item.
     * 
     * @param aIndex Index of the content item.
     * @return KErrNone or one of the system wide error codes.
     */
    TInt CancelDownloadL( TInt aIndex );
    
    /**
     * Search content by uri.
     * @param aUri uri of the content to be found
     * @return Content which has the given uri or NULL if not found.
     */
    CVcxNsContent* SearchContentByUri( const TDesC& aUri );
    
    /**
     * Search content by TMPXItemId.
     * @param aItemId MPX item id
     * @return Content which has the given TMPXItemId or NULL if not found.
     */
    CVcxNsContent* SearchContentByMpxId( TMPXItemId aItemId );
    
    /**
     * Search content by id.
     * @param aServiceId  Service id  
     * @param aContentId  Content id
     * @return Content which has the given service and content id.
     */
    CVcxNsContent* SearchContentById( TUint32 aServiceId, TUint32 aContentId );
    
    /**
     * Search content's index in ui model.
     * @param aContent Content item
     * @return Index of the content or KErrNotFound, if not visible.
     */
    TInt ContentIndex( CVcxNsContent* aContent );
    
    /**
     * Tell UI layer that the given single content item needs to be updated.
     * Event is forwarded to MVcxNsContentProviderObserver
     *
     * @param aContentdata Updated content data     
     */
    void UpdateContent( CVcxNsContent* aContentdata );
    
    /**
     * Fetches the icons path from epg database and 
     * updates them to current content array.
     */
    void UpdateIconsPathL();
    
    /**
     * Get the content with given id by any means.
     * @param aServiceId  Service id  
     * @param aContentId  Content id
     * @return Content which has the given service and content id.
     */
    CVcxNsContent* GetContentL( TUint32 aServiceId, TUint32 aContentId );

// From MVcxNsContentProviderObserver

    /**
     * Content update notification.
     *
     * @param aIndex Updated item's index
     */
    void ContentUpdated( TInt aIndex );
    
    /**
     * View needs to be refreshed.
     */
    void RefreshView();

    /**
     * Application state needs to be updated.
     */
    void HandleAppStateChangedL();

    /**
     * Epg update is ongoing, show the notification.
     * When downloading and parsing the xml, both parameters are 0.
     * 
     * @param aShow If ETrue, show the note, if EFalse, hide it.
     * @param aDownloadedTbns count of downloaded thumbnails.
     * @param aTotalTbns Total count of thumbnails to be downloaded.
     */
    void ShowUpdatingNoteL( TBool aShow, TInt aDownloadedTbns, TInt aTotalTbns );
    
    /**
     * Store the last play position to epg db.
     * This is intended to be used just with streams,
     * video attributes are managed by MPX collection.
     * @param aContent Content item
     * @param aType Media content access type 
     * @param aPos Last play position to be set
     */
    void StoreLastPlayPosL( CVcxNsContent* aContent, 
                            CVcxNsContent::TVcxNsContentAccessType aType,
                            TReal32 aPos );
    
    
    /**
     * Handle error, send error information to UI.
     *
     * @param aError Error code.
     * @param aUpdate ETrue if error comes from epg update. 
     *                EFalse if error is from download.
     * @param aServiceId Service ID
     */
    void HandleErrorL( TInt aError, TBool aUpdate, TUint32 aServiceId );
    
    /**
     * Handle dl error. 
     *
     * @param aError Error code.
     * @param aContent Details of the failed video content.
     */
    void HandleDlErrorL( TIptvDlError aError, CVcxNsContent& aContent );
    
    /**
     * Checks if video has more details than just name
     * 
     * @param aSelected Index of selected item
     * @return Does the video have other details than just name
     */
     TBool HasVideoDetailsL( TInt aSelected );
        
     /**
      * Fetches full video details
      * 
      * @param aSelected Index of selected item
      * @return Content full details
      */
     CVcxNsContent* GetFullDetailsL( TInt aSelected );
     
     /**
      * Save content view highlight for currently active category. 
      * @param aHighlight Highlight index to save.
      */
     void SetContentHighlight( TInt aHighlight );
     
     /**
      * Fetches full details for the given content item.
      * 
      * @param aContent Content, which details to fetch
      */
     void FetchFullDetailsL( CVcxNsContent* aContent );

private:

    /**
     * Constructor.
     * @param aUiEngine Ui engine
     */
    CVcxNsContentProvider( CVcxNsUiEngine& aUiEngine );

    /**
     * Get content list from cache.
     * @return Content list
     */
    CVcxNsContentList* GetListFromCache();
    
    /**
     * Get the Content data from DB
     * @return Content list
     */
    CVcxNsContentList* LoadEpgDataL();
    
    /**
     * Adds contents which has mds ids to mpx client's downlolad list.
     * @param aContentList Content list, which items to add to dl list
     */
    void BuildDownloadListL( RPointerArray<CVcxNsContent>& aContentList );
    
    /**
     * Adds valid content accesses from the list to the content.
     * @param aContent Content item
     * @param aContentAccessList Content access list
     */
    void AddContentAccessesL( CVcxNsContent& aContent, 
                    RPointerArray<CIptvMediaContent>& aContentAccessList );
    
    /**
     * Creates ns content item from server side videocenter content object.
     * @param aData Source content data class
     * @param aServiceId Content's service ID 
     * @return Newly constructed content item
     */
    CVcxNsContent* ConstructContentItemL( CIptvVodContentContentBriefDetails* aData,
                                          TUint32 aServiceId );
    
// Data
    
    /**
     * Array of content event observers. Not Own.
     */
    RPointerArray<MVcxNsContentProviderObserver> iContentObservers;
    
    /**
     * UI engine.
     */
    CVcxNsUiEngine& iUiEngine;

    /**
     * Array of content lists.
     */
    RPointerArray<CVcxNsContentList> iContentListList;
    
    /**
     * Pointer to previously fetched content list.
     * 
     * Owned elsewhere.
     */
    CVcxNsContentList* iPreviousList;

    };

#endif // C_VCXNSCONTENTPROVIDER_H
