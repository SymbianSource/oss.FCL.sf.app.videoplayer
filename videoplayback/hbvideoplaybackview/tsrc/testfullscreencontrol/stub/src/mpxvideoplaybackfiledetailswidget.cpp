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
* Description:  Implementation of QMPXVideoPlaybackFileDetailsWidget
*
*/

// Version : %version:  2 %



#include <qfileinfo>
#include <qgraphicslinearlayout>
#include <qdatetime>
#include <qdir>

#include <hblistwidgetitem.h>
#include <hbratingslider.h>
#include <hblistwidget.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackfiledetailswidget.h"
#include "mpxvideoplaybackcontrolscontroller.h"

const float KILOBYTE = 1024 ;
const QString KDATETIMEFORMAT = "d/M/yyyy hh:mm:ss ap";

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::QMPXVideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackFileDetailsWidget::QMPXVideoPlaybackFileDetailsWidget( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
    , mListWidget( 0 )
    , mFileDetailsUpdated( false )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::QMPXVideoPlaybackFileDetailsWidget()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::~QMPXVideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackFileDetailsWidget::~QMPXVideoPlaybackFileDetailsWidget()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::~QMPXVideoPlaybackFileDetailsWidget()"));
   
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::updateControlsWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    Q_UNUSED( details );
}


//End of file
