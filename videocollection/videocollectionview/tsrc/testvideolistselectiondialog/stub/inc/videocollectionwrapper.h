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
* Description:   stub VideoCollectionWrapper class for testing multiple selection dialog
* 
*/

#ifndef __VIDEOCOLLECTIONWRAPPER_H__
#define __VIDEOCOLLECTIONWRAPPER_H__


// INCLUDES
#include <QObject>
#include <qabstractitemmodel.h>
#include "videosortfilterproxymodel.h"

// class declaration required for static instance
class VideoCollectionWrapper;

static VideoCollectionWrapper *mInstance = 0;

class VideoCollectionWrapper : public QObject         
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
    static VideoCollectionWrapper* instance()
    {
       if(!mInstance)
       {
           mInstance = new VideoCollectionWrapper();
       }
       if(!mInstance->mReturnNull)
       {
           mInstance->mReferenceCount++;
           return mInstance;
       }
       return 0;
    }
    
    void decreaseReferenceCount()
    {
        if(mInstance)
        {
            if(--mInstance->mReferenceCount == 0)
            {
                delete mInstance;
                mInstance = NULL;
            }
        }
    }
    
	/**
     * Returns pointer to model
     * 
     * @return address to model or NULL if fails.
     */    
    VideoSortFilterProxyModel* getModel()
    {
        return mModel;
    }
            

    void setModel(VideoSortFilterProxyModel *model)   
    {
        mModel = model;
    }
   

    /**
     * counter for succeed instance calls
     */
    int mReferenceCount;
    
    /**
     * if true, returns null from instance(), does not increate reference count
     */
    bool mReturnNull;
    
private:
  
    /**
     * Private contructor.
     */    
    VideoCollectionWrapper() : mReferenceCount(0), mReturnNull(false){}; 
    
    /**
     * Private destructor.
     *
     */
    virtual ~VideoCollectionWrapper(){};    
    
    /**
     * disables copy-constructor and assingment operator
     */
    Q_DISABLE_COPY(VideoCollectionWrapper)

    
    VideoSortFilterProxyModel* mModel;
   
};

#endif  // __VIDEOCOLLECTIONWRAPPER_H__
// End of file
    


