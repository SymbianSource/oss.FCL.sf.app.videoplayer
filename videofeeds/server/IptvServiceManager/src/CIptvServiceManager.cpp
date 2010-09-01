/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Offers interface to services database*
*/




#include <s32file.h>    // CFileStore & CPermanentFileStore
#include <bautils.h>    // file helpers
#include <eikenv.h>
#include <sysutil.h>    // SysUtil
#include "IptvDebug.h"
#include <f32file.h>
#include <sqldb.h>
#include "CIptvDriveMonitor.h"

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
#include "CIptvUiEngine.h"
#include <aspaclienthelper.h>
#include "iptvlocalisationliterals.h"
#include <ipvideo/IptvActivationMessage.h>
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

#include "CIptvService.h"
#include "CIptvServices.h"
#include "CIptvServiceManagementClient.h"
#include "CIptvProvisioningProcessor.h"
#include "CIptvServer.h"
#include "IptvServer.pan"
#include "MIptvServiceManagerObserver.h"
#include "TIptvSmPassword.h"
#include "CIptvServiceManager.h"

const TInt KIptvServiceManagerFreeSpace = 512000; // 500 KB

//all valid versions are > 0
const TUint32 KIptvSmDbVersion = 3; //1 = Rel1, 2 = Rel2, Rel3, Rel4, 3 = Rel6/NFP1/Rel4 Touch

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
 const TUid KIptvMain = {0x102750E2};
 const TInt KIptvMaxLengthOfEmptyThumbPath( 1 );
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvServiceManager::CIptvServiceManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvServiceManager::CIptvServiceManager()
    {
   	iServicesDbOpen  = EFalse;
   	iPasswordsDbOpen = EFalse;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManager::ConstructL
// Second phase construction. Leaves, if RFs session cannot be created.
// -----------------------------------------------------------------------------
//
void CIptvServiceManager::ConstructL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::ConstructL() begin");	

    iObservers.Reset();
	
    User::LeaveIfError( iFsSession.Connect());

    CIptvDriveMonitor* driveMonitor = CIptvDriveMonitor::NewL( iFsSession );
    CleanupStack::PushL( driveMonitor ); // 1->
    
    for ( TInt i = 0; i < driveMonitor->iAvailableDrives.Count(); i++ )
        {
        CreateWritableRootDirL( static_cast<TDriveNumber>( driveMonitor->iAvailableDrives[i].iDrive ) );
        }

    CleanupStack::PopAndDestroy( driveMonitor ); // <-1
            
    // Services database to private folder, create private folder if it does not exist yet
    iPrivatePath.Zero();
    iServicesFile  = KIptvSmCDiskPrefix;
    iPasswordsFile = KIptvSmCDiskPrefix;
		
    // Get private path
    if ( iFsSession.PrivatePath( iPrivatePath ) )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::ConstructL() can't get private path");	
        User::Panic(KIptvServiceManager, KErrPathNotFound);
        }
	
    // Append privatepath after drive letter
    iServicesFile.Append( iPrivatePath );
    iPasswordsFile.Append( iPrivatePath );
	
    if ( BaflUtils::CheckFolder(iFsSession, iServicesFile) != KErrNone )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::ConstructL() Creating private path ");	
		
        if ( iFsSession.CreatePrivatePath( EDriveC ) != KErrNone )
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::ConstructL() can't create private path ");	
            // This is fatal, owner process is always able to create its private folder
            User::Panic( KIptvServiceManager, KErrDiskFull );
            }
			
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::ConstructL() Private path created");
        }

    iServicesFile.Append( KIptvSmServicesDbFileName );
    iPasswordsFile.Append( KIptvSmPasswordsDbFileName );
        
    CheckSmDbVersionL();
    OpenL();

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
 	TRAPD( err, iActiveSpaceHelper = CActiveSpaceHelper::NewL() );
 	if ( err == KErrNotFound )
 	    {
 	    iActiveSpaceHelper = NULL;      // Active Space not found
 	    }
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    //  Check if there is service update file and if found, do provisioning
    TFileName serviceUpdateXml;
    if (IsServiceUpdateFoundL(serviceUpdateXml))
        {
        ExecuteProvisioningL();
        }

    CompactDatabases();
    Close();
        
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::ConstructL() end");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvServiceManager* CIptvServiceManager::NewL()
    {
    CIptvServiceManager* self = new( ELeave ) CIptvServiceManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CIptvServiceManager::~CIptvServiceManager()
    {
    Close();  // Just in case, if the user does not close this explicitely
    iFsSession.Close();
    iObservers.Close();
    delete iSearchUrl;
    
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    delete iActiveSpaceHelper;
    REComSession::FinalClose();
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::OpenL()
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::OpenL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::OpenL()");	

    CreateFilesL(ETrue /* do provisioning */);
	  
    if ( !iServicesDbOpen )
        {
        OpenServicesDbL();
        }
    if ( !iPasswordsDbOpen )
        {
        OpenPasswordsDbL();
        }
        
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::OpenL() exit");	
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::CreateFilesL()
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::CreateFilesL( TBool aDoProvisioning )
    {
    TBool servicesDbFileExists  = BaflUtils::FileExists( iFsSession, iServicesFile );
    TBool passwordsDbFileExists = BaflUtils::FileExists( iFsSession, iPasswordsFile );

    // Before creating any new databases, check that there is enough disk space.
    if ( !servicesDbFileExists || !passwordsDbFileExists )
        {
        TBool checkResult = EFalse;
        TRAPD( checkError, checkResult =
            SysUtil::DiskSpaceBelowCriticalLevelL(
                &iFsSession, KIptvServiceManagerFreeSpace, EDriveC ) );
        if ( checkError != KErrNone || checkResult )
            {
            // Not enough space on disk, return immediately and do not create databases.
            User::Leave( KErrDiskFull );
            }
        }
    if ( servicesDbFileExists && passwordsDbFileExists )
        {            
        //do nothing
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Files exist, nothing done.");
        return;
        }
    if ( !servicesDbFileExists && !passwordsDbFileExists )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: pwd.dat & services.dat missing");
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: creating services.dat");
        TRAPD( error, CreateServicesDbL() );
        if( error != KErrNone )
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: couldn't create services.dat (%d)", error);
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: deleting services.dat");
            iFsSession.Delete( iPasswordsFile );
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Panicing!");
            User::Panic(KIptvServiceManager, EIptvSmCouldNotCreateServicesFile);
            }
        iServicesDbOpen = ETrue;
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: creating pwd.dat");
        TRAP(error, CreatePasswordsDbL());
        if ( error != KErrNone )
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: couldn't create pwd.dat (%d)", error);
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: deleting pwd.dat");
            iFsSession.Delete(iPasswordsFile);
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Panicing!");
            User::Panic(KIptvServiceManager, EIptvSmCouldNotCreatePasswordsFile);
            }
        iPasswordsDbOpen = ETrue;
        if ( aDoProvisioning )
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: executing provisioning");
            ExecuteProvisioningL();
            }
        return;
        }

    if ( servicesDbFileExists && !passwordsDbFileExists )
        {
        //create pwd.dat
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: pwd.dat missing");
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: creating pwd.dat");
        TRAPD(error, CreatePasswordsDbL());
        if(error != KErrNone)
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: couldn't create pwd.dat (%d)", error);
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: deleting pwd.dat");
            iFsSession.Delete(iPasswordsFile);
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Panicing!");
            User::Panic(KIptvServiceManager, EIptvSmCouldNotCreatePasswordsFile);
            }
        iPasswordsDbOpen = ETrue;
        return;
        }

    if ( !servicesDbFileExists && passwordsDbFileExists )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: services.dat missing, pwd.dat exists.");
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: deleting pwd.dat");
        iFsSession.Delete(iPasswordsFile);
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: creating services.dat");
        TRAPD(error, CreateServicesDbL());
        if ( error != KErrNone )
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: couldn't create services.dat (%d)", error);
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: deleting services.dat");
            iFsSession.Delete(iServicesFile);
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Panicing!");
            User::Panic(KIptvServiceManager, EIptvSmCouldNotCreateServicesFile);
            }
        iServicesDbOpen = ETrue;
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: creating pwd.dat");
        TRAP( error, CreatePasswordsDbL() );
        if ( error != KErrNone )
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: couldn't create pwd.dat (%d)", error);
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: deleting pwd.dat");
            iFsSession.Delete(iPasswordsFile);
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Panicing!");
            User::Panic(KIptvServiceManager, EIptvSmCouldNotCreatePasswordsFile);
            }
        
        iPasswordsDbOpen = ETrue;
        if ( aDoProvisioning )
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: executing provisioning");
            ExecuteProvisioningL();
            }
        
        return;
        }        
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::OpenServicesDbL()
// Open Services database, it's already checked that file exists
// ---------------------------------------------------------------------------
void CIptvServiceManager::OpenServicesDbL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::OpenServicesDbL()");

    if ( iServicesDbOpen )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: was already open.");
        return;
        }
        
    //open Services database
    TRAPD(error,
        {
        iServicesFileStore = CPermanentFileStore::OpenL(
            iFsSession, iServicesFile, EFileRead|EFileWrite );
        iServicesFileStore->SetTypeL( iServicesFileStore->Layout() );
        iServicesDb.OpenL( iServicesFileStore, iServicesFileStore->Root() );
        });
    
    if ( error != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManager:: Could not open services database(%d)", error);

        if ( error != KErrNoMemory && 
             error != KErrLocked && 
             error != KErrDisMounted &&
             error != KErrDiskFull &&
             error != KErrNotReady )
            {
            // Delete and recreate database files. Cannot recover other way. 
            RemoveDbL();
            TRAPD( err, CreateFilesL( ETrue /* do provisioning */ ));
            if ( err != KErrNone )
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: couldnt recreate services.dat & pwd.dat -> panicing");
                User::Panic(KIptvServiceManager, EIptvSmCouldNotOpenServicesDb);
                }
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: leaving");
            User::Leave(error);
            }
        }

    iServicesDbOpen = ETrue;

    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::OpenServicesDbL() exit");
    return;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::OpenPasswordsDbL()
// Open Passwords database, its already checked that file exists
// ---------------------------------------------------------------------------
void CIptvServiceManager::OpenPasswordsDbL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::OpenPasswordsDbL()");
    
    if(iPasswordsDbOpen)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: was already open.");
        return;
        }
        
    //open Passwords database
    TRAPD(error,
        { 
        iPasswordsFileStore = CPermanentFileStore::OpenL(
            iFsSession, iPasswordsFile, EFileRead|EFileWrite );
        iPasswordsFileStore->SetTypeL( iPasswordsFileStore->Layout() );
        iPasswordsDb.OpenL( iPasswordsFileStore, iPasswordsFileStore->Root() );
        });

    if(error != KErrNone)
        {
        if ( error != KErrNoMemory &&
             error != KErrLocked &&
             error != KErrDisMounted &&
             error != KErrDiskFull &&
             error != KErrNotReady )
            {
            // Delete and recreate database files. Cannot recover other way. 
            IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManager:: Could not open password database(%d)", error);
            RemoveDbL();
            TRAPD( err, CreateFilesL( ETrue /* do provisioning */ ));
            if ( err != KErrNone )
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: couldnt recreate services.dat & pwd.dat -> panicing");
                User::Panic(KIptvServiceManager, EIptvSmCouldNotOpenPasswordsDb);
                }
            }
        else
            {
            // Database file might be ok, couldn't just temporarily open it
            IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: not removing database files, leaving");
            User::Leave( error );
            }
        }
        
    iPasswordsDbOpen = ETrue;
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::OpenPasswordsDbL() exit");
    return;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::CreateServicesDbL()
// Create new Services database.
// ---------------------------------------------------------------------------
void CIptvServiceManager::CreateServicesDbL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::CreateServicesDbL()");
        
    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManager:: Services db filename: %S", &iServicesFile);

    SaveSmDbVersion();
                
    // Create empty services database file.
    iServicesFileStore = CPermanentFileStore::ReplaceL(iFsSession, iServicesFile, EFileRead|EFileWrite);
    iServicesFileStore->SetTypeL(iServicesFileStore->Layout()); // Set file store type
    TStreamId id = iServicesDb.CreateL(iServicesFileStore);     // Create stream object
    iServicesFileStore->SetRootL(id);                           // Keep database id as root of store
    iServicesFileStore->CommitL();                              // Complete creation by commiting

    CreateServicesTableL();
    CreateServicesDbIndexL();
     
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreateServicesDbL() Exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::CreatePasswordsDbL()
// Create new Passwords database.
// ---------------------------------------------------------------------------
void CIptvServiceManager::CreatePasswordsDbL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::CreatePasswordsDbL()");
        
    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManager:: Passwords db filename: %S", &iPasswordsFile);
    
    // Create empty passwords database file.
    iPasswordsFileStore = CPermanentFileStore::ReplaceL(iFsSession, iPasswordsFile, EFileRead|EFileWrite);
    iPasswordsFileStore->SetTypeL(iPasswordsFileStore->Layout()); // Set file store type
    TStreamId id = iPasswordsDb.CreateL(iPasswordsFileStore);    // Create stream object
    iPasswordsFileStore->SetRootL(id);                           // Keep database id as root of store
    iPasswordsFileStore->CommitL();                              // Complete creation by commiting

    CreatePasswordsTableL();
    CreatePasswordsDbIndexL();
    
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreatePasswordsDbL() Exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::RemoveDbL()
// ---------------------------------------------------------------------------
void CIptvServiceManager::RemoveDbL()
    {
    Close();

    if ( BaflUtils::FileExists( iFsSession, iServicesFile ) )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: deleting services.dat");
        iFsSession.Delete( iServicesFile );
        }

    if ( BaflUtils::FileExists( iFsSession, iPasswordsFile ) )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: deleting pwd.dat");
        iFsSession.Delete( iPasswordsFile );
        }
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::Close()
// Close databases.
// ---------------------------------------------------------------------------
void CIptvServiceManager::Close()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::Close() start");

    if ( iServicesDbOpen )
        {        
        iServicesDb.Close();
        
        delete iServicesFileStore;
        iServicesFileStore = NULL;

        iServicesDbOpen = EFalse;
        }

    if ( iPasswordsDbOpen )
        {        
        iPasswordsDb.Close();
        
        delete iPasswordsFileStore;
        iPasswordsFileStore = NULL;

        iPasswordsDbOpen = EFalse;
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::Close() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::CreateServicesTableL()
// Creates Services table. Leaves, if the table cannot be created.
// ---------------------------------------------------------------------------
void CIptvServiceManager::CreateServicesTableL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreateServicesTableL()");
    
    // Specify columns for Services table
    TDbCol idCol( KIptvSmServicesDbIdCol, EDbColUint32 );
    idCol.iAttributes |= TDbCol::EAutoIncrement; 
    TDbCol nameCol( KIptvSmServicesDbNameCol, EDbColText, KIptvSmServicesDbNameMaxLength );
    TDbCol addressCol( KIptvSmServicesDbAddressCol, EDbColText, KIptvSmServicesDbAddressMaxLength );
    TDbCol iconPathCol( KIptvSmServicesDbIconPathCol, EDbColText, KIptvSmServicesDbIconPathMaxLength );
    TDbCol epgPluginUidCol( KIptvSmServicesDbEpgPluginUidCol, EDbColInt32 );
    TDbCol vodPluginUidCol( KIptvSmServicesDbVodPluginUidCol, EDbColInt32 );
    TDbCol iptvPluginUidCol( KIptvSmServicesDbIptvPluginUidCol, EDbColInt32 );
    TDbCol typeCol( KIptvSmServicesDbTypeCol, EDbColUint8);
    TDbCol iapListCol( KIptvSmServicesDbIapListCol, EDbColText8, KIptvSmServicesDbIapListMaxLength );
    TDbCol descCol( KIptvSmServicesDbDescCol, EDbColLongText ); // Stream Data
    TDbCol dateCol( KIptvSmServicesDbDateCol, EDbColDateTime );    
    TDbCol flagsCol( KIptvSmServicesDbFlagsCol, EDbColUint32 );
    TDbCol applicationUidCol( KIptvSmServicesDbApplicationUidCol, EDbColInt32 );
    TDbCol displayOrderCol( KIptvSmServicesDbDisplayOrderCol, EDbColUint32 );
    TDbCol providerIdCol( KIptvSmServicesDbProviderIdCol, EDbColText, KIptvSmServicesDbProviderIdMaxLength );
    TDbCol accountManagementUrlCol( KIptvSmServicesDbAccountManagementUrlCol, EDbColText, KIptvSmServicesDbAccountManagementUrlMaxLength );
    TDbCol epgUpdateTimeCol( KIptvSmServicesDbEpgUpdateTimeCol, EDbColDateTime );
    epgUpdateTimeCol.iAttributes |= TDbCol::ENotNull;
    TDbCol iapNameCol( KIptvSmServicesDbIapNameCol, EDbColText, KIptvSmServicesDbIapNameMaxLength );
    TDbCol searchUrlCol( KIptvSmServicesDbSearchUrlCol, EDbColLongText, KIptvSmServicesDbSearchUrlMaxLength ); // Stream Data
    TDbCol dlNetwork( KIptvSmServicesDbScheduledDlNetworkCol, EDbColUint32 );
    TDbCol dlTime( KIptvSmServicesDbScheduledDlTimeCol, EDbColUint32 );
    TDbCol dlType( KIptvSmServicesDbScheduledDlTypeCol, EDbColUint32 );
    TDbCol lastDlTimeCol( KIptvSmServicesDbLastScheduledDlTime, EDbColDateTime );
    TDbCol groupIdCol( KIptvSmServicesDbGroupIdCol, EDbColUint32 );
    TDbCol useCateCol( KIptvSmServicesDbUseCategoryCol, EDbColUint8 );
    TDbCol uploadCol( KIptvSmServicesDbUploadProviderCol, EDbColText, KIptvSmServicesDbNameMaxLength );
    TDbCol iconUrlCol( KIptvSmServicesDbIconUrlCol, EDbColText, KIptvSmServicesDbAddressMaxLength );
    TDbCol recordUrlCol( KIptvSmServicesDbRecordUrlCol, EDbColLongText, KIptvSmServicesDbRecordUrlMaxLength); // Stream Data

    // Add columns to column set
    CDbColSet* serviceColSet = CDbColSet::NewLC();
    serviceColSet->AddL( idCol );                   // 1
    serviceColSet->AddL( nameCol );                 // 2
    serviceColSet->AddL( addressCol );              // 3
    serviceColSet->AddL( iconPathCol );             // 4
    serviceColSet->AddL( epgPluginUidCol );         // 5
    serviceColSet->AddL( vodPluginUidCol );         // 6
    serviceColSet->AddL( iptvPluginUidCol );        // 7
    serviceColSet->AddL( typeCol );                 // 8
    serviceColSet->AddL( iapListCol );              // 9
    serviceColSet->AddL( descCol );                 // 10
    serviceColSet->AddL( dateCol );                 // 11
    serviceColSet->AddL( flagsCol );                // 12
    serviceColSet->AddL( applicationUidCol );       // 13
    serviceColSet->AddL( displayOrderCol );         // 14
    serviceColSet->AddL( providerIdCol );           // 15
    serviceColSet->AddL( accountManagementUrlCol ); // 16
    serviceColSet->AddL( epgUpdateTimeCol );        // 17 //last item in vers 1
    serviceColSet->AddL( iapNameCol );              // 18
    serviceColSet->AddL( searchUrlCol );            // 19
    serviceColSet->AddL( dlNetwork );               // 20
    serviceColSet->AddL( dlTime );                  // 21
    serviceColSet->AddL( dlType );                  // 22
    serviceColSet->AddL( lastDlTimeCol );           // 23 //last item in vers 2
    serviceColSet->AddL( groupIdCol );              // 24 
    serviceColSet->AddL( useCateCol );              // 25 
    serviceColSet->AddL( uploadCol );               // 26
    serviceColSet->AddL( iconUrlCol );              // 27 //last item in vers 3
    serviceColSet->AddL( recordUrlCol );            // 28 //last item in vers 3

    // Create the Services table
    User::LeaveIfError(
        iServicesDb.CreateTable( KIptvSmServicesTable, *serviceColSet ) );
    CleanupStack::PopAndDestroy( serviceColSet );
    
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreateServicesTableL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::CreatePasswordsTableL()
// Creates Passwords table. Leaves, if the table cannot be created.
// ---------------------------------------------------------------------------
void CIptvServiceManager::CreatePasswordsTableL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreatePasswordsTableL()");
    
    // Specify columns for Passwords table
    TDbCol idCol( KIptvSmPasswordsDbIdCol, EDbColUint32 );
    TDbCol userNameCol( KIptvSmPasswordsDbUserNameCol,
        EDbColText, KIptvSmPasswordsDbUserNameMaxLength );
    TDbCol passwordCol( KIptvSmPasswordsDbPasswordCol,
        EDbColText, KIptvSmPasswordsDbPasswordMaxLength );

    // Add columns to column set
    CDbColSet* passwordColSet = CDbColSet::NewLC();
    passwordColSet->AddL( idCol );
    passwordColSet->AddL( userNameCol );
    passwordColSet->AddL( passwordCol );

    // Create the Passwords table
    User::LeaveIfError(
        iPasswordsDb.CreateTable( KIptvSmPasswordsTable, *passwordColSet ) );
    CleanupStack::PopAndDestroy( passwordColSet );
    
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreatePasswordsTableL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::CreateServicesDbIndexL()
// Creates an index for Services table. Leaves, if the index cannot be created.
// ---------------------------------------------------------------------------
void CIptvServiceManager::CreateServicesDbIndexL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreateServicesDbIndexL()");

    // Create index consisting of ID
    TDbKeyCol idCol( KIptvSmServicesDbIdCol );

    CDbKey* index = CDbKey::NewLC();   // create index key set
    index->AddL( idCol );
    index->MakeUnique();
    User::LeaveIfError( iServicesDb.CreateIndex(
        KIptvSmServicesDbIndexName, KIptvSmServicesTable, *index ) );
    CleanupStack::PopAndDestroy( index );

    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreateServicesDbIndexL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::CreatePasswordsDbIndexL()
// Creates an index for Passwords table in Passwords db. Leaves, if the index
// cannot be created.
// ---------------------------------------------------------------------------
void CIptvServiceManager::CreatePasswordsDbIndexL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreatePasswordsDbIndexL()");

    // Create index consisting of ID
    TDbKeyCol idCol( KIptvSmPasswordsDbIdCol );

    CDbKey* index = CDbKey::NewLC();   // create index key set
    index->AddL( idCol );
    index->MakeUnique();
    User::LeaveIfError( iPasswordsDb.CreateIndex(
        KIptvSmPasswordsDbIndexName, KIptvSmPasswordsTable, *index ) );
    CleanupStack::PopAndDestroy( index );

    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CreatePasswordsDbIndexL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::AddOrUpdateServiceL
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::AddOrUpdateServiceL(
    CIptvService& aService,
    TUint32 aModeFlags )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::AddOrUpdateServiceL()");
    
    if ( !( aModeFlags & EDontOpen ) )
        {
        OpenL();
        CleanupClosePushL( *this ); // 1->
        }
    
    CompactDatabases();
    
    RDbTable servicesTable;
    RDbTable passwordsTable;

    TBool sendServiceAddedEvent = EFalse;
    TBool sendServiceModifiedEvent = EFalse;
    TBool sendServiceScheduledEvent = EFalse;
    TBool sendServiceSelectionModifiedEvent = EFalse;

    // Create an updateable database table object
    TInt err( servicesTable.Open(
        iServicesDb, KIptvSmServicesTable, servicesTable.EUpdatable ) );
    if ( err != KErrNone )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Could not get services table, aborting!");
        User::Leave( KErrGeneral );
        }
    CleanupClosePushL( servicesTable ); // 2->

    err = passwordsTable.Open(
        iPasswordsDb, KIptvSmPasswordsTable, passwordsTable.EUpdatable );
    if ( err != KErrNone )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Could not get passwords table, aborting!");
        User::Leave( KErrGeneral );
        }
    CleanupClosePushL( passwordsTable ); // 3->

    if ( servicesTable.CountL() >= KIptvSmMaxServices )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: DB max entries exceeded, aborting!");
        User::Leave( KErrOverflow );    
        }
        
    CDbColSet* servicesColSet = servicesTable.ColSetL();
    CleanupStack::PushL( servicesColSet );  // 4->
    CDbColSet* passwordsColSet = passwordsTable.ColSetL();
    CleanupStack::PushL( passwordsColSet ); // 5->

    servicesTable.SetIndex( KIptvSmServicesDbIndexName );
    servicesTable.Reset();
    passwordsTable.SetIndex( KIptvSmPasswordsDbIndexName );
    passwordsTable.Reset();

    TUint serviceId = 0;
    
    if ( aModeFlags & EAddService )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Adding a new service");
		
        // add new entry to Services table, ID is autoincrement
        servicesTable.InsertL();
        
        // Get ID
        serviceId = servicesTable.ColUint32(
            servicesColSet->ColNo( KIptvSmServicesDbIdCol ) );

        // Inform observers later if not grouped service
        if ( !( aService.GetFlags() & CIptvService::EGroupedService ) )
            {
            sendServiceAddedEvent = ETrue;
            }
        
        // Add id+1 as default display order value for service
        servicesTable.SetColL( servicesColSet->ColNo(
            KIptvSmServicesDbDisplayOrderCol ), serviceId + 1 );
        
        // Add new entry to Passwords db, use ID from Services table
        passwordsTable.InsertL();
        passwordsTable.SetColL( passwordsColSet->ColNo(
            KIptvSmPasswordsDbIdCol ), serviceId );
        
        // Update new Service Id to the actual service.
        aService.SetId( serviceId );
        }
    else
        {
        IPTVLOGSTRING_HIGH_LEVEL("Updating existing service");
        //Mode = EUpdateService 
		
        if ( aService.GetDisplayOrder() == 0 )
            {
            aService.SetDisplayOrder( MaxDisplayOrderL() );
            }

        //-> find the service item to update
        if ( !servicesTable.SeekL( ( TInt )( aService.GetId() ) ) )
            {
            //not found
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: Service(%d) not found, aborting", aService.GetId() );			
            User::Leave( KErrNotFound );
            }

        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Service entry found, updating..");
        servicesTable.UpdateL();

        if ( passwordsTable.SeekL( ( TInt )( aService.GetId() ) ) )
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Password entry found, updating..");
            passwordsTable.UpdateL();
            }
        else
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: Passwords db does not contain ID: %d", aService.GetId() );
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Adding it.");
            passwordsTable.InsertL();
            passwordsTable.SetColL( passwordsColSet->ColNo(
                KIptvSmPasswordsDbIdCol ), aService.GetId() );
            }
        }
        
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbNameCol ), aService.GetName() );

    //check if address is modified and do we need to send event
    TPtrC address;
    address.Set( servicesTable.ColDes(
        servicesColSet->ColNo( KIptvSmServicesDbAddressCol ) ) );

    if ( address != aService.GetAddress() ) //has address changed
        {
        if ( !( aModeFlags & EAddService ) ) //is this a modify call
            {
            sendServiceModifiedEvent = ETrue;
            }
        }

    // check if the scheduled download settings have been changed and
    TUint dlNetwork = servicesTable.ColUint32(
        servicesColSet->ColNo( KIptvSmServicesDbScheduledDlNetworkCol ) );
    TUint dlTime = servicesTable.ColUint32(
        servicesColSet->ColNo( KIptvSmServicesDbScheduledDlTimeCol ) );
    TUint dlType = servicesTable.ColUint32(
        servicesColSet->ColNo( KIptvSmServicesDbScheduledDlTypeCol ) );
    TTime dlLastTime = servicesTable.ColTime(
        servicesColSet->ColNo( KIptvSmServicesDbLastScheduledDlTime ) );

    if ( dlNetwork != aService.ScheduleDlNetwork() ||
         dlTime != aService.ScheduleDlTime() ||
         dlType != aService.ScheduleDlType() ||
         dlLastTime != aService.GetScheduledLastDownloadTime() )
        {
        // changed, we need to send event
        sendServiceScheduledEvent = ETrue;
        }

    // check if Selected flag is modified and do we need to send event
    TUint newSelectedFlag = aService.GetFlags() & CIptvService::ESelected;
    TUint oldSelectedFlag = servicesTable.ColUint32(
        servicesColSet->ColNo( KIptvSmServicesDbFlagsCol ) ) & CIptvService::ESelected;
    if ( newSelectedFlag != oldSelectedFlag )
        {
        sendServiceSelectionModifiedEvent = ETrue;
        }

    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbAddressCol ), aService.GetAddress() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbIconPathCol ), aService.GetIconPath() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbEpgPluginUidCol ), aService.GetEpgPluginUid().iUid );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbVodPluginUidCol ), aService.GetVodPluginUid().iUid );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbIptvPluginUidCol ), aService.GetIptvPluginUid().iUid );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbTypeCol), aService.GetType() );
    
    CIptvIapList& iapList = aService.GetIapList();
    RDesWriteStream writeStream;
    HBufC8* iapListDes = HBufC8::NewL( iapList.CountExternalizeSize() +1 );
    CleanupStack::PushL( iapListDes ); // 5->
    TPtr8 iapListPtr8( iapListDes->Des() );
    writeStream.Open( iapListPtr8 );
    CleanupClosePushL( writeStream ); // 6->
    iapList.ExternalizeL( writeStream );
    CleanupStack::PopAndDestroy( &writeStream ); // <-6
    servicesTable.SetColL( servicesColSet->ColNo( KIptvSmServicesDbIapListCol ), iapListPtr8 );
    CleanupStack::PopAndDestroy( iapListDes ); // <-5
    
    passwordsTable.SetColL( passwordsColSet->ColNo(
        KIptvSmPasswordsDbUserNameCol ), aService.GetUserName() );
    passwordsTable.SetColL( passwordsColSet->ColNo(
        KIptvSmPasswordsDbPasswordCol ), aService.GetPassword() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbDescCol ), aService.GetDesc() );
    
    if ( aModeFlags & EAddService )
        {
        TTime time;	
        time.HomeTime();
        servicesTable.SetColL( servicesColSet->ColNo( KIptvSmServicesDbDateCol ), time );
        }
        
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbFlagsCol ), aService.GetFlags() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbApplicationUidCol ), aService.GetApplicationUid() );
    
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbProviderIdCol ), aService.GetProviderId() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbAccountManagementUrlCol ), aService.GetAccountManagementUrl() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbEpgUpdateTimeCol ), aService.GetEpgUpdateTimeL() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbIapNameCol ), aService.GetIapNameL() );

    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbSearchUrlCol ), aService.SearchUrl() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbScheduledDlNetworkCol ), aService.ScheduleDlNetwork() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbScheduledDlTimeCol ), aService.ScheduleDlTime() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbScheduledDlTypeCol ), aService.ScheduleDlType() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbLastScheduledDlTime ), aService.GetScheduledLastDownloadTime() );

    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbGroupIdCol ), aService.GetGroupId() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbUseCategoryCol), aService.GetUseCategory() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbUploadProviderCol ), aService.GetUploadProvider() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbIconUrlCol ), aService.GetIconUrl() );
    servicesTable.SetColL( servicesColSet->ColNo(
        KIptvSmServicesDbRecordUrlCol ), aService.RecordUrl() );

    //check if display order is modified
    TUint32 sourceDisplayOrder = servicesTable.ColUint32( servicesColSet->ColNo( KIptvSmServicesDbDisplayOrderCol ) );
    TUint32 targetDisplayOrder = aService.GetDisplayOrder();

    if ( !( aModeFlags & EAddService ) 
        && sourceDisplayOrder != targetDisplayOrder )
        {
        // Set defined display order value for service 
        servicesTable.SetColL( servicesColSet->
            ColNo( KIptvSmServicesDbDisplayOrderCol ), targetDisplayOrder );
        sendServiceModifiedEvent = ETrue;
        }
                   
    servicesTable.PutL();    // Complete changes (the insertion)			
    passwordsTable.PutL();   // Complete changes (the insertion)			
 
    CleanupStack::PopAndDestroy( passwordsColSet ); // <-5
    CleanupStack::PopAndDestroy( servicesColSet );	// <-4           
    
    if ( !( aModeFlags & EAddService ) 
        && sourceDisplayOrder != targetDisplayOrder )
        {
        for ( servicesTable.FirstL(); servicesTable.AtRow(); servicesTable.NextL() )
            { 
              //Get row
            servicesTable.GetL();
            servicesColSet = servicesTable.ColSetL();
            CleanupStack::PushL( servicesColSet );
            
            TUint32 currentServiceId =
                servicesTable.ColUint32( servicesColSet->ColNo( KIptvSmServicesDbIdCol ) ); 
            
            TUint32 currentDisplayOrder = 
                servicesTable.ColUint32( servicesColSet->ColNo( KIptvSmServicesDbDisplayOrderCol ) );
                
            if ( currentServiceId != aService.GetId() )
                {
                servicesTable.UpdateL();
                
                if ( ( currentDisplayOrder <= targetDisplayOrder ) &&
                     ( currentDisplayOrder >= sourceDisplayOrder ) )
                    {
                    currentDisplayOrder--;
                    }
                else if ( ( currentDisplayOrder >= targetDisplayOrder ) &&
                          ( currentDisplayOrder <= sourceDisplayOrder ) ) 
                    {
                    currentDisplayOrder++;
                    }
                    
                servicesTable.SetColL( servicesColSet->ColNo(
                    KIptvSmServicesDbDisplayOrderCol ), currentDisplayOrder );
                servicesTable.PutL(); 
                }
                
            CleanupStack::PopAndDestroy( servicesColSet );
            }
        }    

    CleanupStack::PopAndDestroy( &passwordsTable ); // <-3
    CleanupStack::PopAndDestroy( &servicesTable );  // <-2

    if ( !( aModeFlags & EDontOpen ) )
        {
        CleanupStack::PopAndDestroy( this );        // <-1
        }

    if ( sendServiceAddedEvent )
        {
        InformObserversL( CIptvSmEvent::EServiceAdded, serviceId, &aService );
        }

    if ( sendServiceModifiedEvent )
        {
        InformObserversL( CIptvSmEvent::EServiceModified, aService.GetId(), &aService );
        }

    if ( sendServiceScheduledEvent )
        {
        InformObserversL( CIptvSmEvent::EServiceScheduleModified, aService.GetId(), &aService );
        }

    if ( sendServiceSelectionModifiedEvent )
        {
        InformObserversL( CIptvSmEvent::EServiceSelectionModified, aService.GetId(), &aService );
        }

    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::AddOrUpdateServiceL() exit");
    }

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
// -----------------------------------------------------------------------------
// CIptvServiceManager::UpdateVideoStoreSpaceDbL()
// Updates Video Store Service to active space database.
// -----------------------------------------------------------------------------
//
void CIptvServiceManager::UpdateVideoStoreSpaceDbL( CIptvService& aService )
    {
    if ( !iActiveSpaceHelper )
        {
        User::Leave( KErrNotFound );
        }

    IPTVLOGSTRING_LOW_LEVEL( "IptvUiEngine ## CIptvServiceManager::UpdateSpaceDb() - Enter" );

    CActiveSpaceBase* activeSpace = iActiveSpaceHelper->NewActiveSpaceL();
    CleanupStack::PushL( activeSpace );
    IPTVLOGSTRING_LOW_LEVEL( "IptvUiEngine ## CIptvServiceManager::UpdateSpaceDb() - CActiveSpace Created" );

    // Create item to 6th category as Video Store.
    TActiveSpaceDataProperties serviceProperties;
    serviceProperties.iGenre = EActiveSpaceGenreWatch;
    serviceProperties.iSource = EActiveSpaceSourceLocal;
    serviceProperties.iOwner = EActiveSpaceOwnerCommercial;
    serviceProperties.iFlags = EActiveSpaceFlagNone;
    serviceProperties.iCategory = EActiveSpaceCategory6th;

    // Create parent
    CActiveSpaceDataBase* parentData = iActiveSpaceHelper->NewActiveSpaceDataL();
    CleanupStack::PushL( parentData );
    IPTVLOGSTRING_LOW_LEVEL( "IptvUiEngine ## CIptvServiceManager::UpdateVideoStoreSpaceDbL() - CActiveSpaceData Parent Created" );
    parentData->SetParent( CActiveSpaceDataBase::EActiveSpaceRoot );		
    parentData->SetName( EVideoTileNokiaVideoShop );
    parentData->SetProperties( serviceProperties );

    if ( ( aService.GetIconPath().Length() > KIptvMaxLengthOfEmptyThumbPath ) &&
         BaflUtils::FileExists( iFsSession, aService.GetIconPath() ) )
        {
        parentData->SetIconL( aService.GetIconPath() );
        IPTVLOGSTRING_LOW_LEVEL( "IptvUiEngine ## CIptvServiceManager::UpdateVideoStoreSpaceDbL() - Icon added" );
        }
    else
        {
        parentData->SetIcon( EAspaLogicalIconVideoTileDefault );
        IPTVLOGSTRING_LOW_LEVEL( "IptvUiEngine ## CIptvServiceManager::UpdateVideoStoreSpaceDbL() - Default icon added" );
        }

    TInt parentId = activeSpace->AddDataL( parentData, CActiveSpaceDataBase::EActiveSpaceRoot );   	  
    IPTVLOGSTRING_LOW_LEVEL("IptvUiEngine ## CIptvServiceManager::UpdateVideoStoreSpaceDbL() - Parent data added");  

    if ( parentId > 0 )          // No error
        {
        // Create child
        CActiveSpaceDataBase* childData = iActiveSpaceHelper->NewActiveSpaceDataL();
        CleanupStack::PushL( childData );  
        childData->SetParent( parentId );
        childData->SetName( EVideoTileNokiaVideoShopAd );
        serviceProperties.iCategory = EActiveSpaceCategory1st;
        childData->SetProperties( serviceProperties );
	    TInt childId = activeSpace->AddDataL( childData, parentId );
	    IPTVLOGSTRING_LOW_LEVEL("IptvUiEngine ## CIptvServiceManager::UpdateVideoStoreSpaceDbL() - Child data added");
	
        CASpaAppLauncherActionDataBase* serviceData = iActiveSpaceHelper->NewAppLauncherActionDataL();
	    CleanupStack::PushL( serviceData );

	    TUid videoCenterAppUid = KIptvMain;
	    TUid vodPlayerUid = TUid::Uid( KIptvVodStartPlayer );
	
        TIptvActivationMessage params;
		params.iMsgType = TIptvActivationMessage::EOpenVideoShop;
		params.iMsgSender = TIptvActivationMessage::EActiveSpace;
		params.iFileId = 0;
		params.iDrive = 0;
		TPckg<TIptvActivationMessage> paramsPckg( params );
	
	    serviceData->SetApplicationUid( videoCenterAppUid );
	    serviceData->SetApaLaunchingMethod(
	        CASpaAppLauncherActionDataBase::
            EApaLaunchMethodStartApaMessageWithTail );               
	    serviceData->SetAdditionalDataL( paramsPckg );                 
	    serviceData->SetMessageUid( vodPlayerUid );	
	    
	    const TInt KCASpaAppLauncherPlugin = 0x10282E5F;
	    TUid AppLauncherPlugin( TUid::Uid( KCASpaAppLauncherPlugin ) );

	    CActiveSpaceActionBase* serviceAction = iActiveSpaceHelper->NewActiveSpaceActionL();
	    CleanupStack::PushL( serviceAction );

	    serviceAction->SetType( EActiveSpaceActionTypeLaunchApplication );
	    serviceAction->SetActionHandlerUid( TUid::Uid( KCASpaAppLauncherPlugin ) );	
	    serviceAction->SetKeys( EActiveSpaceActionKeySelect );
	    serviceAction->SetDataL( serviceData->GetBufferL() );
		
	    IPTVLOGSTRING_LOW_LEVEL("IptvUiEngine ## CIptvServiceManager::UpdateVideoStoreSpaceDbL() - CActiveSpaceAction Object Created"); 									
										
	    TInt actionId = activeSpace->AddActionL( serviceAction );
	    activeSpace->BindActionToDataL( parentId, actionId );
	    IPTVLOGSTRING_LOW_LEVEL("IptvUiEngine ## CIptvServiceManager::UpdateVideoStoreSpaceDbL() - Action binded");
	
	    CleanupStack::PopAndDestroy( serviceAction );
	    CleanupStack::PopAndDestroy( serviceData );                        
	    CleanupStack::PopAndDestroy( childData );
	    }
	CleanupStack::PopAndDestroy( parentData );	
	CleanupStack::PopAndDestroy( activeSpace );

	IPTVLOGSTRING_LOW_LEVEL("IptvUiEngine ## CIptvServiceManager::UpdateVideoStoreSpaceDbL() - Exit");
	}
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

// ---------------------------------------------------------------------------
// CIptvServiceManager::GetServicesL()
// Get services from databases.
// ---------------------------------------------------------------------------
//
CIptvServices* CIptvServiceManager::GetServicesL(
    TUint32 aSearchLimitFlags,
    CIptvServiceManagementClient::TOrder aOrder,
    TInt aVersion )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvServiceManager::GetServicesL(aSearchLimitFlags, aOrder)" );

    OpenL();
    CleanupClosePushL( *this ); // 1->

    CIptvServices* services = CIptvServices::NewL();
    CleanupStack::PushL( services ); // 2->
    
    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManager:: Search limit flags: %d", aSearchLimitFlags );	

    TInt32 epgPluginUid, vodPluginUid, iptvPluginUid, applicationUid;
    TUint32 id, flags, displayOrder;
    TPtrC name, address, iconPath, providerId, accountManagementUrl, iapName;
    TPtrC8 iapList;
    TUint32 dlNetwork( 0 );
    TUint32 dlTime( 0 );
    TUint32 dlType( 0 );
    TUint32 groupId( 0 );
    TTime lastScheduledDlTime;
    TUint8 type;
    TUint8 useCategory( 0 );
    TPtrC uploadProvider, iconUrl;
    TBuf<KIptvSmServicesDbDescMaxLength> desc;
    TTime epgUpdateTime;
    
    CIptvService* iptvService;
    iptvService = CIptvService::NewL();
    CleanupStack::PushL( iptvService ); // 3->

    RDbView view;    // Create a view on the database

/*
SELECT "column_name"
FROM "table_name"
[WHERE "condition"]
ORDER BY "column_name" [ASC, DESC]
*/
    const TInt32 KIptvSmServiceManagerSelectCmdLength = 32 + 20;
    _LIT( KIptvSmSelectCmdFirstPart, "SELECT * FROM Services ORDER BY " );
    _LIT( KIptvSmSelectCmdDateAsc, "Date ASC" );
    _LIT( KIptvSmSelectCmdDateDescending, "Date DESC" );
    _LIT( KIptvSmSelectCmdDisplayOrderAsc, "DisplayOrder ASC" );
    _LIT( KIptvSmSelectCmdDisplayOrderDescending, "DisplayOrder DESC" );
    
    TBuf<KIptvSmServiceManagerSelectCmdLength> selectCmd;
    
    _LIT( KDesDesFormat, "%S%S" );
    
    switch ( aOrder )
        {
        case CIptvServiceManagementClient::EDateAscending:
            selectCmd.Format( KDesDesFormat,
                             &KIptvSmSelectCmdFirstPart,
                             &KIptvSmSelectCmdDateAsc); 
            break;
        case CIptvServiceManagementClient::EDateDescending:
            selectCmd.Format( KDesDesFormat, 
                             &KIptvSmSelectCmdFirstPart,
                             &KIptvSmSelectCmdDateDescending); 
            break;
        case CIptvServiceManagementClient::EDisplayOrderAscending:
            selectCmd.Format( KDesDesFormat,
                             &KIptvSmSelectCmdFirstPart,
                             &KIptvSmSelectCmdDisplayOrderAsc); 
            break;
        case CIptvServiceManagementClient::EDisplayOrderDescending:
            selectCmd.Format( KDesDesFormat,
                             &KIptvSmSelectCmdFirstPart,
                             &KIptvSmSelectCmdDisplayOrderDescending); 
            break;
            
        default:
            User::Leave( KErrArgument );
        }
             
    User::LeaveIfError( view.Prepare( iServicesDb, TDbQuery( selectCmd, EDbCompareFolded ) ) );
    User::LeaveIfError( view.EvaluateAll() );
    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL( colSet ); // 4->
    
    for ( view.FirstL(); view.AtRow(); view.NextL() )
        {
        desc.Zero();
        view.GetL();

        //set variables and set pointers to point to db items
        id = view.ColUint32( colSet->ColNo( KIptvSmServicesDbIdCol ) );
        name.Set( view.ColDes( colSet->ColNo( KIptvSmServicesDbNameCol ) ) );
        address.Set( view.ColDes( colSet->ColNo( KIptvSmServicesDbAddressCol ) ) );
        iconPath.Set( view.ColDes( colSet->ColNo( KIptvSmServicesDbIconPathCol ) ) );        
        epgPluginUid = view.ColInt32( colSet->ColNo( KIptvSmServicesDbEpgPluginUidCol ) );
        vodPluginUid = view.ColInt32( colSet->ColNo( KIptvSmServicesDbVodPluginUidCol ) );
        iptvPluginUid = view.ColInt32( colSet->ColNo( KIptvSmServicesDbIptvPluginUidCol ) );
        type = view.ColUint8( colSet->ColNo( KIptvSmServicesDbTypeCol ) );
        iapList.Set( view.ColDes8( colSet->ColNo( KIptvSmServicesDbIapListCol ) ) );
        
        TDbColNo descrColNo = colSet->ColNo( KIptvSmServicesDbDescCol );
        RDbColReadStream readStream; // A stream object for long columns
        readStream.OpenLC( view,descrColNo ); // 5->
        readStream.ReadL( desc, view.ColLength( descrColNo ) );
        readStream.Close();
        CleanupStack::Pop(); // <-5
				
        //date not needed

        flags = view.ColUint32( colSet->ColNo( KIptvSmServicesDbFlagsCol ) );
        applicationUid = view.ColInt32( colSet->ColNo( KIptvSmServicesDbApplicationUidCol ) );
        displayOrder = view.ColUint32( colSet->ColNo( KIptvSmServicesDbDisplayOrderCol ) );
        providerId.Set( view.ColDes( colSet->ColNo( KIptvSmServicesDbProviderIdCol ) ) );
        accountManagementUrl.Set( view.ColDes( colSet->ColNo( KIptvSmServicesDbAccountManagementUrlCol ) ) );
        TInt colNum = colSet->ColNo( KIptvSmServicesDbEpgUpdateTimeCol );
        
        if ( aVersion >= 2 || aVersion == 0 )
            {
            epgUpdateTime = view.ColTime( colNum );
            iapName.Set( view.ColDes( colSet->ColNo( KIptvSmServicesDbIapNameCol ) ) );
            
            delete iSearchUrl;
            iSearchUrl = NULL;
            TDbColNo searchUrlColNo = colSet->ColNo( KIptvSmServicesDbSearchUrlCol );
            iSearchUrl = HBufC::NewL( view.ColLength( searchUrlColNo ) );
            RDbColReadStream readStream;
            readStream.OpenLC( view, searchUrlColNo ); // 6->
            TPtr searchUrlPtr( iSearchUrl->Des() );
            readStream.ReadL( searchUrlPtr, view.ColLength( searchUrlColNo ) );
            readStream.Close();
            CleanupStack::Pop(); // <-6

            dlNetwork = view.ColUint32( colSet->ColNo( KIptvSmServicesDbScheduledDlNetworkCol ) );
            dlTime = view.ColUint32( colSet->ColNo( KIptvSmServicesDbScheduledDlTimeCol ) );
            dlType = view.ColUint32( colSet->ColNo( KIptvSmServicesDbScheduledDlTypeCol ) );
            lastScheduledDlTime = view.ColTime( colSet->ColNo( KIptvSmServicesDbLastScheduledDlTime ) );
            }
            
        if ( aVersion >= 3 || aVersion == 0 )
            {
            groupId = view.ColUint32( colSet->ColNo( KIptvSmServicesDbGroupIdCol ) );
            useCategory = view.ColUint8( colSet->ColNo( KIptvSmServicesDbUseCategoryCol ) );
            uploadProvider.Set( view.ColDes( colSet->ColNo( KIptvSmServicesDbUploadProviderCol ) ) );
            iconUrl.Set( view.ColDes( colSet->ColNo( KIptvSmServicesDbIconUrlCol ) ) );

            TDbColNo recordUrlColNo = colSet->ColNo( KIptvSmServicesDbRecordUrlCol );
            delete iRecordUrl;
            iRecordUrl = NULL;
            iRecordUrl = HBufC::NewL( view.ColLength( recordUrlColNo ) );

            RDbColReadStream readStreamRec;
            readStreamRec.OpenLC( view, recordUrlColNo ); // 6-> 
            TPtr recordUrlPtr( iRecordUrl->Des() );
            readStreamRec.ReadL( recordUrlPtr, view.ColLength( recordUrlColNo ) );
            readStreamRec.Close();
            CleanupStack::Pop(); // <-6
            }
        
        //construct CIptvService object using set methods
        iptvService->SetId( id );
        iptvService->SetName( name );
        iptvService->SetAddress( address );
        iptvService->SetIconPath( iconPath );        

        TUid uid;
        uid.iUid = epgPluginUid;
        iptvService->SetEpgPluginUid( uid );
        uid.iUid = vodPluginUid;
        iptvService->SetVodPluginUid( uid );        
        uid.iUid = iptvPluginUid;
        iptvService->SetIptvPluginUid( uid );
        iptvService->SetType( ( CIptvService::TServiceType)type );

        //set IAP list        
        RDesReadStream readStreamIap;
        readStreamIap.Open( iapList );
        CleanupClosePushL( readStreamIap ); // 6->
        iptvService->iIapList->InternalizeL( readStreamIap );
        CleanupStack::PopAndDestroy( &readStreamIap ); // <-6
        
        iptvService->SetDesc( desc );
        iptvService->SetFlags( flags );
        iptvService->SetApplicationUid( applicationUid );
        iptvService->SetDisplayOrder( displayOrder );
        iptvService->SetProviderId( providerId );
        iptvService->SetAccountManagementUrl( accountManagementUrl );
        
        if ( aVersion >= 2 || aVersion == 0 )
            {
            iptvService->SetEpgUpdateTimeL( epgUpdateTime );
            iptvService->SetIapNameL( iapName );

            TPtr searchUrlPtr( iSearchUrl->Des() );
            iptvService->SetSearchUrlL( searchUrlPtr );
            delete iSearchUrl;
            iSearchUrl = NULL;
            iptvService->SetScheduleDlNetwork( dlNetwork );
            iptvService->SetScheduleDlTime( dlTime );
            iptvService->SetScheduleDlType( dlType );
            iptvService->SetScheduledLastDownloadTime( lastScheduledDlTime );
            }
                
        if ( aVersion >= 3 || aVersion == 0 )
            {
            iptvService->SetGroupId( groupId );
            iptvService->SetUseCategory( useCategory );
            iptvService->SetUploadProvider( uploadProvider );
            iptvService->SetIconUrl( iconUrl );
            TPtr recordUrlPtr( iRecordUrl->Des() );
            iptvService->SetRecordUrlL( recordUrlPtr );
            delete iRecordUrl;
            iRecordUrl = NULL; 
            }

        // Copy service to services if limit flags allow, get stuff from Password db too
        if ( IsServiceIncludedInSearch( aSearchLimitFlags, *iptvService ) )
            {
            TIptvSmPassword password;            
            TRAPD(err, GetPasswordL(iptvService->GetId(), password));
            
            if ( err != KErrNone )
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: could not get password, using empty descriptor!");
                iptvService->SetPassword( KIptvEmptyDes );
                iptvService->SetUserName( KIptvEmptyDes );
                }
            else
                {
                iptvService->SetPassword( password.iPassword );
                iptvService->SetUserName( password.iUserName );                        
                }
            
            services->AddL( *iptvService );
            }

        } //for(

    view.Close();

    CleanupStack::PopAndDestroy( colSet );     //<-4
    CleanupStack::PopAndDestroy( iptvService );//<-3
    CleanupStack::Pop( services );             //<-2 ownership moves to caller
    CleanupStack::PopAndDestroy( this );       //<-1
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::GetServicesL(aSearchLimitFlags, aOrder) exit");
    return services;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::GetServicesUsingIdL()
// Get services from the database using index search.
// ---------------------------------------------------------------------------
//
CIptvServices* CIptvServiceManager::GetServicesUsingIdL(
    TUint32 aStartId,
    TUint32 aEndId,
    TBool aDoOpenClose)
    {	    
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::GetServicesUsingIdL(aStartId, aEndId)");
	
	if ( aDoOpenClose )
        {
	    OpenL();
	    CleanupClosePushL( *this ); // 1->
        }
        	  
    TUint i;
    RDbTable rowset;
    TDbSeekKey seekKey;

    TInt err( rowset.Open(iServicesDb, KIptvSmServicesTable, rowset.EReadOnly ) );		
    if ( err != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::GetServicesL(aStartId, aEndId, aResultArray) Can't open service DB, aborting!!!");
        User::Leave( err );
        }
    
    CleanupClosePushL( rowset ); // 2->
    err = rowset.SetIndex( KIptvSmServicesDbIndexName );
    if ( err != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::GetServicesL(aStartId, aEndId, aResultArray) Can't use index, aborting!!!");
        User::Leave( err );
        }

    IPTVLOGSTRING3_LOW_LEVEL("CIptvServiceManager:: startId = %d, endId = %d", aStartId, aEndId);
    CDbColSet* colSet = rowset.ColSetL();
    CleanupStack::PushL( colSet ); // 3->
    
    TInt idColumnNo                   = colSet->ColNo( KIptvSmServicesDbIdCol );
    TInt nameColumnNo                 = colSet->ColNo( KIptvSmServicesDbNameCol );
    TInt addressColumnNo              = colSet->ColNo( KIptvSmServicesDbAddressCol );
    TInt iconPathColumnNo             = colSet->ColNo( KIptvSmServicesDbIconPathCol );
    TInt epgPluginUidColumnNo         = colSet->ColNo( KIptvSmServicesDbEpgPluginUidCol );
    TInt vodPluginUidColumnNo         = colSet->ColNo( KIptvSmServicesDbVodPluginUidCol );
    TInt iptvPluginUidColumnNo        = colSet->ColNo( KIptvSmServicesDbIptvPluginUidCol );
    TInt typeColumnNo                 = colSet->ColNo( KIptvSmServicesDbTypeCol );
    TInt iapListColumnNo              = colSet->ColNo( KIptvSmServicesDbIapListCol );
    TInt descColumnNo                 = colSet->ColNo( KIptvSmServicesDbDescCol );
    TInt flagsColumnNo                = colSet->ColNo( KIptvSmServicesDbFlagsCol );
    TInt applicationUidColumnNo       = colSet->ColNo( KIptvSmServicesDbApplicationUidCol );
    TInt displayOrderColumnNo         = colSet->ColNo( KIptvSmServicesDbDisplayOrderCol );
    TInt providerIdColumnNo           = colSet->ColNo( KIptvSmServicesDbProviderIdCol );
    TInt accountManagementUrlColumnNo = colSet->ColNo( KIptvSmServicesDbAccountManagementUrlCol );
    TInt epgUpdateTimeColumnNo        = colSet->ColNo( KIptvSmServicesDbEpgUpdateTimeCol );
    TInt iapNameColumnNo              = colSet->ColNo( KIptvSmServicesDbIapNameCol );
    TInt searchUrlColumnNo            = colSet->ColNo( KIptvSmServicesDbSearchUrlCol );
    TInt dlNetworklColumnNo           = colSet->ColNo( KIptvSmServicesDbScheduledDlNetworkCol );
    TInt dlTimeColumnNo               = colSet->ColNo( KIptvSmServicesDbScheduledDlTimeCol );
    TInt dlTypeColumnNo               = colSet->ColNo( KIptvSmServicesDbScheduledDlTypeCol );
    TInt dlLastTimeColumnNo           = colSet->ColNo( KIptvSmServicesDbLastScheduledDlTime );
    TInt groupIdColumnNo              = colSet->ColNo( KIptvSmServicesDbGroupIdCol );
    TInt useCatColumnNo               = colSet->ColNo( KIptvSmServicesDbUseCategoryCol );
    TInt uploadColumnNo               = colSet->ColNo( KIptvSmServicesDbUploadProviderCol );
    TInt iconUrlColumnNo              = colSet->ColNo( KIptvSmServicesDbIconUrlCol );
    TInt recordUrlColumnNo            = colSet->ColNo(KIptvSmServicesDbRecordUrlCol);

    CleanupStack::PopAndDestroy( colSet ); // <-3
    CIptvServices* services = CIptvServices::NewL();
    CleanupStack::PushL( services ); // 3->

    CIptvService* service = CIptvService::NewL();
    CleanupStack::PushL( service ); // 4->
	
    for ( i = aStartId; i <= aEndId; i++ )
        {
        seekKey = i;
		
        // Search the index
        if ( rowset.SeekL( seekKey ) )
            {
            TUid uid;

            rowset.GetL();
            service->SetId( rowset.ColUint32( idColumnNo ) );
            service->SetName( rowset.ColDes( nameColumnNo ) );
            service->SetAddress( rowset.ColDes( addressColumnNo ) );
            service->SetIconPath( rowset.ColDes( iconPathColumnNo ) );

      	    uid.iUid = rowset.ColInt32( epgPluginUidColumnNo );
            service->SetEpgPluginUid( uid );
            uid.iUid = rowset.ColInt32( vodPluginUidColumnNo );
            service->SetVodPluginUid( uid );
            uid.iUid = rowset.ColInt32( iptvPluginUidColumnNo );
            service->SetIptvPluginUid( uid );

            service->SetType( ( CIptvService::TServiceType )( rowset.ColUint8( typeColumnNo ) ) );
            
            //set IAP-list
            RDesReadStream readStreamIap;
            readStreamIap.Open(rowset.ColDes8( iapListColumnNo ) );
            CleanupClosePushL( readStreamIap ); // 5->
            service->iIapList->InternalizeL( readStreamIap );
            CleanupStack::PopAndDestroy( &readStreamIap ); // <-5
                        
            TBuf<KIptvSmServicesDbDescMaxLength> desc;
            RDbColReadStream readStream;       // A stream object for long columns
            readStream.OpenLC( rowset,descColumnNo ); // 5->
            readStream.ReadL( desc, rowset.ColLength( descColumnNo ) );
            CleanupStack::PopAndDestroy( &readStream ); // <-5

            service->SetDesc( desc );

            service->SetFlags(rowset.ColUint32( flagsColumnNo ) );
            service->SetApplicationUid( rowset.ColInt32( applicationUidColumnNo ) );
            service->SetDisplayOrder( rowset.ColUint32( displayOrderColumnNo ) );
            service->SetProviderId( rowset.ColDes( providerIdColumnNo ) );
            service->SetAccountManagementUrl( rowset.ColDes( accountManagementUrlColumnNo ) );
            service->SetEpgUpdateTimeL( rowset.ColTime( epgUpdateTimeColumnNo ) );
            service->SetIapNameL( rowset.ColDes( iapNameColumnNo ) );

            delete iSearchUrl;
            iSearchUrl = NULL;
            iSearchUrl = HBufC::NewL( rowset.ColLength( searchUrlColumnNo ) );
            TPtr searchUrlPtr( iSearchUrl->Des() );
            readStream.OpenLC( rowset, searchUrlColumnNo ); // 5->
            readStream.ReadL( searchUrlPtr, rowset.ColLength( searchUrlColumnNo ) );
            CleanupStack::PopAndDestroy( &readStream ); // <-5
            service->SetSearchUrlL( searchUrlPtr );
            delete iSearchUrl;
            iSearchUrl = NULL;
            
            service->SetScheduleDlNetwork( rowset.ColUint32( dlNetworklColumnNo ) );
            service->SetScheduleDlTime( rowset.ColUint32( dlTimeColumnNo ) );
            service->SetScheduleDlType( rowset.ColUint32( dlTypeColumnNo ) );
            service->SetScheduledLastDownloadTime( rowset.ColTime( dlLastTimeColumnNo ) );

            service->SetGroupId( rowset.ColUint32( groupIdColumnNo ) );
            service->SetUseCategory( ( CIptvService::TServiceType )( rowset.ColUint8( useCatColumnNo ) ) );
            service->SetUploadProvider( rowset.ColDes( uploadColumnNo ) );
            service->SetIconUrl( rowset.ColDes( iconUrlColumnNo ) );

            delete iRecordUrl;
            iRecordUrl = NULL;
            iRecordUrl = HBufC::NewL( rowset.ColLength(recordUrlColumnNo) );
            TPtr recordUrlPtr( iRecordUrl->Des() );
            readStream.OpenLC(rowset, recordUrlColumnNo); // 5->
            readStream.ReadL(recordUrlPtr, rowset.ColLength(recordUrlColumnNo));
            CleanupStack::PopAndDestroy(&readStream); // <-5
            service->SetRecordUrlL( recordUrlPtr );
            delete iRecordUrl;
            iRecordUrl = NULL;
            
            //get password and user name from Passwords DB
            TIptvSmPassword password;
            TRAP( err, GetPasswordL( service->GetId(), password ) )
            if ( err != KErrNone )
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: Couldn't get password, using empty descriptor");
                service->SetUserName(KIptvEmptyDes);
                service->SetPassword(KIptvEmptyDes);
                }
            else
                {
                service->SetUserName( password.iUserName );
                service->SetPassword( password.iPassword );
                }
                
            services->AddL( *service );
            }
        else
            {
            /* was not found, that's ok, there might be gaps in indexes */
            }

        if ( i == 0xffffffff )
            {
            //this is necessarry since 0xffffffff + 0x01 = 0x00 -> for loop gets messed.
            break; //break from for()
            }

        } //for()
	
    CleanupStack::PopAndDestroy( service ); // <-4	
    CleanupStack::Pop( services );          // <-3 ownership moves to caller
    CleanupStack::PopAndDestroy( &rowset ); // <-2

	if ( aDoOpenClose )
        {
        CleanupStack::PopAndDestroy( this ); // <-1
        }
            
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::GetServicesUsingIdL(aStartId, aEndId) exit");

    return services;
    }
// ---------------------------------------------------------------------------
// CIptvServiceManager::DeleteServiceL()
//
// Delete a service using id and RDbUpdate (DML)
// ---------------------------------------------------------------------------
TInt CIptvServiceManager::DeleteServiceL( TUint32 aId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::DeleteServiceL()");
    
    OpenL();

    CompactDatabases();
            
    const TInt KIptvSmIdDesMaxSize = 20;
    TBuf<KIptvSmIdDesMaxSize> idDes;
    idDes.Zero();
    idDes.Num(aId);

    IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManager:: idDes = %S", &idDes);
    
    _LIT(KIptvSmSqlDeleteCmd, "DELETE FROM %S WHERE %S = ");
               	
    // Sql: DELETE FROM Services WHERE %S LIKE 'aId'
    TBuf<KIptvSmCustomSqlMaxLength+KIptvSmIdDesMaxSize> sqlStr;
    sqlStr.Format(KIptvSmSqlDeleteCmd, 
                  &KIptvSmServicesTable,
                  &KIptvSmServicesDbIdCol);

    sqlStr.Append(idDes);

    TInt servicesDbDeleteError = ExecuteSqlCmd(sqlStr, iServicesDb);

    sqlStr.Format(KIptvSmSqlDeleteCmd, 
                  &KIptvSmPasswordsTable,
                  &KIptvSmPasswordsDbIdCol);

    sqlStr.Append(idDes);

    TInt passwordsDbDeleteError = ExecuteSqlCmd(sqlStr, iPasswordsDb);
    
    Close();
    
    InformObserversL(CIptvSmEvent::EServiceDeleted, aId, NULL);

    TInt error = KErrNone;
        
    if(servicesDbDeleteError != KErrNone)
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManager:: error in deleting from Services db: %d", servicesDbDeleteError);
        error = servicesDbDeleteError;
        }
    if(passwordsDbDeleteError != KErrNone)
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManager:: error in deleting from Passwords db: %d", passwordsDbDeleteError);
        error = passwordsDbDeleteError;
        }

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::DeleteServiceL() exit");
    return error;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::IsServiceIncludedInSearch()
//
// Decides if the service is included in the search. 
// ---------------------------------------------------------------------------
//
TBool CIptvServiceManager::IsServiceIncludedInSearch(
    TUint32 aSearchLimitFlags,
    CIptvService& aIptvService ) const
    {
        
    if ( aSearchLimitFlags == 0 )
        {
        return ETrue;
        }
        
    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::ESelectedServices ) &&
        !( aIptvService.GetFlags() & CIptvService::ESelected ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EMainServices ) &&
        !( aIptvService.GetFlags() & CIptvService::EMainService ) )
        {
        return EFalse;
        }
            
    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::ESubServices ) &&
         ( aIptvService.GetFlags() & CIptvService::EMainService ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EVod ) &&
         ( aIptvService.GetType() != CIptvService::EVod ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EVodCast ) &&
         ( aIptvService.GetType() != CIptvService::EVodCast ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::ELiveTv ) &&
         ( aIptvService.GetType() != CIptvService::ELiveTv ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EBrowser ) &&
         ( aIptvService.GetType() != CIptvService::EBrowser ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EVideoRemote ) &&
         ( aIptvService.GetType() != CIptvService::EVideoRemote ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EApplication ) &&
         ( aIptvService.GetType() != CIptvService::EApplication ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EMobileTv ) &&
         ( aIptvService.GetType() != CIptvService::EMobileTv ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EVodServices ) &&
         ( aIptvService.GetType() != CIptvService::EVod ) &&
         ( aIptvService.GetType() != CIptvService::EVodCast ) &&
         ( aIptvService.GetType() != CIptvService::EServiceGroup ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EServiceGroup ) &&
         ( aIptvService.GetType() != CIptvService::EServiceGroup ||
           aIptvService.GetGroupId() == 0 ) )
        {
        return EFalse;
        }
    
    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EGroupedServices ) &&
       ( !( aIptvService.GetFlags() & CIptvService::EGroupedService ) ||
           aIptvService.GetGroupId() == 0 ) )
        {
        return EFalse;
        }

    if ( ( aSearchLimitFlags & CIptvServiceManagementClient::EOther ) &&
         ( aIptvService.GetType() != CIptvService::EOther ) )
        {
        return EFalse;
        }
        
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::GetServiceL()
// ---------------------------------------------------------------------------
//
CIptvService* CIptvServiceManager::GetServiceL(
    TUint32 aServiceId,
    TBool aDoOpenClose )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::GetServiceL()");

    if ( aDoOpenClose )
        {
        OpenL();
        CleanupClosePushL( *this ); // 1->
        }
        
    CIptvServices* services = GetServicesUsingIdL( aServiceId, aServiceId, aDoOpenClose );
    
    CIptvService* service = NULL;

    if ( services )
        {
        CleanupStack::PushL( services ); // 2->
        
        if ( services->Count() != 1 )
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: Could not get service data from Service Manager with ID: %x, returning NULL", aServiceId);
            }
        else
            {    
            service = services->GetServiceL( 0 );
            }
        
        CleanupStack::PopAndDestroy( services ); //<-2
        }
        
    if ( aDoOpenClose )
        {
        CleanupStack::PopAndDestroy( this ); //<-1
        }
            
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::GetServiceL() exit");            
    return service;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::RegisterObserver()
// ---------------------------------------------------------------------------
//
TInt CIptvServiceManager::RegisterObserver(MIptvServiceManagerObserver* aObserver)
    {
    return iObservers.Append(aObserver);
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::DeRegisterObserver()
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::DeRegisterObserver(MIptvServiceManagerObserver* aObserver)
    {
    TInt i;
    for(i = 0; i < iObservers.Count(); i++)
        {
        if(aObserver == iObservers[i])
            {
            iObservers.Remove(i);
            return;
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::InformObserversL()
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::InformObserversL(
    CIptvSmEvent::TEvent aEvent,
    TUint32 aServiceId,
    CIptvService* aService)
    {
    CIptvSmEvent* event = CIptvSmEvent::NewL();
    CleanupStack::PushL(event); // 1->

    event->iEvent = aEvent;
    event->iServiceId = aServiceId;
    event->iService = aService;

    TInt i;
    for ( i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->HandleSmEvent( *event );
        }
    CleanupStack::PopAndDestroy( event ); // <-1
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::GetServicesL()
// ---------------------------------------------------------------------------
//
CIptvServices* CIptvServiceManager::GetServicesL(
    const TDesC& aProviderId,
    TBool aDoOpenClose)
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::GetServicesL(aProviderId)");

    if ( aDoOpenClose )
        {
        OpenL();
        CleanupClosePushL( *this ); // 1->
        }
    
    // Create a view on the database    
    RDbView view;

    _LIT(KSelectCmdFormat, "SELECT ID FROM Services WHERE ProviderId = '%S' ORDER BY ID");
    const TInt KSelectCmdFormatLength = 70;
    TBuf<KIptvSmServicesDbProviderIdMaxLength + KSelectCmdFormatLength> selectCmd;
    selectCmd.Format(KSelectCmdFormat, &aProviderId);
    User::LeaveIfError(view.Prepare(iServicesDb, TDbQuery(selectCmd, EDbCompareFolded)));
    User::LeaveIfError(view.EvaluateAll());

    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet); // 2->
    
    CIptvService* service   = NULL;
    CIptvServices* services = CIptvServices::NewL();
    CleanupStack::PushL(services); // 3->
    
    TUint32 id;
    for (view.FirstL(); view.AtRow(); view.NextL())
        {
        view.GetL();
        id = view.ColUint32(colSet->ColNo(KIptvSmServicesDbIdCol));
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: found ID: %d", id);
        service = GetServiceL(id, aDoOpenClose);
        if ( service )
            {
            CleanupStack::PushL(service); // 4->
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: got service for the id.");
            TInt err = services->AddL(*service);
            if(err == KErrNone)
                {
                IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: added to the list");
                }
            else
                {
                IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: failed to add to the list");
                }
            CleanupStack::PopAndDestroy(service); // <-4
            service = NULL;
            }
        else
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: failed to get service for the ID");
            }
        }

    view.Close();

    CleanupStack::Pop(services);         // <-3
    CleanupStack::PopAndDestroy(colSet); // <-2
    
    if (aDoOpenClose)
        {
        CleanupStack::PopAndDestroy(this); // <-1
        }
        
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::GetServicesL(aProviderId) exit");
    return services;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::GetPasswordL()
// Databases are already open when this func is called
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::GetPasswordL( TUint32 aId, TIptvSmPassword& aPassword )
    {    
    RDbTable passwordsTable;
    TDbSeekKey seekKey;
    	
    TInt err = passwordsTable.Open(iPasswordsDb, KIptvSmPasswordsTable, passwordsTable.EReadOnly);
    if(err != KErrNone)
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Could not get passwords table, aborting!");
        User::Leave(KErrGeneral);
        }
    CleanupClosePushL(passwordsTable); // 1->

    err = passwordsTable.SetIndex(KIptvSmPasswordsDbIndexName);

    if(err != KErrNone)
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: could not set index for password table");
        User::Leave(err);
        }

    // Search the index
    seekKey = (TUint)aId;
    if(!passwordsTable.SeekL(seekKey))
        {
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: could not find id %d from passwords table", aId);
        User::Leave(KErrNotFound);
        }

    passwordsTable.GetL();

    CDbColSet* colSet = passwordsTable.ColSetL();
    CleanupStack::PushL(colSet); // 2->
    
    aPassword.iUserName = passwordsTable.ColDes(colSet->ColNo(KIptvSmPasswordsDbUserNameCol));
    aPassword.iPassword = passwordsTable.ColDes(colSet->ColNo(KIptvSmPasswordsDbPasswordCol));
    
    CleanupStack::PopAndDestroy(colSet);          // <-2
    CleanupStack::PopAndDestroy(&passwordsTable); // <-1    
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::ExecuteSqlCmd()
// ---------------------------------------------------------------------------
//
TInt CIptvServiceManager::ExecuteSqlCmd(const TDesC& aSqlCmd, RDbStoreDatabase& aDb)
    {    
    // Initialize execution and perform the first step.
    // Note: Execute() returns 0 (=KErrNone), but it does not affect database
    //       until Next() is called.
    RDbUpdate updOp;
    TInt incStat = updOp.Execute(aDb, aSqlCmd, EDbCompareFolded);
    incStat = updOp.Next(); // This will leave, if Execute() failed.

    while( incStat == 1 ) // Just in case, if the operation has more steps
        {
        incStat = updOp.Next();
        }
    updOp.RowCount();
    updOp.Close();
    return incStat;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::ExecuteProvisioningL()
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::ExecuteProvisioningL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: doing provisioning");

    TFileName servicesXmlFile;
    TBool foundFile = EFalse;
    TBool updateFound = EFalse;

    //  Check if there is update file
    foundFile = IsServiceUpdateFoundL(servicesXmlFile);
    updateFound = foundFile;

    if (!foundFile)
        {
        //  Update file not found, look service file from c-disk
        servicesXmlFile.Zero();
        servicesXmlFile.Append(KIptvSmCDiskPrefix);
        servicesXmlFile.Append(iPrivatePath);
        servicesXmlFile.Append(KIptvSmServicesXmlFile);
                
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: looking xml-file from c-disk");
        foundFile = BaflUtils::FileExists(iFsSession, servicesXmlFile);
        }

    if (!foundFile)
        {
        //  Wasn't on the c-disk, look service file from z-disk
        servicesXmlFile.Zero();
        servicesXmlFile.Append(KIptvSmZDiskPrefix);
        servicesXmlFile.Append(iPrivatePath);
        servicesXmlFile.Append(KIptvSmServicesXmlFile);
                
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: looking xml-file from z-disk");
        foundFile = BaflUtils::FileExists(iFsSession, servicesXmlFile);
        }

    if (foundFile)
        {
        //  Some file found, provision it
        DoProvisioningL(servicesXmlFile);

        //  Was it update file
        if (updateFound)
            {
            //  Yes, remove it
            iFsSession.Delete(servicesXmlFile);
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::IsServiceUpdateFoundL()
// ---------------------------------------------------------------------------
//
TBool CIptvServiceManager::IsServiceUpdateFoundL(TFileName& aServicesXmlFile)
    {
    //  Look update file from c-disk
    aServicesXmlFile.Zero();
    aServicesXmlFile.Append(KIptvSmCDiskPrefix);
    aServicesXmlFile.Append(iPrivatePath);
    aServicesXmlFile.Append(KIptvSmServicesUpdateXmlFile);
            
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: looking update xml-file from c-disk");

    return BaflUtils::FileExists(iFsSession, aServicesXmlFile);
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::DoProvisioningL()
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::DoProvisioningL(const TDesC& aServicesXmlFile)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::DoProvisioning()");

    CIptvProvisioningProcessor* provisioningProcessor = CIptvProvisioningProcessor::NewL();
    CleanupStack::PushL(provisioningProcessor); // 1->

    CIptvServices* services = NULL;
    TInt provisionError( KErrNone );
    TRAPD(error, services = provisioningProcessor->LocalProvisionL(aServicesXmlFile, provisionError));
    
    if ( error == KErrNone && provisionError == KErrNone && services )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServiceManager:: services got from xml-file: %d", services->Count());

        CleanupStack::PushL(services); // 2->
        TInt i;
        for(i = 0; i < services->Count(); i++)
            {
            AddServiceL(services->GetServiceRefL(i), EFalse /* dont do open/close */);
            }
        CleanupStack::PopAndDestroy(services); // <-2
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager:: error while executing LocalProvisionL()");
        }

    CleanupStack::PopAndDestroy(provisioningProcessor); // <-1

    IPTVLOGSTRING_LOW_LEVEL("CIptvServiceManager::DoProvisioning() exit");
    }
    
// ---------------------------------------------------------------------------
// CIptvServiceManager::CreateWritableRootDirL
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::CreateWritableRootDirL(TDriveNumber aDrive)
    {
    //Create private writable root path to aDrive if it does not exist yet
    TFileName rootPath;
    TRAPD( err,
           CIptvUtil::GetPathL( iFsSession, EIptvPathWritableRoot, rootPath, aDrive ) );
		
    if ( err != KErrNone ) 
        {
        if ( aDrive == EDriveC )
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Can't create writable root path to C:, panicing");	
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: system wide error code = %d", err);	
            // This is fatal, we need this folder
            User::Panic(KIptvServiceManager, EIptvSmCouldNotCreateWritableRootPath);
            }
        else
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: Can't create writable root path to E:, skipping");
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::CheckSmDbVersionL
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::CheckSmDbVersionL(void)
    {         
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CheckSmDbVersionL() start");

    TFileName smDbVersionFileName;
    
    smDbVersionFileName = KIptvSmCDiskPrefix;			
    smDbVersionFileName.Append(iPrivatePath);
    smDbVersionFileName.Append(KSmDbVersionFileName);

    TUint32 dbVersion = 0;
    RFile versionFile;

    TInt err = versionFile.Open(iFsSession, smDbVersionFileName, EFileRead);
    if (err == KErrNone)
        {
        const TInt KVersionLength = 4;
        
        TBuf8<KVersionLength> data;
        err = versionFile.Read(data, KVersionLength);
        versionFile.Close();
        
        if ( data.Length() == KVersionLength  && err == KErrNone )
            {
            dbVersion |= (data[0] << 24);
            dbVersion |= (data[1] << 16);
            dbVersion |= (data[2] << 8);
            dbVersion |= data[3];
            }
        else
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: version file is corrupted -> 0 version assumed (= unknown format), err: %d", err);
            }
        }
    
    if ( dbVersion != 0 )
        {
        if ( dbVersion != KIptvSmDbVersion )
            {
            IPTVLOGSTRING3_HIGH_LEVEL("CIptvServiceManager:: db is version %d and sw is version %d -> doing db conversion!", dbVersion, KIptvSmDbVersion);
            ConvertDatabaseL( dbVersion );
            }
        else
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: Both databases and sw are version %d", dbVersion);
            }
        }
    else 
        {
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: db is version 0 (= unknown format), sw is version %d, deleting old databases.", KIptvSmDbVersion);
        iFsSession.Delete(iServicesFile);
        iFsSession.Delete(iPasswordsFile);
        }
        
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CheckSmDbVersionL() exit");
    }        

// ---------------------------------------------------------------------------
// CIptvServiceManager::SaveSmDbVersion
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::SaveSmDbVersion(void)
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::SaveSmDbVersion() start");

    TFileName smDbVersionFileName;
    
    smDbVersionFileName = KIptvSmCDiskPrefix;			
    smDbVersionFileName.Append(iPrivatePath);
    smDbVersionFileName.Append(KSmDbVersionFileName);

    RFile versionFile;
    TInt err = versionFile.Replace(iFsSession, smDbVersionFileName, EFileWrite);
    if (err == KErrNone)
        {
        TBuf8<4> versionDes;
        versionDes.Append( static_cast<TUint8>( ( KIptvSmDbVersion >> 24 ) & 0x000000ff ) );
        versionDes.Append( static_cast<TUint8>( ( KIptvSmDbVersion >> 16 ) & 0x000000ff ) );
        versionDes.Append( static_cast<TUint8>( ( KIptvSmDbVersion >> 8 ) & 0x000000ff ) );
        versionDes.Append( static_cast<TUint8>( KIptvSmDbVersion & 0x000000ff ) );
        err = versionFile.Write(versionDes);
        versionFile.Close();
        if ( err == KErrNone )
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: version saved");    
            }
        else
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: version file write FAILED: %d", err );
            }
        
        }
    else
        {
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: couldn't create %S file!", &KSmDbVersionFileName );
        }
        
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::SaveSmDbVersion() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::ConvertDatabaseL
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::ConvertDatabaseL( TInt aOldVersion )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::ConvertDatabaseL() start");

    //read old services
    CIptvServices* oldDbServices = GetServicesL(
        0 /* 0 = all services */,
        CIptvServiceManagementClient::EDateAscending,
        aOldVersion );
    CleanupStack::PushL( oldDbServices ); // 1->

    //delete old db
    iFsSession.Delete( iServicesFile );
    iFsSession.Delete( iPasswordsFile );

    //create empty db files   
    CreateFilesL( EFalse /* dont do provisioning */);
    
    //open db, does not execute provisioning since files are already created
    OpenL();
    CleanupClosePushL( *this ); // 2->

    //write to new db
    IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: %d services found for conversion", oldDbServices->Count());

    for ( TInt i( 0 ); i < oldDbServices->Count(); i++ )
        {
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager:: converting %S", &(oldDbServices->GetServiceRefL(i).GetName()));
        AddServiceL(oldDbServices->GetServiceRefL(i), EFalse /* dont do open/close */);
        }
    
    ExecuteProvisioningL();
    
    CleanupStack::PopAndDestroy( this ); // <-2
    CleanupStack::PopAndDestroy( oldDbServices ); // <-1
         
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::ConvertDatabaseL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::AddService
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::AddServiceL( 
    CIptvService& aService,
    TBool aDoOpenClose )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::AddServiceL() start");

    if ( aDoOpenClose )
        {
        OpenL();
        CleanupClosePushL( *this ); // 1->
        }
        
    //find out mode (update or add)
    TUint32 mode = 0;
    if ( aService.GetProviderId() != KIptvEmptyDes )
        {
        CIptvServices* servicesAlready = GetServicesL(aService.GetProviderId(), EFalse /* Dont do open & close */);
        CleanupStack::PushL(servicesAlready); // 2->
        if ( servicesAlready->Count() > 0 )
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: found service with same provider id, using update mode");
            mode = 0; //0 = update
            aService.SetId(servicesAlready->GetServiceRefL(0).GetId()); //set id to correct value
            }
        else
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: provider id was unique, using add mode");
            mode = EAddService;
            }
        CleanupStack::PopAndDestroy( servicesAlready ); // <-2
        }
    else
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: provider id was empty, using add mode");
        mode = EAddService;
        }
                
    mode |= EDontOpen;
    
    AddOrUpdateServiceL( aService, mode );

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    // Provision Video Store service to Active Space.
    if ( ( aService.GetName().CompareF( KQtnIptvVideoStoreList ) == 0 ) &&
         ( aService.GetType() == CIptvService::EBrowser ) )
        {
        // Note; missing Active Space helper or leave in Active Space must
        // not interrupt finishing of for example factory provisioning.
        TRAP_IGNORE( UpdateVideoStoreSpaceDbL( aService ) );
        }
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    if ( aDoOpenClose )
        {
        CleanupStack::PopAndDestroy( this ); // <-1
        }
        
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::AddServiceL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::UpdateService
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::UpdateServiceL( CIptvService& aService )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::UpdateServiceL() start");

    AddOrUpdateServiceL( aService, 0 /* 0 = update */ );

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    // Provision Video Store service to Active Space.
    if ( ( aService.GetName().CompareF( KQtnIptvVideoStoreList ) == 0 ) &&
         ( aService.GetType() == CIptvService::EBrowser ) )
        {
        TRAP_IGNORE( UpdateVideoStoreSpaceDbL( aService ) );
        }
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::UpdateServiceL() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::CompactDatabases
// ---------------------------------------------------------------------------
//
void CIptvServiceManager::CompactDatabases()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CompactDatabases() start");

    if ( iCompactCounter == 0 )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager:: running compact for databases");
        iServicesDb.Compact();
        iPasswordsDb.Compact();
        }

    iCompactCounter++;

    if ( iCompactCounter > KIptvSmServiceDbCompactingFrequency )
        {
        iCompactCounter = 0;
        }

    IPTVLOGSTRING_HIGH_LEVEL("CIptvServiceManager::CompactDatabases() exit");
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::MaxDisplayOrderL
// ---------------------------------------------------------------------------
//
TUint32 CIptvServiceManager::MaxDisplayOrderL()
    {
    TUint32 maxDisplayOrder( 0 );

    // Query "SELECT DisplayOrder FROM Services ORDER BY DisplayOrder DESC" to
    // find highest DisplayOrder for moving updated service to top of list
    _LIT( KMaxDisplayOrderQueryFormat, "SELECT %S FROM %S ORDER BY %S DESC" );
    HBufC* maxDisplayOrderQueryBuf = HBufC::NewLC(
            KMaxDisplayOrderQueryFormat().Length()
            + KIptvSmServicesDbDisplayOrderCol().Length()
            + KIptvSmServicesTable().Length()
            + KIptvSmServicesDbDisplayOrderCol().Length() );
    maxDisplayOrderQueryBuf->Des().Format(
            KMaxDisplayOrderQueryFormat,
            &KIptvSmServicesDbDisplayOrderCol,
            &KIptvSmServicesTable,
            &KIptvSmServicesDbDisplayOrderCol );

    IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager::MaxDisplayOrder() query: %S", maxDisplayOrderQueryBuf);
    
    RDbView servicesView;
    User::LeaveIfError( servicesView.Prepare(
            iServicesDb,
            TDbQuery( *maxDisplayOrderQueryBuf ) ) );
    CleanupClosePushL( servicesView );
    User::LeaveIfError( servicesView.EvaluateAll() );
    
    // First result has the highest DisplayOrder value
    if ( servicesView.FirstL() )
        {
        servicesView.GetL();
        maxDisplayOrder = servicesView.ColUint32( 1 );
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvServiceManager::MaxDisplayOrder() maxDisplayOrder: %d", maxDisplayOrder);
        }

    CleanupStack::PopAndDestroy( &servicesView );
    CleanupStack::PopAndDestroy( maxDisplayOrderQueryBuf );

    return maxDisplayOrder;
    }
