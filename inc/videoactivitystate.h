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
* Description:   VideoActivityState class definition
*
*/

// Version : %version:  1 %

#ifndef __VIDEOACTIVITYSTATE_H__
#define __VIDEOACTIVITYSTATE_H__

#include <qhash.h>
#include <qvariant.h>
#include "videoplayerappexport.h"

// videoplayer activity id
static const QString ACTIVITY_VIDEOPLAYER_MAINVIEW = "VideosMainView";


namespace VideoActivityData
{        
    // consts to be used as key values    
    // plugin type (int): collectionplugin or videoplaybackplugin 
    static const QString KEY_VIEWPLUGIN_TYPE = "_VideoActivity_viewplugin_type_";
    
    // type of collectionview plugin's widget level(int): 
    // all videos, collections or collection video list
    static const QString KEY_WIDGET_LEVEL    = "_VideoActivity_widget_level_";
    
    // id of the collection whose videolist is to be shown (int).
    static const QString KEY_WIDGET_ID       = "_VideoActivity_view_item_id_";

    // for playback plugin: view to activate either play or details
    static const QString KEY_PLAYB_VIEW      = "_VideoActivity_playb_view_type_";
          
    // for playback plugin: video path
    static const QString KEY_VIDEO_PATH      = "_VideoActivity_video_path_";
    
    // for playback plugin: video playback position
    static const QString KEY_VIDEO_POS       = "_VideoActivity_video_pos_";
    
    // for playback plugin: video playback state
    static const QString KEY_PLAYB_STATE     = "_VideoActivity_playb_state_";
    
    // for playback plugin: video playback volume
    static const QString KEY_PLAYB_VOLUME    = "_VideoActivity_playb_volume_";
            
}

/**
 * Class is used as a static activity data container for videoplayer application
 * for other videoplayer components to use.
 * 
 * Data is excepted to be saved as QHash<QString, QVariant>. Keys defined above  
 * should be used to access data
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
