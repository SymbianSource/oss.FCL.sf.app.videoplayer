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
#include <mpxitemid.h>

class VideoSortFilterProxyModel;
class HbAction;
class HbMenu;
class QTimer;

class QItemSelection;
class VideoServices;

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
    int activate(VideoListWidget::TVideoListLevel role);

    /**
     * Method disables and hides current active view
     *
     */
    void deactivate();

    /**
     * Used for resolving widget's current type
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
    
protected:
    
    /**
     * Called by the fw when some item is tapped. Method check that
     * index is valid and calls open item for collectionwrapper to
     * open media object throught collection.
     * 
     * @param midelIndex, item's index
     */
    void emitActivated (const QModelIndex &modelIndex);

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

    /**
     * signal is connected to service's itemSelected -slot
     */
    void fileUri(const QString&);

protected slots:
    /**
     * Signaled by the fw during long press and indicating that popupmenu
     * concerning particular item is to be opened.
     * Saves current selected item's index to mCurrentIndex
     */
    void longPressGesture (const QPointF &point);

private slots:

	/**
     * Signaled for item share.
     *
     */
    void shareItemSlot();

    /**
     * Signaled for one item deletion.
     *
     */
    void deleteItemSlot();

    /**
     * Signaled for item rename.
     *
     */
    void renameSlot();

    /**
     * Signaled to play all items.
     *
     */
    void playAllSlot();

    /**
     * Signaled to add an item into currently open collection.
     *
     */
    void addItemSlot();

    /**
     * Signaled to add an item into a collection.
     *
     */
    void addToCollectionSlot();

    /**
     * Signaled when details will be selected from the videolist's
     * context menu. Maps mCurrentIndex to model's source index and
     * calls collection wrapper to open details
     *
     */
    void openDetailsSlot();

    /**
     * Signaled when the application has been lauched as a service and
     * playback is initialized from context menu.
     *
     */
    void playItemSlot();

    /**
     * Signaled when stepping back from collection in collection view
     *
     */
    void back();
	
	/**
	 * Signaled when view scrolling ends, initiates thumbnail fetching
	 * at index of first visible item. 
	 *
	 */
	void scrollingEndedSlot();
	
	/**
	 * Signaled when view scroll position changes, initiates timer to 
	 * fetch thumbnails at index of first visible item.
	 */
	void scrollPositionChangedSlot(const QPointF &newPosition);
	
    // TODO: following can be removed after all implementation ready
    /**
     * Slot is connected into item signals that are not yet implemented.
     * Slot shows "Not yet implemented" note
     */
    void debugNotImplementedYet();

private:

    enum TContextActionIds
    {
        EActionShare = 1,
        EActionDelete,
        EActionDetails,
        EACtionRemoveFromCollection,
        EACtionAddToCollection,
        EACtionRemoveCollection,
        EActionAddVideos,
        EActionRename,
        EActionSetThumb,
    	EActionPlay
    };

    /**
     * Method creates popup menu for list items.
     *
     */
    void createContextMenu();

    /**
     * Method sets correct popup menu for specific list items.
     *
     */
    void setContextMenu(bool isDefaultCollection);

    /**
     * Method connects signals needed by the widget
     *
     * @return int 0 if connect ok, < 0 if connect fails
     */
    int connectSignals();

    /**
     * Method disconnects signals needed by the widget
     *
     */
    void disConnectSignals();

private:

    /**
     * Provided model
     * Not own.
     */
    VideoSortFilterProxyModel *mModel;

    /**
     * pointer to videoservices instance
     */
    VideoServices* mVideoServices;

    /**
     * map containing pointers to main menu actions
     */
    QMap<TContextActionIds, HbAction*> mContextMenuActions;

    /**
     * current level indicating content currently showing: 
     * category or videos
     */
	VideoListWidget::TVideoListLevel mCurrentLevel;

	/**
     * True if signals have been connected
     */
	bool                       mSignalsConnected;
	
	/**
     * True if details plugin is ready
     */
	bool                       mDetailsReady;

    /**
     * Boolean for knowing when the app was started as a service.
     */
	bool                       mIsService;

	/**
     * Secondary softkey action object
     */
	HbAction 				   *mSecSkAction;

    /**
     * Item sensitive context menu
     */
    HbMenu                      *mContextMenu;

    /**
     * Last opened item id is saved when collection is opened from the 
	 * collections -list. Value used to indentify the type of list.
     */
    TMPXItemId                 mLastOpenItemId;
    
    /**
     * Timer used to report thumbnail fetches class index of 
     * first visible item when view is scrolling. 
     */
    QTimer                      *mScrollPositionTimer;
};

#endif // VIDEOLISTWIDGET_H
