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
* Description:  HB Video playback view
*
*/

// Version : %version:  1 %



// This file defines the API for .dll

#ifndef __HBVIDEOBASEPLAYBACKVIEW_H__
#define __HBVIDEOBASEPLAYBACKVIEW_H__

//  Include Files
#include <hbview.h>

class CMPXVideoViewWrapper;
class RWindow;

//  Constants

//  Forward Declarations

//  Class Definitions

class HbVideoBasePlaybackView : public HbView
{
    Q_OBJECT

    public:
        HbVideoBasePlaybackView();
        virtual ~HbVideoBasePlaybackView();

        void handleActivateView(); 
        
        void handleDeactivateView();
        
        void mousePressEvent( QGraphicsSceneMouseEvent *event );
        
        QVariant itemChange( GraphicsItemChange change, const QVariant &value );
        
        void paint( QPainter *painter, 
                    const QStyleOptionGraphicsItem *option,
                    QWidget *widget );
        
        bool event( QEvent *event );
        
        virtual void handleClosePlaybackView();
        
        virtual void handleBufferingState();

        virtual void issuePlayCommand();                

        virtual void handlePluginError( int aError );                
        
        virtual void retrievePdlInformation();
        
        virtual void closePlaybackView();
        
        void startClosingPlaybackView();
        
        RWindow *getWindow();
        
        virtual void doClosePlayer();

    public:
        int mCurrentError;   // default = KErrNone 
        bool mViewActive;
        
        
    public:
        friend class CMPXVideoViewWrapper;
                				
};

#endif  // __HBVIDEOBASEPLAYBACKVIEW_H__

// EOF
