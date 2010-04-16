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

#include "stub/inc/hbinstance.h"
#include <hbmainwindow.h>

bool initialised = false;
HbInstance* mInstance = 0;
QList<HbInstance *> mInstances;

HbInstance::HbInstance()
{
}

HbInstance::~HbInstance()
{
}

HbInstance* HbInstance::instance()
{
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
	return mInstance;
}

QList<HbInstance *> HbInstance::allMainWindows() const
{
	return mInstances;
}

void HbInstance::removeView( QGraphicsWidget* )
{
}

void HbInstance::addView( QGraphicsWidget* )
{
}

void HbInstance::setCurrentView( HbView*, bool animation )
{
}
		