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




#ifndef MVCXNSCONTENTPROVIDEROBSERVER_H
#define MVCXNSCONTENTPROVIDEROBSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "CIptvUtil.h" 

class CVcxNsContent;
// CLASS DECLARATION

/**
 *  MVcxNsContentProviderObserver
 * 
 *  @lib vcxnsuiengine.lib
 */
class MVcxNsContentProviderObserver
    {
    
public:

    /**
     * Notify content's data has been updated.
     *
     * @param aIndex Updated item's index
     */
    virtual void ContentUpdated( TInt aIndex ) = 0;

    /**
     * Full view data needs to be updated.
     */
    virtual void RefreshView() = 0;
    
    /**
     * Application state needs to be updated.
     */
    virtual void HandleAppStateChangedL() = 0;
   
    /**
     * Epg update is ongoing, show the notification.
     * When downloading and parsing the xml, both parameters are 0.
     * 
     * @param aShow If ETrue, show the note, if EFalse, hide it.
     * @param aDownloadedTbns count of downloaded thumbnails.
     * @param aTotalTbns Total count of thumbnails to be downloaded.
     */
    virtual void ShowUpdatingNoteL( 
            TBool aShow, 
            TInt aDownloadedTbns, 
            TInt aTotalTbns ) = 0;
    
    /**
     * Handle error 
     * 
     * @param aError Error to handle in UI
     */
    virtual void HandleUpdateErrorL( TInt aError ) = 0;

    /**
     * Handle download error 
     * 
     * @param aError Error to handle in UI
     * @param aContent Details of the video which download has failed.
     */
    virtual void HandleDownloadErrorL( 
        TIptvDlError aError, 
        CVcxNsContent& aContent ) = 0;

    };

#endif // MVCXNSCONTENTPROVIDEROBSERVER_H
