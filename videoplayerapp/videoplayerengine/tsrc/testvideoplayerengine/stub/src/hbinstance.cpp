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
* Description:  Stub-implementation of HbInstance
*
*/

// Version : %version:  %

#include <hbmainwindow.h>

#include "stub/inc/hbinstance.h"
#include "mpxvideo_debug.h"

bool initialised = false;
HbInstance* mInstance = 0;
QList<HbInstance *> mInstances;

HbInstance::HbInstance()
{
    MPX_DEBUG(_L("HbInstance::HbInstance()"));
}

HbInstance::~HbInstance()
{
    MPX_DEBUG(_L("HbInstance::~HbInstance()"));
}

HbInstance* HbInstance::instance()
{
    MPX_ENTER_EXIT(_L("TestVideoServices::instance()"));
    
    if (!initialised)
    {
    	mInstance = new HbInstance();  
    	initialised = true;
    	mInstances.append(mInstance);
    }
    return mInstance;
}

HbInstance* HbInstance::primaryWindow() const
{
    MPX_DEBUG(_L("HbInstance::primaryWindow()"));
	return mInstance;
}

QList<HbInstance *> HbInstance::allMainWindows() const
{
    MPX_DEBUG(_L("HbInstance::allMainWindows()"));
	return mInstances;
}

void HbInstance::removeView( QGraphicsWidget* )
{
    MPX_DEBUG(_L("HbInstance::removeView()"));
}

void HbInstance::addView( QGraphicsWidget* )
{
    MPX_DEBUG(_L("HbInstance::addView()"));
}

void HbInstance::setCurrentView( HbView*, bool animation )
{
    MPX_DEBUG(_L("HbInstance::setCurrentView()"));
}
		