/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    VoD information popup functionality implementation*
*/




// INCLUDE FILES
#include <AknInfoPopupNoteController.h>
#include <aknview.h>
#include <e32base.h>
#include <eikmenup.h>
#include <StringLoader.h>
#include "vcxnsservice.h"
#include "vcxnsserviceproviderif.h"
#include "vcxnsuiengine.h"
#include <vcxhgvodui.rsg>

#include "vcxhgvodinformationpopup.h"

// CONSTANTS
const TInt KTimeForPopupInView = 60000;   // 60 sec
const TInt KVcxNsInformationPopupTimerMediumDelay = 100000;  // 0,1 sec
const TInt KVcxNsInformationPopupTimerShortDelay = 100;     // 0,0001 sec
const TInt KNoDelay = 10;      // 0,00001 sec

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::CVcxHgVodInformationPopup
// ---------------------------------------------------------
//
CVcxHgVodInformationPopup::CVcxHgVodInformationPopup( 
    CAknView& aParentView, 
    CVcxNsUiEngine& aUiEngine )
: iParentView ( aParentView ),
  iUiEngine ( aUiEngine )
      {
      }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::ConstructL
// ---------------------------------------------------------
//
void CVcxHgVodInformationPopup::ConstructL()
    {
    iTimer = CIptvTimer::NewL( CActive::EPriorityStandard, *this );
    iPopupText = StringLoader::LoadL( R_VCXHG_VOD_UPDATING );
    }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::NewL
// ---------------------------------------------------------
//
CVcxHgVodInformationPopup* CVcxHgVodInformationPopup::NewL( 
    CAknView& aParentView,
    CVcxNsUiEngine& aUiEngine )
    {
    CVcxHgVodInformationPopup* self = 
        new( ELeave ) CVcxHgVodInformationPopup ( aParentView, aUiEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::~CVcxHgVodInformationPopup
// ---------------------------------------------------------
//
CVcxHgVodInformationPopup::~CVcxHgVodInformationPopup()
    {
    DeleteControl();
    
    delete iPopupText;
    
    if (iTimer->IsActive())
        {
        iTimer->Cancel();
        }
    delete iTimer;
    }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::CreateControl
// ---------------------------------------------------------
//
TBool CVcxHgVodInformationPopup::CreateControlL()
    {
    TBool controlCreated( EFalse );
    
    if ( !iPopupController )
        {
        iPopupController = CAknInfoPopupNoteController::NewL();

        iPopupController->AddObserverL(*this);
        iPopupController->SetTimePopupInView( KTimeForPopupInView );
        //Show immediately
        iPopupController->SetTimeDelayBeforeShow( KNoDelay ); 

        if ( !iPopupText )
            {
            iPopupText = StringLoader::LoadL( R_VCXHG_VOD_UPDATING );
            }

        iPopupController->SetTextL( iPopupText->Des() );

        TRect clientRect = iParentView.ClientRect();
        TPoint topRight( clientRect.iBr.iX, clientRect.iTl.iY );
        iTopRight = topRight;
        
        controlCreated = ETrue;
        }

    iPopupController->SetPositionAndAlignment( iTopRight, EHRightVCenter );
    
    return controlCreated;
    }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::DeleteControl
// ---------------------------------------------------------
//
void CVcxHgVodInformationPopup::DeleteControl()
    {
    if ( iPopupController )
        {
        iPopupController->RemoveObserver(*this);
	
    	delete iPopupController;
    	iPopupController = NULL;   
        }
    }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::SetTextL
// ---------------------------------------------------------
//
void CVcxHgVodInformationPopup::SetTextL( const TDesC& aText )
    {
    if ( iPopupText && iPopupController )
        {
        delete iPopupText;
        iPopupText = NULL;
        iPopupText  = HBufC16::NewL( aText.Length() );
        *iPopupText = aText;
        iPopupController->SetTextL( iPopupText->Des() );
        }
    }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::Hide
// ---------------------------------------------------------
//
void CVcxHgVodInformationPopup::Hide()
    {
    if ( iTimer->IsActive() )
        {
        iTimer->Cancel();
        }    

    if ( iPopupController )
        {
        iPopupController->HideInfoPopupNote();
        }
        
    DeleteControl();
    
    // Reset the text when hiding on purpose.
    delete iPopupText;
    iPopupText = NULL;
    }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::ShowL
// ---------------------------------------------------------
//
void CVcxHgVodInformationPopup::ShowL()
    {
    CreateControlL();

    if ( iPopupController )
        {
        iPopupController->ShowInfoPopupNote();    
        }
    }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::HandleInfoPopupNoteEvent
// ---------------------------------------------------------
//
void CVcxHgVodInformationPopup::HandleInfoPopupNoteEvent(
    CAknInfoPopupNoteController* aController,
    MAknInfoPopupNoteObserver::TAknInfoPopupNoteEvent aEvent )
    {
    if ( aController )
        {
        if ( aEvent == EInfoPopupNoteHidden )   
            {
            TVcxNsAppState state = iUiEngine.VcAppState();
            
            if ( ( state == EStateContentView || state == EStateCategoryView )
                    && iParentView.IsForeground() )
                {
                // Check if still updating --> if yes, show note again.
                // We do this because any key press closes the note.
                TRAP_IGNORE 
                    (
                    if ( iUiEngine.GetServiceProviderL()->GetActiveServiceData()->
                               GetUpdateStatus() == CVcxNsService::EUpdateOngoing )
                        {
                        if ( iTimer->IsActive() )
                            {
                            iTimer->Cancel();
                            }
                        iTimer->After( KVcxNsInformationPopupTimerShortDelay );    
                        }
                    )
                }
            }
        else
            {
            if ( iTimer->IsActive() )
                {
                iTimer->Cancel();
                }
            }
        }
    }

// ---------------------------------------------------------
// CVcxHgVodInformationPopup::TimerExpired()
// ---------------------------------------------------------
//
void CVcxHgVodInformationPopup::TimerExpired( CIptvTimer* /*aTimer*/ )
    {
    DeleteControl();
    TVcxNsAppState state = iUiEngine.VcAppState();

    if ( state == EStatePlayback || state == EStateNoState  )
        {
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        return;
        }

    // When timer expires, check that view still exist (might have been closed)
    // and update is still ongoing (might have been finished). If both are true
    // the information pop-up needs to be shown still.
    
    if ( ( state == EStateContentView || state == EStateCategoryView )
            && iParentView.IsForeground() )
        {
        MVcxNsServiceProviderIf* serviceIf = NULL;
        TRAPD( leave, serviceIf = iUiEngine.GetServiceProviderL() );
        if ( !leave && serviceIf &&
             serviceIf->GetActiveServiceData()->GetUpdateStatus() == CVcxNsService::EUpdateOngoing  )
            {
            // If there's a menu, we need to delay note for a moment. Can't display with menu.
            if( iParentView.MenuBar() && iParentView.MenuBar()->IsDisplayed() )
                {
                if ( iTimer->IsActive() )
                    {
                    iTimer->Cancel();
                    }
                iTimer->After( KVcxNsInformationPopupTimerMediumDelay );
                }
            else
                {
                TRAP_IGNORE( ShowL() );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgVodInformationPopup::HandleForegroundEventL( TBool aForeground )
    {
    if ( aForeground )
        { 
        // Don't active timer after Hide() has been called (iPopupText is then NULL)
        if ( iPopupText )
            {
            iTimer->Cancel();
            iTimer->After( KVcxNsInformationPopupTimerShortDelay ); 
            }
        }
    else
        {
        // Nothing to do
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgVodInformationPopup::HandleSizeChange()
    {
    // Must update popup position relative to client rect.
    // Don't active timer after Hide() has been called (iPopupText is then NULL)
    if ( iPopupController && iPopupText )
        {
        iTimer->Cancel();
        iTimer->After( KVcxNsInformationPopupTimerShortDelay ); 
        }
    }
