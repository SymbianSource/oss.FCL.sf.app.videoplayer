/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include <vcxmyvideosdefs.h>
#include <mpxmedia.h>
#include <mpxattribute.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediavideodefs.h>
#include "VcxMyVideosCollectionUtilTest.h"
#include "vcxmyvideoscollectionutil.h"

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

const TMPXAttributeData KMPXMediaFail={0xC0FFEE,0xBADBEEF};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionUtilTest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.        
        ENTRY( "CreateEmptyMediaList", CVcxMyVideosCollectionUtilTest::CreateEmptyMediaListL ),
        ENTRY( "CreateEmptyMessageList", CVcxMyVideosCollectionUtilTest::CreateEmptyMessageListL ),
        ENTRY( "CopyFromListToList", CVcxMyVideosCollectionUtilTest::CopyFromListToListL ),
        ENTRY( "CopyFromListToList2", CVcxMyVideosCollectionUtilTest::CopyFromListToListL2 ),
        ENTRY( "DriveLetter2DriveNumber", CVcxMyVideosCollectionUtilTest::DriveLetter2DriveNumberL ),
        ENTRY( "MakeUniqueFileName", CVcxMyVideosCollectionUtilTest::MakeUniqueFileNameL ),
        ENTRY( "Origin", CVcxMyVideosCollectionUtilTest::OriginL ),
        ENTRY( "Flags", CVcxMyVideosCollectionUtilTest::FlagsL ),
        ENTRY( "DownloadId", CVcxMyVideosCollectionUtilTest::DownloadIdL ),
        ENTRY( "DownloadState", CVcxMyVideosCollectionUtilTest::DownloadStateL ),
        ENTRY( "Id", CVcxMyVideosCollectionUtilTest::IdL ),
        ENTRY( "Duration", CVcxMyVideosCollectionUtilTest::DurationL ),
        ENTRY( "Title", CVcxMyVideosCollectionUtilTest::TitleL ),
        ENTRY( "Rating", CVcxMyVideosCollectionUtilTest::RatingL ),
        ENTRY( "AudioFourCc", CVcxMyVideosCollectionUtilTest::AudioFourCcL ),
        ENTRY( "AreSupported", CVcxMyVideosCollectionUtilTest::AreSupportedL ), 
        ENTRY( "AttrBelongsToFullSet", CVcxMyVideosCollectionUtilTest::AttrBelongsToFullSetL ),
        ENTRY( "Origin2", CVcxMyVideosCollectionUtilTest::Origin ),
        ENTRY( "PrintOpenFileHandles", CVcxMyVideosCollectionUtilTest::PrintOpenFileHandlesL ),
        ENTRY( "GetProcessName", CVcxMyVideosCollectionUtilTest::GetProcessNameL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtil::CreateEmptyMediaListL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::CreateEmptyMediaListL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    delete media;
    
    // Only thing that can fail here is method call leaves. If we reach the end
    // everyting has worked fine and we can return ok.
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::CreateEmptyMessageListL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::CreateEmptyMessageListL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = TVcxMyVideosCollectionUtil::CreateEmptyMessageListL();
    delete media;
    
    // Only thing that can fail here is method call leaves. If we reach the end
    // everyting has worked fine and we can return ok.    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::CopyFromListToListL
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::CopyFromListToListL( CStifItemParser& aItem )
    {
    CMPXMedia* from = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CMPXMedia* to = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CMPXMedia* failList = CMPXMedia::NewL();
    CMPXMedia* media( NULL );
    TInt itemsToCopy( 0 );
    TInt itemsCopied( 0 );
    RArray<TUint32> ids;
    TMPXItemId idGet;
    TInt retVal( KErrGeneral );
    
    TInt numberOfMedia;
    TInt wantedId;
    
    aItem.GetNextInt( numberOfMedia );
    aItem.GetNextInt( wantedId );
    
    ids.Append( wantedId );
    
    // add stuff to be copied to "from" -list
    CleanupStack::PushL( from );
    CleanupStack::PushL( to );
    CMPXMediaArray* fromMessageArray = from->Value<CMPXMediaArray>(
            KMPXMediaArrayContents );
    CleanupStack::PushL( fromMessageArray );
    
    TVcxMyVideosCollectionUtil::CopyFromListToListL( *from, *to, ids );
    
    for(TInt i = 0; i < numberOfMedia; i++ )
        {
        media = CMPXMedia::NewL();
        CleanupStack::PushL( media );
        media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId,
                TMPXItemId( i, i) );
        media->SetTObjectValueL<TInt32>( KVcxMediaMyVideosInt32Value,
                42 );        
        fromMessageArray->AppendL( media );
        if( i == wantedId )
            {
            itemsToCopy++;
            }
        CleanupStack::Pop( media );
        }
    
    TRAPD( err, TVcxMyVideosCollectionUtil::CopyFromListToListL( *from, *failList, ids ) );    
    if( err != KErrArgument )    
        {
        CleanupStack::Pop( fromMessageArray );
        CleanupStack::Pop( to );
        CleanupStack::Pop( from );        
        fromMessageArray->Reset();
        delete failList;
        delete from;
        delete to; 
        return retVal;
        }
    
    TVcxMyVideosCollectionUtil::CopyFromListToListL( *from, *to, ids );
    
    // check that the "to" -list have the items it should have
    CMPXMediaArray* toMessageArray = to->Value<CMPXMediaArray>(
                KMPXMediaArrayContents );
    CleanupStack::PushL( toMessageArray );
    
    for( TInt i = 0; i < toMessageArray->Count(); i++ )
        {        
        idGet = TVcxMyVideosCollectionUtil::IdL( *(toMessageArray->operator[](i)) );
        if( idGet.iId1 == wantedId )
            {
            itemsCopied++;
            }
        }    
    
    if( itemsCopied == itemsToCopy )
        {
        retVal = KErrNone;  
        }
    
    CleanupStack::Pop( toMessageArray );
    CleanupStack::Pop( fromMessageArray );
    CleanupStack::Pop( to );
    CleanupStack::Pop( from );
    
    toMessageArray->Reset();
    fromMessageArray->Reset();
    
    delete failList;
    delete from;
    delete to;    
    return retVal;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::CopyFromListToListL2
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::CopyFromListToListL2( CStifItemParser& aItem )
    {
    RArray<CMPXMedia*> fromArray;
    CMPXMedia* to = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CMPXMedia* failList = CMPXMedia::NewL();
    CMPXMedia* media( NULL );        
    TInt itemsToCopy( 0 );
    TInt itemsCopied( 0 );
    RArray<TUint32> ids;
    TMPXItemId idGet;
    TInt retVal( KErrGeneral );
    
    TInt numberOfMedia;
    TInt wantedId;
    
    aItem.GetNextInt( numberOfMedia );
    aItem.GetNextInt( wantedId );
    
    ids.Append( wantedId );
    
    // add stuff to be copied to "from" -list    
    CleanupStack::PushL( to );    
    
    TVcxMyVideosCollectionUtil::CopyFromListToListL( fromArray, *to, ids );
    
    for(TInt i = 0; i < numberOfMedia; i++ )
        {
        media = CMPXMedia::NewL();
        CleanupStack::PushL( media );
        media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId,
                TMPXItemId( i, i) );
        media->SetTObjectValueL<TInt32>( KVcxMediaMyVideosInt32Value,
                42 );        
        fromArray.AppendL( media );
        if( i == wantedId )
            {
            itemsToCopy++;
            }
        CleanupStack::Pop( media );
        }    
    
    TRAPD(err, TVcxMyVideosCollectionUtil::CopyFromListToListL( fromArray, *failList, ids ));
    if( err != KErrArgument )    
        {        
        CleanupStack::Pop( to );
        fromArray.Reset();
        delete failList;
        delete to; 
        return retVal;
        }
    
    TVcxMyVideosCollectionUtil::CopyFromListToListL( fromArray, *to, ids );
    
    // check that the "to" -list have the items it should have
    CMPXMediaArray* toMessageArray = to->Value<CMPXMediaArray>(
                KMPXMediaArrayContents );
    CleanupStack::PushL( toMessageArray );
    
    for( TInt i = 0; i < toMessageArray->Count(); i++ )
        {        
        idGet = TVcxMyVideosCollectionUtil::IdL( *(toMessageArray->operator[](i)) );
        if( idGet.iId1 == wantedId )
            {
            itemsCopied++;
            }
        }    
    
    if( itemsCopied == itemsToCopy )
        {
        retVal = KErrNone;  
        }
    
    CleanupStack::Pop( toMessageArray );    
    CleanupStack::Pop( to );
        
    toMessageArray->Reset();
    
    for( TInt i = 0; i < fromArray.Count(); i++ )
        {
        CMPXMedia* arrayMedia = fromArray[i];
        delete arrayMedia;        
        }
    fromArray.Reset();    
    delete failList;
    delete to;    
    return retVal;
    }
    
// ---------------------------------------------------------------------------
// This helper function converts drive letter to drive number
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::DriveLetter2DriveNumberL( CStifItemParser& aItem )
    {
    TInt retVal( KErrNone ); 
    TPtrC string;
    TInt expectedValue( -1 );
    
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    User::LeaveIfError( aItem.GetNextString( string ) );
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    _LIT( KNullDescString, "KNullDesC" );
    if ( string.Compare( KNullDescString ) == 0 )
        {
        string.Set( KNullDesC );
        }
  
    TInt driveNumber = TVcxMyVideosCollectionUtil::DriveLetter2DriveNumber( string );
    if (  driveNumber != expectedValue )
        {
        retVal = KErrGeneral;
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::MakeUniqueFileNameL
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::MakeUniqueFileNameL( CStifItemParser& /*aItem*/ )
    {
    TInt retVal( KErrNone );
    RFs fs;
    fs.Connect();
    TBuf<KMaxPath> path;
    _LIT( KFileName1,   "c:\\data\\vcxmyvideoscollectionutiltest_file.txt" );
    _LIT( KFileName2,   "c:\\data\\vcxmyvideoscollectionutiltest_file2" );
    _LIT( KInvalidFile, "c:\\data\\vcxmyvideoscollectionutiltest_DoesNotExist" );
    
    // File with filename extension 
    TVcxMyVideosCollectionUtil::MakeUniqueFileNameL( fs, KFileName1, path );
    if ( path.Length() <= KFileName1().Length() )
        {
        retVal = KErrGeneral;
        }    
    
    // File without filename extension
    TVcxMyVideosCollectionUtil::MakeUniqueFileNameL( fs, KFileName2, path );
    if ( ( path.Find( KFileName2 ) == KErrNotFound ) ||
         ( path.Length() <= KFileName2().Length() ) ) 
        {
        retVal = KErrGeneral;
        }  
    
    // Non-existent file
    TVcxMyVideosCollectionUtil::MakeUniqueFileNameL( fs, KInvalidFile, path );
    if ( path.Compare( KInvalidFile ) != 0 )
        {
        retVal = KErrGeneral;
        }

    // Null descriptor
    TVcxMyVideosCollectionUtil::MakeUniqueFileNameL( fs, KNullDesC, path );
    
    fs.Close();
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::OriginL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::OriginL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();
    TInt retVal( KErrNone );
    TUint8 valueGet = TVcxMyVideosCollectionUtil::OriginL( *media );
    valueGet = TVcxMyVideosCollectionUtil::OriginL( *media );
    if( EVcxMyVideosOriginOther != valueGet )
        {
        retVal = KErrGeneral;
        delete media;
        return retVal;
        }
    TUint8 valueSet = EVcxMyVideosOriginCapturedWithCamera;
    media->SetTObjectValueL<TUint8>( KVcxMediaMyVideosOrigin, valueSet );
    valueGet = TVcxMyVideosCollectionUtil::OriginL( *media );
    if( valueSet != valueGet )
        {
        retVal = KErrGeneral;
        delete media;
        return retVal;
        }    
    valueSet = EVcxMyVideosOriginTvRecording;
    media->SetTObjectValueL<TUint8>( KVcxMediaMyVideosOrigin, valueSet );
    valueGet = TVcxMyVideosCollectionUtil::OriginL( *media );
    if( EVcxMyVideosOriginOther != valueGet )
        {
        retVal = KErrGeneral;
        }
    delete media;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::FlagsL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::FlagsL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();
    TInt retVal( KErrNone );    
    TUint32 flagsSet( 0xBEEF );
    TUint32 flagsGet( TVcxMyVideosCollectionUtil::FlagsL( *media ) );
    if( flagsGet != 0 )
        {
        delete media;
        retVal = KErrGeneral;
        return retVal;
        }
    media->SetTObjectValueL<TUint32>( KMPXMediaGeneralFlags, flagsSet );
    flagsGet= TVcxMyVideosCollectionUtil::FlagsL( *media );
    if( flagsSet != flagsGet )
        {
        retVal = KErrGeneral;
        }
    delete media;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::DownloadIdL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::DownloadIdL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();
    TInt retVal( KErrNone );
    TUint32 idSet( 0xBEEF );
    TUint32 idGet( TVcxMyVideosCollectionUtil::DownloadIdL( *media ) );
    if( 0 != idGet )
        {
        delete media;
        retVal = KErrGeneral;
        return retVal;
        }
    media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, idSet );
    idGet = TVcxMyVideosCollectionUtil::DownloadIdL( *media );
    if( idSet != idGet )
        {
        retVal = KErrGeneral;
        }
    delete media;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::DownloadStateL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::DownloadStateL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();    
    TInt retVal( KErrNone );
    TVcxMyVideosDownloadState stateSet = EVcxMyVideosDlStateFailed;
    TVcxMyVideosDownloadState stateGet = TVcxMyVideosCollectionUtil::DownloadStateL( *media );    
    if( EVcxMyVideosDlStateNone != stateGet )
        {
        delete media;
        retVal = KErrGeneral;
        return retVal;
        }
    media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadState, stateSet );
    stateGet = TVcxMyVideosCollectionUtil::DownloadStateL( *media );    
    if( stateSet != stateGet )
        {
        retVal = KErrGeneral;
        }
    delete media;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::IdL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::IdL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();
    TInt retVal( KErrNone );
    TMPXItemId idSet( 2, 7 );
    TMPXItemId idGet = TVcxMyVideosCollectionUtil::IdL( *media );
    if( (idGet.iId1 != 0) || (idGet.iId2 != 0) )
        {
        delete media;
        retVal = KErrGeneral;
        return retVal;
        }
    media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, idSet );
    idGet = TVcxMyVideosCollectionUtil::IdL( *media );
    if( idSet != idGet )
        {
        retVal = KErrGeneral;
        }
    delete media;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::DurationL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::DurationL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();
    TInt retVal( KErrNone );
    TReal32 durationSet( 150 );
    TReal32 durationGet = TVcxMyVideosCollectionUtil::DurationL( *media );
    if( -1 != durationGet )
        {
        delete media;
        retVal = KErrGeneral;
        return retVal;
        }
    media->SetTObjectValueL<TReal32>( KVcxMediaMyVideosDuration, durationSet );
    durationGet = TVcxMyVideosCollectionUtil::DurationL( *media );
    if( durationSet != durationGet )
        {
        retVal = KErrGeneral;
        }
    delete media;
    return retVal;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::Title
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::TitleL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();
    TInt retVal( KErrNone );
    _LIT( valueSet, "TeamNinja" );
    HBufC* valueGet = TVcxMyVideosCollectionUtil::Title( *media ).AllocL();
    if( valueGet->Size() != 0  )
        {
        delete media;
        delete valueGet;
        retVal = KErrGeneral;
        return retVal;
        }
    delete valueGet;
    media->SetTextValueL( KMPXMediaGeneralTitle, valueSet );
    valueGet = TVcxMyVideosCollectionUtil::Title( *media ).AllocL();
    if( valueGet->CompareF( valueSet ) )
        {
        retVal = KErrGeneral;
        }
    delete media;
    delete valueGet;
    return retVal;       
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::RatingL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::RatingL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();
    TInt retVal( KErrNone );
    TUint8 valueSet( 21 );
    TUint8 valueGet = TVcxMyVideosCollectionUtil::RatingL( *media );;
    if( 0 != valueGet )
        {
        delete media;
        retVal = KErrGeneral;
        return retVal;
        }
    media->SetTObjectValueL<TUint8>( KVcxMediaMyVideosRating, valueSet );
    valueGet = TVcxMyVideosCollectionUtil::RatingL( *media );;
    if( valueSet != valueGet )
        {
        retVal = KErrGeneral;
        }
    delete media;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::AudioFourCcL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::AudioFourCcL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();
    TInt retVal( KErrNone );
    TUint32 valueSet( 21 );
    TUint32 valueGet = TVcxMyVideosCollectionUtil::AudioFourCcL( *media );;
    if( 0 != valueGet )
        {
        delete media;
        retVal = KErrGeneral;
        return retVal;
        }
    media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosAudioFourCc, valueSet );
    valueGet = TVcxMyVideosCollectionUtil::AudioFourCcL( *media );;
    if( valueSet != valueGet )
        {
        retVal = KErrGeneral;
        }
    delete media;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::AreSupported
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::AreSupportedL( CStifItemParser& /*aItem*/ )
    {
    TInt retVal( KErrNone );
    CMPXMedia* media = CMPXMedia::NewL();
    RArray<TMPXAttribute> attrs;
    TBool inMds( EFalse );
    TVcxMyVideosCollectionUtil::AreSupported( *media, attrs.Array(), inMds );        
    attrs.Append( KMPXMediaFail );
    TVcxMyVideosCollectionUtil::AreSupported( *media, attrs.Array(), inMds );
    attrs.Append( KMPXMediaVideoBitRate );
    inMds = ETrue;    
    TVcxMyVideosCollectionUtil::AreSupported( *media, attrs.Array(), inMds );
    media->SetTObjectValueL<TUint16>( KMPXMediaVideoBitRate, 47 );
    TVcxMyVideosCollectionUtil::AreSupported( *media, attrs.Array(), inMds );
    delete media;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::AttrBelongsToFullSet
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::AttrBelongsToFullSetL( CStifItemParser& /*aItem*/ )
    {
    TInt retVal( KErrNone );

    if ( !( TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KMPXMediaGeneralComment ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KMPXMediaGeneralCopyright ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KMPXMediaGeneralMimeType ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KMPXMediaGeneralLastPlaybackPosition ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KVcxMediaMyVideosModifiedDate ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KVcxMediaMyVideosAudioLanguage ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KVcxMediaMyVideosAuthor ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KMPXMediaVideoBitRate ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KMPXMediaVideoHeight ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KMPXMediaVideoWidth ) &&
         TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KMPXMediaVideoArtist ) ) )
        {
        retVal = KErrGeneral;
        return retVal;
        }
    
    if( TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( KMPXMediaFail ) )
        {
        retVal = KErrGeneral;
        }
        
    return retVal;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::Origin
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::Origin( CStifItemParser& /*aItem*/ )
    {
    TInt retVal( KErrNone );
    
    if ( ( TVcxMyVideosCollectionUtil::Origin( KVcxMvcCategoryIdDownloads ) 
            != EVcxMyVideosOriginDownloaded ) ||
         ( TVcxMyVideosCollectionUtil::Origin( KVcxMvcCategoryIdTvRecordings ) 
            != EVcxMyVideosOriginTvRecording ) ||
         ( TVcxMyVideosCollectionUtil::Origin( KVcxMvcCategoryIdCaptured ) 
            != EVcxMyVideosOriginCapturedWithCamera ) ||
         ( TVcxMyVideosCollectionUtil::Origin( KVcxMvcCategoryIdOther ) 
            != EVcxMyVideosOriginOther ) ||
         ( TVcxMyVideosCollectionUtil::Origin( KVcxMvcCategoryIdAll ) 
            != KErrNotFound ) )
        {
        retVal = KErrGeneral;
        return retVal;
        }
    
    if( TVcxMyVideosCollectionUtil::Origin( KCategoryIdExtraItem3 ) != KErrNotFound )
        {
        retVal = KErrGeneral;
        }
    
    return retVal;
    }   

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::PrintOpenFileHandlesL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::PrintOpenFileHandlesL( CStifItemParser& /*aItem*/ )
    {
    TInt retVal( KErrNone );
    
#ifdef _DEBUG
    RFs fs;
    RFile file;
    _LIT( KFileName,        "c:\\data\\vcxmyvideoscollectionutiltest_file.txt" );
    _LIT( KInvalidFileName, "c:\\data\\vcxmyvideoscollectionutiltest_fail.txt" );    
    User::LeaveIfError( fs.Connect() );
    User::LeaveIfError( file.Open( fs, KFileName, EFileShareAny | EFileRead ) );
    
    TVcxMyVideosCollectionUtil::PrintOpenFileHandlesL( KFileName, fs );
    TVcxMyVideosCollectionUtil::PrintOpenFileHandlesL( KInvalidFileName, fs );
    
    file.Close();
    fs.Close();    
#endif // _DEBUG
    
    return retVal;
    }   

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::GetProcessName
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::GetProcessNameL( CStifItemParser& aItem )
    {
    TInt retVal( KErrNone );
    TPtrC testProcessName;
    TUint64 threadId = RThread().Id().Id();
    TFullName processName;
    //_LIT( KTestProcessName, "testscripter_vcxmyvideoscollectionutiltest" );

    retVal = aItem.GetNextString( testProcessName );
    
#ifdef _DEBUG    
    if ( retVal == KErrNone )
        {
        TVcxMyVideosCollectionUtil::GetProcessName( threadId, processName );
        if ( processName.Find( testProcessName ) == KErrNotFound )
            {
            retVal = KErrNotFound;
            }
        }
#endif // _DEBUG    
    return retVal;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
