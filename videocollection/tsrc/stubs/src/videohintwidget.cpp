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
* Description:   Videolist content widget implementation
*
*/

#include "videohintwidget.h"

VideoHintWidget::VideoHintWidget(VideoCollectionUiLoader *uiLoader,
    QGraphicsItem *parent):
    HbWidget(parent),
    mUiLoader(uiLoader),
    mServiceIcon(0),
    mAddVideosIcon(0),
    mCurrentLevel(AllVideos)
{
    // NOP
}

VideoHintWidget::~VideoHintWidget()
{
    // nop
}

int VideoHintWidget::initialize()
{
    // not stubbed
    return 0;
}

void VideoHintWidget::setLevel(HintLevel level)
{
    Q_UNUSED(level);
    // not stubbed
}

void VideoHintWidget::setButtonShown(bool shown)
{
    Q_UNUSED(shown);
    // not stubbed
}

void VideoHintWidget::orientationChangedSlot(Qt::Orientation targetOrientation)
{
    Q_UNUSED(targetOrientation);
    // not stubbed
}

void VideoHintWidget::activate()
{
    // not stubbed
}

void VideoHintWidget::deactivate()
{
    // not stubbed
}

void VideoHintWidget::updateUiComponents()
{
    // not stubbed
}

// end of file
