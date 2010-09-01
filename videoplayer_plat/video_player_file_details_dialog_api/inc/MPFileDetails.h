/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Container for file details*
*/


// Version : %version: 3 %




#ifndef MPFILEDETAILS_H
#define MPFILEDETAILS_H

//  INCLUDES
#include <f32file.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CMPVideoPlayerUIController;


enum TMediaPlayerExpireConstraint
    {
    EMediaPlayerNone,
    EMediaPlayerCount,
    EMediaPlayerTime,
    EMediaPlayerCountAndTime,
    EMediaPlayerAccumulatedTime
    };

enum TMediaPlayerRightsStatus 
    {
    EMediaPlayerFull,
    EMediaPlayerMissing,
    EMediaPlayerRestricted,
    EMediaPlayerExpired,
    EMediaPlayerPreview,
    KMediaPlayerWmdrmValid,
    KMediaPlayerWmdrmExpired
    }; 


// CLASS DECLARATION

/**
*  CMPFileDetails 
*  Container class for file details
*
*  @lib MPEngine.lib
*  @since 2.0
*/
NONSHARABLE_CLASS(CMPFileDetails) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C CMPFileDetails();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMPFileDetails();

    public:    // Data

        HBufC* iTitle;
        HBufC* iFileName;
		HBufC* iFilePath;
        HBufC* iUrl;
        HBufC* iFormat;
        HBufC* iCopyright;      
        HBufC* iArtist;
        HBufC* iAlbum;
        HBufC* iYear;

        HBufC* iOriginalArtist; 
        HBufC* iAlbumTrack;
        HBufC* iGenre;
        HBufC* iComposer;
        HBufC* iAudioFileWebPage;
        HBufC* iComment;
        HBufC* iProvider;
        HBufC* iDescription;

        // For X-pcs Metadata
        HBufC* iArtistXpcs;
        HBufC* iTitleXpcs;
        HBufC* iAlbumXpcs;
        HBufC* iInfoXpcs;
        
        TInt   iResolutionWidth;
        TInt   iResolutionHeight;
        TInt   iBitrate;
        TInt   iSamplerate;
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        TInt64 iSize;
#else
        TInt   iSize;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        TInt   iMaxVolume;
 
        TInt64 iDurationInSeconds;
    
        TTime  iTime;
        
        TBool  iLiveStream;
        TBool  iSeekable;
        TBool  iAudioTrack;
        TBool  iVideoTrack;

        // Drm stuff
        TBool   iDrmProtected;
        TMediaPlayerRightsStatus iDRMRightsStatus;
        TMediaPlayerExpireConstraint iDRMExpireConstraint;
        TBool   iDRMForwardLocked;
        TUint32 iDRMCountsLeft;

        TBool   iDRMFileHasInterval;
        TBool   iDRMIntervalActive;
        TTimeIntervalSeconds iDRMInterval;
        TTimeIntervalSeconds iDRMAccumulatedTime;

        TTime   iDRMValidFrom;
        TTime   iDRMValidUntil;
        
        TBool   iDRMhasInfoURL;
        HBufC*  iDRMInfoURL; //DRM2
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        RFile64 iDetailsFileHandle; 
#else
        RFile   iDetailsFileHandle; 
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        TBool   iReadingDCFOnly;  //True when just DCF header info needs to be read
    };

#endif      // MPFILEDETAILS_H  
            
// End of File
