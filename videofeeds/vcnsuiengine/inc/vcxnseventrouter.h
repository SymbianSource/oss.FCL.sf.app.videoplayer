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




#ifndef VCXNSEVENTROUTER_H
#define VCXNSEVENTROUTER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mpxmessage2.h>
#include "CIptvUtil.h" 

#include "MIptvVodContentClientObserver.h"
#include "MIptvServiceManagementClientObserver.h"

// FORWARD DECLARATIONS
class CVcxNsUiEngine;
class CVcxNsContent;

// CLASS DECLARATION

/**
 *  CVcxNsEventRouter
 * 
 *  @lib vcxnsuiengine
 */
class CVcxNsEventRouter : public CBase,
                          public MIptvServiceManagementClientObserver,
                          public MIptvVodContentClientObserver
    {
public:

    /**
    * Messages from EPG manager are handled here. Messages are defined in CIptvUtil.h
    * @since            Engine 1.1
    * @param aMsg       Message 
    * @param aInfo      Info 
    * @param aServiceId Service id.
    */
    void HandleEpgManagerMsgL( TInt aMsg, TInt aInfo, TIptvServiceId aServiceId );   

    /**
     * Dummy implementation
     */
    void AddServiceResp( TRespStatus /*aRespStatus*/ ) { ; }
    
    /**
     * Dummy implementation
     */
    void UpdateServiceResp( TRespStatus /*aRespStatus*/ ) { ; }
    
    /**
     * Dummy implementation
     */
    void DeleteServiceResp( TRespStatus /*aRespStatus*/ ) { ; }

    /**
     * Dummy implementation
     */
    void GetServicesResp( TRespStatus /*aRespStatus*/, CDesC8ArraySeg* /*aServicesArray*/ ) { ; }

    /**
     * Dummy implementation
     */
    void GetUsedIapResp( TUint32 ,const TDesC&, CIptvNetworkSelection::TConnectionPermission , TBool, CIptvNetworkSelection::TRespStatus ) { ; }

    /**
     * Dummy implementation
     */
    void ServerShutdownResp( TRespStatus /*aRespStatus*/ ) { ; }

    /**
     * @param aEvent Event
     */
    void HandleSmEvent( CIptvSmEvent& aEvent );

    /**
     * Handles collection messages ui update relevant part
     * @param aMsg Message
     * @param aErr Error code
     */
    void HandleCollectionMessageL( CMPXMessage* aMsg, TInt aErr );
    
    virtual ~CVcxNsEventRouter();

    /**
     * Constructor
     * @param aUiEngine Ui engine
     */
    static CVcxNsEventRouter* NewL( CVcxNsUiEngine& aUiEngine );

    /**
     * Shows or hides the popup about epg update.
     */
    void UpdateInfoPopupL();
    
    /**
     * Epg update failed.
     * Do the required processing.
     * 
     * @param aError Error code
     * @param aServiceId Service id.
     */
    void HandleEpgUpdateFailedL( TInt aError, TIptvServiceId aServiceId ); 

    /**
     * Notify to providers that the download has
     * failed and error note needs to be shown.
     *
     * @param aError Error code.
     * @param aContent Details of the content which download has failed.
     */
    void HandleDlErrorL( TIptvDlError aError, CVcxNsContent& aContent );

private:

    /**
     * Constructor
     * @param aUiEngine Ui engine
     */
    CVcxNsEventRouter( CVcxNsUiEngine& aUiEngine );
    
    /**
     * Update the active view by current application state.
     */
    void UpdateActiveView();
    
    /**
     * Remove service's data.
     *
     * @param aServiceId The service id, that's data is to be removed
     */
    void RemoveServiceData( TUint32 aServiceId );
    
    /**
     * Epg update finished successfully,
     * Do the required processing.
     * 
     * @param aServiceId Service id.
     */
    void HandleEpgUpdateSucceededL( TIptvServiceId aServiceId );
    
    /**
     * Check, is category count changed so that the application 
     * state should be changed, and change it.
     * Note that this removes old service data.
     * 
     * @param aServiceId Service id.
     */
    void CheckAppStateL( TIptvServiceId aServiceId );
    
    /**
     * CVcxNsUiEngine ref.
     */
    CVcxNsUiEngine& iUiEngine;
    
    /**
     * Service that is being updated.
     */
    TUint32 iUpdatedService;
    
    /**
     * Total count of thubnails in the service that is being updated
     */
    TInt iThumbnailCount;
    
    /**
     * Count of thubnails that has already been downloaded in the updated service.
     */
    TInt iThumbnailsDownloaded;

    };

#endif // VCXNSEVENTROUTER_H
