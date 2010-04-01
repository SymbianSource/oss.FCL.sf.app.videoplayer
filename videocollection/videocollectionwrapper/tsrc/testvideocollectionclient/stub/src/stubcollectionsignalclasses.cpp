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
* Description:   stub classes for testing VideoCollectionClient
* 
*/

#include <QList>
#include <mpxmediaarray.h>

#include "stubcollectionsignalclasses.h"

// -----------------------------------------------------------------------------
// StubSignalReceiver
// -----------------------------------------------------------------------------
//
StubSignalReceiver::StubSignalReceiver() 
{
}
 
// -----------------------------------------------------------------------------
// ~CStubSignalReceiver
// -----------------------------------------------------------------------------
//
StubSignalReceiver::~StubSignalReceiver()
{
    
}
 
// -----------------------------------------------------------------------------
// newVideoListSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::newVideoListSlot(CMPXMediaArray */*array*/)
{
    
}

// -----------------------------------------------------------------------------
// appendVideoListSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::appendVideoListSlot( CMPXMediaArray* /*videoList*/ )
{
    
}

// -----------------------------------------------------------------------------
// newVideoAvailableSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::newVideoAvailableSlot(CMPXMedia */*media*/)
{
    
}

// -----------------------------------------------------------------------------
// videoDeletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoDeletedSlot(TMPXItemId /*id*/)
{
    
}

// -----------------------------------------------------------------------------
// videoDeleteCompletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoDeleteCompletedSlot(int /*overallCount*/, QList<TMPXItemId>* /*failedIds*/)
{
    
}

// -----------------------------------------------------------------------------
// videoDetailsCompletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoDetailsCompletedSlot(TMPXItemId )
{
    
}

// -----------------------------------------------------------------------------
// StubListenerAllSignals
// -----------------------------------------------------------------------------
//
StubListenerAllSignals:: StubListenerAllSignals(VideoCollectionClient &collectionClient) :
VideoCollectionListener(collectionClient)
{
    
}

// -----------------------------------------------------------------------------
// StubListenerNoNewVideoList
// -----------------------------------------------------------------------------
//
StubListenerNoNewVideoList::StubListenerNoNewVideoList(VideoCollectionClient &collectionClient) :
VideoCollectionListener(collectionClient)
{
    
}
// -----------------------------------------------------------------------------
// StubListenerNoVideoListAppended
// -----------------------------------------------------------------------------
//
StubListenerNoVideoListAppended::StubListenerNoVideoListAppended(VideoCollectionClient &collectionClient) :
VideoCollectionListener(collectionClient)
{
    
}       

// -----------------------------------------------------------------------------
// StubListenerNoNewVideoAvailable
// -----------------------------------------------------------------------------
//
StubListenerNoNewVideoAvailable::StubListenerNoNewVideoAvailable(VideoCollectionClient &collectionClient) :
VideoCollectionListener(collectionClient)
{
    
}

// -----------------------------------------------------------------------------
// StubListenerNoVideoDeleted
// -----------------------------------------------------------------------------
//
StubListenerNoVideoDeleted::StubListenerNoVideoDeleted(VideoCollectionClient &collectionClient) :
VideoCollectionListener(collectionClient) 
{
    
}

// -----------------------------------------------------------------------------
// StubListenerNoVideoDeleteCompleted
// -----------------------------------------------------------------------------
//
StubListenerNoVideoDeleteCompleted::StubListenerNoVideoDeleteCompleted(VideoCollectionClient &collectionClient) :
VideoCollectionListener(collectionClient)
{
    
}

// -----------------------------------------------------------------------------
// StubListenerNoVideoDetailsCompleted
// -----------------------------------------------------------------------------
//
StubListenerNoVideoDetailsCompleted::StubListenerNoVideoDetailsCompleted(VideoCollectionClient &collectionClient) :
VideoCollectionListener(collectionClient) 
{
    
}


