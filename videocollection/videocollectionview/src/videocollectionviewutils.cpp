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

#include <hbdialog.h>
#include <hbmessagebox.h>
#include <centralrepository.h>

#include "videocollectioncommon.h"
#include "videocollectionviewutils.h"

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
VideoCollectionViewUtils::VideoCollectionViewUtils()
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
    return status;    
}

// ---------------------------------------------------------------------------
// loadSortingValues
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::loadSortingValues(int& role, Qt::SortOrder& order)
{
    int status = -1;
    CRepository *cenRep = 0;
    TRAP_IGNORE(cenRep = CRepository::NewL(TUid::Uid(KVideoCollectionViewCenrepUid)));
    if(cenRep)
    {
        TInt roleValue(KErrNotFound);
        TInt orderValue(KErrNotFound);
        status = cenRep->Get(KVideoCollectionViewCenrepSortingRoleKey, roleValue);
        if(status == KErrNone)
        {
            status = cenRep->Get(KVideoCollectionViewCenrepSortingOrderKey, orderValue);
            if(status == KErrNone)
            {
                role = roleValue;
                order = static_cast<Qt::SortOrder>(orderValue);
            }
        }
        delete cenRep;
    }
    return status;        
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
// showStatusMsgSlot
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::showStatusMsgSlot(int statusCode, QVariant &additional)
{
    QString msg("");
    bool error(true);
    if(statusCode == VideoCollectionCommon::statusSingleDeleteFail)
    {
        QString format = tr("Unable to delete item %1. It is currently open.");
        if(additional.isValid())
        {
           msg = format.arg(additional.toString());
        }
    }
    else if(statusCode == VideoCollectionCommon::statusMultipleDeleteFail)
    {
        msg = tr("Unable to delete some items which are currently open.");
    }
    else if(statusCode == VideoCollectionCommon::statusMultipleDeleteSucceed)
    {
        QString format = tr("%1 videos deleted");
        if(additional.isValid())
        {
            msg = format.arg(additional.toString());
        }
        error = false;
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



