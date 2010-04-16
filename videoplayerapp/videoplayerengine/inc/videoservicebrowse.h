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
* Description:   VideoServiceBrowse class definition
*
*/

// Version : %version: %

#ifndef VIDEOSERVICEBROWSE_H
#define VIDEOSERVICEBROWSE_H

//INCLUDES
#include <xqserviceprovider.h>

// FORWARD DECLARATIONS
class VideoServices; 

class VideoServiceBrowse : public XQServiceProvider
{
    Q_OBJECT
    
public:
    VideoServiceBrowse( VideoServices *parent = 0 );
    ~VideoServiceBrowse();
    
public:

    /**
     *  Complete pending service request
     *
     *  @param None
     *  @return None
     */
    void complete();

    /**
     *  Get current browsing category
     *
     *  @param None
     *  @return VideoServices::TVideoCategory
     */
    int getBrowseCategory() const;

    /**
     *  Return context title of videos application
     *
     *  @param None
     *  @return QString
     */
    QString contextTitle() const;
    
public slots:

    /**
     *  Browse video
     *
     *  @param title, Title of the embedded Videos application
     *  @param category, Category which type of videos are browsed
     *  @return None
     */
    void browseVideos( const QString &title, int category );

private:
    /** request index of the service */
    int mRequestIndex;
    
    /** reference to VideoServices instance */
    VideoServices *mServiceApp;
    
    /** current browsing category */
    int mCategory;
    
    /** title of the service client, if not set "Videos" is used */
    QString mTitle;
};

#endif//VIDEOSERVICEBROWSE_H
