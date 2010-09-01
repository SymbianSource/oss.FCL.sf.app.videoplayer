/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Interface for getting and setting the Video Center's last*
*/



#include <bautils.h>
#include "IptvDebug.h"
#include <s32file.h>
#include "CIptvUtil.h"
#include <sysutil.h>

#include "iptvlastwatcheddata.h"
#include "iptvlastwatchedapi.h"

_LIT( KIptvLastWatchedFileName, "lastwatched.dat" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CIptvLastWatchedApi::CIptvLastWatchedApi()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CIptvLastWatchedApi::ConstructL()
    {
    User::LeaveIfError( iFsSession.Connect() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CIptvLastWatchedApi* CIptvLastWatchedApi::NewL()
    {
    CIptvLastWatchedApi* self = CIptvLastWatchedApi::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CIptvLastWatchedApi* CIptvLastWatchedApi::NewLC()
    {
    CIptvLastWatchedApi* self = new( ELeave ) CIptvLastWatchedApi;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CIptvLastWatchedApi::~CIptvLastWatchedApi()
    {
    iFsSession.Close();
    }

// ---------------------------------------------------------------------------
// Write last watched data item to the file
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedApi::SetLastWatchedDataL(
    CIptvLastWatchedData& aData )
    {
    IPTVLOGSTRING_HIGH_LEVEL( ">>>CIptvLastWatchedApi::SetLastWatchedDataL" );

    if ( ! CIptvUtil::LastPlaybackPositionFeatureSupported() )
        {
        aData.SetLastVideoPlayPoint( 0 );
        }    
    
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFsSession,
            static_cast<TInt64>(aData.CountExternalizeSize()), EDriveC ) )
        {
    	IPTVLOGSTRING_HIGH_LEVEL(
    	    "<<<CIptvLastWatchedApi::SetLastWatchedDataL Disk full" );
        return KErrDiskFull;
        }
    else
        {
        TFileName fileName;
        CIptvUtil::GetPathL( iFsSession, EIptvPathEcg, fileName );
        fileName.Append( KIptvLastWatchedFileName );

        RFileWriteStream writeStream;

        TInt retVal = writeStream.Replace( iFsSession, fileName, EFileWrite );

        if ( retVal == KErrNone )
            {
            CleanupClosePushL( writeStream );
            aData.ExternalizeL( writeStream );
            CleanupStack::PopAndDestroy( &writeStream );
            }

    	IPTVLOGSTRING_HIGH_LEVEL(
    	    "<<<CIptvLastWatchedApi::SetLastWatchedDataL" );
        return retVal;
        }
    }

// ---------------------------------------------------------------------------
// Read last watched data item from the file
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedApi::GetLastWatchedDataL(
    CIptvLastWatchedData& aData )
    {
    IPTVLOGSTRING_HIGH_LEVEL( ">>>CIptvLastWatchedApi::GetLastWatchedDataL" );

    RFileReadStream readStream;
 
    TFileName fileName;
    CIptvUtil::GetPathL( iFsSession, EIptvPathEcg, fileName, EDriveC, EFalse );
    fileName.Append( KIptvLastWatchedFileName );

    TInt retVal = readStream.Open( iFsSession, fileName, EFileRead );

    if ( retVal == KErrNone )
        {
        CleanupClosePushL( readStream );
        aData.InternalizeL( readStream );
        CleanupStack::PopAndDestroy( &readStream );
        
        if ( ! CIptvUtil::LastPlaybackPositionFeatureSupported() )
            {
            aData.SetLastVideoPlayPoint( 0 );
            }
        }
    
	IPTVLOGSTRING_HIGH_LEVEL( "<<<CIptvLastWatchedApi::GetLastWatchedDataL" );

    return retVal;
    }

// ---------------------------------------------------------------------------
// Update last video play point
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLastWatchedApi::UpdateLastVideoPlayPointL(
    const TUint32 aLastVideoPlayPoint )
    {
    IPTVLOGSTRING2_HIGH_LEVEL(
        ">>>CIptvLastWatchedApi::UpdateLastVideoPlayPointL( %u )",
        aLastVideoPlayPoint );

    TInt retVal( KErrNotFound );
    
    if ( CIptvUtil::LastPlaybackPositionFeatureSupported() )
        {
        // Create descriptor for last watched data.
        CIptvLastWatchedData* lwData = CIptvLastWatchedData::NewL();
        if ( lwData )
            {
            CleanupStack::PushL( lwData );
    
            // Create name for last watched data file.
            TFileName fileName;
            CIptvUtil::GetPathL( iFsSession, EIptvPathEcg, fileName, EDriveC, EFalse );
            fileName.Append( KIptvLastWatchedFileName );

            // Read original vales.
            RFileReadStream readStream;
            retVal = readStream.Open( iFsSession, fileName, EFileRead );
            if ( retVal == KErrNone )
                {
                CleanupClosePushL( readStream );
                lwData->InternalizeL( readStream );
                CleanupStack::PopAndDestroy( &readStream );
                   
                // Update value.
                lwData->SetLastVideoPlayPoint( aLastVideoPlayPoint );

                // Replace data file with updated values.
                RFileWriteStream writeStream;
                retVal = writeStream.Replace( iFsSession, fileName, EFileWrite );
                if ( retVal == KErrNone )
                    {
                    CleanupClosePushL( writeStream );
                    lwData->ExternalizeL( writeStream );
                    CleanupStack::PopAndDestroy( &writeStream );
                    }
                }
            CleanupStack::PopAndDestroy( lwData );
            }
        }
    else
        {
        IPTVLOGSTRING_HIGH_LEVEL( "CIptvLastWatchedApi::UpdateLastVideoPlayPointL - Feature is not supported" );
        retVal = KErrNotSupported;
        }

	IPTVLOGSTRING_HIGH_LEVEL( "<<<CIptvLastWatchedApi::UpdateLastVideoPlayPointL" );

    return retVal;
    }
