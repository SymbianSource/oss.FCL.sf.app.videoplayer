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
* Description:  ?Description
*
*/


#include "mdeorderrule.h"
#include "mdepropertydef.h"


EXPORT_C TMdEOrderRule::TMdEOrderRule(TOrderRuleType aType, TBool aAscending)
        : iType(aType), 
          iAscending(aAscending),
          iCaseSensitive(ETrue)
    {
    }
        

EXPORT_C TMdEOrderRule::TMdEOrderRule(const CMdEPropertyDef& aPropertyDef, TBool aAscending)
        : iType(EOrderRuleTypeProperty), iPropertyDef(&aPropertyDef), 
          iAscending(aAscending), iCaseSensitive(ETrue)
    {
    }


EXPORT_C TOrderRuleType TMdEOrderRule::Type() const
    {
    }


EXPORT_C void TMdEOrderRule::SetType(TOrderRuleType aType)
    {
    }


EXPORT_C TBool TMdEOrderRule::Ascending() const
    {
    }
    

EXPORT_C void TMdEOrderRule::SetAscending(TBool aAscending)
    {
    }


EXPORT_C const CMdEPropertyDef* TMdEOrderRule::PropertyDef() const
    {
    }


EXPORT_C void TMdEOrderRule::SetPropertyDef(const CMdEPropertyDef& aPropertyDef)
    {
    }

EXPORT_C void TMdEOrderRule::SetCaseSensitive(TBool aCaseSensitive)
    {
    }

EXPORT_C TBool TMdEOrderRule::CaseSensitive()
    {
    }

TUint32 TMdEOrderRule::RequiredBufferSize() const
	{
	}

void TMdEOrderRule::SerializeL(CMdCSerializationBuffer& aBuffer, 
		TMdCOffset& aFreespaceOffset) const
	{
	}
