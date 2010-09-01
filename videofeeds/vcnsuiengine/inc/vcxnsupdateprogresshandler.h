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




#ifndef MVCXNSUPDATEPROGRESSHANDLER_H
#define MVCXNSUPDATEPROGRESSHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "vcxnsuiengine.h"
#include "vcxnsupdateprogressif.h"
#include "MIptvTimerObserver.h"

class CIptvTimer;
// CLASS DECLARATION

/**
 *  MVcxNsUpdateProgressObserver
 * 
 *  @lib vcxnsuiengine.lib
 */
class MVcxNsUpdateProgressObserver
    {
public:

    /**
     * Handle feed update progress change
     * Informs which index in list needs to be updated with new progress
     * @param aIndex Index where progress needs to be updated
     * @param aProgress Progress as percentage 
     *
     */
    virtual void HandleFeedUpdateProgressL( TInt aIndex, TInt aProgress ) = 0;
    };

/**
* CVcxNsUpdateProgressHandler class
*
* @lib vcxnsuiengine.lib
*/
NONSHARABLE_CLASS( CVcxNsUpdateProgressHandler ): public CBase,
                                                  public MVcxNsUpdateProgressIf,
                                                  public MIptvTimerObserver
    {

public:
    
    /**
     * Constructor
     * @param aUiEngine Ui engine
     */
    static CVcxNsUpdateProgressHandler* NewL( CVcxNsUiEngine& aUiEngine );

    /**
     * Destructor
     */
    virtual ~CVcxNsUpdateProgressHandler();
    
public:

    /**
     * From MVcxNsUpdateProgressIf
     * RegisterObserver.
     * @param aObserver Observer 
     */
    void RegisterObserver( MVcxNsUpdateProgressObserver* aObserver );

    /**
     * From MVcxNsUpdateProgressIf
     * DeRegisterObserver.
     * @param aObserver Observer 
     */
    void DeRegisterObserver( MVcxNsUpdateProgressObserver* aObserver );

    /**
     * From MVcxNsUpdateProgressIf
     * Get progress.
     * @param aServiceId Service id which progress is requested
     * @return Progress between 0-99, KErrNotFound if not available
     */
    TInt GetProgress( TUint32 aServiceId );

    /**
     * From MIptvTimerObserver
     * The function to be called when a timeout occurs.
     *
     * @param aTimer Pointer to timer which expired.
     */
    void TimerExpired( CIptvTimer* aTimer );

    /**
     * Cancel progress handler. 
     * 
     */
    void Cancel();
    
public:
    
    /**
     * Update progress value according to status.
     * @param aServiceId Service id
     * @param aUpdateStatusMsg Update status message
     * @param aInfo Additional info with status
     */
    void HandleUpdateStatusChangeL( 
             TUint32 aServiceId,
             TInt aUpdateStatusMsg,
             TInt aInfo );
    
private:
     
    /**
     * Default constructor
     * @param aUiEngine Reference to ui engine.
     */
    CVcxNsUpdateProgressHandler( CVcxNsUiEngine& aUiEngine );
    
    /**
     * Check if thumbnails are allowed.
     * @return ETrue if thumbnail download is allowed
     * otherwise EFalse.
     */
    TBool ThumbnailsAllowedL() const;
    
    /**
     * Start progress update timer
     */
    void StartProgressUpdateTimerL( );
    
    /**
     * Refresh progress with specified progress value.
     * Informs observer of the change.
     * @param aProgress Progress value 
     */
    void RefreshProgressL( TInt aProgress );

private:

// Data

    /**
     * Update progress observer. 
     * Not Own.
     */
    MVcxNsUpdateProgressObserver* iProgressObserver;
    
    /**
     * Reference to UI engine.
     */
    CVcxNsUiEngine& iUiEngine;

    /**
     * Progress timer.  
     * Own.
     */
    CIptvTimer* iProgressTimer;
    
    /**
     * Flag for thumbnail setting.  
     * ETrue if thumbnail download in content update is allowed.
     * By default EFalse.
     */
    TBool iThumbnails;

    /**
     * Previously updated progress value.  
     * 
     */
    TInt iPreviousProgress;
    
    /**
     * Saves information on how much progress is increased  
     * when one thumbnail has been downloaded.
     * If value is 0, progress is calculated based on 
     * iThumbnailsPercent. 
     */
    TInt iProgressPerThumbnail;
    
    /**
     * Information on how many thumbnails must be   
     * downloaded to increase progress by percent.
     */
    TInt iThumbnailsInPercent;

    /**
     * Counter for downloaded thumbnails.  
     */
    TInt iThumbnailCounter;

    /**
     * Service id of currently updated service.  
     */
    TUint32 iCurrentServiceId;
    
    /**
     * Current status of content update.
     */
    TInt iCurrentStatus;
    };


#endif // MVCXNSUPDATEPROGRESSHANDLER_H
