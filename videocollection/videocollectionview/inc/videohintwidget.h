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
* Description:   Videolist hint widget
*
*/


#ifndef VIDEOHINTWIDGET_H
#define VIDEOHINTWIDGET_H

#include <hbwidget.h>

class HbPushButton;
class HbLabel;
class VideoCollectionUiLoader;
class QGraphicsItem;

/**
 * Widget for displaying no videos text and possible hint and button for user
 * find where to download new videos.
 */
class VideoHintWidget : public HbWidget
{
    Q_OBJECT

public:
    
    enum HintLevel {
        AllVideos,
        Collection
    };

    /**
     * Contructor.
     *
     * @param parent parent of this widget
     */
    VideoHintWidget(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent = 0);

    /**
     * Destructor.
     *
     */
    ~VideoHintWidget();
    
    /**
     * Method creates colleciton wrapper, grid and list views,
     * layout and activates correct view based on the current orientation
     *
     * @return int 0 initialization ok, < 0 if fails.
     */
    int initialize();
    
    /**
     * Sets the level where the hint is correctly. Hint displays differently in
     * for example collection level, than in allVideos level.
     * 
     * @param level The current level.
     */
    void setLevel(HintLevel level);
    
    /**
     * Overridden from QGraphicsItem. Sets the visibility of this widget.
     * 
     * @param visible true if widget is set visible, false othervise.
     */
    void setVisible(bool visible);
    
private slots:
    
    /**
     * Method activates correct view based on the given orientation.
     */
    void orientationChangedSlot(Qt::Orientation orientation);
    
private:
    
    /**
     * Method enables and displays current active view
     *
     * @return int 0 initialization ok, < 0 if fails.
     */
    void activate();

    /**
     * Method disables and hides current active view
     */
    void deactivate();
    
    /**
     * Shows/hides the correct UI components for current state.
     */
    void updateUiComponents();

private:
    
    Q_DISABLE_COPY(VideoHintWidget)
    
    /**
     * Pointer to the XML UI (DocML) loader, not owned
     */
    VideoCollectionUiLoader     *mUiLoader;
    
	/**
     * Service button object.
     */
    HbPushButton                *mServiceButton;
    
    /**
     * Hint text label (ie. the second row label).
     */
    HbLabel                     *mHintLabel;
    
    /**
     * Service icon resource string.
     */
    QString                     mServiceIconString;
    
    /**
     * Service icon pressed resource string.
     */
    QString                     mServiceIconPressedString;
    
    /**
     * Service icon.
     */
    HbIcon                      *mServiceIcon;
    
    /**
     * Current hint level.
     */
    HintLevel mCurrentLevel;

};

#endif // VIDEOHINTWIDGET_H
