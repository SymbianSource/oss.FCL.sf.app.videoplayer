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
* Description:    Publish & Subscribe watcher client for battery states.*
*/




#ifndef CIPTVBATTERYWATCHER_H
#define CIPTVBATTERYWATCHER_H

#include <e32base.h>
#include <e32property.h>

class MBatteryWatcherObserver;

/**
 *  Publish & Subscribe watcher client for battery states.
 *
 *  @lib iptvscheduleddownloadplugin.dll
 */
class CIptvBatteryWatcher : public CActive
    {
public:

    /**
     * Two-phased constructor.        
     */
    static CIptvBatteryWatcher* NewL();

    /**
     * Two-phased constructor.        
     */
    static CIptvBatteryWatcher* NewLC();

    /**
     * Destructor.
     */
    virtual ~CIptvBatteryWatcher();

    /**
     * Helper method that returns the battery status.
     *
     * @param aBatteryStatus The battery status.
     * @return Possible error code.
     */
    TInt GetBatteryStatus( TInt& aBatteryStatus );

    /**
     * Helper method that returns the charger status.
     *
     * @param aChargerStatus The charger status.
     * @return Possible error code.
     */
    TInt GetChargerStatus( TInt& aChargerStatus );

    /**
     * Helper method that returns ETrue if the battery is low and
     * no charger is connected or if the battery is empty.
     *
     * @return ETrue if there is no power.
     */
    TBool IsLowPower();

    /**
     * Add the observer.
     *
     * @param aObserver The observer to add.
     */
    void AddObserverL( MBatteryWatcherObserver* aObserver );

    /**
     * Remove the observer.
     *
     * @param aObserver The observer to remove.
     */
    void RemoveObserver( MBatteryWatcherObserver* aObserver );

private:    // from base class CActive

    /**
     * Battery status has been changed, inform observers if battery is low.
     */
    void RunL();

    /**
     * Cancel the battery status property watching.
     */
    void DoCancel();

private:

    /**
     * C++ constructor
     */
    CIptvBatteryWatcher();

 	/**
     * By default Symbian 2nd phase constructor is private.        
     */
   void ConstructL();

private: // data

    /**
     * The property used to watch the battery status.
     */
    RProperty iProperty;

    /**
     * A list of observers. Not own.
     */
    RPointerArray<MBatteryWatcherObserver> iObservers;
    };

#endif // CIPTVBATTERYWATCHER_H
