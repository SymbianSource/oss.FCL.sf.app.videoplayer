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
#ifndef VIDEOSETTINGSGROUP_H
#define VIDEOSETTINGSGROUP_H

#include <cpsettingformitemdata.h>
#include <qabstractitemmodel.h>

class CpItemDataHelper;

class VideoSettingsGroup : public CpSettingFormItemData
{
    Q_OBJECT
public:
    
    /**
     * Constructor
     */
    VideoSettingsGroup(CpItemDataHelper &itemDataHelper);
    
    /**
     * Destructor
     */
    ~VideoSettingsGroup();
    
    /**
     * Setter for the default access point id.
     * 
     * @param defaultAp Access point id.
     */
    void setAccessPointId(const uint& defaultAp);

public:
    /**
     * Item data helper given by the CP framework.
     */
    CpItemDataHelper& mItemDataHelper;
    
    /**
     * Access point id set by the setAccessPointId
     */
    uint mApId;
};

#endif // VIDEOSETTINGSGROUP_H
