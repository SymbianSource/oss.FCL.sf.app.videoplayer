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
* Description:    Class to provide program data to*
*/




#ifndef __CIPTVEPGPROGRAM_H__
#define __CIPTVEPGPROGRAM_H__

#include <e32std.h>
#include <e32base.h>

#include "IptvLiveEpgConstants.h"

/**
*	"Carrier" class to contain data for single program occurence in EPG xml
*	file. This class is used to communicate data from xml content handler
*	to EPG manager to be stored to the database.
*/
class CIptvEpgProgram : public CBase
	{
	public:
	/**
	*	Symbian two phase constructor
	*	@param	None
	*	@return	Created instance of CLiveTvEpgProgram
	*/
	IMPORT_C static CIptvEpgProgram* NewL();
	
	/**
	*	Destructor
	*/
	virtual ~CIptvEpgProgram();

	public: // new methods
	/**
	 * method for sorting by programid
	 */
	static TInt LinearOrderOfProgramsById( const CIptvEpgProgram &p1, 
										   const CIptvEpgProgram &p2);

	/**
	 * method for sorting by program name
	 */
	static TInt LinearOrderOfProgramsByName( const CIptvEpgProgram &p1, 
											 const CIptvEpgProgram &p2);

	protected: // Constructors
	/**
	*	Default constructor
	*/
	CIptvEpgProgram();
	
	/**
	*	Symbian second phase constructor which might contain leaving code
	*/	
	void ConstructL();
	
	
	public:
	// Setters for instance variables
	// <!NOTE>
	// ALL THESE SETTER FUNCTIONS TAKES THE OWNERSHIP OF THE GIVEN HEAP
	// DESCRIPTOR, SO CALLER MUST NOT DELETE HBufC POINTERS PASSED TO
	// THIS CLASS
	// </!NOTE>
	//
	
	/**
	*	Setter for the service id this program belongs to.
	*	@param	aServiceId	Id of the service which this program belongs
	*	@return	None
	*/
	IMPORT_C void SetServiceId( TUint32 aServiceId );
	
	/**
	*	Setter for the channel id this program belongs
	*	@param	aChannelId	Id of the channel which this program belongs
	*	@return	None
	*/
	IMPORT_C void SetChannelId( TInt64 aChannelId );
	
	/**
	*	Setter for the program id of this program object
	*	@param	aProgramId	Id of this program
	*	@return	None
	*/
	IMPORT_C void SetProgramId( TInt64 aProgramId );
	
	/**
	*	Setter for the program rtsp uri
	*	@param	aURI	Descriptor containing the rtsp address for
	*					this program. Ownership of given descriptor
	*					has been transferred to this class so it MUST NOT
	*					be deleted where this method is called.
	*	@return	None
	*/
	IMPORT_C void SetProgramURI( HBufC* aURI );
	
	/**
	*	Setter for the program rtsp uri
	*	@param	aSDP	Descriptor containing the SDP (Session Description
	*					Protocol) for this program.
	*					Ownership of given descriptor has been transferred
	*					to this class so it MUST NOT be deleted where this
	*					method is called.
	*	@return	None
	*/
	IMPORT_C void SetProgramSDP( HBufC* aSDP );
	
	/**
	*	Setter for the program genre
	*	@param	aGenre	Descriptor containing the genre of this program
	*					Ownership of given descriptor has been transferred
	*					to this class so it MUST NOT be deleted where this
	*					method is called.
	*	@return	None
	*/
	IMPORT_C void SetProgramGenre( HBufC* aGenre );
	
	/**
	*	Setter for the program name
	*	@param	aName	Descriptor containing the name of this program
	*					Ownership of given descriptor has been transferred
	*					to this class so it MUST NOT be deleted where this
	*					method is called.
	*	@return	None
	*/
	IMPORT_C void SetProgramName( HBufC* aName );
	
	/**
	*	Setter for the program description
	*	@param	aName	Descriptor containing the description of this program
	*					Ownership of given descriptor has been transferred
	*					to this class so it MUST NOT be deleted where this
	*					method is called.
	*	@return	None
	*/
	IMPORT_C void SetProgramDescription( HBufC* aDescription );
	
	/**
	*	Setter for the program language
	*	@param	aName	Descriptor containing the language of this program
	*					Ownership of given descriptor has been transferred
	*					to this class so it MUST NOT be deleted where this
	*					method is called.
	*	@return	None
	*/
	IMPORT_C void SetProgramLanguage( HBufC* aLanguage );
	
	/**
	*	Setter for the program parental rating
	*	@param	aName	Descriptor containing the parental rating of this
	*					program.
	*					Ownership of given descriptor has been transferred
	*					to this class so it MUST NOT be deleted where this
	*					method is called.
	*	@return	None
	*/
	IMPORT_C void SetProgramParentalRating( HBufC* aParentalRating );
	
	
	// Getters for the instance data
	
	/**
	*	Getter for this program's service id
	*	@param	None
	*	@return	On return contains the id of the service which this program
	*			belongs.
	*/
	IMPORT_C TUint32 ServiceId() const;
	
	/**
	*	Getter for this program's channel id
	*	@param	None
	*	@return	On return contains the id of the channel which this program
	*			belongs.
	*/
	IMPORT_C TInt64 ChannelId() const;
	
	/**
	*	Getter for this program's id
	*	@param	None
	*	@return	On return contains the id of this program
	*/
	IMPORT_C TInt64 ProgramId() const;
	
	/**
	*	Getter for this program's rtsp URI
	*	@param	None
	*	@return	Reference to the descriptor containing the rtsp URI of this
	*			program.
	*/
	IMPORT_C TDesC& ProgramURI() const;
	
	/**
	*	Getter for this program's SDP (Session Description Protocol)
	*	@param	None
	*	@return	Reference to the descriptor containing the SDP of this
	*			program.
	*/
	IMPORT_C TDesC& ProgramSDP() const;
	
	/**
	*	Getter for this program's genre
	*	@param	None
	*	@return	Reference to the descriptor containing the genre of this
	*			program.
	*/
	IMPORT_C TDesC& ProgramGenre() const;
	
	/**
	*	Getter for this program's name
	*	@param	None
	*	@return	Reference to the descriptor containing the name of this
	*			program.
	*/
	IMPORT_C TDesC& ProgramName() const;
	
	/**
	*	Getter for this program's description
	*	@param	None
	*	@return	Reference to the descriptor containing the description of this
	*			program.
	*/
	IMPORT_C TDesC& ProgramDescription() const;
	
	/**
	*	Getter for this program's language
	*	@param	None
	*	@return	Reference to the descriptor containing the language of this
	*			program.
	*/
	IMPORT_C TDesC& ProgramLanguage() const;
	
	/**
	*	Getter for this program's parental rating
	*	@param	None
	*	@return	Reference to the descriptor containing the parental rating of
	*			this program.
	*/
	IMPORT_C TDesC& ProgramParentalRating() const;
	
	// Instance variables
	private:
	/** Id of the service where this program belongs */
	TUint32					iServiceId;
	
	/** Id of the channel this program belong */
	TInt64					iChannelId;
	
	/** Id of the program */
	TInt64					iProgramId;
	
	/** Descriptor containing the URI of the program */
	HBufC*					iURI;
	
	/** Descriptor containing the SDP of the program
		Note, that this descriptor can be quite long */
	HBufC*					iSDP;
	
	/** Descriptor containing the genre of the program */
	HBufC*					iGenre;
	
	/** Descriptor containing the name of the program */
	HBufC*					iName;
	
	/** Descriptor containing the name of the program */
	HBufC*					iDescription;
	
	/** Descriptor containing the language of the program */
	HBufC*					iLanguage;
	
	/** Descriptor containing the parental rating of the program */
	HBufC*					iParentalRating;
	};
	
#endif //__CIPTVEPGPROGRAM_H__

// End of file
