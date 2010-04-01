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
* Description:  stub videolist selection dialog implementation
* 
*/

#include <QGraphicsItem>
#include <hblabel.h>
#include <hbcheckbox.h>
#include <hbaction.h>
#include <hbstackedwidget.h>
#include <hbdeviceprofile.h>
#include "dummydatamodel.h"
#include "videocollectionwrapper.h"
#include "videocollectionuiloader.h"
#include "videolistwidget.h"
#include "videosortfilterproxymodel.h"
#include "videolistselectiondialog.h"

bool VideoListSelectionDialog::mExecReturnPrimary = false;
    
int VideoListSelectionDialog::mSelectionCount = 0;
    
int VideoListSelectionDialog::mMultiSelectionLaunchCount = 0;
    
int VideoListSelectionDialog::mMultiSelectionItemSelectionCount = 0;


// ---------------------------------------------------------------------------
// VideoListSelectionDialog
// ---------------------------------------------------------------------------
//
VideoListSelectionDialog::VideoListSelectionDialog(VideoCollectionUiLoader *uiLoader,
                                                   QGraphicsItem *parent) :
HbDialog(parent)                                                   
{
    Q_UNUSED(uiLoader);
    mPrimary = new HbAction();
    mSecondary = new HbAction();
}

// ---------------------------------------------------------------------------
// ~VideoListSelectionDialog
// ---------------------------------------------------------------------------
//
VideoListSelectionDialog::~VideoListSelectionDialog() 
{
    delete mPrimary;
    delete mSecondary;
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
    Q_UNUSED(title);
    Q_UNUSED(videoList);
}

// ---------------------------------------------------------------------------
// primaryAction
// ---------------------------------------------------------------------------
//
HbAction* VideoListSelectionDialog::primaryAction()
{
    return mPrimary;
}
 
// ---------------------------------------------------------------------------
// secondaryAction
// ---------------------------------------------------------------------------
//
HbAction* VideoListSelectionDialog::secondaryAction()
{
    return mSecondary;
}

// ---------------------------------------------------------------------------
// exec
// ---------------------------------------------------------------------------
//
HbAction* VideoListSelectionDialog::exec()
{
    VideoListSelectionDialog::mMultiSelectionLaunchCount++;
    VideoCollectionWrapper *wrapper = VideoCollectionWrapper::instance();
    wrapper->open(0);

    if(wrapper)
    {
        for(int i=0; i<mSelectionCount; i++)
        {
            QModelIndex index = wrapper->getDataModel()->index(i, 0);
            if(index.isValid())
            {
                mSelection.append(QItemSelectionRange(index));
            }
        }
    }
    VideoListSelectionDialog::mMultiSelectionItemSelectionCount = mSelection.indexes().count();
    wrapper->decreaseReferenceCount();
    
    if(!mExecReturnPrimary)
    {
        return secondaryAction();
    }
    return primaryAction();
        
}



