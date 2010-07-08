/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of HbPanGesture
*
*/

// Version : %version:   1 %




#include "mpxvideo_debug.h"
#include "hbpangesture.h"

// -------------------------------------------------------------------------------------------------
// HbPanGesture::HbPanGesture
// -------------------------------------------------------------------------------------------------
//
HbPanGesture::HbPanGesture()
{
    MPX_ENTER_EXIT(_L("HbPanGesture::HbPanGesture()"));
}

// -------------------------------------------------------------------------------------------------
// HbPanGesture::~HbPanGesture
// -------------------------------------------------------------------------------------------------
//
HbPanGesture::~HbPanGesture()
{
    MPX_DEBUG(_L("HbPanGesture::HbPanGesture") );
}

// -------------------------------------------------------------------------------------------------
// HbPanGesture::sceneDelta
// -------------------------------------------------------------------------------------------------
//
QPointF HbPanGesture::sceneDelta()
{
    MPX_DEBUG(_L("HbPanGesture::sceneDelta") );

    return mSceneDelta;
}

// End of file
