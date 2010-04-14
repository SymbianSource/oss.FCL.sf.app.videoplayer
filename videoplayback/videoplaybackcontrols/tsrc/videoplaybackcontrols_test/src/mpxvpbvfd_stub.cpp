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

// Version : %version: e003sa33#8 %


//
//  INCLUDE FILES
//
#include <f32file.h>
#include <uri16.h>

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
    iPausableStream = ETrue;
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

    iSeekable = 0;

    iAudioEnabled = 0;
    iVideoEnabled = 0;

    iDuration = 0;
    iPausableStream = ETrue;
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
        TParsePtrC filePath( iClipName->Des() );
        fileName = ( filePath.Name() ).AllocL();
	}
	
    return fileName;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackViewFileDetails::GenerateFileTitleL
// -------------------------------------------------------------------------------------------------
//
EXPORT_C HBufC* CMPXVideoPlaybackViewFileDetails::GenerateFileTitleL()
{
    MPX_ENTER_EXIT( _L( "CMPXVideoPlaybackViewFileDetails::GenerateFileTitleL()" ) );

    HBufC* fileTitle = NULL;

    if ( iTitle && iTitle->Length() )
    {
        //
        // use file title in metadata as title cotent directly
        //
        fileTitle = iTitle->AllocL();
    }
    else if ( ( EMPXVideoStreaming == iPlaybackMode || EMPXVideoLiveStreaming == iPlaybackMode ) &&
              iClipName && iClipName->Length() )
    {
        //
        //  parse file name in URL
        //
        TUriParser parser;

        if ( parser.Parse( iClipName->Des() ) == KErrNone )
        {

            MPX_DEBUG(
                _L( "    streaming link: schema = %S, urihost = %S, uriPort = %S, uriPath = %S" ),
                &( parser.Extract( EUriScheme ) ),
                &( parser.Extract( EUriHost ) ),
                &( parser.Extract( EUriPort ) ),
                &( parser.Extract( EUriPath ) ) );

            HBufC* nameAndTail = NULL;

            MPX_TRAPD( err, nameAndTail = parser.GetFileNameL( EUriFileNameTail ) );

            if ( KErrNone == err && nameAndTail )
            {
                CleanupStack::PushL( nameAndTail );
                TInt extPos = nameAndTail->Des().LocateReverse( KExtDelimiter );

                if ( extPos > 0 )
                {
                    fileTitle = ( nameAndTail->Des().Left( extPos ) ).AllocL();
                }
                else
                {
                    fileTitle = nameAndTail->Des().AllocL();
                }

                CleanupStack::PopAndDestroy( nameAndTail );
            }
        }
    }

    return fileTitle;
}

//  EOF
