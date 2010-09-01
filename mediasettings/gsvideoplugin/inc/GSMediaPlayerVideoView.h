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


// Version : %version: 3 %



#ifndef GSMEDIAPLAYERSVIDEOVIEW_H
#define GSMEDIAPLAYERSVIDEOVIEW_H

// INCLUDES
#include <aknview.h>
#include <eiklbo.h>
#include <gstabbedview.h>

// FORWARD DECLARATIONS
class CMPSettingsModelForROP;
class CMPSettingsBaseContainer;
class MGSTabbedView;
class CGSTabHelper;

// CLASS DECLARATION

/**
*  CGSMediaPlayerVideoView class.
* 
*  @since 3.1
*/
class CGSMediaPlayerVideoView : public CAknView,
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
        static CGSMediaPlayerVideoView* NewLC(CMPSettingsModelForROP* aModel, CArrayPtrFlat<MGSTabbedView>* aTabViewArray );

        /**
        * Destructor.
        */
        virtual ~CGSMediaPlayerVideoView();

        /**
        * From CEikAppUi
        * Handle skin change event.
        * @since 3.1
        */
        void HandleClientRectChange();
        
        /**
        * Creates new icon for tab. Ownership is transferred to client.
        * @since 3.1
        */       
        CGulIcon* CreateTabIconL();
        
        /**
        * From CCoeControl
        * Handle skin change event.
        */
        void HandleResourceChange( TInt aType );
        
        /**
        * This function is used to set the current item in the listbox.
        * @since 2.0
        * @param aIndex Current item.
        */
        void SetCurrentItem(TInt aIndex);
        
    private: // Functions from base classes
        
        /**
        * From CAknView
        */
        TUid Id() const;

        /**
        * From MEikMenuObserver
        */
        void HandleCommandL(TInt aCommand);

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
        * From MEikMenuObserver Dynamically customize menu items
        * @param aResourceId Menu pane resource ID
        * @param aMenuPane Menu pane pointer
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From CAknView Deactivate this view
        */
        void DoDeactivate();       
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

    private:

        /**
        * C++ default constructor.
        */
        CGSMediaPlayerVideoView(CMPSettingsModelForROP* aModel, CArrayPtrFlat<MGSTabbedView>* aTabViewArray);

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL(CArrayPtrFlat<MGSTabbedView>* aTabViewArray);

        /**
        * Called at the end of DoActivateL.
        * @since 3.1
        */
        void DynInitContainerL();
                
    private: //data

        CMPSettingsModelForROP* iModel;
        CMPSettingsBaseContainer* iContainer;
        CGSTabHelper* iTabHelper;

        TInt iCurrentItem;
        TInt iTopItemIndex;
    };

#endif      // GSMEDIAPLAYERSVIDEOVIEW_H

// End of File
