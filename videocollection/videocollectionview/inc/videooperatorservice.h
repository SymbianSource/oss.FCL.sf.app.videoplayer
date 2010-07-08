/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VideoOperatorService class declaration.
*
*/


#ifndef VIDEOOPERATORSERVICE_H
#define VIDEOOPERATORSERVICE_H

#include <qobject.h>

class HbPushButton;

/**
 * Loading and launching functionality for operator customisable services.  
 */
class VideoOperatorService : public QObject
{
    Q_OBJECT

public:

    /**
     * Contructor.
     *
     * @param parent parent of this widget
     */
    VideoOperatorService(QObject *parent = 0);

    /**
     * Loads the service.
     * 
     * @titleKey Central Repository key id for the service's title.
     * @iconKey Central Repository key id for the service's icon resource.
     * @uriKey Central Repository key id for the service's URI.
     * @uidKey Central Repository key id for the service's UID.
     * 
     * @return True if service was loaded succesfully.
     */
    bool load(int titleKey, int iconKey, int uriKey, int uidKey);

    /**
     * Returns name for the service.
     * 
     * @return name of the service.
     */
    const QString title() const;
    
    /**
     * Returns the icon resource for the service.
     * 
     * @return path or resource id to the icon.
     */
    const QString iconResource() const;
    
private:
    
    void launchApplicationL(const TUid uid, TInt viewId);
    
public slots:
    
    /**
     * Launches the service.
     */
    void launchService();
    
private:
    
    Q_DISABLE_COPY(VideoOperatorService)
    
    /**
     * Title for the service.
     */
    QString mTitle;
    
    /**
     * Icon for the service.
     */
    QString mIconResource;
    
    /**
     * Service URL if service should launch an URL.
     */
    QString mServiceUri;
    
    /**
     * Application UID if service should launch an external application.  
     */
    int mApplicationUid;
};

#endif // VIDEOOPERATORSERVICE_H

// End of file.
