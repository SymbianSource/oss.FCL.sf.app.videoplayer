/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for "service selection list" in Settings UI.*
*/



// INCLUDE FILES
#include <aknlists.h>
#include <StringLoader.h>
#include <akntitle.h>
#include <eikclbd.h>
#include <barsread.h>
#include <aknViewAppUi.h>
#include <browserlauncher.h>
#include <browseroverriddensettings.h>
#include <uriutils.h>
#include "IptvDebug.h"
#include <AknQueryDialog.h>
#include <ipvideo/vcxconnectionutility.h>
#include <vcxnssettingsview.rsg>
#include "vcxnsservicesettings.h"

#include "CIptvService.h"
#include "feedsettingsviewremoveservicequerydialog.h"
#include "feedsettingsview.h"
#include "feedsettingsviewserviceselection.h"
#include "iptvlocalisationliterals.h"

// CONSTANTS
/* Parameter for embedded browser launch */
_LIT(KUrlLaunchParameter, "4 ");
_LIT(KServicesListItemFormat, "1\t%S");
_LIT( KLinkTagOpen, "<AknMessageQuery Link>" );
_LIT( KLinkTagClose, "</AknMessageQuery Link>" );
_LIT( KNextLine, "\n" );

const TInt KServicesListItemMaxLength = KIptvSmServicesDbNameMaxLength + 5;
const TInt KIptvMskPosition = 3;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::CVcSettingsViewServiceSelection()
// -----------------------------------------------------------------------------
//
CVcxNsSettingsViewServiceSelection::CVcxNsSettingsViewServiceSelection(
        CVcxNsSettingsViewContainer* aParent,
        CVcxNsServiceSettings& aServiceSettings )
 :  iParent(aParent),
    iServiceSettings(aServiceSettings),
    iOwnButtons(EFalse),    
    iServiceType((CIptvService::TServiceType)0)
    {
    __ASSERT_DEBUG(iParent, User::Panic(KNullDesC, KErrBadHandle));
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsSettingsViewServiceSelection* CVcxNsSettingsViewServiceSelection::NewL(
        CVcxNsSettingsViewContainer* aParent,
        CVcxNsServiceSettings& aServiceSettings )
    {
    CVcxNsSettingsViewServiceSelection* self = 
        new (ELeave) CVcxNsSettingsViewServiceSelection(aParent, aServiceSettings);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::ConstructL()
    {
    TResourceReader reader;
    iListbox = new (ELeave) CAknSingleGraphicStyleListBox();
    iParent->ControlEnv()->CreateResourceReaderLC( reader, 
                                 R_VCSETTINGSVIEW_SERVICESELECTION_LIST );
    iListbox->SetContainerWindowL(*iParent);
    iListbox->SetListBoxObserver(iParent);
    iListbox->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader
    const TInt KGranularity( 4 );
    CAknIconArray* icons = new (ELeave) CAknIconArray( KGranularity );
    CleanupStack::PushL(icons);
    icons->ConstructFromResourceL(R_IPTV_CHECKBOX_ICONS);
    CleanupStack::Pop(icons);
    iListbox->ItemDrawer()->ColumnData()->SetIconArray(icons);
    CTextListBoxModel* model = iListbox->Model();
    model->SetOwnershipType(ELbmOwnsItemArray);
    
    iListbox->CreateScrollBarFrameL(ETrue);
    iListbox->SetMopParent(iParent);    
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::~CVcSettingsViewServiceSelection()
// -----------------------------------------------------------------------------
//
CVcxNsSettingsViewServiceSelection::~CVcxNsSettingsViewServiceSelection()
    {
    if ( iOwnButtons && iParent )
        {
        CEikButtonGroupContainer* cba = iParent->SettingsView()->GetButtonGroupContainer();
        if ( cba )
            {
            RemoveCommandsFromCba( *cba );
            }
        }

    delete iListbox;
    delete iServicesArray;
    delete iServicesArrayVodCast;
    delete iLauncher;
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::ActivateL()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::ActivateL()
    {
    // Force MSK refresh after every activation
    iCurrentMskResource = KErrNotFound;

    if (iServicesArray)
        {
        if (iServicesArray->Count() > 0)
            {
            iListbox->SetCurrentItemIndex(0);
            }
        }

    iParent->SetTitleL( R_MPSETT_FEEDS_VIEW_TITLE );
    
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                                         CEikScrollBarFrame::EAuto );
    iListbox->MakeVisible(ETrue);
    iListbox->DrawNow();

    iListbox->UpdateScrollBarsL();
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::Deactivate()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::Deactivate()
    {
    if (iOwnButtons)
        {
        CEikButtonGroupContainer* cba = iParent->SettingsView()->GetButtonGroupContainer();
        if ( cba )
            {
            RemoveCommandsFromCba( *cba );
            cba->DrawNow();
            }
        iOwnButtons = EFalse;
        }

    TRAP_IGNORE(iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(
                CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff));

    // Clear msk value so that when we return 
    // to original menu we can set the correct msk.
    iParent->ClearMsk();    

    iListbox->MakeVisible(EFalse);
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::SetServiceType()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::SetServiceType(
                                         CIptvService::TServiceType aServiceType )
    {
    iServiceType = aServiceType;
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::GetServiceType()
// -----------------------------------------------------------------------------
//
CIptvService::TServiceType CVcxNsSettingsViewServiceSelection::GetServiceType()
    {
    return iServiceType;
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::GetCurrentServiceL()
// -----------------------------------------------------------------------------
//
CIptvService* CVcxNsSettingsViewServiceSelection::GetCurrentServiceL()
    {
    TInt index = iListbox->CurrentItemIndex();

    if (index != KErrNotFound)
        {
        CIptvService* iptvService = CIptvService::NewL();
        CleanupStack::PushL(iptvService);
        iptvService->SetL(iServicesArray->MdcaPoint(index));
        CleanupStack::Pop(iptvService);
        return iptvService;
        }
    else
        {
        return NULL;    
        }
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::IsCurrentServiceReadOnlyL()
// -----------------------------------------------------------------------------
//
TBool CVcxNsSettingsViewServiceSelection::IsCurrentServiceReadOnlyL()
    {
    CIptvService* service  = GetCurrentServiceL();
    TBool toReturn ( EFalse );
    
    if (service)
        {
        CleanupStack::PushL(service);
        
        TUint32 flags = service->GetFlags();
        
        if (flags & CIptvService::EReadOnly)
            {
            toReturn = ETrue;
            }

        CleanupStack::PopAndDestroy(service);        
        }

    return toReturn;
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::ShowAccountMgmtDialogL
//
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsViewServiceSelection::ShowAccountMgmtDialogL()
    {
    TInt result(0);
    
    //Get texts for the message query
    HBufC* removeServiceText = 
        StringLoader::LoadLC( R_VCSETTINGSVIEW_REMOVE_SERVICE_WITH_ACCOUNT );
    HBufC* link = StringLoader::LoadLC( R_VCSETTINGSVIEW_LINK_ACCOUNT );
    HBufC* accMgmtLink = link->ReAllocL( link->Length() +
                                         KLinkTagOpen().Length() + 
                                         KLinkTagClose().Length() );
    
    CleanupStack::Pop( link );
    link = accMgmtLink;
    CleanupStack::PushL( link );
    
    link->Des().Insert(0, KLinkTagOpen);
    link->Des().Append( KLinkTagClose );   

    HBufC* msgText = HBufC::NewLC( removeServiceText->Length() + 
                                   link->Length() + 
                                   KNextLine().Length() );
                             
                                        
    TPtr ptr( msgText->Des() );   
    ptr += *removeServiceText;        
    ptr += KNextLine;
    ptr += *link;

    TCallBack accMgmtCallback( ShowLinkL, this );
        
    // Create and show the message query dialog
    CVcxNsSettingsViewRemoveServiceQueryDialog* dlg =
      CVcxNsSettingsViewRemoveServiceQueryDialog::NewL( *msgText );
    
    // PrepareLC pushes *this to cleanup stack.
    dlg->PrepareLC( R_VCSETTINGSVIEW_REMOVE_SERVICE_QUERY );
    dlg->ButtonGroupContainer().SetCommandSetL( 
                                     R_VCSETTINGSVIEW_SOFTKEYS_REMOVE_CANCEL );
    dlg->SetLink( accMgmtCallback );
    
    result = dlg->RunLD();
    
    CleanupStack::PopAndDestroy( msgText ); // message, this
    CleanupStack::PopAndDestroy( link );
    CleanupStack::PopAndDestroy( removeServiceText );
    
    return result;    
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::ShowLinkL
// Used as a callback function in message query.
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsViewServiceSelection::ShowLinkL( TAny* ptr )
    {
    TInt error(KErrNone);
    
    CVcxNsSettingsViewServiceSelection* serviceSelection = 
        static_cast<CVcxNsSettingsViewServiceSelection*>(ptr);
    TInt index = serviceSelection->ListBox()->CurrentItemIndex();
    TBuf<KIptvSmServicesDbAccountManagementUrlMaxLength> accMgmtUri;
    if ( index >= 0 )
        {
        // Get some basic data about the service.
        CIptvService* iptvService = CIptvService::NewL();
        CleanupStack::PushL(iptvService);
        iptvService->SetL(serviceSelection->iServicesArray->MdcaPoint(index));
        accMgmtUri  = iptvService->GetAccountManagementUrl();
        CleanupStack::PopAndDestroy(iptvService);
        
        if ( accMgmtUri.Length() > 0 )
            {
            serviceSelection->OpenEmbeddedBrowserL( accMgmtUri );
            }
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::DeleteCurrentServiceL()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::DeleteCurrentServiceL()
    {
    TInt index = iListbox->CurrentItemIndex();

    if (index >= 0)
        {
        TBuf<KIptvSmServicesDbNameMaxLength> name;
        TBuf<KIptvSmServicesDbAccountManagementUrlMaxLength> accMgmtUri;
        CAknQueryDialog*                   dlg    = NULL;
        HBufC*                             text   = NULL;
        TUint32                            itemId = 0;
        TInt                               result = 0;

        // Get some basic data about the service.
        CIptvService* iptvService = CIptvService::NewL();
        CleanupStack::PushL(iptvService);
        iptvService->SetL(iServicesArray->MdcaPoint(index));
        itemId      = iptvService->GetId();
        name        = iptvService->GetName();
        accMgmtUri  = iptvService->GetAccountManagementUrl();
        CleanupStack::PopAndDestroy(iptvService);

        // Display confirmation query.
        switch ( iServiceType )
            {
            case CIptvService::EVod:
            case CIptvService::EServiceGroup:
            case CIptvService::ELiveTv:
                {
                if ( accMgmtUri.Length() > 0 )
                    {
                    result= ShowAccountMgmtDialogL();    
                    break;
                    }
                else
                    {
                    text = StringLoader::LoadLC( R_IPTV_REMOVE_SERVICE_CONFIRMATION,
                                                 name );
                    break;
                    }
                }
            case CIptvService::EVodCast:
                {
                // "Remove feed %U?"
                text = StringLoader::LoadLC(R_IPTV_REMOVE_FEED_CONFIRMATION,
                                            name );
                break;
                }

            case CIptvService::EBrowser:
                {
                text = StringLoader::LoadLC(R_IPTV_REMOVE_WEB_LINK_CONFIRMATION,
                                            name );
                break;
                }
            default:
                break;    
            }
        if (text)
            {
            dlg    = CAknQueryDialog::NewL();
            result = dlg->ExecuteLD(R_VCSETTINGSVIEW_DELETE_CONFIRMATION_QUERY, *text);
            CleanupStack::PopAndDestroy(text);
            }

        if (result == EAknSoftkeyYes || result == EAknSoftkeyOk)
            {
                
            // Create copy of currently selected items.
            const CListBoxView::CSelectionIndexArray* indices = 
                                                iListbox->SelectionIndexes();
            RArray<TInt> copyIndices;
            CleanupClosePushL(copyIndices);
            TInt err( KErrNone );
            for (TInt i = 0; i < indices->Count(); i++)
                {
                err = copyIndices.InsertInOrder((*indices)[i]);
                if ( err != KErrNone )
                    {
                    IPTVLOGSTRING2_LOW_LEVEL( 
                        "CVcSettingsViewServiceSelection::DeleteCurrentServiceL:\
                        InsertInOrder failed: %d", err );
                    
                    if ( err != KErrAlreadyExists )
                        {
                        User::Leave( err );
                        }
                    }
                }

            // Remove the item from listbox.
            CTextListBoxModel* model = iListbox->Model();
            CDesCArray* array = STATIC_CAST(CDesCArray*, model->ItemTextArray());
            array->Delete(index);

            // Remove the item from DB.
            iServiceSettings.DeleteServiceL(itemId);

            // Remove the item from iServicesArray-list.
            iServicesArray->Delete(index);

            AknListBoxUtils::HandleItemRemovalAndPositionHighlightL( iListbox, 
                                                                     index, 
                                                                     ETrue );

            // Mark the items that were marked before deletion.
            for (TInt j = 0; j < copyIndices.Count(); j++)
                {
                if (copyIndices[j] < index)
                    {
                    iListbox->View()->SelectItemL(copyIndices[j]);
                    }
                else if (copyIndices[j] > index)
                    {
                    iListbox->View()->SelectItemL(copyIndices[j] - 1);
                    }
                }
            CleanupStack::PopAndDestroy( &copyIndices );
            iListbox->DrawNow();
            iParent->DrawNow();            
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::LoadL()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::LoadL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "Settings view ## CVcSettingsViewServiceSelection::LoadL() - Enter");

    TBuf<KServicesListItemMaxLength> item;

    delete iServicesArray;
    iServicesArray = NULL;
    iServicesArray = iServiceSettings.GetServicesByTypeL(iServiceType);
    
    CTextListBoxModel* model = iListbox->Model();
    CDesCArray* array = STATIC_CAST(CDesCArray*, model->ItemTextArray());
    array->Reset();    
    
    if (iServicesArray)
        {
        CIptvService*  iptvService = CIptvService::NewL();
        CleanupStack::PushL(iptvService);

        // remove services that are inside a servicegroup from the list.
        TInt i = 0;
        while ( i < iServicesArray->MdcaCount() )
            {
            iptvService->SetL(iServicesArray->MdcaPoint(i));

            TUint32 flags = iptvService->GetFlags();
            if ( flags & CIptvService::EGroupedService )
                {
                iServicesArray->Delete( i );
                }
            else
                {
                i++;
                }
            }
        
        // Note: Item order of our iServicesArray-list matches order of 
        // iServiceSelectionList-listbox.

        for (TInt i = 0; i < iServicesArray->MdcaCount(); i++)
            {
            iptvService->SetL(iServicesArray->MdcaPoint(i));
            
            if ( iptvService->GetName().Match( KQtnIptvNewServiceMain ) == 0 )
                {
                // "qtn_iptv_new_service_main" is replaced to localised text.
                HBufC* addServicesLocalised = 
                                StringLoader::LoadLC( R_IPTV_NEW_SERVICE_MAIN );
                item.Format( KServicesListItemFormat, addServicesLocalised );
                CleanupStack::PopAndDestroy( addServicesLocalised );
                }
            else if ( iptvService->GetName().Match( KQtnIptvVideoDirectoryList ) == 0 )
                {
                // "qtn_iptv_video_directory_list" is replaced to localised text.
                HBufC* videoDirectoryLocalised = 
                            StringLoader::LoadLC( R_IPTV_VIDEO_DIRECTORY_LIST );
                item.Format( KServicesListItemFormat, videoDirectoryLocalised );
                CleanupStack::PopAndDestroy( videoDirectoryLocalised );
                }
            else 
                {
                TPtrC16 name( iptvService->GetName() );
                item.Format( KServicesListItemFormat, &name );
                }
        
            array->AppendL( item );

            TUint32 flags = iptvService->GetFlags();
            if (flags & CIptvService::ESelected)
                {
                iListbox->View()->SelectItemL(i);
                }
            else
                {
                iListbox->View()->DeselectItem(i);
                }
            }
           
        CleanupStack::PopAndDestroy(iptvService);
        }

    // Update empty text.
    HBufC* emptyText = NULL;
    switch (iServiceType)
        {
        case CIptvService::EVod:
        case CIptvService::EServiceGroup:
        case CIptvService::ELiveTv:
            {
            // "No services."
            emptyText = StringLoader::LoadLC(R_IPTV_NO_VIDEO_SERVICES);
            break;
            }
        case CIptvService::EVodCast:
            {
            // "No feeds."
            emptyText = StringLoader::LoadLC(R_IPTV_NO_FEEDS_SELECTED);
            break;
            }
        case CIptvService::EBrowser:
            {
            // "No links."
            emptyText = StringLoader::LoadLC(R_IPTV_NO_WEB_LINKS);
            break;
            }
        default:
            break;    
        }
    if (emptyText)
        {
        iListbox->View()->SetListEmptyTextL(*emptyText);
        CleanupStack::PopAndDestroy(emptyText);    
        }

    iListbox->HandleItemAdditionL();
    iListbox->DrawNow();
    iParent->DrawNow();

    IPTVLOGSTRING_LOW_LEVEL(
        "Settings view ## CVcSettingsViewServiceSelection::LoadL() - Exit");
    }

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::SaveL()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::SaveL()
    {
    const CListBoxView::CSelectionIndexArray* indices = iListbox->SelectionIndexes();
    CTextListBoxModel*                        model   = iListbox->Model();

    // We go through each item in list. Those that are in indices
    // list are updated as selected, and those that are not are
    // updated as not selected.
    for (TInt i = 0; i < model->NumberOfItems(); i++)
        {
        TBool bSelected ( EFalse );
        
        for (TInt j = 0; j < indices->Count(); j++)
            {
            // Check if currently processed item is selected.
            if (i == ((*indices)[j]))
                {
                bSelected = ETrue;
                break;
                }
            }

        // Update the information for currently processed item.
        CIptvService* iptvService = CIptvService::NewL();
        CleanupStack::PushL(iptvService);
        iptvService->SetL(iServicesArray->MdcaPoint(i));
        
        TUint32 flags    = iptvService->GetFlags();
        TUint32 oldFlags = flags;
        
        if (bSelected)
            {
            flags |= CIptvService::ESelected;                
            }
        else
            {
            flags &= ~CIptvService::ESelected;
            }

        if (flags != oldFlags)
            {
            iptvService->SetFlags(flags);
            iServiceSettings.UpdateServiceL(*iptvService);
            }

        CleanupStack::PopAndDestroy(iptvService);
        }
    }
    
// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::ListBox()
// -----------------------------------------------------------------------------
//
CAknSingleGraphicStyleListBox* CVcxNsSettingsViewServiceSelection::ListBox()
    {
    __ASSERT_DEBUG(iListbox, User::Panic(KNullDesC, KErrBadHandle));

    return iListbox;
    }    

// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::GetServiceIdL
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::GetServiceIdL( 
    TUint32& aServiceId )
    {
    TInt currentItem = iListbox->CurrentItemIndex();

    // Make sure that view is not empty.
    if ( currentItem >= 0 && currentItem < iServicesArray->Count() )
        {
        CIptvService* iptvService = CIptvService::NewL();
        CleanupStack::PushL( iptvService );
        
        iptvService->SetL( iServicesArray->MdcaPoint( currentItem ) );
        aServiceId = iptvService->GetId();
        
        CleanupStack::PopAndDestroy( iptvService );
        }
    }
    
// -----------------------------------------------------------------------------
// CVcSettingsViewServiceSelection::CheckMsk()
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::CheckMsk( )
    {
    TInt resourceId( 0 );
    
    if ( ! iParent ||
         ! iParent->SettingsView() ||
         ! iParent->SettingsView()->GetButtonGroupContainer() )
        {
        return;
        }
    
    if ( !iListbox || !iListbox->Model() ||
         iListbox->Model()->NumberOfItems() < 1 )
        {
        resourceId = R_VCSETTINGSVIEW_MSK_EMPTY;
        }
    else if ( IsCurrentListItemMarked() )
        {
        resourceId = R_VCSETTINGSVIEW_SERVICE_SELECTION_MSK_BUTTON_UNMARK;
        }
    else
        {
        resourceId = R_VCSETTINGSVIEW_SERVICE_SELECTION_MSK_BUTTON_MARK;
        }

    if ( resourceId )
        {
        iCurrentMskResource = resourceId;

        CEikButtonGroupContainer* cba =
                        iParent->SettingsView()->GetButtonGroupContainer();

        TRAPD( err, cba->SetCommandL( KIptvMskPosition, iCurrentMskResource ) );
        if ( err == KErrNone )
            {                            
            cba->DrawDeferred();            
            }
        }
    }
    
// ---------------------------------------------------------
// CVcSettingsViewServiceSelection::IsCurrentListItemMarked()
// ---------------------------------------------------------
//
TBool CVcxNsSettingsViewServiceSelection::IsCurrentListItemMarked()
    {	
	TInt currentIndex = iListbox->View()->CurrentItemIndex();
	if ( currentIndex >= 0 )
	    {
	    return iListbox->View()->ItemIsSelected( currentIndex );
	    }
	else
	    {
	    return EFalse;
	    }
    }

// -----------------------------------------------------------------------------
// Open browser in embedded mode with specified uri
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::OpenEmbeddedBrowserL( const TDesC& aUri )
    {
    if ( aUri.Length() > 0 )
        {
        CUri8* inetUri = NULL;
        inetUri = UriUtils::CreateUriL( aUri );
        CleanupStack::PushL( inetUri );

        HBufC8* parameter = HBufC8::NewLC( inetUri->Uri().UriDes().Length() ); 
        parameter->Des().Format( _L8( "%S" ), &inetUri->Uri().UriDes() );

        HBufC* parsed = HBufC::NewLC( parameter->Length() );                 
        parsed->Des().Copy(*parameter);
                         
        if ( !iLauncher )
            {
            iLauncher = CBrowserLauncher::NewL();
            } 

        HBufC* urlToLaunch = HBufC::NewLC( 
            parsed->Length() + KUrlLaunchParameter.iTypeLength );
        urlToLaunch->Des().Append( KUrlLaunchParameter );
        urlToLaunch->Des().Append( *parsed );
        
#if defined(__WINSCW__)
        //custom ap overriden settings cannot be used in emulator environment
        iLauncher->LaunchBrowserEmbeddedL( *urlToLaunch, NULL, this );
#else
        iConnUtil = CVcxConnectionUtility::InstanceL();
        TUint32 iap( 0 );
        User::LeaveIfError( iConnUtil->GetIap( iap, ETrue ) );

        TBrowserOverriddenSettings overriddenSettings;
        overriddenSettings.SetBrowserSetting( EBrowserOverSettingsCustomAp, iap );
                
        iLauncher->LaunchBrowserEmbeddedL( *urlToLaunch, NULL, this, &overriddenSettings );
#endif

        CleanupStack::PopAndDestroy( urlToLaunch );
        CleanupStack::PopAndDestroy( parsed );
        CleanupStack::PopAndDestroy( parameter );
        CleanupStack::PopAndDestroy( inetUri );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsViewServiceSelection::RemoveCommandsFromCba( CEikButtonGroupContainer& aCba )
    {
    TInt nPos1 = aCba.PositionById( EAknSoftkeyOptions );
    if ( nPos1 != KErrNotFound )
        {
        aCba.RemoveCommandObserver( nPos1 ); 
        aCba.RemoveCommandFromStack( nPos1, EAknSoftkeyOptions );
        }
    TInt nPos2 = aCba.PositionById( EVcSettingsViewSoftkeyDone );
    if ( nPos2 != KErrNotFound )
        {
        aCba.RemoveCommandObserver( nPos2 );
        aCba.RemoveCommandFromStack( nPos2, EVcSettingsViewSoftkeyDone );
        }
    }
