/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stub declaration for HAL
*
*/

// Version : %version:  1 %



#ifndef __HAL_H__
#define __HAL_H__

#include <e32base.h>	
#include <e32std.h>	 

class HAL : public CBase
	{

public:

	static TInt Get(TAttribute aAttribute, TInt& aValue);

	
	static TInt Set(TAttribute aAttribute, TInt aValue);
	

	};


#endif

