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

#include <hbpushbutton.h>
#include <hblabel.h>
#include <qgraphicsitem.h>
#include <hbinstance.h>

#include "videohintwidget.h"
#include "videocollectionuiloader.h"
#include "videocollectionviewutils.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoHintWidget::VideoHintWidget(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent) :
HbWidget(parent),
mUiLoader(uiLoader),
mServiceButton(0),
mHintLabel(0),
mServiceIcon(0),
mCurrentLevel(AllVideos)
{
    // NOP
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoHintWidget::~VideoHintWidget()
{
    delete mServiceIcon;
}

// ---------------------------------------------------------------------------
// initialize
// ---------------------------------------------------------------------------
//
int VideoHintWidget::initialize()
{
    mServiceButton = mUiLoader->findWidget<HbPushButton>(DOCML_NAME_HINT_BUTTON);
    mHintLabel = mUiLoader->findWidget<HbLabel>(DOCML_NAME_HINT_LABEL);
    if(!mServiceButton || !mHintLabel)
    {
        return -1;
    }
    
    VideoCollectionViewUtils& utils = VideoCollectionViewUtils::instance();
    return utils.getServiceIconStrings(mServiceIconString, mServiceIconPressedString);
}

// ---------------------------------------------------------------------------
// setLevel
// ---------------------------------------------------------------------------
//
void VideoHintWidget::setLevel(HintLevel level)
{
    mCurrentLevel = level;
    if(isVisible()) {
        updateUiComponents();
    }
}

// ---------------------------------------------------------------------------
// setVisible
// ---------------------------------------------------------------------------
//
void VideoHintWidget::setVisible(bool visible)
{
    HbWidget::setVisible(visible);
    
    if(visible) {
        activate();
    } else {
        deactivate();
    }
}

// ---------------------------------------------------------------------------
// orientationChanged
// ---------------------------------------------------------------------------
//
void VideoHintWidget::orientationChangedSlot(Qt::Orientation /*targetOrientation*/)
{
    updateUiComponents();
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
void VideoHintWidget::activate()
{
    if(mServiceIconString.isNull() || mServiceIconString.isEmpty() || 
       mServiceIconPressedString.isNull() || mServiceIconPressedString.isEmpty())
    {
        return;
    }
    
    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
    connect(mainWnd, SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(orientationChangedSlot(Qt::Orientation)));
    
    mServiceIcon = new HbIcon(mServiceIconString);
    mServiceIcon->setIconName(mServiceIconPressedString, QIcon::Normal, QIcon::On);
    
    updateUiComponents();
}

// ---------------------------------------------------------------------------
// deactivate
// ---------------------------------------------------------------------------
//
void VideoHintWidget::deactivate()
{
    disconnect(this, SLOT(orientationChangedSlot(Qt::Orientation)));
    
    mServiceButton->setIcon(HbIcon());
    if(mServiceIcon) {
        delete mServiceIcon;
        mServiceIcon = 0;
    }
}

// ---------------------------------------------------------------------------
// updateUiComponents
// ---------------------------------------------------------------------------
//
void VideoHintWidget::updateUiComponents()
{
    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
    mServiceButton->setVisible(mainWnd->orientation() == Qt::Horizontal);
    
    if(mCurrentLevel == Collection) {
        mHintLabel->setVisible(false);
        mServiceButton->setIcon(HbIcon());
        mServiceButton->setText(tr("Add videos"));
    } else {
        mHintLabel->setVisible(true);
        mServiceButton->setText(QString());
        mServiceButton->setIcon(*mServiceIcon);
    }
}

// end of file
