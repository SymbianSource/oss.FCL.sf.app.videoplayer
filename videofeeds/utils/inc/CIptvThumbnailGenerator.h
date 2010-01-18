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
* Description:    Thumbnail generation for IPTV, using S60 Thumbnail Engine or*
*/




#ifndef CIPTVTHUMBNAILGENERATOR_H
#define CIPTVTHUMBNAILGENERATOR_H

#include <e32base.h>

class MIptvThumbnailObserver;

/**
*  Thumbnail generation for IPTV
*
*  @lib    IptvUtil
*  @since  S60 v3.1
*/
class CIptvThumbnailGenerator : public CActive
    {
public:
    /**
    *  Two-phased constructor
    *  
    *  @param  aObserver  Observer to receive thumbnail generation notifications
    *  @param  aPriority  Priority to be given to actual thumbnail generation, as
    *                     defined by CActive::TPriority
    */
    IMPORT_C static CIptvThumbnailGenerator* NewL( MIptvThumbnailObserver& aObserver, TInt aPriority );

    /**
    *  Two-phased constructor
    *  
    *  @param  aObserver  Observer to receive thumbnail generation notifications
    *  @param  aPriority  Priority to be given to actual thumbnail generation, as
    *                     defined by CActive::TPriority
    */
    IMPORT_C static CIptvThumbnailGenerator* NewLC( MIptvThumbnailObserver& aObserver, TInt aPriority );

    /**
    *  Requests thumbnail generation
    *  
    *  Thumbnail generation completation and failures are notified by the registered observer
    *  
    *  @param  aClipFilename  Filename for source .3gp/.mp4 video clip
    *  @param  aThumbnailFilename Filename for .jpg thumbnail to be generated
    *  @param  aResolution Size for generated thumbnail
    */
    virtual void GenerateThumbnailL( const TDesC& aClipFilename, const TDesC& aThumbnailFilename, const TSize aResolution ) = 0;

    /**
    *  Cancels ongoing thumbnail generation
    */
    virtual void CancelThumbnail() = 0;

protected:
    /**
    *  C++ constructor
    */
    CIptvThumbnailGenerator( TInt aPriority );

    };

#endif // CIPTVTHUMBNAILGENERATOR_H
