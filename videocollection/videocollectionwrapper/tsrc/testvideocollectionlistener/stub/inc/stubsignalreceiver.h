/**
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
* Description:   stub signal receiver for testing VideoCollectionListener
* 
*/

#ifndef __CSTUBSIGNALRECEIVER_H__
#define __CSTUBSIGNALRECEIVER_H__

#include <QObject>
#include <mpxcollectionmessagedefs.h>
#include <mpxitemid.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <Qlist>


class  StubSignalReceiver : public QObject
{
    Q_OBJECT
 
public:
    
    /**
     * contructor
     */
    StubSignalReceiver();
    
    /**
     * destructor
     */
    virtual ~StubSignalReceiver();
    
    /**
     * clears all members
     */
    void resetLatestItems();
    
    /**
     * return mLatestPtr
     */
    void* getLatestPointerAddr();
    
    /**
     * return mLatestItemId
     */
    TMPXItemId getLatestItemId();
    
    /**
     * return mLatesListData
     */
    QList<TMPXItemId>& getLatestListData();
    
    /**
     * return mLatestInteger
     */
    int getLatestIntegerData();
    
public slots:

    /**
     * Saves provided address to mLatestPtr
     */
    void newVideoListSlot(CMPXMediaArray* aVideoList);
    
    /**
     * Saves provided address to mLatestPtr 
     */
    void videoListAppendedSlot(CMPXMediaArray* aVideoList);
      
    /**
     * Saves provided address to mLatestPtr
     */
    void newVideoAvailableSlot(CMPXMedia* aVideo);
    
    /**
     * Saves provided integer to mLatestInt
     */
    void videoDeletedSlot(TMPXItemId videoId);
     
    /**
     * copies provided list data to mLatesListData
     */
    void videoDeleteCompletedSlot(int count, QList<TMPXItemId> *failedMediaIds);
    
    /**
     * Saves provided integer to mLatestInt
     */
    void videoDetailsCompletedSlot(TMPXItemId videoId);
    
private:
    /**
     * Contains address of latest pointer passed to object
     */
    void *mLatestPtr;
    
    /**
     * contains latest integer value sent
     */
    int mLatestInteger;
    
    /**
     * contains value if the latest integer passed to object
     */
    TMPXItemId mLatestItemId;
    
    /**
     * contains contents of list received from signal
     */
    QList<TMPXItemId> mLatesListData;
};

#endif
