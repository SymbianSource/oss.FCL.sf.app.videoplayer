/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for "Add/edit service list" in Settings UI.*
*/




// INCLUDE FILES
#include <aknlists.h>
#include <akntitle.h>
#include <aknPopup.h>
#include <aknViewAppUi.h>
#include <barsread.h>
#include <commdb.h>
#include <StringLoader.h>

#include "IptvEngineUids.h"
#include <vcxnssettingsview.rsg>
#include "vcxnsservicesettings.h"

#include "CIptvService.h"
#include "feedsettingsview.h"
#include "feedsettingsviewaddoreditservice.h"
#include "feedsettingsviewaddservicelistbox.h"
#include "feedsettingsviewserviceselection.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVcxNsSettingsViewAddOrEditService::CVcxNsSettingsViewAddOrEditService()
// -----------------------------------------------------------------------------
//
CVcxNsSettingsViewAddOrEditService::CVcxNsSettingsViewAddOrEditService( 
    CVcxNsSettingsViewContainer* aParent,
    CVcxNsServiceSettings& aServiceSettings )
 :  iParent( aParent ),
    iServiceSettings( aServiceSettings ),
    iServiceType( ( CIptvService::TServiceType ) 0 )
    {
    __ASSERT_DEBUG(iParent, User::Panic(KNullDesC, KErrBadHandle));
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsViewAddOrEditService::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsSettingsViewAddOrEditService* CVcxNsSettingsViewAddOrEditService::NewL(
    CVcxNsSettingsViewContainer* aParent,
    CVcxNsServiceSettings& aServiceSettings )
    {
    CVcxNsSettingsViewAddOrEditService* self = new ( ELeave ) 
        CVcxNsSettingsViewAddOrEditService( aParent, aServiceSettings );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsViewAddOrEditService::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewAddOrEditService::ConstructL()
    {
    iListbox = new (ELeave) CVcxNsSettingsViewAddServiceListbox(
            iParent, 
            iAddServiceName, 
            iAddServiceAddress,
            iAddServicePluginType );

    iListbox->SetContainerWindowL( *iParent );
    iListbox->ConstructFromResourceL( R_VCSETTINGSVIEW_ADDSERVICE_LIST );
    iListbox->SetRect(iParent->Rect());
    iListbox->ListBox()->ActivateL();
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsViewAddOrEditService::~CVcxNsSettingsViewAddOrEditService()
// -----------------------------------------------------------------------------
//
CVcxNsSettingsViewAddOrEditService::~CVcxNsSettingsViewAddOrEditService()
    {
    delete iListbox;
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsViewAddOrEditService::ActivateL()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewAddOrEditService::ActivateL()
    {
	
	iParent->SetTitleL( R_IPTV_TITLE_PANE_SETTINGS_TITLE );
	
    iListbox->ListBox()->ScrollBarFrame()->SetScrollBarVisibilityL(
                                               CEikScrollBarFrame::EOff,
                                               CEikScrollBarFrame::EAuto );
    iListbox->MakeVisible(ETrue);
    iListbox->DrawNow();
    iListbox->ListBox()->UpdateScrollBarsL();
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsViewAddOrEditService::Deactivate()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewAddOrEditService::Deactivate()
    {
    TRAP_IGNORE( iListbox->ListBox()->ScrollBarFrame()->SetScrollBarVisibilityL(
                 CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff) );

    iListbox->MakeVisible(EFalse);
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsViewAddOrEditService::LoadL()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewAddOrEditService::LoadL(
    TBool aAdding, 
    CIptvService::TServiceType aServiceType )
    {
    // The "add/edit service list" is used for both adding a new service and
    // for editing an existing service. In both cases, the list just
    // needs to be initialized with references to correct data items.

    iServiceType = aServiceType;

    if (aAdding) // Adding a new service.
        {
        HBufC* defaultName = StringLoader::LoadLC(R_IPTV_NEW_SERVICE_DEFAULT);
        iAddServiceName = *defaultName;
        iAddServiceAddress.Zero();
        CleanupStack::PopAndDestroy(defaultName);

        iListbox->LoadAddServiceInformationL(aAdding, 
                                            iAddServiceName, 
                                            iAddServiceAddress,
                                            iServiceType,
                                            iAddServicePluginType);
        }
    else // Editing an existing service.
        {
        CIptvService* iptvService = iParent->ServiceSelection()->GetCurrentServiceL();

        if (iptvService)
            {
            iAddServiceName       = iptvService->GetName();
            iAddServiceAddress    = iptvService->GetAddress();
            iPreviousAddress      = iAddServiceAddress;
            iAddServicePluginType = 0;

            delete iptvService;
            iptvService = NULL;

            iListbox->LoadAddServiceInformationL(aAdding,
                                                 iAddServiceName,
                                                 iAddServiceAddress,
                                                 iServiceType,
                                                 iAddServicePluginType);
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsViewAddOrEditService::SaveL()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewAddOrEditService::SaveL()
    {
    iListbox->SaveAddServiceInformationL();

    TUid epgUid;
    TUid iptvUid;
    TUid vodUid;

    switch (iServiceType)
        {
        case CIptvService::ELiveTv:
            break;

        case CIptvService::EVodCast: // fall through
        case CIptvService::EVod:
            {
            epgUid.iUid  = KIptvRssPluginImplementationUid;
            iptvUid.iUid = 0x00000000;
            vodUid.iUid  = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;
            }
            break;
        
        default:
            {
            epgUid.iUid  = KIptvRssPluginImplementationUid;
            iptvUid.iUid = 0x00000000;
            vodUid.iUid  = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;
            }
            break;
        }

    if (iListbox->IsAdding()) // Adding a new service.
        {
        CIptvIapList* iapList = CIptvIapList::NewL();
        CleanupStack::PushL(iapList);

        // Get IAP list for the new service.        
        iServiceSettings.GetIapListForNewServiceL(*iapList);
        // User defined services have special description until updated from EPG.
        HBufC* description = StringLoader::LoadLC(R_IPTV_USER_DEFINED_SERVICE);

        iServiceSettings.AddServiceL(iAddServiceName,
                                 iAddServiceAddress,
                                 *description,
                                 *iapList,
                                 iServiceType,
                                 epgUid,
                                 iptvUid,
                                 vodUid);

        CleanupStack::PopAndDestroy(description);
        CleanupStack::PopAndDestroy(iapList);
        }
    else // Editing an existing service.
        {
        CIptvService* iptvService = iParent->ServiceSelection()->GetCurrentServiceL();
        
        if (iptvService)
            {
            CleanupStack::PushL(iptvService);

            iptvService->SetName(iAddServiceName);
            iptvService->SetAddress(iAddServiceAddress);
            iptvService->SetEpgPluginUid(epgUid);
            iptvService->SetVodPluginUid(vodUid);
            iptvService->SetIptvPluginUid(iptvUid);
            
            if ( iPreviousAddress.Length() > 0 )
                {
                if ( iPreviousAddress.Compare( iAddServiceAddress ) != 0 )
                    {
                    // Address has been modified
                    }
                }
            iPreviousAddress.Zero();

            // Update the information of service.                
            iServiceSettings.UpdateServiceL(*iptvService);

            CleanupStack::PopAndDestroy(iptvService);
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsViewAddOrEditService::ListBox()
// -----------------------------------------------------------------------------
//
CVcxNsSettingsViewAddServiceListbox* CVcxNsSettingsViewAddOrEditService::ListBox()
    {
    __ASSERT_DEBUG(iListbox, User::Panic(KNullDesC, KErrBadHandle));

    return iListbox;    
    }
