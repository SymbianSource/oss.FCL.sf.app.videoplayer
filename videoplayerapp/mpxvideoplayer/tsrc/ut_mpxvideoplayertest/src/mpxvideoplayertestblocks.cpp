/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  mpxvideoplayerappuiengine test class for STIF testing
*
*/

// Version : %version: ou1cpsw#7 %

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "mpxvideoplayertest.h"
#include "mpxvideoplayer_appuienginetester.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cmpxvideoplayer_test::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void Cmpxvideoplayer_test::Delete()
{
    if ( iAppUiEngineTester )
    {
        delete iAppUiEngineTester;
        iAppUiEngineTester = NULL;
    }
}

// -----------------------------------------------------------------------------
// Cmpxvideoplayer_test::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::RunMethodL( CStifItemParser& aItem )
{
    static TStifFunctionInfo const KFunctions[] =
    {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "CreateEngine", Cmpxvideoplayer_test::CreateEngineL ),
        ENTRY( "CreateGenericParametersTerminate", Cmpxvideoplayer_test::CreateGenericParametersTerminateL ),
        ENTRY( "OpenFile", Cmpxvideoplayer_test::OpenFileL ),
        ENTRY( "OpenMedia", Cmpxvideoplayer_test::OpenMediaL ),
        ENTRY( "SetDefaultAccessPoint", Cmpxvideoplayer_test::SetDefaultAccessPointL ),
        ENTRY( "SetQueryAccessPoint", Cmpxvideoplayer_test::SetQueryAccessPointL ),
        ENTRY( "SetGenericParamAccessPoint", Cmpxvideoplayer_test::SetGenericParamAccessPointL ),
        ENTRY( "OpenFileEmbeddedPdl", Cmpxvideoplayer_test::OpenFileEmbeddedPdlL ),
        ENTRY( "HandleMessageEmbeddedPdl", Cmpxvideoplayer_test::HandleMessageEmbeddedPdlL ),
        ENTRY( "PrepareCloseMpx", Cmpxvideoplayer_test::PrepareCloseMpxL ),
        ENTRY( "StartStandAlone", Cmpxvideoplayer_test::StartStandAloneL ),
        ENTRY( "HandleCollectionMessage", Cmpxvideoplayer_test::HandleCollectionMessageL ),
        ENTRY( "HandleCollectionMedia", Cmpxvideoplayer_test::HandleCollectionMediaL ),
        ENTRY( "HandleSoftkeyBack", Cmpxvideoplayer_test::HandleSoftkeyBackL ),
        ENTRY( "HandleOpenPlaylist", Cmpxvideoplayer_test::HandleOpenPlaylistL ),
        ENTRY( "HandleOpenMedia", Cmpxvideoplayer_test::HandleOpenMediaL ),
        ENTRY( "ProcessActivationMessage", Cmpxvideoplayer_test::ProcessActivationMessageL ),
        ENTRY( "HandleViewActivation", Cmpxvideoplayer_test::HandleViewActivation ),
        ENTRY( "ProcessCommandParameters", Cmpxvideoplayer_test::ProcessCommandParametersL ),
        ENTRY( "HandleMessageActivationMessageL", Cmpxvideoplayer_test::HandleMessageActivationMessageL ),
        ENTRY( "GetViewDepth", Cmpxvideoplayer_test::GetViewDepthL ),
        ENTRY( "ClearPdlInformation", Cmpxvideoplayer_test::ClearPdlInformation ),
    };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::CreateEngineL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::CreateEngineL( CStifItemParser& aItem )
{
    iAppUiEngineTester = CMpxVideoPlayer_AppUiEngineTester::NewL( this );

    return iAppUiEngineTester->CreateEngineL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::CreateGenericParametersL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::CreateGenericParametersTerminateL()
{
    return iAppUiEngineTester->CreateGenericParametersTerminateL();
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::OpenFileL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::OpenFileL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->OpenFileL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::OpenMediaL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::OpenMediaL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->OpenMediaL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::SetDefaultAccessPointL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::SetDefaultAccessPointL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->SetDefaultAccessPointL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::SetQueryAccessPointL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::SetQueryAccessPointL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->SetQueryAccessPointL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::SetGenericParamAccessPointL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::SetGenericParamAccessPointL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->SetGenericParamAccessPointL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::OpenFileEmbeddedPdlL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::OpenFileEmbeddedPdlL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->OpenFileEmbeddedPdlL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::HandleMessageEmbeddedPdlL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::HandleMessageEmbeddedPdlL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->HandleMessageEmbeddedPdlL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::PrepareCloseMpxL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::PrepareCloseMpxL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->PrepareCloseMpxL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::StartStandAloneL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::StartStandAloneL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->StartStandAloneL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::HandleCollectionMessageL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::HandleCollectionMessageL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->HandleCollectionMessageL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::HandleCollectionMediaL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::HandleCollectionMediaL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->HandleCollectionMediaL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::HandleSoftkeyBackL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::HandleSoftkeyBackL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->HandleSoftkeyBackL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::HandleOpenPlaylistL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::HandleOpenPlaylistL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->HandleOpenPlaylistL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::HandleOpenMediaL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::HandleOpenMediaL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->HandleOpenMediaL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::ProcessActivationMessageL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::ProcessActivationMessageL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->ProcessActivationMessageL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::HandleViewActivation
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::HandleViewActivation( CStifItemParser& aItem )
{
    return iAppUiEngineTester->HandleViewActivation( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::ProcessCommandParametersL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::ProcessCommandParametersL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->ProcessCommandParametersL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::HandleMessageActivationMessageL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::HandleMessageActivationMessageL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->HandleMessageActivationMessageL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::GetViewDepthL
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::GetViewDepthL( CStifItemParser& aItem )
{
    return iAppUiEngineTester->GetViewDepthL( aItem );
}

// -----------------------------------------------------------------------------
//   Cmpxvideoplayer_test::ClearPdlInformation
// -----------------------------------------------------------------------------
//
TInt Cmpxvideoplayer_test::ClearPdlInformation()
{
    iAppUiEngineTester->ClearPdlInformation();
    return KErrNone;
}

//  [End of File] - Do not remove
