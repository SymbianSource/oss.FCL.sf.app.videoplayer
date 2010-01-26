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
* Description:   Container class for Streaming settings list view.*
*/


// Version : %version: 7 %



// INCLUDE FILES
#include    <aknlists.h>
#include    <barsread.h>
#include    <MediaSettings.rsg>
#include    <csxhelp/iptv.hlp.hrh>

#include    "MPSettingsApp.h"
#include    "MPSettingsStreamingContainer.h"
#include    "MPSettingsConstants.h"
#include	"mpxlog.h"

class CMPSettingsModelForROP;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsStreamingContainer::CMPSettingsStreamingContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsStreamingContainer::CMPSettingsStreamingContainer(CMPSettingsModelForROP* aModel) : iModel( aModel)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsStreamingContainer::CMPSettingsStreamingContainer()"));
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::~CMPSettingsStreamingContainer
// Destructor
// ---------------------------------------------------------
//
CMPSettingsStreamingContainer::~CMPSettingsStreamingContainer()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsStreamingContainer::~CMPSettingsStreamingContainer()"));
	if (iListBox) 
		{
		delete iListBox;
		}
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::ConstructComponentControlsL
// ---------------------------------------------------------
//
void CMPSettingsStreamingContainer::ConstructComponentControlsL()
    {
    MPX_FUNC("#MS# CMPSettingsStreamingContainer::ConstructComponentControlsL()");
    iListBox = new(ELeave) CAknSettingStyleListBox;
   
    iListBox->SetContainerWindowL(*this); // set's iListBox to be contained in this container

    TResourceReader rReader;
    iCoeEnv->Static()->CreateResourceReaderLC(rReader, R_MPSETT_STREAMING_LBX_RESOURCE);
    iListBox->ConstructFromResourceL(rReader); // construct's iListBox from resource file
    CleanupStack::PopAndDestroy(); // rReader
    
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    iComponentControl = iListBox;
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::TitleResourceId
// ---------------------------------------------------------
//
TInt CMPSettingsStreamingContainer::TitleResourceId()
    {
    return R_MPSETT_STREAMING_VIEW_TITLE;
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::GetHelpContext
// ---------------------------------------------------------
//
void CMPSettingsStreamingContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    MPX_FUNC("#MS# CMPSettingsStreamingContainer::GetHelpContext()");
    aContext.iMajor = KIptvHelpContextUid;
    aContext.iContext = KIPTV_HLP_SETTINGS;
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::FocusChanged
// ---------------------------------------------------------
//
void CMPSettingsStreamingContainer::FocusChanged(TDrawNow /*aDrawNow*/)
{
	if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }

	if( iComponentControl)
        {
        iComponentControl->SetFocus( IsFocused() );
        }
}
// End of File  
