/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Offers interface for loading and converting images.*
*/




// INCLUDE FILES
#include <gulicon.h>					// CGulIcon
#include <imageconversion.h>			// CImageDecoder
#include <AknIconArray.h>				// CAknIconArray
#include "IptvLiveLogger.h"				// Live TV logging macros
#include <AknIconUtils.h>				// AknIconUtils
#include <AknsUtils.h>

#include "CIptvLiveUIImageHandler.h"	// CIptvLiveUIImageHandler
#include "MIptvLiveUIImageObserver.h"	// MIptvLiveUIImageObserver


// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvLiveUIImageHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CIptvLiveUIImageHandler* CIptvLiveUIImageHandler::NewL( 
										MIptvLiveUIImageObserver& aObserver )
    {
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::NewL() in"));
    CIptvLiveUIImageHandler* self = 
						new ( ELeave ) CIptvLiveUIImageHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIImageHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvLiveUIImageHandler::ConstructL()
    {
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::ConstructL() in"));
	User::LeaveIfError( iFs.Connect() );
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::ConstructL() out"));
    }
    
// -----------------------------------------------------------------------------
// CIptvLiveUIImageHandler::CIptvLiveUIImageHandler()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CIptvLiveUIImageHandler::CIptvLiveUIImageHandler( 
										MIptvLiveUIImageObserver& aObserver ) 
	: CActive( EPriorityNormal ), iObserver( &aObserver ), 
	iImageDecoder( NULL ), iBitmap( NULL )
    {
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::CIptvLiveUIImageHandler() in"));
    CActiveScheduler::Add( this );
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::CIptvLiveUIImageHandler() out"));
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIImageHandler::~CIptvLiveUIImageHandler()
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvLiveUIImageHandler::~CIptvLiveUIImageHandler()
    {
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::~CIptvLiveUIImageHandler() in"));
    Cancel();
    iObserver = NULL;
	delete iImageDecoder;
	delete iBitmap;
	iFs.Close();
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::~CIptvLiveUIImageHandler() out"));    	
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIImageHandler::LoadImageL(). Asynchronous, 
// MIptvLiveUIImageObserver::ImageReadyL() when loading and converting is complete.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvLiveUIImageHandler::LoadImageL( const TDesC& aFilePath )
    {
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::LoadImageL() in"));
	if ( IsActive() ) 
		{
		LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::LoadImageL() IS ACTIVE, leave with KErrInUse"));
		User::Leave( KErrInUse );
		}
	if ( iImageDecoder ) 
		{
		iImageDecoder->Cancel();
		delete iImageDecoder;
		iImageDecoder = NULL;
		}
	// Create new image decoder 
	iImageDecoder = CImageDecoder::FileNewL( iFs, aFilePath );

	// Create bitmap
	if ( iBitmap ) 
		{
		delete iBitmap;
		iBitmap = NULL;
		}
	TFrameInfo info = iImageDecoder->FrameInfo();
	iBitmap = new ( ELeave ) CFbsBitmap;
	TInt err = iBitmap->Create( info.iOverallSizeInPixels, 
										 info.iFrameDisplayMode );
	if ( err == KErrNone )
		{
		// Start converting the image
		iImageDecoder->Convert( &iStatus, *iBitmap );
		SetActive();	
		}
	else
		{
		delete iImageDecoder;
		iImageDecoder = NULL;
		delete iBitmap;
		iBitmap = NULL;
		User::Leave( err );
		}
	
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::LoadImageL() out"));
   	}
    
// -----------------------------------------------------------------------------
// CIptvImageLoader::LoadIconL
// Loads icon to the given array.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvLiveUIImageHandler::LoadIconL( CAknIconArray* aIcons, 
									const TInt aIconIndex, 
                                    const TInt aMaskIndex, const TSize& aIconSize,
                                    const TDesC& aMifFileName, TBool aInsert )
    {
	LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::LoadIconL() in") );
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
        
    AknIconUtils::CreateIconLC( bitmap, mask, aMifFileName, 
                                aIconIndex, aMaskIndex );

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::PushL( icon );

    // ownership of bitmap and mask transferred to icon
    icon->SetBitmapsOwnedExternally( EFalse );

    TInt err = AknIconUtils::SetSize( bitmap, aIconSize );
	if ( err != KErrNone ) 
		{
		LIVE_TV_TRACE2(_L("CIptvLiveUIImageHandler::LoadIconL() AknIconUtils::SetSize() FAILED: %d"), err);
		}

    if ( aInsert && aIcons )
        {
        aIcons->InsertL( 0, icon );
        }
    else 
        {
		if ( aIcons ) 
			{
			aIcons->AppendL( icon );	
			}
        }

    CleanupStack::Pop( icon );  
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );
	LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::LoadIconL() out") );
    }   

// -----------------------------------------------------------------------------
// Loads color-customized icon to the given array.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvLiveUIImageHandler::LoadIconL( CAknIconArray* aIcons, 
                                    const TAknsItemID aItemId, 
									const TInt aIconIndex, 
                                    const TInt aMaskIndex, 
                                    const TSize& aIconSize,
                                    const TDesC& aMifFileName, 
                                    TBool aInsert
                                    )
    {
	LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::LoadIconL() in") );
        
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();    
            
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TRgb defaultColour( KRgbWhite );
    // Color is not updated if it not found from the skin
    AknsUtils::GetCachedColor( skin,
              defaultColour,
              KAknsIIDQsnIconColors,
              EAknsCIQsnIconColorsCG13 );
    
    AknsUtils::CreateColorIconLC( skin,
              aItemId,
              KAknsIIDQsnIconColors,
              EAknsCIQsnIconColorsCG13,
              bitmap,
              mask,
              aMifFileName,
              aIconIndex,
              aMaskIndex,
              defaultColour,
              aIconSize,
              EAspectRatioPreserved );
        
    CGulIcon* icon = NULL;
    icon = CGulIcon::NewL( bitmap, mask );
    
    CleanupStack::Pop( 2 ); // mask, bitmap        
    // ownership of bitmap and mask transferred to icon
    icon->SetBitmapsOwnedExternally( EFalse );         
    
    CleanupStack::PushL( icon );    

    if ( aInsert && aIcons )
        {
        aIcons->InsertL( 0, icon );
        }
    else 
        {
		if ( aIcons ) 
			{
			aIcons->AppendL( icon );	
			}
        }

    CleanupStack::Pop( icon );  
	LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::LoadIconL() out") );
    }   

// -----------------------------------------------------------------------------
// CIptvLiveUIImageHandler::RunL()
// From CActive
// -----------------------------------------------------------------------------
//
void CIptvLiveUIImageHandler::RunL()
    {
    LIVE_TV_TRACE2(_L("CIptvLiveUIImageHandler::RunL() in, iStatus.Int() = %d"), iStatus.Int());
	if ( iStatus.Int() == KErrNone ) 
		{
		// Image conversion ok
		iObserver->ImageReadyL( KErrNone, *iBitmap );
		}
	else 
		{
		// There was some error, notify observer
		iObserver->ImageReadyL( iStatus.Int(), *iBitmap );
		}
	// free image file and bitmap
	delete iImageDecoder;
	iImageDecoder = NULL;
	delete iBitmap;
	iBitmap = NULL;
	LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::RunL() out"));
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIImageHandler::RunError()
// From CActive
// -----------------------------------------------------------------------------
//
TInt CIptvLiveUIImageHandler::RunError( TInt /*aError*/ )
    {
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::RunError() in"));
	// free image file and bitmap
	delete iImageDecoder;
	iImageDecoder = NULL;
	delete iBitmap;
	iBitmap = NULL;
	LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::RunError() out"));
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CIptvLiveUIImageHandler::DoCancel()
// From CActive
// -----------------------------------------------------------------------------
//
void CIptvLiveUIImageHandler::DoCancel()
    {
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::DoCancel() in"));
	if ( iImageDecoder ) 
		{
		iImageDecoder->Cancel();	
		}
	delete iImageDecoder;
	iImageDecoder = NULL;
    LIVE_TV_TRACE1(_L("CIptvLiveUIImageHandler::DoCancel() out"));
    }

// End of File
