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
* Description: 
*
*/




#ifndef M_VCXNSCONTENTSEARCHIF_H
#define M_VCXNSCONTENTSEARCHIF_H

// INCLUDES
#include <e32base.h>

class MVcxNsContentSearchObserver;

// CLASS DECLARATION

/**
* Class MVcxNsContentSearchIf 
* Interface for using search
*
* @lib vcxnsuiengine.lib
*/
class MVcxNsContentSearchIf
    {

public:

    /**
     * RegisterObserver.
     * @param aObserver Observer
     */
    virtual void RegisterObserver( MVcxNsContentSearchObserver* aObserver ) = 0;

    /**
     * DeRegisterObserver.
     * @param aObserver Observer
     */
    virtual void DeRegisterObserver( MVcxNsContentSearchObserver* aObserver ) = 0;
    
    /**
     * Starts the search with given keyword.
     * @param aSearchString Search string
     */
    virtual void SearchL( const TDesC& aSearchString ) = 0;
    
    /**
     * Cancel ongoing search.
     *
     */
    virtual void CancelSearchL( ) = 0;
    
    /**
     * Handle search related status messages.
     * @param aStatus Search status.
     * @param aInfo Information parameter.
     */
    virtual void HandleSearchMsgL( TInt aStatus, TInt aInfo ) = 0;
    
    /**
     * Handle search cancelled by user.
     */
    virtual void SearchCancelled() = 0;
    
    };

#endif // MVCXNSAPPCATEGORYPROVIDERIF_H
