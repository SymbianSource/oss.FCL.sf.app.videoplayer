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
* Description:  Container for MPX Video File Details
*
*/


// Version : %version: 12 %


#ifndef __MPXVIDEOPLAYBACKVIEWFILEDETAILS__
#define __MPXVIDEOPLAYBACKVIEWFILEDETAILS__

#include "mpxhelixplaybackplugindefs.h"

//
//  CLASS DECLARATION
//
NONSHARABLE_CLASS( CMPXVideoPlaybackViewFileDetails ) : public CBase
{
    public:
        //
        //  Constructors and destructor
        //
        static CMPXVideoPlaybackViewFileDetails* NewL();

        //
        //  Destructor.
        //
        virtual ~CMPXVideoPlaybackViewFileDetails();

        //
        //  Clear all file details
        //
        void ClearFileDetails();

        /**
        * Generate file name, remove all file path
        *
        * @return generated file name, ownership is transferred
        */
        IMPORT_C HBufC* GenerateFileNameL();

    private:
        //
        //  By default Symbian 2nd phase constructor is private.
        //
        void ConstructL();

    public:
        //
        // Data
        //
        HBufC*   iClipName;
        HBufC*   iTitle;
        HBufC*   iArtist;
        HBufC*   iMimeType;

        TMPXVideoMode  iPlaybackMode;
        TBool          iSeekable;
        TBool          iPausableStream;
        TBool          iAudioEnabled;
        TBool          iVideoEnabled;
        TBool          iPartialPlayback;
        TBool          iTvOutConnected;
        TBool          iTvOutPlayAllowed;
        TBool          iDrmProtected;

        TInt           iVideoHeight;
        TInt           iVideoWidth;
        TInt           iBitRate;
        TUint32        iFourCCCode;

        TInt           iDuration;
};

#endif      // __MPXVIDEOPLAYBACKVIEWFILEDETAILS__


// EOF