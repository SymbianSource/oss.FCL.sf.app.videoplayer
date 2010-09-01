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
* Description:      My Videos Video Details Dialog.*
*/




// INCLUDE FILES
#include <aknmessagequerydialog.h>
#include <mpxmediageneraldefs.h>
#include <vcxmyvideosdefs.h>
#include <vcxhgmyvideos.rsg>
#include "vcxhgmyvideosvideodetailsdialog.h"
#include <MPFileDetailsDialog.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediavideodefs.h>
#include <filedetailspluginif.h>


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoDetailsDialog::CVcxHgMyVideosVideoDetailsDialog()
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosVideoDetailsDialog::CVcxHgMyVideosVideoDetailsDialog()
    {
    
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoDetailsDialog::CVcxHgMyVideosVideoDetailsDialog()
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosVideoDetailsDialog::~CVcxHgMyVideosVideoDetailsDialog()
    {
    if ( iDetailsAo->IsActive() )
        {
        iDetailsAo->Cancel();
        }
    delete iDetailsAo;
    delete iDetailsMedia;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDetailsDialog::NewL
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoDetailsDialog* CVcxHgMyVideosVideoDetailsDialog::NewL()
    {
    CVcxHgMyVideosVideoDetailsDialog* self =
            new (ELeave) CVcxHgMyVideosVideoDetailsDialog();
	CleanupStack::PushL( self );		
    self->ConstructL();
	CleanupStack::Pop( self );
    return self;        
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDetailsDialog::ConstructL
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDetailsDialog::ConstructL()
    {
    iDetailsAo = CIdle::NewL( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoDetailsDialog::ShowVideoDetailsDialogL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDetailsDialog::ShowVideoDetailsDialogL( const CMPXMedia& aMedia )
    {
    if ( iDetailsMedia )
        {
        delete iDetailsMedia;
        iDetailsMedia = NULL;
        }
    iDetailsMedia = CMPXMedia::NewL( aMedia );
    
    ActivateDetailsActiveObject();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDetailsDialog::ActivateDetailsActiveObject
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDetailsDialog::ActivateDetailsActiveObject()
    {
    if ( !iDetailsAo->IsActive() )
        {
        iDetailsAo->Start( TCallBack( CVcxHgMyVideosVideoDetailsDialog::ShowDetailsL, this ) );
        }
    }

// -------------------------------------------------------------------------------------------------
//   CVcxHgMyVideosVideoDetailsDialog::ShowDetailsL
// -------------------------------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoDetailsDialog::ShowDetailsL( TAny* aPtr )
{
    static_cast<CVcxHgMyVideosVideoDetailsDialog*>(aPtr)->DoShowDetailsL();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CVcxHgMyVideosVideoDetailsDialog::DoShowDetailsL
// -------------------------------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDetailsDialog::DoShowDetailsL()
    {
    if ( iDetailsMedia )
        {
        CFileDetailsPluginIF* fdPlugin = CFileDetailsPluginIF::NewL();
        CleanupStack::PushL( fdPlugin );
        fdPlugin->ShowFileDetailsL( *iDetailsMedia );
        CleanupStack::PopAndDestroy();
        }
    }


