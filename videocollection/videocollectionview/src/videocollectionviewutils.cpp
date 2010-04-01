/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: video collection view plugin's ui utils class
* 
*/

#include <hblistview.h>
#include <hbscrollbar.h>
#include <hblistviewitem.h>
#include <hbdialog.h>
#include <hbmessagebox.h>
#include <centralrepository.h>

#include "videocollectioncommon.h"
#include "videocollectionviewutils.h"
#include "videosortfilterproxymodel.h"

const int KVideoCollectionViewCenrepUid(0x2002BC63);
const int KVideoCollectionViewCenrepServiceIconKey(0x2);
const int KVideoCollectionViewCenrepServiceIconPressedKey(0x3);
const int KVideoCollectionViewCenrepServiceUriKey(0x4);
const int KVideoCollectionViewCenrepSortingRoleKey(0x5);
const int KVideoCollectionViewCenrepSortingOrderKey(0x6);

// ---------------------------------------------------------------------------
// instance
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils& VideoCollectionViewUtils::instance()
{
     static VideoCollectionViewUtils _popupInstance;
     return _popupInstance;
}

// ---------------------------------------------------------------------------
// VideoCollectionViewUtils
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils::VideoCollectionViewUtils():
    mSortRole(-1),
    mSortOrder(Qt::AscendingOrder)
{
    
}

// ---------------------------------------------------------------------------
// ~VideoCollectionViewUtils
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils::~VideoCollectionViewUtils()
{

}

// ---------------------------------------------------------------------------
// saveSortingValues
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::saveSortingValues(int role, Qt::SortOrder order)
{
    int status = -1;
    CRepository *cenRep = 0;
    TRAP_IGNORE(cenRep = CRepository::NewL(TUid::Uid(KVideoCollectionViewCenrepUid)));
    if(cenRep)
    {
        status = cenRep->Set(KVideoCollectionViewCenrepSortingRoleKey, static_cast<TInt>(role));
        if(status == KErrNone)
        {
            status = cenRep->Set(KVideoCollectionViewCenrepSortingOrderKey, static_cast<TInt>(order));
        }
        delete cenRep;
    }
    
    mSortRole = role;
    mSortOrder = order;
    
    return status;    
}

// ---------------------------------------------------------------------------
// loadSortingValues
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::loadSortingValues(int &role, Qt::SortOrder &order)
{
    int err(0);
    
    if (mSortRole == -1)
    {
        CRepository *cenRep = 0;
        TRAP_IGNORE(cenRep = CRepository::NewL(TUid::Uid(KVideoCollectionViewCenrepUid)));
        if(cenRep)
        {
            int sortRole(-1);
            int sortOrder(-1);
            err = cenRep->Get(KVideoCollectionViewCenrepSortingRoleKey, sortRole);
            if(err == KErrNone)
            {
                mSortRole = sortRole;
                err = cenRep->Get(KVideoCollectionViewCenrepSortingOrderKey, sortOrder);
                if(err == KErrNone)
                {
                    mSortOrder = static_cast<Qt::SortOrder>(sortOrder);
                }
            }
            delete cenRep;
        }
    }
    
    role = mSortRole;
    order = mSortOrder;
    
    return err;        
}

// ---------------------------------------------------------------------------
// getServiceIconStrings
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::getServiceIconStrings(QString& icon, 
        QString& iconPressed)
{
    int status = -1;
    CRepository *cenRep = 0;
    TRAP_IGNORE(cenRep = CRepository::NewL(TUid::Uid(KVideoCollectionViewCenrepUid)));
    if(cenRep)
    {
        TBuf<255> iconValue;
        TBuf<255> pressedValue;
        status = cenRep->Get(KVideoCollectionViewCenrepServiceIconKey, iconValue);
        if(status == KErrNone)
        {
            status = cenRep->Get(KVideoCollectionViewCenrepServiceIconPressedKey, pressedValue);
            if(status == KErrNone)
            {
                QString iconTemp((QChar*)iconValue.Ptr(),iconValue.Length());
                QString pressedTemp((QChar*)pressedValue.Ptr(),pressedValue.Length());
                
                icon = iconTemp;
                iconPressed = pressedTemp;
            }
        }
        delete cenRep;
    }
    return status;        
}

// ---------------------------------------------------------------------------
// getServiceUriString
// ---------------------------------------------------------------------------
//
QString VideoCollectionViewUtils::getServiceUriString()
{
    QString uri;
    CRepository *cenRep = 0;
    TRAP_IGNORE(cenRep = CRepository::NewL(TUid::Uid(KVideoCollectionViewCenrepUid)));
    if(cenRep)
    {
        TBuf<255> uriValue;
        if(cenRep->Get(KVideoCollectionViewCenrepServiceIconKey, uriValue) == KErrNone)
        {
            QString uriTemp((QChar*)uriValue.Ptr(),uriValue.Length());
            uri = uriTemp;
        }
        delete cenRep;
    }
    return uri;        
}

// ---------------------------------------------------------------------------
// initListView
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::initListView(HbListView *view)
{
    if (view)
    {
        HbListViewItem *prototype = view->listItemPrototype();
        if(prototype)
        {
            //Use image layout in prototype
            prototype->setGraphicsSize(HbListViewItem::Thumbnail);
        }
        view->setItemRecycling(true);
        view->setClampingStyle(HbScrollArea::BounceBackClamping);
        view->setScrollingStyle(HbScrollArea::PanOrFlick);
        view->setFrictionEnabled(true);
        view->setUniformItemSizes(true);  
        view->setSelectionMode(HbAbstractItemView::NoSelection);
        
        //Use scrollbar
        HbScrollBar *scrollBar = view->verticalScrollBar();
        if (scrollBar)
        {
            scrollBar->setInteractive(true);
        }
    }
}

// ---------------------------------------------------------------------------
// sortModel
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::sortModel(
    VideoSortFilterProxyModel *model,
    bool async)
{
    if (model)
    {
        // setup sorting order for model
        int sortRole(VideoCollectionCommon::KeyDateTime);
        Qt::SortOrder sortOrder(Qt::AscendingOrder);
            
        // return value ignored, as in case of error the sortRole and sortOrder variables
        // stay at their predefined values, and in error cases those are the sorting values
        // that are used.
        VideoCollectionViewUtils &self = VideoCollectionViewUtils::instance();
        self.loadSortingValues(sortRole, sortOrder);
        model->doSorting(sortRole, sortOrder, async);
    }
}

// ---------------------------------------------------------------------------
// showStatusMsgSlot
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::showStatusMsgSlot(int statusCode, QVariant &additional)
{
    QString msg("");
    QString format("");
    bool error(true);
    switch(statusCode)
    {
        case VideoCollectionCommon::statusSingleDeleteFail:
            format = tr("Unable to delete item %1. It is currently open."); //TODO: localisation
            if(additional.isValid())
            {
                msg = format.arg(additional.toString());
            }
        break;
        case VideoCollectionCommon::statusMultipleDeleteFail:
            msg = tr("Unable to delete some items which are currently open.");
        break;
        case VideoCollectionCommon::statusSingleRemoveFail:
            format = tr("Unable to remove collection %1."); //TODO: localisation
            if(additional.isValid())
            {
                msg = format.arg(additional.toString());
            }
        break;
        case VideoCollectionCommon::statusMultiRemoveFail:
            msg = tr("Unable to remove some collections.");
        break;
        case VideoCollectionCommon::statusVideosAddedToCollection:
            format = tr("Videos added to %1 collection."); //TODO: localisation
            if(additional.isValid())
            {
                msg = format.arg(additional.toString());
            }
            error = false;
        break;
        case VideoCollectionCommon::statusAllVideosAlreadyInCollection:
            msg = tr("All videos already added to this collection.");            
        break;
        case VideoCollectionCommon::statusDeleteInProgress:
            format = tr("%1 videos are being deleted."); //TODO: localisation
            if(additional.isValid())
            {
                msg = format.arg(additional.toString());
            }
            error = false;
        break;
        default: // no msg to show
        return;    
    }
        
    if(msg.count() > 0)
    {
        if(error)
        {
            HbMessageBox::warning(msg);
        }
        else
        {
            HbMessageBox::information(msg);
        }
     
    }  
}

