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

// Version : %version: 39 %

// INCLUDE FILES
#include <hbglobal.h>
#include <hblistview.h>
#include <hbscrollbar.h>
#include <hblistviewitem.h>
#include <hbdialog.h>
#include <hbmessagebox.h>
#include <hbnotificationdialog.h>
#include <centralrepository.h>

#include "videocollectioncommon.h"
#include "videocollectionviewutils.h"
#include "videosortfilterproxymodel.h"
#include "videocollectiontrace.h"

// Object names.
const char* const VIEW_UTILS_OBJECT_NAME_STATUS_MSG          = "vc:ViewUtilsStatusMessage";
const char* const VIEW_UTILS_OBJECT_NAME_MESSAGE_BOX_WARNING = "vc:ViewUtilsMessageBoxWarning";

// Cenrep constants.
const int KVideoCollectionViewCenrepUid(0x2002BC63);
const int KVideoCollectionViewCenrepServiceIconKey(0x2);
const int KVideoCollectionViewCenrepServiceIconPressedKey(0x3);
const int KVideoCollectionViewCenrepVideoSortingRoleKey(0x5);
const int KVideoCollectionViewCenrepVideoSortingOrderKey(0x6);
const int KVideoCollectionViewCenrepCollectionsSortingRoleKey(0x7);
const int KVideoCollectionViewCenrepCollectionsSortingOrderKey(0x8);

const int KAddToCollectionDataCount(2);
const int KAddToCollectionCountIndex(0);
const int KAddToCollectionNameIndex(1);

// ---------------------------------------------------------------------------
// instance
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils& VideoCollectionViewUtils::instance()
{
    FUNC_LOG;
    static VideoCollectionViewUtils _popupInstance;
    return _popupInstance;
}

// ---------------------------------------------------------------------------
// VideoCollectionViewUtils
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils::VideoCollectionViewUtils():
    mVideosSortRole(-1),
    mCollectionsSortRole(-1),
    mVideosSortOrder(Qt::AscendingOrder),
    mCollectionsSortOrder(Qt::AscendingOrder)
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// ~VideoCollectionViewUtils
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils::~VideoCollectionViewUtils()
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// saveSortingValues
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::saveSortingValues(int role, Qt::SortOrder order, VideoCollectionCommon::TCollectionLevels target)
{
	FUNC_LOG;
	INFO_3("VideoCollectionViewUtils::saveSortingValues() saving: role: %d, order: %d, target: %d", role, order, target);

    int status = -1;
    if (target != VideoCollectionCommon::ELevelInvalid)
	{
	    CRepository *cenRep = 0;
	    TRAP_IGNORE(cenRep = CRepository::NewL(TUid::Uid(KVideoCollectionViewCenrepUid)));

		int *rolePtr            = &mVideosSortRole;
		Qt::SortOrder *orderPtr = &mVideosSortOrder;

		int roleKey(KVideoCollectionViewCenrepVideoSortingRoleKey);
		int orderKey(KVideoCollectionViewCenrepVideoSortingOrderKey);

		if(target == VideoCollectionCommon::ELevelCategory)
	    {
	    	rolePtr  = &mCollectionsSortRole;
	    	orderPtr = &mCollectionsSortOrder;

	    	roleKey  = KVideoCollectionViewCenrepCollectionsSortingRoleKey;
			orderKey = KVideoCollectionViewCenrepCollectionsSortingOrderKey;
	    }

	    if(cenRep)
	    {
	    	status = cenRep->Set(roleKey, static_cast<TInt>(role));
	        if(status == KErrNone)
	        {
	            status = cenRep->Set(orderKey, static_cast<TInt>(order));
	        }
	        delete cenRep;
	    }

	    *rolePtr = role;
	    *orderPtr = order;
	}
    return status;
}

// ---------------------------------------------------------------------------
// loadSortingValues
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::loadSortingValues(int &role, Qt::SortOrder &order, VideoCollectionCommon::TCollectionLevels target)
{
	FUNC_LOG;
    int err(0);

    if ((target > VideoCollectionCommon::ELevelCategory) &&
		(mVideosSortRole != -1))
    {
    	role = mVideosSortRole;
    	order = mVideosSortOrder;
    }
    else if ((target == VideoCollectionCommon::ELevelCategory) &&
			 (mCollectionsSortRole != -1))
    {
    	role = mCollectionsSortRole;
    	order = mCollectionsSortOrder;
    }
    else if (target != VideoCollectionCommon::ELevelInvalid)
    {
    	int *rolePtr            = &mVideosSortRole;
    	Qt::SortOrder *orderPtr = &mVideosSortOrder;
    	int roleKey(KVideoCollectionViewCenrepVideoSortingRoleKey);
    	int orderKey(KVideoCollectionViewCenrepVideoSortingOrderKey);

    	if(target == VideoCollectionCommon::ELevelCategory)
        {
    		roleKey  = KVideoCollectionViewCenrepCollectionsSortingRoleKey;
    		orderKey = KVideoCollectionViewCenrepCollectionsSortingOrderKey;
        	rolePtr  = &mCollectionsSortRole;
        	orderPtr = &mCollectionsSortOrder;
        }

		CRepository *cenRep = 0;
		TRAP(err, cenRep = CRepository::NewL(TUid::Uid(KVideoCollectionViewCenrepUid)));
		if(cenRep)
		{
			int sortRole(-1);
			int sortOrder(-1);
			err = cenRep->Get(roleKey, sortRole);
			if(err == KErrNone)
			{
				err = cenRep->Get(orderKey, sortOrder);
				if(err == KErrNone)
				{
					*orderPtr = static_cast<Qt::SortOrder>(sortOrder);
					*rolePtr = sortRole;
				}
			}
			delete cenRep;
		}

		role  = *rolePtr;
		order = *orderPtr;
    }
    INFO_3("VideoCollectionViewUtils::loadSortingValues() loaded: role: %d, order: %d, target: %d", role, order, target);
    return err;
}

// ---------------------------------------------------------------------------
// getServiceIconStrings
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::getServiceIconStrings(QString& icon,
        QString& iconPressed)
{
	FUNC_LOG;
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
	FUNC_LOG;
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
	FUNC_LOG;
    if (view)
    {
        HbListViewItem *prototype = view->listItemPrototype();
        if(prototype)
        {
            //Use image layout in prototype
            prototype->setGraphicsSize(HbListViewItem::WideThumbnail);
        }
        view->setItemRecycling(true);
        view->setClampingStyle(HbScrollArea::BounceBackClamping);
        view->setScrollingStyle(HbScrollArea::PanWithFollowOn);
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
    bool async,
    VideoCollectionCommon::TCollectionLevels target)
{
	FUNC_LOG;
    if (model)
    {
        // setup sorting order for model
        int sortRole(VideoCollectionCommon::KeyDateTime);

        // default for categories
        if(target == VideoCollectionCommon::ELevelCategory)
        {
            sortRole = VideoCollectionCommon::KeyTitle;
        }

        Qt::SortOrder sortOrder(Qt::AscendingOrder);

        // return value ignored, as in case of error the sortRole and sortOrder variables
        // stay at their predefined values, and in error cases those are the sorting values
        // that are used.
        VideoCollectionViewUtils &self = VideoCollectionViewUtils::instance();
        self.loadSortingValues(sortRole, sortOrder, target);

        model->doSorting(sortRole, sortOrder, async);
    }
}

// ---------------------------------------------------------------------------
// showStatusMsgSlot
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::showStatusMsgSlot(int statusCode, QVariant &additional)
{
	FUNC_LOG;
    QString msg("");
    QString format("");
    bool error(true);
    switch(statusCode)
    {
        case VideoCollectionCommon::statusSingleDeleteFail:
            format = hbTrId("txt_videos_info_unable_to_delete_1_it_is_current");
            if(additional.isValid())
            {
                msg = format.arg(additional.toString());
            }
        break;
        case VideoCollectionCommon::statusMultipleDeleteFail:
            msg = hbTrId("txt_videos_info_unable_to_delete_some_items_which");
        break;
        case VideoCollectionCommon::statusSingleRemoveFail:
            format = hbTrId("txt_videos_info_unable_to_remove_collection_1");
            if(additional.isValid())
            {
                msg = format.arg(additional.toString());
            }
        break;
        case VideoCollectionCommon::statusMultiRemoveFail:
            msg = hbTrId("txt_videos_info_unable_to_remove_some_collections");
        break;
        case VideoCollectionCommon::statusVideosAddedToCollection:            
            // videos added to collection - status should containg both count and collection name
            if(additional.isValid() && additional.toList().count() == KAddToCollectionDataCount)
            {
                int count = additional.toList().at(KAddToCollectionCountIndex).toInt();
                QString name = additional.toList().at(KAddToCollectionNameIndex).toString();
                if(count && name.length())
                {
                    msg = hbTrId("txt_videos_dpopinfo_ln_videos_added_to_1", count).arg(name);
                }
            }
            error = false;
        break;
        case VideoCollectionCommon::statusAllVideosAlreadyInCollection:
            msg = hbTrId("txt_videos_info_all_videos_already_added_to_this_c");
        break;
        case VideoCollectionCommon::statusDeleteInProgress:
            if(additional.isValid())
            {
                msg = hbTrId("txt_videos_dpopinfo_ln_videos_are_being_deleted", additional.toInt());
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
            HbMessageBox *messageBox = new HbMessageBox(msg, HbMessageBox::MessageTypeWarning);
            messageBox->setAttribute(Qt::WA_DeleteOnClose);
            messageBox->setObjectName(VIEW_UTILS_OBJECT_NAME_MESSAGE_BOX_WARNING);
            messageBox->show();
        }
        else
        {
            HbNotificationDialog *infoNote = new HbNotificationDialog();
            infoNote->setAttribute(Qt::WA_DeleteOnClose);
            // only title can be two rows for HbNotificationDialog
            infoNote->setTitleTextWrapping(Hb::TextWordWrap);
            infoNote->setTitle(msg);
            infoNote->setObjectName(VIEW_UTILS_OBJECT_NAME_STATUS_MSG);
            infoNote->show();
        }
    }
}

// End of file
