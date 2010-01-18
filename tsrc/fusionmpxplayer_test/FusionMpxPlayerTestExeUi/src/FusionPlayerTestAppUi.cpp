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
* Description: 
*
*/


#include <aknutils.h>
#include <S32FILE.H>
#include <f32file.h>
#include <EIKAPP.H>

#include "VCXTestLog.h"
#include "VCXTestCommon.h"
#include "FusionMpxPlayerTestCommon.h"
#include "FusionPlayerTestApp.hrh"
#include "FusionPlayerTestAppUi.h"

// Constants

// ConstructL is called by the application framework
void CFusionPlayerTestAppAppUi::ConstructL()
    {
	BaseConstructL(EAknEnableSkin);

	RProcess process;
	
	iCmdListener = CFusionMpxPlayerTestCmdListener::NewL( process.Id(), this );
	
    User::LeaveIfError( CVCXTestCommon::GetPSProperty( KFusionMpxPlayerTestPScategory, 
            KFusionMpxPlayerTestExeGlobalTesterCount, iPSKeyBase ) );
    iPSKeyBase = iPSKeyBase * 1000 + process.Id();    
    }

CFusionPlayerTestAppAppUi::CFusionPlayerTestAppAppUi()
    {
    }

CFusionPlayerTestAppAppUi::~CFusionPlayerTestAppAppUi()
    {
    VCXLOGLO1(">>>CFusionPlayerTestAppAppUi::~CFusionPlayerTestAppAppUi");
    
    RProcess process;
	
    delete iCmdListener;
	iCmdListener = NULL;
	
    VCXLOGLO2("CFusionPlayerTestAppAppUi:: Sending EVcxFusionMpxPlayerResponseShutdown (%d) ----->", 
            KFusionMpxPlayerTestExePsKeyResponse + iPSKeyBase );
    
    CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyResponse + iPSKeyBase, EVcxFusionMpxPlayerResponseShutdown );
    
    VCXLOGLO1("<<<CFusionPlayerTestAppAppUi::~CFusionPlayerTestAppAppUi");
    }

// handle menu commands
void CFusionPlayerTestAppAppUi::HandleCommandL(TInt aCommand)
    {
    VCXLOGLO1(">>>CFusionPlayerTestAppAppUi::HandleCommandL");
    
    TInt err( KErrNone );
    
    switch( aCommand )
        {
        case EEikCmdExit:
		case EAknSoftkeyExit:	// only softkey handled at this level
            Exit();
            break;
		
		case ESettingCmdPlayVideo:
		    err = iCmdListener->PlayLocalVideo( _L("e:\\testing\\data\\prettyfu.3gp") );
		    if( err != KErrNone )
		        {
		        VCXLOGLO2("CFusionPlayerTestAppAppUi:: PlayLocalVideo returned err: %d", err);
		        }
		    break;
		
		default:        
            break;
        }
        
    VCXLOGLO1("<<<CFusionPlayerTestAppAppUi::HandleCommandL");
    }

void CFusionPlayerTestAppAppUi::ShutdownCommandReceived()
    {
    VCXLOGLO1("CFusionPlayerTestAppAppUi::ShutdownCommandReceived ------> ");
    Exit();
    }

//EOF
