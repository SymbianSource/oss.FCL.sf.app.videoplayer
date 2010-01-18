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




#ifndef CIPTVTHUMBNAILIMPL_H
#define CIPTVTHUMBNAILIMPL_H

#include <TNEVideoClipInfo.h>
#include <f32file.h>
#include "CIptvThumbnailGenerator.h"
#include "MIptvRtpTo3gpObserver.h"

class CImageEncoder;
class CFbsBitmap;
class CIptvRtpTo3gpConverter;

/**
 * Thumbnail generation implementation using VideoEditorEngine
 * or S60 Thumbnail Engine.
 *
 * @lib IptvUtil.lib
 * @since Video Center 1.0
 */
class CIptvThumbnailImpl : public CIptvThumbnailGenerator,
                           public MIptvRtpTo3gpObserver,
                           public MTNEVideoClipInfoObserver,
                           public MTNEVideoClipThumbObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aObserver
     * @param aPriority
     */
    static CIptvThumbnailImpl* NewLC( MIptvThumbnailObserver& aObserver, TInt aPriority );

    /**
     * Destructor.
     */     
    virtual ~CIptvThumbnailImpl();

    /**
     * Request thumbnail generation.
     *
     * @param aClipFilename
     * @param aThumbFilename
     * @param aResolution
     */     
    void GenerateThumbnailL( const TDesC& aClipFilename, const TDesC& aThumbFilename, const TSize aResolution );

    /**
     * Cancel ongoing thumbnail generation.
     */     
    void CancelThumbnail();

private:

    /**
     * Default C++ construct
     *
     * @param aObserver
     * @param aPriority
     */
    CIptvThumbnailImpl( MIptvThumbnailObserver& aObserver, TInt aPriority );

    /**
     * 2nd phase constructor
     */
    void ConstructL();

    /**
     * from MIptvRtpTo3gpObserver
     *
     * @param aConverter
     * @param aError
     */
    void RtpTo3gpConversionReady( CIptvRtpTo3gpConverter& aConverter, TInt aError );

    /**
     * From MTNEVideoClipInfoObserver
     *
     * @param aInfo
     * @param aError
     */     
    void NotifyVideoClipInfoReady( CTNEVideoClipInfo& aInfo, TInt aError );

    /**
     * From MTNEVideoClipThumbObserver
     *
     * @param aInfo
     * @param aError
     * @param aThumbBitmap
     */     
    void NotifyVideoClipThumbCompleted( CTNEVideoClipInfo& aInfo, TInt aError, CFbsBitmap* aThumbBitmap );

    /**
     * From CActive, image encoding is ready
     */
    void RunL();

    /**
     * From CActive, cancel image encoding
     */
    void DoCancel();

    /**
     * Cleanup results from previous generations
     */     
    void DoCleanup();

    /**
     * Start actual thumbnail generation
     *
     * @param aClipFilename
     */     
    void DoStartThumbnailL( const TDesC& aClipFilename );

private:

    /**
     * Observer for generation completed notifications.
     */     
    MIptvThumbnailObserver& iObserver;
    
    /**
     * Priority for actual thumbnail generation.
     */     
    TInt iPriority;
    
    /**
     * File system handle.
     */     
    RFs iFs;
    
    /**
     * Cache for thumbnail filename.
     * Own.
     */     
    HBufC* iThumbFilename;
    
    /**
     * Cache for thumbnail resolution.
     */     
    TSize iResolution;
    
    /**
     * Memory bitmap to .jpg image encoder.
     * Own.
     */     
    CImageEncoder* iImageEncoder;
    
    /**
     * Memory bitmap.
     * Own.
     */     
    CFbsBitmap* iThumbBitmap;
    
    /**
     * .rtp to .3gp file converter.
     * Own.
     */     
    CIptvRtpTo3gpConverter* iRtpConverter;
    
    /**
     * File name for temporary .3gp clip used in .rtp generation.
     */
    TFileName iTempClipFilename;

    /**
     * Cache for clip info (underlying generator)
     * Own.     
     */     
    CTNEVideoClipInfo* iClipInfo;
    };

#endif // CIPTVTHUMBNAILIMPL_H
