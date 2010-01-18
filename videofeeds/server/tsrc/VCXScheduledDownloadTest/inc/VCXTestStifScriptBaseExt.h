/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


// Version : %version: 3 %

/*
 * VCXTestStifScriptBaseExt.h
 *
 *  Created on: Apr 9, 2009
 *      Author: senbom
 */

#ifndef VCXTESTSTIFSCRIPTBASEEXT_H_
#define VCXTESTSTIFSCRIPTBASEEXT_H_

#include <TestScripterInternal.h>

class CVCXTestStifScriptBaseExt : public CScriptBase 
    {
    public:
        CVCXTestStifScriptBaseExt( CTestModuleIf& aTestModuleIf ) :
            CScriptBase( aTestModuleIf )
            {
            
            }
        
        virtual TInt RunMethodL( CStifItemParser& aItem ) { return KErrNone; };
    };


#endif /* VCXTESTSTIFSCRIPTBASEEXT_H_ */
