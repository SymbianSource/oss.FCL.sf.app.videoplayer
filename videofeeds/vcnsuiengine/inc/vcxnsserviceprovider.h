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




#ifndef C_VCXNSSERVICEPROVIDER_H
#define C_VCXNSSERVICEPROVIDER_H

// INCLUDES
#include "CIptvUtil.h"
#include <e32base.h>
#include <f32file.h>

#include "vcxnsserviceproviderif.h"
#include "vcxnsserviceproviderobserver.h"
#include "vcxnsservice.h"

// FORWARD DECLARATIONS
class MVcxNsServiceProviderObserver;
class CVcxNsUiEngine;

// CLASS DECLARATION

/**
* Class CVcxNsServiceProvider handles application services.
*
* @lib vcxnsuiengine.lib
*/
class CVcxNsServiceProvider : public CBase,
                              public MVcxNsServiceProviderIf
    { 

public:

    /**
     * Constructor
     * @param aUiEngine Reference to UI engine interface.
     * @return Pointer to CVcxNsServiceProvider object.
     */
    static CVcxNsServiceProvider* NewL( CVcxNsUiEngine& aUiEngine );

    virtual ~CVcxNsServiceProvider();

    /**
     * RegisterObserver.
     * @param aObserver Observer
     */
    void RegisterObserver( MVcxNsServiceProviderObserver* aObserver );

    /**
     * DeRegisterObserver.
     * @param aObserver Observer
     */
    void DeRegisterObserver( MVcxNsServiceProviderObserver* aObserver );

    /**
     * Gets all subscribed services.
     *
     * @return Vod services.
     */
    RPointerArray<CVcxNsService>& GetSubscribedServicesL();

    /**
     * Gets all services under active service group.
     * @return Group service list for active service group.
     */
    RPointerArray<CVcxNsService>& GetActiveGroupListL( );

    /**
     * Opens selected service.
     *
     * @param aIndex Index of the service to be opened.
     */
    void OpenServiceL( TInt aIndex );

    /**
     * Open service with given service id. 
     * 
     * @param aServiceId Id of service to open
     */
    void OpenServiceL( TUint32 aServiceId );
    
    /**
     * Get highlight index for category list. 
     * 
     * @return Current highlight index in category list.
     * If service is not categorised, 
     * then returns highlight for content list.
     */
    TInt GetCurrentHighlight();
    
    /**
     * Set highlight index for category list. 
     * @param aIndex Index to save
     */
    void SetCurrentHighlight( TInt aIndex );
    
    /**
     * Remove service
     * @param aIndex Index of the service to be removed
     */
    void RemoveServiceL( TInt aIndex );
    
    /**
     * Get pointer to service data
     * @param aIndex Index of the service in list
     * @return Pointer to service data.
     *         NULL returned, if service data not found
     */
    CVcxNsService* GetServiceData( TInt aIndex );
    
    /**
     * Get service data
     * @param aServiceId Service Id
     * @return Service data
     */
    CVcxNsService* GetServiceDataById( TUint32 aServiceId );
   
    /**
     * Return pointer to active service data
     * @return Pointer to active service data.
     *         NULL returned, if service data not found
     */
    CVcxNsService* GetActiveServiceData();
    
    /**
     * Reset all the service data.
     */
    void RemoveServiceData();

    /**
     * Handle the back command by choosing new application state.
     * @return Was the back event consumed. If EFalse, 
     *         the back event should be handled by caller.     
     */
    TBool HandleBack();
    
    /**
     * Start ecg update for all visible feeds.
     */
    void RefreshAllServicesL();
    
    /**
     * Open video directory
     */
    void OpenVideoDirectoryL();
    
    /**
     * Manage account for selected service.
     * @param aSelected Selected item index
     */
    void ManageAccountL( TInt aSelected );
    
    /**
     * Save authentication data for service.
     * This handles data for active service.
     * @param aUserName username
     * @param aPasswd passwd
     */
    void SetAuthenticationDataL( const TDesC& aUserName, 
                                 const TDesC& aPasswd );    
    
    /**
     * Notify service has been added.
     *
     * @param aIndex Added item's index
     */
    void ServiceAdded( TInt aIndex );

    /**
     * Service update notification.
     *
     * @param aIndex Updated item's index
     */
    void ServiceUpdated( TInt aIndex );
    
    /**
     * View needs to be refreshed.
     */
    void RefreshView();
    
    /**
     * Application state needs to be updated.
     */
    void HandleAppStateChangedL();
    
    /**
     * Update a service status by it's id
     * @param aServiceId service id
     * @param aStatus status
     */    
    void UpdateServiceUpdateStatusL( TUint32 aServiceId,
                                    CVcxNsService::TServiceUpdateStatus aStatus );
    
    /**
     * Start ecg update for next pending service
     */
    void RefreshNextPendingService();
    
    /**
     * Call update for the service list item with given ID.
     * @param aServiceId service id
     */
    void UpdateService( TUint aServiceId );
    
    /**
     * Update service's thumbnailpath from smdb.
     * @param aServiceId Service Id
     */
    void ReloadThumbnailPathL( TUint aServiceId );
    
    /**
     * Checks if feed has more details than just name
     * 
     * @param aSelected Index of selected item
     * @return Does the feed have other details than just name
     */
    TBool HasFeedDetails( TInt aSelected );

    /**
     * Get list index for defined service id.
     * @param aServiceId Service id
     * @return  Service's index with given ID.
     */
    TInt ServiceIndex( TUint32 aServiceId );
    
    /**
     * Moves a service from given position to another.
     * 
     * @param aSourceIndex where from move the service
     * @param aTargetIndex  where to move the service.
     * @return error code
     */
     TInt MoveServiceL( TInt aSourceIndex, TInt aTargetIndex );
     
     /**
      * Check if item has account management uri.
      * @param aSelected Index of selected item.
      * @return ETrue if account management uri available.
      */
     TBool HasAccountManagementUri( TInt aSelected );
     
     /**
      * Update service data in ui engine cache
      * @param aServiceId Service id of updated service
      */
     void UpdateServiceInCacheL( TUint32 aServiceId );

     /**
      * Cancel update queue.
      */
     void CancelUpdateQueue();

private:

    /**
     * Constructor
     * @param aUiEngine Reference to UI engine interface.
     */
    CVcxNsServiceProvider( CVcxNsUiEngine& aUiEngine );
    
    /**
     * Constructor.
     */
    void ConstructL();

    /**
     * Check, that the file with given path actually exists.
     * @param File path to check
     * @return Do the file with given path exist.
     */
    TBool EnsureFileExists( const TDesC& aPath );

    /**
     * Start manual update for service.
     * @param aServiceId Service id
     */
    void StartManualUpdateL( TUint32 aServiceId );
    
    /**
     * Start manual update for service.
     * @param aService Service data
     */
    void UpdateServiceL( CVcxNsService& aService );
    
    /**
     * Set service back to queue to wait for update.
     * @param aServiceId Service id.
     */
    void SetBackToUpdateQueue( TUint32 aServiceId );
    
    /**
     * Get grouped services.
     * @param aGroupId Id of service group
     * @return Grouped service list.
     */
    RPointerArray<CVcxNsService>& GetGroupedServicesL( TUint32 aGroupId );
    
    /**
     * Open selected group service.
     *
     * @param aIndex List index from customized service view.
     */
    void OpenServiceFromGroupL( TInt aIndex );
    
    /**
     * Open link to browser.
     * @param aUri Link to open in browser.
     */
    void OpenToBrowserL( const TDesC& aUri );
    
    /**
     * Open link to player.
     *
     * @param aUri Link to open in player.
     * Leaves with KErrArgument if not rtsp link.
     */
    void OpenToPlayerL( const TDesC& aName, const TDesC& aUri );
    
    /**
     * Opens a video service
     *
     * @param aServiceId Service Id to open.
     * @param aIndex Service's Index to open.
     */
    void OpenVideoServiceL( TUint32 aServiceId, TInt aIndex );
    
private:    

// Data
    
    /**
     * Array of service event observers. Not Own.
     */
    RPointerArray<MVcxNsServiceProviderObserver> iServiceObservers;
    
    /**
     * UI engine.
     */
    CVcxNsUiEngine& iUiEngine;
    
    /**
     * Service list.
     */
    RPointerArray<CVcxNsService> iServiceList;
    
    /**    
     * RFs.
     */
    RFs iFs;
    
    /**
     * Ids of services in update queue.
     * This array should only have contents when user has
     * selected refresh for all feeds.
     */
    RArray<TUint32> iUpdateQueue;   
    };

#endif // C_VCXNSSERVICEPROVIDER_H
