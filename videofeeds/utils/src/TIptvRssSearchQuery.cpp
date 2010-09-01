/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Data class for RSS search query.*
*/




#include "TIptvRssSearchQuery.h"
#include <s32mem.h>

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C TIptvRssSearchQuery::TIptvRssSearchQuery()
	{
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C TIptvRssSearchQuery::~TIptvRssSearchQuery()
	{
	}

// ---------------------------------------------------------------------------
// Getter.
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC& TIptvRssSearchQuery::SearchString()
	{
	return iSearchString;
	}

// ---------------------------------------------------------------------------
// Setter.
// ---------------------------------------------------------------------------
//
EXPORT_C void TIptvRssSearchQuery::SetSearchString( TDesC& aSearchString )
	{
	iSearchString.Zero();
	TInt stringLength = 
	    Min( aSearchString.Length(), KIptvRssSearchMaxSearchStringLength );
	iSearchString.Copy( aSearchString.Mid( 0, stringLength ) );
	}
