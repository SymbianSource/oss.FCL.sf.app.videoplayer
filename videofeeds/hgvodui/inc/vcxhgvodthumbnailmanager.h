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




#ifndef VCXHGVODTHUMBNAILMANAGER_H
#define VCXHGVODTHUMBNAILMANAGER_H

#include <coemain.h>

#include "vcxnsimageconverterobserver.h"

class CVcxNsImageConverter;
class MVcxHgVodThumbnailObserver;
class CVcxHgVodThumbnailRequest;

// CLASS DECLARATION
/**
 *  Utility class for handling thumbnail loading
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CVcxHgVodThumbnailManager ): public CBase,
                                                public MVcxNsImageConverterObserver
    {
public:
    
    static CVcxHgVodThumbnailManager* NewL( RFs& aFsSession );
    
    static CVcxHgVodThumbnailManager* NewLC( RFs& aFsSession );

    virtual ~CVcxHgVodThumbnailManager();
    
    /**
     * Resets the thumbnail fetching buffer from all clients. Ok, as we should have
     * only one client that actually matters (ie. displays thumbnails on screen).
     */
    void Reset();
    
    /**
     * Adds a new thumbnail fetching request to buffer.
     * 
     * @param aObserver Observer which is notified when thumbnail is ready.
     * @param aThumbnailPath Path to the thumbnail file.
     * @param aRequestedSize Size of the needed thumbnail.
     * @param aClientId Client specific unique id for the thumbnail.
     */
    void AddThumbnailRequestL( MVcxHgVodThumbnailObserver& aObserver,
            const TDesC& aThumbnailPath, TSize aRequestedSize, TInt64 aClientId );

    /**
     * From MVcxNsImageConverterObserver
     * Notify about completed image conversion.
     *
     * @param aError Error code.
     * @param aIcon  Pointer to the created icon, ownership is transferred.
     *               NULL if conversion failed with error.
     */  
    void ImageConversionCompletedL( TInt aError, CGulIcon* aIcon );        

private:    
    
    CVcxHgVodThumbnailManager();

    void ConstructL( RFs& aFsSession );
    
private: // data
    
    /**
     * Converts the images to requested size.
     * Own.
     */
    CVcxNsImageConverter* iImageConverter;
    
    /**
     * Buffer for thumbnail requests. Owns items in array.
     */
    RPointerArray<CVcxHgVodThumbnailRequest> iRequestBuffer;
    
    };

#endif // VCXHGVODTHUMBNAILMANAGER_H
