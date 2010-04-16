// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:  Stub declaration for central repository
//

#ifndef __CENTRALREPOSITORY_H__
#define __CENTRALREPOSITORY_H__

#include <e32base.h>


class CRepository : public CBase
	{
public:

	static CRepository* NewLC(TUid aRepositoryUid);

	virtual ~CRepository();

	TInt Get(TUint32 aKey, TInt& aValue);

	};

#endif // __CENTRALREPOSITORY_H__
