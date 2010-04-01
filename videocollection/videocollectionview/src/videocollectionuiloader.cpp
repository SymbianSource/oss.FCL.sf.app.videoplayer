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
* Description:
*
*/

#include <qgraphicswidget.h>
#include <qaction.h>
#include <qactiongroup.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbgroupbox.h>
#include <hbpushbutton.h>

#include "videocollectionuiloader.h"
#include "videolistview.h"
#include "videolistselectiondialog.h"
#include "videolistwidget.h"
#include "videohintwidget.h"
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodel.h"
#include "videocollectionviewutils.h"
#include "videoservices.h"

// ---------------------------------------------------------------------------
// VideoCollectionUiLoader
// ---------------------------------------------------------------------------
//
VideoCollectionUiLoader::VideoCollectionUiLoader():
    HbDocumentLoader(),
    mTimerId(0),
    mSortGroup(0),
    mIsService(false)
{
}

// ---------------------------------------------------------------------------
// VideoCollectionUiLoader
// ---------------------------------------------------------------------------
//
VideoCollectionUiLoader::~VideoCollectionUiLoader()
{
    // selection dialog needs to be deleted manually
    VideoListSelectionDialog *dialog =
        findWidget<VideoListSelectionDialog>(
            DOCML_NAME_DIALOG);
    delete dialog;
    
    // clear queue and hash tables
    mQueue.clear();
    mWidgets.clear();
    mObjects.clear();
}

// ---------------------------------------------------------------------------
// startLoading
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::startLoading(QSet<QString> uiSections,
    QObject *receiver,
    const char *widgetSlot,
    const char *objectSlot)
{
    if (uiSections.contains(DOCML_NAME_VC_HEADINGBANNER))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_VC_HEADINGBANNER,
            true, // is widget
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            widgetSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_VC_VIDEOLISTWIDGET))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_VC_VIDEOLISTWIDGET,
            true, // is widget
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            widgetSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_OPTIONS_MENU))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_OPTIONS_MENU,
            true, // is widget
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            widgetSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_ADD_TO_COLLECTION))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_ADD_TO_COLLECTION,
            false, // is object
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            objectSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_CREATE_COLLECTION))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_CREATE_COLLECTION,
            false, // is object
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            objectSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_DELETE_MULTIPLE))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_DELETE_MULTIPLE,
            false, // is object
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            objectSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_VC_VIDEOHINTWIDGET))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_VC_VIDEOHINTWIDGET,
            true, // is widget
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            widgetSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_HINT_BUTTON))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_HINT_BUTTON,
            true, // is widget
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            widgetSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_NO_VIDEOS_LABEL))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_NO_VIDEOS_LABEL,
            true, // is widget
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            widgetSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_SORT_BY_DATE))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_SORT_BY_DATE,
            false, // is object
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            objectSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_SORT_BY_NAME))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_SORT_BY_NAME,
            false, // is object
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            objectSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS,
            false, // is object
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            objectSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_SORT_BY_TOTAL_LENGTH))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_SORT_BY_TOTAL_LENGTH,
            false, // is object
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            objectSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_SORT_BY_SIZE))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_SORT_BY_SIZE,
            false, // is object
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            objectSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_SORT_MENU))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_SORT_MENU,
            true, // is widget
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            widgetSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_VC_COLLECTIONWIDGET))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_VC_COLLECTIONWIDGET,
            true, // is widget
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            widgetSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET))
    {
        VideoCollectionUiLoader::Params params(
            DOCML_NAME_VC_COLLECTIONCONTENTWIDGET,
            true, // is widget
            receiver,
            DOCML_VIDEOCOLLECTIONVIEW_FILE,
            widgetSlot);
        addToQueue(params);
    }
    if (uiSections.contains(DOCML_NAME_DIALOG))
    {
        VideoCollectionUiLoader::Params params(DOCML_NAME_DIALOG,
            true, // is widget
            receiver,
            DOCML_VIDEOSELECTIONDIALOG_FILE,
            widgetSlot);
        addToQueue(params);
    }
}

// ---------------------------------------------------------------------------
// setIsService
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::setIsService(bool isService)
{
    mIsService = isService;
}

// ---------------------------------------------------------------------------
// doFindWidget
// ---------------------------------------------------------------------------
//
QGraphicsWidget* VideoCollectionUiLoader::doFindWidget(const QString &name)
{
    QGraphicsWidget *widget = 0;
    
    // 1. check from hash
    if (mWidgets.contains(name))
    {
        widget = mWidgets.value(name);
    }
    
    // 2. load from document
    else
    {
        widget = HbDocumentLoader::findWidget(name);
        if (widget)
        {
            // initialize widget
            initWidget(widget, name);
            
            // add it to the hash
            mWidgets.insert(name, widget);

            // check if the widget is being loaded and remove it from queue
            int count = mQueue.count();
            for (int i = 0; i < count; i++)
            {
                const Params& params = mQueue.at(i);
                if (params.mName.compare(name) == 0)
                {
                    if (connect(
                        this, SIGNAL(widgetReady(QGraphicsWidget*, const QString&)),
                        params.mReceiver, params.mMember))
                    {
                        emit widgetReady(widget, params.mName);
                        disconnect(
                            this, SIGNAL(widgetReady(QGraphicsWidget*, const QString&)),
                            params.mReceiver, params.mMember);
                    }
                    mQueue.removeAt(i);
                    break;
                }
            }            
        }
    }
    
    return widget;
}

// ---------------------------------------------------------------------------
// doFindObject
// ---------------------------------------------------------------------------
//
QObject* VideoCollectionUiLoader::doFindObject(const QString &name)
{
    QObject *object = 0;
    
    // 1. check from hash
    if (mObjects.contains(name))
    {
        object = mObjects.value(name);
    }
    
    // 2. load from document and cancel async loading
    else
    {
        object = HbDocumentLoader::findObject(name);
        if (object)
        {
            // initialize widget
            initObject(object, name);
            
            // add it to the hash
            mObjects.insert(name, object);

            // check if the object is being loaded and remove it from queue
            int count = mQueue.count();
            for (int i = 0; i < count; i++)
            {
                const Params& params = mQueue.at(i);
                if (params.mName.compare(name) == 0)
                {
                    if (connect(
                        this, SIGNAL(objectReady(QObject*, const QString&)),
                        params.mReceiver, params.mMember))
                    {
                        emit objectReady(object, params.mName);
                        disconnect(
                            this, SIGNAL(objectReady(QObject*, const QString&)),
                            params.mReceiver, params.mMember);
                    }
                    mQueue.removeAt(i);
                    break;
                }
            }            
        }
    }
    
    return object;
}

// ---------------------------------------------------------------------------
// addToQueue
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::addToQueue(Params &params)
{
    if (isValid(params))
    {
        // add the params in async queue
        mQueue.append(params);
        runNext();
    }
}

// ---------------------------------------------------------------------------
// initWidget
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::initWidget(QGraphicsWidget *widget,
    const QString &name)
{
    if (widget)
    {
        VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
        
        if (name.compare(DOCML_NAME_VC_VIDEOLISTWIDGET) == 0)
        {
            VideoListWidget *videoList = qobject_cast<VideoListWidget*>(widget);
            if (videoList)
            {
                VideoSortFilterProxyModel *model =
                    wrapper.getModel(VideoCollectionWrapper::EAllVideos);
                if(model)
                {
                    // open and sort model
                    model->open(VideoCollectionCommon::ELevelVideos);
                    VideoCollectionViewUtils::sortModel(model, true);
                    
                    // init widget
                    VideoServices *videoServices = 0;
                    if (mIsService)
                    {
                        videoServices = VideoServices::instance();
                    }
                    videoList->initialize(*model, videoServices);
                }
            }
        }
        else if (name.compare(DOCML_NAME_VC_COLLECTIONWIDGET) == 0)
        {
            VideoSortFilterProxyModel *model = wrapper.getModel(
                VideoCollectionWrapper::ECollections);
            if (model)
            {
                model->open(VideoCollectionCommon::ELevelCategory);

                // initialize video collection widget
                VideoListWidget *videoList =
                    qobject_cast<VideoListWidget*>(widget);
                if (videoList)
                {
                    // init widget
                    VideoServices *videoServices = 0;
                    if (mIsService)
                    {
                        videoServices = VideoServices::instance();
                    }
                    videoList->initialize(*model, videoServices);
                }
            }
        }
        else if (name.compare(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET) == 0)
        {
            VideoSortFilterProxyModel *model = wrapper.getModel(
                VideoCollectionWrapper::ECollectionContent);
            if (model)
            {
                VideoListWidget *videoList = qobject_cast<VideoListWidget*>(widget);
                if (videoList)
                {
                    // init widget
                    VideoServices *videoServices = 0;
                    if (mIsService)
                    {
                        videoServices = VideoServices::instance();
                    }
                    videoList->initialize(*model, videoServices);
                }
            }
        }
        else if (name.compare(DOCML_NAME_DIALOG) == 0)
        {
            // by default, initialize the selection dialog to delete mode
            VideoListSelectionDialog *dialog =
                qobject_cast<VideoListSelectionDialog*>(widget);
            if (dialog)
            {
                dialog->setupContent(VideoListSelectionDialog::EDeleteVideos,
                    TMPXItemId::InvalidId());
            }
        }
        else if (name.compare(DOCML_NAME_SORT_MENU) == 0)
        {
            HbMenu *menu = qobject_cast<HbMenu*>(widget);
            if (menu)
            {
                // create sort by menu action
                mMenuActions[EActionSortBy] = menu->menuAction();

                // ensure that all the actions related to sort menu are loaded
                // when sort menu is loaded
                findObject<HbAction>(DOCML_NAME_SORT_BY_DATE);
                findObject<HbAction>(DOCML_NAME_SORT_BY_NAME);
                findObject<HbAction>(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
                findObject<HbAction>(DOCML_NAME_SORT_BY_TOTAL_LENGTH);
                findObject<HbAction>(DOCML_NAME_SORT_BY_SIZE);

                // add sub menu actions
                if (!mSortGroup)
                {
                    mSortGroup = new QActionGroup(menu);
                    mSortGroup->addAction(mMenuActions[EActionSortByDate]);
                    mSortGroup->addAction(mMenuActions[EActionSortByName]);
                    mSortGroup->addAction(mMenuActions[EACtionSortByItemCount]);
                    mSortGroup->addAction(mMenuActions[EActionSortByLength]);
                    mSortGroup->addAction(mMenuActions[EActionSortBySize]);
                    
                    // set all sub menu items checkable
                    foreach (QAction *action, menu->actions()) 
                    {
                        action->setCheckable(true);
                    }
                }
            }
        }
        else if (name.compare(DOCML_NAME_VC_VIDEOHINTWIDGET) == 0)
        {
            VideoHintWidget *hintWidget = qobject_cast<VideoHintWidget*>(widget);
            if (hintWidget)
            {
                hintWidget->initialize();
            }
        }
        else if (name.compare(DOCML_NAME_OPTIONS_MENU) == 0)
        {
            // ensure that all the actions related to options menu are loaded
            // when options menu is loaded
            findObject<HbAction>(DOCML_NAME_ADD_TO_COLLECTION);
            findObject<HbAction>(DOCML_NAME_CREATE_COLLECTION);
            findObject<HbAction>(DOCML_NAME_DELETE_MULTIPLE);
        }
    }
}

// ---------------------------------------------------------------------------
// initObject
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::initObject(QObject *object,
    const QString &name)
{
    if (object)
    {
        if (name.compare(DOCML_NAME_ADD_TO_COLLECTION) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionAddToCollection] = action;
            }
        }
        else if (name.compare(DOCML_NAME_CREATE_COLLECTION) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionNewCollection] = action;
            }
        }
        else if (name.compare(DOCML_NAME_DELETE_MULTIPLE) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionDelete] = action;
            }
        }
        else if (name.compare(DOCML_NAME_SORT_BY_DATE) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionSortByDate] = action;
            }
        }
        else if (name.compare(DOCML_NAME_SORT_BY_NAME) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionSortByName] = action;
            }
        }
        else if (name.compare(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EACtionSortByItemCount] = action;
            }
        }
        else if (name.compare(DOCML_NAME_SORT_BY_TOTAL_LENGTH) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionSortByLength] = action;
            }
        }
        else if (name.compare(DOCML_NAME_SORT_BY_SIZE) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionSortBySize] = action;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// timerEvent
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::timerEvent(QTimerEvent *event)
{
    if (event)
    {
        if (event->timerId() == mTimerId)
        {
            // get current request from queue
            const Params &params = mQueue.at(0);
            
            // load the widget / object
            if (params.mIsWidget)
            {
                // correct timer id, emit signal for receiver
                bool ok = connect(
                    this, SIGNAL(widgetReady(QGraphicsWidget*, const QString&)),
                    params.mReceiver, params.mMember);
                if (ok)
                {
                    QGraphicsWidget *widget =
                        HbDocumentLoader::findWidget(params.mName);
                    if (!widget)
                    {
                        // widget not found, try to load the docml
                        load(params.mDocml, &ok);
                        if (ok)
                        {
                            widget = HbDocumentLoader::findWidget(params.mName);
                        }
                    }
                    if (widget)
                    {
                        // widget found, add it to the hash
                        if (!mWidgets.contains(params.mName))
                        {
                            // initialize widget
                            initWidget(widget, params.mName);

                            // insert widget in hash table
                            mWidgets.insert(params.mName, widget);
                        }
                        emit widgetReady(widget, params.mName);
                    }
                }
                
                // disconnect
                disconnect(
                    this, SIGNAL(widgetReady(QGraphicsWidget*, const QString&)),
                    params.mReceiver, params.mMember);
            }
            else
            {
                // correct timer id, emit signal for receiver
                bool ok = connect(
                    this, SIGNAL(objectReady(QObject*, const QString&)),
                    params.mReceiver, params.mMember);
                if (ok)
                {
                    QObject *object =
                        HbDocumentLoader::findObject(params.mName);
                    if (!object)
                    {
                        // widget not found, try to load the docml
                        load(params.mDocml, &ok);
                        if (ok)
                        {
                            object = HbDocumentLoader::findObject(params.mName);
                        }
                    }
                    if (object)
                    {
                        // object found, add it to the hash
                        if (!mObjects.contains(params.mName))
                        {
                            // initialize object
                            initObject(object, params.mName);

                            // add object in hash table
                            mObjects.insert(params.mName, object);
                        }
                        emit objectReady(object, params.mName);
                    }
                }
                
                // disconnect
                disconnect(
                    this, SIGNAL(objectReady(QObject*, const QString&)),
                    params.mReceiver, params.mMember);
            }
        }
        
        // remove the request from the queue and run next request if any
        mQueue.removeAt(0);
        runNext();
    }
}

// ---------------------------------------------------------------------------
// createObject
// ---------------------------------------------------------------------------
//
QObject* VideoCollectionUiLoader::createObject( const QString& type, const QString &name )
{
    QObject* object = 0;

    if ( type == VideoListView::staticMetaObject.className() )
    {
        object = new VideoListView(this);
    }
    else if ( type == VideoListSelectionDialog::staticMetaObject.className() )
    {
        object = new VideoListSelectionDialog(this);
    }
    else if ( type == VideoListWidget::staticMetaObject.className() )
    {
        object = new VideoListWidget(this);
    }
    else if ( type == VideoHintWidget::staticMetaObject.className() )
    {
        object = new VideoHintWidget(this);
    }
    if ( object )
    {
        object->setObjectName( name );
        return object;
    }

    return HbDocumentLoader::createObject( type, name );
}

// ---------------------------------------------------------------------------
// runNext
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::runNext()
{
    if (mQueue.count())
    {
        if (!mTimerId)
        {
            // timer not running, start it
            mTimerId = startTimer(ASYNC_FIND_TIMEOUT);
        }
    }
    else
    {
        // no new requests, kill timer
        if (mTimerId)
        {
            killTimer(mTimerId);
            mTimerId = 0;
        }
    }
}

// ---------------------------------------------------------------------------
// isValid
// ---------------------------------------------------------------------------
//
bool VideoCollectionUiLoader::isValid(const Params &params)
{
    bool valid = true;
    
    if (params.mName.length() &&
        params.mDocml &&
        params.mMember &&
        params.mReceiver)
    {
        // check if the param is already in the queue
        int count = mQueue.count();
        for (int i = 0; i < count; i++)
        {
            if (mQueue.at(i).isDuplicate(params))
            {
                valid = false;
                break;
            }
        }
        
        // check that the item has not already been loaded
        if (valid)
        {
            if (params.mIsWidget)
            {
                valid = !mWidgets.contains(params.mName);
            }
            else
            {
                valid = !mObjects.contains(params.mName);
            }
        }
    }
    else
    {
        valid = false;
    }
    
    return valid;
}
