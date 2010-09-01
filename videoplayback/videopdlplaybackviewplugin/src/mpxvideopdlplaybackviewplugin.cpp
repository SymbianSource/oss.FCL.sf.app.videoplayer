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
* Description:  Implementation of Video Playback view plugin
*
*/

// Version : %version: 3 %



//  Include Files  

#include "mpxvideopdlplaybackview.h"
#include "mpxvideopdlplaybackviewplugin.h"

//  Member Functions

CMPXVideoPdlPlaybackViewPlugin* CMPXVideoPdlPlaybackViewPlugin::NewLC()
{
    CMPXVideoPdlPlaybackViewPlugin* self = new (ELeave) CMPXVideoPdlPlaybackViewPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CMPXVideoPdlPlaybackViewPlugin* CMPXVideoPdlPlaybackViewPlugin::NewL()
{
    CMPXVideoPdlPlaybackViewPlugin* self = CMPXVideoPdlPlaybackViewPlugin::NewLC();
    CleanupStack::Pop(self);
    return self;
}

CMPXVideoPdlPlaybackViewPlugin::CMPXVideoPdlPlaybackViewPlugin()
// note, CBase initialises all member variables to zero
{
}

void CMPXVideoPdlPlaybackViewPlugin::ConstructL()
{
}

CMPXVideoPdlPlaybackViewPlugin::~CMPXVideoPdlPlaybackViewPlugin()
{
}

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CMPXVideoPdlPlaybackViewPlugin::ConstructViewLC()
{
    return CMPXVideoPdlPlaybackView::NewLC();
}

//  End of File
