/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declares view for GSVideoPlugin application.*
*/


// Version : %version: 4 %



#ifndef GSMEDIAPLAYERSTREAMINGVIEW_H
#define GSMEDIAPLAYERSTREAMINGVIEW_H

// INCLUDES
#include <aknview.h>
#include <eiklbo.h>
#include <gstabbedview.h>

// CLASS DECLARATION
class CMPSettingsModelForROP;
class CMPSettingsBaseContainer;
class MGSTabbedView;
class CGSTabHelper;

/**
*  CGSMediaPlayerStreamingView class.
* 
*  @since 3.1
*/
class CGSMediaPlayerStreamingView : public CAknView,
                                    public MGSTabbedView    
{
    public: // Constructors and destructor
    
        enum KGSViewPanicCodes
            {
            EGSViewPanicNullPtr = 1
            };
            
        /**
        * Two-phased constructor.
        */
        static CGSMediaPlayerStreamingView* NewLC(CMPSettingsModelForROP* aModel, CArrayPtrFlat<MGSTabbedView>* aTabViewArray );

        /**
        * Destructor.
        */
        virtual ~CGSMediaPlayerStreamingView();

        /** 
        * Handle screen size change.
        * @since 3.1
        */
        void HandleClientRectChange();
        
        /**
        * Creates new icon for tab. Ownership is transferred to client.
        * @since 3.1
        */         
        CGulIcon* CreateTabIconL();

        /**
        * This function is used to set the current item in the listbox.
        * @since 2.0
        * @param aIndex Current item.
        */
        void SetCurrentItem(TInt aIndex);
        
    private: // Functions from base classes
        /**
        * From CAknView returns UID of view
        * @return TUid uid of the view
        */
        TUid Id() const;

        /**
        * From CAknView Handle commands
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknView Activate this view
        * @param aPrevViewId 
        * @param aCustomMessageId 
        * @param aCustomMessage 
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From CAknView Deactivate this view
        */
        void DoDeactivate();       

        /**
        * From MEikMenuObserver Dynamically customize menu items
        * @param aResourceId Menu pane resource ID
        * @param aMenuPane Menu pane pointer
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
        * Creates new container.
        * @since 3.1
        */
        void NewContainerL();
        
        /**
        * Get CMPSettingsBaseContainer's container.
        * @since 3.1
        */
        CMPSettingsBaseContainer* Container(); 
               
        /**
        * Creates new container.
        * @since 3.1
        */
        void CreateContainerL();       

    protected:

        /**
        * Called at the end of DoActivateL.
        * @since 2.0
        */
        void DynInitContainerL();
        
    private:
        /**
        * C++ default constructor.
        */
        CGSMediaPlayerStreamingView( CMPSettingsModelForROP* aModel, 
		                             CArrayPtrFlat<MGSTabbedView>* aTabViewArray);

        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.1
        */
        void ConstructL( CArrayPtrFlat<MGSTabbedView>* aTabViewArray );
        
    private: // Data
        
        CMPSettingsModelForROP* iModel;
        CMPSettingsBaseContainer* iContainer; // owned
        CGSTabHelper* iTabHelper; // owned

        TInt iCurrentItem;
        TInt iTopItemIndex;
    };

#endif //GSMEDIAPLAYERSTREAMINGVIEW_H

// End of File
