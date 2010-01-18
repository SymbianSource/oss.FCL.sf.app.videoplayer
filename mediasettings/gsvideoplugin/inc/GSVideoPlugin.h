/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Video Plugin sub-menu in General Settings.*
*/


// Version : %version: 4 %



#ifndef CGSVIDEOPLUGIN_H
#define CGSVIDEOPLUGIN_H

// INCLUDES
#include <gsplugininterface.h>
#include <aknview.h>
#include <ConeResLoader.h>
#include <gsfwviewuids.h>

// Move to CPP file
#include <gsbaseview.h>

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CMPSettingsMainView;
class CAknViewAppUi;
class CMPSettingsBaseContainer;
class CMPSettingsMainContainer;
class CAknView;
class CGulIcon;
class CGSTabHelper;
class CMPSettingsModelForROP;
   
class MGSTabbedView;

// Constants
_LIT( KGSVideoPluginResourceFileName, "z:mediasettings.rsc" );
_LIT( KGSVideoPluginIconFileName, "\\resource\\apps\\mediasettings.mif");
_LIT( KGSDoActivateError, "DoActivateL" );

enum KGSMediaSettingsListViewIndex
    {
    EGSMediaSettingsVideoIndex = 0,
    EGSMediaSettingsStreamingIndex = 1
    };


// CLASS DECLARATION
/**
*  CGSVideoPlugin class 
*/
class CGSVideoPlugin : public CGSBaseView
    {
    public: // Constructor and destructor

        enum KGSViewPanicCodes
            {
            EGSViewPanicNullPtr = 1
            };
            
        /* Symbian OS two-phase Constructor 
        *
        * @return pointer to CGSVideoPlugin object
        */
        static CGSVideoPlugin* NewL( /* TAny* aInitParams */);

        /**
        * Destructor.
        */
        ~CGSVideoPlugin();

    public: // Functions from base classes
        
        /**
        * From CAknView, returns the views id.
        *
        * @return Returns id of the view.
        */
        TUid Id() const;

        /**
        * From MEikCommandObserver, handles the menu based commands.
        *
        * @param aCommand identifies the given command.
        */
        void HandleCommandL( TInt aCommand );
       
        /**
        * Updates specific value to container 
        * @param aListItem item index to update
        * @param aNewValue new value for the setting
        */
        void UpdateListBoxL( const TInt& aListItem, TInt aNewValue );

        /**
        * See base class.
        */
        void ResetSelectedItemIndex();

        /**
        * See base class.
        */
        void SetCurrentItem( TInt aIndex );

                       
    public: // From CGSPluginInterface - See CGSPluginInterface header file.
    
        /**
        * Method for getting caption of this plugin. This should be the
        * localized name of the settings view to be shown in parent view.
        * @param aCaption pointer to Caption variable
        * @since 3.1
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * Creates a new icon of desired type. Override this to provide custom
        * icons. Othervise default icon is used. Ownership of the created icon
        * is transferred to the caller.
        *
        * Icon type UIDs (use these defined constants):
        * KGSIconTypeLbxItem  -   ListBox item icon.
        * KGSIconTypeTab      -   Tab icon.
        *
        * @param aIconType UID Icon type UID of the icon to be created.
        * @return Pointer of the icon. NOTE: Ownership of this icon is
        *         transferred to the caller.
        * @since 3.1
        */
        CGulIcon* CreateIconL( const TUid aIconType );

         /**
        * Method for reading the ID of the plugin provider category. See
        * TGSPluginProviderCategory. PluginProviderCategory can be used for
        * sorting plugins.
        *
        * Default value is EGSPluginProvider3rdParty. Override this function
        * to change the category.
        *
        * @return Plugin provider category ID defined by
        *         TGSPluginProviderCategory
        * @since 3.1
        */        
        TInt PluginProviderCategory() const;

        /**
        * From CEikAppUi
        * Handle skin change event.
        * @since 3.1
        */
        void HandleClientRectChange();      
        
        /**
        * From MGSTabbedView
        * @return Reference to owned tabbed views which should be included in 
        *         the sub-view's tab group.
        * @since 3.1
        */
        CArrayPtrFlat<MGSTabbedView>* TabbedViews(); 

        /**
        * From MEikMenuObserver Dynamically customize menu items
        * @param aResourceId Menu pane resource ID
        * @param aMenuPane Menu pane pointer
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane ); 

        
    public:
    
        /**
        * Get CGSVideoPlugin's container.
        */
        CMPSettingsMainContainer* Container();       

        void CreateContainerL();
        
        /**
        * Search drives to locate Resource and Bitmap files
        * @since 3.2
        */
        void LocateFilePathL( TFileName& aFileName, TBool aBitmapFile = ETrue );
      
     protected: // from CGSBaseView

        void NewContainerL();
        
        void HandleListBoxSelectionL();
     
    protected: // from CAknView
    
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid /* aCustomMessageId */,
                          const TDesC8& /* aCustomMessage */);

        void DoDeactivate();
    
    protected: //new

        /**
        * Symbian OS default constructor. 
        */
        void ConstructL();
        
        /**
        * C++ Constructor 
        */
        CGSVideoPlugin();

    private:

        void CreateLocalViewsL();

               
    private:
    
        // resource loader
        RConeResourceLoader iResources;
             
        CArrayFix<TUid>*             iViewIds;
        CMPSettingsMainView*      iMainView;                   
        CGSTabHelper*                iTabHelper;
        CMPSettingsModelForROP*         iModel;
        CMPSettingsBaseContainer* iContainer;
        RImplInfoPtrArray       iImplInfoArray;
        TBool   iConstructAsGsPlugin;
        CArrayPtrFlat<MGSTabbedView>* iViewArray; 
        TVwsViewId iPrevViewId; // Previous view. 
    
};


#endif //CGSVIDEOPLUGIN_H

// End of File
