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
* Description:  MPX Video File Details (QT)
*
*/

// Version : %version: 2 %




#ifndef __MPXVIDEOPLAYBACKVIEWFILEDETAILS__
#define __MPXVIDEOPLAYBACKVIEWFILEDETAILS__

#include <qobject.h>
#include "mpxhelixplaybackplugindefs.h"


// 
//  CLASS DECLARATION
//
class QMPXVideoPlaybackViewFileDetails : public QObject
{
    public:  
        //
        //  Constructor
        //
        QMPXVideoPlaybackViewFileDetails();
        
        //
        //  Destructor.
        //
        virtual ~QMPXVideoPlaybackViewFileDetails();
        
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
};

#endif      // __MPXVIDEOPLAYBACKVIEWFILEDETAILS__  
            
// EOF
