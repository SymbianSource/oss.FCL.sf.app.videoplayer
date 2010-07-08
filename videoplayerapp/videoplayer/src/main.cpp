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
* Description:  Implementation of main.cpp
*
*/

// Version : %version: 10 %


#include <QObject>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <xqserviceutil.h>
#include <hbactivitymanager.h>
#include <hbtranslator.h> 

#include "videoplayerengine.h"
#include "videoactivitystate.h"

int main(int argc, char *argv[])
{
    HbApplication app( argc, argv, Hb::SplashFixedVertical );

    // 
    // automatically loads & installs corresponding translation file
    //
    HbTranslator translator("videos");
    translator.loadCommon();
    
    //
    // has the application been launched via XQ Service Framework
    //
    bool isService = XQServiceUtil::isService();
    
    if ( ! isService )
    {
        app.setApplicationName( hbTrId("txt_videos_title_videos") );
        
        HbActivityManager *actManager = app.activityManager();
        // save activity data locally
        VideoActivityState::instance().setActivityData( actManager->activityData( ACTIVITY_VIDEOPLAYER_MAINVIEW ) );
        // remove from activitymanager
        actManager->removeActivity( ACTIVITY_VIDEOPLAYER_MAINVIEW );
    }

    HbMainWindow mainWindow( 0, Hb::WindowFlagTransparent );

    VideoPlayerEngine *engine = new VideoPlayerEngine( isService );
    engine->initialize();
    mainWindow.show();
    return app.exec();
}
