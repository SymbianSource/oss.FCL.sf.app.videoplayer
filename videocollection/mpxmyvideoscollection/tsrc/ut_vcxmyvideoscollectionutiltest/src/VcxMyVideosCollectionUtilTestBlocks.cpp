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

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

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
    RArray<TUint32> ids;
    
    TInt numberOfMedia;
    TInt wantedId;
    
    aItem.GetNextInt( numberOfMedia );
    aItem.GetNextInt( wantedId );
    
    ids.Append( wantedId );
    
    // add stuff to be copied to "from" -list
    
    
    TVcxMyVideosCollectionUtil::CopyFromListToListL( *from, *to, ids );
    
    // check that the "to" -list have the items it should have
    
    delete from;
    delete to;
    
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// This helper function converts drive letter to drive number
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::DriveLetter2DriveNumberL( CStifItemParser& /*aItem*/ )
    {
    TVcxMyVideosCollectionUtil::DriveLetter2DriveNumber( _L("E:") );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::MakeUniqueFileNameL
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::MakeUniqueFileNameL( CStifItemParser& /*aItem*/ )
    {
    RFs fs;
    fs.Connect();
    TBuf<KMaxPath> path;
    TVcxMyVideosCollectionUtil::MakeUniqueFileNameL( fs, _L("c://testframework//testframework.ini"), path );
    fs.Close();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::OriginL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosCollectionUtilTest::OriginL( CStifItemParser& /*aItem*/ )
    {
    CMPXMedia* media = CMPXMedia::NewL();
    TInt retVal( KErrNone );
    TUint8 valueSet = EVcxMyVideosOriginCapturedWithCamera;
    media->SetTObjectValueL<TUint8>( KVcxMediaMyVideosOrigin, valueSet );
    TUint8 valueGet = TVcxMyVideosCollectionUtil::OriginL( *media );
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
    media->SetTObjectValueL<TUint32>( KMPXMediaGeneralFlags, flagsSet );
    TUint32 flagsGet( TVcxMyVideosCollectionUtil::FlagsL( *media ) );
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
    media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, idSet );
    TUint32 idGet( TVcxMyVideosCollectionUtil::DownloadIdL( *media ) );
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
    media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadState, stateSet );
    TVcxMyVideosDownloadState stateGet = TVcxMyVideosCollectionUtil::DownloadStateL( *media );    
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
    media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, idSet );
    TMPXItemId idGet = TVcxMyVideosCollectionUtil::IdL( *media );;
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
    media->SetTObjectValueL<TReal32>( KVcxMediaMyVideosDuration, durationSet );
    TReal32 durationGet = TVcxMyVideosCollectionUtil::DurationL( *media );;
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
    media->SetTextValueL( KMPXMediaGeneralTitle, valueSet );
    HBufC* valueGet = TVcxMyVideosCollectionUtil::Title( *media ).AllocL();
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
    media->SetTObjectValueL<TUint8>( KVcxMediaMyVideosRating, valueSet );
    TUint8 valueGet = TVcxMyVideosCollectionUtil::RatingL( *media );;
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
    media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosAudioFourCc, valueSet );
    TUint32 valueGet = TVcxMyVideosCollectionUtil::AudioFourCcL( *media );;
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
    attrs.Append( KMPXMediaGeneralMimeType );
    TBool inMds( EFalse );
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
    // TMPXAttribute attr;
    // attr = KMPXMediaGeneralMimeType;
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

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtilTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CVcxMyVideosCollectionUtilTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
