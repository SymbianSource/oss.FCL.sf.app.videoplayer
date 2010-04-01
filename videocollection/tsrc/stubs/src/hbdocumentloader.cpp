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

#include "hbdocumentloader.h"
#include "hbmainwindow.h"
#include "hbmenu.h"
#include "hbview.h"
#include "hbstackedwidget.h"
#include "hblabel.h"
#include "hbwidget.h"
#include "hbcheckbox.h"
#include "hbpushbutton.h"
#include "hblabel.h"
#include "videocollectionuiloader.h"

bool HbDocumentLoader::mFindWidgetFails = false;
bool HbDocumentLoader::mFindObjectFails = false;
bool HbDocumentLoader::mCreateObjectFails = false;

HbDocumentLoader::HbDocumentLoader() : mCreatingObject(false)
{

}

HbDocumentLoader::HbDocumentLoader(const HbMainWindow *window) : mCreatingObject(false)
{
    Q_UNUSED(window);
    // Not stubbed.
}

HbDocumentLoader::~HbDocumentLoader()
{
    reset();
}

QObjectList HbDocumentLoader::load( const QString &fileName, const QString &section , bool *ok)
{
    Q_UNUSED(fileName);
    Q_UNUSED(section);

    *ok = true;
    
    QObjectList objs;
    return objs;
}

QObjectList HbDocumentLoader::load( const QString &fileName, bool *ok)
{
    Q_UNUSED(fileName);
    Q_UNUSED(ok);

    *ok = true;
    
    QObjectList objs;
    return objs;
}
    
QGraphicsWidget *HbDocumentLoader::findWidget(const QString &name)
{   
    if(mFindWidgetFails)
    {
        return 0;
    }
    
    QObject *obj = 0;
    
    for(int i = 0; i < mObjects.count(); i++)
    {
        if(mObjects[i]->mName == name)
        {
            obj = mObjects[i]->mObject;
            break;
        }
    }
    
    if(!obj && !mCreatingObject)
    {
        mCreatingObject = true;
        obj = createObject(QString(), name);
        mCreatingObject = false;
    }
    
    if(obj)
    {
        return qobject_cast<QGraphicsWidget *>(obj);
    }
    
    return 0;
}

QObject *HbDocumentLoader::findObject(const QString &name)
{
    if(mFindObjectFails)
    {
        return 0;
    }
    
    QObject *obj = 0;
    for(int i = 0; i < mObjects.count(); i++)
    {
        if(mObjects[i]->mName == name)
        {
            obj = mObjects[i]->mObject;
            break;
        }
    }

    if(!obj && !mCreatingObject)
    {
        mCreatingObject = true;
        obj = createObject(QString(), name);
        mObjects.append(new ObjectData(obj, name));
        mCreatingObject = false;
    }    
    
    return obj;
}

void HbDocumentLoader::reset()
{
    while(!mObjects.isEmpty())
    {
        ObjectData *o = mObjects.takeFirst();
        delete o;
    }
    mObjects.clear();
}

QObject *HbDocumentLoader::createObject(const QString& type, const QString &name)
{
    Q_UNUSED(type);
    
    QObject *obj = 0;
    
    if(mCreateObjectFails) 
    {
        return 0;
    }
    
    if(name == DOCML_NAME_OPTIONS_MENU ||
       name == DOCML_NAME_SORT_MENU)
    {
        obj = new HbMenu();
        mObjects.append(new ObjectData(obj, name));
    }

    if(obj)
    {
        return obj;
    }
    
    obj = doCreateObject(name);
    return obj;
}

QObject *HbDocumentLoader::doCreateObject(const QString &name)
{
    QObject *obj = 0;
    if(name == DOCML_NAME_VIEW)
    {
        
    } 
    else if(name == DOCML_NAME_VC_HEADINGBANNER)
    {
        
    }
    else if(name == DOCML_NAME_VC_COLLECTIONWIDGET)
    {
        
    }
    else if(name == DOCML_NAME_VC_COLLECTIONCONTENTWIDGET)
    {
        
    }
    else if(name == DOCML_NAME_VC_VIDEOLISTWIDGET)
    {
        
    }
    else if(name == DOCML_NAME_VC_VIDEOHINTWIDGET)
    {
        
    }
    else if(name == DOCML_NAME_OPTIONS_MENU)
    {
        
    }
    else if(name == DOCML_NAME_SORT_MENU)
    {
        
    }
    else if(name == DOCML_NAME_SORT_BY_DATE)
    {
        
    }
    else if(name == DOCML_NAME_SORT_BY_NAME)
    {
        
    }
    else if(name == DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS)
    {
        
    }
    else if(name == DOCML_NAME_SORT_BY_RATING)
    {
        
    }
    else if(name == DOCML_NAME_SORT_BY_SIZE)
    {
        
    }
    else if(name == DOCML_NAME_ADD_TO_COLLECTION)
    {
        
    }
    else if(name == DOCML_NAME_CREATE_COLLECTION)
    {
        
    }
    else if(name == DOCML_NAME_DELETE_MULTIPLE)
    {
        
    }
    else if(name == DOCML_NAME_HINT_BUTTON)
    {
        obj = new HbPushButton();
    }
    else if(name == DOCML_NAME_HINT_LABEL)
    {
        obj = new HbLabel();
    }
    else if(name == DOCML_NAME_NO_VIDEOS_LABEL)
    {
        obj = new HbLabel();
    }
    else if(name == DOCML_VIDEOSELECTIONDIALOG_FILE)
    {
        
    }
    else if(name == DOCML_NAME_DIALOG)
    {
        
    }
    else if(name == DOCML_NAME_DLG_HEADINGLBL)
    {
        obj = new HbLabel();
    }
    else if(name == DOCML_NAME_CHECK_CONTAINER)
    {
        obj = new HbWidget();
    }
    else if(name == DOCML_NAME_MARKALL)
    {
        obj = new HbCheckBox();
    }
    else if(name == DOCML_NAME_LBL_SELECTION)
    {
        obj = new HbLabel();
    }
    else if(name == DOCML_NAME_LIST_CONTAINER)
    {
        obj = new HbStackedWidget();
    }
    
    if(obj)
    {   
        mObjects.append(new ObjectData(obj, name));
    }
    return obj;
}

