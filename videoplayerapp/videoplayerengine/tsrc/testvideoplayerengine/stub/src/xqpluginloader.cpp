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
* Description:   XQPluginLoader stub
*
*/

#include "stub/inc/xqpluginloader.h"
#include "stub/inc/xqplugininfo.h"
#include "testviewplugin.h"
#include "mpxvideo_debug.h"

QHash<int, MpxViewPlugin*> XQPluginLoader::mPluginList;
int XQPluginLoader::mFailToLoadPluginUid = -1;

XQPluginLoader::XQPluginLoader( int uid, QObject* parent )
{
    MPX_DEBUG(_L("XQPluginLoader::XQPluginLoader()"));  
    
    Q_UNUSED( parent );    
    mUid = uid;
}

XQPluginLoader::~XQPluginLoader()
{
    MPX_DEBUG(_L("XQPluginLoader::~XQPluginLoader()"));      
}

bool XQPluginLoader::listImplementations( const QString &interfaceName, 
                                          QList<XQPluginInfo > &impls )
{
    MPX_DEBUG(_L("XQPluginLoader::listImplementations()"));  
    return true;
}

QObject* XQPluginLoader::instance()
{
    MPX_ENTER_EXIT(_L("XQPluginLoader::instance()"));
    
    if ( mUid == mFailToLoadPluginUid )
    {
        return 0;
    }
    
    if ( ! mPluginList.contains( mUid ) )
    {
        mPluginList[mUid] = new TestViewPlugin();
    }

    return mPluginList[mUid];
}

void XQPluginLoader::cleanup()
{
    MPX_ENTER_EXIT(_L("XQPluginLoader::cleanup()"));
    
    QHash<int, MpxViewPlugin*>::const_iterator i = mPluginList.constBegin();
    while(i != mPluginList.constEnd()) 
    {
        TestViewPlugin *view = dynamic_cast<TestViewPlugin*>(i.value());
        if(view)
            view->destroyView(); 
        delete i.value();
         ++i;
    }
}

/**
 * Sets plugin load to fail.
 */
void XQPluginLoader::setPluginLoadFailure( int uid )
{
    MPX_DEBUG(_L("XQPluginLoader::setPluginLoadFailure()"));
    mFailToLoadPluginUid = uid;
}
