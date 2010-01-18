/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef C_VCXNSNOTIFICATIONLAUNCHER_H
#define C_VCXNSNOTIFICATIONLAUNCHER_H

#include <e32std.h>
#include <e32base.h>
#include <MediatorEventConsumer.h>

class CVcxNsChMCategoryListener;
class CVcxNsChPublishableData;
class CVcxNsChPublisher;

/**
 *  CVcxNsNotificationLauncher
 * 
 *  @lib vcxnscontentharvesterplugin.dll
 *  @since S60 5.0
 */
class CVcxNsNotificationLauncher : public CBase,
                                   public MMediatorEventObserver
    {

public:

    static CVcxNsNotificationLauncher* NewL( CVcxNsChPublishableData* aPublishableData,
                                             CVcxNsChPublisher* aPublisher );

    virtual ~CVcxNsNotificationLauncher();
    
    /**
     * From MMediatorEventObserver
     * 
     * Catches the events
     */
    void MediatorEventL( TUid aDomain,
                         TUid aCategory, 
                         TInt aEventId, 
                         const TDesC8& aData );

private:

    CVcxNsNotificationLauncher( CVcxNsChPublishableData* aPublishableData,
                                CVcxNsChPublisher* aPublisher );

    void ConstructL();
        

public:
    
    /**
     * Launch download completed notification.
	 *
	 * @param aMedia Downloaded video.
     */
    void DlCompletedNotificationL( const CMPXMedia& aMedia );
    
    /**
     * Launches soft notification.         
     */
    void LaunchExitingNotificationL();
                            
private: // New functions
    
    /**
    * Loads a resource stringL.
    * 
    * @param  Resource id.
    * @return the string. ownership transferred to caller.
    */
    HBufC* LoadResourceStringL( TInt aResourceId );

// Data members
    
    /**
    * Mediator helper object
    */
    CVcxNsChMCategoryListener* iCatListener;
    
    /**
    * pointer to plugin data, not owned
    */
    CVcxNsChPublishableData* iPublishableData;
 
    /**
     * Publisher object used to publish data from CVcxNsChPublishableData
     * to service api 
     * NOT OWN
     */
    CVcxNsChPublisher* iPublisher;

    /**
    * Localized string for popup
    */
    HBufC* iDownloadCompletedBuffer;
    
    /**
    * Localized string for popup
    */
    HBufC* iDownloadsContinueBuffer;
    
    };

#endif // C_VCXNSNOTIFICATIONLAUNCHER_H
