/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mpx video utility stub for STIF testing
*
*/

// Version : %version: ou1cpsw#5 %


#include "mpxviewutilityimp_stub.h"
#include "mpxvideoplayerconstants.h"
#include "stifutilities.h"
#include "mpxvideo_debug.h"


MMPXViewUtility* MMPXViewUtility::UtilityL()
{
    return CMPXViewUtility::UtilityL();
}

// ---------------------------------------------------------------------------
// Retrieves view utility from TLS. Creates if it's not there.
// ---------------------------------------------------------------------------
//
MMPXViewUtility* CMPXViewUtility::UtilityL()
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::UtilityL()"));

    StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

    if ( ! utility )
    {
        utility = new (ELeave) StifUtilities;

        Dll::SetTls( utility );
    }

    if ( ! utility->iViewUtility )
    {
        utility->iViewUtility = CMPXViewUtility::NewL();
    }

    utility->iViewUtility->iRefCount++;

    return utility->iViewUtility;
}

CMPXViewUtility::CMPXViewUtility()
    : iViewHistoryDepth( 0 )
    , iViewTypeUid(KNullUid)
{
}

CMPXViewUtility::~CMPXViewUtility()
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::~CMPXViewUtility()"));
    iObservers.Reset();
}

void CMPXViewUtility::ConstructL()
{
}


CMPXViewUtility* CMPXViewUtility::NewL()
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::NewL()"));

    CMPXViewUtility* self = new ( ELeave ) CMPXViewUtility();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
}

void CMPXViewUtility::AddObserverL( MMPXViewActivationObserver* aObserver )
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::AddObserverL()"));

    if ( aObserver )
    {
        iObservers.AppendL( aObserver );
    }
}

void CMPXViewUtility::RemoveObserver( MMPXViewActivationObserver* aObserver )
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::RemoveObserver()"));

    TInt index = iObservers.Find( aObserver );

    if ( index >= 0 )
    {
        iObservers.Remove( index );
    }
}

TUid CMPXViewUtility::ConstructDefaultViewHistoryL()
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::ConstructDefaultViewHistoryL()"));

    return KNullUid;
}

void CMPXViewUtility::Close()
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::Close()"),
                   _L("iRefCount = %d"), iRefCount );

    if ( --iRefCount == 0 )
    {
        StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

        utility->DeleteViewUtility();
    }
}

void CMPXViewUtility::PreLoadViewL( const TUid& aUid )
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::PreLoadViewL()"));

    iViewTypeUid = aUid;

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EViewUtilityPreLoadView;
        event->iUid   = aUid;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXViewUtility::ActivateViewL( const RArray<TUid>& aUids, const TDesC* /*aParam*/ )
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::ActivateViewL(RArray<TUid>)"));

    TBool pdlView = EFalse;

    //
    //  Check if the PDL view is in the array
    //
    for ( TInt i = 0 ; i < aUids.Count() ; i++ )
    {
        if ( aUids[i] == KVideoPdlPlaybackViewUid )
        {
            pdlView = ETrue;
            break;
        }
    }

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;

        if ( pdlView )
        {
            event->iEvent = EViewUtilityActivatePdlView;
        }
        else
        {
            event->iEvent = EViewUtilityActivateView;
        }

        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXViewUtility::ActivateViewL( const TUid& aUid, const TDesC* /*aParam*/ )
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::ActivateViewL( TUid )"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EViewUtilityActivateViewUid;
        event->iUid   = aUid;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXViewUtility::SetAsDefaultViewL( const TUid& aUid )
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::SetAsDefaultViewL()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EViewUtilitySetAsDefaultView;
        event->iUid   = aUid;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXViewUtility::ActivatePreviousViewL()
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::ActivatePreviousViewL()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EViewUtilityActivatePreviousView;
        iStifObserver->HandleUtilityEvent( event );
    }

}

TInt CMPXViewUtility::ViewHistoryDepth()
{
    return iViewHistoryDepth;
}

TUid CMPXViewUtility::ActiveViewImplementationUid()
{
    return KNullUid;
}

TUid CMPXViewUtility::ActiveViewType()
{
    MPX_DEBUG(_L("CMPXViewUtility::ActiveViewType(0x%x)"), iViewTypeUid.iUid );
    return iViewTypeUid;
}

TUid CMPXViewUtility::PreviousViewType()
{
    return KNullUid;
}

TUid CMPXViewUtility::DefaultViewUid()
{
    return KNullUid;
}

void CMPXViewUtility::PushDefaultHistoryL()
{
}

MMPXViewPluginManager& CMPXViewUtility::ViewPluginManager()
{
}

void CMPXViewUtility::PreLoadViewsL()
{
}

void CMPXViewUtility::SetAsDefaultViewL( const RArray<TUid>& /*aUids*/ )
{
}

void CMPXViewUtility::AddStifObserverL( MAppUiEngineStifTestObserver* aObserver )
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::AddStifObserverL()"));

    iStifObserver = aObserver;
}

void CMPXViewUtility::RemoveStifObserver()
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::RemoveStifObserver()"));

    iStifObserver = NULL;
}

void CMPXViewUtility::SetViewHistoryDepth( TInt aDepth )
{
    MPX_DEBUG(_L("CMPXViewUtility::CMPXViewUtility::SetViewHistoryDepth(%d)"), aDepth);

    iViewHistoryDepth = aDepth;
}

void CMPXViewUtility::PreLoadViewsL( const TUid& /* aUid */ )
{
    MPX_ENTER_EXIT(_L("CMPXViewUtility::PreLoadViewsL( aUid )"));
}
