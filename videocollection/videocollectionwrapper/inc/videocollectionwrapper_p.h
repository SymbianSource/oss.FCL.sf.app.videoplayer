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
* Description:   VideoCollectionWrapperPrivate class definition
* 
*/

#ifndef __VIDEOCOLLECTIONWRAPPERPRIVATE_H__
#define __VIDEOCOLLECTIONWRAPPERPRIVATE_H__


// INCLUDES
#include <QObject>
#include <QPointer>
#include <qabstractitemmodel.h>
#include "videocollectioncommon.h"

// FORWARD DECLARATIONS
class CMPXMediaArray;
class CMPXMedia;
class VideoListDataModel;
class VideoSortFilterProxyModel;

class VideoCollectionWrapperPrivate : public QObject        
{    
	Q_DISABLE_COPY(VideoCollectionWrapperPrivate) 
	
	/**
     * define to be able to use signals and slots
     */
    Q_OBJECT
	
public: // Constructor

	/**
	* Default constructor
    */
	VideoCollectionWrapperPrivate();	
	
	/**
	 * Destructor
	 */
	~VideoCollectionWrapperPrivate();	

    /**
     * Returns the pointer into model. Creates the model if it doesn't exists yet.
     * 
     * @param type type of model
     * @return address of model, NULL if creation did not succeed.
     */    
	VideoSortFilterProxyModel* getModel(VideoCollectionCommon::TModelType &type);

private slots:
  
    
    /**
     * Signaled when UI environment is about to be destroyed. 
     * Source model needs to be cleaned up before of that
     * 
     */
    void aboutToQuitSlot();
           
private:
    
	/**
     * data model for all videos
     */
	QPointer<VideoSortFilterProxyModel> mAllVideosModel;
	
	/**
	 * data model for collections
	 */
	QPointer<VideoSortFilterProxyModel> mCollectionsModel;
	
	/**
	 * data model for collection content
	 */
    QPointer<VideoSortFilterProxyModel> mCollectionContentModel;
	
	/**
	 * data model for collection content
	 */
	QPointer<VideoSortFilterProxyModel> mGenericModel;
	
	/**
	 * source model
	 */
	QPointer<VideoListDataModel>        mSourceModel;
    
};
#endif  // __VIDEOCOLLECTIONWRAPPERPRIVATE_H__

// End of file
    


