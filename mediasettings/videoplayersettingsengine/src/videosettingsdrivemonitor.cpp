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
* Description: 
*
*/




// INCLUDE FILES
#include "videosettingsdrivemonitor.h"
#include "mvideosettingsdrivemonitorobserver.h"
#include "tvideosettingsdrivemonitorevent.h"
#include "tvideosettingsdriveinfo.h"

#include <bldvariant.hrh>
#ifdef RD_MULTIPLE_DRIVE_ENABLERS
#include <driveinfo.h>
#endif

#include <centralrepository.h>
#include "videoplayercenrepkeys.h"

#include "mpxlog.h"

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

_LIT( KIptvEmptyDes, "" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::CVideoSettingsDriveMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVideoSettingsDriveMonitor::CVideoSettingsDriveMonitor()
: CActive( EPriorityStandard )
    {
    MPX_FUNC("CIptvDriveMonitor::CIptvDriveMonitor()");
    }

// -----------------------------------------------------------------------------
// CIptvEvents::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVideoSettingsDriveMonitor::ConstructL( const RFs& aFileSession )
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
void CVideoSettingsDriveMonitor::ConstructL()
    {
    iObservers.Reset();
    iAvailableDrives.Reset();
    iPreviousAvailableDrives.Reset();
    
    if ( !iIfsIsProvidedByUser )
        {
        User::LeaveIfError( iFs.Connect() );
        }
        
    CActiveScheduler::Add( this );
    RefreshAvailableDrivesL();

    MPX_DEBUG2("CIptvDriveMonitor:: PhoneMemoryDrive() returns %d",
        PhoneMemoryDrive());
    MPX_DEBUG2("CIptvDriveMonitor:: MemoryCardDrive() returns %d",
        MemoryCardDrive());
    MPX_DEBUG2("CIptvDriveMonitor:: MassStorageDrive() returns %d",
        MassStorageDrive());
    MPX_DEBUG2("CIptvDriveMonitor:: FixedDrive(c-drive excluded) returns %d",
        FixedDrive( ETrue ));
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVideoSettingsDriveMonitor* CVideoSettingsDriveMonitor::NewL()
    {
    MPX_FUNC("CIptvDriveMonitor::NewL() start");

    CVideoSettingsDriveMonitor* self = new( ELeave ) CVideoSettingsDriveMonitor();
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVideoSettingsDriveMonitor* CVideoSettingsDriveMonitor::NewL( const RFs& aFileSession )
    {
    MPX_FUNC("CIptvDriveMonitor::NewL(RFs&) start");

    CVideoSettingsDriveMonitor* self = new( ELeave ) CVideoSettingsDriveMonitor();
        
    CleanupStack::PushL( self );
    self->ConstructL( aFileSession );
    CleanupStack::Pop(self);

    return self;
    }
    
// Destructor
EXPORT_C CVideoSettingsDriveMonitor::~CVideoSettingsDriveMonitor()
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
// CVideoSettingsDriveMonitor::RegisterObserverL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVideoSettingsDriveMonitor::RegisterObserverL( MIptvDriveMonitorObserver* aObserver )
    {
    iObservers.AppendL( aObserver );

    if ( !IsActive() )
        {
        iFs.NotifyChange( ENotifyDisk, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::RemoveObserverL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVideoSettingsDriveMonitor::RemoveObserverL( MIptvDriveMonitorObserver* aObserver )
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
// CVideoSettingsDriveMonitor::RefreshAvailableDrivesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVideoSettingsDriveMonitor::RefreshAvailableDrivesL()
    {
    MPX_FUNC("CIptvDriveMonitor::RefreshAvailableDrivesL() start");
    
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
        if ( driveList[i] != 0 && i != EDriveY ) // Some phones have a 1MB small flash drive at y-drive, rule it out
            {
            TDriveInfo driveInfo;
            User::LeaveIfError( iFs.Drive( driveInfo, i ) );
            
            /*
            From e32cost.h:
            const TUint KMediaAttVariableSize   = 0x01;
            const TUint KMediaAttDualDensity    = 0x02;
            const TUint KMediaAttFormattable    = 0x04;
            const TUint KMediaAttWriteProtected = 0x08;
            const TUint KMediaAttLockable       = 0x10;
            const TUint KMediaAttLocked         = 0x20;
            const TUint KMediaAttHasPassword    = 0x40;
            const TUint KMediaAttReadWhileWrite = 0x80;
            const TUint KMediaAttDeleteNotify   = 0x100;
            
            The drive attributes.    
            
            const TUint KDriveAttLocal          = 0x01;
            const TUint KDriveAttRom            = 0x02;
            const TUint KDriveAttRedirected     = 0x04;
            const TUint KDriveAttSubsted        = 0x08;
            const TUint KDriveAttInternal       = 0x10;
            const TUint KDriveAttRemovable      = 0x20;
            const TUint KDriveAttRemote         = 0x40;
            const TUint KDriveAttTransaction    = 0x80;
            */

            MPX_DEBUG3("CIptvDriveMonitor:: drive[%d]: type = %S",
                                     i,
                                     &DriveTypeDes( driveInfo.iType ));

            MPX_DEBUG3(
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


#ifdef RD_MULTIPLE_DRIVE_ENABLERS

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

#else

                    if ( driveInfo.iDriveAtt & KDriveAttRemovable )
                        {
                        if ( driveInfo.iMediaAtt & KMediaAttLockable )
                            {
                            //memory card has KDriveAttRemovable and KMediaAttLockable flags set
                            iptvDriveInfo.iFlags |= TIptvDriveInfo::ERemovable;
                            }
                        else
                            {
                            //mass storage has KDriveAttRemovable set and KMediaAttLockable not set
                            iptvDriveInfo.iFlags |= TIptvDriveInfo::EMassStorage;
                            }
                        }

#endif // RD_MULTIPLE_DRIVE_ENABLERS

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
            MPX_DEBUG1(
                "CIptvDriveMonitor:: e & f-drives available -> removing c from available drives");
            iAvailableDrives.Remove( cDrivePos );
            iAvailableDrives.Compress();
            }
        }
    
    }
    
// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::ArrayPos
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::ArrayPos( TInt aDrive )
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
// CVideoSettingsDriveMonitor::ArrayPosL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::ArrayPosL( TInt aDrive )
    {
    TInt pos;
    User::LeaveIfError( pos = ArrayPos( aDrive ) );
    return pos;
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::FindMemoryCardDriveL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::MemoryCardDriveL()
    {
    TInt drive;
    User::LeaveIfError( drive = MemoryCardDrive() );
    return drive;
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::MemoryCardDrive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::MemoryCardDrive()
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iFlags & TIptvDriveInfo::ERemovable )
            {
            return iAvailableDrives[i].iDrive;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::MassStorageDriveL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::MassStorageDriveL()
    {
    TInt drive;
    User::LeaveIfError( drive = MassStorageDrive() );
    return drive;
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::MassStorageDrive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::MassStorageDrive()
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iFlags & TIptvDriveInfo::EMassStorage )
            {
            return iAvailableDrives[i].iDrive;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::PhoneMemoryDriveL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::PhoneMemoryDriveL()
    {
    TInt drive;
    User::LeaveIfError( drive = PhoneMemoryDrive() );
    return drive;
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::PhoneMemoryDrive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::PhoneMemoryDrive()
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        if ( iAvailableDrives[i].iDrive == EDriveC )
            {
            return iAvailableDrives[i].iDrive;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::FixedDriveL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::FixedDriveL( TBool aExcludeCDrive )
    {
    TInt drive;
    User::LeaveIfError( drive = FixedDrive( aExcludeCDrive ) );
    return drive;
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::FixedDrive
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVideoSettingsDriveMonitor::FixedDrive( TBool aExcludeCDrive )
    {
    TInt i;
    for ( i = 0; i < iAvailableDrives.Count(); i++ )
        {
        MPX_DEBUG2("CIptvDriveMonitor:: iAvailableDrives[%d]", i);

        if ( !(iAvailableDrives[i].iFlags & TIptvDriveInfo::ERemovable) )
            {
            if ( iAvailableDrives[i].iDrive != EDriveC )
                {
                MPX_DEBUG2("CIptvDriveMonitor:: FixedDrive() returning drive %d", iAvailableDrives[i].iDrive);
                return iAvailableDrives[i].iDrive;
                }
            else
                {
                if ( !aExcludeCDrive )
                    {
                    MPX_DEBUG2("CIptvDriveMonitor:: FixedDrive() returning drive %d", iAvailableDrives[i].iDrive);
                    return iAvailableDrives[i].iDrive;
                    }   
                }
            }
        }
    return KErrNotFound;
    }
    
// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::IsMmcInserted()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVideoSettingsDriveMonitor::IsMmcInserted()
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
// CVideoSettingsDriveMonitor::RunL()
// -----------------------------------------------------------------------------
//
void CVideoSettingsDriveMonitor::RunL()
    {
    MPX_FUNC("CIptvDriveMonitor::RunL() start");
    
    if ( iStatus == KErrNone )
        {        
        iFs.NotifyChange( ENotifyDisk, iStatus );
        
        TRAPD( err, RefreshAvailableDrivesL() );
        if ( err != KErrNone )
            {
            MPX_DEBUG2("CIptvDriveMonitor:: RefreshAvailableDrivesL leaved with error code %d, ignoring", err);
            } 
        CompareOldAndNewAvailableDrives();
        SetActive();
        }
    else
        {
        iFs.NotifyChange( ENotifyDisk, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::DoCancel()
// -----------------------------------------------------------------------------
//
void CVideoSettingsDriveMonitor::DoCancel()
    {
    iFs.NotifyChangeCancel( iStatus );
    }       

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::RunError()
// -----------------------------------------------------------------------------
//
TInt CVideoSettingsDriveMonitor::RunError( TInt /* aError */ )
    {
    MPX_FUNC("CIptvDriveMonitor::RunL() leaved (should never happen).");
    return KErrNone;
    }       

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::SendEvent()
// -----------------------------------------------------------------------------
//
void CVideoSettingsDriveMonitor::SendEvent( TIptvDriveMonitorEvent::TEvent aEvent,
    TInt aDrive )
    {
    MPX_DEBUG4("CIptvDriveMonitor:: Sending event: %S(%d), drive: %d",
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
// CVideoSettingsDriveMonitor::CompareOldAndNewAvailableDrives
// -----------------------------------------------------------------------------
//
void CVideoSettingsDriveMonitor::CompareOldAndNewAvailableDrives()
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
// CVideoSettingsDriveMonitor::DriveType()
// -----------------------------------------------------------------------------
//
#if IPTV_LOGGING_METHOD != 0 
const TDesC& CVideoSettingsDriveMonitor::DriveTypeDes( TInt aType )
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
const TDesC& CVideoSettingsDriveMonitor::DriveTypeDes( TInt /*aType*/ )
    {
    return KIptvEmptyDes;
    }
#endif
    
// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::PrintAvailableDrives()
// -----------------------------------------------------------------------------
//
void CVideoSettingsDriveMonitor::PrintAvailableDrives()
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

// -----------------------------------------------------------------------------
// CVideoSettingsDriveMonitor::GetUsedMemoryL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVideoSettingsDriveMonitor::GetUsedMemoryL( TInt& aUsedMemory )
    {
    MPX_FUNC("CIptvDriveMonitor::GetUsedMemoryL() start");
    
    TInt cenRepMemory = KErrNotFound; //init to some non valid drive value, in case that cenrep wont change it at all
    
    CRepository* cenRep = CRepository::NewL( KIptvCenRepUid );
    CleanupStack::PushL( cenRep ); // 1->

    TInt cenRepError = cenRep->Get( KIptvCenRepPreferredMemoryKey, cenRepMemory );
                
    aUsedMemory = EDriveC;
        
    if ( ArrayPos( cenRepMemory ) == KErrNotFound
        || cenRepError != KErrNone )
        {
        MPX_DEBUG1(
            "CIptvDriveMonitor:: cenrep value for used memory was not valid or not found.");

        // Non existing drive or no cen rep value -> figure out the default drive
        TInt massStorageDrive = MassStorageDrive();
        if ( massStorageDrive != KErrNotFound )
            {
            MPX_DEBUG1("CIptvDriveMonitor:: using mass storage");
            aUsedMemory = massStorageDrive;
            }
        else
            {    
            TInt memoryCardDrive  = MemoryCardDrive();
            if ( memoryCardDrive != KErrNotFound )
                {
                MPX_DEBUG1("CIptvDriveMonitor:: using memory card");
                aUsedMemory = memoryCardDrive;
                }
            }

        MPX_DEBUG2(
            "CIptvDriveMonitor:: writing new value to cenrep (%d)", aUsedMemory);

        cenRep->Set( KIptvCenRepPreferredMemoryKey, aUsedMemory );
        }
    else
        {
        //Cenrep value was valid, use it
        MPX_DEBUG1(
            "CIptvDriveMonitor:: cenrep value for used memory was valid, using it.");
        aUsedMemory = cenRepMemory;
        }

    CleanupStack::PopAndDestroy( cenRep ); // <-1
            
    MPX_DEBUG2("CIptvDriveMonitor:: using drive %d", aUsedMemory);
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File  
