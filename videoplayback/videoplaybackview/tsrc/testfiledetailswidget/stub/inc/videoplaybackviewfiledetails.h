/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Video File Details (QT)
*
*/

// Version : %version:  4 %




#ifndef __VIDEOPLAYBACKVIEWFILEDETAILS__
#define __VIDEOPLAYBACKVIEWFILEDETAILS__

#include <qobject.h>
#include "mpxhelixplaybackplugindefs.h"


// 
//  CLASS DECLARATION
//
class VideoPlaybackViewFileDetails : public QObject
{
    public:  
        //
        //  Constructor
        //
        VideoPlaybackViewFileDetails();
        
        //
        //  Destructor.
        //
        virtual ~VideoPlaybackViewFileDetails();
        
        //
        //  Clear all file details
        //
        void clearFileDetails();

    public:    
        //
        // Data
        //
        QString   mClipName;
        QString   mTitle;
        QString   mArtist;
        QString   mMimeType;
        QString   mDescription;
        QString   mLocation;
        QString   mCopyright;
        QString   mLanguage;
        QString   mKeywords;        

        TMPXVideoMode mPlaybackMode;
        bool          mSeekable;
        bool          mPausableStream;
        bool          mAudioEnabled;
        bool          mVideoEnabled;
        bool          mPartialPlayback;
        bool          mRNFormat;
        bool          mTvOutConnected;
        bool          mDrmProtected;
        bool          mMultiItemPlaylist;

        int           mVideoHeight;
        int           mVideoWidth;
        int           mBitRate;
        int           mDuration;
        int           mCreationTime;
        int           mModificationTime;
};

#endif      // __VIDEOPLAYBACKVIEWFILEDETAILS__  
            
// EOF
