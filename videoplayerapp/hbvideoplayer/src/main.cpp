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

// Version : %version: 8 %


#include <QObject>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <xqserviceutil.h>

#include "videoplayerengine.h"

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);

    // has the application been launched via XQ Service Framework
    bool isService = XQServiceUtil::isService(); 
    
     if (!isService)
     {
    	 app.setApplicationName( QObject::tr("Videos") );
     }

    HbMainWindow mainWindow( 0, Hb::WindowFlagTransparent );
    mainWindow.setAttribute( Qt::WA_OpaquePaintEvent );
    
    QVideoPlayerEngine *engine = new QVideoPlayerEngine(isService);
    engine->initialize();
    mainWindow.show();
    return app.exec();
   
}
