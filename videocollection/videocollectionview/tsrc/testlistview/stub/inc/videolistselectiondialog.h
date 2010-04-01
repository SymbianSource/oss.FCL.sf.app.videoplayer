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
* Description:   Vstub ideolist selection dialog 
* 
*/

#ifndef VIDEOLISTSELECTIONDIALOG_H
#define VIDEOLISTSELECTIONDIALOG_H

#include <hbdialog.h>
#include <qlist.h>
#include <qitemselectionmodel.h>

class QGraphicItem;
class VideoListWidget;
class VideoCollectionUiLoader;


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
      
    /**
     * Shows modal dialog build from the provided Videolistwidget.
     *
     * @return HbAction primary action if "OK" iis pressed
     */
    HbAction* exec();
    
    /**
     * returs primary action
     */
    HbAction* primaryAction();
        
    /**
     * returns secondary action
     */
    HbAction* secondaryAction();
    
    /**
     * selection
     */
    QItemSelection mSelection;
    
    /**
     * if true, exec returns primary action
     */
    static bool mExecReturnPrimary;
    
    /**
     * if selection count to return from getSelection
     */
    static int mSelectionCount;
    
    /**
     * exec call count
     */
    static int mMultiSelectionLaunchCount;
    
    /**
     * 
     */
    static int mMultiSelectionItemSelectionCount;
private:
    
    /**
     * primary action
     */
    HbAction *mPrimary;
    
    /**
     * secondary action
     */
    HbAction *mSecondary;
};

#endif  //VIDEOLISTSELECTIONDIALOG_H
