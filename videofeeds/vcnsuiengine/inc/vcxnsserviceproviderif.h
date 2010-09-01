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




#ifndef M_VCXNSAPPSERVICEPROVIDERIF_H
#define M_VCXNSAPPSERVICEPROVIDERIF_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class MVcxNsServiceProviderObserver;
class CVcxNsService;

/**
* Class MVcxNsServiceProviderIf 
* Interface for using UI engine's service provider
*
* @lib vcxnsuiengine.lib
*/
class MVcxNsServiceProviderIf
    {

public:

    /**
     * RegisterObserver.
     * @param aObserver Observer
     */
    virtual void RegisterObserver( MVcxNsServiceProviderObserver* aObserver ) = 0;

    /**
     * DeRegisterObserver.
     * @param aObserver Observer
     */
    virtual void DeRegisterObserver( MVcxNsServiceProviderObserver* aObserver ) = 0;

    /**
     * Gets all subscribed services.
     *
     * @return Vod services.
     */
    virtual RPointerArray<CVcxNsService>& GetSubscribedServicesL() = 0 ;
    
    /**
     * Gets all services under active service group.
     * @return Group service list for active service group.
     */
    virtual RPointerArray<CVcxNsService>& GetActiveGroupListL() = 0;

    /**
     * Opens selected service.
     *
     * @param aIndex Index of the service to be opened.
     */
    virtual void OpenServiceL( TInt aIndex ) = 0 ;

    /**
     * Open service with given service id. 
     * 
     * @param aServiceId Id of service to open
     */
    virtual void OpenServiceL( TUint32 aServiceId ) = 0;

    /**
     * Remove service
     *
     * @param aIndex Index of the service to be removed.
     */
    virtual void RemoveServiceL( TInt aIndex ) = 0;
    
    /**
     * Get pointer to service data
     * @param aIndex Index of the service in list
     * @return Pointer to service data.
     *         NULL returned, if service data not found
     */
    virtual CVcxNsService* GetServiceData( TInt aIndex ) = 0;
    
    /**
     * Return pointer to active service data
     * @return Pointer to active service data.
     *         NULL returned, if service data not found
     */
    virtual CVcxNsService* GetActiveServiceData() = 0;

    /**
     * Handle the back command by choosing new application state.
     * @return Was the back event consumed. If EFalse, 
     *         the back event should be handled by caller.     
     */
    virtual TBool HandleBack() = 0;

    /**
     * Start ecg update for all visible feeds.
     */
    virtual void RefreshAllServicesL() = 0;
    
    /**
     * Open video directory.
     */
    virtual void OpenVideoDirectoryL() = 0;

    /**
     * Manage account for selected service.
     * @param aSelected Selected item index
     */
    virtual void ManageAccountL( TInt aSelected ) = 0;
    
    /**
     * Save authentication data for service.
     * This handles data for active service.
     * @param aUserName username
     * @param aPasswd passwd
     */
    virtual void SetAuthenticationDataL( 
            const TDesC& aUserName, 
            const TDesC& aPasswd ) = 0;

    /**
      * Checks if feed has more details than just name
      * 
      * @param aSelected Index of selected item
      * @return Does the feed have other details than just name
      */
     virtual TBool HasFeedDetails( TInt aSelected ) = 0;
     
     /**
      * Moves a service from given position to another.
      * 
      * @param aSourceIndex where from move the service
      * @param aTargetIndex  where to move the service.
      * @return error code
      */
     virtual TInt MoveServiceL( TInt aSourceIndex, TInt aTargetIndex ) = 0;
     
     /**
      * Check if item has account management uri.
      * @param aSelected Index of selected item.
      * @return ETrue if account management uri available.
      */
     virtual TBool HasAccountManagementUri( TInt aSelected ) = 0;
     
    };

#endif // MVCXNSAPPSERVICEPROVIDERIF_H
