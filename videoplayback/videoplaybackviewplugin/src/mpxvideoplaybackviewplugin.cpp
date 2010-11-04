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

#include "mpxvideoplaybackview.h"
#include "mpxvideoplaybackviewplugin.h"

//  Member Functions

CMPXVideoPlaybackViewPlugin* CMPXVideoPlaybackViewPlugin::NewLC()
{
	CMPXVideoPlaybackViewPlugin* self = new (ELeave) CMPXVideoPlaybackViewPlugin;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CMPXVideoPlaybackViewPlugin* CMPXVideoPlaybackViewPlugin::NewL()
{
	CMPXVideoPlaybackViewPlugin* self = CMPXVideoPlaybackViewPlugin::NewLC();
	CleanupStack::Pop(self);
	return self;
}

CMPXVideoPlaybackViewPlugin::CMPXVideoPlaybackViewPlugin()
// note, CBase initialises all member variables to zero
{
}

void CMPXVideoPlaybackViewPlugin::ConstructL()
{
}

CMPXVideoPlaybackViewPlugin::~CMPXVideoPlaybackViewPlugin()
{
}

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CMPXVideoPlaybackViewPlugin::ConstructViewLC()
{
    return CMPXVideoPlaybackView::NewLC();
}

//  End of File
