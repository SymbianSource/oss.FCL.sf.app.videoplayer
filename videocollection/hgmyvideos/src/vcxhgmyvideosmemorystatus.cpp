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
* Description:      Common model class for component*
*/




// INCLUDE FILES
#include <aknmessagequerydialog.h>
#include <StringLoader.h>
#include "CIptvDriveMonitor.h"
#include <vcxhgmyvideos.rsg>
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideosmemorystatus.h"

// CONSTANTS
_LIT( KEnterString, "\n" );
const TInt KVcxHgExtraSpace ( 20 );
const TInt KVcxHgVideoSizeGB ( 0x40000000 );
const TInt KVcxHgVideoSizeHalfGB ( 0x20000000 );
const TInt KVcxHgVideoSizeMB ( 0x100000 );
const TInt KVcxHgVideoSizeHalfMB ( 0x80000 );
const TInt KVcxHgVideoSizeKB ( 0x400 );
const TInt KVcxHgVideoSizeHalfKB ( 0x200 );
const TUint64 KVcxHgFullPercentage ( 100 );

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// TVcxHgMyVideosMemoryStatusDialog::TVcxHgMyVideosMemoryStatusDialog()
// ---------------------------------------------------------------------------
//
TVcxHgMyVideosMemoryStatusDialog::TVcxHgMyVideosMemoryStatusDialog()
    {
    }

// ---------------------------------------------------------------------------
// TVcxHgMyVideosMemoryStatusDialog::ShowMemoryStatusDialogL()
// ---------------------------------------------------------------------------
//
void TVcxHgMyVideosMemoryStatusDialog::ShowMemoryStatusDialogL( CVcxHgMyVideosModel& aModel )
    {
    HBufC* strFreeMemory = NULL;
    HBufC* strUsedMemory = NULL;
    TUint64 dispSize = 0;
    TUint64 driveTotalSize = 0;
    TUint64 driveFreeSize = 0;
    TUint64 driveUsedSize = 0;
    TInt freePercentage = 0;
    TInt usedPercentage = 0;
    TVolumeInfo volumeInfo;

    for ( TInt i = 0; i < aModel.DriveMonitorL().iAvailableDrives.Count(); i++ )
            {
            if ( aModel.FileServerSessionL().Volume( 
                     volumeInfo, aModel.DriveMonitorL().iAvailableDrives[i].iDrive )
                 == KErrNone )
                {
                driveFreeSize += volumeInfo.iFree;
                driveTotalSize += volumeInfo.iSize;
                }
            }

    if ( !driveTotalSize )
        {
        freePercentage = 0;
        }
    else
        {
        freePercentage = driveFreeSize * KVcxHgFullPercentage / driveTotalSize;
        }
    
    usedPercentage = KVcxHgFullPercentage - freePercentage;
    driveUsedSize = driveTotalSize - driveFreeSize;

    HBufC* strFreePercentage = StringLoader::LoadLC( R_VCXHGMYVIDEOS_MEMORY_PERCENTAGE,
                                   ( freePercentage ) );
    HBufC* strUsedPercentage = StringLoader::LoadLC( R_VCXHGMYVIDEOS_MEMORY_PERCENTAGE, 
                                   ( usedPercentage ) );

    HBufC* FreeMemoryTitle = StringLoader::LoadLC( R_VCXHGMYVIDEOS_FREE_MEMORY );    
    
    if ( driveFreeSize >= KVcxHgVideoSizeGB )
        {
        dispSize = driveFreeSize + KVcxHgVideoSizeHalfGB;
        dispSize /= KVcxHgVideoSizeGB;
        strFreeMemory = StringLoader::LoadLC( R_VCXHGMYVIDEOS_SIZE_GB_SHORT,
                                                    static_cast<TUint32>( dispSize ) );        
        }
    
    else if ( driveFreeSize >= KVcxHgVideoSizeMB )
        {
        dispSize = driveFreeSize + KVcxHgVideoSizeHalfMB;
        dispSize /= KVcxHgVideoSizeMB;
        strFreeMemory = StringLoader::LoadLC( R_VCXHGMYVIDEOS_SIZE_MB_SHORT,
                                                    static_cast<TUint32>( dispSize ) );        
        }
    
    else
        {
        dispSize = driveFreeSize + KVcxHgVideoSizeHalfKB;
        dispSize /= KVcxHgVideoSizeKB;
        strFreeMemory = StringLoader::LoadLC( R_VCXHGMYVIDEOS_SIZE_KB_SHORT,
                                                    static_cast<TUint32>( dispSize ) );        
        }
    
    HBufC* UsedMemoryTitle = StringLoader::LoadLC( R_VCXHGMYVIDEOS_USED_MEMORY );

    if ( driveUsedSize >= KVcxHgVideoSizeGB )
        {
        dispSize = driveUsedSize + KVcxHgVideoSizeHalfGB;
        dispSize /= KVcxHgVideoSizeGB;
        strUsedMemory = StringLoader::LoadLC( R_VCXHGMYVIDEOS_SIZE_GB_SHORT,
                                                    static_cast<TUint32>( dispSize ) );        
        }
    
    else if ( driveUsedSize >= KVcxHgVideoSizeMB )
        {
        dispSize = driveUsedSize + KVcxHgVideoSizeHalfMB;
        dispSize /= KVcxHgVideoSizeMB;
        strUsedMemory = StringLoader::LoadLC( R_VCXHGMYVIDEOS_SIZE_MB_SHORT,
                                                    static_cast<TUint32>( dispSize ) );        
        }
    
    else
        {
        dispSize = driveUsedSize + KVcxHgVideoSizeHalfKB;
        dispSize /= KVcxHgVideoSizeKB;
        strUsedMemory = StringLoader::LoadLC( R_VCXHGMYVIDEOS_SIZE_KB_SHORT,
                                                    static_cast<TUint32>( dispSize ) );        
        }    
    
    HBufC* context = HBufC::NewLC(FreeMemoryTitle->Length() +
                                  strFreePercentage->Length() +
                                  strFreeMemory->Length() +
                                  UsedMemoryTitle->Length() +
                                  strUsedPercentage->Length() +
                                  strUsedMemory->Length() +
                                  KVcxHgExtraSpace );
    
    context->Des().Append( *FreeMemoryTitle );
    context->Des().Append( KEnterString );
    context->Des().Append( *strFreePercentage);
    context->Des().Append( KEnterString );
    context->Des().Append( *strFreeMemory );
    context->Des().Append( KEnterString );
    context->Des().Append( *UsedMemoryTitle );
    context->Des().Append( KEnterString );
    context->Des().Append( *strUsedPercentage );
    context->Des().Append( KEnterString );
    context->Des().Append( *strUsedMemory );
                               
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *context );
    dlg->ExecuteLD(R_VCXHGMYVIDEOS_MEMORY_STATUS_MESSAGE_QUERY);

    CleanupStack::PopAndDestroy( context );
    CleanupStack::PopAndDestroy( strUsedMemory );
    CleanupStack::PopAndDestroy( UsedMemoryTitle );
    CleanupStack::PopAndDestroy( strFreeMemory );
    CleanupStack::PopAndDestroy( FreeMemoryTitle );
    CleanupStack::PopAndDestroy( strUsedPercentage );
    CleanupStack::PopAndDestroy( strFreePercentage );
    }
