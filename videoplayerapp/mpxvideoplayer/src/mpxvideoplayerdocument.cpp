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
* Description:  Document class
*
*/

// Version : %version: da1mmcf#9 %


#include <AiwGenericParam.h>
#include <mpxviewutility.h>

#include "mpxvideoplayerdocument.h"                    // header for this class
#include "mpxvideoplayerappui.h"                       // application UI class
#include "mpxvideoplayerlogger.h"
#include "mpxvideo_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMpxVideoPlayerDocument::CMpxVideoPlayerDocument
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerDocument::CMpxVideoPlayerDocument( CEikApplication& aApp )
    : CAiwGenericParamConsumer( aApp )
{
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerDocument::ConstructL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerDocument::ConstructL()
{
    // Get view utility. View utility must be created before AppUi.
    iViewUtility = MMPXViewUtility::UtilityL();
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerDocument::NewL
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerDocument* CMpxVideoPlayerDocument::NewL( CEikApplication& aApp )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerDocument::NewL()"));

    CMpxVideoPlayerDocument* self = new ( ELeave ) CMpxVideoPlayerDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerDocument::~CMpxVideoPlayerDocument
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerDocument::~CMpxVideoPlayerDocument()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerDocument::~CMpxVideoPlayerDocument()"));

    if ( iViewUtility )
    {
        iViewUtility->Close();
    }
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerDocument::CreateAppUiL
// -----------------------------------------------------------------------------
//
CEikAppUi* CMpxVideoPlayerDocument::CreateAppUiL()
{
    return new ( ELeave ) CMpxVideoPlayerAppUi();
}

// ---------------------------------------------------------------------------
// From class CAknDocument.
// Called by the framework to open a file.
// ---------------------------------------------------------------------------
//
CFileStore* CMpxVideoPlayerDocument::OpenFileL( TBool aDoOpen,
                                                const TDesC& aFilename,
                                                RFs& aFs )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerDocument::OpenFileL()"),
                   _L("aDoOpen = %d, aFileanme = %S"), aDoOpen, &aFilename );

    if ( aDoOpen )
    {
        //
        //  Parse input parameters here
        //

        iAppUi->OpenFileL( aFilename );
    }

    return CAknDocument::OpenFileL( aDoOpen, aFilename, aFs );
}


// ---------------------------------------------------------------------------
// From class CAknDocument.
// Called by the framework to open a file.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerDocument::OpenFileL( CFileStore*& aFileStore, RFile& aFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerDocument::OpenFileL(RFile)"));

    const CAiwGenericParamList* params = GetInputParameters();

    static_cast<CMpxVideoPlayerAppUi*>( iAppUi )->OpenFileL( aFile, params );

    return CAknDocument::OpenFileL( aFileStore, aFile );
}

// EOF
