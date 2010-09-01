/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Thumbnail generation implementation*
*/




// System includes
#include "IptvDebug.h"
#include <imageconversion.h>
#include <apgcli.h>
#include <bautils.h>

// User includes
#include "CIptvUtil.h"
#include "CIptvThumbnailImpl.h"
#include "CIptvThumbnailObserver.h"
#include "CIptvRtpTo3gpConverter.h"

// Module constants
_LIT(KTempClipFilename,"thumbtmp.3gp");

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
CIptvThumbnailImpl* CIptvThumbnailImpl::NewLC( MIptvThumbnailObserver& aObserver, TInt aPriority )
    {
    CIptvThumbnailImpl* self = new (ELeave) CIptvThumbnailImpl( aObserver, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CIptvThumbnailImpl
// ---------------------------------------------------------------------------
CIptvThumbnailImpl::CIptvThumbnailImpl( MIptvThumbnailObserver& aObserver, TInt aPriority )
    : CIptvThumbnailGenerator(aPriority), iObserver(aObserver), iPriority(aPriority)
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    CActiveScheduler::Add( this );
    //iRtpConverter = CIptvRtpTo3gpConverter::NewL( iFs, *this, iPriority );
    CIptvUtil::GetPathL( iFs, EIptvPathTmp, iTempClipFilename );
    iTempClipFilename.Append( KTempClipFilename );
    }

// ---------------------------------------------------------------------------
// DoCleanup
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::DoCleanup()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::DoCleanup IN" );
    if( iImageEncoder )
        {
        iImageEncoder->Cancel();
        delete iImageEncoder;
        iImageEncoder = NULL;
        }

    delete iThumbBitmap;
    iThumbBitmap = NULL;

    delete iThumbFilename;
    iThumbFilename = NULL;

    if( iTempClipFilename.Length() )
        {
        // delete temp file ignoring errors (NotFound,...)
        (void)iFs.Delete( iTempClipFilename );
        }

    if( iClipInfo )
        {
        // Save iClipInfo to local var and set to NULL to avoid recursive cancellation
        CTNEVideoClipInfo* info = iClipInfo;
        iClipInfo = NULL;
        info->CancelThumb();
        delete info;
        }

    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::DoCleanup OUT" );
    }

// ---------------------------------------------------------------------------
// ~CIptvThumbnailImpl
// ---------------------------------------------------------------------------
CIptvThumbnailImpl::~CIptvThumbnailImpl()
    {
    Cancel();
    DoCleanup();

    //delete iRtpConverter;
    //iRtpConverter = NULL;

    iFs.Close();
    }

// ---------------------------------------------------------------------------
// GenerateThumbnailL
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::GenerateThumbnailL( const TDesC& aClipFilename, const TDesC& aThumbFilename, const TSize aResolution )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::GenerateThumbnailL IN");
    IPTVLOGSTRING2_LOW_LEVEL( "clip-file='%S'", &aClipFilename );
    IPTVLOGSTRING2_LOW_LEVEL( "thumb-file='%S'", &aThumbFilename );

    // Check that no thumbnail generation is already ongoing.
    if ( IsActive() )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::GenerateThumbnailL object is active, leaving!");
        User::Leave( KErrInUse );
        }

    // Everything must be clean before starting generation
    DoCleanup();

    // 1. create VideoClipInfo object, actual info availability is
    // signaled by NotifyVideoClipInfoReady
    iThumbFilename = aThumbFilename.AllocL();
    iResolution = aResolution;

    DoStartThumbnailL( aClipFilename );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::GenerateThumbnailL OUT" );
    }

// ---------------------------------------------------------------------------
// CancelThumbnail
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::CancelThumbnail()
    {
    // Cancel on going thumb creation
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::CancelThumbnailL IN" );
    if ( IsActive() )
        {
        Cancel();
        // DoCleanup is done in DoCancel called by cancel.
        }
    else 
        {
        DoCleanup();
        }

    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::CancelThumbnailL OUT" );
    }

// ---------------------------------------------------------------------------
// NotifyVideoClipInfoReady
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::NotifyVideoClipInfoReady( CTNEVideoClipInfo& aInfo, TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipInfoReady IN" );

    // Check for errors on info creation, like file not found as so on
    if( aError != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipInfoReady: CVedVideoClipInfo::NewL FAILED, aError=%d", aError );
        iObserver.ThumbnailCompleted( aError );
        IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipInfoReady OUT" );
        return;
        }

    // 2. ClipInfo is ready, request actual thumbnail. Notified by NotifyVideoClipFrameCompleted.
    TRAPD( error, aInfo.GetThumbL(*this, KBestThumbIndex, &iResolution, ENone, EFalse, iPriority) );

    if( error != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipInfoReady: GetFrameL FAILED, leave=%d", error );
        iObserver.ThumbnailCompleted( error );
        IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipInfoReady OUT" );
        return;
        }

    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipInfoReady OK" );
    }

// ---------------------------------------------------------------------------
// NotifyVideoClipFrameCompleted
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::NotifyVideoClipThumbCompleted( CTNEVideoClipInfo&, TInt aError, CFbsBitmap* aThumbBitmap )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipFrameCompleted IN" );

    // Ownership transfer: manage aThumbBitmap from now on
    // Note: Object needs clean-up also in error case.
    iThumbBitmap = aThumbBitmap;

    // Check for errors on thumb creation, like format not supported, codec not found
    if( aError != KErrNone )
        {
        // Note: Must not cleanup here. VED library (or Thumbnail Engine library) uses its 
        // own member variables immediately after this event has finished. Would cause crash.
        // In this case we are trusting on clean-up in next thumbnail or class destructor.
        IPTVLOGSTRING2_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipFrameCompleted: GetFrameL FAILED, aError=%d", aError );
        iObserver.ThumbnailCompleted( aError );
        IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipFrameCompleted OUT" );
        return;
        }

    // 3. Request thumbnail encoding to file
    _LIT8( KJpgMimeType, "image/jpeg" );
    TRAPD( error, iImageEncoder = CImageEncoder::FileNewL(iFs, iThumbFilename->Des(), KJpgMimeType) );
    if( error != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipFrameCompleted: FileNewL FAILED, aError=%d", error );
        iObserver.ThumbnailCompleted( error );
        IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipFrameCompleted OUT" );
        return;
        }

    iImageEncoder->Convert( &iStatus, *iThumbBitmap );
    SetActive();
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::NotifyVideoClipFrameCompleted OUT" );
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::RunL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::RunL IN" );

    // Check for errors on image conversion and file save
    if( iStatus.Int() != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL( "CIptvThumbnailImpl::RunL CImageEncoder::Convert FAILED, error=%d", iStatus.Int() );
        DoCleanup();
        iObserver.ThumbnailCompleted( iStatus.Int() );
        return;
        }

    // 4. Everything went fine, signal user
    DoCleanup();
    iObserver.ThumbnailCompleted( KErrNone );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::RunL OUT" );
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::DoCancel()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::DoCancel IN" );
    DoCleanup();
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::DoCancel OUT" );
    }

// ---------------------------------------------------------------------------
// DoStartThumbnailL
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::DoStartThumbnailL( const TDesC& aClipFilename )
    {
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvThumbnailImpl::DoStartThumbnailL IN, clip-filename='%S'", &aClipFilename );
    iClipInfo = CTNEVideoClipInfo::NewL( aClipFilename, *this );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::DoStartThumbnailL OUT" );
    }

// ---------------------------------------------------------------------------
// RtpTo3gpConversionReady
// ---------------------------------------------------------------------------
void CIptvThumbnailImpl::RtpTo3gpConversionReady( CIptvRtpTo3gpConverter&, TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::RtpTo3gpConversionReady IN" );

    // Check for errors in conversion
    if( aError!=KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL( "CIptvThumbnailImpl::RtpTo3gpConversionReady: ConvertL FAILED, error=%d", aError );
        DoCleanup();
        iObserver.ThumbnailCompleted( aError );
        IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::RtpTo3gpConversionReady OUT [1]" );
        return;
        }

    // Start actual thumbnail generation
    TRAPD( error, DoStartThumbnailL(iTempClipFilename) );
    if( error!=KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL( "CIptvThumbnailImpl::RtpTo3gpConversionReady: DoStartThumbnailL FAILED, error=%d", error );
        DoCleanup();
        iObserver.ThumbnailCompleted( error );
        IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::RtpTo3gpConversionReady OUT [2]" );
        return;
        }

    IPTVLOGSTRING_LOW_LEVEL( "CIptvThumbnailImpl::RtpTo3gpConversionReady OUT [0]" );
    }
