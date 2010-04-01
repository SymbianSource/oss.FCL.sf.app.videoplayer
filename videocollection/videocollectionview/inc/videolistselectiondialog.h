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
* Description:   Videolist selection dialog 
* 
*/

#ifndef VIDEOLISTSELECTIONDIALOG_H
#define VIDEOLISTSELECTIONDIALOG_H

#include <hbdialog.h>
#include <qlist.h>
#include <qitemselectionmodel.h>

class QGraphicItem;
class QItemSelection;
class HbLabel;
class HbCheckBox;
class HbStackedWidget;
class VideoListWidget;
class VideoCollectionUiLoader;
class VideoSortFilterProxyModel;

class VideoListSelectionDialog: public HbDialog
{     
    /**
     * definition required for emitting / connecting signals 
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
    Q_DISABLE_COPY(VideoListSelectionDialog) 

    
public: 
    
    /**
     * Constructor
     *
     * @param uiLoader used to load UI components from docml
     * @param parent item's parent component
     */
    VideoListSelectionDialog(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent=0);
    
    /**
     * Destructor
     */
    ~VideoListSelectionDialog();
   
    /**
     * Method setups provided title and videolist content widget.
     * Prepares dialog for showing.
     * If either provided title is empty or widget is NULL, dialog
     * is in invalid state and cannot be shown using exec.
     *
     * @param title title of the dialog
     * @param videoList videolist widget.
     */
    void setContent(const QString &title, VideoListWidget *videoList);
    
    /**
     * Returns selection (mSelection). Selection will be empty in case
     * dialog is closed using cancell button,
     *
     * @return HbAction primary action if "OK" iis pressed
     */
    const QItemSelection& getSelection() const;
      
public slots:

    /**
     * Shows modal dialog build from the provided Videolistwidget.
     *
     * @return HbAction primary action if "OK" iis pressed
     */
    HbAction* exec();
    
private slots:
    
    /**
     * Slot connected to mCheckBox's stateChanged -signal. 
     * Selects or deselects all items if mForcedCheck -flag is false.
     * 
     * If Forcedcehck is true, it indeicates that we want to change the checked state
     * only, not slect or deselect anything. This is needed for example if after 
     * all items are marked, user manually deselects item's. In that case all is not
     * marked, so checked state needs to be changed. 
     *
     * @param state Qt::Checked everything should be set as selected.
     */
    void markAllStateChangedSlot(int state);
    
    /**
     * disables or enables popup's primary action in case selection changes
     *
     * @param selected item selection list containing selected
     * @param deselected item selection list containing  not selected (not used in this scope)
     */
    void selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected); 
    
    /**
     * Changes the counter value. Sets the checkbutton state based on selection count.
     *
     */
    void updateCounterSlot();
    
private:
    
    /**
     * connects all required signals into appropriate slots 
     * for selection mode
     */
    void connectSignals();
    
    /**
     * disconnects all signals 
     */
    void disconnectSignals();
    
private:    
   
    /**
     * docml UI loader, not owned
     */
    VideoCollectionUiLoader *mUiLoader;
        
    /**
     * content videolist, not owned
     */
    VideoListWidget *mVideoList;
    
    /**
     * Selection 
     */
    QItemSelection mSelection;
    
    /**
     * header label
     */
    HbLabel *mHeading;
    
    /**
     * counter label from docml
     */
    HbLabel *mItemCount;
    
    /**
     * mark all checkbox
     */
    HbCheckBox *mCheckBox;
    
    /**
     * videolist container in dialog
     */
    HbStackedWidget *mListContainer;
    
    /**
     * flag indicating that we've changed check-btn state 
     * explicitly and don't wanna handle selection based on that 
     */
    bool mForcedCheck;
    
};

#endif  //VIDEOLISTSELECTIONDIALOG_H
