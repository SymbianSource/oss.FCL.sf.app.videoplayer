/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Test stub class
*
*/

#include <e32base.h>
#include <s32file.h>
#include <mpxmedia.h>
#include <mpxcollectionplaylist.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxuser.h>
#include <mpxcmn.h>
#include <mpxcollectionutility.h>
#include <mpxcollectiontype.h>
#include <mpxcollectionmediator.h>
#include <mpxplaylistenginedefs.h>
#include <mpxlog.h>
#include <mpxcollectionmessage.h>
#include <mpxtaskqueue.h>
#include <mpxcollectionplugin.hrh>
#include <mpxharvesterutility.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <UsbWatcherInternalPSKeys.h>
#include <usbpersonalityids.h>
#include <mpxcollectionhelperfactory.h>

#include "mpxcollectionuihelperimp_stub.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Factory method to create the collection ui helper
// ---------------------------------------------------------------------------
// 
MMPXCollectionUiHelper* CMPXCollectionHelperFactory::NewCollectionUiHelperL(const TUid& aModeId)
    {
    return CMPXCollectionUiHelperImp::NewL(aModeId);
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Private Constructor
// ---------------------------------------------------------------------------
//
CMPXCollectionUiHelperImp::CMPXCollectionUiHelperImp()
    {
    }

// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXCollectionUiHelperImp* CMPXCollectionUiHelperImp::NewL( const TUid& )
    {
    return new( ELeave ) CMPXCollectionUiHelperImp();
    }

// ---------------------------------------------------------------------------
// Virtual Destructor
// ---------------------------------------------------------------------------
//
CMPXCollectionUiHelperImp::~CMPXCollectionUiHelperImp()
    {
    }

// ---------------------------------------------------------------------------
// Open the collection in embedded mode for a media
// Cannot cleanup aObs because UPnP is using this.
// ---------------------------------------------------------------------------
//
void CMPXCollectionUiHelperImp::OpenL( const TUid& , CMPXMedia& ,
                                       MMPXCHelperEmbeddedOpenObserver* ,
                                       TInt )
    {
    }

// ---------------------------------------------------------------------------
// Frees this object
// ---------------------------------------------------------------------------
//
void CMPXCollectionUiHelperImp::Close()
    {
    }
