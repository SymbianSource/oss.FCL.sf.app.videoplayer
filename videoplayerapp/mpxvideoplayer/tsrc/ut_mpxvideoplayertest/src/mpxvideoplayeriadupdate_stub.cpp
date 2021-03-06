/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    IAD update handling.
*
*/


// Version : %version: 1 %

#include <mpxviewutility.h>

#include "mpxvideoplayeriadupdate_stub.h"


// --------------------------------------------------------------------------
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CMpxVideoPlayerIadUpdate* CMpxVideoPlayerIadUpdate::NewL( MMPXViewUtility& aViewUtility )
    {
    CMpxVideoPlayerIadUpdate* self = new( ELeave ) CMpxVideoPlayerIadUpdate( aViewUtility );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// --------------------------------------------------------------------------
//
CMpxVideoPlayerIadUpdate::CMpxVideoPlayerIadUpdate( MMPXViewUtility& aViewUtility )
  : iViewUtility( aViewUtility )
    {
    // None
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerIadUpdate::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CMpxVideoPlayerIadUpdate::~CMpxVideoPlayerIadUpdate()
    {
    }

// --------------------------------------------------------------------------
// Starts update process.
// --------------------------------------------------------------------------
//
void CMpxVideoPlayerIadUpdate::StartL()
    {
    }
	
