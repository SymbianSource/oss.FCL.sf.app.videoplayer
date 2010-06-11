/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#include "videosettingsgroup.h"
#include <cpitemdatahelper.h>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoSettingsGroup::VideoSettingsGroup(CpItemDataHelper &itemDataHelper) :
    CpSettingFormItemData(HbDataFormModelItem::GroupItem,QString("VideoSettings")),
    mItemDataHelper(itemDataHelper),
    mApId(0)
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoSettingsGroup::~VideoSettingsGroup()
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::setAccessPointId(const uint& defaultAp)
{
    mApId = defaultAp;
}
