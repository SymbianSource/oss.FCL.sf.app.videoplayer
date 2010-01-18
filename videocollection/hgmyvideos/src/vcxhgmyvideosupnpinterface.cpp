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
* Description:      CCVcxuPnPInterface class deifnition*
*/




// INCLUDE FILES
#include <upnpshowcommand.h>
#include "vcxhgmyvideosupnpinterface.h"

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
    Stop();
    delete iShowCommand;
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
    self->iExtObserver = aExtObserver;
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
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosUPnPInterface::ConstructL()
    {
    TryCreateCmd();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::TryCreateCmd()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosUPnPInterface::TryCreateCmd()
    {
    if( !iShowCommand )
        {
        TRAPD( showCreate, iShowCommand = CUpnpShowCommand::NewL( this ) );
        if( showCreate != KErrNone )
            {    
            iShowCommand = NULL;
            return EFalse;
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::IsApplicable()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosUPnPInterface::IsApplicable() 
    {

    if( !TryCreateCmd() )
        {
        return EFalse;
        }

    TBool available( EFalse );
    TRAPD(err, available = iShowCommand->IsAvailableL() );
               
    if( err != KErrNone || !available ) 
        {
        return EFalse; 
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::IsStarted()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosUPnPInterface::IsStarted() 
    {

    if( !TryCreateCmd() )
        {
        return EFalse;        
        }
    return iStarted;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::Start()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosUPnPInterface::Start()
    { 
 
    if( !TryCreateCmd() )
        {
        iStarted = EFalse;
        return KErrGeneral;
        }
    TRAPD(err, iShowCommand->StartShowingL() );
    
    if( err != KErrNone )
        {
        iStarted = EFalse;
        }
    else
        {
        iStarted = ETrue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::ShowVideo()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosUPnPInterface::ShowVideo( const TDesC& aVideoPath )
    {
    TInt err( KErrGeneral );   
    if( iStarted && aVideoPath.Length() > 0 && TryCreateCmd() )
        {
        TRAP( err, iShowCommand->ShowVideoL( aVideoPath ) );
        }  
    return err;
    }
 
// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::ShowThumbnail()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosUPnPInterface::ShowThumbnail( const TDesC& aPath )
    {
    TInt err( KErrGeneral );
    if( iStarted && aPath.Length() > 0, TryCreateCmd() )
        {
        TRAP( err, iShowCommand->ShowImageL( aPath ) );  
        }
        
    return err;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::Stop()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosUPnPInterface::Stop()
    {
    TInt err( KErrNone );
    if( iStarted )
        {
        if(  TryCreateCmd() )
            {
            TRAP( err, iShowCommand->StopShowingL( ) );
            }
        else
            {
            err = KErrGeneral;
            }
        iStarted = EFalse;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosUPnPInterface::CommandComplete()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosUPnPInterface::CommandComplete( TInt aStatusCode )
    {
    if( aStatusCode == KErrDisconnected )
        {             
        iStarted = EFalse;
        }
    if( iExtObserver )
        {
        iExtObserver->CommandComplete( aStatusCode );
        }
    }


