/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: video collection view plugin's ui utils class
* 
*/

#ifndef __VIDEOCOLLECTIONVIEWUTILS_H__
#define __VIDEOCOLLECTIONVIEWUTILS_H__

#include <qobject.h>
#include "videocollectioncommon.h"

class HbListView;
class VideoSortFilterProxyModel;

class VideoCollectionViewUtils : public QObject
{
    Q_OBJECT
    
public:
    /**
     * Returns singleton instance for this class.
     * 
     * WARNING! Not safe to call this from destructor of another function scope static object!
     * 
     * @return The singleton instance.
     */
    static VideoCollectionViewUtils& instance();
    
    /**
     * Saves the sorting role and order to cenrep.
     * 
     * @param role The sorting role.
     * @param order The sorting order (e.g. ascending or descending).
     * @return Zero if operation succeeded, less than zero in error cases.
     */
    int saveSortingValues(int role, Qt::SortOrder order, VideoCollectionCommon::TCollectionLevels target);
    
    /**
     * Loads the sorting role and order from cenrep.
     * 
     * @param role On return contains the sorting role.
     * @param order On return contains the sorting order
     * @return Zero if operation succeeded, less than zero in error cases.
     */
    int loadSortingValues(int& role, Qt::SortOrder& order, VideoCollectionCommon::TCollectionLevels target);
    
    /**
     * Get service icon resource strings from cenrep.
     * 
     * @param icon On return contains the resource string for icon image.
     * @param iconPressed On return contains the resource string for pressed icon image.
     * @return Zero if operation succeeded, less than zero in error cases. 
     */
    int getServiceIconStrings(QString& icon, QString& iconPressed);
    
    /**
     * Get service URI string.
     * 
     * @return Service URI string. Invalid string in error cases.
     */
    QString getServiceUriString();
    
public:
    /**
     * Initializes list view for collection and selection dialog with common
     * properties.
     */
    static void initListView(HbListView *view);
    
    /**
     * Initilizes model sort values.
     */
    static void sortModel(VideoSortFilterProxyModel *model, bool async, VideoCollectionCommon::TCollectionLevels target);
    
    /**
     * Method saves the latest videolist widget level into activity manager db
     * 
     * @param level type of widget 
     */
    static void saveWidgetLevel(VideoCollectionCommon::TCollectionLevels &level);
    
    /**
     * Method loads the latest videolist widget level from activity manager db.
     * Possible values are ELevelCategory or ELevelVideos
     * 
     * @return VideoCollectionCommon::TCollectionLevels
     */
    static VideoCollectionCommon::TCollectionLevels loadWidgetLevel();

public slots:
    
    /**
     * Shows status msgs
     *
     * @param statusCode status code
     * @param additional additional data gotten from the status 
     */
    void showStatusMsgSlot(int statusCode, QVariant &additional);
   
private:
    
    /**
     * disables copy-constructor and assingment operator
     */
    Q_DISABLE_COPY(VideoCollectionViewUtils)
    
    /**
     * constructor
     */
    VideoCollectionViewUtils();
     
    /**
     * destructor
     */
    virtual ~VideoCollectionViewUtils();

private:
    /** current sorting role */
    int mVideosSortRole;
    int mCollectionsSortRole;
    
    /** current sorting order */
    Qt::SortOrder mVideosSortOrder;
    Qt::SortOrder mCollectionsSortOrder;
};

#endif //__VIDEOCOLLECTIONUIUTILS_H__

