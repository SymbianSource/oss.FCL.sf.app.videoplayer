/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Object to hold service information*
*/





#ifndef CIPTVSERVICE_H
#define CIPTVSERVICE_H

//  INCLUDES
#include "CIptvUtil.h"
#include "TIptvIap.h"
#include "CIptvIapList.h"
#include <s32mem.h>

// CONSTANTS
                                            
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
* CIptvService encapsulates Video Center service information.
* This class is used by CIptvServiceManager, CIptvServiceManagementClient
* and MIptvServiceManagementClientObserver classes. This is just an container,
* database is not modified when this class is edited. Editing is done
* through CIptvServiceManagementClient instance.
*/
class CIptvService : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvService* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvService();

        /**
        * TServiceType.
        */
        enum TServiceType
            {
            /**
            * Video on demand
            */
            EVod = 0,
            
            /**
            * Live TV
            */
            ELiveTv,
            
            /**
            * Vod cast
            */
            EVodCast,
            
            /**
            * Launches browser to a certain url.
            */
            EBrowser,
            
            /**
            * Videos on a remote device.
            */
            EVideoRemote,
              
            /**
            * Launches application (UID defines which one)
            */
            EApplication,
            
            /**
            * DVB-H, launches the mobile tv application.
            */
            EMobileTv,

            /**
            * Launches browser to an url which is
            * constructed by appending dynamic data (country code etc)
            * to address ( received with CIptvService::GetAddress()).
            * Dynamic data is generated with SecureProvisioningUrlParametersL() function.
            */
            ESecureProvisioningServer,
            
            /**
            * Video Store.
            */
            EVideoStore,
            
            /**
            * Service group feed.
            */
            EServiceGroup,
            
            /**
            * Search URL feed.
            */
            ESearch,

            /**
            * Upload URL feed.
            */
            EUpload,

            /**
            * All other service types
            */
            EOther
            
            };
        
        //1 = flag is set, 0 = flag is not set    
        enum TFlags
            {
            /**
            * Is the service selected (=shown) by UI
            */
            ESelected = ( 1 << 0 ),           // 0
            
            /**
            * This affects every field except IAP list, provisioned services are read only.
            */
            EReadOnly = ( 1 << 1 ),           // 2
            
            /**
            * If not set, the service is subservice
            */                                
            EMainService = ( 1 << 2 ),        // 4
            
            /**
            * If set, connection to this service is approved by UI
            */ 
            EConnectionApproved = ( 1 << 3 ), // 8
            
            /**
            * If set, IAPs can't be modified.
            */ 
            EReadOnlyIaps = ( 1 << 4 ),       // 16

            /**
            * If set, belongs to some service group.
            */ 
            EGroupedService = ( 1 << 5 ),     // 32

            /**
            * If set, downloaded group is received.
            */ 
            EGroupReceived = ( 1 << 6 )       // 64
            };
            
    public: // New functions

        /**
         * Sets service unique ID.
         * ID is created by Service Manager when service is added to database for the first time.
         * Client needs to set this only when updating existing service.
         * When adding a service to database this value may be anything.
         * @param aId unique id.
         */
        IMPORT_C void SetId( TUint32 aId );

        /**
         * Gets service unique ID.
         * @return TUint32 service unique ID.
         */
        IMPORT_C TUint32 GetId();
        
        /**
         * Sets the service name, KIptvSmServicesDbNameMaxLength is the max length. 
         * @param aName Name of the service
         * @return      System wide error code.
         */
        IMPORT_C TInt SetName( const TDesC16& aName );

        /**
         * Gets the service name.
         * @return Name of the service.
         */
        IMPORT_C TDes16& GetName();

        /**
        * Sets the service address, KIptvSmServicesDbAddressMaxLength
        * (defined in CIptvUtil.h) is the max length in 16-bit words.
        * @param aAddress The address of the service
        * @return         System wide error code.
        */
        IMPORT_C TInt SetAddress( const TDesC16& aAddress );

        /**
        * Gets the service address url.
        * @return A Reference to service address.
        */
        IMPORT_C TDes16& GetAddress();

        /**
        * Sets path to icon file. KIptvSmServicesDbIconPathMaxLength is the max
        * length (in 16-bit words). 
        * @param aIconPath Path to the icon file
        * @return          System wide error code.
        */
        IMPORT_C TInt SetIconPath( const TDesC16& aIconPath );

        /**
        * Gets path to icon file.
        * @return A Reference to icon path.
        */ 
        IMPORT_C TDes16& GetIconPath();

        /**
        * Sets EPG plugin UID.
        * @param aEpgPluginUid EPG plugin UID
        */
        IMPORT_C void SetEpgPluginUid( TUid aEpgPluginUid );

        /**
        * Gets EPG plugin UID.
        * @return TUid EPG plugin UID.
        */
        IMPORT_C TUid GetEpgPluginUid();

        /**
        * Sets VOD plugin UID.
        * @param aVodPluginUid VOD plugin UID
        */
        IMPORT_C void SetVodPluginUid( TUid aVodPluginUid );

        /**
        * Gets VOD plugin UID.
        * @return TUid
        */
        IMPORT_C TUid GetVodPluginUid();

        /**
        * Sets IPTV plugin UID.
        * @param aIptvPluginUid IPTV plugin UID
        */
        IMPORT_C void SetIptvPluginUid( TUid aIptvPluginUid );

        /**
        * Gets IPTV plugin UID.
        * @return TUint32 IPTV Plugin UID.
        */
        IMPORT_C TUid GetIptvPluginUid();

        /**
        * Sets service type.
        * @param Service type.
        */
        IMPORT_C void SetType( TServiceType aType );

        /**
        * Gets service type.
        * @return TServiceType.
        */
        IMPORT_C TServiceType GetType();
        
        /**
         * Sets Internet access point list.
         * @param aIapList access point list to set.
         * @return System wide error code.
         */
        IMPORT_C TInt SetIapListL( CIptvIapList& aIapList );
        
        /**
         * Gets Internet access point list.
         * @return IAP list
         */
        IMPORT_C CIptvIapList& GetIapList();

        /**
         * Sets user name, KIptvSmPasswordsDbUserNameMaxLength (defined in CIptvUtil.h,
         * in 16 byte words) is the max length.
         * @param aUserName User name, used for authentication.
         * @return          System wide error code.
         */
        IMPORT_C TInt SetUserName( const TDesC16& aUserName );

        /**
         * Gets user name.
         * @return User name
         */
        IMPORT_C TDes16& GetUserName();

        /**
         * Sets password, KIptvSmPasswordsDbPasswordMaxLength is the max length (in 16-bit words,
         * defined in CIptvUtil.h).
         * @param aPassword Password, used for authentication.
         * @return          System wide error code.
         */
        IMPORT_C TInt SetPassword( const TDesC16& aPassword );

        /**
         * Gets password.
         * @param aPassword Password, used for authentication.
         */
        IMPORT_C TDes16& GetPassword();

        /**
         * Sets description of the service.
         * KIptvSmServicesDbDescMaxLength is the max length (in 16-bit words).
         * @param aDesc Description.
         * @return System wide error code.
         */
        IMPORT_C TInt SetDesc( const TDesC16& aDesc );

        /**
         * Gets description of the service.
         * @return Description of the service.
         */
        IMPORT_C TDes16& GetDesc();
        
        /**
         * Sets service flags, see TFlags for bit values.
         * @param aFlags 32 bits of flags
         */    	
        IMPORT_C void SetFlags( TUint32 aFlags );

        /**
         * Gets service flags, see TFlags for bit values.
         * @return Flags.
         */    	
        IMPORT_C TUint32 GetFlags();

        /**
         * Sets application UID value.
         * @param aUid
         */    	
        IMPORT_C void SetApplicationUid( TInt32 aUid );

        /**
         * Gets application UID.
         * @return Application UID.
         */    	
        IMPORT_C TInt32 GetApplicationUid();

        /**
         * Sets display order. Client can use this to store services
         * position on the screen.
         * @param aOrder
         */    	
        IMPORT_C void SetDisplayOrder( TUint32 aOrder );

        /**
         * Gets display order.
         * @return Display order.
         */    	
        IMPORT_C TUint32 GetDisplayOrder();
                
        /**
         * Sets provider ID.
         * KIptvSmServicesDbProviderIdMaxLength is the maximum length.
         * This is unique, if one tries to add a new service to database
         * and there is already a service with the same provider id and the provider id
         * is != empty descriptor, then new record is not created, instead the existing service
         * is updated.
         * @param aProviderId
         * @return System wide error code.
         */
        IMPORT_C TInt SetProviderId( const TDesC& aProviderId );

        /**
         * Gets provider ID.
         * @return provider id
         */
        IMPORT_C TDesC& GetProviderId();

        /**
         * Sets account management url.
         * KIptvSmServicesDbAccountManagementUrlMaxLength is the maximum length.
         * @param aAccountManagementUrl
         * @return System wide error code.
         */
        IMPORT_C TInt SetAccountManagementUrl( const TDesC& aAccountManagementUrl );

        /**
         * Gets account management url.
         * @return account management url.
         */
        IMPORT_C TDesC& GetAccountManagementUrl();

        /**
         * Sets EPG update time.
         * @param aTime time.
         */
        IMPORT_C void SetEpgUpdateTimeL( const TTime& aTime );

        /**
         * Gets time when EPG was last time updated.
         * @return time
         */
        IMPORT_C TTime& GetEpgUpdateTimeL();
        
        /**
         * Sets IAP name. KIptvSmServicesDbIapNameMaxLength is the maximum length.
         * IAP name is set when service is being added by provisioning, since provisioning
         * data can't contain the IAP ID which is generated by the CommsDb and differs from
         * device to device. IAP name is converted to IAP ID in Network Selection when it
         * is used for the first time.
         * @param aIapName IAP name.
         */
        IMPORT_C void SetIapNameL( const TDesC& aIapName );

        /**
         * Gets IAP name.
         * @return IAP name.
         */
        IMPORT_C const TDesC& GetIapNameL();

        /**
         * Sets the search url. KIptvSmServicesDbSearchUrlMaxLength is the maximum length.
         * @param aSearchUrl The search url.
         */
        IMPORT_C void SetSearchUrlL( const TDesC& aSearchUrl );

        /**
         * Sets the record url. KIptvSmServicesDbRecordUrlMaxLength is the maximum length.
         * @param aRecordUrl The record url.
         */
        IMPORT_C void SetRecordUrlL(const TDesC& aRecordUrl);
        
        /**
         * Gets the search url.
         * @return The search url.
         */
        IMPORT_C const TDesC& SearchUrl() const;

        /**
         * Gets the record url.
         * @return The record url.
         */
        IMPORT_C const TDesC& RecordUrl() const;
        
        /**
         * Sets the scheduled download network.
         * @param aScheduleDlNetwork The scheduled download network.
         */
        IMPORT_C void SetScheduleDlNetwork( TUint aScheduleDlNetwork );

        /**
         * Get the scheduled download network.
         * @return The scheduled download network.
         */
        IMPORT_C TUint ScheduleDlNetwork() const;

        /**
         * Sets the scheduled download time.
         * @param aScheduleDlTime The scheduled download time.
         */
        IMPORT_C void SetScheduleDlTime( TUint aScheduleDlTime );

        /**
         * Get the scheduled download time.
         * @return The scheduled download time.
         */
        IMPORT_C TUint ScheduleDlTime() const;

        /**
         * Sets the scheduled download type.
         * @param aScheduleDlNetwork The scheduled download type.
         */
        IMPORT_C void SetScheduleDlType( TUint aScheduleDlType );

        /**
         * Get the scheduled download type.
         * @return The scheduled download type.
         */
        IMPORT_C TUint ScheduleDlType() const;

        /**
         * Sets the last succesfull scheduled download time.
         * @param aScheduleDlNetwork The last succesfull scheduled download time.
         */
        IMPORT_C void SetScheduledLastDownloadTime( const TTime& aTime );

        /**
         * Get the last succesfull scheduled download time.
         * @return The last succesfull scheduled download time.
         */
        IMPORT_C const TTime& GetScheduledLastDownloadTime() const;

        /**
        * Sets service group ID.
        * @param aId a group id.
        */
        IMPORT_C void SetGroupId( TUint32 aGroupId );

       /**
        * Gets service group ID.
        * @return TUint32 service group ID.
        */
        IMPORT_C TUint32 GetGroupId();
       
        /**
         * Sets use category flag.
         * @param aUseCategory use category state.
         */
        IMPORT_C void SetUseCategory( const TBool& aUseCategory );

        /**
         * Gets use category flag.
         * @return use category state.
         */
        IMPORT_C TBool GetUseCategory();

        /**
         * Sets the upload provider. KIptvSmServicesDbNameMaxLength is the maximum length.
         * @param aProvider The upload provider.
         * @return          System wide error code.
         */
        IMPORT_C TInt SetUploadProvider( const TDesC& aProvider );

        /**
         * Gets the upload url.
         * @return The upload provider.
         */
        IMPORT_C const TDesC& GetUploadProvider() const;

        /**
        * Sets the icon address, KIptvSmServicesDbAddressMaxLength
        * (defined in CIptvUtil.h) is the max length in 16-bit words.
        * @param aAddress The address of the icon
        * @return         System wide error code.
        */
        IMPORT_C TInt SetIconUrl( const TDesC16& aAddress );

        /**
        * Gets the icon address url.
        * @return A Reference to icon address.
        */
        IMPORT_C TDes16& GetIconUrl();

        /**
         * Sets the service.\n
         * Sets all service parameters from a single binary descriptor.\n
         * This format is used when sending service data over IPC.\n
         * This descriptor can be retrieved with GetL method.\n
         * SetL uses the same binary format as InternalizeL.\n
         * \n
         * @param aService Service data as a binary descriptor.
         * @return         System wide error code.
         */
        IMPORT_C TInt SetL( const TDesC8& aService );
        
        /**
         * Gets the service data as a binary descriptor.\n
         * This format is used when sending service data over IPC or\n
         * when handling arrays of services.\n
         * User is responsible for freeing the HBufC8*.\n
         * GetL uses the same binary format as ExternalizeL.\n
         * \n
         * @return Service data as a binary data descriptor.
         */
        IMPORT_C HBufC8* GetL();

        /**
         * Calculates binary descriptor size with current member values.
         * @return size.
         */
        IMPORT_C TUint32 CountSize();

        /**
         * Read data stream and construct this class from it.
         * @param aStream Stream to read data from.
         */
        IMPORT_C void InternalizeL( RDesReadStream& aStream );


        /**
         * Write internal data to binary format which can be sent over IPC.
         * @param aStream The Stream where data is written.
         */
        IMPORT_C void ExternalizeL( RDesWriteStream& aStream ) const;

        /**
         * Counts size of the externalize result. User can call this to find out
         * how much data will be received from ExternalizeL.
         */
        IMPORT_C TUint32 CountExternalizeSize();

        /**
         * Compares this object to aService object.
         * @param aService Object to be compared with this object.
         * @return         ETrue if services are identical, EFalse otherwise.
         */
        IMPORT_C TBool CompareL( CIptvService& aService );

        /**
        * CIptvService::ESecureProvisioningServer type service needs additional dynamic
        * parameters after the address url (CIptvService::GetAddress()), this function
        * generates the dynamic part.
        *
        * @return Pointer to dynamic part of the url, caller gets ownership of the pointer.
        */
        IMPORT_C static HBufC* SecureProvisioningUrlParametersL();

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    public:
    
        /**
         * Internet access point list
         */
		CIptvIapList* iIapList;
    
    private: // Data
        
        /*
         * Unique Service ID.
         */ 
        TUint32 iId;
        
        /*
         * Name of the service.
         */
        TBuf16<KIptvSmServicesDbNameMaxLength> iName;
        
        /*
         * Address of the service.
         */
        TBuf16<KIptvSmServicesDbAddressMaxLength> iAddress;
        
        /*
         * Path to icon file.
         */
        TBuf16<KIptvSmServicesDbIconPathMaxLength> iIconPath;
        
        /*
         * Epg plugin Uid.
         */
        TInt32 iEpgPluginUid;
        
        /*
         * Vod plugin Uid.
         */
        TInt32 iVodPluginUid;

        /*
         * Iptv plugin Uid.
         */
        TInt32 iIptvPluginUid;
        
        /* 
         * Service type.
         */
        TServiceType iType;
		    
	    /*
	     * User name.
	     */
	    TBuf16<KIptvSmPasswordsDbUserNameMaxLength> iUserName;
	
	    /*
	     * Password.
	     */
	    TBuf16<KIptvSmPasswordsDbPasswordMaxLength> iPassword;
			
	    /*
	     * Description of the service.
	     */
	    TBuf16<KIptvSmServicesDbDescMaxLength> iDesc;

        /*
         * See CIptvService::TFlags.
         */
        TUint32 iFlags;
    
        /*
         * Used to identify which application is launched for this service.
         */
        TInt32  iApplicationUid;
    
        /*
         * Defines the order in which the service is shown on UI.
         */
        TUint32 iDisplayOrder;
                
        /*
         * Provider ID.
         */
        TBuf<KIptvSmServicesDbProviderIdMaxLength> iProviderId;

        /*
         * Account management URL.
         */
        TBuf<KIptvSmServicesDbAccountManagementUrlMaxLength> iAccountManagementUrl;
        
        /*
         * Last time when epg was updated for this service.
         */
        TTime iEpgUpdateTime;
        
        /*
         * Iap name, this set by provisioning since it does not have knowledge of iap ids.
         */
        TBuf<KIptvSmServicesDbIapNameMaxLength> iIapName;
    
        /**
         * The search url for retrieving search based .rss.
         */
        HBufC* iSearchUrl;
        
        /**
         * The record url.  
         */
         HBufC* iRecordUrl;
         
        /**
         * The scheduled download network setting.
         */
        TUint iScheduleDownloadNetwork;

        /**
         * The scheduled download time setting.
         */
        TUint iScheduleDownloadTime;

        /**
         * The scheduled download type setting.
         */
        TUint iScheduleDownloadType;

        /*
         * The last successfull scheduled download time.
         */
        TTime iLastScheduledDownloadTime;

        /**
         * Service group ID.
         */
        TUint32 iGroupId;

        /**
         * Categories in use flag.
         */
        TUint8 iUseCategory;

        /**
         * The upload url for retrieving upload based .rss.
         */
        TBuf16<KIptvSmServicesDbNameMaxLength> iUploadProvider;
          
        /*
         * Address to icon url.
         */
        TBuf16<KIptvSmServicesDbAddressMaxLength> iIconUrl;

    };

#endif      // CIPTVSERVICE_H   
            
// End of File
