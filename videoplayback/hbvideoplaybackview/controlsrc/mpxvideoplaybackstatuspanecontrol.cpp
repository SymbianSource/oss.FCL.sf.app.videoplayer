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

// Version : %version: 16 %



#include <QFileInfo>

#include <hbmenu.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbgroupbox.h>
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
    , mTitleLabel( NULL )
    , mTitleGroupBox( NULL )
    , mTitleLayout( NULL )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackStatusPaneControl::QMPXVideoPlaybackStatusPaneControl()"));

    Q_UNUSED( widget );

    mActionBack = new HbAction( Hb::BackNaviAction );

    //
    // Press "back" key means going back to previous view if it's avaiable
    //
    connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
    mController->view()->setNavigationAction( mActionBack );

    connect( mController->view()->menu(), SIGNAL( aboutToShow() ), this, SLOT( handleAboutToShow() ) );
    connect( mController->view()->menu(), SIGNAL( aboutToHide() ), this, SLOT( handleAboutToHide() ) );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::~QMPXVideoPlaybackStatusPaneControl()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackStatusPaneControl::~QMPXVideoPlaybackStatusPaneControl()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackStatusPaneControl::~QMPXVideoPlaybackStatusPaneControl"));

    disconnect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
    disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
    mController->view()->setNavigationAction( 0 );

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
        mController->view()->setTitleBarVisible( true );
        mController->view()->setStatusBarVisible( true );

        if ( mController->viewMode() == EFullScreenView ||
             mController->viewMode() == EDetailsView )
        {
            mTitleLayout->setVisible( true );
        }
    }
    else
    {
        mController->view()->menu()->close();
        mController->view()->setTitleBarVisible( false );
        mController->view()->setStatusBarVisible( false );

        mTitleLayout->setVisible( false );
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

    if ( ! mTitleLabel )
    {
        //
        // If title is not available, show clip name
        //
        QString title = mController->fileDetails()->mTitle;

        if ( title.count() == 0 )
        {
            QFileInfo fileInfo( mController->fileDetails()->mClipName );
            title = fileInfo.baseName ();
        }

        QGraphicsWidget *qWidget = mController->layoutLoader()->findWidget( QString( "title" ) );
        mTitleLabel = qobject_cast<HbLabel*>( qWidget );
        mTitleLabel->setPlainText( title );

        qWidget = mController->layoutLoader()->findWidget( QString( "titleGroupBox" ) );
        mTitleGroupBox = qobject_cast<HbGroupBox*>( qWidget );
        mTitleGroupBox->setHeading( title );

        mTitleLayout = mController->layoutLoader()->findWidget( QString( "titleLayout" ) );

        //
        // Set framedrawer for semi transparent background
        //
        HbFrameItem *frameItem = new HbFrameItem();
        frameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_multimedia_trans" );
        frameItem->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
        frameItem->frameDrawer().setFillWholeRect( true );
        mTitleLabel->setBackgroundItem( frameItem );
    }

    switch( mController->viewMode() )
    {
        case EFullScreenView:
        {
            //
            // Set TitleBar transparent and go back to preview view with back key
            // 
            disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
            connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );

            mController->view()->setViewFlags( 
                    HbView::HbViewFlags( HbView::ViewTitleBarTransparent | HbView::ViewStatusBarTransparent ) );

            mTitleLabel->setVisible( true );
            mTitleGroupBox->setVisible( false );

            break;
        }
        case EDetailsView:
        {
            //
            // Set TitleBar opaque and go back to full screen view with back key
            // 
            disconnect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
            connect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );

            mController->view()->setViewFlags( HbView::ViewFlagNone );

            mTitleGroupBox->setVisible( true );
            mTitleLabel->setVisible( false );

            break;
        }
        case EAudioOnlyView:
        {
            //
            // Set TitleBar opaque and go back to preview view with back key
            // 
            disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
            connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );

            mController->view()->setViewFlags( HbView::ViewFlagNone );
            break;
        }
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
