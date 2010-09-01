/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class to provide channel data to*
*/




#ifndef __CIPTVEPGCHANNEL_H__
#define __CIPTVEPGCHANNEL_H__

#include <e32std.h>
#include <e32base.h>
#include <fbs.h>

#include "IptvLiveEpgConstants.h"

/**
*	"Carrier" class to contain data for single channel occurence in EPG xml
*	file. This class is used to communicate data from xml content handler
*	to EPG manager to be stored to the database.
*/
class CIptvEpgChannel : public CBase
	{
	public:
	/**
	*	Symbian two phase constructor
	*	@param	None
	*	@return	Created instance of CIptvEpgChannel
	*/
	IMPORT_C static CIptvEpgChannel* NewL();
	
	/**
	*	Destructor
	*/
	virtual ~CIptvEpgChannel();
	
	public: // New functions
	/** 
	* LinearOrderByOrderNum. Static function to compare two channel objects
	* based on their iOrder member variable. If first and second channels
	* both have iOrder == KMaxTUint32 (deactivated) then they are compared by their
	* name.
	* @param aFirst First channel object
	* @param aSecond Second channel object
	* @return Positive integer if first channel's order number is greater
	* than second one', zero if order numbers are equal, negative value if
	* first channel's order is less than second one's. If either channel's
    * iOrder is KMaxTUint32 or zero, the logic is inverse. If both orders are 
	* deactivated (KMaxTUint32) then the comparison is made using channel name.
	*/
	static TInt LinearOrderByOrderNum( const CIptvEpgChannel& aFirst, 
									   const CIptvEpgChannel& aSecond );

	private:
	/**
	*	Default constructor
	*/
	CIptvEpgChannel();

	/**
	*	Symbian second phase constructor which might contain leaving code
	*/	
	void ConstructL();
	
	public:
	// Setters
	
	IMPORT_C void SetChannelId( TInt64 aChannelId );
	
	IMPORT_C void SetServiceId( TUint32 aServiceId );
	
	IMPORT_C void SetChannelName( HBufC* aChannelName );
	
	IMPORT_C void SetChannelLogoPath( HBufC* aLogoPath );
	
	IMPORT_C void SetChannelDescription( HBufC* aDescription );
	
	IMPORT_C void SetChannelURI( HBufC* aURI );
	
	IMPORT_C void SetChannelSDP( HBufC* aSDP );
	
	IMPORT_C void SetChannelOrder( const TUint32 aOrder );

	IMPORT_C void SetChannelIconL( const CFbsBitmap& aBitmap );
	
	// Getters
	
	IMPORT_C TUint32 ServiceId() const;
	
	IMPORT_C TInt64 ChannelId() const;
	
	IMPORT_C TDesC& ChannelName() const;
	
	IMPORT_C TDesC& ChannelLogoPath() const;
	
	IMPORT_C TDesC& ChannelDescription() const;
	
	IMPORT_C TDesC& ChannelURI() const;
	
	IMPORT_C TDesC& ChannelSDP() const;

	IMPORT_C TUint32 ChannelOrder() const;
	
	IMPORT_C CFbsBitmap* ChannelIcon();

	// Instance variables
	private:
	TUint32					iOrder;
	
	/** Id of the channel */
	TInt64					iChannelId;
	
	/** Id of the service provider */
	TUint32					iServiceProviderId;
	
	/** Descriptor containing channel's name */
	HBufC* 					iChannelName;

	/** Descriptor containing the logo path */
	HBufC*					iLogoPath;
	
	/** Descriptor containing the channel description */
	HBufC*					iDescription;
	
	/** Descriptor containing the URI of the channel */
	HBufC*					iURI;
	
	/** Descriptor containing the SDP of the channel
		Note, that this descriptor can be quite long */
	HBufC*					iSDP;

	/** Channel icon bitmap, set and used in the UI side */
	CFbsBitmap*				iBitmap;
	};

#endif // __CIPTVEPGCHANNEL_H__

// End of file
