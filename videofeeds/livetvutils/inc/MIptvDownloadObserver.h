/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Http download callback interface*
*/



#ifndef __MIPTVDOWNLOADOBSERVER_H
#define __MIPTVDOWNLOADOBSERVER_H

/**
*	Class to provide method(s) to deliver status information from download
*	object to the class that requested download action.
*/
class MIptvDownloadObserver
	{
	public:
	/**
	*	Method that gets called when download is finished. aError defines
	*	if download action was successful or no.
	*	@param	aError	KErrNone if successful, otherwise symbian wide error code
	*	@return	None;
	*/
	virtual void DownloadFinished( TInt aError ) = 0;

	/**
	*	Setter for modified since and ETag values received in http response
	*	header. Those values can be stored by observer to be used in next
	*	download request to avoid unnecessary downloads.
	*	@param	aModifiedSince	Last modified time value parsed from http
	*							response header
	*	@param	aETag			ETag value parsed from received http response
	*							header.
	*	@return	None
	*/
	virtual void SetModifiedSinceAndETag( const TDesC& aModifiedSince,
										  const TDesC& aETag ) = 0;
	};

#endif // __MIPTVDOWNLOADOBSERVER_H
