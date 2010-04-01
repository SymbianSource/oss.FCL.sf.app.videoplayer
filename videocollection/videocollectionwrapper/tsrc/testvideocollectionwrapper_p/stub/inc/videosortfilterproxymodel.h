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
* Description:   stub Videosortfilterproxymodel for VideoCollectionWrapperPrivate unit tests  
* 
*/

#ifndef VIDEOSORTFILTERPROXYMODEL_H
#define VIDEOSORTFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>

class QTimer;
class VideoListDataModel;
class VideoCollectionClient;

class VideoSortFilterProxyModel : public QObject 
{
    Q_OBJECT
    
public: 
    
    /**
     * Contructor.
     *
     * @param parent parent of this widget
     * @param client Collection client pointer to use.
     */
    VideoSortFilterProxyModel( int type, QObject *parent=0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoSortFilterProxyModel();
    
    /**
     * Initializes model, calls initialize to source model, and sets
     * the given sourceModel as source model for this proxy model.
     * 
     * @param collection mpx video collectionobject
     * @param sourceModel the source model
     * 
     * @return int: 0 if everything ok
     */
    int initialize(VideoListDataModel *sourceModel);
    
public:
 
    
    /**
     * sets gInitFails
     * 
     * @param bool
     */
    static void setInitFailure(bool fails);

    
    /**
     * Pointer to the actual model.
     * Not owned.
     */
    VideoListDataModel *mModel;
    
    /**
     * type
     */
    int mType;

};



#endif  //VIDEOSORTFILTERPROXYMODEL_H
