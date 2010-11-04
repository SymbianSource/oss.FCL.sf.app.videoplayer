/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application class
*
*/

// Version : %version: da1mmcf#4 %


#include "mpxvideoplayerapplication.h"                 // header for this class
#include "mpxvideoplayerdocument.h"                    // application document class
#include "mpxvideoplayerconstants.h"
#include <eikstart.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMpxVideoPlayerApplication::AppDllUid
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CMpxVideoPlayerApplication::AppDllUid() const
    {
    return KUidMpxVideoPlayerApplication;
    }

// -----------------------------------------------------------------------------
// CMpxVideoPlayerApplication::CreateDocumentL
// Creates CMpxVideoPlayerDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CMpxVideoPlayerApplication::CreateDocumentL()
    {
    return CMpxVideoPlayerDocument::NewL( *this );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

LOCAL_C CApaApplication* NewApplication()
    {
    return new CMpxVideoPlayerApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
