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
* Description:  stub classes for testing CVideoCollectionClient
* 
*/

#ifndef __STUBCOLLECTIONSIGNALRECEIVER_H
#define __STUBCOLLECTIONSIGNALRECEIVER_H

#include <QObject>
// INCLUDES
#include "videodatasignalreceiver.h"
#include "stub/inc/videocollectionlistener.h"
#include "videocollectionclient.h"

/**
 * implementation class for slots to be connected
 */
class  StubSignalReceiver  : public VideoDataSignalReceiver
{

    Q_OBJECT
    
    Q_DISABLE_COPY(StubSignalReceiver)
    
public:
    
    /**
     * contructor
     */
    StubSignalReceiver();
    
    /**
     * destructor
     */
    virtual ~StubSignalReceiver();
    
public slots:   
    
    /**
     * No implementation needed for these tests
     */
    void newVideoListSlot(CMPXMediaArray *array);
    
    /**
     * No implementation needed for these tests
     */
    void appendVideoListSlot( CMPXMediaArray* videoList );
          
    /**
     * No implementation needed for these tests
     */
    void newVideoAvailableSlot(CMPXMedia *media);
    
    /**
     * No implementation needed for these tests
     */
    void videoDeletedSlot(TMPXItemId id);
           
    /**
     * No implementation needed for these tests
     */
    void videoDeleteCompletedSlot(int, QList<TMPXItemId> *failedIds);
    
    /**
     * No implementation needed for these tests
     */
    void videoDetailsCompletedSlot(TMPXItemId id);
 
};

/**
 * Class for testing signal connections, all signals exists
 */
class StubListenerAllSignals : public VideoCollectionListener
{
    Q_OBJECT
    
public:
    /**
     * contructor
     */
    StubListenerAllSignals(VideoCollectionClient &collectionClient); 
signals:

    /**
     * stub signal definition, not used
     */
    void newVideoList(CMPXMediaArray*);
    
    /**
     * stub signal definition, not used
     */
    void videoListAppended(CMPXMediaArray*);
              
    /**
     * stub signal definition, not used
     */
    void newVideoAvailable(CMPXMedia*);
        
    /**
     * stub signal definition, not used
     */
    void videoDeleted(TMPXItemId videoId);
               
    /**
     * stub signal definition, not used
     */
    void videoDeleteCompleted(int, QList<TMPXItemId>*);
        
    /**
     * stub signal definition, not used
     */
    void videoDetailsCompleted(TMPXItemId videoId);
};


/**
 * Class for testing signal connections, no expected newVideoList -signal
 */
class StubListenerNoNewVideoList : public VideoCollectionListener
{
    Q_OBJECT
    
public:
    /**
     * contructor
     */
    StubListenerNoNewVideoList(VideoCollectionClient &collectionClient);  
signals:
    
    /**
     * stub signal definition, not used
     */
    void videoListAppended(CMPXMediaArray*);
    
    /**
     * stub signal definition, not used
     */
    void newVideoAvailable(CMPXMedia*);
        
    /**
     * stub signal definition, not used
     */
    void videoDeleted(TMPXItemId videoId);
               
    /**
     * stub signal definition, not used
     */
    void videoDeleteCompleted(int, QList<TMPXItemId>*);
        
    /**
     * stub signal definition, not used
     */
    void videoDetailsCompleted(TMPXItemId videoId);
};

/**
 * Class for testing signal connections, no expected videoListAppended -signal
 */
class StubListenerNoVideoListAppended : public VideoCollectionListener
{   
    Q_OBJECT
    
public:
    /**
     * contructor
     */
    StubListenerNoVideoListAppended(VideoCollectionClient &collectionClient);  
signals:

    /**
     * stub signal definition, not used
     */
    void newVideoList(CMPXMediaArray*);
    
    /**
     * stub signal definition, not used
     */
    void newVideoAvailable(CMPXMedia*);
    
    /**
     * stub signal definition, not used
     */
    void videoDeleted(TMPXItemId videoId);
               
    /**
     * stub signal definition, not used
     */
    void videoDeleteCompleted(int, QList<TMPXItemId>*);
        
    /**
     * stub signal definition, not used
     */
    void videoDetailsCompleted(TMPXItemId videoId);
};



/**
 * Class for testing signal connections, no expected newVideoAvailable -signal
 */
class StubListenerNoNewVideoAvailable : public VideoCollectionListener
{   
    Q_OBJECT
    
public:
    /**
     * contructor
     */
    StubListenerNoNewVideoAvailable(VideoCollectionClient &collectionClient);  
signals:

    /**
     * stub signal definition, not used
     */
    void newVideoList(CMPXMediaArray*);
    
    /**
     * stub signal definition, not used
     */
    void videoListAppended(CMPXMediaArray*);
        
    /**
     * stub signal definition, not used
     */
    void videoDeleted(TMPXItemId videoId);
               
    /**
     * stub signal definition, not used
     */
    void videoDeleteCompleted(int, QList<TMPXItemId>*);
        
    /**
     * stub signal definition, not used
     */
    void videoDetailsCompleted(TMPXItemId videoId);
};

/**
 * Class for testing signal connections, no expected videoDeleted -signal
 */
class StubListenerNoVideoDeleted : public VideoCollectionListener
{   
    Q_OBJECT
    
public:
    /**
     * contructor
     */ 
    StubListenerNoVideoDeleted(VideoCollectionClient &collectionClient); 
signals:

    /**
     * stub signal definition, not used
     */
    void newVideoList(CMPXMediaArray*);
    
    /**
     * stub signal definition, not used
     */
    void videoListAppended(CMPXMediaArray*);
               
    /**
     * stub signal definition, not used
     */
    void newVideoAvailable(CMPXMedia*);
               
    /**
     * stub signal definition, not used
     */
    void videoDeleteCompleted(int, QList<TMPXItemId>*);
        
    /**
     * stub signal definition, not used
     */
    void videoDetailsCompleted(TMPXItemId videoId);
};

/**
 * Class for testing signal connections, no expected videoDeleteCompleted -signal
 */
class StubListenerNoVideoDeleteCompleted : public VideoCollectionListener
{
    Q_OBJECT
    
public:
    /**
     * contructor
     */
    StubListenerNoVideoDeleteCompleted(VideoCollectionClient &collectionClient);         
signals:

    /**
     * stub signal definition, not used
     */
    void newVideoList(CMPXMediaArray*);
    
    /**
     * stub signal definition, not used
     */
    void videoListAppended(CMPXMediaArray*);
               
    /**
     * stub signal definition, not used
     */
    void newVideoAvailable(CMPXMedia*);
        
    /**
     * stub signal definition, not used
     */
    void videoDeleted(TMPXItemId videoId);
        
    /**
     * stub signal definition, not used
     */
    void videoDetailsCompleted(TMPXItemId videoId);
};

/**
 * Class for testing signal connections, no expected videoDetailsCompleted -signal
 */
class StubListenerNoVideoDetailsCompleted : public VideoCollectionListener
{
    Q_OBJECT
    
public:
    /**
     * contructor
     */
    StubListenerNoVideoDetailsCompleted(VideoCollectionClient &collectionClient);   
      
signals:

    /**
     * stub signal definition, not used
     */
    void newVideoList(CMPXMediaArray*);
    
    /**
     * stub signal definition, not used
     */
    void videoListAppended(CMPXMediaArray*);
               
    /**
     * stub signal definition, not used
     */
    void newVideoAvailable(CMPXMedia*);
        
    /**
     * stub signal definition, not used
     */
    void videoDeleted(TMPXItemId videoId);
               
    /**
     * stub signal definition, not used
     */
    void videoDeleteCompleted(int, QList<TMPXItemId>*);
        
};

#endif
