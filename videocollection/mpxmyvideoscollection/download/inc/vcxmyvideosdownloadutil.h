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




#ifndef VCXMYVIDEOSDOWNLOADUTIL_H
#define VCXMYVIDEOSDOWNLOADUTIL_H

// INCLUDES

#include <DownloadMgrClient.h>
#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>
#include <ipvideo/vcxconnutilengineobserver.h>
#include "vcxmyvideostimer.h"

// FORWARD DECLARATIONS
class CVcxConnectionUtility;
class CMediatorEventProvider;
class RFs;

// CONSTANTS
const TInt KVcxSubDirLevels     = 2;
const TInt KVcxMaxSubDirLength  = 10;    
const TInt KVcxMaxSubDirsLength = KVcxMaxSubDirLength * KVcxSubDirLevels;
const TInt KVcxMaxItemsInDir    = 100;

// CLASS DECLARATION

NONSHARABLE_CLASS(MVcxMyVideosDownloadUtilObserver)
    {
    public:
        /**
        * Event handler which client must implement.
        *
        * @param aDownloadId           Download ID in Download Manager.
        * @param aProgress             Download progress, 0-100.
        * @param aDownloaded           Current file size, in bytes.
        * @param aDownloadError        If download goes to failed state, this contains the reason code
        *                              otherwise 0.
        * @param aDownloadGlobalError  if download goes to failed state, this contains the global reason
        *                              code, otherwise 0.
        */
        virtual void HandleDlEvent( TVcxMyVideosDownloadState aState,
                TUint32 aDownloadId,
                TInt aProgress,
                TInt64 aDownloaded,
                TInt32 aDownloadError,
                TInt32 aDownloadGlobalError ) = 0;
    };

/**
* Download related stuff.
*
* @lib mpxmyvideoscollectionplugin.dll
*/
NONSHARABLE_CLASS(CVcxMyVideosDownloadUtil) :
    public CBase,
    public MHttpDownloadMgrObserver,
    public MVcxMyVideosTimerObserver,
    public MConnUtilEngineObserver
    {    
    public: // Constructors and destructor

        /**
        * Two-phased constructor
        * @param aObserver Observer
        * @param aFs       File server session.
        * @return          Object constructed
        */
        static CVcxMyVideosDownloadUtil* NewL ( MVcxMyVideosDownloadUtilObserver& aObserver, RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosDownloadUtil();

    public:
        /**
        * Starts download. New download ID is written to aDownload. If KVcxMediaMyVideosDownloadId
        * != 0, then the existing download is resumed.
        *
        * @param aDownload download parameters
        */
        void StartDownloadL( CMPXMedia& aDownload );

        /**
        * Pauses download.
        *
        * @param aDownloadId Download ID.
        * @return            System-wide error code.
        */
        TInt PauseDownload( TUint32 aDownloadId );

        /**
        * Returns download by download ID. Ownership is not moved.
        *
        * @param aDownloadId Download ID.
        * @return            Download, or NULL if not found.
        */
        RHttpDownload* Download( TUint32 aDownloadId );

        /**
        * Returns download which has aFileName as EDlAttrDestFilename.
        * Ownership is not moved.
        *
        * @param aFileName Filename.
        * @return          Download, or NULL if not found.
        */
        RHttpDownload* Download( const TDesC& aFileName );
        
        /**
        * Returns array to downloads.
        */
        const CDownloadArray& Downloads();

        /**
        * Deletes download from download manager using download id.
        *
        * @param aDeleteContent If ETrue, then content is deleted also.
        * @return               System-wide error code.
        */
        TInt CancelDownload( TInt32 aId, TBool aDeleteContent );

        /**
        * Gets MPX/MDS version of the download state.
        *
        * @param aDownload       Download whose state is fetched.
        * @param aDownloadState  On completion, the download state is written here.
        */        
        void GetDownloadState(
                RHttpDownload& aDownload,
                TVcxMyVideosDownloadState& aDownloadState );
        
        /**
        * Returns download state.
        *
        * @param aDownload   Download
        * @param aDownloaded How much the file has been downloaded in bytes.
        * @param aAllowSilentReset if ETrue, silent flag is set to EFalse, in
        *                          case dl size is not gotten ok.
        * @return            Download progress, in percentages.
        */        
        TInt8 DownloadProgress( RHttpDownload& aDownload, TUint64& aDownloaded,
                                TBool aAllowSilentReset );

        
        /**
        * Checks that every download item in Download Manager has corresponding
        * object in MPX/MDS (= aVideoList), if not, deletes the download.
        */
        void ClearOrphanDownloadsL( CMPXMedia& aVideoList );

        /**
        * Deletes download asynchronously.
        *
        * @param aDownloadId    Download to delete.
        * @param aDeleteContent If ETrue, then the content file is also deleted.
        */
        void DeleteDownloadAsync( TInt32 aDownloadId, TBool aDeleteContent );

        /**
        * Creates path and filename for the new video download.
        *
        * @return The path. 
        */
        HBufC* CreateFilePathL( const CMPXMedia& aMedia );
        
        /**
        * Notify via mediator, that one download has finished succesfully.
        *
        * @param aMsg Buffer that contains the data that is sent with event.
        */
        void NotifyDownloadCompleted( const TDesC8& aMsg );
        
        /**
        * Notify via mediator, that count of new videos has decreased.
        * 
        * @param aMsg Buffer that constains the data that is sent with event.  
        */
        void NotifyNewVideosCountDecreased( const TDesC8& aMsg );
        
    public: // From MHttpDownloadMgrObserver 

        /**
         * Handler function for download events.
         *
         * @param aDownload download
         * @aEvent aEvent event
         */
        void HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent );

    public: // from MConnUtilEngineObserver
    
        /**
         * Roaming request from connectioutility.
         * Download utility decides whether it allows 
         * iap roaming and returns true or false based on decision.
         *
         * @return TBool
         */
        TBool RequestIsRoamingAllowedL();
    
        /**
         * Iap change notification from connectiontutility
         *
         */
        void  IapChangedL();              
    
    public: // from MVcxMyVideosTimer
        /**
        * Handle timer event.
        *
        * @param aTimer pointer to timer which expired.
        */
        void TimerExpired( CVcxMyVideosTimer* aTimer );
            
    private:
        /**
        * Constructor
        *
        * @param aObserver Observer
        * @param aFs       File server session.
        */
        CVcxMyVideosDownloadUtil( MVcxMyVideosDownloadUtilObserver& aObserver, RFs& aFs );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL ();

#ifdef _DEBUG
        const TDesC& GetS60DlStateDes( TInt32 aState );
        const TDesC& GetS60DlProgressDes( TInt32 aProgress );
#endif

        /**
        * Signal via mediator event that there is ongoing downloads.
        */
        void NotifyDownloadStarted();

        /**
        * Check, is there running downloads. If not, notify via mediator
        */
        void NotifyIfNoActiveDownloads();

        /**
        * Counts active downloads.
        * @return Active download count.
        */
        TInt32 ActiveDownloadCount();
        
        /**
        * Creates filepath.
        *
        * @param aMedia    Media item whose path is created
        * @param aPath     New path is written here.
        * @param aCounter  Counter value which is inserted to filename. 
        */
        void GeneratePathL( const CMPXMedia& aMedia, TDes& aPath, TInt aCounter );

        /**
        * Finds a sub dir which has less than KVcxMaxItemsInDir items, if not
        * found, creates one. The result is stored to iSubDir.
        *
        * @param aPath path
        */
        void FindSubDirL( const TDesC& aPath );

        /**
        * Calculates entries from the path.
        *
        * @param aPath Path.
        * @returm      Entrycount on the path.
        */
        TInt FileCountL( const TDesC& aPath );
        
        /**
        * Returns the drive section of the file path,
        * according to preferred cenrep setting. If drive
        * is locked or not present, the next available drive is
        * tried. If no available drives found, system drive is used.
        * Stores value to iPreferredMemoryDrivePath.
        *
        * @return   The drive section of the file path.
        */
        const TDesC& UsedMemoryDesL();

        /**
        * Stops iDownloadProgressTimer.
        */
        void StopProgressTimer();

        /**
        * Starts iDownloadProgressTimer.
        */
        void StartProgressTimer();

    private: //data
    
        /**
        * S60 Download manager client object.
        */
        RHttpDownloadMgr iDownloadManager;

        /**
        * Storage class for user name, used for 16->8 bit conversion.
        * This is member variable to reduce stack usage.
        */
        TBuf8<KMaxDefAttrLength> iUserName;

        /**
        * Storage class for password, used for 16->8 bit conversion.
        * This is member variable to reduce stack usage.
        */
        TBuf8<KMaxDefAttrLength> iPassword;

        /**
        * Storage class for url, used for 16->8 bit conversion.
        * This is member variable to reduce stack usage.
        */
        TBuf8<KMaxUrlLength> iUrl;
        
        /**
        * Observer of this class.
        */
        MVcxMyVideosDownloadUtilObserver& iObserver;

        /**
        * Used to delete download asynchronously, own.
        */
        CVcxMyVideosTimer* iDownloadDeleteTimer;

        /**
        * Used to update download progresses, own.
        */
        CVcxMyVideosTimer* iDownloadProgressTimer;

        /**
        * Downloads which are deleted asynchronously are stored here.
        */
        RArray<TInt32> iDownloadsToDelete;         

        /**
        * Delete content flags for async download delete are stored here.
        */
        RArray<TBool> iDeleteContent;
        
        /**
        * connectionutility
        */
        CVcxConnectionUtility* iConnUtil;
    
        /**
        * iMediatorEventProvider
        */
        CMediatorEventProvider* iMediatorEventProvider;

        /**
        * IAP currently set to Download Manager.
        * is used to avoid new set if IAP is the same already.
        */
        TUint32 iCurrentIap;
        
        /**
        * Used for storing the drive section of the path,
        * c:, e:, etc.
        */
        TBuf<2> iUsedMemoryDrivePath;
        
        /**
        * Drive used for downloading.
        */
        TInt iUsedDrive;
        
        /**
        * Fileserver session.
        */
        RFs& iFs;
        
        /**
        * SubDir is stored here.
        */
        TBuf<KVcxMaxSubDirsLength> iSubDir;
        
        /**
        * Flag that tells do we really have a roaming situation.
        */
        TBool iRoamingOngoing;
    };

#endif   // VCXMYVIDEOSDOWNLOADUTIL_H
