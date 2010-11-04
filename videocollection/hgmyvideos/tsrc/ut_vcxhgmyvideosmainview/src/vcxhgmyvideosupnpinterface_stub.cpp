/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Stub class for unit testing*
*/



#include "VcxHgMyVideosUPnPInterface.h"

// INCLUDE FILES

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::CVcxHgMyVideosuPnPInterface()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosUPnPInterface::CVcxHgMyVideosUPnPInterface()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::~CVcxHgMyVideosuPnPInterface()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosUPnPInterface::~CVcxHgMyVideosUPnPInterface() 
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosUPnPInterface* CVcxHgMyVideosUPnPInterface::NewLC( 
        MUpnpCommandObserver* aExtObserver )
    {
    CVcxHgMyVideosUPnPInterface* self = new ( ELeave )CVcxHgMyVideosUPnPInterface();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosUPnPInterface* CVcxHgMyVideosUPnPInterface::NewL( 
        MUpnpCommandObserver* aExtObserver )
    {
    CVcxHgMyVideosUPnPInterface* self = CVcxHgMyVideosUPnPInterface::NewLC( aExtObserver );    
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosUPnPInterface::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::TryCreateCmd()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosUPnPInterface::TryCreateCmd()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::IsApplicable()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosUPnPInterface::IsApplicable() 
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::IsStarted()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosUPnPInterface::IsStarted() 
    {
    return EFalse;        
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::Start()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosUPnPInterface::Start()
    { 
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::ShowVideo()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosUPnPInterface::ShowVideo( const TDesC& aVideoPath )
    { 
    return KErrNone;
    }
 
// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::ShowThumbnail()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosUPnPInterface::ShowThumbnail( const TDesC& aPath )
    { 
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::Stop()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosUPnPInterface::Stop()
    { 
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::Stop()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosUPnPInterface::CommandComplete( TInt aStatusCode )
    {
    }


