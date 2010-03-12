/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  class for MPX Video File Details
*
*/

// Version : %version: e003sa33#9 %


//
//  INCLUDE FILES
//
#include <f32file.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackviewfiledetails.h"

// ============================ MEMBER FUNCTIONS ===================================================

CMPXVideoPlaybackViewFileDetails*
CMPXVideoPlaybackViewFileDetails::NewL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackViewFileDetails::NewL()"));

    CMPXVideoPlaybackViewFileDetails* p = new (ELeave) CMPXVideoPlaybackViewFileDetails();
    CleanupStack::PushL(p);
    p->ConstructL();
    CleanupStack::Pop(p);
    return p;
}

CMPXVideoPlaybackViewFileDetails::~CMPXVideoPlaybackViewFileDetails()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackViewFileDetails::~CMPXVideoPlaybackViewFileDetails()"));

    ClearFileDetails();
}

void
CMPXVideoPlaybackViewFileDetails::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackViewFileDetails::ConstructL()"));
    iSeekable = ETrue;
}

void
CMPXVideoPlaybackViewFileDetails::ClearFileDetails()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackViewFileDetails::ClearFileDetails()"));

    if ( iMimeType )
    {
        delete iMimeType;
        iMimeType = NULL;
    }

    if ( iTitle )
    {
        delete iTitle;
        iTitle = NULL;
    }

    if ( iArtist )
    {
        delete iArtist;
        iArtist = NULL;
    }

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }

    iPlaybackMode = EMPXVideoLocal;
    iSeekable = ETrue;
    iAudioEnabled = EFalse;
    iVideoEnabled = EFalse;
    iPartialPlayback = EFalse;
    iRNFormat = EFalse;

    iDuration = 0;
    iTvOutConnected   = EFalse;
    iTvOutPlayAllowed = ETrue;
    iDrmProtected = EFalse;
    
    iVideoHeight = 0;
    iVideoWidth  = 0;
    iBitRate = 0;
    iFourCCCode = 0;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackViewFileDetails::GenerateFileNameL
// -------------------------------------------------------------------------------------------------
//
EXPORT_C HBufC* CMPXVideoPlaybackViewFileDetails::GenerateFileNameL()
{
    MPX_ENTER_EXIT( _L( "CMPXVideoPlaybackViewFileDetails::GenerateFileNameL()" ) );
    
	HBufC* fileName = NULL;

	if ( iClipName && iClipName->Length()
			&& EMPXVideoStreaming != iPlaybackMode &&
    		EMPXVideoLiveStreaming != iPlaybackMode )
	{
        //
        // Get only file name for media details viewer 
        //
        //TParsePtrC filePath( iClipName->Des() );
        //fileName = ( filePath.Name() ).AllocL();
        TParse parse;
        parse.Set( iClipName->Des(), NULL, NULL);
        fileName = ( parse.Name() ).AllocL();

	}
	
    return fileName;
}

//  EOF
