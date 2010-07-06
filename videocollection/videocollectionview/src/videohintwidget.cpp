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

// Version : %version: 14 %

// INCLUDE FILES
#include <hbpushbutton.h>
#include <hblabel.h>
#include <qgraphicsitem.h>
#include <hbinstance.h>

#include "videohintwidget.h"
#include "videocollectionuiloader.h"
#include "videocollectionviewutils.h"
#include "videocollectiontrace.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoHintWidget::VideoHintWidget( VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent ) 
    : HbWidget( parent )
    , mUiLoader( uiLoader )
    , mServiceIcon( 0 )
    , mCurrentLevel( AllVideos )
    , mButtonShown( false )
    , mActivated( false )
{
    FUNC_LOG;
    // NOP
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoHintWidget::~VideoHintWidget()
{
    FUNC_LOG;
    delete mServiceIcon;
}

// ---------------------------------------------------------------------------
// initialize
// ---------------------------------------------------------------------------
//
int VideoHintWidget::initialize()
{
    FUNC_LOG;
    VideoCollectionViewUtils& utils = VideoCollectionViewUtils::instance();
    return utils.getServiceIconStrings(mServiceIconString, mServiceIconPressedString);
}

// ---------------------------------------------------------------------------
// setLevel
// ---------------------------------------------------------------------------
//
void VideoHintWidget::setLevel(HintLevel level)
{
    FUNC_LOG;
    INFO_1("VideoHintWidget::setLevel() level: %d", level);
    mCurrentLevel = level;
    if(mActivated) {
        updateUiComponents();
    }
}

// ---------------------------------------------------------------------------
// setButtonShown
// ---------------------------------------------------------------------------
//
void VideoHintWidget::setButtonShown(bool shown)
{
    FUNC_LOG;
    INFO_1("VideoHintWidget::setButtonShown() shown: %d", shown);
	mButtonShown = shown;
    if(mActivated) {
        updateUiComponents();
    }
}

// ---------------------------------------------------------------------------
// orientationChanged
// ---------------------------------------------------------------------------
//
void VideoHintWidget::orientationChangedSlot(Qt::Orientation targetOrientation)
{
    FUNC_LOG;
    Q_UNUSED(targetOrientation);
    updateUiComponents();
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
void VideoHintWidget::activate()
{
    FUNC_LOG;
    if (!mActivated)
    {
        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
        connect(
            mainWnd, SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(orientationChangedSlot(Qt::Orientation)));
        
        if(!mServiceIconString.isEmpty())
        {
            mServiceIcon = new HbIcon(mServiceIconString);
        }
        if(mServiceIcon && !mServiceIconPressedString.isEmpty())
        {
            mServiceIcon->setIconName(mServiceIconPressedString, QIcon::Normal, QIcon::On);
        }
        
        updateUiComponents();
        
        setVisible(true);
        
        mActivated = true;
    }
}

// ---------------------------------------------------------------------------
// deactivate
// ---------------------------------------------------------------------------
//
void VideoHintWidget::deactivate()
{
    FUNC_LOG;
    if (mActivated)
    {
        mActivated = false;
        
        setVisible(false);
        
        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
        disconnect(
            mainWnd, SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(orientationChangedSlot(Qt::Orientation)));
        
        HbPushButton *serviceButton =
            mUiLoader->findWidget<HbPushButton>(
                DOCML_NAME_HINT_BUTTON);
        if (serviceButton)
        {
            serviceButton->setIcon(HbIcon());
        }

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
    FUNC_LOG;
    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
    if (mainWnd)
    {
        HbPushButton *serviceButton =
            mUiLoader->findWidget<HbPushButton>(
                DOCML_NAME_HINT_BUTTON);
        HbLabel *hintLabel =
            mUiLoader->findWidget<HbLabel>(
                DOCML_NAME_HINT_LABEL);
        HbLabel *noVideosLabel =
            mUiLoader->findWidget<HbLabel>(
                DOCML_NAME_NO_VIDEOS_LABEL);
        if (serviceButton && hintLabel && noVideosLabel)
        {
            noVideosLabel->setVisible(true);
            if (mServiceIcon && mCurrentLevel == AllVideos)
            {
                hintLabel->setVisible(true);
                serviceButton->setIcon(*mServiceIcon);
                serviceButton->setVisible(mButtonShown);
            }
            else
            {
                hintLabel->setVisible(false);
                serviceButton->setVisible(false);
            }
        }
    }
}

// end of file
