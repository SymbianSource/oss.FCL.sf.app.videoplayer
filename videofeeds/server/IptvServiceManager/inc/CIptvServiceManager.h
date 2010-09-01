/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    header file for CIptvServiceManager class*
*/





#ifndef CIPTVSERVICEMANAGER_H
#define CIPTVSERVICEMANAGER_H

//  INCLUDES
#include <e32std.h>
#include <badesca.h>    // CDesCArrayFlat (cannot be forward declarated)
#include <d32dbms.h>    // RDbStoreDatabase
#include <f32file.h>    // RFs
#include "CIptvUtil.h"

#include "CIptvSmEvent.h"
#include "CIptvServiceManagementClient.h"

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
class CActiveSpaceHelper;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

// CONSTANTS

const TInt KIptvSmCustomSqlMaxLength = 256;
const TUint8 KIptvSmServiceDbCompactingFrequency = 3;
const TInt KIptvSmMaxServices = 500;

_LIT( KIptvSmCDiskPrefix, "c:");
_LIT( KIptvSmZDiskPrefix, "z:");

_LIT( KIptvSmServicesDbFileName, "services.dat");
_LIT( KIptvSmPasswordsDbFileName, "pwd.dat");
_LIT( KIptvSmServicesXmlFile, "services.xml");
_LIT( KIptvSmServicesUpdateXmlFile, "updateservices.xml");
_LIT( KSmDbVersionFileName, "SmDbVersion.dat");

_LIT( KIptvSmServicesTable, "Services");

_LIT( KIptvSmServicesDbIdCol, "ID" );                                     /* 1st column     */
_LIT( KIptvSmServicesDbNameCol, "Name" );                                 /* 2nd column     */
_LIT( KIptvSmServicesDbAddressCol, "Address" );                           /* 3rd column     */
_LIT( KIptvSmServicesDbIconPathCol, "IconPath" );                         /* 4th column     */
_LIT( KIptvSmServicesDbEpgPluginUidCol, "EpgPluginUid" );                 /* 5th column     */
_LIT( KIptvSmServicesDbVodPluginUidCol, "VodEpgPluginUid" );              /* 6th column     */
_LIT( KIptvSmServicesDbIptvPluginUidCol, "IptvEpgPluginUid" );            /* 7th column     */
_LIT( KIptvSmServicesDbTypeCol, "Type" );                                 /* 8th column     */
_LIT( KIptvSmServicesDbIapListCol, "Iap" );                               /* 9th column     */
_LIT( KIptvSmServicesDbDescCol, "Desc" );                                 /* 10th column    */
_LIT( KIptvSmServicesDbDateCol, "Date" );                                 /* 11th column    */
_LIT( KIptvSmServicesDbFlagsCol, "Flags" );                               /* 12th column    */
_LIT( KIptvSmServicesDbApplicationUidCol, "ApplicationUid" );             /* 13th column    */
_LIT( KIptvSmServicesDbDisplayOrderCol, "DisplayOrder" );                 /* 14th column    */
_LIT( KIptvSmServicesDbProviderIdCol, "ProviderId" );                     /* 15th column    */
_LIT( KIptvSmServicesDbAccountManagementUrlCol, "AccountManagementUrl" ); /* 16th column    */
_LIT( KIptvSmServicesDbEpgUpdateTimeCol, "EpgUpdateTime" );               /* 17th column    */ //added in db version 2
_LIT( KIptvSmServicesDbIapNameCol, "IapName" );                           /* 18th column    */ //added in db version 2
_LIT( KIptvSmServicesDbSearchUrlCol, "SearchUrl" );                       /* 19th column    */ //added in db version 2
_LIT( KIptvSmServicesDbScheduledDlNetworkCol, "ScheduledDlNetwork" );     /* 20th column    */ //added in db version 2
_LIT( KIptvSmServicesDbScheduledDlTimeCol, "ScheduledDlTime" );           /* 21th column    */ //added in db version 2
_LIT( KIptvSmServicesDbScheduledDlTypeCol, "ScheduledDlType" );           /* 22th column    */ //added in db version 2
_LIT( KIptvSmServicesDbLastScheduledDlTime, "LastScheduledDlTime" );      /* 23th column    */ //added in db version 2
_LIT( KIptvSmServicesDbGroupIdCol, "GroupID" );                           /* 24th column    */ //added in db version 3
_LIT( KIptvSmServicesDbUseCategoryCol, "UseCatecory" );                   /* 25th column    */ //added in db version 3
_LIT( KIptvSmServicesDbUploadProviderCol, "UploadProvider" );             /* 26th column    */ //added in db version 3
_LIT( KIptvSmServicesDbIconUrlCol, "IconUrl" );                           /* 27th column    */ //added in db version 3
_LIT( KIptvSmServicesDbRecordUrlCol, "RecordUrl" );                       /* 28th column    */ //added in db version 3

_LIT(KIptvSmServicesDbIndexName,"ServicesIndex");  /* Name of the ID index        */

_LIT(KIptvSmPasswordsTable, "Passwords");
_LIT(KIptvSmPasswordsDbIdCol, "ID");             /* 1st column     */
_LIT(KIptvSmPasswordsDbUserNameCol, "UserName"); /* 2nd column     */
_LIT(KIptvSmPasswordsDbPasswordCol, "Password"); /* 3rd column     */

_LIT(KIptvSmPasswordsDbIndexName,"PasswordsIndex");  /* Name of the ID index */

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFileStore;
class CIptvService;
class MIptvServiceManagerObserver;
class TIptvSmPassword;

// CLASS DECLARATION

/**
 * Class:       CIptvServiceManager
 *
 * Description: An instance of class CIptvServiceManager provides services
 *              database access. 
 *
 * Database definitions:
 *
 *              The Services database (services.dat) contains Services table:
 *
 *              Table name: Services\n
 *
 *                 Column:                     Type:          Length (bytes):                                In versions: \n
 *                 ------                      -----          ---------------                                ------------ \n
 *                 ID                          TUint32        4                                              vers 1, 2    \n
 *                 Name                        EDbColText     KIptvSmServicesDbNameMaxLength*2               vers 1, 2    \n     
 *                 Address                     EDbColText     KIptvSmServicesDbAddressMaxLength*2            vers 1, 2    \n
 *                 IconPath                    EDbColText     KIptvSmServicesDbIconPathMaxLength*2           vers 1, 2    \n
 *                 EpgPluginUid                TInt32         4                                              vers 1, 2    \n
 *                 VodPluginUid                TInt32         4                                              vers 1, 2    \n
 *                 IptvPluginUid               TInt32         4                                              vers 1, 2    \n
 *                 Type                        TUint8         1                                              vers 1, 2    \n
 *                 IAP List                    EDbColText8    KIptvSmServicesDbIapMaxLength                  vers 1, 2    \n
 *                 User name                   EDbColText     KIptvSmServicesDbPasswordMaxLength*2           vers 1, 2    \n
 *                 Password                    EDbColText     KIptvSmServicesDbPasswordMaxLength*2           vers 1, 2    \n
 *                 Desc                        EDbColText     KIptvSmServicesDbDescMaxLength*2               vers 1, 2    \n
 *                 Date                        TTime (TInt64) 8                                              vers 1, 2    \n
 *                 Flags                       TUint32        4                                              vers 1, 2    \n
 *                 ApplicationUid              TInt32         4                                              vers 1, 2    \n
 *                 DisplayOrder                TUint32        4                                              vers 1, 2    \n
 *                 ProviderId                  EDbColText     KIptvSmServicesDbProviderIdMaxLength           vers 1, 2    \n
 *                 AccountManagementUrl        EDbColText     KIptvSmServicesDbAccountManagementUrlMaxLength vers 1, 2    \n
 *                 EpgUpdateTime               TTime (TInt64) 8                                              vers 2       \n
 *                 IapName                     EDbColText     KIptvSmServicesDbIapNameMaxLength              vers 2       \n
 *                 SearchUrl                   EDbColText     KIptvSmServicesDbAddressMaxLength*2            vers 2       \n
 *                 ScheduledDlNetwork          TUint32        4                                              vers 2       \n
 *                 ScheduledDlTime             TUint32        4                                              vers 2       \n
 *                 ScheduledDlType             TUint32        4                                              vers 2       \n
 *                 LastScheduledDlTime         TTime (TInt64) 8                                              vers 2       \n
 *                 RecordUrl                   EDbColText     KIptvSmServicesDbAddressMaxLength*2            vers 3       \n
 *       
 *              There is also index for Services table with name "ServicesIndex". It's\n
 *              constructed from ID column.\n
 *
 *             The Passwords database (pwd.dat) contains Passwords table:\n
 *
 *              Table name: Passwords\n
 *
 *                 Column:              Type:          Length (bytes):                    Desc:  \n
 *                 ------               -----          -----------                        -----  \n
 *                 ID                   TUint32        4                                         \n
 *                 User name            EDbColText     KIptvSmPasswordsDbPasswordMaxLength*2     \n     
 *                 Password             EDbColText     KIptvSmPasswordsDbPasswordMaxLength*2     \n
 *
 */
class CIptvServiceManager : public CBase
    {
    public:
    
        /**
         * These flags are used in AddOrUpdateServiceL() -method in aModeFlags argument. 
         */
        enum TAddServiceModeFlags
            {
            /**
            * If set, the mode is add, if not set the mode is update
            */
            EAddService = (1 << 0),
            
            /**
            * Used when doing nested calls, used by CIptvServiceManager itself
            */
            EDontOpen   = (1 << 1)
            };
    
    public:  // Constructors and destructor

        /**
         * NewL
         * Get instance of a CIptvServiceManager object.
         */
        static CIptvServiceManager* NewL();

        /**
         * ~CIptvServiceManager
         * Destroy the object.
         */
        ~CIptvServiceManager();
        
    public: // Public API for database operations

        /**
         * Removes Services and Passwords databases. Closes any open database,
         * before deleting files.
         */
        void RemoveDbL();

        /**
         * Retrieve services from databases.
         *
         * @param  aSeachLimitFlags See flags from CIptvServiceManagementClient::TSearchLimitFlag type.
         *                          If aSeachLimitFlags is 0, all services are returned. If aSeachLimitFlags != 0, then
         *                          only services which have the corresponding feature matching are returned.
         * @param  aOrder           The order in which the services are returned.
         * @param  aVersion         database version. If 0, then the latest possible version is used.
         * @return CIptvServices*   Pointer to class containing services found. 
         *                          User is responsible for freeing the returned pointer.
         */
        CIptvServices *GetServicesL(TUint32 aSearchLimitFlags,
                                    CIptvServiceManagementClient::TOrder aOrder,
                                    TInt aVersion);

        /**
         * Gets services which IDs are on the range of aStartId and aEndId.\n
         * Result might have gaps, ie every ID value might not contain service.\n
         * This method is not meant for searching service records, instead one should \n
         * use this when the service IDs are already known.\n
         *
         * @param  aStartId     The first ID to fetch.
         * @param  aEndId       The last ID to fetch.
         * @param  aDoOpenClose If ETrue, then the database is tried to open, and is closed at the end.
         *                      If EFalse, then the database is not opened at start and not closed at end.
         *                      This param can be used to remove unnecessary open/closes. 
         * @return              Pointer to class containing services.
         *                      User is responsible of freeing the returned pointer.
         */
        CIptvServices* GetServicesUsingIdL(TUint32 aStartId,
                                           TUint32 aEndId,
                                           TBool aDoOpenClose);

        /**
         * Gets service using ID.
         *
         * @param  aServiceId   Service ID in Services database.
         * @param  aDoOpenClose If ETrue, then the database is tried to open, and is closed at the end.
         *                      If EFalse, then the database is not opened at start and closed at end.
         *                      This param can be used to remove unnecessary open/closes. 
         * @return CIptvService* on success, NULL on fail.
         */
        CIptvService* GetServiceL(TUint32 aServiceId, TBool aDoOpenClose);

        /**
         * Gets services using provider ID.
         *
         * @param  aProviderId  Providerd ID in Services database.
         * @param  aDoOpenClose If ETrue, then the database is tried to open, and is closed at the end.
         *                      If EFalse, then the database is not opened at start and closed at end.
         *                      This param can be used to remove unnecessary open/closes. 
         * @return              CIptvServices* on success, NULL on fail.
         */
        CIptvServices* GetServicesL(const TDesC& aProviderId, TBool aDoOpenClose);

        /**
         * Deletes service from databases (Services DB and Passwords DB).
         *
         * @param aId The ID of the service to be deleted.
         * @return    KErrNone or one of the system-wide error codes.
         *            Returns KErrNone also if service with aId is not found.
         */
        TInt DeleteServiceL(TUint32 aId);

        /**
         * Registers observer. Ownership does not transfer.
         * Observers must inherit MIptvServiceManagerObserver
         * and implement virtual functions.
         *
         * @param aObserver Observer which will receive the events.
         * @return          System-wide error code.
         */
        TInt RegisterObserver(MIptvServiceManagerObserver* aObserver);

        /**
         * DeRegisters observer. Ownership does not transfer.
         *
         * @param aObserver Observer to be removed from observers list.
         */
        void DeRegisterObserver(MIptvServiceManagerObserver* aObserver);

        /**
         * Add service to database, if there is already a service with same
         * aService.iProviderId, then the existing service is updated with new values
         * and the new record is not created.
         *
         * @param aService     A Service to be added to database.
         * @param aDoOpenClose Used to optimize unnecessarry open/close calls.
         *                     Normal user uses ETrue always.
         */        
        void AddServiceL(CIptvService& aService, TBool aDoOpenClose);

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
        /**
         * This method is used to update Video Store service to
         * active space database.
         *
         * @param aServive Service info.
         */
        void UpdateVideoStoreSpaceDbL( CIptvService& aServive );
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

        /**
         * Update existing service in database. aService.iId is used as an identifier.
         *
         * @param aService New values to existing service in database.
         */        
        void UpdateServiceL(CIptvService& aService);

        /**
         * Closes Services and Passwords databases opened with OpenL.
         * Public since cleanupstack calls this.
         * It is safe to call this even if databases are already closed.
         */
        void Close();
            
    private:

        /**
         * Adds a new service to Services database or updates existing one.
         *
         * @param aService service to add, or update
         * @param aMode see values from TAddServiceModeFlags.
         */
        void AddOrUpdateServiceL(CIptvService& aService, TUint32 aModeFlags);

        /**
         * Creates index for Services DB. ID column is used as key.
         */
        void CreateServicesDbIndexL();

        /**
         * Creates index for Passwords DB. ID column is used as key.
         */        
        void CreatePasswordsDbIndexL();

        /**
         * Opens and creates databases if not done already. Can be called
         * even if already opened and created.
         */
        void OpenL();

        /**
         * Creates files if they are missing, executes provisioning if necessarry.
         *
         * @param aDoProvisioning If ETrue, then provisioning is done, if EFalse then it is
         *                        not performed.
         */
        void CreateFilesL(TBool aDoProvisioning);

        /**
         * Opens Services database.
         */
        void OpenServicesDbL();

        /**
         * Opens Passwords database.
         */
        void OpenPasswordsDbL();

        /**
         * Creates and opens a new Services database. Creates a
         * database file, table structure and an index constructed from ID column.
         * The database will be open in exclusive (non-shareable) mode. The
         * database must be closed, when not used any more. If the
         * database exists, it is replaced.
         * Leaves if the file cannot be created or database initialized.
         * Leaves with system wide error codes.
         */
        void CreateServicesDbL();

        /**
         * Creates and opens a new Passwords database. Creates a
         * database file and table structure. The database
         * will be open in exclusive (non-shareable) mode. The
         * database must be closed, when not used any more. If the
         * database exists, it is replaced.
         * Leaves if the file cannot be created or database initialized.
         * Leaves with system wide error codes.
         */
        void CreatePasswordsDbL();

        /**
         * Perform the first phase of two phase construction.
         */
        CIptvServiceManager();

        /**
         * Perform the second phase construction of a CIptvServiceManager
         * object
         */
        void ConstructL();

        /**
         * Sends event to all observers.
         *
         * @param aEvent     Event to send.
         * @param aServiceId Service which caused the event.
         * @param aService   Service data.
         */
        void InformObserversL( CIptvSmEvent::TEvent aEvent,
                               TUint32 aServiceId,
                               CIptvService* aService);

        /**
         * Get User name and password from Passwords DB.
         *
         * @param aId id of the service
         * @param aPassword username & password which will be filled from database.
         */      
        void GetPasswordL( TUint32 aId, TIptvSmPassword& aPassword );

    private:

        void CreateServicesTableL();
        void CreatePasswordsTableL();
        void CreateServicesIndexL();
        TBool IsServiceIncludedInSearch(TUint32 aSearchLimitFlags,
                                        CIptvService& aIptvService) const;
        TInt ExecuteSqlCmd(const TDesC& aSqlCmd, RDbStoreDatabase& aDb);
        void ExecuteProvisioningL();
        TBool IsServiceUpdateFoundL(TFileName& aServicesXmlFile);
        void DoProvisioningL(const TDesC& aServicesXmlFile);
        void CreateWritableRootDirL(TDriveNumber aDrive);

        /**
         * Reads SM DB version from SmDbVersion.dat. If version is found and it differs from
         * SW version, then services.dat and pwd.dat files are deleted. ConvertDatabaseL is run
         * if DB version is 1 and SW version is 2.
         */
        void CheckSmDbVersionL( void );
        
        /**
         * Saves sw version to SmDbVersion.dat.
         */ 
        void SaveSmDbVersion( void );
        
        /**
         * Converts SM DB to same version as SW.
         * Currently supports 1 -> 2 conversion.
         * @param aOldVersion Current DB version on the disk
         */         
        void ConvertDatabaseL( TInt aOldVersion );

        void CompactDatabases();

        /**
         * Returns highest DisplayOrder value from services DB
         * 
         * @return highest DisplayOrder value from services DB or zero.
         */
        TUint32 MaxDisplayOrderL();

    private:    // Data

        RArray<MIptvServiceManagerObserver*> iObservers; 

        /**
        * For use of iServicesFileStore / iPasswordsFileStore
        */
        RFs iFsSession;
    
        /**
        * For database operations
        */
        RDbStoreDatabase iServicesDb;
        RDbStoreDatabase iPasswordsDb;
    
        /**
        * For creating and opening services database file
        */
        CFileStore* iServicesFileStore;

        /**
        * For creating and opening passwords database file
        */
        CFileStore* iPasswordsFileStore;

        /**
        * Flag indicating iServicesDb open status
        */
        TBool iServicesDbOpen;

        /**
        * Flag indicating iPasswordsDb open status
        */
        TBool iPasswordsDbOpen;
    
        /**
        * Services database filename
        */
        TFileName iServicesFile;

        /**
        * Passwords database filename
        */
        TFileName iPasswordsFile;

        /**
        * The first and every 3rd delete/update/add operation causes database compacting
        */
        TUint8 iCompactCounter;
        
        /**
        * Private path
        */
        TFileName iPrivatePath;

        /**
        * Used to store search url temporarily in db operations.
        */
        HBufC* iSearchUrl;
        
        /**
        * Used to store record url temporarily in db operations.
        */
        HBufC* iRecordUrl;

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)	
        /**
         * ActiveSpace Helper class.
         * Own.
         */
		CActiveSpaceHelper* iActiveSpaceHelper;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    };

#endif // CIPTVSERVICEMANAGER_H   
