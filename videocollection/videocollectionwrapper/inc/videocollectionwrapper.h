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
* Description:   VideoCollectionWrapper class definition
* 
*/

#ifndef __VIDEOCOLLECTIONWRAPPER_H__
#define __VIDEOCOLLECTIONWRAPPER_H__


// INCLUDES
#include <QObject>
#include <qabstractitemmodel.h>
#include "videocollectionexport.h"

// FORWARD DECLARATIONS
class VideoCollectionWrapperPrivate;
class VideoSortFilterProxyModel;


/**
 * Class is used as an interface of the video collection QT wrapper.
 * 
 * Singleton instance is deleted when it's not used anymore by clients. At instantion time  
 * reference count is incremented. Client must call decreaseReferenceCount when it doesn't 
 * need the instance anymore. When reference count is 0 the instance is destroyed.
 * 
 * * Usage:
 *  @code
 *  #include "videocollectionwrapper.h"
 *  #include "videosortfilterproxymodel.h"
 *  
 *  ...
 *  ////
 *  // Getting the instances
 *  ////
 *  VideoCollectionWrapper *wrapper = VideoCollectionWrapper::instance();
 * 
 *  VideoSortFilterProxyModel *model = wrapper->getModel();
 *  ...
 *  ////
 *  // Opening collection and start fetching video item data
 *  ////
 *  if(model)
 *  {
 *      mModel->open(VideoListWidget::ELevelVideos);
 *  }
 *  // see model documentation for the open funtionality
 *
 *  /////
 *  // Instance is not used anymore. 
 *  // Wrapper owns the model, so client should not deallocate it
 *  /////
 *  wrapper->decreaseReferenceCount();
 *  wrapper = 0; // Don't use before new instantion.
 * 
 *  @endcode
 * 
 */
class VIDEOCOLLECTION_DLL_EXPORT VideoCollectionWrapper : public QObject         
{    
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
    
public: // Constructor
    
    /**
     * Returns singleton instance for this class.
     * 
     * WARNING! Not safe to call this from destructor of another function scope static object!
     * 
     * @return The singleton instance.
     */
    static VideoCollectionWrapper *instance();
    
    /**
     * Decreases the reference count, when count reaches zero cleanup is done.
     * 
     */
    void decreaseReferenceCount();    
    
	/**
     * Returns pointer to model
     * 
     * @return address to model or NULL if fails.
     */    
    VideoSortFilterProxyModel* getModel();

    /**
     * Method can be used by client to emit status signal
     * containing status code from particular async status.
     * 
     * @param statusCode code of status
     * 
     * @param additional additional information for the code
     */
    void sendAsyncStatus(int statusCode, QVariant &additional);
     
signals:     

    /**
     * Signal that can be emitted by the wrapper to indicate status
     * of some async operation.
     * 
     * See videocollectioncommon.h for codes
     * 
     * @param status code of status
     * 
     * @param additional additional information for the code
     */
    void asyncStatus(int statusCode, QVariant &additional);

private:
    
    /**
     * Private contructor.
     */    
    VideoCollectionWrapper(); 
    
    /**
     * Private destructor.
     *
     */
    virtual ~VideoCollectionWrapper();    
    
    /**
     * disables copy-constructor and assingment operator
     */
    Q_DISABLE_COPY(VideoCollectionWrapper)    
    
private:

	/**
     * d -pointer for actual implementation
     */ 
    VideoCollectionWrapperPrivate *d;

    /**
     * Reference count.
     */
    int mReferenceCount;
    
    /**
     * Singleton instance.
     */
    static VideoCollectionWrapper* mInstance;
};

#endif  // __VIDEOCOLLECTIONWRAPPER_H__

// End of file
