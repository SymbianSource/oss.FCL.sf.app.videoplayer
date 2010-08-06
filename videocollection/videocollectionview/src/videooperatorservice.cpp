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
* Description:   VideoOperatorService class implementation.
*
*/

// Version : 

// INCLUDE FILES

#include <qdesktopservices.h>
#include <qurl.h>

#include <e32base.h>
#include <eikenv.h>
#include <eikappui.h>
#include <apgcli.h>

#include "videooperatorservice.h"
#include "videocollectionviewutils.h"
#include "videocollectioncenrepdefs.h"
#include "videocollectiontrace.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoOperatorService::VideoOperatorService(QObject *parent) : 
    QObject(parent)
{
    FUNC_LOG;
}

// ---------------------------------------------------------------------------
// load
// ---------------------------------------------------------------------------
//
bool VideoOperatorService::load(int titleKey, int iconKey, int serviceUriKey, int appUidKey)
{
    FUNC_LOG;
    VideoCollectionViewUtils& utils = VideoCollectionViewUtils::instance();
    
    mTitle = utils.getCenRepStringValue(titleKey);
    mIconResource = utils.getCenRepStringValue(iconKey);
    mServiceUri = utils.getCenRepStringValue(serviceUriKey);
    mApplicationUid = utils.getCenRepIntValue(appUidKey);
    
    // Icon is required, either service uri or application uid is required.
    if(mIconResource.isEmpty() || (mServiceUri.isEmpty() && mApplicationUid <= 0))
    {
        return false;
    }
    
    return true;
}

// ---------------------------------------------------------------------------
// title
// ---------------------------------------------------------------------------
//
const QString VideoOperatorService::title() const
{
    return mTitle;
}

// ---------------------------------------------------------------------------
// iconResource
// ---------------------------------------------------------------------------
//
const QString VideoOperatorService::iconResource() const
{
    return mIconResource;
}

// ---------------------------------------------------------------------------
// launchService
// ---------------------------------------------------------------------------
//
void VideoOperatorService::launchService()
{
    FUNC_LOG;
    
    if(!mServiceUri.isEmpty())
    {
        INFOQSTR_1("VideoOperatorService::launchService() starting url: %S", mServiceUri);
        QDesktopServices::openUrl(QUrl(mServiceUri));
    }
    else
    {
        INFO_1("VideoOperatorService::launchService() starting application 0x%x", mApplicationUid);
        TRAP_IGNORE(launchApplicationL(TUid::Uid(mApplicationUid), 0));
    }
}

// ---------------------------------------------------------------------------
// launchApplicationL
// ---------------------------------------------------------------------------
//
void VideoOperatorService::launchApplicationL(const TUid uid, TInt viewId)
{
    CEikonEnv *eikEnv = CEikonEnv::Static();
    
    if (viewId > 0 && eikEnv) {
        TVwsViewId view(uid, TUid::Uid(viewId));
        eikEnv->EikAppUi()->ActivateViewL(view);
    } else 
    {
        RWsSession wsSession;
        User::LeaveIfError(wsSession.Connect());
        CleanupClosePushL<RWsSession>(wsSession);

        // TApaAppInfo size is greater then 1024 bytes
        // so its instances should not be created on the stack.
        TApaAppInfo* appInfo = new (ELeave) TApaAppInfo;
        CleanupStack::PushL(appInfo);
        TApaAppCapabilityBuf capabilityBuf;
        RApaLsSession appArcSession;
        User::LeaveIfError(appArcSession.Connect());
        CleanupClosePushL<RApaLsSession>(appArcSession);

        User::LeaveIfError(appArcSession.GetAppInfo(*appInfo, uid));
        User::LeaveIfError(appArcSession.GetAppCapability(
           capabilityBuf, uid));

        TApaAppCapability &caps = capabilityBuf();
        CApaCommandLine *cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL(appInfo->iFullName);

        if (caps.iLaunchInBackground) {
            cmdLine->SetCommandL(EApaCommandBackground);
        } else {
            cmdLine->SetCommandL(EApaCommandRun);
        }

        cmdLine->SetTailEndL(KNullDesC8);

        User::LeaveIfError(appArcSession.StartApp(*cmdLine));

        CleanupStack::PopAndDestroy(cmdLine);
        CleanupStack::PopAndDestroy(&appArcSession);
        CleanupStack::PopAndDestroy(appInfo);

        CleanupStack::PopAndDestroy(&wsSession);
    }
}

// End of file.
