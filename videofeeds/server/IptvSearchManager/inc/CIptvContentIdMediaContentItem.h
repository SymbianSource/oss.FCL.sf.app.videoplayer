/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef CIPTVCONTENTIDMEDIACONTENTITEM_H
#define CIPTVCONTENTIDMEDIACONTENTITEM_H

#include <e32base.h>

class CIptvMediaContent;
class CIptvMyVideosGlobalFileId;

/**
*	This class is used for storage purposes to store key +
*	media contents defined for that key.
*	There might be sevaral CIptvMediaContent objects constructed
*	by the plugin for one individual content so this is the class
*	that represents the media content set for that content id.
*/
class CIptvContentIdMediaContentItem : public CBase
	{
	public:
	/**
	*	Symbian two phase constructor
	*/
	static CIptvContentIdMediaContentItem* NewL();
	
	/**
	*	Destructor
	*/
	~CIptvContentIdMediaContentItem();
	
	// SETTERS
	/**
	*	Setter for media content array. Ownership of aArray IS NOT
	*	transferred to this class but we do instead copy of the 
	*	array content to be stored in this class. Therefore
	*	CIptvContentIdMediaContentItem is not responsible for deleting
	*	the content of aArray elements
	*	@param	aArray	Array of media contents to be stored to this
	*					item
	*	@return	None
	*/
	void SetMediaContentArrayL( RPointerArray<CIptvMediaContent>& aArray );
	
	/**
	*	Setter for content id that is used to recognize the media
	*	content when requested by the UI.
	*	@param	aContentId	Id of the content this classes media content
	*						refers to
	*	@return	None
	*/
	void SetContentId( TUint32 aContentId );

	// GETTERS
	
	/**
	*	Getter method for media contents. This method copies the contents
	*	of iMediaContentArray to aArray. Caller is responsible for freeing
	*	the memory allocated in aArray.
	*	@param	aArray	Array to be filled with this classes media content
	*					items
	*	@return	None
	*/
	void GetMediaContentsL( RPointerArray<CIptvMediaContent>& aArray );
	
	/**
	*	Getter for media content by given index. This method returns
	*	iMediaContentArray[aIndex] item to the caller. New object is created
	*	and ownership is transferred to the caller.
	*	@param	aIndex	Index of the item to be retrieved
	*	@return	On return contains a COPY of the media content found
	*			on the given index.
	*/
	CIptvMediaContent* MediaContentByIndexL( TInt aIndex );
	
	/**
	*	Getter for media content id assigned for this class.
	*	@param	None
	*	@return	Content id of this object
	*/
	TUint32 ContentId() const;
	
	/**
	*	Method to return the amount of media contents inside the
	*	iMediaContentArray object.
	*	@param	None
	*	@return	Amount of items in iMediaContentArray
	*/
	TInt MediaContentCount() const;
	
	/**
	*	Resets the global file id if found inside an array of this
	*	class (array being iMediaContentArray).
	*	@param	aId		Global file id values to be searched
	*	@return	KErrNone if successful, KErrNotFound if the aId values
	*			was not found inside the iMediaContentArray, otherwise
	*			some Symbian error code
	*/
	TInt ResetGlobalId( CIptvMyVideosGlobalFileId& aId );
	
	/**
	*	Sets the global file id for specified content key.
	*	@param	aContentKey		Id specifying the contentid that we are
	*							searching. This values is compared to
	*							iContentId of this class
	*	@param	aId				Class containing the file id and drive
	*							definition for the key. This value is
	*							located in CIptvMediaContent items stored
	*							to the iMediaContentArray
	*	@param	aIndex			Index specifying the index inside the
	*							iMediaContentArray
	*	@return	KErrNone if successful, KErrNotFound if aContentId != iContentId,
	*			otherwise some Symbian error code.
	*/
	TInt SetGlobalId( TUint32 aContentKey,
					  CIptvMyVideosGlobalFileId& aId,
					  TUint32 aIndex );
	
	private:

	/**
	*	Default constructor
	*/
	CIptvContentIdMediaContentItem();
	
	/**
	*	Utility method to copy contents from one class to another
	*	@param	aOriginal	Class where from data is copied
	*	@param	aNew		Class where to data is copied
	*	@return	None
	*/
	void CopyMediaContentL( CIptvMediaContent& aOriginal, CIptvMediaContent* aNew );
	
	private:
	/**
	*	Array containing all the media contents assigned for this content id
	*/
	RPointerArray<CIptvMediaContent>	iMediaContentArray;
	
	/**
	*	Content id defining the identity of this class
	*/
	TUint32 							iContentId;
	};

#endif // CIPTVCONTENTIDMEDIACONTENTITEM_H
