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
 * Description:   VideoCollectionViewPlugin class definition
 * 
 */

#ifndef VIDEOVIEWPLUGIN_H
#define VIDEOVIEWPLUGIN_H

#include <mpxviewpluginqt.h>

class VideoCollectionUiLoader;
class VideoListView;

class VideoCollectionViewPlugin: public MpxViewPlugin
{
    Q_OBJECT

public: // Constructor / destructor

    /**
     * Contructor.
     *
     */
    VideoCollectionViewPlugin();

    /**
     * Destructor.
     *
     */
    virtual ~VideoCollectionViewPlugin();

public: // from QViewPlugin

    /**
     * Allocates view and it's objects to be ready to 
     * be activated.
     *
     */
    void createView();

    /**
     * Deallocates view and it's objects.
     */
    void destroyView();

    /**
     * Activates view
     *
     */
    void activateView();

    /**
     * Deactivates view
     *
     */
    void deactivateView();

    /**
     * Returns a pointer to the view
     *
     * @return QGraphicsWidget*
     */
    QGraphicsWidget* getView();

signals:
    /**
     * Command signal, plugin user shoulf connect this in case it wants
     * to receive commands emitted from the view.
     *
     * @param command id
     */
    void command(int);

public slots: // from QViewPlugin

    /**
     * Plugin user can notify orientation changes by connecting into this slot
     * ti it's signal. In normal cases, view handles orientation chages itself.
     *
     * @param orientation new orientation
     */
    void orientationChange(Qt::Orientation orientation);

    /**
     * Plugin user can notify oback button changes by connecting into this slot
     *
     */
    void back();

private:

    /**
     * docml ui loader, owned
     */
    VideoCollectionUiLoader *mUiLoader;

    /**
     * View object, owned
     */
    VideoListView *mView;

    /**
     * Activated flag. Set as true when view is properly activated.
     * If flag is false, no operations can be do to the view.
     */
    bool mActivated;
};

#endif  // VIDEOVIEWPLUGIN_H
// End of File
