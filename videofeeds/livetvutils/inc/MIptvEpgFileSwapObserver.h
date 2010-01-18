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



#ifndef __MUPTVPEGFILESWAPOBSERVER_H
#define __MUPTVPEGFILESWAPOBSERVER_H

class MIptvEpgFileSwapObserver
	{
	public:
	/**
	*	Callback to signalize completed file swap.
	*	@param	aError	Possible error occured during swap.
	*					If everything went well KErrNone, otherwise
	*					one of the Symbian wide error codes
	*	@return	None
	*/
	virtual void FileSwapComplete( TInt aError ) = 0;
	};

#endif // __MUPTVPEGFILESWAPOBSERVER_H
