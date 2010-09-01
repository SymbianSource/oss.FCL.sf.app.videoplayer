/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Observer interface for battery watcher.*
*/




#ifndef CIPTVBATTERYWATCHEROBSERVER_H
#define CIPTVBATTERYWATCHEROBSERVER_H

/**
 *  Observer interface for battery watcher.
 *
 *  @lib iptvscheduleddownloadplugin.dll
 */
class MBatteryWatcherObserver
    {
public:

    /**
     * This callback will be called when the battery level is low.
     */
    virtual void BatteryLow() = 0;
    };

#endif // CIPTVBATTERYWATCHEROBSERVER_H
