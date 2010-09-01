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




#ifndef M_VCXNSUPDATEPROGRESSIF_H
#define M_VCXNSUPDATEPROGRESSIF_H

// INCLUDES
#include <e32base.h>

class MVcxNsUpdateProgressObserver;

// CLASS DECLARATION

/**
* Class MVcxNsUpdateProgressIf
*
* @lib vcxnsuiengine.lib
*/
class MVcxNsUpdateProgressIf
    {

public:

    /**
     * From MVcxNsUpdateProgressIf
     * RegisterObserver.
     * @param aObserver Observer 
     */
    virtual void RegisterObserver( MVcxNsUpdateProgressObserver* aObserver ) = 0;

    /**
     * From MVcxNsUpdateProgressIf
     * DeRegisterObserver.
     * @param aObserver Observer 
     */
    virtual void DeRegisterObserver( MVcxNsUpdateProgressObserver* aObserver ) = 0;

    /**
     * From MVcxNsUpdateProgressIf
    * Get progress.
    * @param aServiceId Service id which progress is requested
    * @return Progress between 0-99, KErrNotFound if not available
     */
    virtual TInt GetProgress( TUint32 aServiceId ) = 0;
    
    };

#endif // M_VCXNSUPDATEPROGRESSIF_H
