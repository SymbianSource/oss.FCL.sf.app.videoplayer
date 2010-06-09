/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Queues commands to MDS.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include "vcxmyvideosmdscmdqueue.h"

#include <mdesession.h>
#include <mdequery.h>
#include <harvesterclient.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>
#include <e32property.h>
#include "vcxmyvideosalbum.h"

#define protected public
#define private public
#include "vcxmyvideosmdsdb.h"
#undef private
#undef protected

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsCmdQueue::CVcxMyVideosMdsCmdQueue( CVcxMyVideosMdsDb& aMdsDb )
: iMdsDb( aMdsDb )
    {
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::ConstructL()
    {}

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsCmdQueue* CVcxMyVideosMdsCmdQueue::NewL( CVcxMyVideosMdsDb& aMdsDb )
    {
    CVcxMyVideosMdsCmdQueue* self = new(ELeave) CVcxMyVideosMdsCmdQueue( aMdsDb );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsCmdQueue::~CVcxMyVideosMdsCmdQueue()
    {}

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsCmdQueue::Cancel
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::Cancel( CVcxMyVideosMdsDb::TRequestType aType )
    {}

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsCmdQueue::ExecuteCmdL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::ExecuteCmdL( CVcxMyVideosMdsCmd* aCmd )
    {}
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsCmdQueue::CmdFinished
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::CmdFinished()
    {}

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsCmdQueue::DoExecuteCmdL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsCmdQueue::DoExecuteCmdL( CVcxMyVideosMdsCmd* aCmd )
    {}    
// END OF FILE
