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
* Description:   Videolist content widget
*
*/


#ifndef VIDEOLISTWIDGET_H
#define VIDEOLISTWIDGET_H

#include <QGraphicsWidget>
#include <QPixmap>
#include <QModelIndex>
#include <qmap.h>
#include <qpoint.h>
#include <hbview.h>
#include <hblistview.h>

class VideoSortFilterProxyModel;
class HbAction;
class HbMenu;
class VideoServices;
class QItemSelection;

/**
 * Widget for displaying all videos, either using list- or grid view.
 * Widget contains 2 widgets to be used for different orientation in the stacked layout.
 *
 * Depending on the orientation and it's change, corresponding view is set active.
 *
 */
class VideoListWidget : public HbListView
{
    Q_OBJECT
    Q_DISABLE_COPY(VideoListWidget)
public:

    enum TVideoListLevel
    {
        ELevelCategory = 2,
        ELevelVideos   = 3
    };

    enum TVideoListType
    {
        EUnknown,
        EAllVideos,        // list displaying all videos
        ECollections,      // list displaying all collections
        EDefaultColItems,  // list displying videos in default collection
        EUserColItems      // list displaying videos in user defined collection
    };

    /**
     * Contructor.
     *
     * @param parent parent of this widget
     */
    VideoListWidget(HbView *parent = 0);

    /**
     * Destructor.
     *
     */
    virtual ~VideoListWidget();

    /**
     * Method creates colleciton wrapper, grid and list views,
     * layout and activates correct view based on the current orientation
     * 
     * @param model Model for this list view.
     * @return int 0 initialization ok, < 0 if fails.
     */
    int initialize(VideoSortFilterProxyModel &model, VideoServices* videoServices  = 0);

    /**
     * Method enables and displays current active view
     *
     * @return int 0 initialization ok, < 0 if fails.
     */
    int activate();

    /**
     * Method enables and displays current active view
     *
     * @return int 0 initialization ok, < 0 if fails.
     */
    int activate(VideoListWidget::TVideoListLevel level);

    /**
     * Method disables and hides current active view
     *
     */
    void deactivate();

    /**
     * Used for resolving widget's current level
     *
     * @return TVideoListType current level.
     */
    TVideoListType getType();
    
    /**
     * returns widget's model
     * 
     * @return VideoSortFilterProxyModel*
     */
    VideoSortFilterProxyModel& getModel();
    
signals:

    /**
     * Command signal.
     *
     * @param int command id
     */
    void command(int);

    /**
     * Signals collection view's state. Used to indicate
     * view incase some collection item is opened
     *
     * @param true if opened, false if closed.
     * @param optional name string
     */
    void collectionOpened(bool, const QString&);
    
public:

    VideoSortFilterProxyModel *mModel;
    
	VideoListWidget::TVideoListLevel mCurrentLevel;
	
	VideoListWidget::TVideoListType mType;
	
	static int mInitializeCount;
	
	static int mInitializeReturnValue;
	
	static int mActivateCount;
	
	static int mActivateReturnValue;
	
	static bool mActive;
	
    static int mDeactivateCount;

};

#endif // VIDEOLISTWIDGET_H
