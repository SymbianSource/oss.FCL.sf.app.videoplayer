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
* Description:   Videolist content widget implementation
*
*/

#include <hblabel.h>
#include <qgraphicsitem.h>

#include "videohintwidget.h"
#include "videocollectionuiloader.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoHintWidget::VideoHintWidget(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent) :
HbWidget(parent),
mUiLoader(uiLoader),
mHint(0)
{
    // NOP
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoHintWidget::~VideoHintWidget()
{
}

// ---------------------------------------------------------------------------
// initialize
// ---------------------------------------------------------------------------
//
int VideoHintWidget::initialize()
{
    mHint = mUiLoader->findWidget<HbLabel>(DOCML_NAME_HINT_LABEL);
    return 0;
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
int VideoHintWidget::activate()
{
    // TODO load graphics.
    return 0;
}

// ---------------------------------------------------------------------------
// deactivate
// ---------------------------------------------------------------------------
//
void VideoHintWidget::deactivate()
{
    // TODO delete graphics (to save IVE memory).
}


// ---------------------------------------------------------------------------
// deactivate
// ---------------------------------------------------------------------------
//
void VideoHintWidget::setLevel(HintLevel level)
{
    // TODO delete graphics (to save IVE memory).
}

// ---------------------------------------------------------------------------
// aboutToChangeOrientation
// ---------------------------------------------------------------------------
//
void VideoHintWidget::aboutToChangeOrientation()
{
}

// ---------------------------------------------------------------------------
// orientationChanged
// ---------------------------------------------------------------------------
//
void VideoHintWidget::orientationChanged( Qt::Orientation targetOrientation )
{
}

// ---------------------------------------------------------------------------
// orientationChanged
// ---------------------------------------------------------------------------
//
void VideoHintWidget::buttonPressedSlot()
{
    // TODO implementation.
}

// end of file
