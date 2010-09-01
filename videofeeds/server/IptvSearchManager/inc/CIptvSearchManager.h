/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef CIPTVSEARCHMANAGER_H
#define CIPTVSEARCHMANAGER_H

// System includes
#include <e32base.h>
#include "MIptvEpgVodCallback.h"
#include "MIptvEpgManagerUpdateObserver.h"
#include "CIptvUtil.h"
#include <cenrepnotifyhandler.h>
#include "CIptvEpgManagerUpdater.h"

// Forward declarations
class TIptvRssSearchQuery;
class CIptvVodContentContentFullDetails;
class CIptvVodContentContentBriefDetails;
class CIptvVodContentCategoryBriefDetails;
class CIptvService;
class CIptvServer;
class CIptvEpgPluginInterface;
class CIptvEpgVodMsqQueue;
class CIptvContentIdMediaContentItem;
class CIptvEpgSession;
class CIptvEpgService;
class CRepository;

/**
*	CIptvSearchManager is the class to receive search requests from the client,
*	passes them to the RSS plugin and receives content information from the plugin
*	when it reports about downloaded and parsed categories and contents. This
*	class stores the found search results in RAM; results are not stored to any
*	database.
*	When new search is performed old search results will be deleted.
*/
class CIptvSearchManager : public CBase,
                           public MIptvEpgVodCallback,
                           public MIptvEpgManagerUpdateObserver,
		                   public MCenRepNotifyHandlerCallback
	{
	public:

    	/**
    	*	Symbian two phase constructor
    	*	@param	None
    	*	@return	Pointer to a newly created CIptvSearchManager object
    	*/
    	static CIptvSearchManager* NewL( CIptvServer& aServer,
    	                                 CIptvEpgSession& aParent,
    									 CIptvEpgVodMsqQueue* aVodMsgQueue );
    	
    	/**
    	*	Destructor
    	*/
    	~CIptvSearchManager();
	
	public: // New methods
	
    	/**
    	*	Method to perform search
    	*/
    	TInt Search( TIptvRssSearchQuery& aQuery );
    	
    	/**
    	*	Method to cancel current search. This causes the cleanup for
    	*	search results and RSS plugin to be stopped
    	*	@param	None
    	*	@return	None
    	*/
    	TInt CancelSearch();
    	
    	/**
    	*	Getter for brief details of the search category.
    	*	@param	None
    	*	@return	Pointer to the brief details object that descvribes the search
    	*			category. If search is not possible we return NULL here. Ownership
    	*			of the returned object is transferred.
    	*/
    	CIptvVodContentCategoryBriefDetails* SearchCategoryContentBriefDetailsL();
    	
    	/**
    	*	Getter method for the brief details of the specified content.
    	*	@param	aContentId		Id of the content that's details are requested
    	*	@param	aSearchString	Not used in search operations
    	*	@param	aFrom			Not used in search operations
    	*	@param	aAmount			Not used in search operations
    	*	@param	aTotalAmount	Number of details found for the given content id
    	*	@param	aEcgList		On return contains the list of the details object
    	*							that are matching the given content id
    	*/    	
    	void GetSearchContentBriefDetailsL( TIptvCategoryId aContentId,
    										TDesC& aSearchString,
    										TUint32 aFrom,
    										TUint32 aAmount,
    										TUint32& aTotalAmount,
    										RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList );
    	
    	/**
    	*	Getter for the parent category for the given category. In this method
    	*	aCategoryId is compared with KIptvRssSearchCategoryId
    	*	@param	aCategoryId			Used to identify if parent category for the search
    	*								is asked (compared with KIptvRssSearchCategoryId)
    	*	@param	aParentCategoryId	On return contains KIptvVodContentCategoryRootId if
    	*								aCategoryId == KIptvRssSearchCategoryId)
    	*	@return	KErrNone if aCategoryId == KIptvRssSearchCategoryId, otherwise KErrNotSupported
    	*/    	
    	TInt GetParentCategory( TIptvCategoryId aCategoryId, TIptvCategoryId& aParentCategoryId );
    	
    	/**
    	*	Setter for service id. This method must be called before starting the
    	*	update.
    	*	@param	aServiceId	Value for current service's id
    	*	@return	None
    	*/
    	TInt SetServiceId( TUint32 aServiceId );
    	
    	/**
    	*	Sets the iap to be used for search operation. This variable is
    	*	passed to RSS plugin so it knows what iap user has selected.
    	*	@param	aIap	Iap to be used for search operations
    	*	@return	None
    	*/    	
    	void SetIap( TUint32 aIap );
    	
    	/**
    	*	Returns the boolean value if current service supports search
    	*	functionality
    	*	@param	None
    	*	@return	ETrue if search is possible, otherwise EFalse
    	*/    	
    	TBool IsSearchPossible() const;
    	
    	/**
    	*	Setter for the service information to be used for the search operations.
    	*	Given service defines if the search operations are allowed for the service
    	*	and as well as the url for the search operations. Contents of the aService
    	*	are stored into the iCurrentService member variable.
    	*	@param	aService	Pointer to the service object that describes currently
    	*						active service.
    	*/    	
    	void SetServiceL( CIptvEpgService* aService );
    	
    	/**
    	*	Method to retrieve a list of media content access definitions for
    	*	the specified content id. There might be different locations for same content
    	*	where bitrate, mime type and so on can vary. Higher level implementations
    	*	can select the proper content from the list to be downloaded.
    	*	@param	aContentId		Content id which media contents are to be retrieved
    	*	@param	aContents		On return contains the list of different sets of
    	*							media contents available for specified content.
    	*/    	
    	void GetContentAccessListL( TUint32 aContentId, RPointerArray<CIptvMediaContent>& aContents );
    	
    	/**
    	*	Method to retrieve the proper media content based on the content id
    	*	and an index inside the array of media contents of aContentId.
    	*	@param	aContentId	ContentId which content access is to be
    	*						retrieved
    	*	@param	aIndex		Index of the media content client is willing to retrieve.
    	*	@return	On return contains the item from iMediaContents array that has
    	*			matching content id.
    	*/    	
    	CIptvMediaContent* GetContentAccessL( TUint32 aContentId, TUint32 aIndex );
    	
    	/**
    	*	Getter for more specified data for the specified content.
    	*	@param	aContentId			Content id which details is to be retrieved
    	*	@param	aContentFullDetails	On return contains the full details of the
    	*								specified content.
    	*	@return	KErrNone if successful, otherwise Symbian error code.
    	*/    	
    	TInt GetContentDetailsL( TIptvContentId aContentId,
    							 CIptvVodContentContentFullDetails& aContentFullDetails );
    	
	    /**
	    *	Method to add a message queue for the local array.
	    *	@param	aVodMsgQueue	Message queue pointer to be inserted to the
	    *							local array.
	    *	@return	None
	    */
        void AddEpgVodMsgQueue( CIptvEpgVodMsqQueue* aVodMsgQueue );
	
	    /**
	    *	Method to remove specified message queue object from the local array.
	    *	@param	aVodMsgQueue	Message queue pointer to be removed from
	    *							the array
	    *	@return	None
	    */
        void RemoveEpgVodMsgQueue( CIptvEpgVodMsqQueue* aVodMsgQueue );	

        /**
        *	Clears all the results and returns the search manager to initiate
        *	state. This method call takes care of freeing all the memory in
        *	the arrays inside this class as well as deleting thumbnails
        *	downloaded for search result located in iThumbnailDirectory.
        *	@param	None
        *	@return	None
        */
        void ClearAllResults();

        /**
        * Updates Search Manager's list of Content ID numbers that have been
        * used by downloads from previous search results. This method should 
        * be called before starting a new search. Call is external, because
        * Search Manager does not have all the necessary dependencies to generate
        * the list.
        *
        * @param aUsedIdArray New array of used Content ID numbers.
        */
        void UpdateUsedContentIdsListL( RArray<TInt>& aUsedIdArray );

	public: // Methods to satisfy MIptvEpgVodCallback inheritance
	
        /**
        *	Method called by the plugin to tell that plugin has started doing
        *	it's job.
        *	@param	None
        *	@return	None
        */
        void StartedL();

        /**
        *	ECG content update from XML completed. Thumbnails are not downloaded yet.
        *	@param	None
        *	@return	None
        */
        void ContentUpdateCompletedL();

        /**
        *	Method to used to signalize that plugin has finished it's job
        *	@param aUpdateInterval	Update interval defines how long EPG data
        *							is valid.
        *	@return	KErrNone if successful, otherwise some of the Symbian wide
        *			error codes
        */
        TInt FinishedL( TTime& aUpdateInterval );

        /**
        *	Adds new category to be added to the result array
        *
        *	@param aCategory	Reference to CIptvEpgCategory object.
        *	@param aCategoryKey	New category key value is inserted to
        *						aCategoryKey
        *	@return				KErrNone if successful, otherwise another
        *						of the system-wide error codes.
        */
        TInt AddCategoryL( CIptvEpgCategory& aCategory, TUint32& aCategoryKey );

        /**
        *	Adds new content to added to the result array
        *	@param aContent			Reference to CIptvEpgContent object.         
        *	@param aMediaContents	Pointer array containing media content objects.
        *							There can be n media content's for one content.
        *	@param aContentKey		New content key value is inserted to aContentKey
        *	@return KErrNone if successful, otherwise another of the system-wide
        			error codes.
        */
        TInt AddContentL( CIptvEpgContent& aContent,
        				  RPointerArray<CIptvMediaContent> aMediaContents,
        				  TUint32& aContentKey );

        /**
        *	Updates the thumbnail path for the service.
        *	@param aIconFilePath	Full path to service thumbnail in local filesystem.
        *   @param aGrouped         if true updates also grouped item, otherwise not.
        * 	@return KErrNone if successful, otherwise another of the system-wide
        *			error codes.
        */
        TInt UpdateServiceThumbnailL( TDesC& aIconFilePath, const TBool aGrouped );

        /**
        *	Add content to specified category. Value is updated to the result array
        *	@param	aCategoryKey      Category key.        
        * 	@param	aContentKey       Content key.
        *	@return	KErrNone on success, otherwise one of the system-wide error codes
        */    
        TInt AddRelationL( TUint32& aCategoryKey, TUint32& aContentKey );

        /**
        *	Error that has occured during the parsing
        *	@param	aError	Error code describing the occured error
        *	@return	None
        */    
        void ErrorL(TInt aError, TInt aInfo);      

        /**
        *	Method to update icon for a category
        *	@param	aCategoryKey	Category key.
        *	@param	aIconFileId		Icon file id to search from the database.         
        *	@param	aIconFilePath	Full path to icon file or URL + filename.
        *	@param	aIsUrl			If aIconFilePath is URL aIsUrl == ETrue,
        *							if aIconFilePath is path to local filesystem
        *							aIsUrl == EFalse.
        *	@return KErrNone if successful, otherwise another of the system-wide error codes.
        */
        TInt UpdateCategoryIconL( TUint32& aCategoryKey,
        						  TUint32& aIconFileId,
        						  TDesC& aIconFilePath,
        						  TBool aIsUrl);

        /**
         *	Method to update icon for content
         *	@param	aCategoryKey	Content key.
         *	@param	aIconFileId		Icon file id to search from the database.         
         *	@param	aIconFilePath	Full path to icon file or URL + filename.
         *	@param	aIsUrl			If aIconFilePath is URL aIsUrl == ETrue,
         *							if aIconFilePath is path to local filesystem
         *							aIsUrl == EFalse.
         *	@return KErrNone if successful, otherwise another of the system-wide error codes.
         */
        TInt UpdateContentIconL( TUint32& aContentKey,
        						 TUint32& aIconFileId,
        						 TDesC& aIconFilePath,
        						 TBool aIsUrl);

		/**
		*	Resets the global file id values for both media access array and
		*	parent details as well
		*	@param	aId		Id defining the global file id's to be reseted
		*	@return	KErrNone if successful, otherwise a Symbian error code
		*/
		TInt ResetGlobalId( CIptvMyVideosGlobalFileId& aId );
    
		/**
		*	Sets the global file id values for media content specified by content id
		*	@param	aContentKey	Key to describe content id of the media content
		*	@param	aId		Id defining the global file id's to be reseted
		*	@param	aIndex	Index of media content array which details is updated
		*	@return	KErrNone if successful, otherwise a Symbian error code
		*/
		TInt SetGlobalId( TUint32 aContentKey,
					  	  CIptvMyVideosGlobalFileId& aId,
					  	  TUint32 aIndex );
		
		public: // From MIptvEpgManagerUpdateObserver
      
        /**        
         * Check if plugin is running, and closes everything down if plugin has
         * completed.
         */
        void CheckIsPluginRunning();
    
        /**
         * Returns ETrue if plugin is running.
         */
        TBool IsPluginRunning() { return iPluginIsRunning; }

	public: // from MCenRepNotifyHandlerCallback
		/**
		*	Receive notification about changed int value in central repository
		*	@param	aId			Id of the key that is changed in the central
		*						repository
		*	@param	aNewValue	New value of aId in the central repository
		*	@return	None
		*/
	    void HandleNotifyInt( TUint32 aId, TInt aNewValue );


	private: // Private methods

    	/**
    	*	Default constructor
    	*/
    	CIptvSearchManager( CIptvServer& aServer, CIptvEpgSession& aParent );
    	
    	/**
    	*	Symbian second phase constructor for initialization code that might
    	*	leave
    	*	@param	None
    	*	@return	None
    	*/
    	void ConstructL( CIptvEpgVodMsqQueue* aVodMsgQueue );
    	
    	/**
    	*	Method to take care of inserting search parameters to the search
    	*	http url. Search parameters are delivered to this class when
    	*	CIptvSearchManager::Search( TSearchQuery ) is called. Values found
    	*	from TSearchQuery is inserted into the search http address found
    	*	from CIptvService object's iSearchUrl.
    	*	@param	None
    	*	@return	None
    	*/
    	void ParseSearchAddressL( TIptvRssSearchQuery& aQuery );

        /**
        *	Handler method for search operation. This is the method that
        *	actually creates the plugin instance and sets all the necessary
        *	information for it in order to make metadata download and parsing
        *	possible
        *	@param	aQuery	Class that contains the search query parameters
        *	@return	None
        */    	
    	void HandleSearchL( TIptvRssSearchQuery& aQuery );
    	
        /**
        *	Creates a full details item of plugin provided epg content object
        *	@param	aContent	Content that contains the details to be stored
        *	@return	On return contains new full details object with details
        *			provided by aContent
        */
        CIptvVodContentContentFullDetails*
        	CreateContentFullDetailsFromEpgDataL( CIptvEpgContent& aContent );
        
        /**
        *	Directory is parsed when search manager receives the information
        *	about used service. Retrieved thumbnaildirectory path is stored to
        *	iThumbnailDirectory member variable.
        *	@param	None
        *	@return	None
        */
        void ResolveThumbnailDirectory();
        
        /**
        *	Reads the central repository values that are needed by the search
        *	manager.
        *	@param	None
        *	@return	None
        */
        void ReadCenRepData();

        /**
        * Resets base Content key to first available value.
        */        
        void ResetContentKey();

        /**
        * Increments base Content key to next available value.
        */        
        void IncrementContentKey();

        /**
        * Return current value of base Content key. Call IncrementContentKey()
        * separately to increment the key to next value.
        *
        * @return Current value of base Content key.
        */        
        TUint32 GetContentKey();

        /**
        * Sets base Content key to first available value starting from given base
        * value. Should not be called directly, as this is helper for other Content
        * key methods.
        *
        * @param aBase Base value for Content key's new value.
        */
        void SetToNextAvailableContentKey( TUint32 aBase );

	private: // Instance variables

    	/**
    	*	This is the category that represents the 'Search' to be displayed at the UI
    	*/
    	CIptvVodContentCategoryBriefDetails* iSearchCategory;
    	
    	/**
    	*	Pointer array containing the parsed content from the search url.
    	*/
    	RPointerArray<CIptvVodContentContentFullDetails> iAllContentFullDetails;
    	
    	
    	/**
    	*	Id for the service we are currently handling
    	*/
    	TUint32 iServiceId;
    	
    	/**
    	*	Locally stored object that has the details of the currently
    	*	active service.
    	*/    	
    	CIptvEpgService* iCurrentService;
    	
    	/**
    	*	Reference to the server object
    	*/    	
    	CIptvServer& iServer;
    	
    	/**
    	*	Epg session handle to check if vod update is run by RSS plugin
    	*/
        CIptvEpgSession& iParent;
    	
    	/**
    	*	Array of vod message queues.
    	*/    	
    	RPointerArray<CIptvEpgVodMsqQueue> iVodMsgQueues;
    	
    	/**
    	*	Pointer to the RSS plugin. This plugin is used to perform the
    	*	search operation (download, parsing and content detail sending
    	*	to search manager)
    	*/    	
    	CIptvEpgPluginInterface* iRssPlugin;
    	
    	/**
    	*	Iap to be used for the search http transactions.
    	*/    	
    	TUint32 iIap;
    	
    	/**
    	*	Boolean to define if this instance of the search manager is
    	*	running a RSS plugin for the search.
    	*/    	
    	TBool iPluginIsRunning;
    	
    	/**
    	*	Array that contains the media content items for all the 
    	*	found search results
    	*/    	
    	RPointerArray<CIptvContentIdMediaContentItem> iMediaContents;
    	
    	/**
    	*	Auto-increment value that is used as content id for the
    	*	CIptvVodContentContentFullDetails objects stored in
    	*	iAllContentFullDetails array. This value is set 
    	*/    	
    	TUint32 iContentKey;
    	
    	/**
    	* Array of Content ID numbers that have been used by downloads of previous
    	* searches. Array is stored so we don't re-use same key for two downloads.
    	*/
    	RArray<TUint32> iUsedContentKeys;

    	/**
    	*	Buf to contain the search url template that is used for search
    	*	This template contains the copy of search url value of the
    	*	iCurrentService
    	*/    	
    	HBufC* iSearchUrlTemplate;
    	
    	/**
    	*	Updater object that monitors the update progress
    	*/    	
    	CIptvEpgManagerUpdater* iUpdater;
    	
    	/**
    	*	Directory path pointing to directory where search thumbnails
    	*	are stored.
    	*/
    	TBuf<KIptvMaxPath> iThumbnailDirectory;
    	
    	/**
    	*	Fileserver session instance
    	*/
    	RFs iFs;
    	
    	/**
    	*	File manager instance used to delete directory that contains
    	*	thumbnails when this class get's deleted or we are about to
    	*	start new search
    	*/
    	CFileMan* iFileMan;
    	
    	/**
    	*	Central repository handle to read parental rating data
    	*/
    	CRepository* iCenRepSession;
    	
    	/**
    	*	Notify handler to receive notifications about changed value(s)
    	*	in central repository
    	*/
    	CCenRepNotifyHandler* iCenRepNotifyHandler;
    	
    	/**
    	*	Currently used parental rating value. This value is initialised
    	*	to 0 (KParentalRatingOff constant defined in .cpp file). Set value
    	*	is read from the central repository and this value can be updated
    	*	from central repository observer method HandleNotifyInt.
    	*/
    	TInt iParentalControl;
	};

#endif // CIPTVSEARCHMANAGER_H

// End of file.
