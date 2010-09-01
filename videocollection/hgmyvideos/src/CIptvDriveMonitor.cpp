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
* Description: 
*
*/





// INCLUDE FILES
#include "CIptvDriveMonitor.h"
#include "MIptvDriveMonitorObserver.h"
#include "TIptvDriveMonitorEvent.h"
#include <bldvariant.hrh>
#ifdef RD_MULTIPLE_DRIVE_ENABLERS
#include <driveinfo.h>
#endif
#include <centralrepository.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
#if IPTV_LOGGING_METHOD != 0
_LIT( KIptvMediaNotPresentDes, "EMediaNotPresent");
_LIT( KIptvMediaUnknownDes, "EMediaUnknown");
_LIT( KIptvMediaFloppyDes, "EMediaFloppy");
_LIT( KIptvMediaHardDiskDes, "EMediaHardDisk");
_LIT( KIptvMediaCdRomDes, "EMediaCdRom");
_LIT( KIptvMediaRamDes, "EMediaRam");
_LIT( KIptvMediaFlashDes, "EMediaFlash");
_LIT( KIptvMediaRomDes, "EMediaRom");
_LIT( KIptvMediaRemoteDes, "EMediaRemote");
_LIT( KIptvMediaNANDFlashDes, "EMediaNANDFlash");
#endif

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::CIptvDriveMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvDriveMonitor::CIptvDriveMonitor()
: CActive( EPriorityStandard )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvDriveMonitor::CIptvDriveMonitor() start");
    IPTVLOGSTRING_HIGH_LEVEL("CIptvDriveMonitor::CIptvDriveMonitor() exit");
    }

// -----------------------------------------------------------------------------
// CIptvEvents::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::ConstructL( const RFs& aFileSession )
    {
    iFs                  = aFileSession;
    iIfsIsProvidedByUser = ETrue;
   
    ConstructL();    
    }

// -----------------------------------------------------------------------------
// CIptvEvents::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::ConstructL()
    {
    iObservers.Reset();
    iAvailableDrives.Reset();
    iPreviousAvailableDrives.Reset();
    
    if ( !iIfsIsProvidedByUser )
        {
        User::LeaveIfError( iFs.Connect() );
        }

    iSystemDrive = iFs.GetSystemDrive();
    TInt err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, iDefaultMassStorageDrive );
    if ( err != KErrNone )
        {
        iDefaultMassStorageDrive = KErrNotFound;        
        }
    err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRemovableMassStorage, iDefaultMemoryCardDrive );
    if ( err != KErrNone )
        {
        iDefaultMassStorageDrive = KErrNotFound;        
        }
            
    CActiveScheduler::Add( this );
    RefreshAvailableDrivesL();

    IPTVLOGSTRING2_HIGH_LEVEL("CIptvDriveMonitor:: PhoneMemoryDrive() returns %d",
        PhoneMemoryDrive());
    IPTVLOGSTRING2_HIGH_LEVEL("CIptvDriveMonitor:: MemoryCardDrive() returns %d",
        MemoryCardDrive());
    IPTVLOGSTRING2_HIGH_LEVEL("CIptvDriveMonitor:: MassStorageDrive() returns %d",
        MassStorageDrive());
    IPTVLOGSTRING2_HIGH_LEVEL("CIptvDriveMonitor:: FixedDrive(c-drive excluded) returns %d",
        FixedDrive( ETrue ));
    
#if IPTV_LOGGING_METHOD != 0
    PrintAvailableDrives();
#endif
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvDriveMonitor* CIptvDriveMonitor::NewL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvDriveMonitor::NewL() start");

    CIptvDriveMonitor* self = new( ELeave ) CIptvDriveMonitor();
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    IPTVLOGSTRING_HIGH_LEVEL("CIptvDriveMonitor::NewL() exit");

    return self;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvDriveMonitor* CIptvDriveMonitor::NewL( const RFs& aFileSession )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvDriveMonitor::NewL(RFs&) start");

    CIptvDriveMonitor* self = new( ELeave ) CIptvDriveMonitor();
        
    CleanupStack::PushL( self );
    self->ConstructL( aFileSession );
    CleanupStack::Pop(self);

    IPTVLOGSTRING_HIGH_LEVEL("CIptvDriveMonitor::NewL(RFs&) exit");

    return self;
    }
    
// Destructor
CIptvDriveMonitor::~CIptvDriveMonitor()
    {
    iObservers.Close();
    iAvailableDrives.Close();
    iPreviousAvailableDrives.Close();
    Cancel();
    
    if ( !iIfsIsProvidedByUser )
        {
        iFs.Close();
        }
    }


// -----------------------------------------------------------------------------
// CIptvDriveMonitor::RegisterObserverL
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::RegisterObserverL( MIptvDriveMonitorObserver* aObserver )
    {
    iObservers.AppendL( aObserver );

    if ( !IsActive() )
        {
        iFs.NotifyChange( ENotifyDisk, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::RemoveObserverL
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::RemoveObserverL( MIptvDriveMonitorObserver* aObserver )
    {
    TInt i;
    for ( i = 0; i < iObservers.Count(); i++ )
        {
        if ( aObserver == iObservers[i] )
            {
            iObservers.Remove( i );
            iObservers.Compress();
            break;
            }
        }
        
    if ( iObservers.Count() == 0 )
        {
        iFs.NotifyChangeCancel();
        }    
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::RefreshAvailableDrivesL
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::RefreshAvailableDrivesL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvDriveMonitor::RefreshAvailableDrivesL() start");
    
    TDriveList driveList;
    User::LeaveIfError( iFs.DriveList( driveList ) );

    iPreviousAvailableDrives.Reset();
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        iPreviousAvailableDrives.Append( iAvailableDrives[i] );
        }
            
    iAvailableDrives.Reset();
    
    for ( i = 0; i < driveList.Length(); i++ )
        {
        if ( driveList[i] != 0 )
            {
            TDriveInfo driveInfo;
            User::LeaveIfError( iFs.Drive( driveInfo, i ) );
            
            IPTVLOGSTRING3_LOW_LEVEL("CIptvDriveMonitor:: drive[%d]: type = %S",
                                     i,
                                     &DriveTypeDes( driveInfo.iType ));

            IPTVLOGSTRING3_LOW_LEVEL(
                "CIptvDriveMonitor::           MediaAtt flags = %x DriveAtt flags = %x",
                    driveInfo.iMediaAtt,
                    driveInfo.iDriveAtt );

#ifdef __WINSCW__
            // (2) c-drive is EMediaRam
            // (3) d-drive is EMediaHardDisk
            // (4) e-drive is EMediaHardDisk or EMediaNotPresent
            // (25) z-drive is EMediaRom
            if ( (driveInfo.iType == EMediaRam) ||                                     // c
                 ((driveInfo.iType == EMediaHardDisk) && i != EDriveD /* skip d */) || // e
                 (driveInfo.iType == EMediaNANDFlash) ||                               // nothing
                 (driveInfo.iType == EMediaNotPresent) )                               // e
#else
            // (2) c-drive is EMediaNANDFlash
            // (3) d-drive is EMediaRam
            // (4) e-drive is EMediaHardDisk or EMediaNotPresent
            // (5) f-drive is EMediaHardDisk or EMediaNotPresent
            // (25) z-drive is EMediaRom
            if ( (driveInfo.iType == EMediaHardDisk) ||  // e,f
                 (driveInfo.iType == EMediaNANDFlash) || // c
                 (driveInfo.iType == EMediaNotPresent) ) // e,f
#endif // __WINSCW__
                {                
                if ( !(driveInfo.iMediaAtt & KMediaAttWriteProtected) &&
                      (driveInfo.iDriveAtt & KDriveAttLocal) )
                    {
                    TIptvDriveInfo iptvDriveInfo;
                    TVolumeInfo volumeInfo;
                    TChar ch;

                    iFs.DriveToChar( i, ch );

                    iptvDriveInfo.iDrive     = i;
                    iptvDriveInfo.iDriveChar = ch;
                    iptvDriveInfo.iType      = driveInfo.iType;
                    
                    TUint driveStatus;
                    TInt err = DriveInfo::GetDriveStatus( iFs, i, driveStatus );
                    if ( err == KErrNone )
                        {
                        if ( driveStatus & DriveInfo::EDriveRemovable )
                            {
                            iptvDriveInfo.iFlags |= TIptvDriveInfo::ERemovable;
                            }
                        }

                    TInt massStorageDrive;
                    err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage,
                                                      massStorageDrive );
                    if ( err == KErrNone )
                        {
                        if ( i == massStorageDrive )
                            {
                            iptvDriveInfo.iFlags |= TIptvDriveInfo::EMassStorage;
                            }                        
                        }

                    if ( driveInfo.iMediaAtt & KMediaAttLocked )
                        {
                        iptvDriveInfo.iFlags |= TIptvDriveInfo::ELocked;
                        }
                    if ( driveInfo.iType == EMediaNotPresent )
                        {
                        iptvDriveInfo.iFlags |= TIptvDriveInfo::EMediaNotPresent;
                        }

                    // When mass storage mode is activated while application is running,
                    // the only hint about that is the TVolumeInfo not being available. 
                    // Therefore we mark drive as EMediaNotPresent if that structure
                    // is not available.
                    if ( iFs.Volume( volumeInfo, i ) != KErrNone )
                        {
                        iptvDriveInfo.iFlags |= TIptvDriveInfo::EMediaNotPresent;
                        iptvDriveInfo.iVolumeName.Zero();
                        }
                    else
                        {
                        iptvDriveInfo.iVolumeName = volumeInfo.iName;
                        }

                    if ( iptvDriveInfo.iFlags & TIptvDriveInfo::EMediaNotPresent )
                        {
                        iptvDriveInfo.iFlags |= TIptvDriveInfo::ERemovable;
                        }
                                            
                    iAvailableDrives.AppendL( iptvDriveInfo );
                    }
                }
            }
        } // for

    // if e and f-drives are available, then c-drive is removed from the available drives list
    if ( ArrayPos( EDriveE ) != KErrNotFound &&
         ArrayPos( EDriveF ) != KErrNotFound )
        {
        TInt cDrivePos = ArrayPos( EDriveC );
        if ( cDrivePos != KErrNotFound )
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "CIptvDriveMonitor:: e & f-drives available -> removing c from available drives");
            iAvailableDrives.Remove( cDrivePos );
            iAvailableDrives.Compress();
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvDriveMonitor::RefreshAvailableDrivesL() exit");
    }
    
// -----------------------------------------------------------------------------
// CIptvDriveMonitor::ArrayPos
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::ArrayPos( TInt aDrive )
    {
    TInt i;
    for (i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iDrive == aDrive )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::ArrayPosL
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::ArrayPosL( TInt aDrive )
    {
    TInt pos;
    User::LeaveIfError( pos = ArrayPos( aDrive ) );
    return pos;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::MemoryCardDriveL
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::MemoryCardDriveL()
    {
    TInt drive;
    User::LeaveIfError( drive = MemoryCardDrive() );
    return drive;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::MemoryCardDrive
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::MemoryCardDrive()
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iDrive == iDefaultMemoryCardDrive )
            {
            return iAvailableDrives[i].iDrive;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::MassStorageDriveL
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::MassStorageDriveL()
    {
    TInt drive;
    User::LeaveIfError( drive = MassStorageDrive() );
    return drive;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::MassStorageDrive
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::MassStorageDrive()
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iDrive == iDefaultMassStorageDrive )
            {
            return iAvailableDrives[i].iDrive;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::PhoneMemoryDriveL
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::PhoneMemoryDriveL()
    {
    TInt drive;
    User::LeaveIfError( drive = PhoneMemoryDrive() );
    return drive;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::PhoneMemoryDrive
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::PhoneMemoryDrive()
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iDrive == iSystemDrive )
            {
            return iAvailableDrives[i].iDrive;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::FixedDriveL
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::FixedDriveL( TBool aExcludeCDrive )
    {
    TInt drive;
    User::LeaveIfError( drive = FixedDrive( aExcludeCDrive ) );
    return drive;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::FixedDrive
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::FixedDrive( TBool aExcludeCDrive )
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvDriveMonitor:: iAvailableDrives[%d]", i);

        if ( !(iAvailableDrives[i].iFlags & TIptvDriveInfo::ERemovable) )
            {
            if ( iAvailableDrives[i].iDrive != EDriveC )
                {
                IPTVLOGSTRING2_LOW_LEVEL("CIptvDriveMonitor:: FixedDrive() returning drive %d", iAvailableDrives[i].iDrive);
                return iAvailableDrives[i].iDrive;
                }
            else
                {
                if ( !aExcludeCDrive )
                    {
                    IPTVLOGSTRING2_LOW_LEVEL("CIptvDriveMonitor:: FixedDrive() returning drive %d", iAvailableDrives[i].iDrive);
                    return iAvailableDrives[i].iDrive;
                    }   
                }
            }
        }
    IPTVLOGSTRING_LOW_LEVEL("CIptvDriveMonitor:: FixedDrive() fixed drive not found");
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::IsMassStorageDriveL
// -----------------------------------------------------------------------------
//
TBool CIptvDriveMonitor::IsMassStorageDriveL( TInt aDrive )
    {
    if ( aDrive == iDefaultMassStorageDrive )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::IsMemoryCardDriveL
// -----------------------------------------------------------------------------
//
TBool CIptvDriveMonitor::IsMemoryCardDriveL( TInt aDrive )
    {
    TInt pos = ArrayPosL( aDrive );
    if ( iAvailableDrives[pos].iFlags & TIptvDriveInfo::ERemovable )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::IsPresentL()
// -----------------------------------------------------------------------------
//
TBool CIptvDriveMonitor::IsPresentL( TInt aDrive )
    {
    TInt pos = ArrayPosL( aDrive );
    TUint32 flags = iAvailableDrives[pos].iFlags;
    if ( (flags & TIptvDriveInfo::ELocked) || (flags & TIptvDriveInfo::EMediaNotPresent) )
        {
        return ETrue;
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CIptvDriveMonitor::SystemDrive()
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::SystemDrive()
    {
    return iSystemDrive;
    }
    
// -----------------------------------------------------------------------------
// CIptvDriveMonitor::IsMmcInserted()
// -----------------------------------------------------------------------------
//
TBool CIptvDriveMonitor::IsMmcInserted()
    {   
    TInt drive = MemoryCardDrive();
    
    if ( drive == KErrNotFound )
        {
        return EFalse;
        }
        
    TInt arrayPos = ArrayPos( drive );
    
    if ( arrayPos == KErrNotFound )
        {
        return EFalse;
        }
    
    TUint32 flags = iAvailableDrives[arrayPos].iFlags;
    
    if ( (flags & TIptvDriveInfo::ELocked) || (flags & TIptvDriveInfo::EMediaNotPresent) )
        {
        return EFalse;
        } 
    
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// CIptvDriveMonitor::RunL()
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::RunL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvDriveMonitor::RunL() start");
    
    if ( iStatus == KErrNone )
        {        
        iFs.NotifyChange( ENotifyDisk, iStatus );
        
        TRAPD( err, RefreshAvailableDrivesL() );
        if ( err != KErrNone )
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvDriveMonitor:: RefreshAvailableDrivesL leaved with error code %d, ignoring", err);
            } 
        CompareOldAndNewAvailableDrives();
        SetActive();
        }
    else
        {
        iFs.NotifyChange( ENotifyDisk, iStatus );
        SetActive();
        }

    IPTVLOGSTRING_HIGH_LEVEL("CIptvDriveMonitor::RunL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::DoCancel()
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::DoCancel()
    {
    iFs.NotifyChangeCancel( iStatus );
    }       

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::RunError()
// -----------------------------------------------------------------------------
//
TInt CIptvDriveMonitor::RunError( TInt /* aError */ )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvDriveMonitor::RunL() leaved (should never happen).");
    return KErrNone;
    }       

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::SendEvent()
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::SendEvent( TIptvDriveMonitorEvent::TEvent aEvent,
    TInt aDrive )
    {
    IPTVLOGSTRING4_HIGH_LEVEL("CIptvDriveMonitor:: Sending event: %S(%d), drive: %d",
        &TIptvDriveMonitorEvent::EventDes( aEvent ),
        aEvent,
        aDrive );

    TIptvDriveMonitorEvent event;
        
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        event.iEvent = static_cast<TInt>(aEvent);
        event.iDrive = aDrive;
        iObservers[i]->HandleDriveMonitorEvent( event );
        }

#if IPTV_LOGGING_METHOD != 0
    PrintAvailableDrives();
#endif
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::CompareOldAndNewAvailableDrives
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::CompareOldAndNewAvailableDrives()
    {
    // Run through old drives list to detect changes
    TInt i;
    for ( i = 0; i < iPreviousAvailableDrives.Count(); i++ )
        {
        TInt pos = ArrayPos( iPreviousAvailableDrives[i].iDrive );
        if ( pos == KErrNotFound )
            {
            // drive was dismounted
            SendEvent( TIptvDriveMonitorEvent::EDriveDismounted,
                iPreviousAvailableDrives[i].iDrive );
            }
        else 
            {
            // drive still present
            if ( iPreviousAvailableDrives[i].iFlags != iAvailableDrives[pos].iFlags )
                {
                // drive was changed
                if ( (iPreviousAvailableDrives[i].iFlags & TIptvDriveInfo::EMediaNotPresent) && 
                     !(iAvailableDrives[pos].iFlags & EMediaNotPresent) )
                    {
                    SendEvent( TIptvDriveMonitorEvent::EMediaInserted,
                        iAvailableDrives[pos].iDrive );
                    }
                    
                if ( !(iPreviousAvailableDrives[i].iFlags & TIptvDriveInfo::EMediaNotPresent) && 
                     (iAvailableDrives[pos].iFlags & TIptvDriveInfo::EMediaNotPresent) )
                    {
                    SendEvent( TIptvDriveMonitorEvent::EMediaRemoved,
                        iAvailableDrives[pos].iDrive );
                    }

                if ( !(iPreviousAvailableDrives[i].iFlags & TIptvDriveInfo::ELocked) && 
                     (iAvailableDrives[pos].iFlags & TIptvDriveInfo::ELocked) )
                    {
                    SendEvent( TIptvDriveMonitorEvent::EDriveLocked,
                        iAvailableDrives[pos].iDrive );
                    }

                if ( (iPreviousAvailableDrives[i].iFlags & TIptvDriveInfo::ELocked) && 
                     !(iAvailableDrives[pos].iFlags & TIptvDriveInfo::ELocked) )
                    {
                    SendEvent( TIptvDriveMonitorEvent::EDriveUnlocked,
                        iAvailableDrives[pos].iDrive );
                    }               
                }
            // drive volume name was changed
            if ( iPreviousAvailableDrives[i].iVolumeName.Compare( 
                 iAvailableDrives[pos].iVolumeName ) != 0 )
                {
                SendEvent( TIptvDriveMonitorEvent::EDriveVolumeNameChanged, 
                    iAvailableDrives[pos].iDrive ); 
                }
            }
        }

    // Run through new drives list to detect mounted drives
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        TBool found = EFalse;
        for ( TInt j = 0; j < iPreviousAvailableDrives.Count(); j++ )
            {
            if ( iPreviousAvailableDrives[j].iDrive == iAvailableDrives[i].iDrive )
                {
                found = ETrue;
                break;
                }
            }
        if ( !found )
            {
            SendEvent( TIptvDriveMonitorEvent::EDriveMounted,
                iAvailableDrives[i].iDrive );
            }
        }
    }

// -----------------------------------------------------------------------------
// CIptvDriveMonitor::DriveType()
// -----------------------------------------------------------------------------
//
#if IPTV_LOGGING_METHOD != 0 
const TDesC& CIptvDriveMonitor::DriveTypeDes( TInt aType )
    {
    switch (aType)
        {     
        case EMediaNotPresent:
            return KIptvMediaNotPresentDes;
	      case EMediaUnknown:
            return KIptvMediaUnknownDes;
        case EMediaFloppy:
            return KIptvMediaFloppyDes;
        case EMediaHardDisk:
            return KIptvMediaHardDiskDes;
        case EMediaCdRom:
            return KIptvMediaCdRomDes;
        case EMediaRam:
            return KIptvMediaRamDes;
        case EMediaFlash:
            return KIptvMediaFlashDes;
        case EMediaRom:
            return KIptvMediaRomDes;
        case EMediaRemote:
            return KIptvMediaRemoteDes;
        case EMediaNANDFlash:
            return KIptvMediaNANDFlashDes;
        default:
            return KIptvMediaUnknownDes;
        }
    }
#else
const TDesC& CIptvDriveMonitor::DriveTypeDes( TInt /*aType*/ )
    {
    return KIptvEmptyDes;
    }
#endif
    
// -----------------------------------------------------------------------------
// CIptvDriveMonitor::PrintAvailableDrives()
// -----------------------------------------------------------------------------
//
void CIptvDriveMonitor::PrintAvailableDrives()
    {
#if IPTV_LOGGING_METHOD != 0 
    for ( TInt i = 0; i < iAvailableDrives.Count(); i++ )
        {
        IPTVLOGSTRING3_HIGH_LEVEL("CIptvDriveMonitor:: iAvailableDrives[%d].iDrive = %d",
            i, iAvailableDrives[i].iDrive);
        IPTVLOGSTRING2_HIGH_LEVEL("    ERemovableDrive  = %d",
            !!(iAvailableDrives[i].iFlags & TIptvDriveInfo::ERemovable));
        IPTVLOGSTRING2_HIGH_LEVEL("    EMassStorage     = %d",
            !!(iAvailableDrives[i].iFlags & TIptvDriveInfo::EMassStorage));
        IPTVLOGSTRING2_HIGH_LEVEL("    ELockedDrive     = %d",
            !!(iAvailableDrives[i].iFlags & TIptvDriveInfo::ELocked));
        IPTVLOGSTRING2_HIGH_LEVEL("    EMediaNotPresent = %d",
            !!(iAvailableDrives[i].iFlags & TIptvDriveInfo::EMediaNotPresent));
        }
#endif
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File  
