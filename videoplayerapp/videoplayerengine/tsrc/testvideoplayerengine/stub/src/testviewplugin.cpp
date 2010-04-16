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
* Description:   Stub test class
*
*/

// INCLUDE FILES
#include "testviewplugin.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TestViewPlugin::TestViewPlugin()
      : mActive(false)
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
TestViewPlugin::~TestViewPlugin()
{
}

// ---------------------------------------------------------------------------
// Create view
// ---------------------------------------------------------------------------
//
void TestViewPlugin::createView()
{
}

// ---------------------------------------------------------------------------
// Destroy view
// ---------------------------------------------------------------------------
//
void TestViewPlugin::destroyView()
{
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
void TestViewPlugin::activateView()
{
    mActive = true;
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
bool TestViewPlugin::activated()
{
    return mActive;
}

// ---------------------------------------------------------------------------
// Deactivate view
// ---------------------------------------------------------------------------
//
void TestViewPlugin::deactivateView()
{
    mActive = false;
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
QGraphicsWidget* TestViewPlugin::getView()
{
    return 0;
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
MpxViewPlugin* TestViewPlugin::viewPlugin()
{
    return this;
}

// ---------------------------------------------------------------------------
// Slot: Orientation change
// ---------------------------------------------------------------------------
//
void TestViewPlugin::orientationChange( Qt::Orientation orientation )
{
}

// ---------------------------------------------------------------------------
// Slot: back
// ---------------------------------------------------------------------------
//
void TestViewPlugin::back()
{
}

// end of file
