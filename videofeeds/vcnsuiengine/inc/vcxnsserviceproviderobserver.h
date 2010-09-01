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




#ifndef MVCXNSSERVICEPROVIDEROBSERVER_H
#define MVCXNSSERVICEPROVIDEROBSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "CIptvUtil.h"

class CVcxNsService;
class TViaPlayerCustomMessage;

// CLASS DECLARATION

/**
 *  MVcxNsServiceProviderObserver
 * 
 *  @lib vcxnsuiengine.lib
 */
class MVcxNsServiceProviderObserver
    {
    
public:

    /**
     * Notify service has been added.
     *
     * @param aIndex Added item's index
     */
    virtual void ServiceAdded( TInt aIndex ) = 0;

    /**
     * Notify service's data has been updated.
     *
     * @param aIndex Updated item's index
     */
    virtual void ServiceUpdated( TInt aIndex ) = 0;

    /**
     * Notify services data has been removed
     *
     * @param aIndex Removed item's index
     * @param aUpdatedServiceList Updated service list array
     */
    virtual void ServiceRemoved( 
            TInt aIndex, 
            RPointerArray<CVcxNsService>& aUpdatedServiceList ) = 0;
    
    /**
     * Full view data needs to be updated.
     */
    virtual void RefreshView() = 0;
    
    /**
     * Application state needs to be updated.
     */
    virtual void HandleAppStateChangedL() = 0;
    
    /**
     * Open browser with specified url.
     * @param aUri URL
     */    
    virtual void OpenBrowserLinkL( const TDesC& aUri ) = 0;
    
    /**
     * Open streaming link with specified player message.
     * @param aPlayerMsg Player message
     */
    virtual void OpenStreamingLinkL( const TDesC& aUri ) = 0;

    };

#endif // MVCXNSSERVICEPROVIDEROBSERVER_H
