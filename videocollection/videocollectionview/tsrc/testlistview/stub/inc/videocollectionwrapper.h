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
* Description:   VideoCollectionWrapper dummy class definition
* 
*/

#ifndef __VIDEOCOLLECTIONWRAPPER_H__
#define __VIDEOCOLLECTIONWRAPPER_H__


// INCLUDES
#include <QObject>


// FORWARD DECLARATIONS
class VideoSortFilterProxyModel;
class DummyDataModel;

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
    static VideoCollectionWrapper *instance();

    /**
     * Decreases the reference count, when count reaches zero cleanup is done.
     * 
     */
    void decreaseReferenceCount();    
        
    
	/**
     * Opens a collection into all videos category, if not yet opened, and
     * returns pointer to model
     * 
     * @return address to model or NULL if fails.
     */    
    VideoSortFilterProxyModel* open(int level);

	/**
     * Returns pointer to model
     * 
     * @return address to model or NULL if fails.
     */    
    VideoSortFilterProxyModel* getModel();
    
    /**
     * Method can be used by client to emit status signal
     * indicating async operation status.
     * 
     * @param statusCode code of error
     * 
     * @param additional additional information for the code
     */
    void sendAsyncStatus(int statusCode, QVariant &additional);    
	
public: // Test functions:
	
	/**
     * Returns pointer to model with the data.
     * 
     * @return address to model or NULL if fails.
     */    
    DummyDataModel* getDataModel();    
    
signals:     

	/**
	 * Signal to be emitted in case of some error
	 * 
	 * @param statusCode code of error
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
     * Creates model.
     */
    void createModel();
    
    /**
     * disables copy-constructor and assingment operator
     */
    Q_DISABLE_COPY(VideoCollectionWrapper)
    
public:
    
	VideoSortFilterProxyModel *mModel;
	DummyDataModel *mDataModel;
	
	static VideoCollectionWrapper *mInstance;
	
	static int mReferenceCount;
	
public:
	static bool mOpenFails;
	static bool mGetModelFails;
};

#endif  // __VIDEOCOLLECTIONWRAPPER_H__
// End of file
    


