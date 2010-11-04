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
* Description:  mpx collection utility stub for STIF testing
*
*/

// Version : %version: ou1cpsw#5 %


#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionplaylistobserver.h>

#include "stifutilities.h"
#include "mpxcollectionutilityimp_stub.h"
#include "mpxvideo_debug.h"


CMPXCollectionUtility*
CMPXCollectionUtility::NewL( const TUid& /*aModeId*/, MMPXCollectionObserver* aObs )
{
    MPX_ENTER_EXIT(_L("CMPXCollectionUtility::NewL()"));

    StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

    if ( ! utility )
    {
        utility = new (ELeave) StifUtilities;

        Dll::SetTls( utility );
    }

    if ( ! utility->iCollectionUtility )
    {
        utility->iCollectionUtility = new (ELeave) CMPXCollectionUtility( aObs );
    }

    utility->iCollectionUtility->AddRefCount();

    return utility->iCollectionUtility;
}

CMPXCollectionUtility::~CMPXCollectionUtility()
{
    MPX_ENTER_EXIT(_L("CMPXCollectionUtility::~CMPXCollectionUtility()"));

    if ( iCollectionPath )
    {
        delete iCollectionPath;
    }
}

CMPXCollectionUtility::CMPXCollectionUtility( MMPXCollectionObserver* aObs )
    : iObserver( aObs ),
      iCollectionPath( NULL ),
      iRefCount(0)
{
}

void CMPXCollectionUtility::ConstructL( const TUid& /*aModeId*/ )
{
}

void CMPXCollectionUtility::AddStifObserverL( MAppUiEngineStifTestObserver* aObserver )
{
    MPX_ENTER_EXIT(_L("CMPXCollectionUtility::AddStifObserverL()"));

    iStifObserver = aObserver;
}

void CMPXCollectionUtility::RemoveStifObserverL()
{
    MPX_ENTER_EXIT(_L("CMPXCollectionUtility::RemoveStifObserverL()"));

    iStifObserver = NULL;
}

MMPXCollection&
CMPXCollectionUtility::Collection()
{
    MPX_ENTER_EXIT(_L("CMPXCollectionUtility::Collection()"));
    return *this;
}

TUid
CMPXCollectionUtility::CollectionIDL( const TArray<TUid>& /*aUids*/ )
{
    return KNullUid;
}

void CMPXCollectionUtility::AddSubscriptionL( const CMPXSubscription& /*aSubscription*/ )
{
}

void CMPXCollectionUtility::RemoveSubscriptionL( const CMPXSubscription& /*aSubscription*/ )
{
}

void CMPXCollectionUtility::ClearSubscriptionsL()
{
}

void CMPXCollectionUtility::AddRefCount()
{
    iRefCount++;
}

void CMPXCollectionUtility::Close()
{
    MPX_ENTER_EXIT(_L("CMPXCollectionUtility::Close()"),
                   _L("iRefCount = %d"), iRefCount );

    if ( --iRefCount == 0 )
    {
        StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

        utility->DeleteCollectionUtility();
    }
}

void CMPXCollectionUtility::OpenL( TMPXOpenMode /*aMode*/ )
{
}

void CMPXCollectionUtility::OpenL( TInt /*aIndex*/, TMPXOpenMode /*aMode*/ )
{
}

void CMPXCollectionUtility::OpenL( const CMPXCollectionPath& /*aPath*/, TMPXOpenMode /*aMode*/ )
{
    MPX_ENTER_EXIT(_L("CMPXCollectionUtility::OpenL()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = ECollectionUtilityOpen;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXCollectionUtility::OpenL( TInt /*aIndex*/,
                                   const TArray<TMPXAttribute>& /*aAttrs*/,
                                   TMPXOpenMode /*aMode*/ )
{
}

void CMPXCollectionUtility::OpenL( const CMPXCollectionPath& /*aPath*/,
                                   const TArray<TMPXAttribute>& /*aAttrs*/,
                                   TMPXOpenMode /*aMode*/ )
{
}

void CMPXCollectionUtility::OpenL( const TArray<TUid>& /*aUids*/, TMPXOpenMode /*aMode*/ )
{
}

void CMPXCollectionUtility::OpenL( const TUid& /*aUid*/, TMPXOpenMode /*aMode*/ )
{
}

void CMPXCollectionUtility::SetFilterL( CMPXFilter* /*aFilter*/ )
{
}

CMPXFilter* CMPXCollectionUtility::FilterL()
{
    return NULL;
}

TUid
CMPXCollectionUtility::UidL() const
{
    return KNullUid;
}

CMPXCollectionPath* CMPXCollectionUtility::PathL()
{
    MPX_ENTER_EXIT(_L("CMPXCollectionUtility::PathL()"));

    CMPXCollectionPath* cp = CMPXCollectionPath::NewL();

    TMPXItemId item1;
    TMPXItemId item2;

    cp->AppendL( item1 );
    cp->AppendL( item2 );

    return cp;
}

void CMPXCollectionUtility::BackL()
{
}

TBool CMPXCollectionUtility::IsRemote( const CMPXCollectionPath& /*aPath*/ )
{
    return EFalse;
}

void CMPXCollectionUtility::CancelRequest()
{
}

void CMPXCollectionUtility::AddL( const CMPXMedia& /*aNewProperties*/ )
{
}

void CMPXCollectionUtility::RemoveL( const CMPXCollectionPath& /*aPath*/,
                                     MMPXCollectionRemoveObserver* /*aObs*/ )
{
}

void CMPXCollectionUtility::RemoveL( const CMPXMedia& /*aProperties*/ )
{
}

void CMPXCollectionUtility::SetSyncL( const CMPXMedia& /*aMedia*/ )
{
}

void CMPXCollectionUtility::SetL( const CMPXMedia& /*aMedia*/ )
{
}

void CMPXCollectionUtility::FindAllL( const CMPXMedia& /*aMedia*/,
                                      const TArray<TMPXAttribute>& /*aAttrs*/,
                                      MMPXCollectionFindObserver& /*aObs*/ )
{
}

// ----------------------------------------------------------------------------
// Find media sync
// ----------------------------------------------------------------------------
//
CMPXMedia*
CMPXCollectionUtility::FindAllL( const CMPXMedia& /*aMedia*/,
                                 const TArray<TMPXAttribute>& /*aAttrs*/ )
{
    return NULL;
}

void CMPXCollectionUtility::MediaL( const CMPXCollectionPath& /*aPath*/,
                                    const TArray<TMPXAttribute>& /*aAttrs*/,
                                    CMPXAttributeSpecs* /*aSpecs*/,
                                    CMPXFilter* /*aFilter*/ )
{
    MPX_ENTER_EXIT(_L("CMPXCollectionUtility::MediaL()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = ECollectionUtilityMedia;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXCollectionUtility::NotifyL( TMPXCollectionBroadCastMsg /*aMsg*/, TInt /*aData*/ )
{
}

void CMPXCollectionUtility::CommandL( TMPXCollectionCommand /*aCmd*/, TInt /*aData*/ )
{
}

void CMPXCollectionUtility::CommandL( CMPXCommand& /*aCmd*/ )
{
}

void CMPXCollectionUtility::GetSupportedTypesL( RPointerArray<CMPXCollectionType>& /*aArray*/ )
{
}

void CMPXCollectionUtility::GetSupportedCapabilitiesL( TCollectionCapability& /*aCapability*/ )
{
}

TUid CMPXCollectionUtility::CollectionIDL( TUid& /*aCollection*/ )
{
    return KNullUid;
}

void CMPXCollectionUtility::MessageReceived( TInt /*aMsgData*/, TInt /*aError*/ )
{
}

EXPORT_C void CMPXCollectionPlaylist::SetAutoPlay( TBool /*aAutoPlay*/ )
{
}

// End of file
