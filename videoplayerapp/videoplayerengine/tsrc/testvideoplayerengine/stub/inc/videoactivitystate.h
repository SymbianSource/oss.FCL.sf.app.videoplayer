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
* Description:   VideoActivityState stub class definition
*
*/

// Version : %version:  da1mmcf#2 %

#ifndef __VIDEOACTIVITYSTATE_H__
#define __VIDEOACTIVITYSTATE_H__

#include <qhash.h>
#include <qvariant.h>
#include "videoplayerappexport.h"

// videoplayer activity id
static const QString ACTIVITY_VIDEOPLAYER_MAINVIEW = "VideosMainView";

// last position of the last played media clip (int)
static const QString KEY_LAST_PLAY_POSITION_ID   = "_VideoActivity_last_play_position_id_";

// name of the media clip last played (QString)
static const QString KEY_LAST_PLAYED_CLIP = "_VideoActivity_last_played_clip_";

// Key for plugin type in activity manager
static const QString KEY_VIEWPLUGIN_TYPE = "_VideoActivity_viewplugin_type_";

// was the last played media clip a local clip (not streaming or prog. download)
static const QString KEY_LAST_LOCAL_PLAYBACK   = "_VideoActivity_last_local_playback_";

/**
 * Class is used as a stub for testing activity saving from videoplayerengine
 *
 */
class VIDEOPLAYERAPP_DLL_EXPORT VideoActivityState 
{
    
public:
 
    /**
     * method returns a static instance of this class
     */
    static VideoActivityState &instance();
    
    /**
     * Method clears all activity data.
     */
    void clearActivityData();
    
    /**
     * Sets activity data. If dataKey is not defined, all activity data
     * will be overwritten by a provided data.
     * 
     * @param data Data to save
     * @param dataKey Key indicating dataitem. If empty, all data will be overwritten by provided data 
     */
    void setActivityData(const QVariant& data, const QString& dataKey = QString(""));

    /**
     * Get activity data. If dataKey is not defined, all activity data
     * will be returned. If item corresponding provided dataKey is not found,
     * method returns empty QVariant.
     * 
     * @param dataKey Key indicating dataitem client wants. If empty, all data will be returned
     * 
     * @return QVariant 
     */
    const QVariant getActivityData(const QString& dataKey = QString(""));
    
    static int mAllDataGetCount;

private:
    
    /**
     * private default contructor
     */
    VideoActivityState();  
    
    /**
     * copy contructor definition
     */
    VideoActivityState(const VideoActivityState& other);  
    
    /**
     * Private destructor.
     *
     */
    virtual ~VideoActivityState();    

private:
    
    /**
     * locally saved activity data
     */
    QHash<QString, QVariant> mActivityData;
        
};

#endif //__VIDEOACTIVITYSTATE_H__
