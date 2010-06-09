/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: This file contains testclass implementation.
*
*/

// Version : %version: 6 %

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>

#include <mpxlog.h>
#include <mdeconstants.h>
#include <mdequery.h>
#include <bautils.h>
#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxmediaarray.h>
#include <mpxmediavideodefs.h>
#include <centralrepository.h>

#include <mdesession.h>
#include <mdequery.h>
#include <harvesterclient.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>
#include <e32property.h>
#include "vcxmyvideosalbum.h"

#define protected public
#define private public
#include "vcxmyvideosmdsdb.h"
#undef private
#undef protected

#include "vcxmyvideoscollectionutil.h"


#include "VcxMyVideosMdsDbTest.h"


        
// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//

TInt NextIntOrZero( CStifItemParser& aItem ) 
    {
    TInt a ( 0 );
    if ( !aItem.GetNextInt( a ) )
        {
        return a;
        }
    else
        {
        return 0;
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxMyVideosMdsDbTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CVcxMyVideosMdsDbTest::Delete() 
    {
    delete iVideoList;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosMdsDbTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVcxMyVideosMdsDbTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        ENTRY( "CreateL", CVcxMyVideosMdsDbTest::CreateL ),
        ENTRY( "DeleteL", CVcxMyVideosMdsDbTest::DeleteL ),
        ENTRY( "CancelL", CVcxMyVideosMdsDbTest::CancelL ),
        ENTRY( "AddVideoL", CVcxMyVideosMdsDbTest::AddVideoL ),
        ENTRY( "RemoveVideoL", CVcxMyVideosMdsDbTest::RemoveVideoL ),
        ENTRY( "UpdateVideoL", CVcxMyVideosMdsDbTest::UpdateVideoL ),
        ENTRY( "CreateVideoListL", CVcxMyVideosMdsDbTest::CreateVideoListL ),
        ENTRY( "DoCreateVideoListL", CVcxMyVideosMdsDbTest::DoCreateVideoListL ),
        ENTRY( "CreateVideoL", CVcxMyVideosMdsDbTest::CreateVideoL ),
        ENTRY( "HandleQueryNewResultsL", CVcxMyVideosMdsDbTest::HandleQueryNewResultsL ),
        ENTRY( "HandleQueryCompletedL", CVcxMyVideosMdsDbTest::HandleQueryCompletedL ),
        ENTRY( "AsyncHandleQueryCompletedL", CVcxMyVideosMdsDbTest::AsyncHandleQueryCompletedL ),
        ENTRY( "HandleSessionErrorL", CVcxMyVideosMdsDbTest::HandleSessionErrorL ),
        ENTRY( "HandleObjectNotificationL", CVcxMyVideosMdsDbTest::HandleObjectNotificationL ),
        ENTRY( "HandleObjectPresentNotification", CVcxMyVideosMdsDbTest::HandleObjectPresentNotification ),
        ENTRY( "ShutdownNotificationL", CVcxMyVideosMdsDbTest::ShutdownNotificationL ),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

TInt CVcxMyVideosMdsDbTest::CreateL( CStifItemParser& aItem )
    {
    int a ( 0 );
    
    if ( !aItem.GetNextInt( a ) )
        {
        mdssessionerror = a;
        }
    
    if ( !aItem.GetNextInt( a ) )
        {
        mdssessionasyncerror = a;
        }
    
    iMdsDb = CVcxMyVideosMdsDb::NewL( this, iFs );
    
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::DeleteL( CStifItemParser& )
    {
    delete iMdsDb;
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::CancelL( CStifItemParser& aItem )
    {
    TInt type = NextIntOrZero( aItem );
    iMdsDb->Cancel( ( CVcxMyVideosMdsDb::TRequestType ) type );
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::AddVideoL( CStifItemParser& aItem )
    {
    mdsoperationreturns = NextIntOrZero( aItem );
    mdsoperationleaves  = NextIntOrZero( aItem );
    TBool setMimeType   = NextIntOrZero( aItem ) ;
    propertynotdefined  = NextIntOrZero( aItem );
    propertyisnull      = NextIntOrZero( aItem );
    TBool setAttrs      = NextIntOrZero( aItem ) ;
    
    CMPXMedia* media  = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    TUint32 mdsId;
    
    media->SetTextValueL( KMPXMediaGeneralUri, _L("c:\\data\\videos\\addtest1.mp4") );
    
    if ( setAttrs )
        {
        media->SetTextValueL( KMPXMediaGeneralTitle, _L("KMPXMediaGeneralTitle") );
        media->SetTextValueL( KMPXMediaGeneralComment, _L("KMPXMediaGeneralComment") );
        media->SetTObjectValueL<TInt>( KMPXMediaGeneralSize, 100 );
        media->SetTObjectValueL<TInt64>( KMPXMediaGeneralDate, 1465324313154 );
        media->SetTObjectValueL<TUint32>( KMPXMediaGeneralFlags, 10 );
        media->SetTextValueL( KMPXMediaGeneralCopyright, _L("Copyright info") );    
        media->SetTObjectValueL<TInt>( KMPXMediaGeneralLastPlaybackPosition, 13232 );
        media->SetTObjectValueL<TInt64>( KVcxMediaMyVideosModifiedDate, 1465324313154 );
        media->SetTObjectValueL<TInt64>( KVcxMediaMyVideosAgeProfile, 18 );
        media->SetTextValueL( KVcxMediaMyVideosAudioLanguage, _L("Audio language") );
        media->SetTextValueL( KVcxMediaMyVideosAuthor, _L("Author") );
        media->SetTObjectValueL<TUint8>( KVcxMediaMyVideosOrigin, 1 );
        media->SetTObjectValueL<TReal32>( KVcxMediaMyVideosDuration, 1212 );
        media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, 0 );
        media->SetTObjectValueL<TUint8>( KVcxMediaMyVideosRating, 1 );
        media->SetTObjectValueL<TInt>( KVcxMediaMyVideosDownloadError, -1 );
        media->SetTObjectValueL<TInt>( KVcxMediaMyVideosDownloadGlobalError, -2 );
        media->SetTObjectValueL<TUint16>( KMPXMediaVideoBitRate, 1235 );
        media->SetTObjectValueL<TUint16>( KMPXMediaVideoWidth, 15 );
        media->SetTObjectValueL<TUint16>( KMPXMediaVideoHeight, 12 );
        media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosAudioFourCc, 0 );
        media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosAudioFourCc, 0 );
        media->SetTextValueL( KMPXMediaVideoArtist, _L("artist") );
        media->SetTObjectValueL<TInt64>( KMPXMediaGeneralExtSizeInt64, 123456 );
        }

    if ( setMimeType )
        {
        media->SetTextValueL( KMPXMediaGeneralMimeType, _L("Mime type") );    
        }
    
    iMdsDb->AddVideoL( *media, mdsId );
    
    CleanupStack::PopAndDestroy( media );
    
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::RemoveVideoL( CStifItemParser& aItem )
    {
    mdsoperationreturns = NextIntOrZero( aItem );
    mdsoperationleaves  = NextIntOrZero( aItem );
    TInt id             = NextIntOrZero( aItem );

    return iMdsDb->RemoveVideo( id );
    }

TInt CVcxMyVideosMdsDbTest::UpdateVideoL( CStifItemParser& aItem )
    {
    mdsoperationreturns = NextIntOrZero( aItem );
    mdsoperationleaves  = NextIntOrZero( aItem );
    mdeobjectopenresult = NextIntOrZero( aItem );
    
    CMPXMedia* media  = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    //1. MPX ID, set by collection
    //2
    media->SetTextValueL( KMPXMediaGeneralTitle, _L("Titteli") );
    //3
    media->SetTextValueL( KMPXMediaGeneralComment, _L("Desciä") );
    //4
    media->SetTextValueL( KMPXMediaGeneralUri, _L("c:\\data\\videos\\addtest1.mp4") );
    //5
    media->SetTObjectValueL<TInt>( KMPXMediaGeneralSize, 100 );
    //6 Creation date
    TTime time;
    time.HomeTime();
    media->SetTObjectValueL<TInt64>( KMPXMediaGeneralDate, time.Int64() );
    //7
    TUint32 flags = EVcxMyVideosVideoDrmProtected || EVcxMyVideosVideoNew;
    media->SetTObjectValueL<TUint32>( KMPXMediaGeneralFlags, flags );
    //8
    media->SetTextValueL( KMPXMediaGeneralCopyright, _L("Copyright info") );    
    //9
    media->SetTextValueL( KMPXMediaGeneralMimeType, _L("Mime type") );    
    //10
    TInt lastPlaybackPos( 13232 );
    media->SetTObjectValueL<TInt>( KMPXMediaGeneralLastPlaybackPosition, lastPlaybackPos );
    //11
    media->SetTObjectValueL<TInt64>( KVcxMediaMyVideosModifiedDate, time.Int64() );    
    //12
    media->SetTObjectValueL<TInt64>( KVcxMediaMyVideosAgeProfile, 18 );
    //13
    media->SetTextValueL( KVcxMediaMyVideosAudioLanguage, _L("Audio language") );
    //14
    media->SetTextValueL( KVcxMediaMyVideosAuthor, _L("Author") );
    //15
    media->SetTObjectValueL<TUint8>( KVcxMediaMyVideosOrigin, 1 );
    //16
    TReal32 duration( 1212 );
    media->SetTObjectValueL<TReal32>( KVcxMediaMyVideosDuration, duration );
    //17
    media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, 0 );
    //18
    media->SetTObjectValueL<TUint8>( KVcxMediaMyVideosRating, 1 );

    media->SetTObjectValueL<TInt>( KVcxMediaMyVideosDownloadError, -1 );
    
    media->SetTObjectValueL<TInt>( KVcxMediaMyVideosDownloadGlobalError, -2 );
    
    iMdsDb->UpdateVideoL( *media );
    
    CleanupStack::PopAndDestroy( media );
    
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::CreateVideoListL( CStifItemParser& )
    {    
    TVcxMyVideosSortingOrder sortingorder = ( TVcxMyVideosSortingOrder ) 0;

    CMPXMedia* videoList( NULL );
    CMPXMedia* localvideoList( NULL );
    CMPXMediaArray* mediaArray( NULL );
                        
   
    iMdsDb->CreateVideoListL( sortingorder, 0, 0, videoList );
    
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::DoCreateVideoListL( CStifItemParser& aItem )
    {
    TBool bitmask = NextIntOrZero( aItem );
    TBool storelist = NextIntOrZero( aItem );
    
    TBool ascending                       = bitmask & 0x01;
    TBool fullDetails                     = bitmask & 0x02;
    TBool docreate                        = bitmask & 0x04;
    TBool fetchongoing                    = bitmask & 0x08;
    
    TVcxMyVideosSortingOrder sortingorder = ( TVcxMyVideosSortingOrder ) ( bitmask >> 4 );

    CMPXMedia* videoList( NULL );
    CMPXMedia* localvideoList( NULL );
    CMPXMediaArray* mediaArray( NULL );
                        
    if ( docreate )
        {
        localvideoList = CMPXMedia::NewL();
        CleanupStack::PushL( localvideoList );
        mediaArray = CMPXMediaArray::NewL();
        CleanupStack::PushL( mediaArray );
        localvideoList->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, mediaArray );
        CleanupStack::PopAndDestroy( mediaArray );
        
        videoList = localvideoList;
        }
    
    if ( fetchongoing )
        {
        iMdsDb->DoCreateVideoListL( sortingorder, ascending, fullDetails, videoList );
        iMdsDb->iVideoListFetchingIsOngoing = fetchongoing;
        }
    
    iMdsDb->DoCreateVideoListL( sortingorder, ascending, fullDetails, videoList );
    
    if ( docreate )
        {
        CleanupStack::PopAndDestroy( localvideoList );
        }
    else if ( storelist )
        {
        iVideoList = videoList;
        }
    else
        {
        delete videoList;
        }
    
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::CreateVideoL( CStifItemParser& aItem )
    {
    mdsoperationreturns = NextIntOrZero( aItem );
    mdsoperationleaves  = NextIntOrZero( aItem );
    TBool fulldetails   = NextIntOrZero( aItem );
    TUint32 id          = NextIntOrZero( aItem );
    
    CMPXMedia* media ( NULL );
    media = iMdsDb->CreateVideoL( id, fulldetails );
    delete media;
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::HandleQueryNewResultsL( CStifItemParser& aItem )
    {
    querycountresult        = NextIntOrZero( aItem );
    propertyisnull          = NextIntOrZero( aItem );
    propertynotdefined      = NextIntOrZero( aItem );
    handledbeventleave      = NextIntOrZero( aItem );
    
    TInt firstnewitemindex  = NextIntOrZero( aItem );
    TInt newitemcount       = NextIntOrZero( aItem );

    
    iMdsDb->HandleQueryNewResults( *(CMdEQuery*) NULL, firstnewitemindex, newitemcount );

    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::HandleQueryCompletedL( CStifItemParser&  )
    {
    CMdEQuery* query ( NULL );
    iMdsDb->HandleQueryCompleted( *query, 0 );
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::AsyncHandleQueryCompletedL( CStifItemParser&  aItem )
    {
    TInt a = NextIntOrZero( aItem );
    TInt b = NextIntOrZero( aItem );
    TInt ret = CVcxMyVideosMdsDb::AsyncHandleQueryCompleted( iMdsDb );
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::HandleSessionErrorL( CStifItemParser& aItem )
    {
    iMdsDb->iVideoListFetchingIsOngoing = NextIntOrZero( aItem );
    TUint32 err = NextIntOrZero( aItem );
    
    CMdESession* cmdesession ( NULL );
    iMdsDb->HandleSessionError( *cmdesession, err );
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::HandleObjectNotificationL( CStifItemParser& aItem )
    {
    TInt notificationtype = NextIntOrZero( aItem );
    handledbeventleave    = NextIntOrZero( aItem );
    
    RArray<TItemId> ids;
    ids.Append( 0 );
    ids.Append( 1 );
    ids.Append( 2 );
    
    iMdsDb->HandleObjectNotification( *(CMdESession*)0, ( TObserverNotificationType )notificationtype, ids );
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::HandleObjectPresentNotification( CStifItemParser& aItem )
    {
    TInt a = NextIntOrZero( aItem );
    CMdESession* cmdesession ( NULL );
    RArray<TItemId> ids;
    ids.Append( 0 );
    ids.Append( 1 );
    ids.Append( 2 );
    handledbeventleave = NextIntOrZero(aItem);
    iMdsDb->HandleObjectPresentNotification( *cmdesession, a, ids );
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::ShutdownNotificationL( CStifItemParser& aItem )
    {
    TInt a = NextIntOrZero( aItem );
    iMdsDb->ShutdownNotification( a );
    return KErrNone;
    }

void CVcxMyVideosMdsDbTest::PrepareMdsL(  CStifItemParser& aItem  ) 
    {
    mdsoperationreturns = NextIntOrZero( aItem );
    mdsoperationleaves  = NextIntOrZero( aItem );
    }

void CVcxMyVideosMdsDbTest::HandleMyVideosDbEvent( TMPXChangeEventType , RArray<TUint32>&  )
    {
    if ( handledbeventleave )
        {
        User::Leave( handledbeventleave );
        }
    }

void CVcxMyVideosMdsDbTest::HandleCreateVideoListResp( CMPXMedia* aVideoList, TInt aNewItemsStartIndex, TBool aComplete )
    {
    if ( handledbeventleave )
        {
        User::Leave( handledbeventleave );
        }
    }
