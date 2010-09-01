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




#include "CIptvMediaContent.h"

#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvContentIdMediaContentItem.h"

// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::NewL
// 
// ---------------------------------------------------------
CIptvContentIdMediaContentItem* CIptvContentIdMediaContentItem::NewL()
	{
	CIptvContentIdMediaContentItem* self = new (ELeave) CIptvContentIdMediaContentItem();

	return self;
	}

// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::CIptvContentIdMediaContentItem
// 
// ---------------------------------------------------------
CIptvContentIdMediaContentItem::CIptvContentIdMediaContentItem()
	{
	}

// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::~CIptvContentIdMediaContentItem
// 
// ---------------------------------------------------------
CIptvContentIdMediaContentItem::~CIptvContentIdMediaContentItem()
	{
	iMediaContentArray.ResetAndDestroy();
	}

// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::SetMediaContentArrayL
// 
// ---------------------------------------------------------
void CIptvContentIdMediaContentItem::SetMediaContentArrayL(
        RPointerArray<CIptvMediaContent>& aArray )
	{
	for( TInt i = 0; i < aArray.Count(); i++ )
		{
		CIptvMediaContent* original = aArray[i];
		CIptvMediaContent* newContent = CIptvMediaContent::NewL();
		CleanupStack::PushL( newContent );
		CopyMediaContentL( *original, newContent );
		newContent->iContentKey = iContentId;
		iMediaContentArray.AppendL( newContent );
		CleanupStack::Pop( newContent );
		}
	}
	
// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::CopyMediaContentL
// 
// ---------------------------------------------------------
void CIptvContentIdMediaContentItem::CopyMediaContentL( CIptvMediaContent& aOriginal,
														CIptvMediaContent* aNew )
	{
    aNew->iContentKey = aOriginal.iContentKey;
    aNew->iIndex = aOriginal.iIndex;
    aNew->iDownloadType = aOriginal.iDownloadType;
    aNew->iFileId = aOriginal.iFileId;
    aNew->iDriveId = aOriginal.iDriveId;
    aNew->iFileSize = aOriginal.iFileSize; 
    aNew->iMimeType.Copy( aOriginal.iMimeType );
    aNew->iMedium.Copy( aOriginal.iMedium );
    aNew->iIsDefault = aOriginal.iIsDefault;
    aNew->iExpression = aOriginal.iExpression;
    aNew->iBitrate = aOriginal.iBitrate;
    aNew->iFramerate = aOriginal.iFramerate;
    aNew->iSamplingrate = aOriginal.iSamplingrate;
    aNew->iDuration = aOriginal.iDuration;  
    aNew->iHeight = aOriginal.iHeight;
    aNew->iWidth = aOriginal.iWidth;
    aNew->iLanguage.Copy( aOriginal.iLanguage );
    aNew->iLastPosition = aOriginal.iLastPosition;
    aNew->SetMediaContentUrlL( aOriginal.GetMediaContentUrl() );
	}

	
// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::GetMediaContentsL
// 
// ---------------------------------------------------------
void CIptvContentIdMediaContentItem::GetMediaContentsL( RPointerArray<CIptvMediaContent>& aArray )
	{
	for( TInt i = 0; i < iMediaContentArray.Count(); i++ )
		{
		CIptvMediaContent* original = iMediaContentArray[i];
		CIptvMediaContent* newContent = CIptvMediaContent::NewL();
		CleanupStack::PushL( newContent );
		CopyMediaContentL( *original, newContent );
		aArray.AppendL( newContent );
		CleanupStack::Pop( newContent );
		}
	}

// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::MediaContentByIndexL
// 
// ---------------------------------------------------------
CIptvMediaContent* CIptvContentIdMediaContentItem::MediaContentByIndexL( TInt aIndex )
	{
	CIptvMediaContent* returnContent = NULL;
	if( aIndex < iMediaContentArray.Count() )
		{
		CIptvMediaContent* content = iMediaContentArray[aIndex];
		CleanupStack::PushL( content );
		returnContent = CIptvMediaContent::NewL();
		CopyMediaContentL( *content, returnContent );
		CleanupStack::Pop( content );
		}
	return returnContent;
	}

// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::SetContentId
// 
// ---------------------------------------------------------
void CIptvContentIdMediaContentItem::SetContentId( TUint32 aContentId )
	{
	iContentId = aContentId;
	}
	
// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::ContentId
// 
// ---------------------------------------------------------
TUint32 CIptvContentIdMediaContentItem::ContentId() const
	{
	return iContentId;
	}
	
// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::MediaContentCount
// 
// ---------------------------------------------------------
TInt CIptvContentIdMediaContentItem::MediaContentCount() const
	{
	return iMediaContentArray.Count();
	}

// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::ResetGlobalId
// 
// ---------------------------------------------------------
TInt CIptvContentIdMediaContentItem::ResetGlobalId( CIptvMyVideosGlobalFileId& aId )
	{
	TInt retVal = KErrNotFound;
	for( TInt i = 0; i < iMediaContentArray.Count(); i++ )
		{
		CIptvMediaContent* content = iMediaContentArray[i]; // Owned by the array
		if( content->iFileId == aId.iFileId &&
			content->iDriveId == aId.iDrive )
			{
			content->iFileId = 0;
			content->iDriveId = 0;
			retVal = KErrNone;
			i = iMediaContentArray.Count(); // break; would've done the same
			}
		}
	return retVal;
	}
	
// ---------------------------------------------------------
// CIptvContentIdMediaContentItem::SetGlobalId
// 
// ---------------------------------------------------------
TInt CIptvContentIdMediaContentItem::SetGlobalId( TUint32 aContentKey,
					  							  CIptvMyVideosGlobalFileId& aId,
					  							  TUint32 aIndex )
	{
	TInt retVal = KErrNotFound;
	if( aContentKey == iContentId && aIndex < iMediaContentArray.Count() )
		{
		CIptvMediaContent* content = iMediaContentArray[aIndex]; // Owned by the array
		content->iFileId = aId.iFileId;
		content->iDriveId = aId.iDrive;
		retVal = KErrNone;
		}
	return retVal;
	}

	
// End of file
