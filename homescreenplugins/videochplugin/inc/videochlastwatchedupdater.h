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
* Description:    CVcxNsChLastWatchedUpdater class declaration*
*/




#ifndef _VCXCHLASTWATCHEDUPDATER_H
#define _VCXCHLASTWATCHEDUPDATER_H 

// INCLUDES
#include <e32std.h>
#include <e32cmn.h>
#include <e32base.h>
#include <contentharvesterplugin.h>
#include <contentharvesterpluginuids.hrh>
#include <MediatorEventConsumer.h>

// FORWARD DECLARATIONS
class CVcxNsChMCategoryListener;
class CVcxNsChPublishableData;
class CVcxNsChPublisher;

// CLASS DECLARATION

/**
 *  Ns Content Harvester plugin
 *
 *  @lib vcxnscontentharvesterplugin.dll
 *  @since S60 v5.0
 */

class CVcxNsChLastWatchedUpdater: public CBase,
                                  public MMediatorEventObserver
    {
    
public:

    static CVcxNsChLastWatchedUpdater* NewL( CVcxNsChPublishableData* aPublishableData,
                                             CVcxNsChPublisher* aPublisher );
    
    /**
     * Destructor
     * 
     */
    virtual ~CVcxNsChLastWatchedUpdater();
    
    /**
     * From MMediatorEventObserver
     * 
     * Catches mediator events
     */
    void MediatorEventL( TUid aDomain,
                         TUid aCategory, 
                         TInt aEventId, 
                         const TDesC8& aData );   

private:
    
    void ConstructL();

    CVcxNsChLastWatchedUpdater( CVcxNsChPublishableData* aPublishableData,
                                CVcxNsChPublisher* aPublisher );
    
    /**
     *  Writes the preloaded video data to last watched
     */
    void  WritePreloadedToLastWatchedL();
   
private: // DATA
    
    /**
     * Publishable data container 
     * NOT OWN
     */
    CVcxNsChPublishableData* iPublishableData;
    
    /**
     * Publisher object used to publish data from CVcxNsChPublishableData
     * to service api 
     * NOT OWN
     */
    CVcxNsChPublisher* iPublisher;
    
    /**
     * Mediator helper object for registering and subscribing 
     * player related mediator events
     * OWN
     */
    CVcxNsChMCategoryListener* iMPlayerCategoryListener;
    
    /**
     * Mediator helper object for registering and subscribing 
     * IPTV related mediator events
     * OWN
     */
    CVcxNsChMCategoryListener* iMIPTVCategoryListener;
     
    };

#endif // _VCXCHLASTWATCHEDUPDATER_H 
