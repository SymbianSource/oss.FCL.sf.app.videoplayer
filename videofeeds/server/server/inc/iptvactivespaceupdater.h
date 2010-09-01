/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for following new video and service count and update*
*/




#ifndef IPTVACTIVESPACEUPDATER_H
#define IPTVACTIVESPACEUPDATER_H

// INCLUDES
#include <e32base.h>

#include "MIptvMyVideos.h"
#include "CIptvServiceManager.h"
#include "MIptvServiceManagerObserver.h"
#include "MIptvMyVideosClientObserver.h"

// CLASS DECLARATION

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
 class CActiveSpaceHelper;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

#if !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__) 
 
 /**
  * Class for following new video and service count and update Active Space
  * whenever changes happen.
  */
 
class CIptvActiveSpaceUpdater : public CBase,
                                public MIptvServiceManagerObserver
    {
    public:

        /**
         * Two-phased constructor. Creates a CIptvActiveSpaceUpdater object.
         *
         * @return Pointer to the created instance of CIptvActiveSpaceUpdater.
         */
        static CIptvActiveSpaceUpdater* NewL();

        /**
         * Destructor.
         */
        virtual ~CIptvActiveSpaceUpdater();

        /**
         * Called to initialise class with latest video and service data.
         *
         * @param aMyVideos Pointer to My Videos interface (not used).
         * @param aService Manager Pointer to Service Manager.
         */
        void InitialiseL( MIptvMyVideos* aMyVideos,
                          CIptvServiceManager* aServiceManager );
        
        // From MIptvServiceManagerObserver

        /**
         * CIptvServiceManager calls this to inform about events.
         *
         * @param aEvent Event sent by Service Manager.
         */
        void HandleSmEvent( CIptvSmEvent& aEvent );

    private:

        /**
         * Constructor.
         */
        CIptvActiveSpaceUpdater();

       /**
        * Updates service count based on Service Manager event.
        *
        * @param aEvent Event sent by Service Manager.     
        */
       void HandleSmEventL( CIptvSmEvent& aEvent ); 
        
        /**
         * Updates eiher vod service or live tv service array and
         * call corresponding update -method based on added service
         * 
         * @param aService service that has been added
         * 
         */
        void HandleServiceAddedUpdateL( CIptvService* aService );
        
        /**
         * Updates eiher vod service or live tv service array and
         * call corresponding update -method based on deleted service
         * 
         * @param aServiceId service that has been deleted
         * 
         */
        void HandleServiceDeletedUpdate( TUint32& aServiceId );
           
        /**
         * Lists all vod and Live tv services and updates service counts to clients.
         */
        void GetVodAndLiveTVServicesCountL();
        
    private: // DATA        
        /**
        * Pointer to Service Manager.
        */
       CIptvServiceManager* iServiceManager;

       /**
        * List of vod services.
        */
       RArray<TUint32> iVodServices;
 
       /**
        * List of live tv services.
        */
       RArray<TUint32> iLiveTvServices;
        
        /**
         * Count of vodcast services in video center.
         */
        RProperty iVodCastCountProperty;
        
        /**
         * Count of live tv services
         */
        RProperty iLiveTVServiceCountProperty;
      
    };

#else
 
class CIptvActiveSpaceUpdater : public CBase,
                                public MIptvMyVideosClientObserver,
                                public MIptvServiceManagerObserver
    {

public:

    /**
     * Two-phased constructor. Creates a CIptvActiveSpaceUpdater object.
     *
     * @return Pointer to the created instance of CIptvActiveSpaceUpdater.
     */
    static CIptvActiveSpaceUpdater* NewL();

    /**
     * Destructor.
     */
    virtual ~CIptvActiveSpaceUpdater();

    /**
     * Called to initialise class with latest video and service data.
     *
     * @param aMyVideos Pointer to My Videos interface.
     * @param aService Manager Pointer to Service Manager.
     */
    void InitialiseL( MIptvMyVideos* aMyVideos,
                      CIptvServiceManager* aServiceManager );

   
// From MIptvMyVideosClientObserver

    /**
     * Asynchronous callback observer function signaling application that
     * My Videos video contents has changed. Updated new video count.
     *
     * @param aEvent Event describing the change.
     */
    void ContentsUpdated( TIptvContentUpdatedEvent aEvent );

    /**
     * Asynchronous response callback function to video GetTotalVideoLenght().
     * Result is provided asynchronously because calculation may take some
     * time.
     * 
     * @param aTotalPlayTime Total play time in seconds.
     * @param aTotalFileSize Total file size in kilo bytes.
     */
    void TotalVideoLenghtResponse(
        TIptvPlayTime aTotalPlayTime,
        TIptvFileSize aTotalFileSize );

    /**
     * Asynchronous callback function signaling that copy or move operation
     * has been finished.
     *
     * @param aServerStatus    Request status.
     * @param aOperationStatus Status of actual copy or move operation.
     * @param aFailed          If several items was copied or moved and some
     *                         failed, the count of failed items.
     */
    void CopyOrMoveResponse(
        TInt aServerStatus,
        TInt aOperationStatus,
        TInt aFailed );

    /**
     * Asynchronous callback function signaling that delete operation has been
     * finished.
     *
     * @param aStatus Status of delete operation.
     */
    void DeleteResponse(
        TInt aStatus );
    
// From MIptvServiceManagerObserver

    /**
     * CIptvServiceManager calls this to inform about events.
     *
     * @param aEvent Event sent by Service Manager.
     */
    void HandleSmEvent( CIptvSmEvent& aEvent );

private:

    /**
     * Constructor.
     */
    CIptvActiveSpaceUpdater();

    /**
     * Updates service count to Active Space and/or PS keys.
     */
    void UpdateServiceCountToSpaceL();
    
    /**
     * Updates new video count to Active Space and/or PS keys.
     *
     */
    void UpdateVideoCountToSpaceL();

    /**
     * Lists all videos and updates new video count to clients.
     *
     * @param aForceUpdate If ETrue, new video count is always updated
     *                     to clients, no matter if it changes or not.     
     */
    void GetVideosCountL( TBool aForceUpdate );
   
    /**
     * Lists all services and updates service count to clients.
     */
    void GetVodcastServicesCountL();
      
    /**
     * Get newest unwatched video name and thumbnail path
     * @param aVideoList List of videos.
     */
    void GetNewestVideoL( RPointerArray<CIptvMyVideosVideoBriefDetails>& aVideoList );
    
    /**
     * Create list for newest video search.
     */ 
    void GetNewestVideoListL();
    
    /**
     * Set newest video name and thumbnail path.
     * @param aDetails Video details.
     */
    void SetNewestVideoL( CIptvMyVideosVideoBriefDetails& aDetails ); 
    
    /**
     * Updates service count based on Service Manager event.
     *
     * @param aEvent Event sent by Service Manager.     
     */
    void HandleSmEventL( CIptvSmEvent& aEvent );
 
    /**
     * Updates new video count based on My Videos event.
     *
     * @param aEvent Event describing the change.
     */
    void ContentsUpdatedL( TIptvContentUpdatedEvent aEvent );

private:

    /**
     * Data class for Global File Id information.
     */
    class TNewVideoInformation
        {
        public:

            /**
             * Drive Id.
             */
            TUint32 iDrive;

            /**
             * File Id.
             */            
            TUint32 iFileId;
        };
    
private:
    
    /**
     * Pointer to Service Manager.
     */
    CIptvServiceManager* iServiceManager;

    /**
     * List of vod services.
     */
    RArray<TUint32> iServices;
    
    /**
     * Pointer to My Videos interface.
     */
    MIptvMyVideos* iMyVideos;
    
    /**
     * List of new videos.
     */
    RArray<TNewVideoInformation> iNewVideos;
    
    /**
     * Newest video name.
     */
    HBufC*  iNewestVideoName;
    
    /**
     * Newest video thumbnail path.
     */
    HBufC*  iNewestVideoThumbnailPath;
      
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    CActiveSpaceHelper* iActiveSpaceHelper;	 // ActiveSpace Helper class
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    
    /**
     * Count of new videos in video storage
     */
    RProperty iNewVideoCountProperty;
    
    /**
     * Count of vodcast services in video center.
     */
    RProperty iVodCastCountProperty;
    
    /**
     * Newest video name in video storage.
     */
    RProperty iNewestVideoNameProperty;
    
    /**
     * Newest video thumbnail path in video storage.
     */
    RProperty iNewestVideoThumbnailPathProperty;
 
    };
#endif // !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)

#endif // IPTVACTIVESPACEUPDATER_H
