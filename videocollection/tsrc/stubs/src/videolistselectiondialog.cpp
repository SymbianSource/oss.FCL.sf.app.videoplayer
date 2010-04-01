/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <hbaction.h>

#include "videolistselectiondialog.h"
#include "videolistselectiondialogdata.h"
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodel.h"

bool VideoListSelectionDialogData::mExecReturnPrimary = false;
int VideoListSelectionDialogData::mSelectionCount = 0;
int VideoListSelectionDialogData::mMultiSelectionLaunchCount = 0;    
int VideoListSelectionDialogData::mMultiSelectionItemSelectionCount = 0;

VideoListSelectionDialog::VideoListSelectionDialog(
    VideoCollectionUiLoader *uiLoader,
    QGraphicsItem *parent) : 
    HbDialog(parent),
    mUiLoader(uiLoader),
    mHeading(0),
    mCheckboxContainer(0),
    mItemCount(0),
    mCheckBox(0),
    mListContainer(0),
    mForcedCheck(false),
    mModel(0),
    mListWidget(0)
{
    setDismissPolicy(HbDialog::NoDismiss);
    setTimeout(HbDialog::NoTimeout);
    
    // create primary action
    HbAction *action = new HbAction();
    if (action)
    {
        setPrimaryAction(action);
    }
    
    // create secondary action
    action = new HbAction();
    if (action)
    {
        setSecondaryAction(action);
    }
}

VideoListSelectionDialog::~VideoListSelectionDialog() 
{
    VideoListSelectionDialogData::reset();
}

void VideoListSelectionDialog::setupContent(int type, TMPXItemId activeItem)
{
    Q_UNUSED(type);
    Q_UNUSED(activeItem);
    // not stubbed
}

bool VideoListSelectionDialog::initDialog()
{
    // not stubbed
    return false;
}

HbAction* VideoListSelectionDialog::exec()
{
    // fill selection
    VideoListSelectionDialogData::mMultiSelectionLaunchCount++;
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EGeneric);
    if (model)
    {
        for (int i = 0; i < VideoListSelectionDialogData::mSelectionCount; i++)
        {
            QModelIndex index = model->index(i, 0);
            if(index.isValid())
            {
                mSelection.append(QItemSelectionRange(index));
            }
        }
    }
    VideoListSelectionDialogData::mMultiSelectionItemSelectionCount = mSelection.indexes().count();
    
    // return wanted action
    HbAction *action = 0;
    if (VideoListSelectionDialogData::mExecReturnPrimary)
    {
        action = primaryAction();
    }
    else
    {
        action = secondaryAction();
    }
    
    return action;
}

void VideoListSelectionDialog::markAllStateChangedSlot(int state)
{
    Q_UNUSED(state);
    // not stubbed
}

void VideoListSelectionDialog::selectionChangedSlot(
    const QItemSelection &selected,
    const QItemSelection &deselected)
{  
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
    // not stubbed
}

void VideoListSelectionDialog::singleItemSelectedSlot(const QModelIndex &index)
{
    Q_UNUSED(index);
    // not stubbed
}

void VideoListSelectionDialog::modelReadySlot()
{
    // not stubbed
}

void VideoListSelectionDialog::updateCounterSlot()
{
    // not stubbed
}

void VideoListSelectionDialog::primaryActionTriggeredSlot()
{
    // not stubbed
}

void VideoListSelectionDialog::connectSignals()
{
    // not stubbed
}
 
void VideoListSelectionDialog::disconnectSignals()
{
    // not stubbed
}

// end of file
