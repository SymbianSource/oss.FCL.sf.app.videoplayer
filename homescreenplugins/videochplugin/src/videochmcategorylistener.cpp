/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    CVcxNsChMCategoryListener class definition*
*/




#include <MediatorEventConsumer.h>
#include <MediatorEventProvider.h>
#include <MediatorServiceDefs.h>

#include "videochmcategorylistener.h"
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>

// -----------------------------------------------------------------------------
// CVcxNsChMCategoryListener::CVcxNsChMCategoryListener
// -----------------------------------------------------------------------------
//
CVcxNsChMCategoryListener::CVcxNsChMCategoryListener( TUid& aMDomain,
                                                      TUid& aMCategory,
                                                      TVersion& aVersion) :
    iMediatorDomain( aMDomain ),
    iMediatorCat( aMCategory ),
    iMVersion( aVersion )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CVcxNsChMCategoryListener::~CVcxNsChMCategoryListener
// -----------------------------------------------------------------------------
//
CVcxNsChMCategoryListener::~CVcxNsChMCategoryListener()
    {
    UnsubscribeAndDeregisterAll();
    iEventArray.Close();
    delete iEventConsumer;    
    }

// -----------------------------------------------------------------------------
// CVcxNsChMCategoryListener::NewLC
// -----------------------------------------------------------------------------
//
CVcxNsChMCategoryListener* CVcxNsChMCategoryListener::NewLC( MMediatorEventObserver* aObserver, 
                                                             TUid& aMDomain,
                                                             TUid& aMCategory,
                                                             TVersion& aVersion)
    {
    if( !aObserver )
        {
        User::Leave( KErrNotFound );
        }
    
    CVcxNsChMCategoryListener* self = 
                new (ELeave)CVcxNsChMCategoryListener( aMDomain, aMCategory, aVersion );
    
    CleanupStack::PushL(self);
    self->ConstructL( aObserver );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsChMCategoryListener::NewL
// -----------------------------------------------------------------------------
//
CVcxNsChMCategoryListener* CVcxNsChMCategoryListener::NewL( MMediatorEventObserver* aObserver, 
                                                            TUid& aMDomain,
                                                            TUid& aMCategory,
                                                            TVersion& aVersion )
    {
    CVcxNsChMCategoryListener* self = 
            CVcxNsChMCategoryListener::NewLC( aObserver, aMDomain,
                                              aMCategory, aVersion );
    CleanupStack::Pop( self ); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsChMCategoryListener::ConstructL
// -----------------------------------------------------------------------------
//
void CVcxNsChMCategoryListener::ConstructL( MMediatorEventObserver* aObserver )
    {
    iEventConsumer = CMediatorEventConsumer::NewL( aObserver );
    }

// -----------------------------------------------------------------------------
// CVcxNsChMCategoryListener::RegisterAndSubscribe
// -----------------------------------------------------------------------------
//
void CVcxNsChMCategoryListener::RegisterAndSubscribeL( const TInt& aEvent )
    {
    
    TCapabilitySet mediatorVcxNsCaps = TCapabilitySet( ECapabilityReadUserData ); 
    CMediatorEventProvider* provider = CMediatorEventProvider::NewL();
    
    TInt ret = provider->RegisterEvent( iMediatorDomain, iMediatorCat, aEvent, 
                                        iMVersion, mediatorVcxNsCaps );
    delete provider;
         
    if( ret != KErrNone && ret != KErrMediatorEventAlreadyExists )
        {        
        User::Leave( ret );
        }
     
    User::LeaveIfError( iEventConsumer->SubscribeEvent( iMediatorDomain, 
                                                        iMediatorCat,
                                                        aEvent, 
                                                        iMVersion ) );

    iEventArray.AppendL( aEvent );    
    }

// -----------------------------------------------------------------------------
// CVcxNsChMCategoryListener::UnsubscribeAndDeregisterAll
// -----------------------------------------------------------------------------
//
void CVcxNsChMCategoryListener::UnsubscribeAndDeregisterAll()
    {    
    CMediatorEventProvider* provider = NULL;
        
    TRAPD( err, provider = CMediatorEventProvider::NewL() );
    if( err != KErrNone )
        {
        return;
        }
    
    TInt count = iEventArray.Count(); 
    for( TInt i = 0; i < count; ++i )
        {
        iEventConsumer->UnsubscribeEvent( iMediatorDomain, 
                                          iMediatorCat,
                                          iEventArray[ i ] );
        
        provider->UnregisterEvent( iMediatorDomain,
                                   iMediatorCat,
                                   iEventArray[ i ] );        
        }
    delete provider;  
    }
// end of file
