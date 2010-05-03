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
* Description:  Implementation of QMPXVideoPlaybackDocumentLoader
*
*/

// Version : %version:  1 %



#ifndef MPXVIDEOPLAYBACKDOCUMENTHANDLER_H_
#define MPXVIDEOPLAYBACKDOCUMENTHANDLER_H_


#include <qobject>
#include <hbdocumentloader.h>

class QList;
class QGraphicsWidget;
class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackDocumentLoader : public QObject
{
    public:
        QMPXVideoPlaybackDocumentLoader( QMPXVideoPlaybackControlsController *controller );

        virtual ~QMPXVideoPlaybackDocumentLoader();

    public:
        
        QGraphicsWidget* findWidget( const QString &name );
        void load( const QString &fileName, const QString &section , bool *ok );
        void load( const QString &fileName, bool *ok );
        bool isVisible( int index );

    private:
        QGraphicsWidget* createWidget( const QString &name );
        int exist( const QString &name );

    public:
        QList<QGraphicsWidget*>                 mWidgets;
        
    private:
        
        QMPXVideoPlaybackControlsController*    mController;
        int                                     mIndex;
};

#endif /*MPXVIDEOPLAYBACKDOCUMENTHANDLER_H_*/
