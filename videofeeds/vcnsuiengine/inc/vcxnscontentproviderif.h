/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef M_VCXNSAPPCONTENTPROVIDERIF_H
#define M_VCXNSAPPCONTENTPROVIDERIF_H

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION
class MVcxNsContentProviderObserver;
class CVcxNsContent;

/**
* Class MVcxNsContentProviderIf 
* Interface for using UI engine's content provider
*
* @lib vcxnsuiengine.lib
*/
class MVcxNsContentProviderIf
    {

public:

    /**
     * RegisterObserver.
     * @param aObserver Observer
     */
    virtual void RegisterObserver( MVcxNsContentProviderObserver* aObserver ) = 0;

    /**
     * DeRegisterObserver.
     * @param aObserver Observer
     */
    virtual void DeRegisterObserver( MVcxNsContentProviderObserver* aObserver ) = 0;

    /**
     * Get contents with highlighted position
     * @param aHighlight On return, highlight index for content list
     * @return Array of contents.
     */
    virtual RPointerArray<CVcxNsContent>& GetContentsL( TInt& aHighlight ) = 0;

    /**
     * Gets contents.
     *
     * @return Vod contents
     */
    virtual RPointerArray<CVcxNsContent>& GetContentsL() = 0;

    /**
     * Starts the content update.
     *
     * @return KErrNone or one of the system wide error codes.
     */
    virtual TInt RefreshContentsL() = 0;
    
    /**
     * Starts download of given content item.
     * @param aIndex Index of the content to be downloaded
     * @return KErrNone or one of the system wide error codes.
     */
    virtual TInt StartDownloadL( TInt aIndex ) = 0;
    
    /**
     * Pause download of given content item.
     * 
     * @param aIndex Index of the content item.
     * @return KErrNone or one of the system wide error codes.
     */
    virtual TInt PauseDownloadL( TInt aIndex ) = 0;
    
    /**
     * Resume download of given content item.
     * 
     * @param aIndex Index of the content item.
     * @return KErrNone or one of the system wide error codes.
     */
    virtual TInt ResumeDownloadL( TInt aIndex ) = 0;
    
    /**
     * Resume download of given content item.
     * 
     * @param aContent Content details
     * @return KErrNone or one of the system wide error codes.
     */
    virtual TInt ResumeDownloadL( CVcxNsContent& aContent ) = 0;
    
    /**
     * Cancels downloading of given content item.
     * 
     * @param aIndex Index of the content item.
     * @return KErrNone or one of the system wide error codes.
     */
    virtual TInt CancelDownloadL( TInt aIndex ) = 0;

    
    /**
     * Checks if video has more details than just name
     * 
     * @param aSelected Index of selected item
     * @return Does the video have other details than just name
     */
    virtual TBool HasVideoDetailsL( TInt aSelected ) = 0;
    
    /**
     * Fetches full video details
     * 
     * @param aSelected Index of selected item
      * @return Content full details
     */
    virtual CVcxNsContent* GetFullDetailsL ( TInt aSelected ) = 0;
    
    /**
     * Save content view highlight for currently active category. 
     * @param aHighlight Highlight index to save.
     */
    virtual void SetContentHighlight( TInt aHighlight ) = 0;
    
    };

#endif // MVCXNSAPPCONTENTPROVIDERIF_H
