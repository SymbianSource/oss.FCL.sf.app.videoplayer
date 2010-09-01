/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Query dialog class*
*/




#ifndef CIPTVREMOVESERVICEQUERYDIALOG_H
#define CIPTVREMOVESERVICEQUERYDIALOG_H

// INCLUDES
#include <e32std.h>
#include <aknmessagequerydialog.h> 

/**
 *  Class for service remoce confirmation query
 *
 *  @lib vcsettingsview.lib
 *  @since S60 3.2
 */

// CLASS DECLARATION
NONSHARABLE_CLASS( CVcxNsSettingsViewRemoveServiceQueryDialog ) : public CAknMessageQueryDialog
    {
    
public:

    /**
     * @param aMessageText Message text inside query.
     */
    static CVcxNsSettingsViewRemoveServiceQueryDialog* NewL( const TDesC& aMessageText );

    virtual ~CVcxNsSettingsViewRemoveServiceQueryDialog();

private: // New functions           

    CVcxNsSettingsViewRemoveServiceQueryDialog( );

    /**
     * @param aMessageText Message text inside query.
     */
    void ConstructL( const TDesC& aMessageText );

    /**
     * From @ CCoeControl.
     * Handles key events.
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode );

    /**
     * From CAknQueryDialog
     * Called to confirm that's OK to exit.
     */
    TBool OkToExitL( TInt aButtonId );

    /**
     * From CAknQueryDialog
     * Called by OfferkeyEventL(), gives a change to dismiss the query even with
     * keys different than Enter of Ok.
     * @param aMessageText Message text inside query.
     */
    TBool NeedToDismissQueryL(const TKeyEvent& aKeyEvent);

    /**
     * From CAknQueryDialog
     * Acts on the menu selection if menu is showing.
     */
    void ProcessCommandL( TInt aCommandId );
     
    };

#endif  // CIPTVREMOVESERVICEQUERYDIALOG_H
