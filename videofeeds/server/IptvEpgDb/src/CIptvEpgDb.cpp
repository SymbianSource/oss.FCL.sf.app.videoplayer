/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Offers interface to Epg database*
*/




#include <badesca.h>    // CDesCArrayFlat
#include <s32file.h>    // CFileStore & CPermanentFileStore
#include <bautils.h>    // file helpers
#include <eikenv.h>
#include <sysutil.h>    // SysUtil
#include <centralrepository.h>

#include "IptvLiveDataStructures.h" // CleanupResetAndDestroyPushL
#include "CIptvMediaContent.h"
#include "CIptvEpgCategory.h"
#include "CIptvEpgContent.h"
#include "CIptvUtil.h"
#include "IptvDebug.h"

#include "CIptvServer.h"
#include "CIptvService.h"
#include "CIptvEpgSession.h"
#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvEpgDb.h"
#include "IptvServer.pan"
#include "CIptvServiceManager.h"

_LIT( KSqlAll, "*" );
_LIT( KHyp, "'" );
_LIT( KSqlSelect, "SELECT " );
_LIT( KSqlDot, ", " );
_LIT( KSqlFrom, " FROM " );
_LIT( KSqlWhere, " WHERE " );
_LIT( KSqlIs, " = " );
_LIT( KSqlAnd, " AND " );
_LIT( KSqlNot, " <> " );
_LIT( KSqlOr, " OR " );
_LIT( KSqlOrderBy, " ORDER BY " );
_LIT( KIptvTmp, ".tmp" );
_LIT( KEmptyString, "" );

const TInt KIptvDbVersion( 9 );

const TUint32 KIptvRootParent( 0xFFFFFFF );
const TInt KMaxCountOfFullDetails( 1 );
const TInt KMaxCountOfParentCategories( 1 );
const TInt KIptvFileId( 0 );
const TInt KIptvDrive( 0 );
const TInt KValueNotSet( -1 );

const TInt KIptvEpgManagerFreeSpace = 512000; // 500 KB

const TInt KParentalControlOff( 0 );

const TIptvContentId KIptvInvalidContentId(
    static_cast<TIptvContentId>( KErrNotFound ) );

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CIptvEpgDb::CIptvEpgDb
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CIptvEpgDb::CIptvEpgDb(
    const TUint32& aServiceId,
    CIptvServer& aServer ) :
    iTmpDbOpen( EFalse ),
    iServiceId( aServiceId ),
    iServer( aServer ),
    iParentalControl( KParentalControlOff )
    {
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::ConstructL
// Second phase construction. Leaves, if RFs session cannot be created.
// --------------------------------------------------------------------------
//
void CIptvEpgDb::ConstructL( const TFileName& aEpgFile )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::ConstructL" );

    User::LeaveIfError( iFsSession.Connect() );
    iEpgFile = aEpgFile;

    MakeSqlStrings();
    TBool created = EFalse;
    CreateDbL( iEpgFile, created );
    OpenDatabaseL( iEpgDb, iEpgFile );

    // If new database created -> Set service information
    if ( created )
        {
        TTime a( TInt64( 0 ) );
        SetServiceInformationL( iEpgDb, a, a );
        CreateRootCategoryL( iEpgDb );
        created = EFalse;
        }

    // Read versio information from database
    iEpgDb.Compact();
    TInt version = 0;
    version = GetVersionL();
    if ( version != KIptvDbVersion )
        {
        iEpgDb.Close();
        // Delete old database.
        CFileMan* fileMan = CFileMan::NewL( iFsSession );
        CleanupStack::PushL( fileMan );
        fileMan->Delete( iEpgFile );
        CleanupStack::PopAndDestroy( fileMan );

        // Create new database with current version of database tables.
        CreateDbL( iEpgFile, created );
        OpenDatabaseL( iEpgDb, iEpgFile );

        // If new database created -> Set service information
        if ( created )
            {
            TTime a( TInt64( 0 ) );
            SetServiceInformationL( iEpgDb, a, a );
            CreateRootCategoryL( iEpgDb );
            }
        }

    TRAPD( ret, iCenRepSession = CRepository::NewL( KIptvCenRepUid ) );

    if ( ret == KErrNone && iCenRepSession )
        {
        // Register CenRep change notifier.
        iCenRepNotifyHandler = CCenRepNotifyHandler::NewL(
            *this,
            *iCenRepSession,
            CCenRepNotifyHandler::EIntKey,
            KIptvCenRepParentControlKey );
        iCenRepNotifyHandler->StartListeningL();

        // Read settings from CenRep.
        ReadCenRepData();
        }
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::GetVersionL()
//
// Get database version
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetVersionL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetVersionL" );

    TInt version = 0;

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KSqlAll );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgServiceTable );

    RDbView view;
    TBool found = EFalse;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        TInt count = view.ColCount();
        if ( count > 0 )
            {
            for ( TInt i = 1; i <= count; i++ )
                {
                TDbCol col = view.ColDef( i );
                if ( col.iName.Compare( KIptvEpgServiceVersion ) == 0 )
                    {
                    found = ETrue;
                    }
                }
            }
        }

    if ( found )
        {
        view.EvaluateAll();
        view.FirstL();
        CDbColSet* colSet = view.ColSetL();
        CleanupStack::PushL( colSet );
        TInt versionCol = colSet->ColNo( KIptvEpgServiceVersion );
        CleanupStack::PopAndDestroy( colSet );

        TRAPD( getError, view.GetL() );
        if ( getError != KErrNone )
            {
            view.Close();
            }

        version = view.ColUint32( versionCol );
        }

    view.Close();
    return version;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::OpenDatabaseL()
//
// Open database
// --------------------------------------------------------------------------
//
void CIptvEpgDb::OpenDatabaseL( RDbNamedDatabase& aDb, const TDesC& aFile )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::OpenDatabaseL" );

    TInt error = aDb.Open( iFsSession, aFile );
    if ( error != KErrNone )
        {
        if ( error != KErrNoMemory &&
             error != KErrLocked &&
             error != KErrDisMounted &&
             error != KErrDiskFull &&
             error != KErrNotReady )
            {
            // Delete the database file. Cannot recover.
            CFileMan* fileMan = CFileMan::NewL( iFsSession );
            CleanupStack::PushL( fileMan );
            fileMan->Delete( aFile );
            CleanupStack::PopAndDestroy( fileMan );
            User::Leave( error );
            }
        else
            {
            User::Leave( error );
            }
        }
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CIptvEpgDb* CIptvEpgDb::NewL(
    const TFileName& aEpgFile,
    const TUint32& aServiceId,
    CIptvServer& aServer )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::NewL" );

    CIptvEpgDb* self = new ( ELeave ) CIptvEpgDb( aServiceId, aServer );
    CleanupStack::PushL( self );
    self->ConstructL( aEpgFile );
    CleanupStack::Pop( self );

    return self;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::~CIptvEpgDb()
// Destructor
// --------------------------------------------------------------------------
//
CIptvEpgDb::~CIptvEpgDb()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::~CIptvEpgDb" );

    iEpgDb.Close();
    if ( iDeleteDatabaseFiles )
        {
        TRAP_IGNORE( DeleteDatabaseL() )
        }

    if ( iTmpDbOpen )
        {
        iTmpDb.Close();
        }

    iFsSession.Close();

    if ( iCenRepNotifyHandler )
        {
        iCenRepNotifyHandler->StopListening();
        delete iCenRepNotifyHandler;
        }

    delete iCenRepSession;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateDbL()
//
// Create a new database.
// --------------------------------------------------------------------------
//
void CIptvEpgDb::CreateDbL( const TDesC& aFile, TBool& aCreated )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateDbL" );
	
    if ( !BaflUtils::FileExists( iFsSession, aFile ) )
        {
        // Before creating any new databases, check that there is enough disk space.
        TBool checkResult = EFalse;
        TRAPD(
            checkError, checkResult = SysUtil::DiskSpaceBelowCriticalLevelL(
                &iFsSession, KIptvEpgManagerFreeSpace, EDriveC ) );
        if ( checkError != KErrNone || checkResult )
            {
            // Not enough space on disk, return immediately and do not create databases.
            User::Leave( KErrDiskFull );
            }

        TRAPD( error,
            {
            RDbNamedDatabase database;
            User::LeaveIfError( database.Create( iFsSession, aFile ) );
            CleanupClosePushL( database );

            CreateCategoryTableL( database );
            CreateCategoryIndexL( database );

            CreateContentTableL( database );
            CreateContentIndexL( database );

            CreateCategoryContentTableL( database );
            CreateCategoryContentIndexL( database );

            CreateServiceTableL( database );
            CreateContentAccessTableL( database );
            CleanupStack::PopAndDestroy(); // database
            aCreated = ETrue;
            } );

        if ( error != KErrNone )
            {
            // Delete the database file. Cannot recover.
            CFileMan* fileMan = CFileMan::NewL( iFsSession );
            CleanupStack::PushL( fileMan );
            fileMan->Delete( aFile );
            CleanupStack::PopAndDestroy( fileMan );
            IPTVLOGSTRING_LOW_LEVEL(
                "CIptvEpgDb::CreateDbL --- PANIC --- Failed to create epg database" );
            User::Panic( KIptvEpgDb, EFailedToCreateEpgDatabase );
            }
        }
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateRootCategoryL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::CreateRootCategoryL( RDbNamedDatabase& aDb ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateRootCategoryL" );

    CIptvEpgCategory* root = CIptvEpgCategory::NewL();
    root->iName = KEmptyString;
    root->iParentKey = KIptvRootParent;
    CleanupStack::PushL( root );
    TUint32 key;
    AddCategoryL( aDb, *root, key );
    if ( key != KIptvVodContentCategoryRootId )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CIptvEpgDb::CreateRootCategoryL --- PANIC --- Failed to create root category" );
        User::Panic( KIptvEpgDb, EIptvFailedToCreateRootCategory );
        }

    CleanupStack::PopAndDestroy( root );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateContentAccessTableL()
//
// Creates ContentAccess table. Leaves, if the table cannot be created.
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::CreateContentAccessTableL(
    RDbNamedDatabase& aDatabase ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateContentAccessTableL" );

    // Content key
    TDbCol contentKeyCol(
        KIptvCAContentKey, EDbColUint32 );

    // Content URL
    TDbCol contentUrlCol(
        KIptvCAContentUrl, EDbColLongText, KIptvCAContentUrlLength );

    // Download type
    TDbCol dlTypeCol( KIptvCADlType, EDbColUint32 );

    // Language
    TDbCol languageCol( KIptvCALanguage, EDbColText, KIptvCALanguageLength );

    // Price
    TDbCol priceCol( KIptvCAPrice, EDbColText, KIptvCAPriceLength );

    // Purchase type
    TDbCol purchaceTypeCol( KIptvCAPurchaseType, EDbColUint32 );

    // Purchase URL
    TDbCol purchaseUrlCol(
        KIptvCAPurchaseUrl, EDbColLongText, KIptvCAPurchaseUrlLength );

    // Last play position
    TDbCol lastPlayPositionCol( KIptvCALastPlayPosition, EDbColUint32 );

    // Mime type
    TDbCol mimeTypeCol( KIptvCAMimeType, EDbColText, KIptvCAMimeTypeLength );

    // FileSize
    TDbCol fileSizeCol( KIptvCAFileSize, EDbColUint32 );

    // Duration
    TDbCol durationCol( KIptvCADuration, EDbColUint32 );

    // File id
    TDbCol fileIdCol( KIptvCAFileId, EDbColUint32 );

    // Drive id
    TDbCol driveIdCol( KIptvCADriveId, EDbColUint32 );

    // Index
    TDbCol indexCol( KIptvCAIndex, EDbColUint32 );

    // Expression
    TDbCol expressionCol( KIptvCAExpression, EDbColUint32 );

    CDbColSet* colSet = CDbColSet::NewLC();
    colSet->AddL( contentKeyCol );
    colSet->AddL( contentUrlCol );
    colSet->AddL( dlTypeCol );
    colSet->AddL( languageCol );
    colSet->AddL( priceCol );
    colSet->AddL( purchaceTypeCol );
    colSet->AddL( purchaseUrlCol );
    colSet->AddL( lastPlayPositionCol );
    colSet->AddL( mimeTypeCol );
    colSet->AddL( fileSizeCol );
    colSet->AddL( durationCol );
    colSet->AddL( fileIdCol );
    colSet->AddL( driveIdCol );
    colSet->AddL( indexCol );
    colSet->AddL( expressionCol );

    TInt error = aDatabase.CreateTable( KIptvCATable, *colSet );
    CleanupStack::PopAndDestroy( colSet );

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateServiceTableL()
//
// Creates Service table. Leaves, if the table cannot be created.
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::CreateServiceTableL( RDbNamedDatabase& aDatabase ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateServiceTableL" );

    // Last update key
    TDbCol lastUpdateKeyCol( KIptvEpgServiceLastUpdate, EDbColDateTime );

    // Next update
    TDbCol nextUpdateKeyCol( KIptvEpgServiceNextUpdate, EDbColDateTime );

    // ETag
    TDbCol eTagCol( KIptvEpgServiceETag, EDbColText, KIptvEpgDbLastModifiedMaxLength );

    // LastModifiedSince
    TDbCol lastModifiedSinceCol(KIptvEpgServiceLastModifiedSince, EDbColText, KIptvEpgDbLastModifiedMaxLength);

    // Hash string from service XML file
    TDbCol xmlFileHashCol( KIptvEpgServiceXmlFileHash, EDbColText, KIptvEpgDbXmlFileHashMaxLength );

    // Version
    TDbCol versionKeyCol( KIptvEpgServiceVersion, EDbColUint32 );

    CDbColSet* colSet = CDbColSet::NewLC();
    colSet->AddL( lastUpdateKeyCol );
    colSet->AddL( nextUpdateKeyCol );
    colSet->AddL( eTagCol );
    colSet->AddL( lastModifiedSinceCol );
    colSet->AddL( xmlFileHashCol );
    colSet->AddL( versionKeyCol );

    TInt error = aDatabase.CreateTable( KIptvEpgServiceTable, *colSet );
    CleanupStack::PopAndDestroy( colSet );
    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateCategoryContentTableL()
//
// Creates CategoryContent table. Leaves, if the table cannot be created.
// --------------------------------------------------------------------------
//
void CIptvEpgDb::CreateCategoryContentTableL(
    RDbNamedDatabase& aDatabase ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateCategoryContentTableL" );

    // Category key
    TDbCol categoryKeyCol( KIptvEpgCategoryContentCategoryKeyCol, EDbColUint32 );

    // Content key
    TDbCol contentKeyCol( KIptvEpgCategoryContentContentKeyCol, EDbColUint32 );

    CDbColSet* colSet = CDbColSet::NewLC();
    colSet->AddL( categoryKeyCol );
    colSet->AddL( contentKeyCol );

    TInt error = aDatabase.CreateTable( KIptvEpgCategoryContentTable, *colSet );
    if ( error != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvEpgDb::CreateCategoryContentTableL error = %d", error );
        }

    CleanupStack::PopAndDestroy( colSet );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateContentTableL()
//
// Creates Content table. Leaves, if the table cannot be created.
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::CreateContentTableL( RDbNamedDatabase& aDatabase ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateContentTableL" );

    // Key
    TDbCol keyCol( KIptvEpgContentDbKeyCol, EDbColUint32 );
    keyCol.iAttributes = TDbCol::EAutoIncrement;

    // Id
    TDbCol idCol( KIptvEpgContentDbIdCol, EDbColText, KIptvIdMaxLength );

    // Name
    TDbCol nameCol(
        KIptvEpgContentDbNameCol, EDbColText, KIptvEpgContentNameMaxLength );

    // File Id
    TDbCol fileIdCol(
        KIptvEpgContentDbFileIdCol, EDbColUint32 );

    // Drive
    TDbCol driveCol( KIptvEpgContentDbDriveIdCol, EDbColUint32 );

    // Preview file id
    TDbCol pFileIdCol( KIptvEpgContentDbPreviewFileIdCol, EDbColUint32 );

    // Preview drive id
    TDbCol pDriveId( KIptvEpgContentDbPreviewDriveIdCol, EDbColUint32 );

    // Content protection
    TDbCol contentProtectionCol(
        KIptvEpgContentDbContentProtectionCol, EDbColUint32 );

    // Icon file Id
    TDbCol iconFileIdCol( KIptvEpgContentDbIconFileIdCol, EDbColUint32 );

    // IsIconFilePathUrl
    TDbCol isIconFilePathUrlCol(
        KIptvEpgContentDbIsIconFilePathUrlCol, EDbColUint32 );

    // Icon file path (full path + filename)
    TDbCol iconFilePathCol(
        KIptvEpgContentDbIconFilePathCol,
        EDbColText,
        KIptvEpgContentIconFilePathMaxLength );

    // Content file Id
    TDbCol contentFileIdCol(
        KIptvEpgContentDbContentFileIdCol, EDbColUint32 );

    // Description
    TDbCol descriptionCol(
        KIptvEpgContentDbDescriptionCol,
        EDbColText,
        KIptvEpgContentDescriptionMaxLength );

    // PubDate
    TDbCol pubDateCol( KIptvEpgContentDbPubDateCol, EDbColDateTime );

    // Browser url
    TDbCol browserUrlCol(
        KIptvEpgContentDbBrowserUrlCol,
        EDbColLongText,
        KIptvEpgContentBrowserUrlMaxLength );

    // Order
    TDbCol orderCol( KIptvEpgContentDbOrderCol, EDbColUint32 );

    // Author
    TDbCol authorCol(
        KIptvEpgContentDbAuthorCol,
        EDbColText,
        KIptvEpgContentAuthorLength );

    // Copyright
    TDbCol copyrightCol(
        KIptvEpgContentDbCopyrightCol,
        EDbColText,
        KIptvEpgContentCopyrightLength );

    // Size
    TDbCol sizeCol( KIptvEpgContentDbSizeCol, EDbColUint32 );

    // Duration
    TDbCol durationCol( KIptvEpgContentDbDurationCol, EDbColUint32 );

    // Language
    TDbCol languageCol(
        KIptvEpgContentDbLanguageCol, EDbColText, KIptvCALanguageLength );

    // RatingAge
    TDbCol ratingAgeCol(
        KIptvEpgContentDbRatingAgeCol, EDbColUint16 );

    CDbColSet* contentColSet = CDbColSet::NewLC();
    contentColSet->AddL( keyCol );
    contentColSet->AddL( idCol );
    contentColSet->AddL( nameCol );
    contentColSet->AddL( fileIdCol );
    contentColSet->AddL( driveCol );
    contentColSet->AddL( pFileIdCol );
    contentColSet->AddL( pDriveId );
    contentColSet->AddL( contentProtectionCol );
    contentColSet->AddL( iconFileIdCol );
    contentColSet->AddL( isIconFilePathUrlCol );
    contentColSet->AddL( iconFilePathCol );
    contentColSet->AddL( contentFileIdCol );
    contentColSet->AddL( descriptionCol );
    contentColSet->AddL( pubDateCol );
    contentColSet->AddL( browserUrlCol );
    contentColSet->AddL( orderCol );
    contentColSet->AddL( authorCol );
    contentColSet->AddL( copyrightCol );
    contentColSet->AddL( sizeCol );
    contentColSet->AddL( durationCol );
    contentColSet->AddL( languageCol );
    contentColSet->AddL( ratingAgeCol );

    TInt error =
        aDatabase.CreateTable( KIptvEpgContentTable, *contentColSet );
    CleanupStack::PopAndDestroy( contentColSet );

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateContentIndexL()
//
// Creates an index for Content table.
// Leaves, if the index cannot be created.
// --------------------------------------------------------------------------
//
void CIptvEpgDb::CreateContentIndexL( RDbNamedDatabase& aDatabase ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateContentIndexL" );

    TDbKeyCol keyCol( KIptvEpgContentDbKeyCol );
    CDbKey* index = CDbKey::NewLC();
    index->AddL( keyCol );
    index->MakeUnique();
    User::LeaveIfError( aDatabase.CreateIndex(
        KIptvEpgContentDbIndexName, KIptvEpgContentTable, *index ) );
    CleanupStack::PopAndDestroy( index );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateCategoryContentIndexL()
//
// Creates an index for CategoryContent table.
// Leaves, if the index cannot be created.
// --------------------------------------------------------------------------
//
void CIptvEpgDb::CreateCategoryContentIndexL(
    RDbNamedDatabase& aDatabase ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateCategoryContentIndexL" );

    TDbKeyCol catCol( KIptvEpgCategoryContentCategoryKeyCol );
    TDbKeyCol contCol( KIptvEpgCategoryContentContentKeyCol );

    CDbKey* index = CDbKey::NewLC();
    index->AddL( contCol );
    index->AddL( catCol );
    index->MakeUnique();
    User::LeaveIfError( aDatabase.CreateIndex(
        KIptvEpgCategoryContentDbIndexName,
        KIptvEpgCategoryContentTable, *index ) );
    CleanupStack::PopAndDestroy( index );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateCategoryTableL()
//
// Creates Category table. Leaves, if the table cannot be created.
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::CreateCategoryTableL( RDbNamedDatabase& aDatabase ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateCategoryTableL" );

    // Key
    TDbCol keyCol( KIptvEpgCategoryDbKeyCol, EDbColUint32 );
    keyCol.iAttributes = TDbCol::EAutoIncrement;

    // Parent key
    TDbCol parentKeyCol( KIptvEpgCategoryDbParentKeyCol, EDbColUint32 );

    // Id
    TDbCol idCol( KIptvEpgCategoryDbIdCol, EDbColText, KIptvIdMaxLength );

    // Name
    TDbCol nameCol( KIptvEpgCategoryDbNameCol, EDbColText,
        KIptvEpgCategoryNameMaxLength );

    // Icon file Id - Obsolete field
    TDbCol iconFileIdCol( KIptvEpgCategoryDbIconFileIdCol, EDbColUint32 );

    // IsIconFilePathUrl - Obsolete field
    TDbCol isIconFilePathUrlCol( KIptvEpgCategoryDbIsIconFilePathUrlCol,
        EDbColUint32 );

    // Icon file path - Obsolete field
    TDbCol iconFilePathCol( KIptvEpgCategoryDbIconFilePathCol, EDbColText,
        KIptvEpgCategoryIconFilePathMaxLength );

    // PubDate - Obsolete field
    TDbCol pubDateCol( KIptvEpgCategoryDbPubDateCol, EDbColDateTime );

    // Description
    TDbCol descriptionCol( KIptvEpgCategoryDbDescriptionCol, EDbColText,
        KIptvEpgCategoryDescriptionMaxLength );

    // Content count
    TDbCol contentCountCol( KIptvEpgCategoryDbContentCountCol, EDbColUint32 );

    CDbColSet* categoryColSet = CDbColSet::NewLC();
    categoryColSet->AddL( keyCol );
    categoryColSet->AddL( idCol );
    categoryColSet->AddL( parentKeyCol );
    categoryColSet->AddL( nameCol );
    categoryColSet->AddL( iconFileIdCol );
    categoryColSet->AddL( isIconFilePathUrlCol ); // Obsolete field
    categoryColSet->AddL( iconFilePathCol ); // Obsolete field
    categoryColSet->AddL( pubDateCol ); // Obsolete field
    categoryColSet->AddL( descriptionCol );
    categoryColSet->AddL( contentCountCol );

    TInt error =
        aDatabase.CreateTable( KIptvEpgCategoryTable, *categoryColSet );
    CleanupStack::PopAndDestroy( categoryColSet );

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CreateCategoryIndexL()
//
// Creates an index for Category table. Leaves, if the index cannot be
// created.
// --------------------------------------------------------------------------
//
void CIptvEpgDb::CreateCategoryIndexL( RDbNamedDatabase& aDatabase ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CreateCategoryIndexL" );

    TDbKeyCol keyCol( KIptvEpgCategoryDbKeyCol );
    CDbKey* index = CDbKey::NewLC();
    index->AddL( keyCol );
    index->MakeUnique();
    User::LeaveIfError( aDatabase.CreateIndex(
        KIptvEpgCategoryDbIndexName, KIptvEpgCategoryTable, *index ) );
    CleanupStack::PopAndDestroy( index );
    }

// Database insert and update methods
// from MIptvEpgPluginManager interface

// --------------------------------------------------------------------------
// CIptvEpgDb::AddCategoryL()
//
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::AddCategoryL(
    RDbNamedDatabase& aDb,
    CIptvEpgCategory& aCategory,
    TUint32& aCategoryKey ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::AddCategoryL" );

    RDbView view;
    TInt error = KErrNone;
    error = view.Prepare(
        aDb, TDbQuery( iSqlCategory ), TDbWindow::EUnlimited,
        RDbView::EInsertOnly );
    if ( error == KErrNone )
        {
        view.InsertL();
        WriteCategoryToDbL( view, aCategory );
        view.PutL();

        // Get new category key
        CDbColSet* colSet = view.ColSetL();
        TInt keyColumnNo = colSet->ColNo( KIptvEpgCategoryDbKeyCol );
        delete colSet;
        aCategoryKey = view.ColUint32( keyColumnNo );
        view.Close();
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::AddCategoryL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::AddCategoryL(
    CIptvEpgCategory& aCategory,
    TUint32& aCategoryKey )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::AddCategoryL" );

    RDbView view;
    TInt error = KErrNone;
    error = view.Prepare(
        iTmpDb,
        TDbQuery( iSqlCategory ),
        TDbWindow::EUnlimited,
        RDbView::EInsertOnly );

    if ( error == KErrNone )
        {
        view.InsertL();
        WriteCategoryToDbL( view, aCategory );
        view.PutL();

        // Get new category key
        CDbColSet* colSet = view.ColSetL();
        TInt keyColumnNo = colSet->ColNo( KIptvEpgCategoryDbKeyCol );
        delete colSet;
        aCategoryKey = view.ColUint32( keyColumnNo );
        view.Close();
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::WriteCategoryToDbL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::WriteCategoryToDbL(
    RDbView& aView, CIptvEpgCategory& aCategory ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::WriteCategoryToDbL" );

    CDbColSet* categoryColSet = aView.ColSetL();
    CleanupStack::PushL( categoryColSet );

    // Key (is auto-increment)

    // Parent key
    aView.SetColL(
        categoryColSet->ColNo( KIptvEpgCategoryDbParentKeyCol ),
        aCategory.iParentKey );

    // Id
    aView.SetColL(
        categoryColSet->ColNo( KIptvEpgCategoryDbIdCol ),
        aCategory.iId );

    // Name
    aView.SetColL(
        categoryColSet->ColNo( KIptvEpgCategoryDbNameCol ),
        aCategory.iName );

    // Icon file Id
    aView.SetColL(
        categoryColSet->ColNo( KIptvEpgCategoryDbIconFileIdCol ),
        aCategory.iIconFileId ); // Obsolete field

    // IsIconFilePathUrl
    aView.SetColL(
        categoryColSet->ColNo( KIptvEpgCategoryDbIsIconFilePathUrlCol ),
        aCategory.iIsIconFilePathUrl ); // Obsolete field

    // Icon file path
    aView.SetColL(
        categoryColSet->ColNo( KIptvEpgCategoryDbIconFilePathCol ),
        aCategory.iIconFilePath ); // Obsolete field

    // Pub date
    aView.SetColL(
        categoryColSet->ColNo( KIptvEpgCategoryDbPubDateCol ),
        aCategory.iPubDate ); // Obsolete field

    // Description
    aView.SetColL(
        categoryColSet->ColNo( KIptvEpgCategoryDbDescriptionCol ),
        aCategory.iDescription );

    // Content count
    aView.SetColL(
        categoryColSet->ColNo( KIptvEpgCategoryDbContentCountCol ),
        aCategory.iContentCount );

    CleanupStack::PopAndDestroy( categoryColSet );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::AddContentL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::AddContentL(
    CIptvEpgContent& aContent,
    RPointerArray<CIptvMediaContent> aMediaContents,
    TUint32& aContentKey )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::AddContentL" );

    RDbView view;
    TInt error = KErrNone;
    error = view.Prepare(
        iTmpDb, TDbQuery( iSqlContent ),
        TDbWindow::EUnlimited,
        RDbView::EInsertOnly );
    if ( error == KErrNone )
        {
        view.InsertL();
        WriteContentToDbL( view, aContent, EFalse );
        view.PutL();

        // Get new content key
        CDbColSet* colSet = view.ColSetL();
        TInt keyColumnNo = colSet->ColNo( KIptvEpgContentDbKeyCol );
        delete colSet;
        aContentKey = view.ColUint32( keyColumnNo );
        view.Close();
        }

    // Add media contents
    TInt count = aMediaContents.Count();
    if ( count > 0 )
        {
        HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
        TPtr16 sqlPtr = sql->Des();

        sqlPtr.Append( KSqlSelect );
        sqlPtr.Append( KSqlAll );
        sqlPtr.Append( KSqlFrom );
        sqlPtr.Append( KIptvCATable );

        RDbView caView;
        error = caView.Prepare(
            iTmpDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited,
            RDbView::EInsertOnly );

        CleanupStack::PopAndDestroy( sql );

        if ( error == KErrNone )
            {
            for ( TInt i = 0; i < count; i++ )
                {
                caView.InsertL();
                WriteContentAccessToDbL(
                    caView, aMediaContents[i], aContentKey, i );
                caView.PutL();
                }
            }
        caView.Close();
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::WriteContentAccessToDbL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::WriteContentAccessToDbL(
    RDbView& aView,
    const CIptvMediaContent* aMediaContent,
    TUint32 aContentKey,
    TUint32 aIndex ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::WriteContentAccessToDbL" );

    if ( aMediaContent )
        {
        CDbColSet* colSet = aView.ColSetL();
        CleanupStack::PushL( colSet );

        // Content key
        aView.SetColL( colSet->ColNo( KIptvCAContentKey ), aContentKey );

        // Content URL
        RDbColWriteStream writeStream;
        writeStream.OpenLC( aView, colSet->ColNo( KIptvCAContentUrl ) );
        writeStream.WriteL( aMediaContent->GetMediaContentUrl() );
        writeStream.Close();
        CleanupStack::Pop(); // writeStream

        // Download type
        aView.SetColL(
            colSet->ColNo( KIptvCADlType ),
            aMediaContent->iDownloadType );

        // Language
        aView.SetColL(
            colSet->ColNo( KIptvCALanguage ),
            aMediaContent->iLanguage );

        // Last play position
        aView.SetColL(
            colSet->ColNo( KIptvCALastPlayPosition ),
            aMediaContent->iLastPosition );

        // Mime type
        aView.SetColL(
            colSet->ColNo( KIptvCAMimeType ),
            aMediaContent->iMimeType );

        // File size
        aView.SetColL(
            colSet->ColNo( KIptvCAFileSize ),
            aMediaContent->iFileSize );

        // Duration
        aView.SetColL(
            colSet->ColNo( KIptvCADuration ),
            aMediaContent->iDuration );

        // File id
        aView.SetColL( colSet->ColNo( KIptvCAFileId ), 0 );

        // Drive id
        aView.SetColL( colSet->ColNo( KIptvCADriveId ), 0 );

        // Index
        aView.SetColL( colSet->ColNo( KIptvCAIndex ), aIndex );

        // Medium
        // Not used

        // Is default
        // Not used

        // Expression
        aView.SetColL(
            colSet->ColNo( KIptvCAExpression ),
            aMediaContent->iExpression );

        // Price
        // Not in media content

        // Purchase type
        // Not in mediacontent

        // Purchase URL
        // Not in media content

        // Last play position
        // Not in media content

        CleanupStack::PopAndDestroy( colSet );
        }
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::WriteContentToDbL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::WriteContentToDbL(
    RDbView& aView, CIptvEpgContent& aContent, TBool aLeave ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::WriteContentToDbL" );

    CDbColSet* colSet = aView.ColSetL();
    CleanupStack::PushL( colSet );

    // Key (is auto-increment)

    if ( !aLeave )
        {
        // File Id
        aView.SetColL(
            colSet->ColNo( KIptvEpgContentDbFileIdCol ),
            KIptvFileId );

        // Drive
        aView.SetColL(
            colSet->ColNo( KIptvEpgContentDbDriveIdCol ),
            KIptvDrive );
        }

    // Id
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbIdCol ),
        aContent.iId );

    // Name
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbNameCol ),
        aContent.iName );

    // Content protection
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbContentProtectionCol ),
        aContent.iContentProtection );

    // Icon file Id
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbIconFileIdCol ),
        aContent.iIconFileId );

    // Is icon file path URL
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbIsIconFilePathUrlCol ),
        aContent.iIsIconFilePathUrl );

    // Icon file path
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbIconFilePathCol ),
        aContent.iIconFilePath );

    // Content file Id
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbContentFileIdCol ),
        aContent.iContentFileId );

    // Description
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbDescriptionCol ),
        aContent.iDescription );

    // Pub date
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbPubDateCol ),
        aContent.iPubDate );

    // Browser url
    RDbColWriteStream writeStream;
    writeStream.OpenLC(
        aView, colSet->ColNo( KIptvEpgContentDbBrowserUrlCol ) );
    writeStream.WriteL( aContent.iBrowserUrl );
    writeStream.Close();
    CleanupStack::Pop( &writeStream );

    // Order
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbOrderCol ),
        aContent.iOrder );

    // Author
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbAuthorCol ),
        aContent.iAuthor );

    // Copyright
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbCopyrightCol ),
        aContent.iCopyright );

    // Size
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbSizeCol ),
        aContent.iSize );

    // Duration
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbDurationCol ),
        aContent.iDuration );

    // Language
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbLanguageCol ),
        aContent.iLanguage );

    // RatingAge
    aView.SetColL(
        colSet->ColNo( KIptvEpgContentDbRatingAgeCol ),
        aContent.iRatingAge );

    CleanupStack::PopAndDestroy( colSet );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::IsDownloadedL
// --------------------------------------------------------------------------
//
TBool CIptvEpgDb::IsDownloadedL( const RDbView& aView ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::IsDownloadedL" );

    CDbColSet* colSet = aView.ColSetL();
    CleanupStack::PushL( colSet );
    TInt fileIdCol = colSet->ColNo( KIptvEpgContentDbFileIdCol );
    TInt driveCol = colSet->ColNo( KIptvEpgContentDbDriveIdCol );
    CleanupStack::PopAndDestroy( colSet );

    TUint32 fileId = aView.ColUint32( fileIdCol );
    TUint32 drive = aView.ColUint32( driveCol );

    TBool downloaded = EFalse;
    if ( fileId == 0 && drive == 0 )
        {
        downloaded = EFalse;
        }
    else
        {
        downloaded = ETrue;
        }

    return downloaded;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::UpdateCategoryIconL
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::UpdateCategoryIconL(
    TUint32& aCategoryKey,
    TUint32& /*aIconFileId*/,
    const TDesC& aIconFilePath,
    TBool aIsUrl )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::UpdateCategoryIconL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgCategoryDbKeyCol );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgCategoryDbIconFileIdCol );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgCategoryDbIconFilePathCol );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgCategoryDbIsIconFilePathUrlCol );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgCategoryTable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvEpgCategoryDbKeyCol );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aCategoryKey ) );

    RDbView view;
    TInt error = KErrNone;
    error = view.Prepare( iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        if ( !view.IsEmptyL() )
            {
            view.FirstL();
            view.UpdateL();

            CDbColSet* colSet = view.ColSetL();
            CleanupStack::PushL( colSet );
            view.SetColL(
                colSet->ColNo( KIptvEpgCategoryDbIconFilePathCol ),
                aIconFilePath );
            view.SetColL(
                colSet->ColNo( KIptvEpgCategoryDbIsIconFilePathUrlCol ),
                aIsUrl );
            CleanupStack::PopAndDestroy( colSet );
            view.PutL();
            }
        else
            {
            error = KErrNotFound;
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::UpdateContentIconL
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::UpdateContentIconL(
    TUint32& aContentKey,
    TUint32& /*aIconFileId*/,
    const TDesC& aIconFilePath,
    TBool aIsUrl )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::UpdateContentIconL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgContentDbKeyCol );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgContentDbIconFileIdCol );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgContentDbIconFilePathCol );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgContentDbIsIconFilePathUrlCol );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgContentTable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvEpgContentDbKeyCol );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aContentKey ) );

    RDbView view;
    TInt error = KErrNone;
    error = view.Prepare( iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        if ( !view.IsEmptyL() )
            {
            view.FirstL();
            view.UpdateL();

            CDbColSet* colSet = view.ColSetL();
            CleanupStack::PushL( colSet );
            view.SetColL(
                colSet->ColNo( KIptvEpgContentDbIconFilePathCol ),
                aIconFilePath );
            view.SetColL( colSet->ColNo(
                KIptvEpgContentDbIsIconFilePathUrlCol ), aIsUrl );
            CleanupStack::PopAndDestroy( colSet );
            view.PutL();
            }
        else
            {
            error = KErrNotFound;
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::AddRelationL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::AddRelationL( TUint32& aCategoryKey, TUint32& aContentKey )
    {
    IPTVLOGSTRING3_LOW_LEVEL(
        "CIptvEpgDb::AddRelationL --- Add relation from category %d to content %d",
        aCategoryKey, aContentKey );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgCategoryContentCategoryKeyCol );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgCategoryContentContentKeyCol );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgCategoryContentTable );

    RDbView view;
    TInt error = KErrNone;
    error = view.Prepare(
        iTmpDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited, RDbView::EInsertOnly );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.InsertL();

        CDbColSet* colSet = view.ColSetL();
        CleanupStack::PushL( colSet );

        // Category
        view.SetColL( colSet->ColNo(
            KIptvEpgCategoryContentCategoryKeyCol ), aCategoryKey );

        // Content
        view.SetColL( colSet->ColNo(
            KIptvEpgCategoryContentContentKeyCol ), aContentKey );

        CleanupStack::PopAndDestroy( colSet );

        TRAPD( putError, view.PutL() );
        if ( putError == KErrAlreadyExists )
            {
            view.Close();
            }

        view.Close();
        }
    else
        {
        IPTVLOGSTRING2_LOW_LEVEL( "CIptvEpgDb::AddRelationL ERROR =  %d", error );
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::MakeSqlStrings()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::MakeSqlStrings()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::MakeSqlStrings" );

    // Category
    iSqlCategory.Append( KSqlSelect );
    iSqlCategory.Append( KSqlAll );
    iSqlCategory.Append( KSqlFrom );
    iSqlCategory.Append( KIptvEpgCategoryTable );

    // Content
    iSqlContent.Append( KSqlSelect );
    iSqlContent.Append( KSqlAll );
    iSqlContent.Append( KSqlFrom );
    iSqlContent.Append( KIptvEpgContentTable );
    }

// Query methods for EpgMsgHandler from MIptvVodContent interface

// --------------------------------------------------------------------------
// CIptvEpgDb::GetEcgCategoryList()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetEcgCategoryListL(
    TIptvCategoryId aParentCategoryId,
    RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetEcgCategoryListL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KSqlAll );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgCategoryTable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvEpgCategoryDbParentKeyCol );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aParentCategoryId ) );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        while ( view.AtRow() )
            {
            view.GetL();
            CIptvVodContentCategoryBriefDetails* details =
                CIptvVodContentCategoryBriefDetails::NewL();
            CleanupStack::PushL( details );
            ReadCategoryFromDbL( view, *details );
            aCategoryList.Append ( details );
            CleanupStack::Pop( details );
            view.NextL();
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::GetCategoryDetails()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetCategoryDetailsL(
    TIptvCategoryId aCategoryId,
    CIptvVodContentCategoryBriefDetails& aCategory )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetCategoryDetailsL" );

    // aCategoryId = category key

    if( aCategoryId == KIptvRssSearchCategoryId)
        {
        return KErrNotSupported;
        }

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( iSqlCategory );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvEpgCategoryDbKeyCol );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aCategoryId ) );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        TInt count = view.CountL();
        if ( count > 0 )
            {
            if ( count == KMaxCountOfFullDetails )
                {
                view.GetL();
                ReadCategoryFromDbL( view, aCategory );
                }
            else
                {
                error = KErrNotSupported;
                }
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::ReadCategoryFromDbL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::ReadCategoryFromDbL(
    const RDbView& aView,
    CIptvVodContentCategoryBriefDetails& aCategory ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::ReadCategoryFromDbL" );

    CDbColSet* colSet = aView.ColSetL();
    CleanupStack::PushL( colSet );
    TInt keyColumnNo = colSet->ColNo( KIptvEpgContentDbKeyCol );
    TInt nameColumnNo = colSet->ColNo( KIptvEpgCategoryDbNameCol );
     // Obsolete field:
    TInt pubDateColumnNo = colSet->ColNo( KIptvEpgCategoryDbPubDateCol );
     // Obsolete field:
    TInt thumbnailColumnNo =
        colSet->ColNo( KIptvEpgCategoryDbIconFilePathCol );
    TInt contentCountColumnNo =
        colSet->ColNo( KIptvEpgCategoryDbContentCountCol );

    CleanupStack::PopAndDestroy( colSet );

    // iCategoryId = category key
    aCategory.iCategoryId = aView.ColUint32( keyColumnNo );

    // Name
    TPtrC name;
    name.Set( aView.ColDes( nameColumnNo ) );
    TInt valueLength = name.Length();
    TInt dataToCopy = Min( valueLength, KIptvEpgCategoryNameMaxLength );
    aCategory.iName.Copy( name.Mid( 0, dataToCopy ) );

    // pubDate - Obsolete field
    aCategory.iPubDate = aView.ColTime( pubDateColumnNo );

    // Thumbnail path - Obsolete field
    TPtrC path;
    path.Set( aView.ColDes( thumbnailColumnNo ) );
    valueLength = path.Length();
    dataToCopy = Min( valueLength, KIptvEpgCategoryIconFilePathMaxLength );
    aCategory.iThumbnailPath.Copy( path.Mid( 0, dataToCopy ) );

    // Content count
    aCategory.iContentCount = aView.ColUint32( contentCountColumnNo );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::GetParentCategory()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetParentCategoryL(
    TIptvCategoryId aCategoryId,
    TIptvCategoryId& aParentId )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetParentCategoryL" );

    // aCategoryId = category key

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgCategoryDbKeyCol );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgCategoryDbParentKeyCol );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgCategoryTable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvEpgCategoryDbKeyCol );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aCategoryId ) );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        TInt count = view.CountL();
        if ( count == KMaxCountOfParentCategories )
            {
            CDbColSet* colSet = view.ColSetL();
            CleanupStack::PushL( colSet );
            TInt parentColumnNo = colSet->ColNo(
                KIptvEpgCategoryDbParentKeyCol );
          CleanupStack::PopAndDestroy( colSet );
            view.GetL();
            aParentId = view.ColUint32( parentColumnNo );
            }
        else
            {
            error = KErrNotSupported;
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::GetEcgListL()
// Get all contents in given category in order defined by database originated
// from service provider
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetEcgListL(
    TIptvCategoryId aCategoryId,
    const TDesC& aSearchString,
    TUint32 aFrom,
    TUint32 aAmount,
    TUint32& aTotalAmount,
    RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetEcgListL" );

    RArray<TUint32> contents;

    // aCategoryId = category key

    // First get all contents belonging to given category
    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgCategoryContentCategoryKeyCol );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgCategoryContentContentKeyCol );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgCategoryContentTable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvEpgCategoryContentCategoryKeyCol );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aCategoryId ) );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );
    if ( error == KErrNone )
        {
        view.EvaluateAll();

        CDbColSet* colSet = view.ColSetL();
        CleanupStack::PushL( colSet );
        TInt contentKeyColumnNo =
            colSet->ColNo( KIptvEpgCategoryContentContentKeyCol );
        CleanupStack::PopAndDestroy( colSet );

        view.FirstL();
        while ( view.AtRow() )
            {
            view.GetL();
            TUint32 contentKey = view.ColUint32( contentKeyColumnNo );
            contents.AppendL( contentKey );
            view.NextL();
            }
        }

    view.Close();

    // Add all contents matching search string (if defined) to temporary
    // RPointerArray data.
    RPointerArray<CIptvVodContentContentBriefDetails> data;
    RDbView contentView;

    // Add "order by order field"
    sqlPtr.Zero();
    sqlPtr.Append( iSqlContent );
    sqlPtr.Append( KSqlOrderBy );
    sqlPtr.Append( KIptvEpgContentDbOrderCol );

    error = contentView.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        CDbColSet* cs = contentView.ColSetL();
        CleanupStack::PushL( cs );
        TInt nameCs = cs->ColNo( KIptvEpgContentDbNameCol );
        TInt keyCs = cs->ColNo( KIptvEpgContentDbKeyCol );
        TInt ageCs = cs->ColNo( KIptvEpgContentDbRatingAgeCol );

        contentView.EvaluateAll();
        contentView.FirstL();
        while ( contentView.AtRow() )
            {
            contentView.GetL();

            // Check that content is in given category ->
            // content key must be found on the contents array.
            TUint32 key = contentView.ColUint32( keyCs );
            TInt count = contents.Count();
            TBool inCategory( EFalse );
            for ( TInt i = 0; ( i < count ) && !inCategory; i++ )
                {
                if ( key == contents[i] )
                    {
                    inCategory = ETrue;
                    }
                }

            if ( inCategory )
                {
                TBool add( EFalse );
                TUint16 ratingAge( contentView.ColUint16( ageCs ) );

                if ( ( KParentalControlOff == iParentalControl ) ||
                     ( ratingAge < iParentalControl ) )
                    {
                    if ( aSearchString.Compare( KEmptyString ) != 0 )
                        {
                        // Name
                        TPtrC name;
                        name.Set( contentView.ColDes( nameCs ) );
                        TInt searchStringLength = aSearchString.Length();
                        TInt nameLength = name.Length();
                        TInt dataToCompare = Min( searchStringLength, nameLength );
                        if ( aSearchString.Compare( name.Mid( 0, dataToCompare ) ) == 0 )
                            {
                            add = ETrue;
                            }
                        }
                    else
                        {
                        add = ETrue;
                        }
                    }

                if ( add )
                    {
                    CIptvVodContentContentBriefDetails* bd =
                        CIptvVodContentContentBriefDetails::NewL();
                    CleanupStack::PushL( bd );
                    ReadContentBriefDetailsFromDbL( contentView, *bd );
                    data.Append( bd );
                    CleanupStack::Pop( bd );
                    }
                }

            contentView.NextL();
            }

        CleanupStack::PopAndDestroy( cs );
        }

    contentView.Close();

    TUint32 count = 0;
    TUint32 totalAmount = data.Count();
    aTotalAmount = totalAmount;
    for ( TUint32 i = 0; i < totalAmount; i++ )
        {
        if ( i >= aFrom )
            {
            if ( aAmount == 0 || count < aAmount )
                {
                count++;
                aEcgList.AppendL( data[0] );
                data.Remove( 0 );
                }
            }
        }

    data.ResetAndDestroy();

    contents.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::ReadContentBriefDetailsFromDbL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::ReadContentBriefDetailsFromDbL(
    const RDbView& aView,
    CIptvVodContentContentBriefDetails& aBriefDetails ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::ReadContentBriefDetailsFromDbL" );

    CDbColSet* colSet = aView.ColSetL();
    CleanupStack::PushL( colSet );
    TInt keyColumnNo = colSet->ColNo( KIptvEpgContentDbKeyCol );
    TInt nameColumnNo = colSet->ColNo( KIptvEpgContentDbNameCol );
    TInt iconFilePathColumnNo = colSet->ColNo( KIptvEpgContentDbIconFilePathCol );
    TInt fileIdColumnNo = colSet->ColNo( KIptvEpgContentDbFileIdCol );
    TInt driveColumnNo = colSet->ColNo( KIptvEpgContentDbDriveIdCol );
    TInt pFileIdColumnNo = colSet->ColNo( KIptvEpgContentDbPreviewFileIdCol );
    TInt pDriveIdColumnNo = colSet->ColNo( KIptvEpgContentDbPreviewDriveIdCol );
    TInt pubDateColumnNo = colSet->ColNo( KIptvEpgContentDbPubDateCol );
    TInt browserUrlColumnNo = colSet->ColNo( KIptvEpgContentDbBrowserUrlCol );
    TInt sizeColumnNo = colSet->ColNo( KIptvEpgContentDbSizeCol );
    TInt durationColumnNo = colSet->ColNo( KIptvEpgContentDbDurationCol );
    TInt languageColumnNo = colSet->ColNo( KIptvEpgContentDbLanguageCol );
    TInt ratingAgeColumnNo = colSet->ColNo( KIptvEpgContentDbRatingAgeCol );

    CleanupStack::PopAndDestroy( colSet );

    // TIptvContentId
    aBriefDetails.iContentId = aView.ColUint32( keyColumnNo );

    // Name
    TPtrC name;
    name.Set( aView.ColDes( nameColumnNo ) );
    TInt valueLength = name.Length();
    TInt dataToCopy = Min( valueLength, KIptvEpgContentNameMaxLength );
    aBriefDetails.iName.Copy( name.Mid( 0, dataToCopy ) );

    // Thumbnail path
    TPtrC iconFilePath;
    iconFilePath.Set( aView.ColDes( iconFilePathColumnNo ) );
    valueLength = iconFilePath.Length();
    dataToCopy = Min( valueLength, KIptvEpgContentIconFilePathMaxLength );
    aBriefDetails.iThumbnailPath.Copy( iconFilePath.Mid( 0, dataToCopy ) );

    // Global file id
    aBriefDetails.iFileId->iDrive = aView.ColUint32( driveColumnNo );
    aBriefDetails.iFileId->iFileId = aView.ColUint32( fileIdColumnNo );

    // Preview global file id
    aBriefDetails.iPreviewGlobalFileId->iDrive = aView.ColUint32( pDriveIdColumnNo );
    aBriefDetails.iPreviewGlobalFileId->iFileId = aView.ColUint32( pFileIdColumnNo );

    // pubDate
    aBriefDetails.iPubDate = aView.ColTime( pubDateColumnNo );

    // Browser Url
    RDbColReadStream readStream;
    readStream.OpenLC( aView, browserUrlColumnNo );
    dataToCopy =
        Min( KIptvEpgContentBrowserUrlMaxLength,
        aView.ColLength( browserUrlColumnNo ) );
    TBuf<KIptvEpgContentBrowserUrlMaxLength> temp;
    readStream.ReadL( temp, dataToCopy );
    aBriefDetails.SetBrowserUrlL( temp );
    readStream.Close();
    CleanupStack::Pop( &readStream );

    // Size
    aBriefDetails.iSize = aView.ColUint32( sizeColumnNo );

    // Duration
    aBriefDetails.iPlaytime = aView.ColUint32( durationColumnNo );

    // Language
    TPtrC language;
    language.Set( aView.ColDes( languageColumnNo ) );
    valueLength = language.Length();
    dataToCopy = Min( valueLength, KIptvCALanguageLength );
    aBriefDetails.iLanguage.Copy( language.Mid( 0, dataToCopy ) );

    // Rating age
    aBriefDetails.iRatingAge = aView.ColUint16( ratingAgeColumnNo );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::GetEcgAllListL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetEcgAllListL(
    const TDesC& aSearchString,
    TUint32 aFrom,
    TUint32 aAmount,
    TUint32& aTotalAmount,
    RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetEcgAllListL" );

    // First lets get all contents matching search string (if defined)
    // to temporary RPointerArray data.
    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( iSqlContent );
    sqlPtr.Append( KSqlOrderBy );
    sqlPtr.Append( KIptvEpgContentDbNameCol );

    RPointerArray<CIptvVodContentContentBriefDetails> data;
    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        CDbColSet* cs = view.ColSetL();
        CleanupStack::PushL( cs );
        TInt nameCs = cs->ColNo( KIptvEpgContentDbNameCol );
        TInt ageCs = cs->ColNo( KIptvEpgContentDbRatingAgeCol );

        view.EvaluateAll();
        view.FirstL();
        while ( view.AtRow() )
            {
            view.GetL();
            TBool add( EFalse );
            TUint16 ratingAge( view.ColUint16( ageCs ) );

            if ( ( KParentalControlOff == iParentalControl ) ||
                 ( ratingAge < iParentalControl ) )
                {
                if ( aSearchString.Compare( KEmptyString ) != 0 )
                    {
                    // Name
                    TPtrC name;
                    name.Set( view.ColDes( nameCs ) );
                    TInt searchStringLength = aSearchString.Length();
                    TInt nameLength = name.Length();
                    TInt dataToCompare = Min( searchStringLength, nameLength );
                    if ( aSearchString.Compare( name.Mid( 0, dataToCompare ) ) == 0 )
                        {
                        add = ETrue;
                        }
                    }
                else
                    {
                    add = ETrue;
                    }
                }

            if ( add )
                {
                CIptvVodContentContentBriefDetails* bd =
                    CIptvVodContentContentBriefDetails::NewL();
                CleanupStack::PushL( bd );
                ReadContentBriefDetailsFromDbL( view, *bd );
                data.Append( bd );
                CleanupStack::Pop( bd );
                }
            view.NextL();
            }
        CleanupStack::PopAndDestroy( cs );
        }

    TUint32 count = 0;
    TUint32 totalAmount = data.Count();
    aTotalAmount = totalAmount;
    for ( TUint32 i = 0; i < totalAmount; i++ )
        {
        if ( i >= aFrom )
            {
            if ( aAmount == 0 || count < aAmount )
                {
                count++;
                aEcgList.AppendL( data[0] );
                data.Remove( 0 );
                }
            }
        }

    data.ResetAndDestroy();
    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::GetContentDetailsL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetContentDetailsL(
    TIptvContentId aContentId,
    CIptvVodContentContentFullDetails& aContentFullDetails )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetContentDetailsL" );

    // aContentId = content key

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( iSqlContent );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvEpgContentDbKeyCol );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aContentId ) );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        TInt count = view.CountL();
        if ( count > 0 )
            {
            if ( count == KMaxCountOfFullDetails )
                {
                view.GetL();
                ReadContentFullDetailsFromDbL( view, aContentFullDetails );
                }
            else
                {
                error = KErrNotSupported;
                }
            }
        }

    view.Close();

    // Get language from content access objects. Get "full" video's language.

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::ReadContentFullDetailsFromDbL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::ReadContentFullDetailsFromDbL(
    const RDbView& aView,
    CIptvVodContentContentFullDetails& aContentFullDetails ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::ReadContentFullDetailsFromDbL" );

    CDbColSet* colSet = aView.ColSetL();
    CleanupStack::PushL( colSet );
    TInt keyColumnNo = colSet->ColNo( KIptvEpgContentDbKeyCol );
    TInt nameColumnNo = colSet->ColNo( KIptvEpgContentDbNameCol );
    TInt iconFilePathColumnNo = colSet->ColNo( KIptvEpgContentDbIconFilePathCol );
    TInt fileIdColumnNo = colSet->ColNo( KIptvEpgContentDbFileIdCol );
    TInt pubDateColumnNo = colSet->ColNo( KIptvEpgContentDbPubDateCol );
    TInt browserUrlColumnNo = colSet->ColNo( KIptvEpgContentDbBrowserUrlCol );
    TInt driveColumnNo = colSet->ColNo( KIptvEpgContentDbDriveIdCol );
    TInt pFileIdColumnNo = colSet->ColNo( KIptvEpgContentDbPreviewFileIdCol );
    TInt pDriveIdColumnNo = colSet->ColNo( KIptvEpgContentDbPreviewDriveIdCol );
    TInt descColumnNo = colSet->ColNo( KIptvEpgContentDbDescriptionCol );
    TInt contentProtectionColumnNo = colSet->ColNo( KIptvEpgContentDbContentProtectionCol );
    TInt authorColNo = colSet->ColNo( KIptvEpgContentDbAuthorCol );
    TInt copyrightColNo = colSet->ColNo( KIptvEpgContentDbCopyrightCol );
    TInt sizeColumnNo = colSet->ColNo( KIptvEpgContentDbSizeCol );
    TInt durationColumnNo = colSet->ColNo( KIptvEpgContentDbDurationCol );
    TInt languageColumnNo = colSet->ColNo( KIptvEpgContentDbLanguageCol );
    TInt ratingAgeColumnNo = colSet->ColNo( KIptvEpgContentDbRatingAgeCol );
    CleanupStack::PopAndDestroy( colSet );

    // TIptvContentId
    aContentFullDetails.iContentId = aView.ColUint32( keyColumnNo );

    // Name
    TPtrC name;
    name.Set( aView.ColDes( nameColumnNo ) );
    TInt valueLength = name.Length();
    TInt dataToCopy = Min( valueLength, KIptvEpgContentNameMaxLength );
    aContentFullDetails.iName.Copy( name.Mid( 0, dataToCopy ) );

    // Thumbnail path
    TPtrC iconFilePath;
    iconFilePath.Set( aView.ColDes( iconFilePathColumnNo ) );
    valueLength = iconFilePath.Length();
    dataToCopy = Min( valueLength, KIptvEpgContentIconFilePathMaxLength );
    aContentFullDetails.iThumbnailPath.Copy( iconFilePath.Mid( 0, dataToCopy ) );

    // File id
    aContentFullDetails.iFileId->iDrive = aView.ColUint32( driveColumnNo );
    aContentFullDetails.iFileId->iFileId = aView.ColUint32( fileIdColumnNo );

    // Preview global file id
    aContentFullDetails.iPreviewGlobalFileId->iDrive = aView.ColUint32( pDriveIdColumnNo );
    aContentFullDetails.iPreviewGlobalFileId->iFileId = aView.ColUint32( pFileIdColumnNo );

    // pubDate
    aContentFullDetails.iPubDate = aView.ColTime( pubDateColumnNo );

    // Browser Url
    RDbColReadStream readStream;
    readStream.OpenLC( aView, browserUrlColumnNo );
    dataToCopy = Min(
        KIptvEpgContentBrowserUrlMaxLength,
        aView.ColLength( browserUrlColumnNo ) );
    TBuf<KIptvEpgContentBrowserUrlMaxLength> temp;
    readStream.ReadL( temp, dataToCopy );
    aContentFullDetails.SetBrowserUrlL( temp );
    readStream.Close();
    CleanupStack::Pop( &readStream );

    // Description
    TPtrC desc;
    desc.Set( aView.ColDes( descColumnNo ) );
    valueLength = desc.Length();
    dataToCopy = Min( valueLength, KIptvEpgContentDescriptionMaxLength );
    aContentFullDetails.iDescription.Copy( desc.Mid( 0, dataToCopy ) );

    // DRM placeholder.
    aContentFullDetails.iContentProtection =
        aView.ColUint32( contentProtectionColumnNo );

    // Author
    TPtrC author;
    author.Set( aView.ColDes( authorColNo ) );
    valueLength = author.Length();
    dataToCopy = Min( valueLength, KIptvEpgContentAuthorLength );
    aContentFullDetails.iAuthor.Copy( author.Mid( 0, dataToCopy ) );

    // Copyright
    TPtrC copyright;
    copyright.Set( aView.ColDes( copyrightColNo ) );
    valueLength = copyright.Length();
    dataToCopy = Min( valueLength, KIptvEpgContentCopyrightLength );
    aContentFullDetails.iCopyright.Copy( copyright.Mid( 0, dataToCopy ) );

    // Size
    aContentFullDetails.iSize = aView.ColUint32( sizeColumnNo );

    // Duration
    aContentFullDetails.iPlaytime = aView.ColUint32( durationColumnNo );

    // Language
    TPtrC language;
    language.Set( aView.ColDes( languageColumnNo ) );
    valueLength = language.Length();
    dataToCopy = Min( valueLength, KIptvCALanguageLength );
    aContentFullDetails.iLanguage.Copy( language.Mid( 0, dataToCopy ) );

    // Rating age
    aContentFullDetails.iRatingAge = aView.ColUint16( ratingAgeColumnNo );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::GetServiceInformationL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetServiceInformationL(
    TTime& aLastUpdate, TTime& aInterval )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetServiceInformationL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgServiceNextUpdate );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgServiceLastUpdate );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgServiceTable );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        TInt count = view.CountL();
        if ( count > 0 )
            {
            CDbColSet* colSet = view.ColSetL();
            CleanupStack::PushL( colSet );
            TInt nextUpdateColumnNo = colSet->ColNo( KIptvEpgServiceNextUpdate );
            TInt lastUpdateColumnNo = colSet->ColNo( KIptvEpgServiceLastUpdate );
            CleanupStack::PopAndDestroy( colSet );

            view.GetL();
            aLastUpdate = view.ColTime( lastUpdateColumnNo );
            aInterval = view.ColTime( nextUpdateColumnNo );
            }
        else
            {
            error = KIptvErrorGetUpdateInformationLNotFound;
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::SetServiceInformationL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::SetServiceInformationL(
    RDbNamedDatabase& aDb,
    const TTime& aLastUpdate,
    const TTime& aInterval) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::SetServiceInformationL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KSqlAll );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgServiceTable );

    RDbView view;
    TInt error = view.Prepare(
        aDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited, RDbView::EInsertOnly );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.InsertL();
        CDbColSet* categoryColSet = view.ColSetL();
        CleanupStack::PushL( categoryColSet );

        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceLastUpdate ), aLastUpdate );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceNextUpdate ), aInterval );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceETag ), KIptvEmptyDes );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceLastModifiedSince ), KIptvEmptyDes );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceXmlFileHash ), KIptvEmptyDes );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceVersion ), KIptvDbVersion );
        view.PutL();
        CleanupStack::PopAndDestroy( categoryColSet );
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::UpdateServiceInformationL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::UpdateServiceInformationL(
    const TTime& aLastUpdate,
    const TTime& aInterval )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::UpdateServiceInformationL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgServiceNextUpdate );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgServiceLastUpdate );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgServiceTable );

    RDbView view;
    TInt error = KErrNone;
    error = view.Prepare( iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        view.UpdateL();

        CDbColSet* categoryColSet = view.ColSetL();
        CleanupStack::PushL( categoryColSet );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceLastUpdate ), aLastUpdate );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceNextUpdate ), aInterval );
        view.PutL();
        CleanupStack::PopAndDestroy( categoryColSet );
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::GetServiceETagAndLastmodifiedSinceL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetServiceETagAndLastmodifiedSinceL(
    TDes& aETag,
    TDes& aLastModifiedSince )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetServiceETagAndLastmodifiedSinceL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgServiceETag );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgServiceLastModifiedSince );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgServiceTable );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL(); 
        TInt count = view.CountL();
        if ( count > 0 )
            {
            CDbColSet* colSet = view.ColSetL();
            CleanupStack::PushL( colSet );
            TInt eTagColumnNo = colSet->ColNo( KIptvEpgServiceETag );
            TInt lastModifiedSinceColumnNo = colSet->ColNo( KIptvEpgServiceLastModifiedSince );
            CleanupStack::PopAndDestroy( colSet );

            view.GetL();
            aETag.Copy( view.ColDes( eTagColumnNo ) );
            aLastModifiedSince.Copy( view.ColDes( lastModifiedSinceColumnNo ) );
            }
        else
            {
            IPTVLOGSTRING2_LOW_LEVEL( "CIptvEpgDb:: Failed to get last modified since info: %d.", error );
            error = KIptvErrorGetUpdateInformationLNotFound;
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::UpdateServiceETagAndLastModifiedSinceL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::UpdateServiceETagAndLastModifiedSinceL(
    const TDesC& aETag,
    const TDesC& aLastModifiedSince )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::UpdateServiceETagAndLastModifiedSinceL" );
	
    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgServiceETag );
    sqlPtr.Append( KSqlDot );
    sqlPtr.Append( KIptvEpgServiceLastModifiedSince );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgServiceTable );

    RDbView view;
    TInt error = KErrNone;
    error = view.Prepare( iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        view.UpdateL();

        CDbColSet* categoryColSet = view.ColSetL();
        CleanupStack::PushL( categoryColSet );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceETag ), aETag );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceLastModifiedSince ), aLastModifiedSince );
        view.PutL();
        CleanupStack::PopAndDestroy( categoryColSet );
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::GetServiceXmlFileHashL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetServiceXmlFileHashL(
    TDes& aHash)
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetServiceXmlFileHashL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgServiceXmlFileHash );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgServiceTable );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        TInt count = view.CountL();
        if ( count > 0 )
            {
            CDbColSet* colSet = view.ColSetL();
            CleanupStack::PushL( colSet );
            TInt aXmlFileHashColumnNo = colSet->ColNo( KIptvEpgServiceXmlFileHash );
            CleanupStack::PopAndDestroy( colSet );

            view.GetL();
            aHash.Copy( view.ColDes( aXmlFileHashColumnNo ) );
            }
        else
            {
            error = KIptvErrorGetUpdateInformationLNotFound;
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::UpdateServiceXmlFileHashL()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::UpdateServiceXmlFileHashL(
    const TDesC& aHash)
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::UpdateServiceXmlFileHashL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KIptvEpgServiceXmlFileHash );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvEpgServiceTable );

    RDbView view;
    TInt error = KErrNone;
    error = view.Prepare( iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        view.UpdateL();

        CDbColSet* categoryColSet = view.ColSetL();
        CleanupStack::PushL( categoryColSet );
        view.SetColL( categoryColSet->ColNo( KIptvEpgServiceXmlFileHash ), aHash );
        view.PutL();
        CleanupStack::PopAndDestroy( categoryColSet );
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::ResetGlobalId()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::ResetGlobalId( CIptvMyVideosGlobalFileId& aId )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::ResetGlobalId" );

    TInt error( KErrNone );

    TRAPD( err, ResetGlobalIdL( aId, error ) );

    if ( KErrNone != err )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvEpgDb::ResetGlobalId, failed: %d", err );
        error = err;
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::ResetGlobalIdL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::ResetGlobalIdL(
    CIptvMyVideosGlobalFileId& aId,
    TInt& aError )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::ResetGlobalIdL" );

    // Find content access object with given GFID -> reset GFID ->
    // check content access "parent" -> reset parent's GFID which is same as
    // given global file id (parameter)

    TInt key = KErrNotFound;

    // Reset global file id from content access
    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KSqlAll );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvCATable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvCAFileId );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aId.iFileId ) );
    sqlPtr.Append( KSqlAnd );
    sqlPtr.Append( KIptvCADriveId );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aId.iDrive ) );

    RDbView view;
    aError = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );
    if ( aError == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        TInt count = view.CountL();
        if ( count > 0 )
            {
            view.UpdateL();
            CDbColSet* cs = view.ColSetL();
            CleanupStack::PushL( cs );
            view.SetColL( cs->ColNo( KIptvCAFileId ), KIptvFileId );
            view.SetColL( cs->ColNo( KIptvCADriveId ), KIptvDrive );
            view.PutL();

            // Get content key
            TInt keyCol = cs->ColNo( KIptvCAContentKey );
            key = view.ColUint32( keyCol );
            CleanupStack::PopAndDestroy( cs );
            }
        }

    view.Close();

    // Check content access "parent"
    // -> reset parent's GFID which is same as given global file id (parameter)

    if ( key != KErrNotFound )
        {
        sqlPtr.Zero();
        sqlPtr.Append( KSqlSelect );
        sqlPtr.Append( KSqlAll );
        sqlPtr.Append( KSqlFrom );
        sqlPtr.Append( KIptvEpgContentTable );
        sqlPtr.Append( KSqlWhere );
        sqlPtr.Append( KIptvEpgContentDbKeyCol );
        sqlPtr.Append( KSqlIs );
        sqlPtr.AppendNum( TInt64( key ) );

        aError = view.Prepare(
            iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );
        if ( aError == KErrNone )
            {
            view.EvaluateAll();
            view.FirstL();
            view.GetL();
            TInt count = view.CountL();
            if ( count > 0 )
                {
                // Get global file id
                CDbColSet* colSet = view.ColSetL();

                TInt fileId = view.ColUint32(
                    colSet->ColNo( KIptvEpgContentDbFileIdCol ) );
                TInt driveId = view.ColUint32(
                    colSet->ColNo( KIptvEpgContentDbDriveIdCol ) );

                // Get preview global file id
                TInt pFileId = view.ColUint32(
                    colSet->ColNo( KIptvEpgContentDbPreviewFileIdCol ) );
                TInt pDriveId = view.ColUint32(
                    colSet->ColNo( KIptvEpgContentDbPreviewDriveIdCol ) );

                delete colSet;

                view.UpdateL();
                CDbColSet* c = view.ColSetL();
                CleanupStack::PushL( c );

                if ( fileId == aId.iFileId && driveId == aId.iDrive )
                    {
                    view.SetColL( c->ColNo(
                        KIptvEpgContentDbFileIdCol ),
                        KIptvFileId );
                    view.SetColL( c->ColNo(
                        KIptvEpgContentDbDriveIdCol ),
                        KIptvDrive );
                    }
                if ( pFileId == aId.iFileId && pDriveId == aId.iDrive )
                    {
                    view.SetColL( c->ColNo(
                        KIptvEpgContentDbPreviewFileIdCol ), KIptvFileId );
                    view.SetColL( c->ColNo(
                        KIptvEpgContentDbPreviewDriveIdCol ), KIptvDrive );
                    }

                view.PutL();
                CleanupStack::PopAndDestroy( c );
                }
            }
        view.Close();
        }

    CleanupStack::PopAndDestroy( sql );
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::SetGlobalId()
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::SetGlobalId(
    TUint32 aContentKey,
    CIptvMyVideosGlobalFileId& aId,
    TUint32 aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::SetGlobalId" );

    TInt error( KErrNone );

    TRAPD( err, SetGlobalIdL( aContentKey, aId, aIndex, error ) );

    if ( KErrNone != err )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvEpgDb::SetGlobalId, failed: %d", err );
        error = err;
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::SetGlobalIdL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::SetGlobalIdL(
    TUint32 aContentKey,
    CIptvMyVideosGlobalFileId& aId,
    TUint32 aIndex,
    TInt& aError )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::SetGlobalIdL" );

    // Set global file id first to content access object.
    // If content access is "full" or "preview" version,
    // set global file id also to main content object.

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KSqlAll );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvCATable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvCAContentKey );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aContentKey ) );
    sqlPtr.Append( KSqlAnd );
    sqlPtr.Append( KIptvCAIndex );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aIndex ) );

    TInt expression( KValueNotSet );
    RDbView view;
    aError = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );
    if ( aError == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        TInt count = view.CountL();
        if ( count > 0 )
            {
            view.UpdateL();
            CDbColSet* cs = view.ColSetL();
            CleanupStack::PushL( cs );
            view.SetColL( cs->ColNo( KIptvCAFileId ), aId.iFileId );
            view.SetColL( cs->ColNo( KIptvCADriveId ), aId.iDrive );
            view.PutL();

            // Get expression
            TInt expCol = cs->ColNo( KIptvCAExpression );
            expression = view.ColUint32( expCol );
            CleanupStack::PopAndDestroy( cs );
            }
        else
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "CIptvEpgDb::SetGlobalIdL NO CONTENT ACCESS FOUND WITH ID %d",
                aContentKey );
            }
        }

    view.Close();

    if ( ( expression == CIptvMediaContent::EFull ) ||
         ( expression == CIptvMediaContent::ESample ) )
        {
        sqlPtr.Zero();
        sqlPtr.Append( KSqlSelect );
        sqlPtr.Append( KSqlAll );
        sqlPtr.Append( KSqlFrom );
        sqlPtr.Append( KIptvEpgContentTable );
        sqlPtr.Append( KSqlWhere );
        sqlPtr.Append( KIptvEpgContentDbKeyCol );
        sqlPtr.Append( KSqlIs );
        sqlPtr.AppendNum( TInt64( aContentKey ) );

        aError = view.Prepare(
            iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );
        if ( aError == KErrNone )
            {
            view.EvaluateAll();
            view.FirstL();
            TInt count = view.CountL();
            if ( count > 0 )
                {
                view.UpdateL();
                CDbColSet* c = view.ColSetL();
                CleanupStack::PushL( c );

                if ( expression == CIptvMediaContent::EFull )
                    {
                    view.SetColL(
                        c->ColNo( KIptvEpgContentDbFileIdCol ),
                        aId.iFileId );
                    view.SetColL(
                        c->ColNo( KIptvEpgContentDbDriveIdCol ),
                        aId.iDrive );
                    }

                if ( expression == CIptvMediaContent::ESample )
                    {
                    view.SetColL(
                        c->ColNo( KIptvEpgContentDbPreviewFileIdCol ),
                        aId.iFileId );
                    view.SetColL(
                        c->ColNo( KIptvEpgContentDbPreviewDriveIdCol ),
                        aId.iDrive );
                    }

                view.PutL();
                CleanupStack::PopAndDestroy( c );
                }
            else
                {
                IPTVLOGSTRING2_LOW_LEVEL(
                    "CIptvEpgDb::SetGlobalIdL NO CONTENT FOUND WITH ID %d", aContentKey );
                }
            }

        view.Close();
        }

    CleanupStack::PopAndDestroy( sql );
    }

// Engine 1.1

// --------------------------------------------------------------------------
// Start updating to temporary work database.
// --------------------------------------------------------------------------
//
void CIptvEpgDb::StartUpdateL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::StartUpdateL" );

    // In error case, work database may be left open.
    if ( iTmpDbOpen )
        {
        iTmpDb.Close();
        iTmpDbOpen = EFalse;
        }

    iTmpDbName.Zero();
    iTmpDbName.Copy( iEpgFile );
    iTmpDbName.Append( KIptvTmp );
    IPTVLOGSTRING2_LOW_LEVEL(
        "CIptvEpgDb::StartUpdateL -- TMP database is %S", &iTmpDbName );

    if ( BaflUtils::FileExists( iFsSession, iTmpDbName ) )
        {
        CFileMan* fileMan = CFileMan::NewL( iFsSession );
        fileMan->Delete( iTmpDbName );
        delete fileMan;
        }

    TBool created = EFalse;
    CreateDbL( iTmpDbName, created );
    OpenDatabaseL( iTmpDb, iTmpDbName );
    iTmpDbOpen = ETrue;

    // If new database created -> Set service information
    if ( created )
        {
        TTime a( TInt64( 0 ) );
        SetServiceInformationL( iTmpDb, a, a );
        CreateRootCategoryL( iTmpDb );
        created = EFalse;
        }
    }

// --------------------------------------------------------------------------
// Replace used database with work database just completed to update.
// --------------------------------------------------------------------------
//
void CIptvEpgDb::EndUpdateL( CIptvEpgSession& aSession )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::EndUpdateL() - Enter" );

    RPointerArray<CIptvMediaContent> contents;
    CleanupResetAndDestroyPushL( contents );
    RArray<TUint32> oldContentIds;
    CleanupClosePushL( oldContentIds );
    RArray<TUint32> newContentIds;
    CleanupClosePushL( newContentIds );

    // Copy global file ID's from EPG database to temporary array.
    GetGlobalFileIdsL( contents );

    // Close old epg database and delete it.
    iEpgDb.Close();
    CFileMan* fileMan = CFileMan::NewL( iFsSession );
    CleanupStack::PushL( fileMan );
    fileMan->Delete( iEpgFile );

    // Copy global file ID's to temporal DB.
    TInt count = contents.Count();
    IPTVLOGSTRING2_LOW_LEVEL(
        "CIptvEpgDb::EndUpdateL -- %d items were downloaded before update!",
        count );
    for ( TInt i = 0; i < count; i++ )
        {
        CopyGlobalFileIdToNewDbL( contents[i], oldContentIds, newContentIds );
        }

    // Rename temporary DB to EPG DB.
    iTmpDb.Close();
    iTmpDbOpen = EFalse;
    fileMan->Copy( iTmpDbName, iEpgFile );
    fileMan->Delete( iTmpDbName );
    iTmpDbName.Zero();
    OpenDatabaseL( iEpgDb, iEpgFile );


    // If everything went ok, update changed Content IDs to others.
    if ( ( oldContentIds.Count() > 0 ) &&
         ( oldContentIds.Count() == newContentIds.Count() ) )
        {
        aSession.UpdateChangedContentIdsL( oldContentIds, newContentIds );
        }

    CleanupStack::PopAndDestroy( fileMan );
    CleanupStack::PopAndDestroy( &newContentIds );
    CleanupStack::PopAndDestroy( &oldContentIds );
    CleanupStack::PopAndDestroy(); // contents
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::EndUpdateL() - Exit" );
    }

// --------------------------------------------------------------------------
// Finish database update and delete work database if still exists.
// --------------------------------------------------------------------------
//
void CIptvEpgDb::FinishUpdateL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::FinishUpdateL" );

    if ( iTmpDbOpen )
        {
        iTmpDb.Close();
        iTmpDbOpen = EFalse;
        CFileMan* fileMan = CFileMan::NewL( iFsSession );
        fileMan->Delete( iTmpDbName );
        iTmpDbName.Zero();
        delete fileMan;
        }
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::CopyGlobalFileIdToNewDbL()
// --------------------------------------------------------------------------
//
void CIptvEpgDb::CopyGlobalFileIdToNewDbL( CIptvMediaContent* aContent,
                                           RArray<TUint32>& aOldContentIds,
                                           RArray<TUint32>& aNewContentIds )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::CopyGlobalFileIdToNewDbL()" );
    IPTVLOGSTRING_LOW_LEVEL( "Copying old global file IDs to new DB from content." );

    if ( aContent )
        {
        aContent->PrintL();

        HBufC* sql = HBufC::NewLC(
            KCustomSqlMaxLength + aContent->GetMediaContentUrl().Length() );
        TPtr16 sqlPtr = sql->Des();
        TUint32 contentKey( KIptvInvalidContentId );

        sqlPtr.Append( KSqlSelect );
        sqlPtr.Append( KSqlAll );
        sqlPtr.Append( KSqlFrom );
        sqlPtr.Append( KIptvCATable );
        sqlPtr.Append( KSqlWhere );
        sqlPtr.Append( KIptvCAContentUrl );
        sqlPtr.Append( KSqlIs );
        sqlPtr.Append( KHyp );
        sqlPtr.Append( aContent->GetMediaContentUrl() );
        sqlPtr.Append( KHyp );

        TInt expression( KValueNotSet );
        RDbView view;
        TInt error = KErrNone;
        error = view.Prepare(
            iTmpDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );
        if ( error == KErrNone )
            {
            view.EvaluateAll();
            view.FirstL();
            TInt count = view.CountL();
            if ( count > 0 )
                {
                CDbColSet* cs = view.ColSetL();
                CleanupStack::PushL( cs );

                // Get expression & content key from DB.
                view.GetL();
                TInt expCol = cs->ColNo( KIptvCAExpression );
                expression = view.ColUint32( expCol );
                TInt contentCol = cs->ColNo( KIptvCAContentKey );
                contentKey = view.ColUint32( contentCol );

                // Update global file ID to DB.
                view.UpdateL();
                view.SetColL( cs->ColNo( KIptvCAFileId ), aContent->iFileId );
                view.SetColL( cs->ColNo( KIptvCADriveId ), aContent->iDriveId );
                view.PutL();

                CleanupStack::PopAndDestroy( cs );
                }
            }
        view.Close();

        // Store information about Content ID changes. Note that client
        // gets also a notification of removed items as new Content ID
        // can be KIptvInvalidContentId.
        if ( aContent->iContentKey != contentKey )
            {
            aOldContentIds.AppendL( aContent->iContentKey );
            aNewContentIds.AppendL( contentKey );
            }

        if ( ( expression == CIptvMediaContent::EFull ) ||
             ( expression == CIptvMediaContent::ESample ) )
            {
            sqlPtr.Zero();
            sqlPtr.Append( KSqlSelect );
            sqlPtr.Append( KSqlAll );
            sqlPtr.Append( KSqlFrom );
            sqlPtr.Append( KIptvEpgContentTable );
            sqlPtr.Append( KSqlWhere );
            sqlPtr.Append( KIptvEpgContentDbKeyCol );
            sqlPtr.Append( KSqlIs );
            sqlPtr.AppendNum( TInt64( contentKey ) );

            error = view.Prepare(
                iTmpDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

            if ( error == KErrNone )
                {
                view.EvaluateAll();
                view.FirstL();
                view.UpdateL();
                CDbColSet* colSet = view.ColSetL();
                CleanupStack::PushL( colSet );

                if ( expression == CIptvMediaContent::EFull )
                    {
                    view.SetColL(
                        colSet->ColNo( KIptvEpgContentDbFileIdCol ),
                        aContent->iFileId );
                    view.SetColL(
                        colSet->ColNo( KIptvEpgContentDbDriveIdCol ),
                        aContent->iDriveId );
                    }

                if ( expression == CIptvMediaContent::ESample )
                    {
                    view.SetColL(
                        colSet->ColNo( KIptvEpgContentDbPreviewFileIdCol ),
                        aContent->iFileId );
                    view.SetColL(
                        colSet->ColNo( KIptvEpgContentDbPreviewDriveIdCol ),
                        aContent->iDriveId );
                    }

                view.PutL();
                CleanupStack::PopAndDestroy( colSet );
                }

            view.Close();
            }

        CleanupStack::PopAndDestroy( sql );        
        }
    }

// --------------------------------------------------------------------------
// Fetch content access from database.
// --------------------------------------------------------------------------
//
CIptvMediaContent* CIptvEpgDb::GetContentAccessL(
    TUint32 aContentId, TUint32 aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetContentAccessL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KSqlAll );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvCATable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvCAContentKey );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aContentId ) );
    sqlPtr.Append( KSqlAnd );
    sqlPtr.Append( KIptvCAIndex );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aIndex ) );

    RDbView view;
    CIptvMediaContent* content = NULL;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        while ( view.AtRow() )
            {
            view.GetL();
            content = ReadContentAccessFromDbL( view );
            view.NextL();
            }
        }

    view.Close();

    return content;
    }

// --------------------------------------------------------------------------
// Fetch content access list from database.
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::GetContentAccessListL(
    TUint32 aContentId,
    RPointerArray<CIptvMediaContent>& aContents )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetContentAccessListL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KSqlAll );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvCATable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvCAContentKey );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aContentId ) );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        while ( view.AtRow() )
            {
            view.GetL();
            CIptvMediaContent* p = ReadContentAccessFromDbL( view );
            if ( p )
                {
                aContents.AppendL( p );
                }
            view.NextL();
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CIptvEpgDb::GetGlobalFileIdsL(
    RPointerArray<CIptvMediaContent>& aContents )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::GetGlobalFileIdsL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();
    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KSqlAll );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvCATable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvCAFileId );
    sqlPtr.Append( KSqlNot );
    sqlPtr.AppendNum( TInt64( 0 ) );
    sqlPtr.Append( KSqlOr );
    sqlPtr.Append( KIptvCADriveId );
    sqlPtr.Append( KSqlNot );
    sqlPtr.AppendNum( TInt64( 0 ) );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        view.FirstL();
        while ( view.AtRow() )
            {
            view.GetL();
            CIptvMediaContent* p = ReadContentAccessFromDbL( view );
            if ( p )
                {
                aContents.AppendL( p );
                }
            view.NextL();
            }
        }

    view.Close();
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
CIptvMediaContent* CIptvEpgDb::ReadContentAccessFromDbL(
    const RDbView& aView ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::ReadContentAccessFromDbL" );

    CIptvMediaContent* content = CIptvMediaContent::NewL();
    CleanupStack::PushL( content );

    CDbColSet* cs = aView.ColSetL();
    CleanupStack::PushL( cs );

    // Content key
    content->iContentKey = aView.ColUint32( cs->ColNo( KIptvCAContentKey ) );

    // Content URL
    RDbColReadStream readStream;
    TInt col = cs->ColNo( KIptvCAContentUrl );
    readStream.OpenLC( aView, col );
    TInt dataToCopy = Min( KIptvCAContentUrlLength, aView.ColLength( col ) );

    HBufC* url = HBufC::NewLC( dataToCopy );
    TPtr16 p = url->Des();

    readStream.ReadL( p, dataToCopy );
    content->SetMediaContentUrlL( p );

    CleanupStack::PopAndDestroy( url );
    readStream.Close();
    CleanupStack::Pop( &readStream );

    // Download type
    content->iDownloadType = aView.ColUint32( cs->ColNo( KIptvCADlType ) );

    // Language
    TPtrC lan;
    lan.Set( aView.ColDes( cs->ColNo( KIptvCALanguage ) ) );
    dataToCopy = Min( lan.Length(), KIptvCALanguageLength );
    content->iLanguage.Copy( lan.Mid( 0, dataToCopy ) );

    // Last play position
    content->iLastPosition = aView.ColUint32( cs->ColNo( KIptvCALastPlayPosition ) );

    // Mime type
    TPtrC mime;
    mime.Set( aView.ColDes( cs->ColNo( KIptvCAMimeType ) ) );
    dataToCopy = Min( mime.Length(), KIptvCAMimeTypeLength );
    content->iMimeType.Copy( mime.Mid( 0, dataToCopy ) );

    // File size
    content->iFileSize = aView.ColUint32( cs->ColNo( KIptvCAFileSize ) );

    // Duration
    content->iDuration = aView.ColUint32( cs->ColNo( KIptvCADuration ) );

    // File id
    content->iFileId = aView.ColUint32( cs->ColNo( KIptvCAFileId ) );

    // Drive id
    content->iDriveId = aView.ColUint32( cs->ColNo( KIptvCADriveId ) );

    // Index
    content->iIndex = aView.ColUint32( cs->ColNo( KIptvCAIndex ) );

    // Expression
    content->iExpression = aView.ColUint32( cs->ColNo( KIptvCAExpression ) );

    // Price
    // Not in media content

    // Purchase type
    // Not in mediacontent

    // Purchase URL
    // Not in media content

    // Last play position
    // Not in media content

    CleanupStack::PopAndDestroy( cs );
    CleanupStack::Pop( content );

    return content;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::SetLastPositionL(
    TUint32 aContentId, TUint32 aIndex, TUint32 aPosition )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::SetLastPositionL" );

    HBufC* sql = HBufC::NewLC( KCustomSqlMaxLength );
    TPtr16 sqlPtr = sql->Des();

    sqlPtr.Append( KSqlSelect );
    sqlPtr.Append( KSqlAll );
    sqlPtr.Append( KSqlFrom );
    sqlPtr.Append( KIptvCATable );
    sqlPtr.Append( KSqlWhere );
    sqlPtr.Append( KIptvCAContentKey );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aContentId ) );
    sqlPtr.Append( KSqlAnd );
    sqlPtr.Append( KIptvCAIndex );
    sqlPtr.Append( KSqlIs );
    sqlPtr.AppendNum( TInt64( aIndex ) );

    RDbView view;
    TInt error = view.Prepare(
        iEpgDb, TDbQuery( sqlPtr ), TDbWindow::EUnlimited );

    CleanupStack::PopAndDestroy( sql );

    if ( error == KErrNone )
        {
        view.EvaluateAll();
        if ( !view.IsEmptyL() )
            {
            view.FirstL();
            view.UpdateL();

            CDbColSet* cs = view.ColSetL();
            CleanupStack::PushL( cs );
            view.SetColL( cs->ColNo( KIptvCALastPlayPosition ), aPosition );
            CleanupStack::PopAndDestroy( cs );
            view.PutL();
            }
        }

    view.Close();

    return error;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
TInt CIptvEpgDb::SetMpxIdL( TUint32 aContentId,
                            TUint32 aCaIndex,
                            TUint32 aMpxId )
    {
    // check the final way to store mpx id.

    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::SetLastPositionL" );
    TInt error(KErrNone);
    CIptvMyVideosGlobalFileId* fileId =  CIptvMyVideosGlobalFileId::NewL();
    CleanupStack::PushL( fileId );
    
    fileId->iFileId = aMpxId;
    
    SetGlobalIdL( aContentId, *fileId, aCaIndex, error );
        
    CleanupStack::PopAndDestroy( fileId );

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgDb::SetUpdateTimeToServiceManagerL
// --------------------------------------------------------------------------
//
void CIptvEpgDb::SetUpdateTimeToServiceManagerL( const TTime& aTime )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::SetUpdateTimeToServiceManagerL" );

    CIptvService* service = iServer.iServiceManager->GetServiceL(
        iServiceId, ETrue /* do open/close */ );

    if ( service )
        {
        CleanupStack::PushL( service );
        service->SetEpgUpdateTimeL( aTime );
        iServer.iServiceManager->UpdateServiceL( *service );
        CleanupStack::PopAndDestroy( service );
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosManagerImpl::ReadCenRepData()
// CenRep
// -----------------------------------------------------------------------------
//
void CIptvEpgDb::ReadCenRepData()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::ReadCenRepData" );

    if ( iCenRepSession )
        {
        TInt age;
        if ( iCenRepSession->Get( KIptvCenRepParentControlKey, age ) != KErrNone )
            {
            age = KIptvCenRepParentControlKeyDefault;
            }

        iParentalControl = age;
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosManagerImpl::HandleNotifyInt()
// CenRep
// -----------------------------------------------------------------------------
//
void CIptvEpgDb::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::HandleNotifyInt" );

    if ( aId == KIptvCenRepParentControlKey )
        {
        iParentalControl = aNewValue;
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosManagerImpl::HandleNotifyGeneric()
// CenRep
// -----------------------------------------------------------------------------
//
void CIptvEpgDb::HandleNotifyGeneric( TUint32 aId )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::HandleNotifyGeneric" );

    if ( aId == NCentralRepositoryConstants::KInvalidNotificationId ||
         aId == KIptvCenRepParentControlKey )
        {
        ReadCenRepData();
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosManagerImpl::HandleNotifyError()
// CenRep
// -----------------------------------------------------------------------------
//
void CIptvEpgDb::HandleNotifyError(
    TUint32 /*aId*/, TInt /*error*/, CCenRepNotifyHandler* /*aHandler*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::HandleNotifyError" );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CIptvEpgDb::DeleteDatabaseFiles( TBool aDelete )
    {
    iDeleteDatabaseFiles = aDelete;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CIptvEpgDb::DeleteDatabaseL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgDb::DeleteDatabaseL" );

    // Delete database file
    TBuf<KIptvMaxPath> path;
    CIptvUtil::GetPathL( iFsSession, EIptvPathEcg, path, EDriveC, EFalse );
    path.Append( KIptvEpgDbName );

    TBuf<KIptvServiceIdMaxLength> service;
    service.Num( iServiceId );
    path.Append( service );

    if ( BaflUtils::FileExists( iFsSession, path ) )
        {
        TInt err = BaflUtils::DeleteFile( iFsSession, path );
        IPTVLOGSTRING3_LOW_LEVEL( "CIptvEpgDb::DeleteDatabaseL --> deleted database : %S, err=%d", &path, err );
        }

    // Delete content thumbnails
    path.Zero();
    CIptvUtil::GetPathL( iFsSession, EIptvPathEcg, path, EDriveC, EFalse );
    path.Append( service );
    path.Append( _L( "\\" ) );

    if ( BaflUtils::FolderExists( iFsSession, path ) )
        {
        CFileMan* fm = CFileMan::NewL( iFsSession );
        CleanupStack::PushL( fm );
        TInt error = fm->RmDir( path );
        if ( error == KErrNone )
            {
            IPTVLOGSTRING2_LOW_LEVEL( "CIptvEpgDb::DeleteDatabaseL --> deleted thumbnail directory : %S", &path );
            }
        CleanupStack::PopAndDestroy( fm );
        }

    // Delete service thumbnail
    path.Zero();
    CIptvUtil::GetPathL( iFsSession, EIptvPathWritableRoot, path, EDriveC, EFalse );
    path.Append( _L( "srv\\" ) );
    path.Append( service );
    path.Append( _L( ".*" ) );

    BaflUtils::DeleteFile( iFsSession, path );
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvEpgDb::DeleteDatabaseL --> deleted service thumbnail : %S", &path );
    }

