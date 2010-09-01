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


#include "mdeitem.h"


CMdEItem::CMdEItem(CMdESession* aSession, TItemId aId)
		: iSession(aSession), iId(aId)
	{
	}


void CMdEItem::ItemBaseConstruct()
	{
	}


CMdEItem::~CMdEItem()
	{
	}


EXPORT_C TBool CMdEItem::BelongsToSession() const
	{
	}


EXPORT_C CMdESession& CMdEItem::Session() const
	{
	}


void CMdEItem::CheckSession() const
	{
	}


void CMdEItem::SetSession(CMdESession& aSession)
	{
	}


EXPORT_C TBool CMdEItem::InDatabase() const
	{
	}


EXPORT_C TItemId CMdEItem::Id() const
	{
	}


void CMdEItem::SetId(TItemId aId)
	{
	}

