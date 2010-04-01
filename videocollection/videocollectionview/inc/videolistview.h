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
#include <QtCore/QtGlobal>

class QGraphicsItem;
class VideoListWidget;
class VideoHintWidget;
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
     * @return 0 activation ok, < 0 if activation fails
     */
    int activateView();

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

private slots:

    /**
     * slot is connected to model's modelReady -signal
     */
    void modelReadySlot();

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
     * Slot is connected into toolbar's Service tab's
     * triggered signal.
     *
     * Activates Service widget by calling changeWidget.
     *
     */
    void openServicesViewSlot();

     /**
     * Slot is connected into main menus sort -items
     * Method checks sorting role based on active menu item and starts sorting
     *
     */
    void startSorting();

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
     * Slot is connected into main menus aboutToShow -signal
     *
     */
    void aboutToShowMainMenuSlot();

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
    void collectionOpenedSlot(bool collectionOpened, const QString& collection);

    /**
     * Slot is connected into toolbar's sort by tab's
     * triggered signal.
     *
     * Activates sort by popup menu.
     *
     */
    void openSortByMenuSlot();

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
    HbAction* createAction(QString tooltip, QString icon, QActionGroup* actionGroup, const char *slot);

    /**
     * Method creates collection view's main menu and actions
     *
     * @return 0 creation ok, < 0 creation fails
     */
    int createMainMenu();

    /**
     * Method initialises all videos widget used to show all videos
     * either in list or grid. Method also connects signals emitted by the view into
     * corresponding slots.
     *
     * @return 0 creation ok, < 0 creation fails
     */
    int createVideoWidget();
    
    /**
     * Method initializes the mVideoHintWidget.
     * 
     * @return 0 creation ok, < 0 creation fails.
     */
    int createHintWidget();
    
    /**
     * Shows or hides the hint. Only shows the hint if model does not have any
     * items.
     * 
     * @param show Set this to false if you want to force hide the hint.
     */
    void showHint(bool show = true);
    
    /**
     * Updates the sublabel text.
     */
    void updateSubLabel();

private:

    /**
     * actions ids used in main menu and tool bar
     */
    enum TViewActionIds
    {
        EActionSortBy = 1,
        EActionSortByDate,
        EActionSortByName,
        EACtionSortByItemCount,
        EActionSortByLength,
        EActionSortBySize,
        EActionNewCollection,
        EActionAddToCollection,
        EActionDelete,
        ETBActionAllVideos,
        ETBActionCollections,
        ETBActionServices,
        ETBActionAddVideos,
        ETBActionRemoveVideos,
        ETBActionSortVideos
    };


    /**
     * sort menu object.
     */
    HbMenu                   *mSortMenu;

    /**
     * reference to video collection view utils
     */
    VideoCollectionViewUtils &mUiUtils;

    /**
     * pointer to videocollectionwrapper
     */
    VideoCollectionWrapper *mWrapper;

    /**
     * Pointer to the XML UI (DocML) loader, not owned
     */
    VideoCollectionUiLoader* mUiLoader;
    
    /**
     * view model object.
     */
    VideoSortFilterProxyModel *mModel;

    /**
     * Boolean for knowing when the app was started as a service.
     */
    bool mIsService;

    /**
     * Boolean for knowing when the model is ready.
     */
    bool mModelReady;

    /**
     * HbGroupBox object loaded from docml
     */
    HbGroupBox* mSubLabel;

    /**
     * Options menu object loaded from docml
     */
    HbMenu* mOptionsMenu;

    /**
     * pointer to videoservices instance
     */
    VideoServices* mVideoServices;

    /**
     * Widget for showing all videos
     */
    VideoListWidget* mVideoListWidget;
    
    /**
     * Widget for showing the hint text.
     */
    VideoHintWidget* mVideoHintWidget;

    /**
     * Action group for the toolbar.
     */
    QActionGroup* mToolbarViewsActionGroup;

    /**
     * Action group for the toolbar.
     */
    QActionGroup* mToolbarCollectionActionGroup;

    /**
     * map containing pointers to main menu actions
     */
    QMap<TViewActionIds, HbAction*> mMenuActions;

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
    
    /**
     * selection dialog
     */
    VideoListSelectionDialog *mSelectionDialog;

};

#endif // VIDEOLISTVIEW_H

