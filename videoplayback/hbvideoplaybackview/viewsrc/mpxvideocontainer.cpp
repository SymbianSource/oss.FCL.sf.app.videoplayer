/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CCoeControl to provide RWindow for videoplayback
*
*/

// Version : %version:  2 %



#include "mpxvideocontainer.h"
#include "mpxvideo_debug.h"

CMPXVideoContainer::CMPXVideoContainer()
{
    MPX_ENTER_EXIT( _L("CMPXVideoContainer::CMPXVideoContainer()") );
}

CMPXVideoContainer::~CMPXVideoContainer()
{
    MPX_ENTER_EXIT( _L("CMPXVideoContainer::~CMPXVideoContainer()") );

    CloseWindow();
}

void CMPXVideoContainer::ConstructL()
{
    MPX_ENTER_EXIT( _L("CMPXVideoContainer::ConstructL()") );

    CreateWindowL();
    ActivateL();
}

// End of file
