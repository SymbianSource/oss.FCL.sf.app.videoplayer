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
* Description: 
*
*/


// Version : %version: 3.1.1 %




// INCLUDE FILES
#include <eikappui.h>
#include <akntextsettingpage.h>
#include <barsread.h>

#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmapplicationsettingsui.h>

#include <commdb.h>             // CMDBSession
#include <commsdattypesv1_1.h>  // CCDWAPIPBearerRecord

#include "mediasettings.hrh"
#include "MPSettingsStreamingSettingItemList.h"
#include "MPSettingsUDPPortSettingItem.h"
#include "MPSettingsModelForROP.h"
#include "mpxlog.h"

// CONSTANTS
const TInt KMPSettProxyHostListItemId  = 4;
const TInt KMPSettProxyPortListItemId  = 5;
const TInt KMPSettProxyHostMaxLength   = 1000;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::CMPSettingsStreamingSettingItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsStreamingSettingItemList::CMPSettingsStreamingSettingItemList(
        CMPSettingsModelForROP* aModel ) 
	: iModel( aModel), 
	  iProxyHostPtr(NULL, 0)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsStreamingSettingItemList::CMPSettingsStreamingSettingItemList()"));
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::ConstructFromResourceL(TInt aResourceId) 
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingSettingItemList::ConstructFromResourceL(0x%X)"),aResourceId);
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader, aResourceId);
	ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::ConstructFromResourceL(TResourceReader& aReader) 
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsStreamingSettingItemList::ConstructFromResourceL()"));
    LoadAllL();
    CAknSettingItemList::ConstructFromResourceL(aReader);
    SetItemVisibilityL();
    }
  
// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::~CMPSettingsStreamingSettingItemList
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsStreamingSettingItemList::~CMPSettingsStreamingSettingItemList()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsStreamingSettingItemList::~CMPSettingsStreamingSettingItemList()"));
	
    delete iProxyHost;
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::EditItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::EditItemL(TInt aIndex, TBool aCalledFromMenu)
    {
    MPX_DEBUG3(_L("#MS# CMPSettingsStreamingSettingItemList::EditItemL(%d,%d)"),aIndex,aCalledFromMenu);
    CAknSettingItem* settingItem = SettingItemArray()->At(aIndex);
    TInt identifier = settingItem->Identifier();

    if ( identifier == EMPSettDefaultAPSettingId )
        {
        LaunchConnectionSelectionL();
        }
    else
        {
        // Call base call's method
        CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
        }

    // Store setting value to settings model
    settingItem->StoreL();   
    StoreSettingL(identifier);

    // Hide/unhide Proxy port & Proxy host settings according to Proxy mode.
    if (identifier == EMPSettProxyModeSettingId ||
        identifier == EMPSettDefaultAPSettingId )
        {
        SetItemVisibilityL();
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::CreateSettingItemL
// -----------------------------------------------------------------------------
//
CAknSettingItem* CMPSettingsStreamingSettingItemList::CreateSettingItemL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingSettingItemList::CreateSettingItemL(%d)"),aSettingId);
    __ASSERT_DEBUG(iProxyHost, User::Leave(KErrAbort));

    CAknSettingItem* settingItem  = NULL;

    switch ( aSettingId )
        {
        case EMPSettProxyModeSettingId:
            iSettingProxyItem = new(ELeave) CAknBinaryPopupSettingItem( aSettingId, 
                                                                        iProxyMode );
            settingItem = iSettingProxyItem; 
            break;
        
        case EMPSettProxyHostSettingId:
            {
            settingItem = new(ELeave) CAknTextSettingItem(aSettingId, iProxyHostPtr);
            settingItem->SetSettingPageFlags(CAknTextSettingPage::EZeroLengthAllowed);
            break;
            }
        
        case EMPSettProxyPortSettingId:
            settingItem = new(ELeave) CAknIntegerEdwinSettingItem(aSettingId, iProxyPort);
            break;            
        
        case EMPSettDefaultAPSettingId:        
            iSettingConnectionItem = new (ELeave) CAknTextSettingItem( aSettingId, 
                                                                       iSelectedApName );
            settingItem = iSettingConnectionItem;
            break;

        case EMPSettMinUDPPortSettingId:
            settingItem = new(ELeave) CMPSettingsUDPPortSettingItem( aSettingId, 
                                                                     iMinUDPPort, 
                                                                     iMaxUDPPort);
            break;            

        case EMPSettMaxUDPPortSettingId:
            settingItem = new(ELeave) CMPSettingsUDPPortSettingItem( aSettingId, 
                                                                     iMaxUDPPort,
                                                                     iMinUDPPort);
            break;            
            
        default:
            break;
        }

    return settingItem;   
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::SetItemVisibilityL
// -----------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::SetItemVisibilityL()
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingSettingItemList::SetItemVisibilityL() iProxyMode(%d)"),iProxyMode);
    SettingItemArray()->At(KMPSettProxyHostListItemId)->SetHidden( !iProxyMode );
    SettingItemArray()->At(KMPSettProxyPortListItemId)->SetHidden( !iProxyMode );

    HandleChangeInItemArrayOrVisibilityL();
    
    if ( ListBox() )
       {
       // Scrollbar update is required for the case when items are set hidden
       // while list is scrolled down.
       ListBox()->UpdateScrollBarsL();
       }
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::LoadAllL
// -----------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::LoadAllL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsStreamingSettingItemList::LoadAllL()"));
    iModel->LoadSettingsL(EConfigUser);
    LoadSettingL(EMPSettProxyModeSettingId);
    LoadSettingL(EMPSettProxyHostSettingId);
    LoadSettingL(EMPSettProxyPortSettingId);
    LoadSettingL(EMPSettDefaultAPSettingId);
    LoadSettingL(EMPSettMinUDPPortSettingId);
    LoadSettingL(EMPSettMaxUDPPortSettingId);
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::LoadSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::LoadSettingL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingSettingItemList::LoadSettingL(%d)"),aSettingId);
    switch ( aSettingId )
        {
        case EMPSettDefaultAPSettingId:
            {
            TUint32 wapID = 0;
            iModel->GetDefaultAp( wapID ); 
            TRAPD( err, iDefaultAP = static_cast<TInt>( IapIdFromWapIdL( wapID ) ) );
            if ( err )
                {
                iDefaultAP = 0;
                }
            
            if ( iDefaultAP != 0 ) 
                {
                UpdateSelectedConnectionNameL();
                }
            }
            break;

        case EMPSettConnTimeoutSettingId:
            iModel->GetConnectionTimeout(iConnTimeout);
            break;

        case EMPSettServerTimeoutSettingId:
            iModel->GetServerTimeout(iSrvTimeout);
            break;

        case EMPSettMinUDPPortSettingId:
            iModel->GetMinUDPPort(iMinUDPPort);
            break;

        case EMPSettMaxUDPPortSettingId:
            iModel->GetMaxUDPPort(iMaxUDPPort);
            break;
        
        case EMPSettProxyModeSettingId:
            iModel->GetProxyMode(iProxyMode);
            break;
            
        case EMPSettProxyHostSettingId:
            {
            if (!iProxyHost)
                {
                iProxyHost = HBufC::NewL(KMPSettProxyHostMaxLength);
                iProxyHostPtr.Set(iProxyHost->Des());
                }

            iModel->GetProxyHostName(iProxyHostPtr);
            break;
            }
        case EMPSettProxyPortSettingId:
            iModel->GetProxyPort(iProxyPort);
            break;
            
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::StoreSettingL
// -----------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::StoreSettingL(TInt aSettingId)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingSettingItemList::StoreSettingL(%d)"),aSettingId);
    switch (aSettingId)
        {
        case EMPSettDefaultAPSettingId:
            {
            TUint32 wapID = 0;
            TRAPD( err, wapID = WapIdFromIapIdL( static_cast<TUint32>( iDefaultAP ) ) );
            if ( err == KErrNone )
                {
                iModel->SetDefaultAp( wapID );
                }
            }
            break;
        case EMPSettConnTimeoutSettingId:
            iModel->SetConnectionTimeout(iConnTimeout);
            break;
        case EMPSettServerTimeoutSettingId:
            iModel->SetServerTimeout(iSrvTimeout);
            break;
        case EMPSettMinUDPPortSettingId:
            iModel->SetMinUDPPort(iMinUDPPort);
            break;
        case EMPSettMaxUDPPortSettingId:
            iModel->SetMaxUDPPort(iMaxUDPPort);
            break;
        case EMPSettProxyModeSettingId:
            iModel->SetProxyMode(iProxyMode);
            break;
        case EMPSettProxyHostSettingId:
            iModel->SetProxyHostNameL(*iProxyHost);
            break;
        case EMPSettProxyPortSettingId:
            iModel->SetProxyPort(iProxyPort);
            break;
        default:
            break;
        }

    iModel->StoreSettingsL();
    }

// --------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::LaunchConnectionSelectionL()
// --------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::LaunchConnectionSelectionL()
    {
    MPX_FUNC("#MS# CMPSettingsStreamingSettingItemList::LaunchConnectionSelectionL()");
    
    CCmApplicationSettingsUi* settingsUi = CCmApplicationSettingsUi::NewL();
    CleanupStack::PushL ( settingsUi );
    
    TCmSettingSelection selection;
    selection.iId = iDefaultAP;
    selection.iResult = CMManager::EConnectionMethod;
    
    TUint listItems = CMManager::EShowConnectionMethods;
    TBearerFilterArray filters;
    
    TBool retVal = settingsUi->RunApplicationSettingsL( selection, listItems, filters );
    if ( retVal )
        {
        TBool iapChanged = EFalse;
    
        switch ( selection.iResult )
            {
            case CMManager::EDestination:
                break;

            case CMManager::EAlwaysAsk:
                {
                iDefaultAP = 0; // Set AP to "None"
                }
                break;

            case CMManager::EConnectionMethod:
                {
                if ( selection.iId != iDefaultAP)
                    {
                    iDefaultAP = selection.iId;
                    iapChanged = ETrue;
                    }
                }
                break;

            default:
                break;
            }
        
        if ( iapChanged )
            {
            UpdateSelectedConnectionNameL( );
            
            if ( iSettingConnectionItem )
                {
                iSettingConnectionItem->LoadL ( );
                iSettingConnectionItem->UpdateListBoxTextL ( );
                }
    
            CheckProxyDisableL();
            
            DrawDeferred();
            }
        }
    CleanupStack::PopAndDestroy ( settingsUi );
    }

// --------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::CheckProxyDisableL()
// --------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::CheckProxyDisableL()
    {
    if ( iProxyMode && iSettingProxyItem )
        {
        iSettingProxyItem->EditItemL( EFalse );
        
        iSettingProxyItem->StoreL();
        StoreSettingL( EMPSettProxyModeSettingId );
    
        iSettingProxyItem->LoadL();
        iSettingProxyItem->UpdateListBoxTextL();
        }
    }

// --------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::UpdateSelectedConnectionNameL
// --------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::UpdateSelectedConnectionNameL()
    {
    MPX_FUNC("#MS# CMPSettingsStreamingSettingItemList::UpdateSelectedConnectionNameL()");
    
    RCmManagerExt cmManager;
    RCmConnectionMethodExt connMethod;

    CleanupClosePushL( cmManager );
    CleanupClosePushL( connMethod );
    
    cmManager.OpenL();

    // This leaves if conn method is not found.
    TRAPD( leave, connMethod = cmManager.ConnectionMethodL( iDefaultAP ) );
    if ( KErrNone == leave )
        {
        HBufC* tmpBuf = 
                cmManager.GetConnectionMethodInfoStringL( iDefaultAP, CMManager::ECmName);

        if( tmpBuf )
			{
			if ( tmpBuf->Length() < KMaxApNameLength )
            	{
	            iSelectedApName = *tmpBuf;
            	}
			delete tmpBuf;
			}	
        }

    CleanupStack::PopAndDestroy( &connMethod );
    CleanupStack::PopAndDestroy( &cmManager );
    }

// --------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::HandleListBoxEventL()
// --------------------------------------------------------------------------
//
void CMPSettingsStreamingSettingItemList::HandleListBoxEventL(
    CEikListBox* aListBox,
    TListBoxEvent aListBoxEvent )
    {
    if ( ( aListBoxEvent == EEventEnterKeyPressed ) ||
         ( aListBoxEvent == EEventItemSingleClicked ) ||
         ( aListBoxEvent == EEventItemDoubleClicked ) )
        {
        CAknSettingItemList::HandleListBoxEventL( aListBox, aListBoxEvent );
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::WapIdFromIapIdL
// -----------------------------------------------------------------------------
//
TUint32 CMPSettingsStreamingSettingItemList::WapIdFromIapIdL( TUint32 aIapId )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingSettingItemList::WapIdFromIapIdL(%d)"),aIapId);
    
    TUint32 wap = 0;
#ifdef __WINSCW__    
    wap = aIapId;
#else    
    
    CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( db );
    
    // WapIpBearer table contains the mapping between wap and iap id's.
    CCDWAPIPBearerRecord* wapBearerRecord = 
        static_cast<CCDWAPIPBearerRecord*>( 
                CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );
        
    CleanupStack::PushL( wapBearerRecord );
    
    wapBearerRecord->iWAPIAP = aIapId;
    
    TBool found = wapBearerRecord->FindL( *db );
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }

    wap = static_cast<TUint32>( wapBearerRecord->iWAPAccessPointId );
    
    CleanupStack::PopAndDestroy( wapBearerRecord );
    CleanupStack::PopAndDestroy( db );

#endif    
    
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingSettingItemList::WapIdFromIapIdL() - return wap id: %d "), wap);
    return wap;
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingSettingItemList::IapIdFromWapIdL
// -----------------------------------------------------------------------------
//
TUint32 CMPSettingsStreamingSettingItemList::IapIdFromWapIdL( TUint32 aWapId )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingSettingItemList::IapIdFromWapIdL(%d)"),aWapId);
    
    TUint32 iap = 0;
#ifdef __WINSCW__
    iap = aWapId;
#else    
    CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( db );
    
    // WapIpBearer table contains the mapping between wap and iap id's.
    CCDWAPIPBearerRecord* wapBearerRecord = 
        static_cast<CCDWAPIPBearerRecord*>( 
                CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );
        
    CleanupStack::PushL( wapBearerRecord );
    
    wapBearerRecord->iWAPAccessPointId = aWapId;
    
    TBool found = wapBearerRecord->FindL( *db );
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }

    iap = static_cast<TUint32>( wapBearerRecord->iWAPIAP );
    
    CleanupStack::PopAndDestroy( wapBearerRecord );
    CleanupStack::PopAndDestroy( db );

#endif
    
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingSettingItemList::IapIdFromWapIdL() - return iap id: %d "), iap);
    return iap;
    }


//  End of File  
