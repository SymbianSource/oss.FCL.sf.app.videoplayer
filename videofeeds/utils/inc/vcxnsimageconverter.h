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


#ifndef C_VCXNSIMAGECONVERTER_H
#define C_VCXNSIMAGECONVERTER_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CGulIcon;
class CImageDecoder;
class CFbsBitmap;
class MVcxNsImageConverterObserver;

// CONSTS

const TInt KFileExtensionLength( 10 );
const TInt KSuffixPtrMinLength( 2 );
const TInt KSuffixPtrMaxLength( 9 );


/**
 *  Converter that creates icons from picture files.
 *
 *  @lib IptvUtil.lib
 *  @since S60 v3.0
 */
class CVcxNsImageConverter : public CActive
    { 
public: // Data types
    
    /** Image type enumeration */
    enum TImageType
        {
        /** Image is of type .mif */
        EMif,
        /** Image is of other type */
        EOther
        };

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    *
    * @param aObserver Reference to image converter observer.
    * @param aFsSession File Server session.
    */
    IMPORT_C static CVcxNsImageConverter* NewL( 
            MVcxNsImageConverterObserver& aObserver, RFs& aFsSession );

    /**
    * Destructor.
    */
    virtual ~CVcxNsImageConverter();

public:

    /**
    * Checks image type.
    *
    * @param  aFileName  File name of image which type to check.
    * @param  aImageType On return, image type as TImageType enumeration.
    * @return KErrNone if successful.
    */
    IMPORT_C TInt GetImageType( const TDesC& aFileName, 
                                TImageType& aImageType );

    /**
    * Converts picture file (jpg/gif/png/etc.) synchronously.
    *
    * @param aFileName   Name (and path) of the source image file.
    * @param aImageSize  Size of target icon.
    * @param aCreateMask ETrue if icon should have mask, EFalse otherwise.
    * @return New icon if successful, NULL otherwise.
    */
    IMPORT_C CGulIcon* ConvertL( const TDesC& aFileName, TSize aImageSize, 
                                 TBool aCreateMask );

    /**
    * Converts mif file (mif) synchronously.
    *
    * @param aFileName  Name (and path) of the source image file.
    * @param aImageSize Size of target icon.
    * @param aBitmapId  ID of bitmap in mif file.
    * @param aMaskId    ID of mask in mif file.
    * @return New icon if successful, NULL otherwise.        
    */
    IMPORT_C CGulIcon* ConvertL( const TDesC& aFileName, TSize aImageSize, 
                                 TInt aBitmapId, TInt aMaskId );

    /**
    * Converts file (jpg/gif/png/etc.) asynchronously.
    *
    * @param aFileName   Name (and path) of the source image file.
    * @param aImageSize  Size of target icon.
    * @param aCreateMask ETrue if icon should have mask, EFalse otherwise.
    * @return KErrNone, or one of the system wide error codes if could not
    *       start conversion.
    */
    IMPORT_C TInt StartConvertL( const TDesC& aFileName, TSize aImageSize, 
                                 TBool aCreateMask );

    /**
    * Converts mif file (mif) asynchronously.
    *
    * @param aFileName  Name (and path) of the source image file.
    * @param aImageSize Size of target icon.
    * @param aBitmapId  ID of bitmap in mif file.
    * @param aMaskId    ID of mask in mif file.
    * @return KErrNone, or one of the system wide error codes if could not 
    *       start conversion.
    */
    IMPORT_C TInt StartConvertL( const TDesC& aFileName, TSize aImageSize, 
                                 TInt aBitmapId, TInt aMaskId );

private: // From CActive

    /**
    * Handles an active object’s request completion event. Image conversion 
    * complete.
    */
    void RunL();

    /**
    * Implements cancellation of an outstanding request, e.g. image 
    * conversion.
    */
    void DoCancel();

    /**
    * Handles leaves in RunL. Frees memory and informs client.
    *
    * @param aError Error code in RunL leave.
    */
    TInt RunError( TInt aError );

private:     

    /**
    * Constructor
    *
    * @param aObserver Reference to image converter observer.
    * @param aFsSession File Server session.
    */
    CVcxNsImageConverter( MVcxNsImageConverterObserver& aObserver, 
                              RFs& aFsSession );

    /**
    * EPOC constructor.
    */
    void ConstructL();
    
    /**
    * Gets extension from file name.
    *
    * @param aPath Path and file name from which to extract the extension from.
    * @param aExtension On return, extension of the file.
    * @return ETrue if successful, EFalse otherwise.
    */
    TBool GetExtension( const TDesC& aPath, TDes& aExtension );
    
    /**
    * Creates icon after asynchronous conversion and informs observer.
    */
    void CreateAndSetIconL();

    /**
    * Informs observer about a conversion error in asynchronous conversion.
    */
    void ReportErrorL();

private: // Data
    
    // Image decoder.
    CImageDecoder*               iDecoder;
    
    // Bitmap for converted images.
    CFbsBitmap*                  iBitmap;
    // Bitmap mask for converted images.
    CFbsBitmap*                  iMask;
    
    // File server session.
    RFs&                         iFsSession;
    // Used to flag synchronous decoding.
    TBool                        iSynchronous;
    
    // Used to flag that conversion is done to mif file.
    TBool                        iConvertingMif;
    // Stores mif file name in asynchronous case.
    HBufC*                       iMifFileName;
    // Stores mif image size in asynchronous case.
    TSize                        iMifImageSize;
    // Stores mif bitmap Id in asynchronous case.
    TInt                         iMifBitmapId;
    // Stores mif mask Id in asynchronous case.
    TInt                         iMifMaskId;
    
    // Observer.
    MVcxNsImageConverterObserver& iObserver;
    
    // Flag for knowing if image is fully scalable or not
    TBool iImageScalable;
    };

#endif // C_VCXNSIMAGECONVERTER_H
