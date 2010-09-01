/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef VCXTESTMYVIDEOSCOLLECTIONCLIENT_H_
#define VCXTESTMYVIDEOSCOLLECTIONCLIENT_H_

// INCLUDES
#include <e32base.h>
#include "VCXTestMyVideosObserver.h"
#include "MIptvTestTimerObserver.h"

#include <mpxcollectionobserver.h>
#include <mpxmediabase.h>
#include <vcxmyvideosdefs.h>

// CONSTANTS
const TInt KVCXTestMpxLevelRoot          = 1;
const TInt KVCXTestMpxLevelCategory      = 2;
const TInt KVCXTestMpxLevelVideo         = 3;

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// CLASS DECLARATION
class MMPXCollectionUtility;
class CVCXTestCommon;
class CIptvTestActiveWait;
class CIptvTestTimer;

class TVCXTestDownload
    {
    public:
        TVCXTestDownload()
            {
            iMdsId = 0;
            iState = 0; 
            iDownloadId = 0;
            iProgress = 0;
            iDownloadError = 0;
            iGlobalError = 0;                
            }
        
        TInt32 iMdsId;
        TUint8 iState; 
        TUint32 iDownloadId;
        TInt8 iProgress;
        TInt32 iDownloadError;
        TInt32 iGlobalError;                
    };

/**
*  CVCXTestMyVideosCollectionClient 
*
*  @lib ?library
*  @since ?Series60_version
*/
class CVCXTestMyVideosCollectionClient : public CBase, public MMPXCollectionObserver, public MIptvTestTimerObserver
    {

public:  // Constructors and destructor
     
     /**
     * Two-phased constructor.
     */
     IMPORT_C static CVCXTestMyVideosCollectionClient* NewL( MVCXTestMyVideosObserver* aObserver );

     /**
     * Destructor.
     */
     virtual ~CVCXTestMyVideosCollectionClient();

private:
    
    /**
    * C++ default constructor.
    */
    CVCXTestMyVideosCollectionClient( MVCXTestMyVideosObserver* aObserver );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( );
    
public: // Functions from base classes.
    
    /**
    *  From MMPXCollectionObserver->MMPXCollectionMediaObserver
    *  Handle extended media properties
    *
    *  @param aMedia media
    *  @param aError error code
    */
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

    /**
    *  From MMPXCollectionUtilityObserver.
    *  Handle collection message
    *  NOTE: only one of HandleCollectionMessage callback can be implemented
    *
    *  @param aMessage collection message, ownership not transferred.
    *         Please check aMsg is not NULL before using it. If aErr is not
    *         KErrNone, plugin might still call back with more info in the aMsg.
    *  @param aErr system error code.
    */
    void HandleCollectionMessage( CMPXMessage* aMessage, TInt aError );

    void HandleSingleCollectionMessageL( CMPXMessage* aMessage );
    
    void HandleMyVideosMessageL( CMPXMessage* aMessage );
    void HandleGeneralMessageL( CMPXMessage* aMessage );
    void HandleItemChangedMessageL( CMPXMessage* aMessage );

    /**
    *  From MMPXCollectionUtilityObserver.
    *  Handles the collection entries being opened. Typically called
    *  when client has Open()'d a folder
    *
    *  @param aEntries collection entries opened
    *  @param aIndex focused entry
    *  @param aComplete ETrue no more entries. EFalse more entries
    *                   expected
    *  @param aError error code
    */
    void HandleOpenL( const CMPXMedia& aEntries,
                      TInt aIndex, TBool aComplete, TInt aError );

    /**
    *  From MMPXCollectionUtilityObserver.
    *  Handles the item being opened. Typically called
    *  when client has Open()'d an item. Client typically responds by
    *  'playing' the item via the playlist
    *
    *  @param aPlaylist collection playlist
    *  @param aError error code
    */
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError );

    /**
    *  From MMPXCollectionUtilityObserver.
    *  Handle completion of a asynchronous command
    *  @param aCommandResult result of the command, NULL if error
    *  @param aError error code
    */
    void HandleCommandComplete( CMPXCommand* aCommandResult,
                                TInt aError);

    /**
    * From MIptvTestTimerObserver Handles timer completion
    * @since
    * @param aTimerId
    * @param aError
    */
    void TimerComplete( TInt aTimerId, TInt aError );
        
private: // New functions.
    
    IMPORT_C void OpenCollectionL();
    
    IMPORT_C void OpenCollectionLevelL( TUint32 aIndex );
    
    IMPORT_C void UpdateVideoListL( TBool aComplete );
    
    IMPORT_C void UpdateDownloadListL();
    
    IMPORT_C void UpdateMediaArrayL();
    
    IMPORT_C void PrintMediasL( CMPXMediaArray* aMediaArray );
    
    IMPORT_C TBool UpdateDownloadL( CMPXMedia* aMedia );
    
    IMPORT_C static TBool CompareDownloads( const TVCXTestDownload& a1, const TVCXTestDownload& a2 );

    IMPORT_C CMPXCommand* CreateMpxCommandLC( TInt aCommandGeneralId, TInt aMyVideosCommandId, TBool aSync );    
    
public: // New functions.
    
    IMPORT_C void RefreshCollectionL();

    IMPORT_C TInt LevelsL();
    
    IMPORT_C TInt VideoCount();
    
    IMPORT_C CMPXMedia* GetVideoL( TUint aIndex );
    
    IMPORT_C void RemoveVideoL( TUint aIndex );
    
    IMPORT_C TInt DownloadCount();    
    
    IMPORT_C void DownloadL( const TDesC& aUrl, TUint aIapId, const TDesC& aUserName, const TDesC& aPassword );
    
    IMPORT_C void CancelDownloadL( TUint aIndex );
    
    IMPORT_C void EnsureMediaFilesAreNotInUseL();
    
    IMPORT_C TInt GetCountOfOngoingDownloads();
    
    IMPORT_C TInt GetCountOfNewVideos( TBool aIncludeDownloads );
    
private: // Data
    
    TInt iCollectionUid;
    
    MVCXTestMyVideosObserver* iObserver;
    
    TInt iOpenLevelIndex;
    
    // MMPXCollectionUtility.
    MMPXCollectionUtility* iCollectionUtility;
    
    // Entries from MPX plugin are saved here.
    CMPXMedia* iCurrentEntries;

    // Media array for iCurrentEntries.
    CMPXMediaArray* iMediaArray;
    
    // Media array for only videos.
    CMPXMediaArray* iVideos;
    
    // Media array for only downloads.
    CMPXMediaArray* iDownloads;
    
    // RFs.
    RFs iFs;

    // CVCXTestCommon.
    CVCXTestCommon* iTestCommon;

    // Refresh is already ongoing.
    TBool iRefreshingCollection;

    // Active wait.
    CIptvTestActiveWait* iActiveWait;

    // Timer.
    CIptvTestTimer* iTimer;
    
    // IDs of new videos from collection message KMPXMessageIdItemChanged are hold here until 
    // video list is refreshed.
    RArray<TInt> iInsertedVideoMdsIds;
    
    // Information of ongoing downloads.
    RArray<TVCXTestDownload> iOngoingDownloads;
    
    // Information of completed downloads.
    RArray<TVCXTestDownload> iCompletedDownloads;
    };

#endif /* VCXTESTMYVIDEOSCOLLECTIONCLIENT_H_ */
