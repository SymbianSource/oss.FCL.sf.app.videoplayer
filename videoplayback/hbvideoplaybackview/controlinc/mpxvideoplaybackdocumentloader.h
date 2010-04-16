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
* Description:  Implementation of QMPXVideoPlaybackDocumentLoader
*
*/

// Version : %version: 2 %



#ifndef MPXVIDEOPLAYBACKDOCUMENTHANDLER_H_
#define MPXVIDEOPLAYBACKDOCUMENTHANDLER_H_

#include <hbdocumentloader.h>

class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackDocumentLoader : public HbDocumentLoader
{
    public:
        QMPXVideoPlaybackDocumentLoader( QMPXVideoPlaybackControlsController *controller );

        virtual ~QMPXVideoPlaybackDocumentLoader();

    public:
        virtual QObject *createObject( const QString& type, const QString &name ) ;

    private:
        QMPXVideoPlaybackControlsController   *mController;
};

#endif /*MPXVIDEOPLAYBACKDOCUMENTHANDLER_H_*/

