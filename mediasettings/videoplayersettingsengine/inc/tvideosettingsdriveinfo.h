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
* Description:   header file for TIptvDriveInfo class*
*/




#ifndef TVIDEOSETTINGDRIVEINFO_H
#define TVIDEOSETTINGDRIVEINFO_H

//  INCLUDES
#include <e32base.h>

const TInt KIptvDriveNameMaxLength = 256;

// CLASS DECLARATION
/**
* Container class for storing drive info relevant for Video Center.
*/
class TIptvDriveInfo
    {
    public:

        /*
        * Attribute flags used in iFlags member.
        */
        enum TDriveAttributes
            {
            /**
            * Media can be removed from the drive, (E or F, depending on hw configuration).
            * If not set (= 0) then the drive is fixed.
            */
            ERemovable = 1 << 0,

            /**
            * Default mass storage drive.
            */
            EMassStorage = 1 << 1,
            
            /**
            * Media is locked.
            */
            ELocked = 1 << 2,
            
            /**
            * Media is removed from the drive.
            */
            EMediaNotPresent = 1 << 3            
            };
            
        /**
        * Constructor, clears member variables to zero.
        */
        IMPORT_C TIptvDriveInfo();

        /**
        * Destructor.
        */
        IMPORT_C ~TIptvDriveInfo();
            
    public:
    
        /**
        * Drive ID number, 0 points to A:, 1 to B:, 2 to C: and so on.
        */
        TInt iDrive;

        /**
        * Drive char.
        */
        TChar iDriveChar;
        
        /**
        * Drive type, same as TDriveInfo::iType.
        */
        TInt iType;

        /**
        * Properties of the drive, see TDriveAttributes -enum for flag values.
        */
        TUint32 iFlags;
        
        /**
        * Volume name. Removable drive can contain different volumes (mmc cards). This name
        * refers to removable item (card label), not the drive. 
        */
        TBuf<KIptvDriveNameMaxLength> iVolumeName;
    };

#endif
