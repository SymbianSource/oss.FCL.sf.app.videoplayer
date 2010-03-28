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





#ifndef VCXNSSERVICESETTINGS_H
#define VCXNSSERVICESETTINGS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "CIptvNetworkSelection.h"
#include "CIptvService.h"
#include "MIptvServiceManagementClientObserver.h"
#include "CIptvIapList.h"
#include "iptvvodscheduleddownloaddefinitions.h"

// FORWARD DECLARATIONS
class CIptvServiceManagementClient;
class CDesC8ArraySeg;

// CLASS DECLARATION

/**
 *  CVcxNsServiceSettings
 * 
 */
class CVcxNsServiceSettings : public CBase, public MIptvServiceManagementClientObserver
    {
public:

// Helper class

    class TVodcastService
        {
        public:
        
        TUint32                                  iId;
        TBuf<KIptvSmServicesDbNameMaxLength>     iName;
        TTime                                    iUpdateTime;
        TBuf<KIptvSmServicesDbIconPathMaxLength> iIconPath;
        TUint32                                  iDisplayOrder;
        };

// Constructors and destructor

    virtual ~CVcxNsServiceSettings( );
    
    IMPORT_C static CVcxNsServiceSettings* NewL( );
    
// New functions
       
    /**
     * Returns all services.
     *
     * @return Array of services if successful, NULL otherwise.
     */
    IMPORT_C CDesC8ArraySeg* GetAllServicesL();
    
    /**
     * Returns all selected services.
     *
     * @param aError On return, error code from server.
     * @return Array of services if successfull, NULL otherwise.
     */
    IMPORT_C CDesC8ArraySeg* GetAllSelectedServicesL( TInt& aError );
    
    
    /**
     * Returns all services that match given type.
     *
     * @param aServiceType Type of requested services as CIptvService::TServiceType enumeration.
     * @return Array of services if successfull, NULL otherwise.
     */
    IMPORT_C CDesC8ArraySeg* GetServicesByTypeL( CIptvService::TServiceType aServiceType );
    /**
     * Gets IAP list for new service. List is created from existing services.
     *
     * @param aIapList On return, IAP list for new service.
     * @return KErrNone or one of the system wide error codes.
     */
    IMPORT_C TInt GetIapListForNewServiceL( CIptvIapList& aIapList );
    
    /**
      * Adds a new service to Service DB.
      *
      * @param aServiceName Name for service.
      * @param aAddress     Address of service.
      * @param aDescription Description of service.
      * @param aIapList     IAP list for service.
      * @param aServiceType Type of service as CIptvService::TServiceType enumeration.
      * @param aEpgPluginUid   Service EPG plugin Uid.
      * @param aIptvPluginUid  Service IPTV plugin Uid.
      * @param aVodPluginUid   Service VOD plugin Uid.
      * @return KErrNone or one of the system wide error codes.
      */
     IMPORT_C TInt AddServiceL( const TDesC& aServiceName,
                                const TDesC& aAddress,
                                const TDesC& aDescription,
                                CIptvIapList& aIapList,
                                CIptvService::TServiceType aServiceType,
                                const TUid& aEpgPluginUid,
                                const TUid& aIptvPluginUid,
                                const TUid& aVodPluginUid  );
    
     /**
      * Updates existing service.
      *
      * @param aIptvService New service information. Includes ID to identify
      *                     which service to update.
      */         
     IMPORT_C void UpdateServiceL( CIptvService& aIptvService );
    
     /**
      * Deletes a service.
      *
      * @param aItemId Service ID of service to delete.
      */
     IMPORT_C void DeleteServiceL( TUint32 aItemId );
    
     /**
      * Returns a single service based on service Id.
      * Note: Method is not efficient, so use only if absolutely necessary.         
      *
      * @param aServiceId   Service ID of service to get.
      * @param aIptvService On return, the requested service.
      * @return KErrNone or one of the system wide error codes.
      */
    IMPORT_C TInt GetServiceL( TUint32 aServiceId, CIptvService& aIptvService );
    
    // Schedule settings stuff
    
    /**
    * Get the scheduled download settings.
    * 
    * @param aServiceId The service id.
    * @param aDownloadTime The download time.
    * @param aCondition The download condition.
    * @param aType The download type.
    * @return KErrNone if successful,
    *           otherwise one of the other system-wide error codes.
    */
    IMPORT_C TInt GetScheduledDownloadSettingsL(
        TIptvServiceId aServiceId,
        TIptvVodScheduleDownloadtimeCombination& aDownloadTime,
        TIptvVodScheduleConnectionCondition& aCondition,
        TIptvVodScheduleDownloadTypeCombination& aType );

    /**
    * Set the scheduled download settings and reschedule.
    * 
    * @param aServiceId The service id.
    * @param aDownloadTime The download time.
    * @param aCondition The download condition.
    * @param aType The download type.
    * @return KErrNone if successful,
    *           otherwise one of the other system-wide error codes.
    */
    IMPORT_C TInt SetScheduledDownloadSettingsL(
        TIptvServiceId aServiceId,
        TIptvVodScheduleDownloadtimeCombination aDownloadTime,
        TIptvVodScheduleConnectionCondition aCondition,
        TIptvVodScheduleDownloadTypeCombination aType );
    
// From MIptvServiceManagementClientObserver

    /**
     * Not used
     */
    void AddServiceResp( TRespStatus ) { ; }

    /**
     * Not used
     */
    void UpdateServiceResp( TRespStatus  ) { ; }

    /**
     * Not used
     */
    void DeleteServiceResp( TRespStatus ) { ; }

    /**
     * Not used
     */
    void GetServicesResp( TRespStatus, CDesC8ArraySeg* ) { ; }

    /**
     * Not used
     */
    void GetUsedIapResp( TUint32,
                         const TDesC&,
                         CIptvNetworkSelection::TConnectionPermission,
                         TBool,
                         CIptvNetworkSelection::TRespStatus ){ ; }

    /**
     * Not used
     */
    void ServerShutdownResp( TRespStatus ) { ; }

    /**
     * Not used
     */
    void HandleSmEvent( CIptvSmEvent& ) { ; }
    
private:
    
    CVcxNsServiceSettings( );  

    /**
     * GetServiceManagementClientL
     */    
    CIptvServiceManagementClient* GetServiceManagementClientL();

 // Data

    /**
     * CIptvServiceManagementClient
     */ 
    CIptvServiceManagementClient* iIptvServiceManagementClient;

    };

#endif // VCXNSSERVICESETTINGS_H
