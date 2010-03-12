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
#include <thumbnailmanager.h>

#include "CIptvDriveMonitor.h"

// FORWARD DECLARATIONS

class CVcxHgMyVideosCollectionClient;
class CVcxHgMyVideosDownloadClient;
class CRepository;
class CVcxHgMyVideosThumbnailManager;

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
         * Gets the last watched video id from CenRep.
         * 
         * @param aId Video's mpx id (iId1).
         * @return System wide error code
         */
        TInt GetLastWatchedIdL( TInt& aId );

        /**
         * Gets video list sort order (from CenRep).
         * 
         * @return Video list sort order.
         */
        TVcxMyVideosSortingOrder VideolistSortOrderL();

        /**
         * Gets the My Videos customization integer from CenRep.
         * 
         * @param aKey   CenRep key for the value to be fetched.
         * @param aValue On return, fetched value.
         * @return System wide error code
         */
        TInt GetMyVideosCustomizationInt( const TInt& aKey, TInt& aValue );

        /**
         * Gets the My Videos customization string from CenRep.
         * 
         * @param aKey   CenRep key for the value to be fetched.
         * @param aValue On return, fetched value.
         * @return System wide error code
         */
        TInt GetMyVideosCustomizationString( const TInt& aKey, TDes& aValue );
        /**
         * Returns reference to Thumbnail Manager.
         *
         * @return Reference to Thumbnail Manager.
         */
        CVcxHgMyVideosThumbnailManager& ThumbnailManager() const;

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
         * Initializes MyVideos customization CenRep session.
         */
        TInt InitMyVideosCenRepL();
        
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
         * Session to My Videos customization CenRep.
         * Own.
         */
        CRepository* iMyVideosCenRep;
    
        /**
         * Video list sort order.
         */
        TVcxMyVideosSortingOrder iSortOrder;

		/**
         * Thumbnail Manager.
         * Own.
         */
        CVcxHgMyVideosThumbnailManager* iTnManager;
    };

#endif // VCXHGMYVIDEOSMODEL_H
