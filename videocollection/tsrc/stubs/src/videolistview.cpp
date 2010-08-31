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
* Description:   Videolist view class source code
*
*/

#include "videolistview.h"
#include "videocollectionviewutils.h"
#include "videocollectionwrapper.h"

#include "videolistviewdata.h"

int VideoListViewData::mActivateViewCount = 0;
int VideoListViewData::mBackCount = 0;
int VideoListViewData::mDeactivateViewCount = 0;
int VideoListViewData::mInitializeViewCount = 0;
TMPXItemId VideoListViewData::mActivatedItemId = TMPXItemId::InvalidId();

VideoListView::VideoListView(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent) :
HbView(parent),
mUiUtils(VideoCollectionViewUtils::instance()),
mWrapper(VideoCollectionWrapper::instance()),
mUiLoader(uiLoader),
mModelReady(false),
mVideoServices(0),
mCurrentList(0),
mToolbarViewsActionGroup(0),
mToolbarCollectionActionGroup(0),
mToolbarServiceExtension(0)

{
    // not stubbed
}

VideoListView::~VideoListView()
{
    // disconnect signals
    disconnect();
}

int VideoListView::initializeView()
{
    // not stubbed
    VideoListViewData::mInitializeViewCount++;
    return 0;
}

void VideoListView::titleReadySlot(const QString& title)
{
    Q_UNUSED(title);
    // not stubbed
}

int VideoListView::activateView(TMPXItemId &itemId)
{
    Q_UNUSED(itemId);
    
    VideoListViewData::mActivatedItemId = itemId;
    VideoListViewData::mActivateViewCount++;
    return 0;
}

void VideoListView::modelReadySlot()
{
    // not stubbed
}

void VideoListView::albumListReadySlot()
{
    // not stubbed
}

void VideoListView::layoutChangedSlot()
{
    // not stubbed
}

void VideoListView::deactivateView()
{
    VideoListViewData::mDeactivateViewCount++;
}

void VideoListView::back()
{
    VideoListViewData::mBackCount++;
}

void VideoListView::modelReady()
{
    // not stubbed
}

void VideoListView::cleanup()
{
    // not stubbed
}

int VideoListView::createToolbar()
{
    // not stubbed
    return 0;
}

void VideoListView::createOperatorServicesToolbar()
{
    // not stubbed
}

void VideoListView::loadOperatorService(int titleKey, int iconKey, int uriKey, int uidKey)
{
    // not stubbed
    Q_UNUSED(titleKey);
    Q_UNUSED(iconKey);
    Q_UNUSED(uriKey);
    Q_UNUSED(uidKey);
}

HbAction* VideoListView::createAction(QString icon,
    QActionGroup* actionGroup,
    const char *slot)
{
    Q_UNUSED(icon);
    Q_UNUSED(actionGroup);
    Q_UNUSED(slot);
    // not stubbed
    return 0;
}

void VideoListView::showHint(bool show)
{
    Q_UNUSED(show);
    // not stubbed
}

void VideoListView::setHintLevel(VideoHintWidget::HintLevel level)
{
    Q_UNUSED(level);
    // not stubbed
}

void VideoListView::updateSubLabel()
{
    // not stubbed
}

void VideoListView::showAction(bool show, const QString &name)
{
    Q_UNUSED(show);
    Q_UNUSED(name);
    // not stubbed
}

void VideoListView::openAllVideosViewSlot()
{
    // not stubbed
}

void VideoListView::openCollectionViewSlot()
{
    // not stubbed
}

void VideoListView::openOperatorServiceSlot()
{
    // not stubbed
}

void VideoListView::startSorting()
{
    // not stubbed
}

void VideoListView::aboutToChangeOrientationSlot()
{
    // not stubbed
}

void VideoListView::orientationChangedSlot(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
    // not stubbed
}

void VideoListView::deleteItemsSlot()
{
    // not stubbed
}

void VideoListView::createCollectionSlot()
{
    // not stubbed
}

void VideoListView::addVideosToCollectionSlot()
{
    // not stubbed
}

void VideoListView::removeVideosFromCollectionSlot()
{
    // not stubbed
}

void VideoListView::aboutToShowMainMenuSlot()
{
    // not stubbed
}

void VideoListView::prepareBrowseServiceMenu()
{
    // not stubbed
}

void VideoListView::handleAsyncStatusSlot(int statusCode, QVariant &additional)
{
    Q_UNUSED(statusCode);
    Q_UNUSED(additional);
    // not stubbed
}

void VideoListView::collectionOpenedSlot(bool collectionOpened,
    const QString& collection,
    const TMPXItemId &id)
{
    Q_UNUSED(collectionOpened);
    Q_UNUSED(collection);
    Q_UNUSED(id);
    // not stubbed
}

void VideoListView::objectReadySlot(QObject *object, const QString &name)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    // not stubbed
}

void VideoListView::doSorting(int value)
{
    Q_UNUSED(value);
}

void VideoListView::doDelayedsSlot()
{
    // not stubbed    
}

void VideoListView::debugNotImplementedYet()
{
    // not stubbed
}

// end of file
