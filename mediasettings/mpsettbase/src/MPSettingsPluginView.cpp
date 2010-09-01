/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MPSettingsPluginView.cpp*
*/


// Version : %version: 3 %




// INCLUDE FILES
//#include    <e32base.h>
#include    <ecom.h>
#include    "MPSettingsPluginView.h"
#include    "MPSettPluginViewInterfaceUID.hrh"
#include	"mpxlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsPluginView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPSettingsPluginView* CMPSettingsPluginView::NewL(TUid aUid)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsPluginView::NewL(0x%X)"),aUid);
    return static_cast<CMPSettingsPluginView*>(REComSession::CreateImplementationL
        (aUid, _FOFF(CMPSettingsPluginView, iDtor_ID_Key)));
    }
    
// -----------------------------------------------------------------------------
// CMPSettingsPluginView::~CMPSettingsPluginView
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMPSettingsPluginView::~CMPSettingsPluginView()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsPluginView::~CMPSettingsPluginView()"));
    REComSession::FinalClose();
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

// -----------------------------------------------------------------------------
// CMPSettingsPluginView::ListImplementationsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPSettingsPluginView::ListImplementationsL(RImplInfoPtrArray& aImplInfoArray)
    {
    const TUid pluginViewInterfaceUID = {KMPSettPluginViewInterfaceUid};
    MPX_DEBUG2(_L("#MS# CMPSettingsPluginView::ListImplementationsL(0x%X)"),pluginViewInterfaceUID);
    REComSession::ListImplementationsL(pluginViewInterfaceUID, aImplInfoArray);
    }

//  End of File  
