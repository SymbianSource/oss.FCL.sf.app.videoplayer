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
* Description:    CVcxNsChPublisher class declaration*
*/




#ifndef _VCXNSCHMEDIATOTCATLISTENER_H
#define _VCXNSCHMEDIATOTCATLISTENER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>


class MMediatorEventObserver;
class CMediatorEventConsumer;

// CLASS DECLARATION

/**
 * Helper class for listening mediator event
 *
 *  @lib vcxnscontentharverterplugin.dll
 */
class CVcxNsChMCategoryListener : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CVcxNsChMCategoryListener();

    /**
     * Two-phased constructor. Leaves if aObserver is NULL.
     * 
     * @param aObserver actual event listener object
     * @param aMDomain domain id to register and subscribe to
     * @param aMCategory category id to register and subscribe to
     * @param aVersion version number for events to wait for
     * 
     * @return created CVcxNsChMCategoryListener object
     * 
     */
    static CVcxNsChMCategoryListener* NewL( MMediatorEventObserver* aObserver, 
                                            TUid& aMDomain,
                                            TUid& aMCategory,
                                            TVersion& aVersion );

    /**
     * Two-phased constructor. Leaves if aObserver is NULL.
     * 
     * @param aObserver actual event listener object
     * @param aMDomain domain id to register and subscribe to
     * @param aMCategory category id to register and subscribe to
     * @param aVersion version number for events to wait for
     * 
     * @return created CVcxNsChMCategoryListener object
     * 
     */
    static CVcxNsChMCategoryListener* NewLC( MMediatorEventObserver* aObserver, 
                                             TUid& aMDomain,
                                             TUid& aMCategory,
                                             TVersion& aVersion );

private:

    /**
     * Constructor for performing 1st stage construction
     *  
     * @param aMDomain domain id to register and subscribe to
     * @param aMCategory category id to register and subscribe to
     * @param aVersion version number for events to wait for
     */
    CVcxNsChMCategoryListener( TUid& aMDomain,
                               TUid& aMCategory,
                               TVersion& aVersion );

    /**
     * EPOC default constructor for performing 2nd stage construction
     * 
     * @param aObserver actual event listener object
     */
    void ConstructL( MMediatorEventObserver* aObserver );
    
public: // new methods
    
    /**
     * method registers and subcribes given event id for domain / category / version
     * setted for this object
     * 
     * @param aEvent event to register and subscribe to
     */
    void RegisterAndSubscribeL( const TInt& aEvent );
    
private:
    
    /**
     * method unsubscribes and deregisters all events saved in iEventArray
     * 
     */
    void UnsubscribeAndDeregisterAll();
    
private: // DATA
    
    /**
     * domain id for events
     */
    TUid iMediatorDomain;
    
    /**
     * Category id for events
     */
    TUid iMediatorCat;
    
    /**
     * version of events
     */
    TVersion iMVersion;
    
    /**
     * Array of event ids 
     */
    RArray< TInt > iEventArray;
    
    /**
     * event consumer object used to subscribe 
     * events to
     */
    CMediatorEventConsumer* iEventConsumer; 

    };

#endif // _VCXNSCHMEDIATOTCATLISTENER_H
