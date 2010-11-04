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
* Description:   Container class for file details*
*/


// Version : %version: 9 %


// INCLUDE FILES
#include "MPFileDetails.h"
#include <featmgr.h>
#include "mpxvideo_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPFileDetails::CMPFileDetails
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPFileDetails::CMPFileDetails()
    : iSeekable( ETrue ), iDRMExpireConstraint( EMediaPlayerNone ), iDRMhasInfoURL(EFalse)
{
    MPX_ENTER_EXIT(_L("CMPFileDetails::CMPFileDetails()"));
}

// -----------------------------------------------------------------------------
// CMPFileDetails::~CMPFileDetails
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMPFileDetails::~CMPFileDetails()
{
    MPX_ENTER_EXIT(_L("CMPFileDetails::~CMPFileDetails()"));

    delete iTitle;
    delete iFileName;
    delete iFilePath;
    delete iUrl;
    delete iFormat;
    delete iCopyright;
    delete iArtist;
    delete iAlbum;
    delete iYear;

    // ID3v2
    delete iOriginalArtist;
    delete iAlbumTrack;
    delete iGenre;
    delete iComposer;
    delete iAudioFileWebPage;
    delete iComment;
    delete iProvider;
    delete iDescription;

    // For X-pcs meta data
    delete iArtistXpcs;
    delete iTitleXpcs;
    delete iAlbumXpcs;
    delete iInfoXpcs;

    //DRM
    if (iDRMInfoURL && iDRMhasInfoURL)
        delete iDRMInfoURL; //DRM2

    if (iDetailsFileHandle.SubSessionHandle())
        iDetailsFileHandle.Close();
}

//  End of File
