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
* Description:   CVideoCollectionWrapper dummy class definition
*
*/

#ifndef __VIDEOCOLLECTIONWRAPPER_H__
#define __VIDEOCOLLECTIONWRAPPER_H__


// INCLUDES
#include <QObject>


// FORWARD DECLARATIONS
class VideoSortFilterProxyModel;


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
     * Returns pointer to model
     *
     * @return address to model or NULL if fails.
     */
    VideoSortFilterProxyModel* getModel();

/* Additional functions needed for testing purposes */

    void reset();

    void setModel(VideoSortFilterProxyModel* model);

signals:

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

    VideoSortFilterProxyModel* mModel;
    
public:
    
    static VideoCollectionWrapper *mInstance;
    
    static int mReferenceCount;
};

#endif  // __VIDEOCOLLECTIONWRAPPER_H__
// End of file



