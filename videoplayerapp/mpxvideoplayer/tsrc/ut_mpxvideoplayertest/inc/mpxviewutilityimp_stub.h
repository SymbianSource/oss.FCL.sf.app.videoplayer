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

// Version : %version: ou1cpsw#4 %

#ifndef MPXVIEWUTILITY_STUB_H_
#define MPXVIEWUTILITY_STUB_H_

// INCLUDES

#include "mpxvideoplayer_appuienginetester.h"
#include <mpxviewutility.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

NONSHARABLE_CLASS( CMPXViewUtility ) : public CBase,
                                       public MMPXViewUtility
{
    public:
        static MMPXViewUtility* UtilityL();

    public:

        CMPXViewUtility();

        virtual ~CMPXViewUtility();

        void ConstructL();

        static CMPXViewUtility* NewL();

        static void StoreViewUtility( MMPXViewUtility* aUtility );

        //
        //  MMPXViewUtility Implementation
        //
        void Close();
        void ActivateViewL( const TUid& aUid, const TDesC* aParam = NULL );
        void ActivateViewL( const RArray<TUid>& aUids, const TDesC* aParam = NULL );
        void SetAsDefaultViewL( const TUid& aUid );
        void SetAsDefaultViewL( const RArray<TUid>& aUids );
        void ActivatePreviousViewL();
        void PreLoadViewsL();
        void PreLoadViewL( const TUid& aUid );
        MMPXViewPluginManager& ViewPluginManager();
        TUid ConstructDefaultViewHistoryL();
        void PushDefaultHistoryL();
        TUid DefaultViewUid();
        TInt ViewHistoryDepth();
        TUid PreviousViewType();
        TUid ActiveViewType();
        TUid ActiveViewImplementationUid();
        void AddObserverL( MMPXViewActivationObserver* aObserver );
        void RemoveObserver( MMPXViewActivationObserver* aObserver );
        void PreLoadViewsL( const TUid& aUid );

        void AddStifObserverL( MAppUiEngineStifTestObserver* aObserver );
        void RemoveStifObserver();
        void SetViewHistoryDepth( TInt aDepth );

    private:

        RPointerArray<MMPXViewActivationObserver>    iObservers;
        MAppUiEngineStifTestObserver*                iStifObserver;

        TInt iRefCount;
        TInt iViewHistoryDepth;

        TUid iViewTypeUid;
};

#endif /*MPXVIEWUTILITY_STUB_H_*/
