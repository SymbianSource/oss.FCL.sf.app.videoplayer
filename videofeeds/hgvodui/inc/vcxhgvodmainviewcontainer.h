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
* Description:    HG VOD main view container implementation*
*/




#ifndef VCXHGVODMAINVIEWCONTAINER_H
#define VCXHGVODMAINVIEWCONTAINER_H

#include <coecntrl.h>

class CAknViewAppUi;
class CVcxHgVodMainView;
class CVcxNsUiEngine;
class CVcxHgVodUtils;
class CVcxHgVodServiceListImpl;
class CVcxHgVodCategoryListImpl;
class CVcxHgVodContentListImpl;

// CLASS DECLARATION
/**
 *  Main view container
 *  This class provides the adaptation to encapsulate the AlfDisplay
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
class CVcxHgVodMainViewContainer : public CCoeControl
    {

public: // Constructors and destructor


    
    /**
    * C++ default constructor
    */
    CVcxHgVodMainViewContainer( 
        CAknViewAppUi* aAppUi,
        CVcxHgVodMainView& aParent );
                  
    /**
    * Default constructor.
    * 
    * @param aRect Client rectangle
    */
    void ConstructL( const TRect& aRect );

    /**
    * Destructor.
    */
    ~CVcxHgVodMainViewContainer();

    /**
     * Return count of controls.
     *
     * @return count of controls.
     */
    TInt CountComponentControls() const;

    /**
     * Returns control in container.
     * 
     * @param aIndex Index of control.
     * @return Control.
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * Key event handler. 
     *
     * @param aKeyEvent Key event.
     * @param aType     Key event type.
     * @return Response, tells if key was handled or not.
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                 TEventCode aType );
    
    /**
     * From CCoeControl
     * Draws skin background.
     */
    void Draw(const TRect& aRect) const;
    
    /**
     * Returns control's help context.
     * 
     * @param aContext On return help context.
     */
    void GetHelpContext(TCoeHelpContext &aContext) const;
    
private: // data

    /**
     * View Appui.
     * Not own.
     */
    CAknViewAppUi* iAppUi;
    
    /**
     * Parent view
     * 
     */
    CVcxHgVodMainView& iParentView;
    
    /**
     * Pointer to UI Engine
     *  
     */
    CVcxNsUiEngine* iDataProviderIf;

    };

#endif // VCXHGVODMAINVIEWCONTAINER_H
