/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



#ifndef C_VCXNSCONTENTHARVESTERSERVICELISTENER_H
#define C_VCXNSCONTENTHARVESTERSERVICELISTENER_H

#include <e32base.h>
#include <e32property.h>
#include "videochpssubscriber.h"

class CVcxNsChPublishableData;
class CVcxNsChPublisher;

/**
 *  CVcxNsContentHarvesterServiceListener
 * 
 *  @lib vcxnscontentharvesterplugin.dll
 *  @since S60 5.0
 */
class CVcxNsContentHarvesterServiceListener : public CBase,
                                              public MCHPSObserver
    {

public:

    static CVcxNsContentHarvesterServiceListener* NewL( CVcxNsChPublisher* aPublisher,
                                                  CVcxNsChPublishableData* aPublishableData );

    virtual ~CVcxNsContentHarvesterServiceListener();
    
    
private:

    CVcxNsContentHarvesterServiceListener( CVcxNsChPublisher* aPublisher,
                                     CVcxNsChPublishableData* aPublishableData );

    void ConstructL();
    
// from MCHPSObserver
    
    void ValueChanged( const TUint32& aKey, const TInt& aValue );

private: // data

    /**
     * The property to be watched: vod service count
     */
    CVcxNsCHPSSubscriber* iVodServiceCount;
    
    /**
     * The property to be watched: live tx service count
     */
    CVcxNsCHPSSubscriber* iLiveTvServiceCount;

    
    /**
     * CVcxNsChPublisher.
     */
    CVcxNsChPublisher* iPublisher;
    
    /**
     * CVcxNsChPublishableData.
     */
    CVcxNsChPublishableData* iPublishableData;

    };

#endif // C_VCXNSCONTENTHARVESTERMPXCLIENT_H
