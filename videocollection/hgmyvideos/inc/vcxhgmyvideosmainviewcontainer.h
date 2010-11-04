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
* Description:      My Videos UI main view container.*
*/




#ifndef VCXHGMYVIDEOSMAINVIEWCONTAINER_H
#define VCXHGMYVIDEOSMAINVIEWCONTAINER_H

//INCLUDES

#include <coecntrl.h>
#include <aknview.h>
 

// FORWARD DECLARATION

class CVcxHgMyVideosMainView;

// CLASS DECLARATION

/**
 * My Videos UI main view container.
 *
 * @lib vcxhgmyvideos.lib
 */
class CVcxHgMyVideosMainViewContainer : public CCoeControl
    {
    public: // Constructors and destructor

        /**
         * Creates main view container.
         *
         * @param aParentView Parent view of container.    
         * @return New container object.
         */
        static CVcxHgMyVideosMainViewContainer* NewL( 
                CVcxHgMyVideosMainView& aParentView );

        /**
         * Creates main view container.
         *
         * @param aParentView Parent view of container.
         * @return New container object.        
         */
        static CVcxHgMyVideosMainViewContainer* NewLC( 
                CVcxHgMyVideosMainView& aParentView );

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosMainViewContainer();

    public: // From CCoeControl

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
         * Returns control's help context.
         * 
         * @param aContext On return help context.
         */
        void GetHelpContext( TCoeHelpContext &aContext ) const;
        
    private: // Constructors

        /**
         * C++ default constructor.
         *
         * @param aParentView Parent view for this container.
         */
        CVcxHgMyVideosMainViewContainer( CVcxHgMyVideosMainView& aParentView );
   
        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();

    private: // Data

        /**
	     * This container's parent view
	     * Not own.
	     */
        CVcxHgMyVideosMainView& iParentView;
    };

#endif // VCXHGMYVIDEOSMAINVIEWCONTAINER_H   
