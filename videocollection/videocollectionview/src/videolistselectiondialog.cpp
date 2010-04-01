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
#include <hbabstractitemview.h>
#include <hbinputdialog.h>
#include <vcxmyvideosdefs.h>

#include "videocollectionuiloader.h"
#include "videolistwidget.h"
#include "videosortfilterproxymodel.h"
#include "videolistselectiondialog.h"
#include "videocollectionwrapper.h"
#include "videocollectionviewutils.h"
#include "videocollectionclient.h"

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// ---------------------------------------------------------------------------
// VideoListSelectionDialog
// ---------------------------------------------------------------------------
//
VideoListSelectionDialog::VideoListSelectionDialog(VideoCollectionUiLoader *uiLoader,
                                                   QGraphicsItem *parent) : 
HbDialog(parent),
mUiLoader(uiLoader),
mTypeOfSelection(-1),
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

    // create model for list widget
    mModel =
        VideoCollectionWrapper::instance().getModel(
            VideoCollectionWrapper::EGeneric);
    if (mModel)
    {
        // create list widget
        mListWidget = new VideoListWidget(mUiLoader);
        if (mListWidget)
        {
            mListWidget->initialize(*mModel);
        }
    }
}

// ---------------------------------------------------------------------------
// ~VideoListSelectionDialog
// ---------------------------------------------------------------------------
//
VideoListSelectionDialog::~VideoListSelectionDialog() 
{
    delete mListWidget;
}

// ---------------------------------------------------------------------------
// setupContent
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::setupContent(int type, TMPXItemId activeItem)
{
    if(type < EDeleteVideos || type > ESelectCollection)
    {
        return;
    }
   
    mTypeOfSelection = type; 
    if(!initDialog())
    {
        return;
    }
    mSelection.clear();
    mSelectedVideos.clear();
    mSelectedAlbumId = TMPXItemId::InvalidId();
    
    // if provided "owner" is album or category    
    if(activeItem != TMPXItemId::InvalidId())
    {
        // if album, set as "selected"
        if(activeItem.iId2 == KVcxMvcMediaTypeAlbum)
        {
            mSelectedAlbumId = activeItem;
        }
        else if(activeItem.iId2 == KVcxMvcMediaTypeVideo)
        {
            mSelectedVideos.insert(activeItem);
        }
    }
    // set (or reset) generic id filter
    mModel->setGenericIdFilter(activeItem, (mTypeOfSelection == EDeleteVideos));    
    activateSelection();
}

// ---------------------------------------------------------------------------
// initDialog
// ---------------------------------------------------------------------------
//
bool VideoListSelectionDialog::initDialog()
{
    if(!mModel)
    {
        mModel = VideoCollectionWrapper::instance().getModel(VideoCollectionWrapper::EGeneric);
        if (!mModel)
        {
           return false;
        }
    }    
    if(!mListWidget)
    {
        mListWidget = new VideoListWidget(mUiLoader);
        mListWidget->initialize(*mModel);
    }
    if (!mListContainer)
    {
        mListContainer =
                mUiLoader->findWidget<HbStackedWidget>(DOCML_NAME_LIST_CONTAINER);
        if(mListContainer && mListWidget)
        {
            mListContainer->addWidget(mListWidget);
        }
    }
    if(!mHeading)
    {
        mHeading = mUiLoader->findWidget<HbLabel>(DOCML_NAME_DLG_HEADINGLBL);
    }
    if(!mCheckboxContainer)
    {
        mCheckboxContainer = mUiLoader->findWidget<HbWidget>(DOCML_NAME_CHECK_CONTAINER);
    }

    if(!mItemCount)
    {
        mItemCount = mUiLoader->findWidget<HbLabel>(DOCML_NAME_LBL_SELECTION);
        mItemCount->setAlignment(Qt::AlignRight);
    }        
    if(!mCheckBox)
    {
        mCheckBox = mUiLoader->findWidget<HbCheckBox >(DOCML_NAME_MARKALL);
    }
    if(!primaryAction())
    {
        setPrimaryAction(new HbAction(tr("OK"), this)); //TODO: localisation
    }
    if(!secondaryAction())
    {
        setSecondaryAction(new HbAction(tr("Cancel"), this)); //TODO: localisation
    }
    if(!mListWidget || !mListContainer || !mHeading || !mCheckBox || !primaryAction() || !secondaryAction())
    {
        return false;
    }
    return true;  
}

// ---------------------------------------------------------------------------
// activateSelection
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::activateSelection()
{
    // "add to collection" and "select collection" -selections needs 
    // additional functionality for primary key
    if(mTypeOfSelection == EAddToCollection ||
       mTypeOfSelection == ESelectCollection)
    {
        primaryAction()->disconnect(SIGNAL(triggered()));
        connect(primaryAction(), SIGNAL(triggered()), this, SLOT(primaryActionTriggeredSlot()));
    }
    
    if(mTypeOfSelection == EDeleteVideos || mTypeOfSelection == EAddToCollection) 
    {
        mSelection.clear();
        mListWidget->setSelectionMode(HbAbstractItemView::MultiSelection);   
        mListWidget->activate(VideoCollectionCommon::ELevelVideos);
        mCheckboxContainer->setVisible(true);
        mItemCount->setPlainText(tr("0/%1").arg(mModel->rowCount())); 
        mCheckBox->setChecked(false);        
        QString txt("");
        mTypeOfSelection == EDeleteVideos ? txt = tr("Delete items") : txt = tr("Add to collection"); // localisation missing
        mHeading->setPlainText(txt);
        mTypeOfSelection == EDeleteVideos ? txt = hbTrId("txt_common_menu_delete") : txt = tr("Add"); // localisation missing
        primaryAction()->setText(txt);
        primaryAction()->setDisabled(true);
    }
    else
    {
        mListWidget->setSelectionMode(-1);   
        mListWidget->activate(VideoCollectionCommon::ELevelCategory);
        mCheckboxContainer->setVisible(false);
        mHeading->setPlainText(tr("Select collection")); // localisation missing
        primaryAction()->setText(tr("New")); // localisation missing    
        primaryAction()->setDisabled(false);
    }    
    // sort to make sure dialog has correctly filtered content
    // at the same order as in view
    VideoCollectionViewUtils::sortModel(mModel, false);        
}

// ---------------------------------------------------------------------------
// exec
// ---------------------------------------------------------------------------
//
HbAction* VideoListSelectionDialog::exec()
{
    // clear checkbox
    mCheckBox->setChecked(false);
    
    mListWidget->clearSelection(); 
    
    // scroll list back to top
    mListWidget->scrollTo(mModel->index(0, 0));

    connectSignals();
    
    // Launch popup syncronously
    bool accepted = false;
    HbDialog::exec() == primaryAction() ? accepted = true : accepted = false;
    
    disconnectSignals();
   
    if(accepted)
    {
        // user is adding videos into selected collection 
        // or selecting collection where to add videos
        if(mTypeOfSelection == EAddToCollection ||
           mTypeOfSelection == ESelectCollection)
        {            
            if(mSelectedAlbumId != TMPXItemId::InvalidId() && mSelectedVideos.count())
            {
                if(mModel->addItemsInAlbum(mSelectedAlbumId, mSelectedVideos.toList()) == 0)
                {
                    QVariant data = getSelectedName();
                    VideoCollectionViewUtils::instance().showStatusMsgSlot(
                            VideoCollectionCommon::statusVideosAddedToCollection,
                            data);
                }
            }
        }
        // user is deleting videos
        else if(mTypeOfSelection == EDeleteVideos)
        {
            QVariant data = mSelection.indexes().count();
            VideoCollectionViewUtils::instance().showStatusMsgSlot(
                                                   VideoCollectionCommon::statusDeleteInProgress,
                                                   data);
            // delete items            
            mModel->deleteItems(mSelection.indexes());

        }
        return primaryAction();
    } 
    return secondaryAction(); 
}

// ---------------------------------------------------------------------------
// markAllStateChanged
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::markAllStateChangedSlot(int state)
{ 
    if(mForcedCheck)
    {
        return;
    }
    if( state == Qt::Checked)
    {
        mListWidget->selectAll();
    }
    else
    {
        mListWidget->clearSelection();
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
// singleItemSelectedSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::singleItemSelectedSlot(const QModelIndex &index)
{
    if(mTypeOfSelection == ESelectCollection)
    {
       if(index.isValid())
       {           
           mSelectedAlbumId = mModel->getMediaIdAtIndex(index);            
           primaryAction()->trigger();
       }
    }
}

// ---------------------------------------------------------------------------
// modelReadySlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::modelReadySlot()
{
    if(!mModel)
    {
        return;
    }
    
    if(mTypeOfSelection == ESelectCollection && ! mModel->rowCount())
    {
        // in case there are no albums, start input dialog right away
        mSelectedAlbumId = queryNewAlbum();
        if(mSelectedAlbumId != TMPXItemId::InvalidId())
        {
            primaryAction()->trigger();
        }
        else
        {
            secondaryAction()->trigger();
        }
        return;
    }
    updateCounterSlot();
}

// ---------------------------------------------------------------------------
// updateCounterSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::updateCounterSlot()
{
    if(!mItemCount || !mModel)
    {
        return;
    }
    // orbit does not update HbLabel read from docml in case it isn't cleared first
    int rowCount = mModel->rowCount();
    
   
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
// primaryActionTriggeredSlot
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::primaryActionTriggeredSlot()
{
    // reconnect primary action
    primaryAction()->disconnect(SIGNAL(triggered()));
    connect(primaryAction(), SIGNAL(triggered()), this, SLOT(close()));
    
    if(mTypeOfSelection == EDeleteVideos) 
    {       
        primaryAction()->trigger();
		return;
    }
    
    // update video items selection here before content changes.
    int count = mSelection.indexes().count();
    TMPXItemId id = TMPXItemId::InvalidId();
    for(int i = 0; i < count; ++i)
    {
        id = mModel->getMediaIdAtIndex(mSelection.indexes().at(i));
        if(id.iId2 == KVcxMvcMediaTypeVideo)
        {
            mSelectedVideos.insert(id);
        }
    }
    
    if(mSelectedAlbumId != TMPXItemId::InvalidId())
    {
        primaryAction()->trigger();
        return;
    }
    
    
    else if(mTypeOfSelection == ESelectCollection)
    {
        // there's no selected collection and primary action pressed
        // -> user is willing to create a new collection
        mSelectedAlbumId = queryNewAlbum();
        if(mSelectedAlbumId != TMPXItemId::InvalidId())
        {       
            primaryAction()->trigger();
        }
        else
        {
            // cancelled
            secondaryAction()->trigger();
        }     
    }
    else if(mTypeOfSelection == EAddToCollection)
    {
        // videos for collection selected, but collection 
        // not yet selected, activate selection for it
        mTypeOfSelection = ESelectCollection;
        activateSelection();
   }   
}

// ---------------------------------------------------------------------------
// connectSignals
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::connectSignals()
{
    // selection changes
    connect(mListWidget, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(singleItemSelectedSlot(const QModelIndex&)));
    
    connect(mListWidget->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
            this, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection &)));
        
    // model changes signals
    connect(mModel, SIGNAL(modelReady()), this, SLOT(modelReadySlot()));    
    connect(mModel, SIGNAL(modelChanged()), this, SLOT(updateCounterSlot()));    
    
    // mark all state changes
    connect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(markAllStateChangedSlot(int)));
}
 
// ---------------------------------------------------------------------------
// disconnectSignals
// ---------------------------------------------------------------------------
//
void VideoListSelectionDialog::disconnectSignals()
{    
    disconnect(mListWidget, SIGNAL(activated(const QModelIndex&)),
                this, SLOT(singleItemSelectedSlot(const QModelIndex&)));
    
    disconnect(mListWidget->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
                            this, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection &)));
            
    // model changes signals
    disconnect(mModel, SIGNAL(modelReady()), this, SLOT(modelReadySlot()));    
    disconnect(mModel, SIGNAL(modelChanged()), this, SLOT(updateCounterSlot()));   
    
    // mark all state changes
    disconnect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(markAllStateChangedSlot(int)));   
}

// ---------------------------------------------------------------------------
// getSelectedName
// ---------------------------------------------------------------------------
//
QString VideoListSelectionDialog::getSelectedName()
{
    QString name;
    if(mSelectedAlbumId.iId2 > KVcxMvcMediaTypeVideo)
    {
        VideoSortFilterProxyModel *model = 
                VideoCollectionWrapper::instance().getModel(
                            VideoCollectionWrapper::ECollections);
        if(!model)
        {
            return name;
        }
        QModelIndex index = model->indexOfId(mSelectedAlbumId);
        if(index.isValid())
        {
            name = model->data(index, Qt::DisplayRole).toStringList().first();
        }
    }
    
    return name;
}

// ---------------------------------------------------------------------------
// queryNewAlbum
// ---------------------------------------------------------------------------
//
TMPXItemId VideoListSelectionDialog::queryNewAlbum()
{
    TMPXItemId newId = TMPXItemId::InvalidId();
    bool ok = false;
    QString label(tr("Enter name:"));   // localisation missing
    QString text(tr("New collection")); // localisation missing
    text = HbInputDialog::getText(label, text, &ok);
    if (ok && text.length())
    {
       // check for duplicate album name and add new album
        newId = mModel->addNewAlbum(mModel->resolveAlbumName(text));
    }
    return newId;
}

