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
* Description:    HG VOD class for handling thumbnail loading*
*/


#include "vcxnsimageconverter.h"
#include "vcxhgvodthumbnailmanager.h"
#include "vcxhgvodthumbnailobserver.h"

// Private data class.
NONSHARABLE_CLASS( CVcxHgVodThumbnailRequest ) : public CBase
    {
public:

    CVcxHgVodThumbnailRequest( MVcxHgVodThumbnailObserver& aObserver, 
            TSize aRequestedSize, TInt64 aClientId )
      : iObserver( aObserver ),
        iRequestedSize( aRequestedSize ),
        iClientId( aClientId )
        {
        }
    
    void ConstructL( const TDesC& aThumbnailPath )
        {
        iThumbnailPath = aThumbnailPath.AllocL();
        }
    
    ~CVcxHgVodThumbnailRequest()
        {
        delete iThumbnailPath;
        }
    
public: // data
    
    MVcxHgVodThumbnailObserver& iObserver;
    HBufC* iThumbnailPath;
    TSize iRequestedSize;
    TInt64 iClientId;
    
    };

TBool CompareRequests( const CVcxHgVodThumbnailRequest& aFirst, const CVcxHgVodThumbnailRequest& aSecond )
    {
    return aFirst.iClientId == aSecond.iClientId;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodThumbnailManager::~CVcxHgVodThumbnailManager()
// -----------------------------------------------------------------------------
//
CVcxHgVodThumbnailManager::~CVcxHgVodThumbnailManager()
    {
    delete iImageConverter;
    iRequestBuffer.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodThumbnailManager::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgVodThumbnailManager* CVcxHgVodThumbnailManager::NewL( RFs& aFsSession )
    {
    CVcxHgVodThumbnailManager* self = 
        CVcxHgVodThumbnailManager::NewLC( aFsSession );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodThumbnailManager::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodThumbnailManager* CVcxHgVodThumbnailManager::NewLC( RFs& aFsSession )
    {
    CVcxHgVodThumbnailManager* self = 
        new (ELeave) CVcxHgVodThumbnailManager();
    CleanupStack::PushL( self );
    self->ConstructL( aFsSession );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodThumbnailManager::CVcxHgVodThumbnailManager()
// -----------------------------------------------------------------------------
//
CVcxHgVodThumbnailManager::CVcxHgVodThumbnailManager()
    {
    
    }

// -----------------------------------------------------------------------------
// CVcxHgVodThumbnailManager::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodThumbnailManager::ConstructL( RFs& aFsSession )
    {
    iImageConverter = CVcxNsImageConverter::NewL( *this, aFsSession );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodThumbnailManager::Reset()
// -----------------------------------------------------------------------------
//
void CVcxHgVodThumbnailManager::Reset()
    {
    iRequestBuffer.ResetAndDestroy();
    iImageConverter->Cancel();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodThumbnailManager::AddThumbnailRequestL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodThumbnailManager::AddThumbnailRequestL( 
    MVcxHgVodThumbnailObserver& aObserver,
    const TDesC& aThumbnailPath, 
    TSize aRequestedSize, 
    TInt64 aClientId )
    {
    CVcxHgVodThumbnailRequest* newReq = new (ELeave) CVcxHgVodThumbnailRequest(
            aObserver, aRequestedSize, aClientId );
    CleanupStack::PushL( newReq );
    newReq->ConstructL( aThumbnailPath );
    CleanupStack::Pop( newReq );
    
    TInt err( KErrAlreadyExists );
    
    TIdentityRelation<CVcxHgVodThumbnailRequest> relation( CompareRequests );
    
    if ( iRequestBuffer.Find( newReq, relation ) == KErrNotFound )
        {
        err = iRequestBuffer.Append( newReq );
        }

    if ( err != KErrNone )
        {
        delete newReq;
        }
    else
        {
        if ( !(iImageConverter->IsActive()) )
            {
            err = iImageConverter->StartConvertL( *(newReq->iThumbnailPath), 
                    newReq->iRequestedSize, ETrue );
            
            if ( err != KErrNone )
                {
                // image cannot be converted, remove from request list
                iRequestBuffer.Remove( iRequestBuffer.Count() - 1 );
                delete newReq;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodThumbnailManager::ImageConversionCompletedL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodThumbnailManager::ImageConversionCompletedL( 
    TInt aError,
    CGulIcon* aIcon )
    {
    if ( iRequestBuffer.Count() > 0 )
        {
        CVcxHgVodThumbnailRequest* req = iRequestBuffer[0];
        iRequestBuffer.Remove( 0 );
        
        if ( aError == KErrNone )
            {    
            req->iObserver.MvtoThumbnailReady( aIcon, req->iClientId );
            }
        
        delete req;
        
        if ( iRequestBuffer.Count() > 0 )
            {
            req = iRequestBuffer[0];
            iImageConverter->StartConvertL( *(req->iThumbnailPath), req->iRequestedSize, ETrue );
            }
        }
    }
