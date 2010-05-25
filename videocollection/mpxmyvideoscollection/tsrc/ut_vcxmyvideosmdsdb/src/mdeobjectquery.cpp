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
* Description:  Interface for object query
*
*/


#include "mdeobjectquery.h"
#include "mdeobject.h"


CMdEObjectQuery::CMdEObjectQuery(CMdESession& aSession, CMdENamespaceDef& aNamespaceDef, 
		CMdEObjectDef& aObjectDef, RPointerArray<CMdEObjectDef>* aObjectDefs)
        : CMdEQuery(EQueryTypeObject, aSession, aNamespaceDef), 
          iObjectDef(aObjectDef), iObjectDefs(aObjectDefs),
          iFreetextsInResult( ETrue )
    {
    }
CMdEObjectQuery::CMdEObjectQuery()
        : iObjectDef(*(CMdEObjectDef*)NULL), iObjectDefs((RPointerArray<CMdEObjectDef>*)NULL),
          iFreetextsInResult( ETrue )
    {
    }

void CMdEObjectQuery::ObjectQueryConstructL()
    {
    }

CMdEObjectQuery::~CMdEObjectQuery()
	{
	}

EXPORT_C CMdEObject& CMdEObjectQuery::Result(TInt aIndex) const
    {
    }

EXPORT_C CMdEObjectDef& CMdEObjectQuery::ObjectDef() const
	{
	}

EXPORT_C void CMdEObjectQuery::AddPropertyFilterL(const CMdEPropertyDef* aProperty)
    {
    }

EXPORT_C void CMdEObjectQuery::ResetPropertyFilter()
    {
    }
    
RPointerArray<CMdEPropertyDef>& CMdEObjectQuery::PropertyFilter()
	{
	}

RPointerArray<CMdEObjectDef>* CMdEObjectQuery::ObjectDefs() const
	{
	}

void CMdEObjectQuery::DoCancel() 
    {
    }
