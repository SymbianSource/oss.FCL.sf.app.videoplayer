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
* Description:  Implementation of QMPXVideoPlaybackControlConfiguration
*
*/

// Version : %version: da1mmcf#20 %



// INCLUDE FILES

#include <QGraphicsWidget>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolconfiguration.h"
#include "mpxvideoplaybackcontrolscontroller.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::CMPXVideoPlaybackControlConfiguration()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlConfiguration::QMPXVideoPlaybackControlConfiguration(
        QMPXVideoPlaybackControlsController* controller)
    : mControlsController( controller )
{
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlConfiguration::~QMPXVideoPlaybackControlConfiguration()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlConfiguration::~QMPXVideoPlaybackControlConfiguration()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::~QMPXVideoPlaybackControlConfiguration"));

    mControlsList.clear();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlConfiguration::createControlList()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlConfiguration::createControlList()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::createControlList()"));

    QMPXVideoPlaybackViewFileDetails* fileDetails = mControlsController->fileDetails();

    addControlToList( EMPXStatusPane );

    if ( fileDetails->mPlaybackMode == EMPXVideoStreaming ||
         fileDetails->mPlaybackMode == EMPXVideoLiveStreaming )
    {
        //
        // streaming, add branding animation control to show while initializing
        //
        addControlToList( EMPXBufferingAnimation );
    }
    else if ( fileDetails->mRNFormat )
    {
        addControlToList( EMPXRealLogoBitmap );
    }

    emit controlListUpdated();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlConfiguration::controlList
// -------------------------------------------------------------------------------------------------
//
QList<TMPXVideoPlaybackControls>& QMPXVideoPlaybackControlConfiguration::controlList()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::controlList()"));

    return mControlsList;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlConfiguration::updateControlList
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlConfiguration::updateControlList(
    TMPXVideoPlaybackControlCommandIds event )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::updateControlList(%d)"), event);

    QGraphicsWidget *widget = 
            mControlsController->layoutLoader()->findWidget( QString( "transparentWindow" ) );

    switch ( event )
    {
        case EMPXControlCmdFullScreenViewOpened:
        {
            MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::updateControlList() full screen view"));

            widget->setVisible( true );

            deleteControlFromList( EMPXDetailsViewPlaybackWindow );
            deleteControlFromList( EMPXFileDetailsWidget );
            deleteControlFromList( EMPXIndicatorBitmap );

            break;
        }
        case EMPXControlCmdDetailsViewOpened:
        {
            MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::updateControlList() details view"));

            widget->setVisible( false );

            deleteControlFromList( EMPXIndicatorBitmap );

            addControlToList( EMPXDetailsViewPlaybackWindow );

            addControlToList( EMPXFileDetailsWidget );

            break;
        }
        case EMPXControlCmdTvOutConnected:
        case EMPXControlCmdAudionOnlyViewOpened:
        {
            MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::updateControlList() audio only view"));

            widget->setVisible( false );

            deleteControlFromList( EMPXDetailsViewPlaybackWindow );

            addControlToList( EMPXIndicatorBitmap );
            addControlToList( EMPXFileDetailsWidget );

            break;
        }
        case EMPXControlCmdRemoveRNLogo:
        {
            MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::updateControlList() RN Logo removed"));

            deleteControlFromList( EMPXRealLogoBitmap );

            break;
        }
    }

    emit controlListUpdated();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlConfiguration::deleteControlFromList
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlConfiguration::deleteControlFromList(
        TMPXVideoPlaybackControls control )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::deleteControlFromList(%d)"), control);

    mControlsList.removeAll( control );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlConfiguration::addControlToList
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlConfiguration::addControlToList(
        TMPXVideoPlaybackControls control )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::addControlToList(%d)"), control);

    if ( ! mControlsList.contains( control ) )
    {
        mControlsList.append( control );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlConfiguration::updateControlsWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlConfiguration::updateControlsWithFileDetails()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::updateControlsWithFileDetails()"));

    addControlToList( EMPXControlBar );

    if ( mControlsController->fileDetails()->mVideoEnabled )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackControlConfiguration::updateControlsWithFileDetails() video enabled"));

        QGraphicsWidget *widget = 
                mControlsController->layoutLoader()->findWidget( QString( "transparentWindow" ) );
        widget->setVisible( true );
    }

    emit controlListUpdated();
}

// End of File
