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
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <vcxmyvideosdefs.h>
#include <StringLoader.h>
#include <vcxhgmyvideos.rsg>
#include "vcxhgmyvideosvideodetailsdialog.h"
#include <MPFileDetailsDialog.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediavideodefs.h>
#include <filedetailspluginif.h>


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// TVcxHgMyVideosVideoDetailsDialog::TVcxHgMyVideosVideoDetailsDialog()
// ---------------------------------------------------------------------------
//
TVcxHgMyVideosVideoDetailsDialog::TVcxHgMyVideosVideoDetailsDialog()
    {
    }

// ---------------------------------------------------------------------------
// TVcxHgMyVideosVideoDetailsDialog::ShowVideoDetailsDialogL()
// ---------------------------------------------------------------------------
//
void TVcxHgMyVideosVideoDetailsDialog::ShowVideoDetailsDialogL( const CMPXMedia& aMedia )
    {
    CFileDetailsPluginIF* fdPlugin = CFileDetailsPluginIF::NewL();
    fdPlugin->ShowFileDetails( aMedia );
    delete fdPlugin;
    }
