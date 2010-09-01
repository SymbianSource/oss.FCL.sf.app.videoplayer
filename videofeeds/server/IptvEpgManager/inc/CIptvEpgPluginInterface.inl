/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




inline CIptvEpgPluginInterface* CIptvEpgPluginInterface::NewL(const TUid aImplementationUid)
	{			
	TAny* interface = REComSession::CreateImplementationL(aImplementationUid, _FOFF (CIptvEpgPluginInterface, iDestructorIDKey));
	return reinterpret_cast <CIptvEpgPluginInterface*> (interface);		
	}

inline CIptvEpgPluginInterface::~CIptvEpgPluginInterface()
	{
	REComSession::DestroyedImplementation(iDestructorIDKey);
	}
