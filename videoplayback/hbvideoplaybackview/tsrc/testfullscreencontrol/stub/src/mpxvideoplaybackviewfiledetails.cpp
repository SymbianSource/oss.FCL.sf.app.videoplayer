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
* Description:  class for MPX Video File Details (Qt)
*
*/

// Version : %version:   2 %



//
//  INCLUDE FILES
//
#include "mpxvideo_debug.h"
#include "mpxvideoplaybackviewfiledetails.h"


// ============================ MEMBER FUNCTIONS ===================================================

QMPXVideoPlaybackViewFileDetails::QMPXVideoPlaybackViewFileDetails()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackViewFileDetails::QMPXVideoPlaybackViewFileDetails()"));

    clearFileDetails();
}

QMPXVideoPlaybackViewFileDetails::~QMPXVideoPlaybackViewFileDetails()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackViewFileDetails::~QMPXVideoPlaybackViewFileDetails()"));

    clearFileDetails();
}

void
QMPXVideoPlaybackViewFileDetails::clearFileDetails()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackViewFileDetails::ClearFileDetails()"));

    mMimeType = tr("");  
    mTitle = tr("");
    mArtist = tr("");
    mClipName = tr("");
    mDescription = tr("");  
    mLocation = tr("");
    mCopyright = tr("");
    mLanguage = tr("");
    mKeywords = tr("");
    
    mPlaybackMode = EMPXVideoLocal;
    mSeekable = false;
    mPausableStream = false;
    mAudioEnabled = false;
    mVideoEnabled = false;
    mPartialPlayback = false;
    mRNFormat = false;

    mDuration = 0;
    mTvOutConnected   = false;
    mDrmProtected = false;

    mVideoHeight = 0;
    mVideoWidth  = 0;
    mBitRate = 0;
}

//  EOF
