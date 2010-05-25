/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <vcxmyvideosdefs.h>
#include <mpxmessagecontainerdefs.h>
#include <bautils.h>
#include <mpxmediavideodefs.h>
#ifdef _DEBUG
#include <flogger.h>
#include <f32file.h> 
#endif
#include "vcxmyvideoscollectionutil.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtil::CreateEmptyMediaListL
// ----------------------------------------------------------------------------
//
CMPXMedia* TVcxMyVideosCollectionUtil::CreateEmptyMediaListL()
    {
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds ); // 1->
                
    supportedIds.AppendL( KMPXMediaIdContainer );
    supportedIds.AppendL( KMPXMediaIdGeneral );

    //create container
    CMPXMedia* container = CMPXMedia::NewL( supportedIds.Array() );
    CleanupStack::PushL( container ); // 2->

    CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL( mediaArray ); // 3->

    container->SetCObjectValueL( KMPXMediaArrayContents, mediaArray );

    CleanupStack::PopAndDestroy( mediaArray );    // <-3
    CleanupStack::Pop( container );               // <-2    
    CleanupStack::PopAndDestroy( &supportedIds ); // <-1
    return container;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionUtil::CreateEmptyMessageListL
// ----------------------------------------------------------------------------
//
CMPXMedia* TVcxMyVideosCollectionUtil::CreateEmptyMessageListL()
    {
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::CopyFromListToListL
// ---------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::CopyFromListToListL(
        CMPXMedia& aFromList,
        CMPXMedia& aToList,
        RArray<TUint32>& aMdsIds )
    {
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::CopyFromListToListL
// ---------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::CopyFromListToListL(
        RArray<CMPXMedia*>& aFromArray,
        CMPXMedia& aToList,
        RArray<TUint32>& aMdsIds )
    {
    }
    
// ---------------------------------------------------------------------------
// This helper function converts drive letter to drive number
// ---------------------------------------------------------------------------
//
TInt TVcxMyVideosCollectionUtil::DriveLetter2DriveNumber( const TDesC &aDrive )
    {
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::MakeUniqueFileNameL
// ---------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::MakeUniqueFileNameL( RFs& aFs, const TDesC& aPath,
        TDes& aUniquePath )
    {
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::OriginL
// ----------------------------------------------------------------------------
//
TUint8 TVcxMyVideosCollectionUtil::OriginL( CMPXMedia& aVideo )
    {
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::FlagsL
// ----------------------------------------------------------------------------
//
TUint32 TVcxMyVideosCollectionUtil::FlagsL( CMPXMedia& aVideo )
    {
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::DownloadIdL
// ----------------------------------------------------------------------------
//
TUint32 TVcxMyVideosCollectionUtil::DownloadIdL( CMPXMedia& aVideo )
    {
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::DownloadStateL
// ----------------------------------------------------------------------------
//
TVcxMyVideosDownloadState TVcxMyVideosCollectionUtil::DownloadStateL( CMPXMedia& aVideo )
    {
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::IdL
// ----------------------------------------------------------------------------
//
TMPXItemId TVcxMyVideosCollectionUtil::IdL( CMPXMedia& aVideo )
    {
    }



// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::DurationL
// ----------------------------------------------------------------------------
//
TReal32 TVcxMyVideosCollectionUtil::DurationL( CMPXMedia& aVideo )
    {
    }


// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::Title
// ----------------------------------------------------------------------------
//
const TDesC& TVcxMyVideosCollectionUtil::Title( CMPXMedia& aVideo )
    {                            
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::RatingL
// ----------------------------------------------------------------------------
//
TUint8 TVcxMyVideosCollectionUtil::RatingL( CMPXMedia& aVideo )
    {
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::AudioFourCcL
// ----------------------------------------------------------------------------
//
TUint32 TVcxMyVideosCollectionUtil::AudioFourCcL( CMPXMedia& aVideo )
    {
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::AreSupported
// ----------------------------------------------------------------------------
//
TBool TVcxMyVideosCollectionUtil::AreSupported( CMPXMedia& aVideo,
        const TArray<TMPXAttribute>& aAttrs,
        TBool& aNonSupportedAttrCanBeFoundFromMds )
    {
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::AttrBelongsToFullSet
// ----------------------------------------------------------------------------
//
TBool TVcxMyVideosCollectionUtil::AttrBelongsToFullSet( const TMPXAttribute& aAttr )
    {
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::Origin
// ----------------------------------------------------------------------------
//
TInt TVcxMyVideosCollectionUtil::Origin( TInt aCategoryId )
    {
    }
    
#ifdef _DEBUG
// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::PrintOpenFileHandlesL
// ----------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::PrintOpenFileHandlesL( const TDesC& aFileName, RFs& aFs )
    {
    }   

// ----------------------------------------------------------------------------
// TVcxMyVideosCollectionUtil::GetProcessName
// ----------------------------------------------------------------------------
//
void TVcxMyVideosCollectionUtil::GetProcessName( TInt aThreadId, TFullName& aProcessName )
    {
    }   
#endif
   
// End of file
