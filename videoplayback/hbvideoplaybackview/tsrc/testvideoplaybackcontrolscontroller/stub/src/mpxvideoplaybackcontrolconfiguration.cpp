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
* Description:  Implementation of QMPXVideoPlaybackControlConfiguration
*
*/

// Version : %version: 1 %



// INCLUDE FILES

#include <qgraphicswidget>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolconfiguration.h"
#include "mpxvideoplaybackcontrolscontroller.h"

const TInt KControlListUpdated = 12; // additional extension of TMPXPlaybackState

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::CMPXVideoPlaybackControlConfiguration()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlConfiguration::QMPXVideoPlaybackControlConfiguration(
        QMPXVideoPlaybackControlsController* controller)
    : mControlsController( controller )
{
    createControlList();
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

    if ( fileDetails->mPlaybackMode != EMPXVideoLocal )
    {
        //
        // if it's not local mode, add branding animation control to show while initializing
        //
        addControlToList( EMPXBufferingAnimation );
    }

    addControlToList( EMPXStatusPane );
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
    
    if ( event == EMPXControlCmdFullScreenViewOpened ||
         event == EMPXControlCmdDetailsViewOpened ||
         event == EMPXControlCmdPluginInitialized )
    {
        mState = event;
    }
    else if ( event == EMPXControlCmdTvOutConnected ||
              event == EMPXControlCmdAudionOnlyViewOpened )
    {
        mState = EMPXControlCmdTvOutConnected;
    }
    else
    {
        mState = KControlListUpdated;    
    }
    
    
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

    mUpdateControlsWithFileDetails = true;
}

// End of File
