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
* Description:    .rtp to .3gp file converter*
*/




#include <f32file.h>
#include "IptvDebug.h"
#include "CIptvRtpTo3gpConverter.h"
#include "MIptvRtpTo3gpObserver.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CIptvRtpTo3gpConverter* CIptvRtpTo3gpConverter::NewL( RFs& aFs, MIptvRtpTo3gpObserver& aObserver, TInt aPriority )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::NewL IN" );
    CIptvRtpTo3gpConverter* self = new (ELeave) CIptvRtpTo3gpConverter( aFs, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aPriority );
    CleanupStack::Pop( self );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::NewL OUT" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIptvRtpTo3gpConverter
// ---------------------------------------------------------------------------
CIptvRtpTo3gpConverter::CIptvRtpTo3gpConverter( RFs& aFs, MIptvRtpTo3gpObserver& aObserver )
    : iFs(aFs)
    , iObserver(aObserver)
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CIptvRtpTo3gpConverter::ConstructL( TInt aPriority )
    {
    iIdle = CIdle::NewL( aPriority );
    }

// ---------------------------------------------------------------------------
// ~CIptvRtpTo3gpConverter
// ---------------------------------------------------------------------------
CIptvRtpTo3gpConverter::~CIptvRtpTo3gpConverter()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::~CIptvRtpTo3gpConverter IN" );
    DoCleanup( KErrCancel );

    delete iIdle;
    iIdle = NULL;

    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::~CIptvRtpTo3gpConverter OUT" );
    }

// ---------------------------------------------------------------------------
// ConvertL
// ---------------------------------------------------------------------------
void CIptvRtpTo3gpConverter::DoCleanup( TInt aStatus )
    {
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvRtpTo3gpConverter::DoCleanup IN, status=%d", aStatus );

    if( i3gpFilename )
        {
        if( aStatus != KErrNone )
            {
            (void)iFs.Delete( *i3gpFilename );
            }
        delete i3gpFilename;
        i3gpFilename = NULL;
        }
    iKeyframeWritten = EFalse;
    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::DoCleanup OUT" );
    }

// ---------------------------------------------------------------------------
// ConvertL
// ---------------------------------------------------------------------------
void CIptvRtpTo3gpConverter::ConvertL( const TDesC& aSrcRtpFilename, const TDesC& aDst3gpFilename )
    {
    // Not supported, just leave
    (void)aSrcRtpFilename;
    (void)aDst3gpFilename;
    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::ConvertL NOT SUPPORTED, check build configuration" );
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CancelConverstion
// ---------------------------------------------------------------------------
void CIptvRtpTo3gpConverter::CancelConverstion()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::CancelConverstion IN" );
    DoCleanup( KErrCancel );
    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::CancelConverstion OUT" );
    }

// ---------------------------------------------------------------------------
// DoIdleCleanupAndNotify
// ---------------------------------------------------------------------------
void CIptvRtpTo3gpConverter::DoIdleCleanupAndNotify( TInt aStatus )
    {
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvRtpTo3gpConverter::DoIdleCleanupAndNotify IN, status=%d", aStatus );

    // stop any previous idle cleanup
    iIdle->Cancel();

    // launch idle callback for cleanup and notification
    void* self = static_cast<void*>( this );
    TCallBack callback( CleanupAndNotifyCallback, self );
    iIdleStatus = aStatus;
    iIdle->Start( callback );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::DoIdleCleanupAndNotify OUT" );
    }

// ---------------------------------------------------------------------------
// CleanupAndNotifyCallback
// ---------------------------------------------------------------------------
TInt CIptvRtpTo3gpConverter::CleanupAndNotifyCallback( TAny* aSelf )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::CleanupAndNotifyCallback IN" );

    // Do actual cleanup and notification
    CIptvRtpTo3gpConverter* self = static_cast<CIptvRtpTo3gpConverter*>( aSelf );
    self->DoCleanup( self->iIdleStatus );
    self->iObserver.RtpTo3gpConversionReady( *self, self->iIdleStatus );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvRtpTo3gpConverter::CleanupAndNotifyCallback OUT" );
    return EFalse;
    }
