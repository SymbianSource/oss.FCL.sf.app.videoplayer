/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shows the information label in UI.*
*/



#ifndef C_VCXHGVODINFORMATIONPOPUP_H
#define C_VCXHGVODINFORMATIONPOPUP_H

//  INCLUDES
#include <AknUtils.h> 
#include <AknInfoPopupNoteController.h>
#include "CIptvTimer.h"
#include "MIptvTimerObserver.h"

// FORWARD DECLARATIONS
class CVcxNsUiEngine;
class CAknInfoPopupNoteController;
class CAknView; 
                                            
// CLASS DECLARATION

/**
 * CVcxHgVodInformationPopup class 
 * utilizes CAknInfoPopup class that shows the information
 * label in UI
 */
NONSHARABLE_CLASS ( CVcxHgVodInformationPopup ) : public CBase,
                                  public MIptvTimerObserver, 
                                  public MAknInfoPopupNoteObserver
    {    

public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CVcxHgVodInformationPopup* NewL( CAknView& aParentView, 
                                            CVcxNsUiEngine& aUiEngine );
    
    /**
     * Destructor.
     */
    virtual ~CVcxHgVodInformationPopup();

    /** 
     * Shows given data in popup. 
     * 
     */
    void ShowL();

    /**
     * Hides popup 
     */
    void Hide();
    
    /**
     * Set text to info control
     */
    void SetTextL( const TDesC& aText );

    /**
     * Called when internal timer expires.
     *
     * @param aTimer Timer that expired.
     */    
    void TimerExpired( CIptvTimer* aTimer );
    
    /**
     * Handles the foreground event.
     * @param aForeground Is foreground state gained or not.
     */
    void HandleForegroundEventL( TBool aForeground );

    /**
     * Layout/orientation has been changed.
     */
    void HandleSizeChange();

private:
    
    CVcxHgVodInformationPopup( CAknView& aParentView, 
                               CVcxNsUiEngine& aUiEngine );

    void ConstructL();

    /**
     * Creates internal popup note control.
     *
     * @return ETrue if control was created.
     */    
    TBool CreateControlL();

    /**
     * Deletes internal popup note control.
     */
    void DeleteControl(); 

  
    
//From MAknInfoPopupNoteObserver

     /**
     * From MAknInfoPopupNoteObserver
     * Handles events reported from info popup note.
     * 
     */
     void HandleInfoPopupNoteEvent( CAknInfoPopupNoteController* aController,
                                    TAknInfoPopupNoteEvent aEvent );


private:    // Data
	
    //Reference to parent view class
	CAknView&                           iParentView;
    
    //Reference to application ui class
    CVcxNsUiEngine&                      iUiEngine;
    
    //Pointer to popup note controller class ( from avkon )
    CAknInfoPopupNoteController*        iPopupController;
    
    //Text to be shown in ui
	HBufC*			                    iPopupText;
	
    //
	CIptvTimer*                         iTimer;

	// Top right corner of parent rect.
	TPoint iTopRight;
    };

#endif  // C_VCXHGVODINFORMATIONPOPUP_H   

