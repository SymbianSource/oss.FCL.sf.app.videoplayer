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
* Description:  Implementation of QMPXVideoPlaybackStatusPaneControl
*
*/

// Version : %version: 10 %


#include <hbmenu.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbinstance.h>

#include <w32std.h>

#include "mpxvideo_debug.h"
#include "hbvideobaseplaybackview.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackstatuspanecontrol.h"
#include "mpxvideoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::QMPXVideoPlaybackStatusPaneControl()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackStatusPaneControl::QMPXVideoPlaybackStatusPaneControl( 
        QMPXVideoPlaybackControlsController* controller, 
        TMPXVideoPlaybackControls index, 
        HbWidget* widget, 
        TUint controlproperties )
    : QMPXVideoPlaybackFullScreenControl( controller, index, widget, controlproperties )
    , mActionBack( NULL )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackStatusPaneControl::QMPXVideoPlaybackStatusPaneControl()"));

    mActionBack = new HbAction( Hb::BackAction );

    //
    // Press "back" key means going back to previous view if it's avaiable
    //
    connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
    hbInstance->allMainWindows()[0]->addSoftKeyAction( Hb::SecondarySoftKey , mActionBack );

    connect( mController->view()->menu(), SIGNAL( aboutToShow() ), this, SLOT( handleAboutToShow() ) );
    connect( mController->view()->menu(), SIGNAL( aboutToHide() ), this, SLOT( handleAboutToHide() ) );

    QGraphicsWidget *widget1 = mController->layoutLoader()->findWidget( QString( "title" ) );
    mTitleLabel = qobject_cast<HbLabel*>( widget1 );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::~QMPXVideoPlaybackStatusPaneControl()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackStatusPaneControl::~QMPXVideoPlaybackStatusPaneControl()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackStatusPaneControl::QMPXVideoPlaybackStatusPaneControl"));

    disconnect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
    disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
    hbInstance->allMainWindows()[0]->removeSoftKeyAction( Hb::SecondarySoftKey , mActionBack );

    disconnect( mController->view()->menu(), SIGNAL( aboutToShow() ), this, SLOT( handleAboutToShow() ) );
    disconnect( mController->view()->menu(), SIGNAL( aboutToHide() ), this, SLOT( handleAboutToHide() ) );

    mController->view()->menu()->close();

    if ( mActionBack )
    {
        delete mActionBack;
        mActionBack = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::setVisible()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackStatusPaneControl::setVisible( bool visible )
{
    mVisible = visible;

    if ( mVisible )
    {
        mController->view()->showItems(
                Hb::IndicatorItems | Hb::TitlePaneItem | Hb::SecondarySoftKeyItem | Hb::TitleBarItem );

        if ( mController->viewMode() == EFullScreenView ||
             mController->viewMode() == EDetailsView )
        {
            mTitleLabel->setVisible( true );            
        }
    }
    else
    {
        mController->view()->menu()->close();
        mController->view()->hideItems( Hb::AllItems );

        mTitleLabel->setVisible( false );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackStatusPaneControl::isVisible()
{
    return mVisible;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::setVisibility()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackStatusPaneControl::setVisibility( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackStatusPaneControl::setVisibility()"));

    switch( state )
    {
        case EPbStatePlaying:
        case EPbStatePaused:
        {
            setMenu( mController->fileDetails() );
            break;
        }
        case EPbStatePluginSeeking:
        case EPbStateBuffering:
        case EPbStateNotInitialised:
        case EPbStateInitialising:
        {
            mController->view()->menu()->clearActions();
            break;
        }
    }

    QMPXVideoPlaybackFullScreenControl::setVisibility( state );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::updateControlsWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackStatusPaneControl::updateControlsWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackStatusPaneControl::updateControlsWithFileDetails()"));

    //
    // If it's details view, need to go back to the default playback view with secondary key
    // If not, we need to go back to the collection view with the secondary key
    //
    if ( mController->viewMode() == EDetailsView )
    {
        disconnect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
        connect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
    }
    else
    {
        disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
        connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
    }

    setMenu( details );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::setMenu()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackStatusPaneControl::setMenu( QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackStatusPaneControl::setMenu()"));

    //
    // Set menu
    //
    mController->view()->menu()->clearActions();

    if ( ! details->mTvOutConnected || ! details->mTvOutPlayAllowed )
    {
        if ( details->mVideoEnabled && mController->viewMode() != EDetailsView )
        {
            mController->view()->menu()->addAction( tr( "Subtitle" ) );
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::handleAboutToShow()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackStatusPaneControl::handleAboutToShow()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackStatusPaneControl::handleAboutToShow()"));

    mController->resetDisappearingTimers( EMPXTimerCancel );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::handleAboutToHide()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackStatusPaneControl::handleAboutToHide()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackStatusPaneControl::handleAboutToHide()"));

    mController->resetDisappearingTimers( EMPXTimerReset );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::controlListUpdated()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackStatusPaneControl::controlListUpdated(
        QMPXVideoPlaybackViewFileDetails* details )
{
    setMenu( details );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::openFullScreenView()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackStatusPaneControl::openFullScreenView()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackStatusPaneControl::openFullScreenView()"));

    mController->changeViewMode( EFullScreenView );
}

// End of file
