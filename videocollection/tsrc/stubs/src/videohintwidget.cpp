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
#include "videohintwidgetdata.h"

int VideoHintWidgetData::mSettedHintLevel = -1;
bool VideoHintWidgetData::mSettedButtonShowLevel = false;

VideoHintWidget::VideoHintWidget(VideoCollectionUiLoader *uiLoader,
    QGraphicsItem *parent):
    HbWidget(parent),
    mUiLoader(uiLoader),
    mCurrentLevel(AllVideos)
{
    // NOP
}

VideoHintWidget::~VideoHintWidget()
{
    // nop
}

void VideoHintWidget::initialize()
{
    // not stubbed
    return;
}

void VideoHintWidget::setLevel(HintLevel level)
{
    VideoHintWidgetData::mSettedHintLevel = (int)level; 
}

void VideoHintWidget::setButtonShown(bool shown)
{
    VideoHintWidgetData::mSettedButtonShowLevel = shown;
}

void VideoHintWidget::orientationChangedSlot(Qt::Orientation targetOrientation)
{
    Q_UNUSED(targetOrientation);
    // not stubbed
}

void VideoHintWidget::activate()
{
    setVisible(true);
}

void VideoHintWidget::deactivate()
{
    setVisible(false);
}

void VideoHintWidget::updateUiComponents()
{
    // not stubbed
}

// end of file
