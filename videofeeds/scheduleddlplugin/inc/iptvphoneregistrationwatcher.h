/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    This is helper class which observes whether the phone is on*
*/




#ifndef CIPTVPHONEREGISTRATIONWATCHER_H
#define CIPTVPHONEREGISTRATIONWATCHER_H

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etelmm.h>

class MPhoneRegistrationWatcherObserver;

/**
 *  Watcher client for phone registration.
 *
 *  @lib iptvscheduleddownloadplugin.dll
 */
class CIptvPhoneRegistrationWatcher : public CActive
    {
public:

    /**
    * Two-phased constructor.        
    */
    static CIptvPhoneRegistrationWatcher* NewL();

    /**
    * Two-phased constructor.        
    */
    static CIptvPhoneRegistrationWatcher* NewLC();

    /**
    * Destructor.
    */
    virtual ~CIptvPhoneRegistrationWatcher();

    /**
    * Check if we are on home network and return ETrue if so.
    *
    * @return ETrue if we are on homenetwork, EFalse otherwise
    */
    TBool IsOnHomeNetwork();


    /**
    * Start observing the network registration status change.
    */
    void StartObserving();

    /**
     * Add the observer.
     *
     * @param aObserver The observer to add.
     */
    void AddObserverL( MPhoneRegistrationWatcherObserver* aObserver );

    /**
     * Remove the observer.
     *
     * @param aObserver The observer to remove.
     */
    void RemoveObserver( MPhoneRegistrationWatcherObserver* aObserver );

private:

    // from base class CActive

    void RunL();

    void DoCancel();

private:

    /**
     * C++ constructor
     */
    CIptvPhoneRegistrationWatcher();

 	/**
     * By default Symbian 2nd phase constructor is private.        
     */
   void ConstructL();

private: // data

    /**
     * A list of observers. Not own.
     */
    RPointerArray<MPhoneRegistrationWatcherObserver> iObservers;

    /**
     * The tel server.
     */
    RTelServer iTelServer;

    /**
     * The mobile phone
     */
    RMobilePhone iMobilePhone;

    /**
     * Stores retrieved network registration information.
     */
    RMobilePhone::TMobilePhoneRegistrationStatus iRegStatus;
    };

#endif // CIPTVPHONEREGISTRATIONWATCHER_H
