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
* Description:  MPX Video base playback view
*
*/

// Version : %version:  da1mmcf#11 %



// This file defines the API for .dll

#ifndef __HBVIDEOBASEPLAYBACKVIEW_H__
#define __HBVIDEOBASEPLAYBACKVIEW_H__

//  Include Files
#include <hbview.h>

//  Constants

//  Forward Declarations
class QTimer;
class RWindow;
class CMPXVideoViewWrapper;


//  Class Definitions

class HbVideoBasePlaybackView : public HbView
{
    Q_OBJECT

    public:
        HbVideoBasePlaybackView();        
        virtual ~HbVideoBasePlaybackView();

    signals:
        void activatePreviousView();
        void tappedOnScreen();

    protected slots:
        virtual void closePlaybackView();

    public:

        virtual void retrievePdlInformation();
        
        virtual void handleActivateView();
        
        virtual void handleDeactivateView();

        virtual void handlePluginError( int aError );
        
        /*
         *  Handle transition to the stopped state
         */
        virtual void handleStoppedState() = 0; 
        
        virtual void doClosePlayer();

        //
        //  Function the derived classes must implement
        //
        virtual void handleSoftkeyBack() = 0;

        /*
         *  Handle Download State Change
         *  @param  aState  new state of the download
         */
        virtual void handlePdlStateChange( int aState ) = 0;     

        virtual void handleBufferingState();

        virtual void issuePlayCommand();

        void initializeVideoPlaybackView();
        void displayInfoMessage( const QString& qString );
        void displayErrorMessage( const QString& qString );

        virtual void handleClosePlaybackView();
        
        bool event( QEvent *event );

        void paint( QPainter *painter, 
                    const QStyleOptionGraphicsItem *option,
                    QWidget *widget );

        RWindow *getWindow();

    protected:

        void mousePressEvent( QGraphicsSceneMouseEvent *event );
        void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

    protected: // data
        CMPXVideoViewWrapper                *mVideoMpxWrapper;                

        QTimer                              *mTimerForClosingView;

        bool                                 mActivated;

    public:
        friend class CMPXVideoViewWrapper;
};

#endif  // __HBVIDEOBASEPLAYBACKVIEW_H__

// EOF
