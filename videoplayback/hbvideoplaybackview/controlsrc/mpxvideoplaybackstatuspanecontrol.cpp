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

// Version : %version: ou1cpsw#13 %


#include <hbmenu.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>

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
    , mFrameItem( NULL )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackStatusPaneControl::QMPXVideoPlaybackStatusPaneControl()"));

    mActionBack = new HbAction( Hb::BackNaviAction );

    //
    // Press "back" key means going back to previous view if it's avaiable
    //
    connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
    
    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
    HbView *currentView = mainWnd->currentView();
    currentView->setNavigationAction(mActionBack);

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

    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
    HbView *currentView = mainWnd->currentView();
    currentView->setNavigationAction(0);

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

    switch( mController->viewMode() )
    {
        case EFullScreenView:
        {
            //
            // Set TitleBar transparent and go back to preview view with back key
            // 
            disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
            connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );

            mController->view()->setTitleBarFlags( HbView::TitleBarTransparent );
            break;
        }
        case EDetailsView:
        {
            //
            // Set TitleBar opaque and go back to full screen view with back key
            // 
            disconnect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
            connect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );

            mController->view()->setTitleBarFlags( HbView::TitleBarFlagNone );

            break;
        }
        case EAudioOnlyView:
        {
            //
            // Set TitleBar opaque and go back to preview view with back key
            // 
            disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
            connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );

            mController->view()->setTitleBarFlags( HbView::TitleBarFlagNone );
            break;
        }
    }

    setMenu( details );

    //
    // Set framedrawer for semi transparent background
    //
    if ( ! mFrameItem )
    {
        mFrameItem = new HbFrameItem ( mTitleLabel );
        mFrameItem->frameDrawer().setFrameType( HbFrameDrawer::OnePiece );
        mFrameItem->frameDrawer().setFillWholeRect( true );
        mFrameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_status_trans_normal_c" );    
    }

    mFrameItem->setGeometry( mTitleLabel->boundingRect() );
    mFrameItem->setVisible( ( mController->viewMode() == EFullScreenView )? ETrue:EFalse ); 
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::setMenu()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackStatusPaneControl::setMenu( QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackStatusPaneControl::setMenu()"));

    //
    // No available menu for now
    //
    Q_UNUSED( details );
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
