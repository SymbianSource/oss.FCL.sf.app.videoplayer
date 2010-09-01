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
* Description: 
*
*/


#ifndef __MVIDEOSETTINGSDRIVEMONITOROBSERVER_H__
#define __MVIDEOSETTINGSDRIVEMONITOROBSERVER_H__

#include "tvideosettingsdrivemonitorevent.h"

// CLASS DECLARATION

/**
* Observer for CIptvDriveMonitor class.
*/
class MIptvDriveMonitorObserver
    {
        
    public:
        /**
        * Handles drive monitor event.
        *
        * @param aEvent Event.
        */
        virtual void HandleDriveMonitorEvent( TIptvDriveMonitorEvent& aEvent ) = 0;
    };

#endif
