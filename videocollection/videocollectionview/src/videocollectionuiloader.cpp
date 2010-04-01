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

#include "videocollectionuiloader.h"
#include "videolistview.h"
#include "videolistselectiondialog.h"
#include "videolistwidget.h"
#include "videohintwidget.h"

// ---------------------------------------------------------------------------
// VideoCollectionUiLoader
// ---------------------------------------------------------------------------
//
VideoCollectionUiLoader::VideoCollectionUiLoader() :
    HbDocumentLoader()
{
}

// ---------------------------------------------------------------------------
// ~VideoCollectionUiLoader
// ---------------------------------------------------------------------------
//
VideoCollectionUiLoader::~VideoCollectionUiLoader()
{
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
        object = new VideoListWidget();
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
