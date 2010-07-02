/**
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
* Description:   tester for methods in VideoSortFilterProxyModel
* 
*/

#include <hbapplication.h>
#include <hbinstance.h>
#include <xqpluginloader.h>
#include <mpxviewpluginqt.h>
#include <xqplugininfo.h>


#include "testvideoplaybackviewplugin.h"
#include "hbvideoplaybackview.h"

#include "mpxhbvideoplaybackviewplugin.h"
#include "mpxhbvideocommondefs.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    
    TestVideoPlaybackViewPlugin tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoPlaybackViewPlugin.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::init()
{    
    QList<XQPluginInfo> impls;
    XQPluginLoader::listImplementations("org.nokia.mmdt.MpxViewPlugin/1.0", impls);

    XQPluginLoader pluginLoader( MpxHbVideoCommon::KMpxVideoPluginDllPlaybackUid );
    QObject* instance = pluginLoader.instance();    

    mVidPBPlugin = qobject_cast<MpxViewPlugin*>( instance )->viewPlugin();  ;
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::cleanup()
{
    delete mVidPBPlugin; 
    mVidPBPlugin = 0;
}

// ---------------------------------------------------------------------------
// Slot: test create view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testCreateView()
{
    qDebug("===> testCreateView()"); 
               
	init();	  

	QVERIFY(mVidPBPlugin != NULL );
	
	QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mView == NULL );
	
	mVidPBPlugin->createView();
	  
	QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mView != NULL );

}

// ---------------------------------------------------------------------------
// Slot: test activate view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testActivateView()
{
    QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mView != NULL );
				
    mVidPBPlugin->activateView();
		
    QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mViewActivated == true  );		
}

// ---------------------------------------------------------------------------
// Slot: test deactivate view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testDeactivateView()
{
    QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mView != NULL );
    QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mViewActivated == true  );
		
    mVidPBPlugin->deactivateView();
		
    QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mViewActivated == false  );			    
}

// ---------------------------------------------------------------------------
// Slot: test destroy view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testDestroyView()
{
    QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mView != NULL );				            
		
    mVidPBPlugin->destroyView();
		
    QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mView == NULL );
    
}

// ---------------------------------------------------------------------------
// Slot: test get view
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackViewPlugin::testGetView()
{
	QVERIFY( mVidPBPlugin->getView() != NULL );	
	QVERIFY(reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->mView == 
            reinterpret_cast<QMpxHbVideoPlaybackViewPlugin*>( mVidPBPlugin )->getView() );		    
}



// End of file
    


