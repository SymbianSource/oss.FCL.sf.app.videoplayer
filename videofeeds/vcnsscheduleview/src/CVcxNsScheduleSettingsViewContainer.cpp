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
* Description:    Video on Demand schedule settings view container class.*
*/



#include <coecntrl.h>
#include <AknUtils.h>
#include <StringLoader.h>
#include "IptvDebug.h"
#include "vcxnsservicesettings.h"

#include <csxhelp/iptv.hlp.hrh> // KIPTV_HLP_SCHEDULE_DOWNLOAD

#include <vcxnsscheduleview.rsg>
#include "vcxnsscheduleview.hrh"

#include "CVcxNsScheduleSettingsViewContainer.h"
#include "CVcxNsScheduleSettingsSettingItem.h"
#include "CVcxNsScheduleSettingsList.h"

const TUid KUidIptvMainApp = { 0x102750E2 };

CVcxNsScheduleSettingsViewContainer::CVcxNsScheduleSettingsViewContainer(
    CVcxNsServiceSettings& aSettings,
    TIptvServiceId aServiceId )
    : iSettings( aSettings ),
      iServiceId( aServiceId )
    {
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CVcxNsScheduleSettingsViewContainer::~CVcxNsScheduleSettingsViewContainer()
    {
    delete iSettingItem;
    }

// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::ConstructL()
// ---------------------------------------------------------
//
void CVcxNsScheduleSettingsViewContainer::ConstructL( const TRect& aRect )
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "IPTV VOD ## CVcxNsScheduleSettingsViewContainer::ConstructL -- Enter" );
   
    CreateWindowL();
    SetRect( aRect );
    
    iSettingItem = CVcxNsScheduleSettingsSettingItem::NewL( 
            this,iSettings );
            
    if ( iSettingItem )
        {
        iSettingItem->Listbox()->LoadSettingValuesL( iServiceId ); 
        iSettingItem->ActivateL();
        }
	DrawNow();
    ActivateL();
                
    IPTVLOGSTRING_LOW_LEVEL(
        "IPTV VOD ## CVcxNsScheduleSettingsViewContainer::ConstructL -- Exit" );
    }

// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::SizeChanged()
// ---------------------------------------------------------
//
void CVcxNsScheduleSettingsViewContainer::SizeChanged()
    {  
    if ( iSettingItem )
        {
        iSettingItem->Listbox()->SetRect( Rect() );
        }
    }

// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::CountComponentControls()
// ---------------------------------------------------------
//
TInt CVcxNsScheduleSettingsViewContainer::CountComponentControls() const
    {
    if ( iSettingItem )
        {
        return 1;  // iSettingItem->iListbox
        }
    else 
        {
        return 0;    
        }
    }
// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::ComponentControl()
// ---------------------------------------------------------
//
CCoeControl* CVcxNsScheduleSettingsViewContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            if ( iSettingItem )
                {
                return iSettingItem->Listbox();
                }
            return NULL;
            }
        default:
            {
            return NULL;
            }
        }
    }

// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CVcxNsScheduleSettingsViewContainer::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    if ( iSettingItem )
        {
        return iSettingItem->Listbox()->OfferKeyEventL( aKeyEvent, aType );
        }
    return EKeyWasNotConsumed;   
    }
  
// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::Draw
// ---------------------------------------------------------
//
void CVcxNsScheduleSettingsViewContainer::Draw( const TRect& aRect ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "IPTV VOD ## CVcxNsScheduleSettingsViewContainer::Draw ()" );

    CWindowGc& gc = SystemGc();
    gc.Clear(aRect);
    }

// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CVcxNsScheduleSettingsViewContainer::HandleResourceChange( TInt aType )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "IPTV VOD ## CVcxNsScheduleSettingsViewContainer::HandleResourceChange -- Enter" );
        
    CCoeControl::HandleResourceChange( aType );

    iSettingItem->Listbox()->HandleResourceChange( aType );
    
    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        SetRect(rect);
        SizeChanged();
        DrawNow();
        }
        
    IPTVLOGSTRING_LOW_LEVEL(
        "IPTV VOD ## CVcxNsScheduleSettingsViewContainer::HandleResourceChange -- Exit" );
    }
   
// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::HandleChangeSelectedSettingItemL
// ---------------------------------------------------------
//
void CVcxNsScheduleSettingsViewContainer::HandleChangeSelectedSettingItemL( 
    TInt /*aCommand*/ )
    {
    if ( iSettingItem )
        {
        iSettingItem->ChangeSelectedItemL();
        }
    }

// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::SaveSettingsL
// ---------------------------------------------------------
//
void CVcxNsScheduleSettingsViewContainer::SaveSettingsL()
    {
    if ( iSettingItem )
        {
        iSettingItem->Listbox()->SaveSettingValuesL( iServiceId );
        }
    }

// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::GetHelpContext()
// ---------------------------------------------------------
//
void CVcxNsScheduleSettingsViewContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor   = KUidIptvMainApp;
    aContext.iContext = KIPTV_HLP_SCHEDULE_DOWNLOAD;
    }

// ---------------------------------------------------------
// CVcxNsScheduleSettingsViewContainer::IsSchedulingActive
// ---------------------------------------------------------
//
TBool CVcxNsScheduleSettingsViewContainer::IsSchedulingActive()
    {
    if ( iSettingItem )
        {
        return iSettingItem->Listbox()->IsSchedulingActive();
        }
    return EFalse;
    }

// End of File
