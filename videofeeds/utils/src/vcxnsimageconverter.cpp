/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Converter that creates icons from picture files.*
*/




// INCLUDE FILES
#include <AknUtils.h>        // TAknWindowLineLayout
#include <gulicon.h>         // CGulIcon
#include <bautils.h>         // BaflUtils
#include <imageconversion.h> // Image converter
#include <AknIconUtils.h>

#include "vcxnsimageconverter.h"
#include "vcxnsimageconverterobserver.h"
#include "IptvDebug.h"

_LIT(KMifExtension, ".mif");


// ============================ LOCAL FUNCTIONS ================================

void ScaleBitmapL( CFbsBitmap*& aImage, TSize aSize )
    {
    if ( aImage )
        {
        CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
        CleanupStack::PushL( bitmap );
        bitmap->Create( aSize, aImage->DisplayMode() );
        AknIconUtils::ScaleBitmapL( aSize, bitmap, aImage );
        delete aImage;
        aImage = bitmap;
        CleanupStack::Pop( bitmap );
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::CVcxNsImageConverter()
// -----------------------------------------------------------------------------
//
CVcxNsImageConverter::CVcxNsImageConverter(
    MVcxNsImageConverterObserver& aObserver,
    RFs& aFsSession )
  : CActive( EPriorityStandard ),
    iFsSession( aFsSession ),
    iSynchronous( EFalse ),
    iConvertingMif( EFalse ),
    iMifBitmapId( 0 ),
    iMifMaskId( 0 ),
    iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxNsImageConverter::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsImageConverter* CVcxNsImageConverter::NewL(
    MVcxNsImageConverterObserver& aObserver,
    RFs& aFsSession )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::NewL()");

    CVcxNsImageConverter* self = new (ELeave) CVcxNsImageConverter(
            aObserver, aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::~CVcxNsImageConverter()
// -----------------------------------------------------------------------------
//
CVcxNsImageConverter::~CVcxNsImageConverter()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::~CVcxNsImageConverter()");

    Cancel();

    // If decoder is cancelled, these might stay reserved.
    delete iDecoder;
    delete iBitmap;
    delete iMask;
    delete iMifFileName;
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::GetImageType()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVcxNsImageConverter::GetImageType(
    const TDesC& aFileName,
    TImageType& aImageType )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsImageConverter::GetImageType() %S", &aFileName);

    TBuf<KFileExtensionLength> ext;

    if ( GetExtension( aFileName, ext ))
        {
        ext.LowerCase();

        if ( ext.Compare( KMifExtension )) // Returns zero if match.
            {
            aImageType = EOther;
            }
        else
            {
            aImageType = EMif;
            }

        return KErrNone;
        }

    return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::ConvertL()
// Converts image file synchronoysly.
// -----------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CVcxNsImageConverter::ConvertL(
    const TDesC& aFileName,
    TSize aImageSize,
    TBool aCreateMask )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsImageConverter::ConvertL(picture) %S", &aFileName);

    CGulIcon* icon = NULL;

    // Start sycnhronous image convert
    iSynchronous = ETrue;

    if ( StartConvertL( aFileName, aImageSize, aCreateMask ) == KErrNone)
        {
        CActiveScheduler::Start();

        if ( iStatus == KErrNone )
            {
            if ( !iImageScalable ) // use AknIconUtils:ScaleBitmap to scale image.
                {
                ScaleBitmapL( iBitmap, iMifImageSize );
                ScaleBitmapL( iMask, iMifImageSize );
                }

            icon = CGulIcon::NewL( iBitmap, iMask );
            icon->SetBitmapsOwnedExternally( EFalse );

            iBitmap = NULL; // Not owned by us anymore.
            iMask   = NULL;
            }
        }

    return icon;
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::ConvertL()
// Converts mif file synchronoysly.
// -----------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CVcxNsImageConverter::ConvertL(
    const TDesC& aFileName,
    TSize aImageSize,
    TInt aBitmapId,
    TInt aMaskId )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsImageConverter::ConvertL(mif) %S", &aFileName);

	CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    if ( aFileName.Length() == 0 )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::ConvertL(mif) file did not exist.");
        return NULL;
        }

    // Skip the file-existence check with file names starting with \ -character.
    // BaflUtils cannot handle these (e.g. select drive) so it would fail.
    if ( aFileName[0] != '\\' )
        {
        if ( ! BaflUtils::FileExists( iFsSession, aFileName ) )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::ConvertL(mif) file did not exist.");
            return NULL;
            }
        }

    AknIconUtils::CreateIconLC(bitmap, mask, aFileName, aBitmapId, aMaskId);

    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    CleanupStack::PushL(icon);

    // ownership of bitmap and mask transferred to icon
    icon->SetBitmapsOwnedExternally(EFalse);

    AknIconUtils::SetSize(bitmap, aImageSize);
    AknIconUtils::SetSize(mask, aImageSize);

    CleanupStack::Pop(icon);
    CleanupStack::Pop(mask);
    CleanupStack::Pop(bitmap);

    return icon;
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::StartConvertL()
// Starts converting image file asynchronously.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVcxNsImageConverter::StartConvertL(
    const TDesC& aFileName,
    TSize aReqSize,
    TBool aCreateMask )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsImageConverter::StartConvertL(picture) %S", &aFileName);

    if ( IsActive() )
        {
        return KErrInUse;
        }

    if ( iDecoder )
        {
        iDecoder->Cancel();
        delete iDecoder;
        iDecoder = NULL;
        }

    if ( ! BaflUtils::FileExists( iFsSession, aFileName ) )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::StartConvertL(picture) file did not exist.");
        return KErrNotFound;
        }

    TRAPD( createError, { iDecoder = CImageDecoder::FileNewL( iFsSession, aFileName ); } );

    if ( createError != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::StartConvertL(picture) file not supported?");
        return createError;
        }

    // original image size
    TFrameInfo info     = iDecoder->FrameInfo();
    TSize      trueSize = info.iOverallSizeInPixels;
    TSize      wantedSize( trueSize );
    iImageScalable = info.iFlags & TFrameInfo::EFullyScaleable;

    // Count mismatch of true size to requested size in horisontal and vertical direction.
    TReal verticalMismatch   = ((TReal) trueSize.iHeight) / ((TReal) aReqSize.iHeight);
    TReal horisontalMismatch = ((TReal) trueSize.iWidth)  / ((TReal) aReqSize.iWidth);

    if (verticalMismatch == 0 || horisontalMismatch == 0)
        {
        return KErrDivideByZero;
        }

    // Picture size is more off the requested size horisontally than vertically.
    if (horisontalMismatch > verticalMismatch)
        {
        wantedSize.SetSize((TInt) aReqSize.iWidth,                                   // Width
                         (TInt) (((TReal)trueSize.iHeight) / horisontalMismatch)); // Height
        }
    // Picture size is more off the requested size vertically than horisontally.
    else
        {
        wantedSize.SetSize((TInt) (((TReal)trueSize.iWidth) / verticalMismatch), // Width
                         (TInt) aReqSize.iHeight);                             // Height
        }
    
    // Make sure size we did not go over the requested limits because
    // of rounding errors or other additions made to rectangle.
    if (wantedSize.iWidth > aReqSize.iWidth)
        {
        wantedSize.iWidth = aReqSize.iWidth;
        }
    if (wantedSize.iHeight > aReqSize.iHeight)
        {
        wantedSize.iHeight = aReqSize.iHeight;
        }

    if ( iImageScalable )
        {
        trueSize = wantedSize;
        }
    else
        {
        iMifImageSize = wantedSize;
        }

    // create bitmap for the proper size
    iBitmap = new ( ELeave ) CFbsBitmap();
    iBitmap->Create( trueSize, info.iFrameDisplayMode );

    // create mask if available and requested.
    if ( aCreateMask && (info.iFlags & TFrameInfo::ETransparencyPossible) )
        {
        iMask = new (ELeave) CFbsBitmap;

        if ( info.iFlags & TFrameInfo::EAlphaChannel )
            {
            iMask->Create( trueSize, EGray256 );
            }
        else
            {
            iMask->Create( trueSize, EGray2 );
            }
        }
    else
        {
        iMask = NULL;
        }

    // start converting.
    if (iMask)
        {
        iDecoder->Convert( &iStatus, *iBitmap, *iMask );
        }
    else
        {
        iDecoder->Convert( &iStatus, *iBitmap );
        }

    // start waiting for completion
    iConvertingMif = EFalse;
    SetActive();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::StartConvertL()
// Starts converting mif file asynchronously.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVcxNsImageConverter::StartConvertL(
    const TDesC& aFileName,
    TSize aImageSize,
    TInt aBitmapId,
    TInt aMaskId )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsImageConverter::StartConvertL(mif) %S", &aFileName);

    if ( IsActive() )
        {
        return KErrInUse;
        }

    delete iMifFileName;
    iMifFileName = NULL;

    iMifFileName  = HBufC16::NewL( aFileName.Length() );
    *iMifFileName = aFileName;
    iMifImageSize = aImageSize;
    iMifBitmapId  = aBitmapId;
    iMifMaskId    = aMaskId;

    iConvertingMif = ETrue;

    // Sets this object as active and completes immediately, but asynchronously.
    SetActive();
    iStatus = KRequestPending;
    TRequestStatus* stat = &iStatus;
    User::RequestComplete(stat, KErrNone);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::RunL()
// From CActive, called when converting is done
// -----------------------------------------------------------------------------
//
void CVcxNsImageConverter::RunL()
    {
    if ( iConvertingMif )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::RunL() after mif conversion.");

        CGulIcon* icon = ConvertL( *iMifFileName, iMifImageSize, iMifBitmapId, iMifMaskId );

        iObserver.ImageConversionCompletedL( (icon ? KErrNone : KErrGeneral), icon );
        }
    else // Converting image
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::RunL() after image conversion.");
        
        // Free decoder. Otherwise the last image file (on disk) stays reserved
        // until client releases this object (CVcxNsImageConverter).
        if ( iDecoder )
            {
            iDecoder->Cancel();
            delete iDecoder;
            iDecoder = NULL;
            }

        // On error cases, free the already reserved bitmaps.
        if ( iStatus != KErrNone )
            {
            if ( iBitmap )
                {
                delete iBitmap;
                iBitmap = NULL;
                }
            if ( iMask )
                {
                delete iMask;
                iMask = NULL;
                }
            }

        if( iSynchronous )
            {
            // Stop the scheduler. Rest is handlen in ConvertL method
            iSynchronous = EFalse;
            CActiveScheduler::Stop();
            }
        else
            {
            if ( iStatus == KErrNone )
                {
                if ( !iImageScalable ) // use AknIconUtils:ScaleBitmapL to scale image.
                    {
                    ScaleBitmapL( iBitmap, iMifImageSize );
                    ScaleBitmapL( iMask, iMifImageSize );
                    }

                // set converted image to icon array
                CreateAndSetIconL();
                }
            else
                {
                // In case of error, we just set default icon for application
                ReportErrorL();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::DoCancel()
// -----------------------------------------------------------------------------
//
void CVcxNsImageConverter::DoCancel()
    {
    if ( iDecoder )
        {
        iDecoder->Cancel();
        delete iDecoder;
        iDecoder = NULL;
        }

    delete iBitmap;
    iBitmap = NULL;

    delete iMask;
    iMask = NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::RunError()
// -----------------------------------------------------------------------------
//
TInt CVcxNsImageConverter::RunError(TInt aError)
    {
    DoCancel();

    TRAP_IGNORE( iObserver.ImageConversionCompletedL( aError, NULL ) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::CreateAndSetIconL()
// -----------------------------------------------------------------------------
//
void CVcxNsImageConverter::CreateAndSetIconL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::CreateAndSetIconL()");

    CGulIcon* icon = CGulIcon::NewL( iBitmap, iMask );
    CleanupStack::PushL( icon );
    icon->SetBitmapsOwnedExternally( EFalse );

    iBitmap = NULL; // Not owned by us anymore.
    iMask   = NULL;

    iObserver.ImageConversionCompletedL( KErrNone, icon );

    CleanupStack::Pop( icon );
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::ReportErrorL()
// -----------------------------------------------------------------------------
//
void CVcxNsImageConverter::ReportErrorL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsImageConverter::ReportErrorL()");

    iObserver.ImageConversionCompletedL( iStatus.Int(), NULL );
    }

// -----------------------------------------------------------------------------
// CVcxNsImageConverter::GetExtension()
// -----------------------------------------------------------------------------
//
TBool CVcxNsImageConverter::GetExtension(const TDesC& aPath, TDes& aExtension)
    {
    if (aPath.Length() > 0)
        {
        TInt dot = aPath.LocateReverse('.');

        if (dot != KErrNotFound)
            {
            TPtrC16 suffixPtr = aPath.Mid(dot);
            if ( suffixPtr.Length() >= KSuffixPtrMinLength &&
                 suffixPtr.Length() <= KSuffixPtrMaxLength)
                {
                aExtension.Copy(suffixPtr);

                return ETrue;
                }
            else
                {
                aExtension.Zero();
                }
            }
        }

    return EFalse;
    }
