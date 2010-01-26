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
* Description:   GSVideoPlugin.cpp*
*/


// Version : %version: 8 %



// INCLUDES
#include "MPSettingsModelForROP.h"
#include "GSVideoPlugin.h"
#include "MPSettingsConstants.h"
#include "MPSettingsMainView.h"
#include "MPSettingsProxyView.h"
#include "MPSettingsNetworkView.h"
#include "MPSettingsPluginView.h"
#include "MPSettingsAdvancedBwView.h"
#include "MPSettingsMainContainer.h" 
#include "mediasettings.hrh"
#include "gstabhelper.h"
#include "GSMediaPlayerVideoView.h"
#include "GSMediaPlayerStreamingView.h"
#include "feedsettingsview.h"
#include "mpxlog.h"

#include <coeaui.h>
#include <hlplch.h>             // For HlpLauncher
#include <aknview.h>
#include <aknViewAppUi.h>
#include <featmgr.h>
#include <bautils.h>
#include <StringLoader.h>     //for StringLoader
#include <gsprivatepluginproviderids.h>
#include <gsmainview.h>
#include <mediasettings.mbg>
#include <gsfwviewuids.h>     //for KGSMainViewUid
#include <MediaSettings.rsg>
#include <eiktxlbm.h>
#include <eikmenup.h> 

// CONSTANTS
const TInt KGSRopSettViewIdArrayGranularity = 3;
const TInt KGSRopSettTopItemIndex = 0;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSVideoPlugin::CGSVideoPlugin()
// Constructor
//
// ---------------------------------------------------------------------------
//
CGSVideoPlugin::CGSVideoPlugin() 
    : iResources( *iCoeEnv ), iConstructAsGsPlugin(ETrue)
    {
    MPX_FUNC("#MS# CGSVideoPlugin::CGSVideoPlugin()");
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::~CGSVideoPlugin()
//  
// Destructor
// ---------------------------------------------------------------------------
//
CGSVideoPlugin::~CGSVideoPlugin()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::~CGSVideoPlugin()");
    FeatureManager::UnInitializeLib();
    iImplInfoArray.ResetAndDestroy();   
    
    if ( iTabHelper ) 
        {
        delete iTabHelper;
        }

    iResources.Close();
    
    if ( iViewIds )
        {
        delete iViewIds;
        }
    
    if ( iModel )
        {
        delete iModel;
        iModel = NULL;
        }

    if ( iViewArray ) 
        {
        iViewArray->Reset();
        delete iViewArray;
        }
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
//
// ---------------------------------------------------------------------------
//
void CGSVideoPlugin::ConstructL()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::ConstructL()");
    FeatureManager::InitializeLibL();
    iViewArray = new (ELeave) CArrayPtrFlat<MGSTabbedView>( 3 ); 

    TFileName fileName;
    LocateFilePathL( fileName, EFalse );    
    OpenLocalizedResourceFileL( fileName, iResources );
        
    BaseConstructL(R_GS_MEDIASETTING_VIEW);

    iTabHelper = CGSTabHelper::NewL();
    }
   
// ---------------------------------------------------------------------------
// CGSVideoPlugin::NewL()
// Static constructor
//  
// ---------------------------------------------------------------------------
//
CGSVideoPlugin* CGSVideoPlugin::NewL( /* TAny* aInitParams */)
    {
    MPX_FUNC("#MS# CGSVideoPlugin::NewL()");
    CGSVideoPlugin* self = new( ELeave ) CGSVideoPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self; 
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::Id
// 
// Returns own view ID.
// ---------------------------------------------------------------------------
//
TUid CGSVideoPlugin::Id() const
    {
    MPX_FUNC("#MS# CGSVideoPlugin::Id()");
    return KGSVideoPluginUid;
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::HandleClientRectChange
// 
// Handle changes to rect().
// ---------------------------------------------------------------------------
//
void CGSVideoPlugin::HandleClientRectChange()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::HandleClientRectChange()");
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::DoActivateL
// 
// First method called by the Avkon framwork to invoke a view.
// ---------------------------------------------------------------------------
//
void CGSVideoPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
                                TUid /* aCustomMessageId */,
                                const TDesC8& /* aCustomMessage */)
    {
    MPX_FUNC("#MS# CGSVideoPlugin::DoActivateL()");
    iPrevViewId = aPrevViewId;
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );

        delete iContainer;
        iContainer = NULL;
        }

    if (!iViewIds)
        {
        CreateLocalViewsL();        
        }
    CreateContainerL();
    AppUi()->AddToViewStackL( *this, iContainer );
    iContainer->ListBox()->SetListBoxObserver( this ) ;
    ResetSelectedItemIndex();
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::DoDeactivate
// 
// Called by the Avkon view framework when closing.
// ---------------------------------------------------------------------------
//
void CGSVideoPlugin::DoDeactivate()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::DoDeactivate()");
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::HandleCommandL
// 
// Handle user menu actions in this view.
// ---------------------------------------------------------------------------
//
void CGSVideoPlugin::HandleCommandL( TInt aCommand )
    {
    MPX_FUNC("#MS# CGSVideoPlugin::HandleCommandL()");
    switch ( aCommand )
        {
        case EMPSettCmdOpen:
            HandleListBoxSelectionL();
            break;          
        case EAknSoftkeyBack:
            SetCurrentItem(EGSMediaSettingsVideoIndex);
            AppUi()->ActivateLocalViewL( KGSAppsPluginUid );
            break;
        case EMPSettCmdHelp:
            {
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
                }
            break;
            }
        default:
            AppUi()->HandleCommandL( aCommand );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::GetCaption
// 
// Return application/view caption.
// ---------------------------------------------------------------------------
//
void CGSVideoPlugin::GetCaptionL( TDes& aCaption ) const
    {
    MPX_FUNC("#MS# CGSVideoPlugin::GetCaptionL()");
    // the resource file is already opened.
    HBufC* result = StringLoader::LoadL( R_GS_MEDIASETTING_VIEW_CAPTION );
    
    aCaption.Copy( *result );
    delete result;
    }
  
// ---------------------------------------------------------------------------
// CGSVideoPlugin::PluginProviderCategory
// 
// A means to identify the location of this plug-in in the framework.
// ---------------------------------------------------------------------------
//
TInt CGSVideoPlugin::PluginProviderCategory() const
    {
    MPX_FUNC("#MS# CGSVideoPlugin::PluginProviderCategory()");
    //To identify internal plug-ins.
    return KGSPluginProviderInternal;
    }
  
// ========================= From CGSBaseView ==================


// ---------------------------------------------------------------------------
// CGSVideoPlugin::NewContainerL
// 
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CGSVideoPlugin::NewContainerL()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::NewContainerL()");
    iContainer = new( ELeave ) CMPSettingsMainContainer;
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::HandleListBoxSelectionL
// 
// Handle any user actions while in the list view.
// ---------------------------------------------------------------------------
void CGSVideoPlugin::HandleListBoxSelectionL() 
    {
    MPX_FUNC("#MS# CGSVideoPlugin::HandleListBoxSelectionL()");
    iCurrentItem = iContainer->ListBox()->CurrentItemIndex();
    iTopItemIndex = iContainer->ListBox()->TopItemIndex();
    
    CGSMediaPlayerVideoView* videoView = NULL;
    CGSMediaPlayerStreamingView* streamingView = NULL;

    TInt count = iViewIds->Count();

    for (TInt i = 0; i < count; i++)
        {
        if (i == 0) //first item of the list is Video view
            {
            videoView = static_cast<CGSMediaPlayerVideoView*>(AppUi()->View(iViewIds->At(i)));
            videoView->SetCurrentItem(KGSRopSettTopItemIndex);        
            }
        else if (i == 1) //second item of the list is Streaming view
            {
            streamingView = static_cast<CGSMediaPlayerStreamingView*>(AppUi()->View(iViewIds->At(i)));
            streamingView->SetCurrentItem(KGSRopSettTopItemIndex);        
            }
        // Third item of the list is "Video service selection"
        }
         
    AppUi()->ActivateLocalViewL(iViewIds->At(iCurrentItem));
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::Container
// 
// Return handle to container class.
// ---------------------------------------------------------------------------
//
CMPSettingsMainContainer* CGSVideoPlugin::Container()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::Container()");
    return static_cast<CMPSettingsMainContainer*>( iContainer );
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::CreateContainerL()
// 
// 
// ---------------------------------------------------------------------------
//
void CGSVideoPlugin::CreateContainerL()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::CreateContainerL()");
    NewContainerL();
    __ASSERT_DEBUG( 
        iContainer, User::Panic( KGSDoActivateError, EGSViewPanicNullPtr ) );
    iContainer->SetMopParent( this );

    TRAPD( error, iContainer->ConstructL( ClientRect() ) );

    if ( error )
        {
        delete iContainer;
        iContainer = NULL;
        User::Leave( error );
        }
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::CreateLocalViews()
// 
// 
// ---------------------------------------------------------------------------
//
void CGSVideoPlugin::CreateLocalViewsL()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::CreateLocalViewsL()");
    // model creation
    iModel = CMPSettingsModelForROP::NewL();

    // Create vector which contains view id's for all setting folders. 
    iViewIds = new(ELeave) CArrayFixFlat<TUid>(KGSRopSettViewIdArrayGranularity);
    iViewIds->AppendL(KMPSettVideoViewId);
    iViewIds->AppendL(KMPSettStreamingViewId);
    iViewIds->AppendL(KMPSettFeedsViewId);
    CAknView* view;  

    view = CMPSettingsMainView::NewLC(iViewIds,iModel,iConstructAsGsPlugin); 
    AppUi()->AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view

    // iMainView is not owned, i.e. it is not deleted in the destructor ->
    // main view cannot be directly creted to iMainView, as it's against the coding
    // conventions to place class' pointers to cleanup stack => static_cast.
    iMainView = static_cast<CMPSettingsMainView*>(view);

    view = CGSMediaPlayerVideoView::NewLC(iModel,iViewArray);
    AppUi()->AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view

    view = CGSMediaPlayerStreamingView::NewLC(iModel,iViewArray);
    AppUi()->AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view 

    view = CVcxNsSettingsView::NewLC( iViewArray );
    AppUi()->AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view
    
    view = CMPSettingsProxyView::NewLC(iModel,iConstructAsGsPlugin);
    AppUi()->AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view 

    view = CMPSettingsNetworkView::NewLC(iModel,iConstructAsGsPlugin);
    AppUi()->AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view 

    view = CMPSettingsAdvancedBwView::NewLC(iModel,iConstructAsGsPlugin);
    AppUi()->AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view 
    
    // Construct plug-in views (if any exists)
    CMPSettingsPluginView::ListImplementationsL(iImplInfoArray);
    TInt count = iImplInfoArray.Count();

    if (count > 0)
        {
        CMPSettingsPluginView* pluginView = NULL;

        for (TInt ii(0); ii < count; ++ii)
            {
            TUid uid = iImplInfoArray[ii]->ImplementationUid();
            pluginView = CMPSettingsPluginView::NewL(uid);
            CleanupStack::PushL(pluginView);
            AppUi()->AddViewL(pluginView);
            CleanupStack::Pop();    // pluginView

            if (pluginView->OpenedFromMainView())
                {
                iViewIds->AppendL(pluginView->Id());
                }
            }
        // Sort UID array to rising order
        TKeyArrayFix sortKey(0, ECmpTUint);
        User::LeaveIfError(iViewIds->Sort(sortKey));
        }
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::CreateIconL
// 
// Return the icon, if has one.
// ---------------------------------------------------------------------------
//
CGulIcon* CGSVideoPlugin::CreateIconL( const TUid aIconType )
    {
    MPX_FUNC("#MS# CGSVideoPlugin::CreateIconL()");
    //EMbm<Mbm_file_name><Bitmap_name>
    CGulIcon* icon;
    
    TFileName fileName;
    LocateFilePathL( fileName );    
    
    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDDefault, 
        fileName,
        EMbmMediasettingsQgn_menu_video,
        EMbmMediasettingsQgn_menu_video_mask );
        }
    else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    return icon;
    }
  
// ---------------------------------------------------------------------------
// CGSVideoPlugin::TabbedViews()
// Returns pointer to tabbed views of application.
// ---------------------------------------------------------------------------
//
CArrayPtrFlat<MGSTabbedView>* CGSVideoPlugin::TabbedViews()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::TabbedViews()");
    return iViewArray;
    }
    
// -----------------------------------------------------------------------------
// CGSVideoPlugin::ResetSelectedItemIndex()
//
//
// -----------------------------------------------------------------------------
//
void CGSVideoPlugin::ResetSelectedItemIndex()
    {
    MPX_FUNC("#MS# CGSVideoPlugin::ResetSelectedItemIndex()");
    if( iContainer )
        {
        iContainer->SetSelectedItem( iCurrentItem );
        }
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::SetCurrentItem
//
//
// ---------------------------------------------------------------------------
void CGSVideoPlugin::SetCurrentItem( TInt aIndex )
    {
    MPX_FUNC("#MS# CGSVideoPlugin::SetCurrentItem()");
    iCurrentItem = aIndex;
    }

// ---------------------------------------------------------------------------
// CGSVideoPlugin::DynInitMenuPaneL
// Before showing a options menu
// ---------------------------------------------------------------------------
void CGSVideoPlugin::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    MPX_FUNC("#MS# CGSVideoPlugin::DynInitMenuPaneL()");
    if ( aResourceId == R_MPSETT_APP_MENU )
        {
        // Help should be displayed only if the feature is supported according
        // to Feature Manager
        if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
            {
            aMenuPane->SetItemDimmed(EMPSettCmdHelp, ETrue);
            }
        }
    }

// -----------------------------------------------------------------------------
// CGSVideoPlugin::LocateFilePathL
// -----------------------------------------------------------------------------
//
void CGSVideoPlugin::LocateFilePathL( TFileName& aFileName, TBool aBitmapFile )
    {
    _LIT( KGSResourceFileName, "mediasettings.rsc" );
    //_LIT( KGSIconFileName, "mediasettings.mbm");
    _LIT( KGSIconFileName, "mediasettings.mif");
    
    TParse parse;
    TFileName temp;
                
    if ( aBitmapFile )  // Mbm file
        {
        temp.Append(KGSIconFileName);
        }
    else                // Rsc file
        {
        temp.Append(KGSResourceFileName);
        }
        
    parse.Set( temp, &KDC_APP_RESOURCE_DIR, NULL );
    TPtrC iconFile = parse.FullName();
        
    // This is done to ensure upgraded file is used first. 
    // If no upgraded file is found, default file in Z: drive will be used.
    TFindFile find( CCoeEnv::Static()->FsSession() );
    TInt err = find.FindByDir( iconFile, KNullDesC );
        
    if ( err )
        {
        if ( aBitmapFile )
            {
            aFileName.Append( KGSVideoPluginIconFileName );    
            }
        else
            {
            aFileName.Append( KGSVideoPluginResourceFileName );     
            }
        }
    else
        {
        if ( aBitmapFile ) 
            {
            // this Mbm file has the whole path including the drive
            aFileName.Append( find.File() );    
            }
        else               
            {
            // General Settings base class would only take in this format <drive>:<rsc_file_name>
            // So, the file name is returned in that particular format
            TFileName tempFile;
            tempFile.Append( find.File() );    // this rsc file has the whole path including the drive
            aFileName.Append( tempFile[0] );   // append drive letter
            aFileName.Append( tempFile[1] );   // append delimiter
            aFileName.Append( KGSResourceFileName ); // append rsc_file_name
            }
        }
        MPX_DEBUG3(_L("#MS# CGSVideoPlugin::LocateFilePathL(%d,%S)"),aBitmapFile,&aFileName);
    }   

//End of File
