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
* Description:    header file for CIptvEpgDb class. Engine version 1.1.*
*/




#ifndef CIPTVEPGDB_H
#define CIPTVEPGDB_H

#include <e32std.h>
#include <badesca.h>    // CDesCArrayFlat (cannot be forward declarated)
#include <d32dbms.h>    // RDbStoreDatabase
#include <f32file.h>    // RFs
#include "CIptvUtil.h"
#include "MIptvEpgVodCallback.h"
#include <cenrepnotifyhandler.h>

const int KCustomSqlMaxLength( 256 );
const TInt KIptvDbNameMaxLength( 50 );

// Category table
_LIT( KIptvEpgCategoryTable, "Cat" );
_LIT( KIptvEpgCategoryDbKeyCol, "Key" );
_LIT( KIptvEpgCategoryDbParentKeyCol, "PKey" );
_LIT( KIptvEpgCategoryDbIdCol, "Id" );
_LIT( KIptvEpgCategoryDbNameCol, "Name" );
_LIT( KIptvEpgCategoryDbIconFileIdCol, "IconId" ); // Obsolete field
_LIT( KIptvEpgCategoryDbIsIconFilePathUrlCol, "IsIconPathUrl" ); // Obsolete
_LIT( KIptvEpgCategoryDbIconFilePathCol, "IconPath" );
_LIT( KIptvEpgCategoryDbDescriptionCol, "Desc" );
_LIT( KIptvEpgCategoryDbPubDateCol, "PD" ); // Obsolete field
_LIT( KIptvEpgCategoryDbContentCountCol, "ContentCount" );

// Content table
_LIT( KIptvEpgContentTable, "Con" );
_LIT( KIptvEpgContentDbKeyCol, "Key" );
_LIT( KIptvEpgContentDbIdCol, "Id" );
_LIT( KIptvEpgContentDbNameCol, "Name" );
_LIT( KIptvEpgContentDbFileIdCol, "FID" );
_LIT( KIptvEpgContentDbDriveIdCol, "Dr" );
_LIT( KIptvEpgContentDbPreviewFileIdCol, "PFID" );
_LIT( KIptvEpgContentDbPreviewDriveIdCol, "PDr" );
_LIT( KIptvEpgContentDbContentProtectionCol, "CP" );
_LIT( KIptvEpgContentDbIconFileIdCol, "IconId" );
_LIT( KIptvEpgContentDbIsIconFilePathUrlCol, "IsIconPathUrl" );
_LIT( KIptvEpgContentDbIconFilePathCol, "IconPath" );
_LIT( KIptvEpgContentDbContentFileIdCol, "ConId" );
_LIT( KIptvEpgContentDbDescriptionCol, "Desc" );
_LIT( KIptvEpgContentDbPubDateCol, "PD" );
_LIT( KIptvEpgContentDbBrowserUrlCol, "BU" );
_LIT( KIptvEpgContentDbOrderCol, "Ord" );
_LIT( KIptvEpgContentDbAuthorCol, "Aut" );
_LIT( KIptvEpgContentDbCopyrightCol, "Cop" );
_LIT( KIptvEpgContentDbSizeCol, "Siz" );
_LIT( KIptvEpgContentDbDurationCol, "Dur" );
_LIT( KIptvEpgContentDbLanguageCol, "Lan" );
_LIT( KIptvEpgContentDbRatingAgeCol, "RatingAge" );

// Category_Content table
_LIT( KIptvEpgCategoryContentTable, "CategoryContent" );
_LIT( KIptvEpgCategoryContentCategoryKeyCol, "CategoryKey" );
_LIT( KIptvEpgCategoryContentContentKeyCol, "ContentKey" );

// Service table
_LIT( KIptvEpgServiceTable, "Service" );
_LIT( KIptvEpgServiceLastUpdate, "LastUpdate" );
_LIT( KIptvEpgServiceNextUpdate, "NextUpdate" );
_LIT( KIptvEpgServiceETag, "ETag" );
_LIT( KIptvEpgServiceLastModifiedSince, "LastModifiedSince" );
_LIT( KIptvEpgServiceXmlFileHash, "EXmlFileHash" );
_LIT( KIptvEpgServiceVersion, "Version" );

// ContentAccess
_LIT( KIptvCATable, "ContentAccess" );
_LIT( KIptvCAContentKey, "ContentKey" );
_LIT( KIptvCAContentUrl, "ContentUrl" );
_LIT( KIptvCADlType, "DlType" );
_LIT( KIptvCALanguage, "Language" );
_LIT( KIptvCAPrice, "Price" );
_LIT( KIptvCAPurchaseType, "PurchaseType" );
_LIT( KIptvCAPurchaseUrl, "PurchaseUrl" );
_LIT( KIptvCALastPlayPosition, "LastPlayPosition" );
_LIT( KIptvCAMimeType, "MimeType" );
_LIT( KIptvCAFileSize, "FileSize" );
_LIT( KIptvCADuration, "Duration" );
_LIT( KIptvCAFileId, "FileId" );
_LIT( KIptvCADriveId, "DriveId" );
_LIT( KIptvCAIndex, "Index" );
_LIT( KIptvCAExpression, "Expression" );

// Name of the category Key index
_LIT( KIptvEpgCategoryDbIndexName, "CategoryIndex" );

// Name of the content Key index
_LIT( KIptvEpgContentDbIndexName, "ContentIndex" );

// Name of the CategoryContent Key index
_LIT( KIptvEpgCategoryContentDbIndexName, "CatConIndex" );

class CIptvEpgCategory;
class CIptvEpgContent;
class CIptvVodContentCategoryBriefDetails;
class CIptvVodContentContentBriefDetails;
class CIptvVodContentContentFullDetails;
class CIptvMyVideosGlobalFileId;
class CIptvServer;
class CIptvEpgSession;

/**
 *
 * An instance of class CIptvEpgDb provides Epg database access creating and
 * manipulating Epg database files and database entries (categories and
 * contents.
 *
 * Database definition:
 *
 *   The Epg database contains one table as follows:
 *
 *   Table name: Category
 *     Column:            Type:          Length (bytes):
 *     ------             -----          ---------------
 *     Key                EDbColUint32   4
 *     ParentKey          EDbColUint32   4
 *     Id                 EDbColText     KIptvIdMaxLength * 2
 *     Name               EDbColText     KIptvEpgCategoryNameMaxLength * 2
 *     IconFileId         EDbColUint32   4
 *     IsIconFilePathUrl  EDbColUint32   4
 *     IconFilePath       EDbColText     KIptvEpgCategoryIconFilePathMaxLength * 2
 *     PubDate            EDbColDateTime 8
 *     Description        EDbColText     KIptvEpgCategoryDescriptionMaxLength * 2
 *     contentCount       EDbColUint32   4
 *
 *   There is also index for Category table with name "CategoryIndex". It
 *   consists of Key column.
 *
 *   Index provides quick find.
 *
 *
 * Database definition:
 *
 *   The Epg database contains one table as follows:
 *
 *   Table name: Content
 *     Column:              Type:          Length (bytes):
 *     ------               -----          ---------------
 *     Key                  EDbColUint32   4
 *     Id                   EDbColText     KIptvIdMaxLength * 2
 *     Name                 EDbColText     KIptvEpgContentNameMaxLength * 2
 *     FileId               EDbColUint32   4
 *     Drive                EDbColUint32   4
 *     PFileId              EDbColUint32   4
 *     PDrive               EDbColUint32   4
 *     ContentProtection    EDbColUint32   4 (NOT USED)
 *     IconFileId           EDbColUint32   4
 *     IsIconFilePathUrl    EDbColUint32   4
 *     IconFilePath         EDbColText     KIptvEpgContentIconFilePathMaxLength * 2
 *     ContentFileId        EDbColUint32   4
 *     Description          EDbColText     KIptvEpgContentDescriptionMaxLength * 2
 *     PubDate              EDbColDateTime 8
 *     BrowserUrl           EDbColText     KIptvEpgContentBrowserUrlMaxLength * 2
 *     Order                EDbColUint32   4
 *     Author               EDbColText     KIptvEpgContentAuthorLength * 2
 *     Copyright            EDbColText     KIptvEpgContentCopyrightLength * 2
 *     Size                 EDbColUint32   4
 *     Duration             EDbColUint32   4
 *     Language             EDbColText     KIptvCALanguageLength * 2
 *     RatingAge            EDbColUint16   2
 *
 *   Missing from content table, not implemented in increment 1:
 *
 *   - Next postponed download time
 *   - Next scheduled download time
 *
 *   There is also index for Content table with name "ContentIndex". It
 *   consists of Key column.
 *
 *   Index provides quick find.
 *
 * Database definition:
 *
 *   The Epg database contains one table as follows:
 *
 *   Table name: CategoryContent
 *     Column:              Type:          Length (bytes):
 *     ------               -----          --------------
 *     CategoryKey          EDbColUint32   4
 *     ContentKey           EDbColUint32   4
 *
 *
 *
 * Database definition:
 *
 *   The Epg database contains one table as follows:
 *
 *   Table name: Service
 *     Column:              Type:          Length (bytes):
 *     ------               -----          ---------------
 *     LastUpdate           EDbColDateTime   8
 *     NextUpdate           EDbColDateTime   8
 *     ETag                 EDbColText       64
 *     LastModifiedSince    EDbColText       64
 *     EXmlFileHash         EDbColText       20
 *     Version              EDbColUint32     4
 *
 * Database definition:
 *
 *   The Epg database contains one table as follows:
 *
 *   Table name: ContentAccess
 *     Column:              Type:          Length (bytes):
 *     ------               -----          ---------------
 *     ContentKey           EDbColUint32     4
 *     ContentUrl           EDbColLongText   KIptvCAContentUrlLength * 2
 *     DlType               EDbColUint32     4
 *     Language             EDbColText       KIptvCALanguageLength * 2
 *     Price                EDbColText       KIptvCAPriceLength * 2
 *     PurchaseType         EDbColUint32     4
 *     PurchaseURL          EDbColLongText   KIptvCAPurchaseUrlLength * 2
 *     LastPlayPosition     EDbColUint32     4
 *     MimeType             EDbColText       KIptvCAMimeTypeLength * 2
 *     FileSize             EDbColUint32     4
 *     Duration             EDbColUint32     4
 *     FileId               EDbColUint32     4
 *     DriveId              EDbColUint32     4
 *     Index                EDbColUint32     4
 *     Expression           EDbColUint32     4
 *
 *  @lib IptvServer.exe
 *  @since Engine 1.0
 */
class CIptvEpgDb : public CBase,
                   public MCenRepNotifyHandlerCallback
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aEpgFile Database file name.
     * @param aServiceId Service id.
     * @param aServer Server.
     */
    static CIptvEpgDb* NewL(
        const TFileName& aEpgFile,
        const TUint32& aServiceId,
        CIptvServer& aServer );

    /**
     * Destructor.
     */
    ~CIptvEpgDb();

    // Query methods for EpgMsgHandler from VodContentClient interface

    /**
     * Get list of ECG categories.
     * The ownership of the returned list is transferred to caller.
     * The order of categories is defined by database originated from
     * service provider.
     *
     * @param aParentCategoryId Points to category ID which sub-categories
     *                          should be listed. If NULL, the root is
     *                          referenced.
     * @param aCategoryList Reference to a storage where pointer to
     *                      created category list is returned.
     * @return Error code, KErrNone if no errors,
     *                     KErrCouldNotConnect if server is needs
     *                       permission for connection,
     *                     otherwise one of the system wide error codes.
     */
    TInt GetEcgCategoryListL(
        TIptvCategoryId aParentCategoryId,
        RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList );

    /**
     * Get detais of one category item.
     *
     * @since                Engine 1.1
     * @param aCategoryId    Selected category ID identifier.
     * @param aCategory      Reference to a full details data returned.
     * @return               Error code, KErrNone if no errors.
     */
    TInt GetCategoryDetailsL(
        TIptvCategoryId aCategoryId,
        CIptvVodContentCategoryBriefDetails& aCategory );

    /**
     * Get parent category ID identifier.
     * This method allows application to browse toward root category,
     * then two consecutive calls are needed.
     *
     * @param aCategoryId Points to category ID which parent category is
     *                    requested.
     * @param aParentId Reference to a storage where parent ID is returned.
     * @return Error code, KErrNone if no errors,
     *                     KErrCouldNotConnect if server is needs permission
     *                                         for connection,
     *                     otherwise one of the system wide error codes.
     */
    TInt GetParentCategoryL(
        TIptvCategoryId aCategoryId,
        TIptvCategoryId& aParentId );

    /**
     * Get list of ECG contents in the specified category in order defined
     * by database originated from service provider.
     * The ownership of the returned list is transferred to caller.
     *
     * @param aCategoryId Selected category ID identifier
     * @param aSearchString A search string used to filter contents search.
     *                      If NULL, no filtering issued.
     * @param aFrom The start index for listed contents. If NULL, start from
     *              the first one.
     * @param aAmount Amount of content elementes to be listes. If NULL,
     *                all starting from aFrom.
     * @param aTotalAmount Pointer to a variable where total amount of
     *                     content is returned.
     * @param aEcgList Reference to returned ECG list items.
     * @return Error code, KErrNone if no errors,
     *                     KErrCouldNotConnect if server is needs permission
     *                                         for connection,
     *                     otherwise one of the system wide error codes.
     */
    TInt GetEcgListL(
        TIptvCategoryId aCategoryId,
        const TDesC& aSearchString,
        TUint32 aFrom,
        TUint32 aAmount,
        TUint32& aTotalAmount,
        RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList );

    /**
     * Get list of all ECG contents in alphabetical order.
     * The ownership of the returned list is transferred to caller.
     *
     * @param aSearchString A search string used to filter contents search.
     *                      If NULL, no filtering issued.
     * @param aFrom The start index for listed contents.
     *              If NULL, start from the first one.
     * @param aAmount Amount of content elementes to be listes.
     *                If NULL, all starting from aFrom.
     * @param aTotalAmount Pointer to a variable where total amount of
     *                     content is returned.
     * @param aEcgList Reference to returned ECG list items.
     * @return Error code, KErrNone if no errors,
     *                     KErrCouldNotConnect if server is needs permission
     *                                         for connection,
     *                     otherwise one of the system wide error codes.
     */
    TInt GetEcgAllListL(
        const TDesC& aSearchString,
        TUint32 aFrom,
        TUint32 aAmount,
        TUint32& aTotalAmount,
        RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList );

    /**
     * Get full detais of one content item.
     * The ownership of the returned data is transferred to caller.
     *
     * @param aContentId Selected content ID identifier.
     * @param aContentFullDetails Reference to returned full details data.
     * @return Error code, KErrNone if no errors,
     *                     KErrCouldNotConnect if server is needs permission
     *                                         for connection,
     *                     otherwise one of the system wide error codes.
     */
    TInt GetContentDetailsL(
        TIptvContentId aContentId,
        CIptvVodContentContentFullDetails& aContentFullDetails );
    
    /**
     * Marks the database files to be deleted after database has been closed.
     * @param aDelete ETrue to delete the files (default value), EFalse to clear this flag.
     */
    void DeleteDatabaseFiles( TBool aDelete = ETrue );

// from MIptvEpgPluginManager interface

    /**
     * Add new category to database.
     *
     * @param aCategory     Reference to CIptvEpgCategory object.
     * @param aCategoryKey  New category key value is inserted to
     *                      aCategoryKey
     * @return              KErrNone if successful, otherwise another of the
     *                      system-wide error codes.
     */
    TInt AddCategoryL(
        CIptvEpgCategory& aCategory,
        TUint32& aCategoryKey );

    /**
     * Add relation between category and content.
     *
     * @param aCategoryKey  Category key.
     * @param aContentKey   Content key.
     */
    TInt AddRelationL( TUint32& aCategoryKey, TUint32& aContentKey );

    /**
     * Add new content to given category.
     *
     * @param aContent        Reference to CIptvEpgContent object.
     * @param aMediaContents  Pointer array containing media content objects.
     *                        There can be n media content's for one content.
     * @param aContentKey     New content key value is inserted to
     *                        aContentKey
     * @return                KErrNone if successful, otherwise another of
     *                        the system-wide error codes.
     */
    TInt AddContentL(
        CIptvEpgContent& aContent,
        RPointerArray<CIptvMediaContent> aMediaContents,
        TUint32& aContentKey );

    /**
     * Update category icon.
     *
     * @param aCategoryKey      Category key.
     * @param aIconFileId       Icon file id to search from the database.
     * @param aIconFilePath     Full path to icon file or URL + filename.
     * @param aIsUrl            If aIconFilePath is URL aIsUrl == ETrue,
     *                          if aIconFilePath is path
     *                          to local filesystem aIsUrl == EFalse.
     * @return                  KErrNone if successful, otherwise another of
     *                          the system-wide error codes.
     */
    TInt UpdateCategoryIconL(
        TUint32& aCategoryKey,
        TUint32& aIconFileId,
        const TDesC& aIconFilePath,
        TBool aIsUrl );

    /**
     * Update content icon.
     *
     * @param aCategoryKey      Content key.
     * @param aIconFileId       Icon file id to search from the database.
     * @param aIconFilePath     Full path to icon file or URL + filename.
     * @param aIsUrl            If aIconFilePath is URL aIsUrl == ETrue,
     *                          if aIconFilePath is path
     *                          to local filesystem aIsUrl == EFalse.
     * @return                  KErrNone if successful, otherwise another of
     *                          the system-wide error codes.
     */
    TInt UpdateContentIconL(
        TUint32& aContentKey,
        TUint32& aIconFileId,
        const TDesC& aIconFilePath,
        TBool aIsUrl );

    /**
     * Get service update information.
     *
     * @param aLastUpdate   Last update time.
     * @param aInterval     Update interval.
     */
    TInt GetServiceInformationL(
        TTime& aLastUpdate,
        TTime& aInterval);

    /**
     * Update service update information.
     *
     * @param aLastUpdate   Last update time.
     * @param aInterval     Update interval.
     */
    TInt UpdateServiceInformationL(
        const TTime& aLastUpdate,
        const TTime& aInterval );

    /**
     * Get service update information for HTTP headers.
     *
     * @param aETag                 ETag
     * @param aLastModifiedSince    LastModifiedSince
     */
    TInt GetServiceETagAndLastmodifiedSinceL(
        TDes& aETag,
        TDes& aLastModifiedSince );

    /**
     * Update service update information for HTTP headers.
     *
     * @param aETag                 ETag
     * @param aLastModifiedSince    LastModifiedSince
     */
    TInt UpdateServiceETagAndLastModifiedSinceL(
        const TDesC& aETag,
        const TDesC& aLastModifiedSince );

    /**
     * Get hash for service XML file.
     *
     * @param aHash    Hash string
     */
    TInt GetServiceXmlFileHashL(
        TDes& aHash);

    /**
     * Update hash for service XML file.
     *
     * @param aHash    Hash string
     */
    TInt UpdateServiceXmlFileHashL(
        const TDesC& aHash);

    /**
     * Reset global Id.
     *
     * @param aId           Global file id to be reset.
     */
    TInt ResetGlobalId( CIptvMyVideosGlobalFileId& aId );

    /**
     * Set global Id.
     *
     * @param aContentKey   Key of content to be updated.
     * @param aId           Global file id to be set.
     * @param aIndex        Index of content to be updated.
     * @return KErrNone if successful, otherwise another of the system-wide
     *         error codes.
     */
    TInt SetGlobalId(
        TUint32 aContentKey,
        CIptvMyVideosGlobalFileId& aId,
        TUint32 aIndex );

// Engine 1.1

    /**
     * Start to update database, firstly to a temporaty work version.
     */
    void StartUpdateL();

    /**
     * Replace the database in use with the newly updated one.
     *
     * @param aSession Related EPG session.
     */
    void EndUpdateL( CIptvEpgSession& aSession );

    /**
     * Finish database update and delete work database if still exists.
     */
    void FinishUpdateL();

    /**
     * Fetch content access list from database.
     *
     * @param aContentId    Id of required content access list.
     * @param aContents     Required content access list.
     * @return              Completion status.
     */
    TInt GetContentAccessListL(
        TUint32 aContentId,
        RPointerArray<CIptvMediaContent>& aContents );

    /**
     * Fetch content access from database.
     *
     * @param aContentId    Id of required content access.
     * @param aIndex        Index of required content access.
     * @return              Required content access.
     */
    CIptvMediaContent* GetContentAccessL(
        TUint32 aContentId,
        TUint32 aIndex );

    /**
     * Set last play position of the video.
     *
     * @param aContentId    Id of position to be set.
     * @param aIndex        Index of position to be set.
     * @param aPosition     New position.
     * @return              Completion status.
     */
    TInt SetLastPositionL(
        TUint32 aContentId,
        TUint32 aIndex,
        TUint32 aPosition );
    
    /**
     * Set mpx id for content.
     *
     * @param aContentId    Id of position to be set.
     * @param aCaIndex      Index of position to be set.
     * @param aMpxId        Mpx Id.
     * @return              Completion status.
     */
    TInt SetMpxIdL( TUint32 aContentId, TUint32 aCaIndex, TUint32 aMpxId );

    /**
     * Set update time to service manager.
     *
     * @param aTime         New update time.
     */
    void SetUpdateTimeToServiceManagerL( const TTime& aTime );

// From MCenRepNotifyHandlerCallback

    /**
     * CenRep callback.
     *
     * @param aId           CenRep key id.
     * @param aNewValue     CenRep key new value.
     */
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );

    /**
     * CenRep callback.
     *
     * @param aId           CenRep key id.
     * @param error         CenRep failure code.
     * @param aHandler      Handler.
     */
    void HandleNotifyError(
        TUint32 aId,
        TInt error,
        CCenRepNotifyHandler* aHandler );

    /**
     * CenRep callback.
     *
     * @param aId CenRep key id.
     */
    void HandleNotifyGeneric( TUint32 aId ) ;

private:

    /**
     * Default constructor.
     *
     * @param aServiceId    Service id.
     * @param aServer       Server.
     */
    CIptvEpgDb(
        const TUint32& aServiceId,
        CIptvServer& aServer );

    /**
     * ConstructL
     * @param aEpgFile      Full path to DB file.
     * @param aDbCreated    Database was created.
     */
    void ConstructL( const TFileName& aEpgFile );

    /**
     * Initialize DB.
     */
    void InitDbL();

    /**
     *
     * Create new Epg database if DB file is not found.
     *
     * @param aFile         Database file name.
     * @param aCreated      Database was really created.
     */
    void CreateDbL( const TDesC& aFile, TBool& aCreated );

    /**
     * Create category table to DB.
     *
     * @param aDatabase     Handle to database.
     * @return              Completion status.
     */
    TInt CreateCategoryTableL( RDbNamedDatabase& aDatabase ) const;

    /**
     * Create index for category table.
     *
     * @param aDatabase     Handle to database.
     */
    void CreateCategoryIndexL( RDbNamedDatabase& aDatabase ) const;

    /**
     * Create content table to DB.
     *
     * @param aDatabase     Handle to database.
     * @return              Completion status.
     */
    TInt CreateContentTableL( RDbNamedDatabase& aDatabase ) const;

    /**
     * Create index for content table.
     *
     * @param aDatabase     Handle to database.
     */
    void CreateContentIndexL( RDbNamedDatabase& aDatabase ) const;

    /**
     * Create CategoryContent table to DB.
     *
     * @param aDatabase     Handle to database.
     */
    void CreateCategoryContentTableL( RDbNamedDatabase& aDatabase ) const;

    /**
     * Create index for CategoryContent table.
     *
     * @param aDatabase     Handle to database.
     */
    void CreateCategoryContentIndexL( RDbNamedDatabase& aDatabase ) const;

    /**
     * Create Service table to DB.
     *
     * @param aDatabase     Handle to database.
     * @return              Completion status.
     */
    TInt CreateServiceTableL( RDbNamedDatabase& aDatabase ) const;

    /**
     * Create ContentAccess table to DB.
     *
     * @param aDatabase     Handle to database.
     * @return              Completion status.
     */
    TInt CreateContentAccessTableL( RDbNamedDatabase& aDatabase ) const;

    /**
     * This method combines two GetCategoryKeyL methods to one method
     * (with parameters TDesC& aValue and TUint32& aValue).
     *
     * Get category key for given category defined by aField, aDesValue or
     * aIntValue. Key is databases internal autoincrement which is unique
     * for every category. If category is already in the database key value
     * is inserted to aKey parameter and aUpdateCategory is set to ETrue.
     *
     * If category is not in the database, aUpdateCategory parameter is
     * EFalse.
     *
     * @param aField        TCategorySearchField enumeration defines the
     *                      search field.
     * @param aDesValue     Value which is usedas a "search key" if it is
     *                      type TDesC.
     * @param aIntValue     Value which is usedas a "search key" if it is
     *                      type TUint32.
     * @param aKey          Category key if given category found from the
     *                      database.
     * @param aUpdateCategory  ETrue if category can be updated, EFalse if
     *                      new category must be created to database.
     * @return              KErrNone if successful, otherwise another of the
     *                      system-wide error codes.
     */
    TInt GetCategoryKeyL(
        TCategorySearchField aField,
        TDesC& aDesValue,
        TUint32& aIntValue,
        TUint32& aKey,
        TBool& aUpdateCategory );

    /**
     * Set service update information.
     *
     * @param aLastUpdate   Last update time.
     * @param aInterval     Update interval.
     * @param aUpdateSet    Update set.
     * @return              Completion status.
     */
    TInt SetServiceInformationL(
        RDbNamedDatabase& aDb,
        const TTime& aLastUpdate,
        const TTime& aInterval) const;

    /**
     * Initialize common SQL command strings.
     */
    void MakeSqlStrings();

    /**
     * Store category to database..
     *
     * @param aView         Predefined database view.
     * @param aCategory     Category descriptor.
     */
    void WriteCategoryToDbL(
        RDbView& aView,
        CIptvEpgCategory& aCategory ) const;

    /**
     * Store video content to database.
     *
     * @param aView         Predefined database view.
     * @param aContent      Content descriptor.
     * @param aLeave        Disables global file id store.
     */
    void WriteContentToDbL(
        RDbView& aView,
        CIptvEpgContent& aContent,
        TBool aLeave ) const;

    /**
     * Read category from database.
     *
     * @param aView         Predefined database view.
     * @param aCategory     Category descriptor.
     */
    void ReadCategoryFromDbL(
        const RDbView& aView,
        CIptvVodContentCategoryBriefDetails& aCategory ) const;

    /**
     * Read videos short details from database.
     *
     * @param aView         Predefined database view.
     * @param aBriefDetails Content short descriptor
     */
    void ReadContentBriefDetailsFromDbL(
        const RDbView& aView,
        CIptvVodContentContentBriefDetails& aBriefDetails ) const;

    /**
     * Read videos full details from database.
     *
     * @param aView         Predefined database view.
     * @param aContentFullDetails Content full descriptor.
     */
    void ReadContentFullDetailsFromDbL(
        const RDbView& aView,
        CIptvVodContentContentFullDetails& aContentFullDetails ) const;

    /**
     * Fetch content access from database.
     *
     * @param aView         Predefined database view.
     * @return              Required content access.
     */
    CIptvMediaContent* ReadContentAccessFromDbL(
        const RDbView& aView ) const;

    /**
     * Create root category to database.
     *
     * @param aDb           Handle to database.
     */
    void CreateRootCategoryL( RDbNamedDatabase& aDb ) const;

    /**
     * Check whether selected content has been downloaded.
     *
     * @param aView         Predefined database view.
     * @return True indicates download.
     */
    TBool IsDownloadedL( const RDbView& aView ) const;

    /**
     * Open existing database.
     *
     * @param aDb           Handle to database.
     * @param aFile         database file.
     */
    void OpenDatabaseL( RDbNamedDatabase& aDb, const TDesC& aFile );

    /**
     * Get database version.
     *
     * @return              Required version.
     */
    TInt GetVersionL();

    /**
     * Write content access to database.
     *
     * @param aView         Predefined database view.
     * @param aMediaContent Content descriptor.
     * @param aContentKey   Content key.
     * @param aIndex        Content index.
     */
    void WriteContentAccessToDbL(
        RDbView& aView,
        const CIptvMediaContent* aMediaContent,
        TUint32 aContentKey,
        TUint32 aIndex ) const;

    /**
     * Add category to database.
     *
     * @param aDb           Handle to database.
     * @param aCategory     Category descriptor.
     * @param aCategoryKey  Category key.
     * @return              Completion status.
     */
    TInt AddCategoryL(
        RDbNamedDatabase& aDb,
        CIptvEpgCategory& aCategory,
        TUint32& aCategoryKey ) const;

    /**
     * Get list of contents associated with global file id.
     *
     * @param aContents     .Contents list.
     */
    void GetGlobalFileIdsL( RPointerArray<CIptvMediaContent>& aContents );

    /**
     * Copy content associated with global file Id. Also gathers a list of changed
     * Content IDs so that clients having old information can be notified.
     *
     * @param aContent Content descriptor.
     * @param aOldContentIds Array for old Content IDs.
     * @param aNewContentIds Array for new Content IDs.
     */
    void CopyGlobalFileIdToNewDbL( CIptvMediaContent* aContent,
                                   RArray<TUint32>& aOldContentIds,
                                   RArray<TUint32>& aNewContentIds );

    /**
     * Reads CenRep data to internal variables.
     */
    void ReadCenRepData();

    /**
     * Reset global Id.
     *
     * @param aId           Global file id to be reset.
     * @param aError        Completion status.
     */
    void ResetGlobalIdL(
        CIptvMyVideosGlobalFileId& aId,
        TInt& aError );

    /**
     * Set global Id.
     *
     * @param aContentKey   Key of content to be updated.
     * @param aId           Global file id to be set.
     * @param aIndex        Index of content to be updated.
     * @param aError        Completion status.
     * @return KErrNone if successful, otherwise another of the system-wide
     *         error codes.
     */
    void SetGlobalIdL(
        TUint32 aContentKey,
        CIptvMyVideosGlobalFileId& aId,
        TUint32 aIndex,
        TInt& aError );

    /**
     * Deletes EPG-database from file system.
     */
    void DeleteDatabaseL();

private: // Data

    /**
     * File system session.
     */
    RFs iFsSession;

    /**
     * Database file.
     */
    TFileName iEpgFile;

    /**
     * Database session.
     */
    RDbNamedDatabase iEpgDb;

    /**
     * Temporary database for update.
     */
    RDbNamedDatabase iTmpDb;

    /**
     * Temp database is open.
     */
    TBool iTmpDbOpen;

    /**
     * Category SQL-query.
     */
    TBuf<KCustomSqlMaxLength> iSqlCategory;

    /**
     * Content SQL-query.
     */
    TBuf<KCustomSqlMaxLength> iSqlContent;

    /**
     * Service id.
     */
    TUint32 iServiceId;

    /**
     * Temporary database name.
     */
    TBuf<KIptvDbNameMaxLength> iTmpDbName;

    /**
     * Server.
     */
    CIptvServer& iServer;

    /**
     * User age to check rating age limit limit.
     */
    TUint16 iParentalControl;

    /**
     * Central Repository session.
     */
    CRepository* iCenRepSession;

    /**
     * Central Repository handler.
     */
    CCenRepNotifyHandler* iCenRepNotifyHandler;

    /**
     * Flag that tells if database is to be deleted from file system after close.
     */
    TBool iDeleteDatabaseFiles;

    };

#endif // CIPTVEPGDB_H
