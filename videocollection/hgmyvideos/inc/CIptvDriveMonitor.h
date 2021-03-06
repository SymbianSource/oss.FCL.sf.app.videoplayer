/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Helper class for Video Center for drive operations/monitoring.*
*/





#ifndef CIPTVDRIVEMONITOR_H
#define CIPTVDRIVEMONITOR_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "TIptvDriveInfo.h"
#include "TIptvDriveMonitorEvent.h"
#include "MIptvDriveMonitorObserver.h"
#include "IptvDebug.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Helper class for Video Center for drive operations/monitoring.
 * User can get available drives (through iAvailableDrives public member)
 * and be notified when drives change (through MIptvDriveMonitorObserver interface).
 */
NONSHARABLE_CLASS (  CIptvDriveMonitor ) : public CActive
    {                
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIptvDriveMonitor* NewL();

        /**
        * Two-phased constructor. Use this is owner object already has a file session open.
        *
        * @param aFileSession file session.
        */
        static CIptvDriveMonitor* NewL( const RFs& aFileSession );
        
        /**
        * Destructor.
        */
        virtual ~CIptvDriveMonitor();
        
    public:
        
        /**
        * Registers observer for drive events.
        * iAvailableDrives is updated automatically if there are any
        * observers registered, ie there is no need to call RefreshAvailableDrivesL
        * when observing.
        * Don't keep observers around if they do not need events. Observing should be used
        * when user needs to know immedetially about memory card insertion/removed events,
        * or drive locked/unlocked events.
        *
        * @param aObserver observer, ownership does not move.
        */
        void RegisterObserverL( MIptvDriveMonitorObserver* aObserver );
        
        /**
        * Removes observer, ownership does not move.
        * @param aObserver observer to be removed from observers list.
        */ 
        void RemoveObserverL( MIptvDriveMonitorObserver* aObserver );

        /**
        * Updates iAvailableDrives member variable from File server.
        * This is called in ConstructL() also.
        */ 
        void RefreshAvailableDrivesL();

        /**
        * Returns the position index of aDrive in iAvailableDrives.
        *
        * @param aDrive drive to search.
        * @return Position index of aDrive in iAvailableDrives. If not found,
        *         KErrNotFound is returned.
        */        
        TInt ArrayPos( TInt aDrive );

        /**
        * Returns the position index of aDrive in iAvailableDrives. If not
        * found, leaves with KErrNotFound code.
        *
        * @param aDrive drive to search.
        * @return Position index of aDrive in iAvailableDrives.
        */        
        TInt ArrayPosL( TInt aDrive );

        /**
        * Returns the default removable drive (= memory card drive) from iAvailableDrives
        * array. Leaves with KErrNotFound if not found.
        */        
        TInt MemoryCardDriveL();

        /**
        * Returns the default removable drive (= memory card drive) from iAvailableDrives
        * array. Returns KErrNotFound if not found.
        *
        * @return drive number, or KErrNotFound if not found.
        */        
        TInt MemoryCardDrive();

        /**
        * Returns the default mass memory drive if found from iAvailableDrives.
        * Mass storege drive means the drive which is preferred for storing large
        * files in the device, the drive itself can be anything, usually not c though.
        * Leaves with KErrNotFound if not found.
        *
        * @return Drive number.
        */
        TInt MassStorageDriveL();

        /**
        * Returns the default mass memory drive if found from iAvailableDrives.
        * Mass storege drive means the drive which is preferred for storing large
        * files in the device, the drive itself can be anything except c-drive.
        * Returns KErrNotFound if not found.
        *
        * @return Drive number, or KErrNotFound if not found.
        */
        TInt MassStorageDrive();

        /**
        * Returns the phone memory drive from iAvailableDrives array.
        * Leaves with KErrNotFound if not found.
        *
        * @return Drive number.
        */
        TInt PhoneMemoryDriveL();

        /**
        * Returns the PhoneMemory drive from iAvailableDrives array.
        * Returns KErrNotFound if not found.
        *
        * @return Drive number, or KErrNotFound if not found.
        */
        TInt PhoneMemoryDrive();

        /**
        * Returns the first fixed (= non removable) drive from iAvailableDrives array.
        * Leaves with KErrNotFound if not found.
        *
        * @param aExcludeCDrive If ETrue, then C-drive is excluded.
        * @return Drive number.
        */
        TInt FixedDriveL( TBool aExcludeCDrive );

        /**
        * Returns the first fixed (= non removable) drive from iAvailableDrives array.
        * Returns KErrNotFound if not found.
        *
        * @param aExcludeCDrive If ETrue, then C-drive is excluded.
        * @return Drive number, or KErrNotFound if not found.
        */
        TInt FixedDrive( TBool aExcludeCDrive );

        /**
        * @return ETrue if aDrive is the preferred mass storage drive for the large files.
        */
        TBool IsMassStorageDriveL( TInt aDrive );

        /**
        * @return ETrue if aDrive is memory card type.
        */
        TBool IsMemoryCardDriveL( TInt aDrive );

        /**
        * @return ETrue media is present in the drive. (Memory card is inserted and drive is not locked).
        */
        TBool IsPresentL( TInt aDrive );

        /**
        * @return The system drive. Might not be in iAvailableDrives.
        */
        TInt SystemDrive();
        
        /**
        * Finds out if memory card is inserted and not locked.
        *
        * @return Returns ETrue is memory card is inserted and usable (not locked), EFalse otherwise.
        */
        TBool IsMmcInserted();
        
        /**
        * Gets the used memory from cenrep. If cenrep contains invalid drive or value is not found,
        * chooses the preferred drive in the following order: 1. mass storage 2. memory card
        * 3. EDriveC. If cenrep value was not usable, the chosen drive is written to cenrep.
        * Leaves with system-wide error code if wasn't able to get the used memory.
        *
        * @param aUsedMemory The used memory drive is written here.
        */        
        void GetUsedMemoryL( TInt& aUsedMemory );


    protected: // Functions from base classes

        /**
        * From CActive, RunL.
        * Called when operation completes.
        */
        void RunL();

        /**
        * From CActive, cancels request.
        */
        void DoCancel();

        /**
        * From CActive, handles leaves from RunL.
        */
        TInt RunError( TInt aError );
         
    private:

        /**
        * C++ default constructor.
        */
        CIptvDriveMonitor();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aFileSession
        */
        void ConstructL( const RFs& aFileSession );

        /**
        * Sends event to observers.
        *
        * @param aEvent event to send.
        * @param aDrive drive which caused the event.
        */
        void SendEvent( TIptvDriveMonitorEvent::TEvent aEvent, TInt aDrive );

        /**
        * Compares iPreviousAvailableDrives and iAvailableDrives arrays and
        * sends appropriate events if they differ.
        */
        void CompareOldAndNewAvailableDrives();
        
        /**
        * Returns drive type as a descriptor. For debugging only.
        * @param aType Drive type, see valid values from e32const.h TMediaType enum.
        */
        const TDesC& DriveTypeDes( TInt aType );

        /**
        * Prints available drives. For debugging only.
        */
        void PrintAvailableDrives();

    public:
    
        /**
        * Array of currently available drives. These are updated
        * with RefreshAvailableDrivesL -method or from file
        * server event. RefreshAvailableDrivesL is also called upon creation
        * of this object.
        * iAvailableDrives array contains drives which are available for mass
        * storaging videos. If there is hard disk and memory card
        * available(both e and f-drives), then c-drive is excluded from the array.
        */
        RArray<TIptvDriveInfo> iAvailableDrives;

        
    private:

        /**
        * Used to detect if refresh had any effect.
        */
        RArray<TIptvDriveInfo> iPreviousAvailableDrives;

        /**
        * Array of observers. Pointers not owned.
        */
        RArray<MIptvDriveMonitorObserver*> iObservers;
        
        /**
        * Session to file server.
        */
        RFs iFs;
        
        /**
        * ETrue if iFs is provided by the user of this class.
        */
        TBool iIfsIsProvidedByUser;
        
        /**
        * Default memory for storing large files.
        */
        TInt iDefaultMassStorageDrive;
        
        /**
        * Default memory card.
        */
        TInt iDefaultMemoryCardDrive;
        
        /**
        * System drive.
        */
        TInt iSystemDrive;

    };

#endif      // CIPTVDRIVEMONITOR_H   
            
// End of File
