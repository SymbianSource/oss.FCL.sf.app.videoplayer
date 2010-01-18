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
* Description:      Common model class for component.*
*/




#ifndef VCXHGMYVIDEOSMODEL_H
#define VCXHGMYVIDEOSMODEL_H

// INCLUDE FILES
#include <coemain.h>
#include <vcxmyvideosdefs.h>
#include <MediatorEventProvider.h>
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include <videoplayercustommessage.h>

#include "CIptvDriveMonitor.h"

// FORWARD DECLARATIONS

class CVcxHgMyVideosCollectionClient;
class CVcxHgMyVideosDownloadClient;
class CRepository;
class CIptvLastWatchedApi;
class CIptvLastWatchedData;


// CONSTANTS

const TInt KVcxHgMyVideosCategoryControlGroupId(500);
const TInt KVcxHgMyVideosVideoControlGroupId(600);

// CLASS DECLARATION

/**
 * Common model class for component.
 *
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosModel ) : public CBase,
                                           public MThumbnailManagerObserver,
                                           public MIptvDriveMonitorObserver
    {
    public:
        
        /**
         * Application / view states.
         */
        enum TVcxMyVideosAppState
            {
            EVcxMyVideosAppStateUnknown = 0,
            EVcxMyVideosAppStateCategoryBusy,
            EVcxMyVideosAppStateCategoryIdle,
            EVcxMyVideosAppStateVideoBusy,
            EVcxMyVideosAppStateVideoIdle,
            EVcxMyVideosAppStatePlayer
            };
        
        /**
         * Screen resolutions.
         */
        enum TVcxScreenResolution
            {
            EVcxScreenResolutionUnknown = 0,
            EVcxScreenResolutionQVGA,
            EVcxScreenResolutionQHD,
            EVcxScreenResolutionVGA
            };        
        
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @return New object.
         */
        static CVcxHgMyVideosModel* NewL();

        /**
         * Two-phased constructor.
         *
         * @return New object.
         */
        static CVcxHgMyVideosModel* NewLC();

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosModel();

    public:
        
        /**
         * Returns true if device has touch support.
         * 
         * @return ETrue if device supports touch.
         */
        TBool TouchSupport();
        
        /**
         * Returns current screen resolution (QVGA,QHD,VGA).
         * 
         * @param aCoeEnv Pointer to Coe Env.
         * @return Screen resolution.
         */
        TVcxScreenResolution GetScreenResolution( CCoeEnv* aCoeEnv );

        /**
         * Returns MPX Collection Client.
         *
         * @return Reference to Collection client.
         */
        CVcxHgMyVideosCollectionClient& CollectionClient();
        
        /**
         * Returns Download client.
         *
         * @return Reference to Download client.
         */
        CVcxHgMyVideosDownloadClient& DownloadClient();
        
        /**
         * Returns reference to component's File Server session.
         * 
         * @return Reference to File Server session.
         */
        RFs& FileServerSessionL();
        
        /**
         * Returns reference to component's drive monitor.
         * 
         * @return Reference to drive monitor.
         */
        CIptvDriveMonitor& DriveMonitorL();

        /**
         * Sets application state.
         *
         * @param aAppState New application state.
         */
        void SetAppState( TVcxMyVideosAppState aAppState );
        
        /**
         * Gets application state.
         *
         * @return Application state.
         */
        TVcxMyVideosAppState AppState();

        /**
         * Returns previous application state.
         *
         * @return Previous application state.
         */
        TVcxMyVideosAppState PreviousAppState();

        /**
         * Sets video list sort order (to CenRep).
         * 
         * @param aSortOrder New video list sort order.
         */
        void SetVideolistSortOrderL( TVcxMyVideosSortingOrder aSortOrder );

        /**
         * Gets video list sort order (from CenRep).
         * 
         * @return Video list sort order.
         */
        TVcxMyVideosSortingOrder VideolistSortOrderL();

        /**
         * Should be called when playback of video is stopping. Updates last
         * play point of video to 'Last Watched' dat-file.
         */
        void UpdateLastWatchedPlayPositionL( TUint32 aLastVideoPlayPoint );
        
        /**
         * Cancels the download completed soft notification.
         */
        void ResetDownloadNotification();
        
        /**
         * Should be called when playback of video is starting. Updates 'Last
         * Watched' information to Matrix and dat-file.
         * 
         * @param aVideoInfo Information about played video.
         * @param aMpxId1 MPX ID 1 of the played video.
         * @param aAgeProfile Age profile of the video for parental control.
         */
        void SetVideoAsLastWatchedL( TVideoPlayerCustomMessage& aVideoInfo, 
                                     TInt aMpxId1,
                                     TUint32 aAgeProfile );

        /**
         * Reads information about 'Last Watched' video clip from dat-file.
         * 
         * @param aVideoInfo On return, information about last played video.
         * @param aMpxId1 On return, MPX Item Id 1.
         * @param aAgeProfile On return, the age profile of the video for 
         *      parental control
         */
        void GetLastWatchedDataL( TVideoPlayerCustomMessage& aVideoInfo, 
                                  TUint32& aMpxId1,
                                  TUint32& aAgeProfile );
        
    public: // From MThumbnailManagerObserver
        
        /**
         * Preview thumbnail generation or loading is complete.
         *
         * @param aThumbnail An object representing the resulting thumbnail.
         * @param aId Request ID for the operation
         */
        void ThumbnailPreviewReady( MThumbnailData& aThumbnail, TThumbnailRequestId aId );
    
        /**
         * Final thumbnail bitmap generation or loading is complete.
         *
         * @param aError Error code.
         * @param aThumbnail An object representing the resulting thumbnail.
         * @param aId Request ID for the operation.
         */
        void ThumbnailReady( TInt aError, MThumbnailData& aThumbnail, TThumbnailRequestId aId );
    
    public: // from MIptvDriveMonitorObserver
        
        /**
        * Empty handler for drive monitor events.
        *
        * @aEvent  The Event.
        */
        void HandleDriveMonitorEvent( TIptvDriveMonitorEvent& aEvent );
        
    private: // Constructors

        /**
         * Default C++ constructor. 
         */
        CVcxHgMyVideosModel();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();

        /**
         * If needed creates, and returns pointer to Last Watched API.
         * 
         * @return Pointer to Last Watched API.
         */
        CIptvLastWatchedApi* LastWatchedApiL();
        
        /**
         * If needed creates, and returns pointer to Last Watched Data.
         * 
         * @return Pointer to Last Watched Data.
         */
        CIptvLastWatchedData* LastWatchedDataL();

        /**
         * Copies data from one Video Player Custom message to another.
         * 
         * @param aSource Source object.
         * @param aTarget Target object.
         */
        void CopyVideoPlayerCustomMessageL( TVideoPlayerCustomMessage& aSource,
                                            TVideoPlayerCustomMessage& aTarget );
        
        /**
         * If needed creates, and returns pointer to Video Player Custom Message.
         * 
         * @return Pointer to Video Player Custom Message.
         */        
         TVideoPlayerCustomMessage* VideoPlayerCustomMessageL();       
        
        /**
         * Handles ThumbnailReady() call.
         *
         * @param aError Error code.
         * @param aThumbnail An object representing the resulting thumbnail.
         * @param aId Request ID for the operation.
         */        
        void HandleThumbnailReadyL( TInt aError,
                                    MThumbnailData& aThumbnail,
                                    TThumbnailRequestId aId );        
        /**
         * If needed creates, and returns pointer to Thumbnail Manager.
         *
         * @return Pointer to Thumbnail Manager.
         */
        CThumbnailManager* ThumbnailManagerL();
        
    private:

        /**
         * Client class for My Videos MPX Collection.
         * Own.
         */
        CVcxHgMyVideosCollectionClient* iCollection;

        /**
         * Application state.
         */
        TVcxMyVideosAppState iAppState;
        
        /**
         * Previous application state.
         */
        TVcxMyVideosAppState iPreviousAppState;        
        
        /**
         * Internal flag for storing touch support information.
         */
        TBool iTouchSupport;
        
        /**
         * Component's File Server session.
         * Own.
         */
        RFs iFsSession;
        
        /**
         * Component's drive monitor.
         * Own.
         */
        CIptvDriveMonitor* iDriveMonitor;
        
        /**
         * Session to MPX Collection CenRep.
         * Own.
         */
        CRepository* iCollectionCenRep;
        
        /**
         * Pointer to 'Last Watched' API.
         * Own.
         */
        CIptvLastWatchedApi* iLastWatchedApi;
        
        /**
         * Pointer to 'Last Watched' data object.
         * Own.
         */
        CIptvLastWatchedData* iLastWatchedData;

        /**
         * Pointer to Video Player custom message.
         * Own.
         */
        TVideoPlayerCustomMessage* iVideoPlayerCustomMessage;
        
        /**
         * S60 Thumbnail Manager.
         * Own.
         */
        CThumbnailManager* iTnManager;
        
        /**
         * Request ID of ongoing thumbnail request, or KErrNotFound.
         */
        TThumbnailRequestId iTnRequestId;

        /**
         * Pointer to Mediator Event class.
         * Own.
         */        
        CMediatorEventProvider* iMediatorEventProvider;
    
        /**
         * Video list sort order.
         */
        TVcxMyVideosSortingOrder iSortOrder;
    };

#endif // VCXHGMYVIDEOSMODEL_H
