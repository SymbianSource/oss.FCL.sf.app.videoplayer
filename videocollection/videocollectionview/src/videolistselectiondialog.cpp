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
* Description:   videolist selection dialog implementation
* 
*/

#include <QGraphicsItem>
#include <hblabel.h>
#include <hbcheckbox.h>
#include <hbaction.h>
#include <hbstackedwidget.h>
#include <hbdeviceprofile.h>
#include "videocollectionuiloader.h"
#include "videolistwidget.h"
#include "videosortfilterproxymodel.h"
#include "videolistselectiondialog.h"

// ---------------------------------------------------------------------------
// VideoListSelectionDialog
// ---------------------------------------------------------------------------
//
VideoListSelectionDialog::VideoListSelectionDialog(VideoCollectionUiLoader *uiLoader,
                                                   QGraphicsItem *parent) : 
HbDialog(parent),
mUiLoader(uiLoader),
mVideoList(0),
mHeading(0),
mItemCount(0),
mCheckBox(0),
mListContainer(0),
mForcedCheck(false)
{
    setDismissPolicy(HbDialog::NoDismiss);
    setTimeout(HbDialog::NoTimeout);
}

// ---------------------------------------------------------------------------
// ~VideoListSelectionDialog
// ---------------------------------------------------------------------------
//
VideoListSelectionDialog::~VideoListSelectionDialog() 
{
    // NOP
}

// ---------------------------------------------------------------------------
// getSelection
// ---------------------------------------------------------------------------
//
const QItemSelection& VideoListSelectionDialog::getSelection() const
{
    return mSelection;
}

// ---------------------------------------------------------------------------
// setContent
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::setContent(const QString &title, VideoListWidget *videoList)
{
    mSelection.clear();
    if(!videoList || title.isEmpty())
    {
        mVideoList = 0;
        return;
    }
    
    mVideoList = videoList; 
    if(!mHeading)
    {
        mHeading = mUiLoader->findWidget<HbLabel>(DOCML_NAME_DLG_HEADINGLBL);
    }
    mHeading->setPlainText(title);
    if(!mItemCount)
    {
        mItemCount = mUiLoader->findWidget<HbLabel>(DOCML_NAME_LBL_SELECTION);
        mItemCount->setAlignment(Qt::AlignRight);
    }
    mItemCount->setPlainText(tr("0/%1").arg(mVideoList->getModel().rowCount()));   
    
    if(!mCheckBox)
    {
        mCheckBox = mUiLoader->findWidget<HbCheckBox >(DOCML_NAME_MARKALL);
    }
    mCheckBox->setChecked(false);
    
    if(!primaryAction())
    {
        setPrimaryAction(new HbAction(tr("OK"), this));
    }
    primaryAction()->setDisabled(true);
    if(!secondaryAction())
    {
        setSecondaryAction(new HbAction(tr("Cancel"), this));
    }
 
}

// ---------------------------------------------------------------------------
// exec
// ---------------------------------------------------------------------------
//
HbAction* VideoListSelectionDialog::exec()
{
    mSelection.clear();
    if(!mVideoList)
    {
        return 0;
    }
   
    // setup content of the popup    
    mVideoList->setSelectionMode(HbAbstractItemView::MultiSelection);   
    
    // clear parent layout item from our widget to be able to insert it
    // to the stackedWidget child   
    QGraphicsLayoutItem *currenParentLayout = mVideoList->parentLayoutItem();
    QGraphicsItem *currentParentItem = mVideoList->parentItem();            
    mVideoList->setParentLayoutItem(0);    
    
    if(!mListContainer)
    {
        mListContainer = mUiLoader->findWidget<HbStackedWidget>(DOCML_NAME_LIST_CONTAINER);
    }
    mListContainer->addWidget(mVideoList);

    connectSignals();
    
    // Launch popup syncronously
    bool accepted = false;
    HbDialog::exec() == primaryAction() ? accepted = true : accepted = false;
    
    disconnectSignals();
 
    // remove out widget from stacked before poptup is to be destroyed to make sure 
    // our widget does not get deleted
    mListContainer->removeAt(0);
    
    // restore widget's original parent and layout.
    mVideoList->setParentItem(currentParentItem);
    mVideoList->setParentLayoutItem(currenParentLayout);

    mVideoList->setSelectionMode(HbAbstractItemView::NoSelection); 
    
    currentParentItem->setVisible(true);
    mVideoList->setVisible(true);
    
    if(!accepted)
    {
        mSelection.clear();
        return secondaryAction();
    }
    return primaryAction();
        
}

// ---------------------------------------------------------------------------
// markAllStateChanged
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::markAllStateChangedSlot(int state)
{
    if(!mVideoList)
    {
        return;
    }
    
    if(mForcedCheck)
    {
        return;
    }
    if( state == Qt::Checked)
    {
        mVideoList->selectAll();
    }
    else
    {
        mVideoList->clearSelection();
    }   
}

// ---------------------------------------------------------------------------
// selectionChangedSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected)
{  
    
    if(!primaryAction())
    {
        return;
    }
    if(selected.indexes().count() > 0)
    {
        mSelection.merge(selected, QItemSelectionModel::Select);
    }
    
    if(deselected.indexes().count() >  0)
    {
        mSelection.merge(deselected, QItemSelectionModel::Deselect);
    }
    
    if(mSelection.indexes().count() > 0)
    {
        primaryAction()->setDisabled(false);
    }
    else
    {
        primaryAction()->setDisabled(true);
    }    

    updateCounterSlot();
}

// ---------------------------------------------------------------------------
// updateCounterSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::updateCounterSlot()
{
    if(!mVideoList || !mItemCount)
    {
        return;
    }
    // orbit does not update HbLabel read from docml in case it isn't cleared first
    int rowCount = mVideoList->getModel().rowCount();
    int selectionCount = mSelection.indexes().count();
    mItemCount->setPlainText(tr("%1/%2").arg(selectionCount).arg(rowCount));
    
    mForcedCheck = true;
    if(selectionCount == rowCount && rowCount > 0)
    {     
        mCheckBox->setChecked(true);
    }
    else
    {
        mCheckBox->setChecked(false);
    }
    mForcedCheck = false;
       
}

// ---------------------------------------------------------------------------
// connectSignals
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::connectSignals()
{
    // selection changes
    connect(mVideoList->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
                        this, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection &)));
        
    // model changes signals
    connect(&(mVideoList->getModel()), SIGNAL(layoutChanged()), this, SLOT(updateCounterSlot()));    
    connect(&(mVideoList->getModel()), SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(updateCounterSlot()));
    connect(&(mVideoList->getModel()), SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(updateCounterSlot()));
    
    // mark all state changes
    connect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(markAllStateChangedSlot(int)));
 
}
 
// ---------------------------------------------------------------------------
// disconnectSignals
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::disconnectSignals()
{
    
    disconnect(mVideoList->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
                            this, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection &)));
            
    // model changes signals
    disconnect(&(mVideoList->getModel()), SIGNAL(layoutChanged()), this, SLOT(updateCounterSlot()));    
    disconnect(&(mVideoList->getModel()), SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(updateCounterSlot()));
    disconnect(&(mVideoList->getModel()), SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(updateCounterSlot()));
    
    // mark all state changes
    disconnect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(markAllStateChangedSlot(int)));
    
}


