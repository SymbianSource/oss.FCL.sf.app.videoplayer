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




#ifndef C_CVCXNSDOWNLOADUPDATER_H
#define C_CVCXNSDOWNLOADUPDATER_H

#include <e32std.h>
#include <e32base.h>
#include "CIptvUtil.h"

class CPeriodic;
class CVcxNsContent;

/**
 *  @lib vcxnsuiengine
 *  @since S60 5.0
 * 
 *  Observer interface for the CVcxNsDownloadUpdater class,
 *  receives notification if the object with given mpx id has 
 *  changed, and needs to be refreshed.
 */
class MVcxNsDownloadUpdaterObserver
    {
    
public:

    /**
     * Notify, that the object with given mpx id has 
     * changed, and needs to be refreshed.
     * @param aMpxId Mpx media id
     */
    virtual void UpdateVideoObject( TUint32 aMpxId ) = 0;
    
    /**
     * Notifies that resume has started from beginning.
     * @param aMpxId MPX Media Id of the video object.
     */
    virtual void ResumeStartedFromBeginningL( TUint32 aMpxId ) = 0;
    };

/**
 *  @lib vcxnsuiengine
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CVcxNsDownloadUpdater ) : public CBase
    {

private:

    /**
     * TDownloadItem
     * Private inner class for storing polled downloads and their
     * previous statuses and progresses.
     */
    class TDownloadItem
        {

    public:

        /**
         * Content object. Must not be deleted until the remove is called
         */
        CVcxNsContent* iContent;
    
        /**
         * Old state, used for determine refresh need.
         */
        TUint32 iState;
        
        /**
         * Old progress, used for determine refresh need.
         */
        TUint32 iProgress;

        };

public:

    /**
     * Two-phased constructor.
     * @param aObserver Callback observer
     */
    static CVcxNsDownloadUpdater* NewL( MVcxNsDownloadUpdaterObserver* aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CVcxNsDownloadUpdater();

    /**
     * Adds download to the refresh list.
     * @note Content object must not be deleted until remove is called
     * @param aContent Content object
     */
    void AddDownload( CVcxNsContent* aContent );
    
    /**
     * Removes download from the refresh list.
     *
     * @param aContent Content object
     */
    void RemoveDownload( CVcxNsContent* aContent );
    
    /**
     * Removes all downloads from the refresh list.
     */
    void RemoveAllDownloads();
    
    /**
    * Periodic's callback.
    * @param aPtr Object pointer to be used with callback routine
    */
    static TInt Callback( TAny *aPtr );
    
    /**
     * Update the downloads.
     */
    void UpdateDownloadsL();

    /**
     * Update given download if it's status has changed.
     *  
     * @param aDownload Download item
     */
    void UpdateDownloadL( TDownloadItem& aDownload );
    
private:

    /**
     * Constructor
     * @param aObserver Callback observer
     */
    CVcxNsDownloadUpdater( MVcxNsDownloadUpdaterObserver* aObserver );

    void ConstructL();
    
    /**
     * Find the download from the downloads list.
     * 
     * @param IN  aContent Content, to which the searched content is compared.
     * @param OUT aIndex index of found download in internal array.
     * @return Is it found.
     */
    TBool FindDownload( CVcxNsContent* aContent , TInt* aIndex );

private: // data

    /**
     * MVcxNsDownloadUpdaterObserver
     */
    MVcxNsDownloadUpdaterObserver* iObserver;

    /**
     * CPeriodic. 
     * Own.
     */
    CPeriodic* iTimer;
    
    /**
     * Array of polled downloads
     * Own.
     */
    RArray<TDownloadItem> iDownloadArray;

    };

#endif // C_CVCXNSDOWNLOADUPDATER_H
