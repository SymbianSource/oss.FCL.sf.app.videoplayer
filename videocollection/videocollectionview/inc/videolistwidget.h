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

#include <hbview.h>
#include <hblistview.h>
#include <qmap.h>
#include <mpxitemid.h>
#include "videocollectioncommon.h"

class VideoSortFilterProxyModel;
class VideoServices;
class VideoCollectionUiLoader;
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


    /**
     * Contructor.
     * @param uiloader
     * @param parent parent of this widget
     */
    VideoListWidget(VideoCollectionUiLoader *uiLoader, HbView *parent = 0);

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
    int activate(VideoCollectionCommon::TCollectionLevels level);

    /**
     * Method disables and hides current active view
     *
     */
    void deactivate();
    
    /**
     * returns current level provided by activate
     * 
     * @return TCollectionLevels
     */
    VideoCollectionCommon::TCollectionLevels getLevel();
    
    /**
     * returns widget's model
     * 
     * @return VideoSortFilterProxyModel*
     */
    VideoSortFilterProxyModel& getModel();
    
public:
    
    /**
     * Called by the fw when some item is tapped. Method check that
     * index is valid and calls open item for collectionwrapper to
     * open media object throught collection.
     * 
     * @param midelIndex, item's index
     */
    void emitActivated (const QModelIndex &modelIndex);
    
    /**
     * Overwritten from Abstractitemview.
     * Sets local selection mode variable value used locally in checks. 
     * If provided value is mode provided by HbAbstractitemview sets 
     * it as view selection mode othervise NoSelection is setted
     * 
     * @param mode 
	 */
    void setSelectionMode(int mode);

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
    void collectionOpened(bool, const QString&, const QModelIndex&);

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
     * Signaled to add an item into a collection.
     *
     */
    void addToCollectionSlot();
    
    /**
     * Signaled to remove a particular video from collection
     */
    void removeFromCollectionSlot();

    /**
     * Signaled to remove a user created collection.
     *
     */
    void removeCollectionSlot();
    
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
     * slot is connected to view's doDelayeds -signal
     *
     */
    void doDelayedsSlot();
    
    /**
     * Signaled when stepping back from collection in collection view
     *
     */
    void back();
	
	/**
	 * Signaled when view scrolling starts, pauses thumbnail creation.
	 *
	 */
	void scrollingStartedSlot();	
	
	/**
	 * Signaled when view scrolling ends, initiates thumbnail fetching
	 * at index of first visible item and enables creation of thumbnails.
	 *
	 */
	void scrollingEndedSlot();
	
	/**
	 * Signaled when view scroll position changes, initiates timer to 
	 * fetch thumbnails at index of first visible item.
	 *
	 * @param newPosition scroll position 
	 *
	 */
	void scrollPositionChangedSlot(const QPointF &newPosition);
	
	/**
	 * Signaled when scroll position timer triggers. Starts fetching thumbnails
	 * for visible items.
	 * 
	 */
	void scrollPositionTimerSlot();

    /**
     * Fetches thumbnails for the visible items.
     *
     */
    void fetchThumbnailsForVisibleItems();
	
    // TODO: following can be removed after all implementation ready
    /**
     * Slot is connected into item signals that are not yet implemented.
     * Slot shows "Not yet implemented" note
     */
    void debugNotImplementedYet();
    
private:

    enum TContextActionIds
    {
    	EActionDelete = 1,
        EActionDetails,
        EActionRemove,
        EActionAddToCollection,
        EActionRemoveCollection,
        EActionRename,
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
    void setContextMenu();

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
    VideoCollectionCommon::TCollectionLevels mCurrentLevel;

	/**
     * True if signals have been connected
     */
	bool                       mSignalsConnected;
	
    /**
     * Boolean for knowing when the app was started as a service.
     */
	bool                       mIsService;

	/**
     * Navigation softkey action object for back.
     */
	HbAction 				   *mNavKeyBackAction;

    /**
     * Navigation softkey action object for quit.
     */
    HbAction                   *mNavKeyQuitAction;

    /**
     * Item sensitive context menu
     */
    HbMenu                      *mContextMenu;
    
    /**
     * loacal selection mode
	 */
    int                        mSelectionMode;
    
    /**
     * Timer used to report thumbnail fetches class index of 
     * first visible item when view is scrolling. 
     */
    QTimer                      *mScrollPositionTimer;
    
    /**
     * ui loade object, not owned
     */
    VideoCollectionUiLoader     *mUiLoader;
};

#endif // VIDEOLISTWIDGET_H
