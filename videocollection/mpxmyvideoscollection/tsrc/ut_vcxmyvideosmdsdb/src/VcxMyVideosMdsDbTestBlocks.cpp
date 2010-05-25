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

// Version : %version: 1 %

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

#include "vcxmyvideosmdsdb.h"
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
        ENTRY( "CreateVideoL", CVcxMyVideosMdsDbTest::CreateVideoL )
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

TInt CVcxMyVideosMdsDbTest::DeleteL( CStifItemParser& aItem )
    {
    delete iMdsDb;
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::CancelL( CStifItemParser& aItem )
    {
    iMdsDb->Cancel();
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::AddVideoL( CStifItemParser& aItem )
    {
    PrepareMdsL( aItem  );
    
    CMPXMedia* media  = CMPXMedia::NewL();
    
    TUint32 mdsId;
    
    //1. MPX ID, set by collection
    //2
    media->SetTextValueL( KMPXMediaGeneralTitle, _L("KMPXMediaGeneralTitle") );
    //3
    media->SetTextValueL( KMPXMediaGeneralComment, _L("KMPXMediaGeneralComment") );
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
    
    iMdsDb->AddVideoL( *media, mdsId );
    
    delete media;
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::RemoveVideoL( CStifItemParser& aItem )
    {
    PrepareMdsL( aItem  );
    
    TInt id;
    aItem.GetNextInt( id );
    return iMdsDb->RemoveVideo( id );
    }

TInt CVcxMyVideosMdsDbTest::UpdateVideoL( CStifItemParser& aItem )
    {
    PrepareMdsL( aItem  );
    
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

TInt CVcxMyVideosMdsDbTest::CreateVideoListL( CStifItemParser& aItem )
    {
    TVcxMyVideosSortingOrder sortingorder = ( TVcxMyVideosSortingOrder ) NextIntOrZero( aItem );
    TBool ascending                       = NextIntOrZero( aItem );
    TBool fullDetails                     = NextIntOrZero( aItem );
    TBool docreate                        = NextIntOrZero( aItem );
    
    CMPXMedia* videoList( NULL );
    CMPXMediaArray* mediaArray( NULL );
                        
    if ( docreate )
        {
        videoList = CMPXMedia::NewL();
        CleanupStack::PushL( videoList );
        mediaArray = CMPXMediaArray::NewL();
        videoList->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, mediaArray );
        CleanupStack::PushL( mediaArray );
        }
    
    iMdsDb->CreateVideoListL( sortingorder, ascending, fullDetails, videoList );
    
    if ( docreate )
        {
        CleanupStack::PopAndDestroy( videoList );
        }
    else
        {
        delete videoList;
        }
    
    return KErrNone;
    }

TInt CVcxMyVideosMdsDbTest::CreateVideoL( CStifItemParser& aItem )
    {
    PrepareMdsL( aItem );
    
    TUint32 aId;
    TBool aFullDetails = ETrue;
    CMPXMedia* media = iMdsDb->CreateVideoL( aId, aFullDetails );
    delete media;
    return KErrNone;
    }


void CVcxMyVideosMdsDbTest::PrepareMdsL(  CStifItemParser& aItem  ) 
    {
    mdsoperationreturns = NextIntOrZero( aItem );
    
    mdsoperationleaves = NextIntOrZero( aItem );
    }

