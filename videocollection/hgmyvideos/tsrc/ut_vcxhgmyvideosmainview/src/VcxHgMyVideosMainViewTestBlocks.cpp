/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains testclass implementation.*
*/


// INCLUDES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "VcxHgMyVideosMainViewTest.h"


#include <coemain.h>
#include <aknappui.h>
#include <aknviewappui.h> 
#include <aknview.h>
#include "vcxviewmessageutility.h"
#include <akntitle.h>

#define private public
#include "vcxhgmyvideosmainview.h"
#undef private

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosMainViewTest::Delete()
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "create" , CVcxHgMyVideosMainViewTest::CreateL  ),
        ENTRY( "delete" , CVcxHgMyVideosMainViewTest::DeleteL  ),
        ENTRY( "appui" , CVcxHgMyVideosMainViewTest::AppUi  ),
        ENTRY( "cba" , CVcxHgMyVideosMainViewTest::Cba  ),
        ENTRY( "titlepane" , CVcxHgMyVideosMainViewTest::TitlePaneL  ),
        ENTRY( "playerwrapper" , CVcxHgMyVideosMainViewTest::PlayerWrapperL  ),
        ENTRY( "categorylist" , CVcxHgMyVideosMainViewTest::CategoryListL  ),
        ENTRY( "videolist" , CVcxHgMyVideosMainViewTest::VideoListL  ),
        ENTRY( "activatecategorylist" , CVcxHgMyVideosMainViewTest::ActivateCategoryListL  ),
        ENTRY( "activatevideolist" , CVcxHgMyVideosMainViewTest::ActivateVideoListL  ),
        ENTRY( "handlecommand" , CVcxHgMyVideosMainViewTest::HandleCommandL  ),
        ENTRY( "updatemsktoolbar" , CVcxHgMyVideosMainViewTest::UpdateMSKToolbarL  ),
        ENTRY( "handlekeyevent" , CVcxHgMyVideosMainViewTest::HandleKeyEventL  ),
        ENTRY( "dyninitmsk" , CVcxHgMyVideosMainViewTest::DynInitMskL  ),
        ENTRY( "maketoolbarvisible" , CVcxHgMyVideosMainViewTest::MakeToolbarVisibleL  ),
        ENTRY( "dyninittoolbar" , CVcxHgMyVideosMainViewTest::DynInitToolbarL  ),
        ENTRY( "handlecustomviewmessage" , CVcxHgMyVideosMainViewTest::HandleCustomViewMessageL  ),
        ENTRY( "doactivate" , CVcxHgMyVideosMainViewTest::DoActivateL  ),
        ENTRY( "dodeactivate" , CVcxHgMyVideosMainViewTest::DoDeactivateL  ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::CreateL( CStifItemParser& aItem )
    {
    iView = CVcxHgMyVideosMainView::NewL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::DeleteL( CStifItemParser& aItem )
    {
    delete iView;
    iView = NULL;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::AppUi( CStifItemParser& aItem )
    {
    iView->AppUi();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::Cba( CStifItemParser& aItem )
    {
    iView->Cba();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::TitlePaneL( CStifItemParser& aItem )
    {
    iView->TitlePaneL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::PlayerWrapperL( CStifItemParser& aItem )
    {
    iView->PlayerWrapperL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::CategoryListL( CStifItemParser& aItem )
    {
    iView->CategoryListL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::VideoListL( CStifItemParser& aItem )
    {
    iView->VideoListL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::ActivateCategoryListL( CStifItemParser& aItem )
    {
    iView->ActivateCategoryListL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::ActivateVideoListL( CStifItemParser& aItem )
    {
    TInt param1;
    aItem.GetNextInt(param1);  
    
    iView->ActivateVideoListL( param1  );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::HandleCommandL( CStifItemParser& aItem )
    {
    TInt param1;
    aItem.GetNextInt(param1);  
    
    iView->HandleCommandL( param1 ); 

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::UpdateMSKToolbarL( CStifItemParser& aItem )
    {
    iView->UpdateMSKToolbarL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::HandleKeyEventL( CStifItemParser& aItem )
    {
    TInt param1;
    aItem.GetNextInt(param1);    
    TInt param2;
    aItem.GetNextInt(param2);    
    TInt param3;
    aItem.GetNextInt(param3);
    TInt param4;
    aItem.GetNextInt(param4);
    
    TKeyEvent event;
    
    event.iCode = param1;
    event.iScanCode = param2;
    event.iRepeats = param3;
    
    iView->HandleKeyEventL( event, ( TEventCode )param4 );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::DynInitMskL( CStifItemParser& aItem )
    {
    iView->DynInitMskL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::MakeToolbarVisibleL( CStifItemParser& aItem )
    {
    TInt param;
    aItem.GetNextInt(param);
    
    iView->MakeToolbarVisibleL( param );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::DynInitToolbarL( CStifItemParser& aItem )
    {
    iView->DynInitToolbarL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::HandleCustomViewMessageL( CStifItemParser& aItem )
    {
    TInt param1;
    aItem.GetNextInt(param1);
    
    TPtrC param2;
    aItem.GetNextString(param2);
    
    HBufC8* pbuf = HBufC8::NewLC( param2.Length() );
    pbuf->Des().Copy( param2 );
    
    iView->HandleCustomViewMessageL( TUid::Uid( param1 ), *pbuf );
    
    CleanupStack::PopAndDestroy( pbuf );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::DoActivateL( CStifItemParser& aItem )
    {
    TVwsViewId vid;

    TInt param1;
    aItem.GetNextInt(param1);
    TUid uid = TUid::Uid ( param1 );
    
    TPtrC param2;
    aItem.GetNextString(param2);
    HBufC8* pbuf = HBufC8::NewLC( param2.Length() );
    pbuf->Des().Copy( param2 );

    iView->DoActivateL( vid,
                        uid,
                        *pbuf );
    
    CleanupStack::PopAndDestroy( pbuf );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosMainViewTest::DoDeactivateL( CStifItemParser& aItem )
    {
    iView->DoDeactivate();

    return KErrNone;
    }
