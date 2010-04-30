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
* Description:   Videolist view class definition
*
*/

#ifndef VIDEOLISTVIEW_H
#define VIDEOLISTVIEW_H

#include <hbview.h>
#include <qnamespace.h>
#include <qabstractitemmodel.h>
#include <hbaction.h>

#include "videohintwidget.h"

class QGraphicsItem;
class VideoListWidget;
class QActionGroup;
class QVariant;
class VideoCollectionViewUtils;
class VideoSortFilterProxyModel;
class VideoCollectionWrapper;
class HbStackedWidget;
class VideoCollectionUiLoader;
class HbGroupBox;
class VideoListSelectionDialog;
class HbMenu;
class VideoServices;
class TMPXItemId;

/**
 * Class acts as an container for widgets that are used to display different
 * data: all videos, video collections or video services.
 *
 * Class is also responsible to mainatain correct widget active selected by the user's
 * tap from the toolbar and creating and maintaining main menu commonly used by all widgets.
 *
 */
class VideoListView : public HbView
{
    Q_OBJECT

public:

    /**
     * Contructor.
     * @param uiLoader VideoCollectionUiLoader instance for this view
     * @param parent parent of this view
     */
    VideoListView(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent = 0);

    /**
     * Destructor.
     *
     */
    ~VideoListView();

    /**
     * Creates and initializes view objects, toolbar and menu and connects signals
     *
     * @return 0 if ok, < 0 if intialisation fails
     */
    int initializeView();

    /**
     * Activates current widget to be shown, enables menu and connects
     * orientation change signals.
     *
     * @param itemId, Id of the widget which is to be activated
     * @return 0 activation ok, < 0 if activation fails
     */
    int activateView(const TMPXItemId &itemId);

    /**
     * Deactivates current widget, disables menu and disconnects
     * orientation change signals.
     *
     */
    void deactivateView();

    /**
     * Back implementation.
     * Not used at the moment
     *
     */
    void back();

signals:

    /**
     * Command signal. Should be connected to the
     * plugin interface's command -signal in order to
     * get the emitted commands transported uotside plugin.
     *
     * @param int command id
     */
    void command(int);
    
    /**
     * Signaled to do delayed loading of components not loaded initially at start up phase
     *
     */
    void doDelayeds();
    
    /**
     * Signaled when view is ready.
     */
    void viewReady();

public slots:

    /**
     * Signaled by HbInputDialog when it's closed. 
     */
    void createCollectionDialogFinished(HbAction *action);
    
private slots:

    /**
     * slot is connected to model's modelReady -signal
     */
    void modelReadySlot();

    /**
     * slot is connected to plugin's doDelayeds -signal
     *
     */
    void doDelayedsSlot();
    
    /**
     * slot is connected to service's titleReady -signal
     */
	void titleReadySlot(const QString& title);

     /**
     * Slot is connected to model slots informing changes in item count
     * Method checks the current model state and updates accrodingly
     *
     */
	void layoutChangedSlot();

    /**
     * Slot is connected into toolbar's all videos tab's
     * triggered signal.
     *
     * Activates all videos widget by calling changeWidget.
     *
     */
    void openAllVideosViewSlot();  

    /**
     * Slot is connected into toolbar's video collection tab's
     * triggered signal.
     *
     * Activates video collection widget by calling changeWidget.
     *
     */
    void openCollectionViewSlot();
    
    /**
     * Slot is connected to model's about to insert rows signal.
     * Opens the new album which was created. 
     */
    void openNewAlbumSlot(const QModelIndex &parent, int start, int end);

    /**
     * Slot is connected into toolbar's Service tab's
     * triggered signal.
     *
     * Activates Service widget by calling changeWidget.
     *
     */
    void openServicesViewSlot();

     /**
     * Slot is connected into main menus sort -items
     * Method checks sorting role based on active menu item and initiates sorting
     *
     */
    void startSorting();

     /**
     * Method checks the sorting role and starts sorting
     *
     */
    void doSorting(int role);
    
    /**
     * Slot is connected into main menus "delete items" (delete...) signal
     *
     * Calls ui utils to show multiple delete dialog for current widget
     *
     */
    void deleteItemsSlot();

    /**
     * Slot is connected into main menus "Create new collection..." signal
     * Shows a selection dialog for creating a new collection
     *
     */
    void createCollectionSlot();

    /**
     * Slot is connected into toolbar's  "Add videos" signal
     *
     */
    void addVideosToCollectionSlot();
    
    /**
     * Slot is connected into toolbar's "remove videos" signal
     */
    void removeVideosFromCollectionSlot();

    /**
     * Slot is connected into main menus aboutToShow -signal
     *
     */
    void aboutToShowMainMenuSlot();
    
    /**
     * Prepare menu when videos used through browsing service.
     */
    void prepareBrowseServiceMenu();

    /**
     * Slot is connected into hbInstance's primary window's
     * aboutToChangeOrientation -signal. This is called when
     * orientation is to be change.
     */
    void aboutToChangeOrientationSlot();

    /**
     * Slot is connected into hbInstance's primary window's
     * aboutToChangeOrientation -signal. This is called when
     * orientation is changed.
     *
     * @param orientation new orientation
     */
    void orientationChangedSlot( Qt::Orientation orientation );

    /**
     * Slot is connected into viewdollectionwrapper's asyncStatus -signal
     * If status is failed delete, refiltering is called to model before
     * error message is shown.
     *
     * @param statusCode code of error
     * @param additional additional data of status
     */
    void handleAsyncStatusSlot(int statusCode, QVariant &additional);

    /**
     * Slot is connected to videolistwidgets collectionOpened -signal
     *
     * @param collectionOpened
     * @param collection contains the name of the collection opened
     */
    void collectionOpenedSlot(bool collectionOpened,
        const QString& collection,
        const QModelIndex &index);
        
    /**
     * Slot which is called when an object has been loaded.
     */
    void objectReadySlot(QObject *object, const QString &name);
    
    // TODO: following can be removed after all implementation ready
    /**
     * Slot is connected into item signals that are not yet implemented.
     * Slot shows "Not yet implemented" note
     */
    void debugNotImplementedYet();

private:
    /**
     * Cleans all possibly created objects from this. In some cases there are no quarantees
     * that they were created correctly, and thus is better to start again from clean slate.
     */
    void cleanup();

    /**
    * Method creates 3 tabs for the view's toolbar: all videos, collections and Services.
    * Tabs' icons are loaded and theiur triggered signals are connected into corresponding slots.
    *
    * @return 0 creation ok, < 0 creation fails
    */
    int createToolbar();

    /**
     * Creates action with given parameters. createActionGroup() must be called successfully
     * before using this method.
     *
     * @param tooltip Tooltip text for the action.
     * @param icon Filepath for the icon file.
     * @param actionGroup Actiongroup for created action.
     * @param slot Slot for the triggered signal of the action.
     * @return HbAction pointer if creation ok, otherwise 0
     */
    HbAction* createAction(QString icon, QActionGroup* actionGroup, const char *slot);

    /**
     * Shows or hides the hint. Only shows the hint if model does not have any
     * items.
     * 
     * @param show Set this to false if you want to force hide the hint.
     */
    void showHint(bool show = true);
    
    /**
     * 
     */
    void setHintLevel(VideoHintWidget::HintLevel level);
    
    /**
     * Updates the sublabel text.
     */
    void updateSubLabel();
    
    /**
     * Shows or hides a menu action.
     */
    void showAction(bool show, const QString &name);
    
    /**
     * Activate to all videos view. 
     */
    int activateVideosView();
    
    /**
     * Activate to collection content view.
     */
    int activateCollectionContentView(const TMPXItemId &itemId);
    
private:

    /**
     * actions ids used in main menu and tool bar
     */
    enum TViewActionIds
    {
        ETBActionAllVideos     = 10,
        ETBActionCollections   = 11,
        ETBActionServices      = 12,
        ETBActionAddVideos     = 13,
        ETBActionRemoveVideos  = 14
    };

    /**
     * reference to video collection view utils
     */
    VideoCollectionViewUtils &mUiUtils;

    /**
     * pointer to videocollectionwrapper
     */
    VideoCollectionWrapper &mWrapper;

    /**
     * Pointer to the XML UI (DocML) loader, not owned
     */
    VideoCollectionUiLoader* mUiLoader;
    
    /**
     * Boolean for knowing when the app was started as a service.
     */
    bool mIsService;
    
    /**
     * Boolean for knowing when the model is ready.
     */
    bool mModelReady;

    /**
     * Hint level for the hint widget.
     */
    VideoHintWidget::HintLevel mHintLevel;

    /**
     * pointer to videoservices instance
     */
    VideoServices* mVideoServices;

    /**
     * Currently used list
     */
    VideoListWidget* mCurrentList;

    /**
     * Action group for the toolbar.
     */
    QActionGroup* mToolbarViewsActionGroup;

    /**
     * Action group for the toolbar.
     */
    QActionGroup* mToolbarCollectionActionGroup;

    /**
     * map containing toolbar actions
     */
    QMap<TViewActionIds, HbAction*> mToolbarActions;

    /**
     * Soring roles mapped to appropriate actions.
     */
    QMap<HbAction*, int> mSortingRoles;

    /**
     * String containing the name of the currently open collection
     */
    QString mCollectionName;
};

#endif // VIDEOLISTVIEW_H

