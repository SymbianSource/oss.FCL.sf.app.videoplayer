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
* Description:    Offers callback interface to Epg database update*
*/




#include <e32base.h>
#include "IptvDebug.h"
#include "CIptvMediaContent.h"
#include "CIptvEpgVodMsqQueue.h"
#include "CIptvEpgVodCallbackImpl.h"
#include "CIptvEpgCategory.h"
#include "CIptvEpgContent.h"
#include "CIptvEpgDb.h"
#include "CIptvEpgSession.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CIptvEpgVodCallbackImpl::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgVodCallbackImpl::ConstructL" );
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::NewL
// Two-phased constructor.
// Create instance of concrete interface implementation
// --------------------------------------------------------------------------
//
CIptvEpgVodCallbackImpl* CIptvEpgVodCallbackImpl::NewL(
    CIptvEpgDb& aVodEpgDb,
    CIptvEpgVodMsqQueue* aMsgQueue,
    CIptvEpgSession& aSession )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgVodCallbackImpl::NewL" );

    CIptvEpgVodCallbackImpl* self =
        new( ELeave ) CIptvEpgVodCallbackImpl(
            aVodEpgDb, aMsgQueue, aSession );
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::~CIptvEpgVodCallbackImpl
// Destructor
// --------------------------------------------------------------------------
//
CIptvEpgVodCallbackImpl::~CIptvEpgVodCallbackImpl()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvEpgVodCallbackImpl::~CIptvEpgVodCallbackImpl" );
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::CIptvEpgVodCallbackImpl
// C++ default constructor
// --------------------------------------------------------------------------
//
CIptvEpgVodCallbackImpl::CIptvEpgVodCallbackImpl(
    CIptvEpgDb& aVodEpgDb,
    CIptvEpgVodMsqQueue* aMsgQueue,
    CIptvEpgSession& aSession ) :
    iVodEpgDb( aVodEpgDb ),
    iMsgQueue( aMsgQueue ),
    iPluginRunning( EFalse ),
    iSession( aSession )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvEpgVodCallbackImpl::CIptvEpgVodCallbackImpl" );
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::ContentUpdateCompletedL
// --------------------------------------------------------------------------
//
void CIptvEpgVodCallbackImpl::ContentUpdateCompletedL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvEpgVodCallbackImpl::ContentUpdateCompletedL" );

    iVodEpgDb.EndUpdateL( iSession );

    // Service id = 0 because we dont have to bring this information
    // over IPC. Service ID is already on the client side.
    if ( iMsgQueue )
        {
        iMsgQueue->SendMessageToClientL( KIptvContentUpdateCompleted, 0 );
        }
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::Started
// From MIptvEpgPluginManager interface
// --------------------------------------------------------------------------
//
void CIptvEpgVodCallbackImpl::StartedL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvEpgVodCallbackImpl::StartedL -- EPG Plugin STARTED!" );

    iPluginRunning = ETrue;
    iVodEpgDb.StartUpdateL();
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::FinishedL
// From MIptvEpgPluginManager interface
// --------------------------------------------------------------------------
//
TInt CIptvEpgVodCallbackImpl::FinishedL( TTime& aUpdateInterval )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvEpgVodCallbackImpl::FinishedL -- EPG Plugin FINISHED!" );

    iVodEpgDb.FinishUpdateL();

    TInt error = KErrNone;
    if ( iPluginRunning )
        {
        TInt64 interval = aUpdateInterval.Int64();
        if ( interval != 0 )
            {
            TTime time;
            time.UniversalTime();
            iVodEpgDb.SetUpdateTimeToServiceManagerL( time );
            error = iVodEpgDb.UpdateServiceInformationL( time, aUpdateInterval );
            if ( error != KErrNone )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "CIptvEpgVodCallbackImpl::FinishedL -- Failed to update service information" );
                }
            }
        }

    iPluginRunning = EFalse;
    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::AddCategory
// From MIptvEpgPluginManager interface
// --------------------------------------------------------------------------
//
TInt CIptvEpgVodCallbackImpl::AddCategoryL(
    CIptvEpgCategory& aCategory,
    TUint32& aCategoryKey )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgVodCallbackImpl::AddCategoryL" );

    TInt error = iVodEpgDb.AddCategoryL( aCategory, aCategoryKey );
    if ( error != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CIptvEpgVodCallbackImpl::AddCategoryL failed!" );
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::AddContentL
// From MIptvEpgPluginManager interface
// --------------------------------------------------------------------------
//
TInt CIptvEpgVodCallbackImpl::AddContentL(
    CIptvEpgContent& aContent,
    RPointerArray<CIptvMediaContent> aMediaContents,
    TUint32& aContentKey )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgVodCallbackImpl::AddContentL" );

    TInt error = iVodEpgDb.AddContentL(
        aContent, aMediaContents, aContentKey );
    if ( error != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CIptvEpgVodCallbackImpl::AddContentL failed!" );
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::UpdateServiceThumbnailL
// From MIptvEpgPluginManager interface
// --------------------------------------------------------------------------
//
TInt CIptvEpgVodCallbackImpl::UpdateServiceThumbnailL(
    TDesC& aIconFilePath,
    const TBool aGrouped )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvEpgVodCallbackImpl::UpdateServiceThumbnailL" );
    iSession.UpdateServiceThumbnailL( aIconFilePath, aGrouped );

    return KErrNone;
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::AddRelationL
// From MIptvEpgPluginManager interface
// --------------------------------------------------------------------------
//
TInt CIptvEpgVodCallbackImpl::AddRelationL(
    TUint32& aCategoryKey, TUint32& aContentKey )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgVodCallbackImpl::AddRelationL" );

    TInt error = iVodEpgDb.AddRelationL( aCategoryKey, aContentKey );
    if ( error != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CIptvEpgVodCallbackImpl::AddRelationL failed!" );
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::Error
// From MIptvEpgPluginManager interface
// --------------------------------------------------------------------------
//
void CIptvEpgVodCallbackImpl::ErrorL( TInt aError, TInt aInfo )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgVodCallbackImpl::ErrorL" );

    if ( iMsgQueue )
        {
        iMsgQueue->SendMessageToClientL( aError, aInfo );
        }
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::UpdateCategoryIconL
// From MIptvEpgPluginManager interface
// --------------------------------------------------------------------------
//
TInt CIptvEpgVodCallbackImpl::UpdateCategoryIconL(
    TUint32& aCategoryKey,
    TUint32& aIconFileId,
    TDesC& aIconFilePath, TBool aIsUrl )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgVodCallbackImpl::UpdateCategoryIconL" );

    TInt error = iVodEpgDb.UpdateCategoryIconL(
        aCategoryKey,
        aIconFileId,
        aIconFilePath,
        aIsUrl );
    if ( error != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CIptvEpgVodCallbackImpl::UpdateCategoryIconL failed!" );
        }

    return error;
    }

// --------------------------------------------------------------------------
// CIptvEpgVodCallbackImpl::UpdateContentIconL
// From MIptvEpgPluginManager interface
// --------------------------------------------------------------------------
//
TInt CIptvEpgVodCallbackImpl::UpdateContentIconL(
    TUint32& aContentKey,
    TUint32& aIconFileId,
    TDesC& aIconFilePath,
    TBool aIsUrl )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgVodCallbackImpl::UpdateContentIconL" );

    TInt error = iVodEpgDb.UpdateContentIconL(
        aContentKey, aIconFileId, aIconFilePath, aIsUrl );
    if ( error != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CIptvEpgVodCallbackImpl::UpdateContentIconL failed!" );
        }

    return error;
    }
