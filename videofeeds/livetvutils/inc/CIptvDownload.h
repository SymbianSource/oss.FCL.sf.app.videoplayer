/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Http download handler for epg plugin*
*/



#ifndef __CLIVETVDOWNLOAD_H
#define __CLIVETVDOWNLOAD_H

// User includes
#include <e32std.h>
#include <DownloadMgrClient.h>
#include "MIptvDownloadObserver.h"
#include "MIptvLiveUITimerObserver.h"

// FORWARD DECLARATIONS
class MIptvDownloadObserver;
class CIptvEpgService;
class CIptvLiveUIObjectTimer;

/**
*	Class to provide download functionality for i.e. epg plugins to download
*	content files over http. Download observer is used to report progress of
*	of the downloads to the user of this class.
*/
class CIptvDownload : public CBase,
					  public MHttpDownloadMgrObserver,
					  public MIptvLiveUITimerObserver
	{
	public:
	/**
	*	Symbian two phase constructor
	*	@param	None
	*	@return	Pointer to newly created CLiveTvHttpDownload object
	*/
	IMPORT_C static CIptvDownload* NewL( MIptvDownloadObserver* aObserver );

	/**
	*	Destructor
	*/
	~CIptvDownload();

	public: // Implemented methods from inherited M-classes

        /**
         *	From MHttpDownloadMgrObserver. Observer function to receive
         *	notification events about sessions and sub-sessions of Download
         *	Manager.
         *	@param	aDownload	Download resource handle which event occured in
         *	@param	aEvent		Occured download event. Contains following
    	 *						two values:
         *						THttpDownloadState
         *						THttpProgressState
         *	@return	None
         */
        void HandleDMgrEventL(RHttpDownload& aDownload, THttpDownloadEvent aEvent);

        /**
        * From MIptvLiveUITimerObserver. Timer fired successfully.
        */
        void TimerFires();

        /**
        * From MIptvLiveUITimerObserver. Error occured in the timer.
        * @param aError KErrNone if timer fired ok, KErrCancel, the timer was
    	* cancelled; KErrAbort, the timer was aborted because the system time
    	* changed; KErrUnderflow, the requested completion time is in the past;
    	* KErrOverFlow, the requested completion time is too far in the future.
        */
        void TimerError( const TInt aError );

	public: // new public methods

    	/**
    	*	Setter for service data to be used to retrieve username and password
    	*	to establish the download.
    	*	@param	aService	Object containing data for download object to
    	*						initialize download. This method needs to be
    	*						called before callind DownloadL.
    	*	@return	None
    	*/
    	IMPORT_C void SetService( CIptvEpgService* aService );

    	/**
    	*	Function to be used to start http download.
    	*	@param	aUid			Appuid of the application which is connecting in
    	*							download manager. This valua can be retrieved from
    	*							CIptvEpgService object
    	*	@param	aUri			Uri where download is done from
    	*	@param	aIapId			Internet accesspoint's id to be  used
    	*	@param	aFileName		Name of the file where retrieved data is put (this
    	*							also requires the path)
    	*	@param	aETag			Descriptor containing the previous ETag value. This
    	*							value is used to check if there has been an update
    	*							for the data on the server side when we receive content
    	*							type and ETag value in response header.
    	*	@param	aModifiedSince	String representing the modified since value of last
    	*							download. This value is used to compare with last modified
    	*							value from http header after receiving content type.
    	*	@return	None
    	*/
    	IMPORT_C void DownloadL( TUid aUid,
    							 const TDesC8& aUri,
    							 TUint32 aIapId,
    							 const TDesC& aFileName,
    							 const TDesC& aETag = KNullDesC(),
    							 const TDesC& aModifiedSince = KNullDesC() );

	private: // new private methods

    	/**
    	*	Default constructor
    	*/
    	CIptvDownload( MIptvDownloadObserver* aObserver );

    	/**
    	*	Symbian second phase constructor
    	*	@param	None
    	*	@return	None
    	*/
    	void ConstructL();

    	/**
    	*	Method to map download error to one of the enumerations defined
    	*	in TIptvDlError.
    	*	@param	aDownload	Object containing data for the download object
    	*						that faced an error situation
    	*	@param	aError		On return contains mapped error enumeration
    	*						and if no match found, EIptvDlGeneral is returned
    	*/
    	void ResolveDownloadErrorCode( RHttpDownload& aDownload,
    								   TIptvDlError& aError ) const;


    	/**
    	*	Initializes the download meaning takes connection to download
    	*	manager and sets used IAP from iService. This method is called
    	*	from SetService.
    	*	@param	None
    	*	@return	None
    	*/
    	void InitializeDownloadL();

        /**
        *  Delete current download asynchronously.
        */
        void DeleteDownloadAsyncL();

        /**
        * Create download.
        */
        void CreateDownloadL( const TDesC8& aUri, TBool aNoContentTypeCheck );

        /**
        * Leaving handler for TimerError().
        * 
        * @param aError KErrNone if timer fired ok, KErrCancel, the timer was
        * cancelled; KErrAbort, the timer was aborted because the system time
        * changed; KErrUnderflow, the requested completion time is in the past;
        * KErrOverFlow, the requested completion time is too far in the future. 
        */
        void HandleTimerErrorL( const TInt aError );
        
	private: // instance variables

    	/**
    	*	Handle to download manager to be used to retrieve data over http
    	*/
        RHttpDownloadMgr iDownloadManager;

        /**
        *	Pointer to download observer interface. This interface is used to
        *	signalize the caller about the progress of the download.
        */
        MIptvDownloadObserver* iObserver;

        /**
        *	Service containing specified data for the current download.
        */
        CIptvEpgService* iService;

    	/**
    	* ETrue if download manager is connected, EFalse otherwise.
    	*/
    	TBool iConnected;

    	TBuf<100> iDownloadETag;

    	TBuf<100> iDownloadLastModified;

    	/**
    	* Url for the download.
    	*/
    	TBuf8<KIptvUrlMaxLength> iUrl;

        /**
        * File name for the download.
        */
    	TBuf<KIptvUrlMaxLength> iFileName;

    	/**
    	* Timer for asynchronous operations like deleting the download.
    	*/
    	CIptvLiveUIObjectTimer* iTimer;

    	/**
    	* Flag to identify if async download deletion is ongoing.
    	*/
    	TBool iDeleteDownloadTimerRunning;

    	/**
        * Identifies download.
        */
        TInt32 iDownloadId;

        /**
        * Error to be sent to observer.
        */
        TInt iDlError;

        /**
        * After getting last-modified info we restart the download.
        */
        TBool iRestartDownloadAfterDeletion;
	};

#endif // __CLIVETVDOWNLOAD_H
