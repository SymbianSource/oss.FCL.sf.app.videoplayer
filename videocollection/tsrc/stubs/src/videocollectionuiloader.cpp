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

#include <qdebug.h>
#include <qset.h>
#include <qstring.h>
#include <hblistview.h>
#include <hbmenu.h>

#include "videocollectionuiloader.h"
#include "videolistview.h"
#include "videolistwidget.h"
#include "videohintwidget.h"
#include "videolistselectiondialog.h"

#include "videocollectionuiloaderdata.h"

bool VideoCollectionUiLoaderData::mFindFailure = false;
bool VideoCollectionUiLoaderData::mFailDialogLoad = false;
QStringList VideoCollectionUiLoaderData::mFindFailureNameList;

VideoCollectionUiLoader::VideoCollectionUiLoader():
    HbDocumentLoader(),
    mTimerId(0)
{
    // not stubbed
}

VideoCollectionUiLoader::~VideoCollectionUiLoader()
{
    VideoCollectionUiLoaderData::reset();
}

void VideoCollectionUiLoader::startLoading(QSet<QString> uiSections,
    QObject *receiver,
    const char *widgetSlot,
    const char *objectSlot)
{
    Q_UNUSED(uiSections);
    Q_UNUSED(receiver);
    Q_UNUSED(widgetSlot);
    Q_UNUSED(objectSlot);
    // not stubbed
}

void VideoCollectionUiLoader::setIsService(bool isService)
{
    Q_UNUSED(isService);
    // not stubbed
}

QGraphicsWidget* VideoCollectionUiLoader::doFindWidget(const QString &name)
{
    QGraphicsWidget *widget = 0;
    if(VideoCollectionUiLoaderData::mFindFailure)
    {
        return 0; 
    }
    if(!VideoCollectionUiLoaderData::mFindFailureNameList.contains(name))
    {
        widget = HbDocumentLoader::findWidget(name);
    }
    return widget;
}

QObject* VideoCollectionUiLoader::doFindObject(const QString &name)
{
    QObject *object = 0;
    if(VideoCollectionUiLoaderData::mFindFailure)
    {
        return 0; 
    }
    if(!VideoCollectionUiLoaderData::mFindFailureNameList.contains(name))
    {
        object = HbDocumentLoader::findObject(name);
    }
    return object;
}

void VideoCollectionUiLoader::addToQueue(Params &params)
{
    Q_UNUSED(params);
    // not stubbed
}

void VideoCollectionUiLoader::initWidget(QGraphicsWidget *widget,
    const QString &name)
{
    Q_UNUSED(widget);
    Q_UNUSED(name);
    // not stubbed
}

void VideoCollectionUiLoader::initObject(QObject *object,
    const QString &name)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    // not stubbed
}

void VideoCollectionUiLoader::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    // not stubbed
}

QObject* VideoCollectionUiLoader::createObject(const QString& type,
    const QString &name)
{
    QObject* object = doFindObject(name);
    if (!object)
    {
        if (name == DOCML_NAME_VIEW)
        {
            object = new VideoListView(this);
        }
        else if (name == DOCML_NAME_VC_COLLECTIONWIDGET ||
                 name == DOCML_NAME_VC_COLLECTIONCONTENTWIDGET ||
                 name == DOCML_NAME_VC_VIDEOLISTWIDGET)
        {
            object = new VideoListWidget(this);
        }
        else if (name == DOCML_NAME_DIALOG)
        {
            if(!VideoCollectionUiLoaderData::mFailDialogLoad)
            {
                object = new VideoListSelectionDialog(this);
            }
            else
            {
                return 0;
            }
        }
        else if (name == DOCML_NAME_VC_VIDEOHINTWIDGET)
        {
            object = new VideoHintWidget(this);
        }
        if (object)
        {
            object->setObjectName(name);


            return object;
        }
        object = HbDocumentLoader::createObject(type, name);
    }
    
    return object;
}

void VideoCollectionUiLoader::runNext()
{
    // not stubbed
}

bool VideoCollectionUiLoader::isValid(const Params &params)
{
    Q_UNUSED(params);
    // not stubbed
    return true;
}
