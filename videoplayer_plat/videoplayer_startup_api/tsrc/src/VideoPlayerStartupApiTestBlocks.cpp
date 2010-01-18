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


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <apgcli.h>    // RApaLsSession
#include <bautils.h>
#include <apacmdln.h>  // CApaCommandLine
#include <w32std.h>    // RWsSession
#include <apgtask.h>   // TApaTask*

#include <mpxcollectionutility.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>

#include "VideoPlayerStartupApiTest.h"
#include "VCXTestLog.h"
#include "VCXTestSmClient.h"
#include "VCXTestMyVideosCollectionClient.h"

// CONSTANTS
const TUid KFusionUid = { 0x200159B2 };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CVideoPlayerStartupApiTest::Delete() 
    {
    iProcessHandles.ResetAndDestroy(); 
    
    delete iMyVideosCollectionClient;
    iMyVideosCollectionClient = NULL;
    
    delete iMessageWait;
    iMessageWait = NULL;    
    }

// -----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVideoPlayerStartupApiTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "ActivationMessage", CVideoPlayerStartupApiTest::ActivationMessageL ),
        ENTRY( "KillFusion", CVideoPlayerStartupApiTest::KillFusionL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::ActivationMessageL
// -----------------------------------------------------------------------------
//
TInt CVideoPlayerStartupApiTest::ActivationMessageL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVideoPlayerStartupApiTest::ActivationMessageL");
    // Print to UI
    _LIT( KVideoPlayerStartupApiTest, "VideoPlayerStartupApiTest" );
    _LIT( KWhere, "In ActivationMessageL" );
    TestModuleIf().Printf( 0, KVideoPlayerStartupApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    
    if( iMessageWait == NULL )
        {
        iMessageWait = CVCXTestMessageWait::NewL( this );
        }
    
    if( iMyVideosCollectionClient == NULL )
        {
        iMyVideosCollectionClient = CVCXTestMyVideosCollectionClient::NewL( this );

        // Wait for video list to open.
        iMessageWait->WaitForMessageL( EVCXTestVideoListRefreshed, 30, ETrue );
        }
    
    if( iCollectionError == KErrNone )
        {
        // Refresh collection.
        iMyVideosCollectionClient->RefreshCollectionL();
        iMessageWait->AddMessage( EVCXTestVideoListRefreshed );
        iMessageWait->WaitForAllL( 10, ETrue );    
        }
    
    if( iCollectionError != KErrNone )
        {
        VCXLOGLO2("CVideoPlayerStartupApiTest::ActivationMessageL: Something failed with my videos collection plugin, err: %d", iCollectionError);
        VCXLOGLO1("<<<CVideoPlayerStartupApiTest::ActivationMessageL");
        return iCollectionError;
        }
    
    TVideoPlayerActivationMessage message;
    
    TInt msgTypeInt;
    User::LeaveIfError( aItem.GetNextInt( msgTypeInt ) );

    TInt msgSenderInt;
    User::LeaveIfError( aItem.GetNextInt( msgSenderInt ) );

    TVideoPlayerActivationMessage::TMessageType msgType = 
        static_cast<TVideoPlayerActivationMessage::TMessageType>( msgTypeInt );
    
    TVideoPlayerActivationMessage::TMessageSender msgSender = 
        static_cast<TVideoPlayerActivationMessage::TMessageSender>( msgSenderInt );
    
    message.iMsgType   = msgType;
    message.iMsgSender = msgSender;
    
    switch( msgType )
        {
        case TVideoPlayerActivationMessage::ENoType:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: ENoType");
            }
            break;
            
        case TVideoPlayerActivationMessage::ELaunchVideoToPlayer:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: ELaunchVideoToPlayer");
            
            TInt videoNum;
            if( aItem.GetNextInt( videoNum ) == KErrNone )
                {
                CMPXMedia* media;
                
                TRAPD( err, media = iMyVideosCollectionClient->GetVideoL( videoNum ) );
                
                if( err == KErrNone )
                    {
                    TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
                    
                    if( media->IsSupported( KMPXMediaGeneralUri ) )
                        {
                        const TDesC& path = media->ValueText( KMPXMediaGeneralUri );
                        message.iFullPath = path;
                        VCXLOGLO2("CVideoPlayerStartupApiTest:: video file: %S", &path);
                        }
                    
                    message.iServiceId = itemId.iId1;
                    }
                else
                    {
                    message.iFullPath = _L("c:\\");
                    message.iServiceId = 60060;
                    }
                }
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenVideoStorage:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenVideoStorage");
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenInternetVideos:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenInternetVideos");
            message.iServiceId = 0;
            message.iActive = 1;
            message.iSingle = ETrue;
            message.iFileId = 0;
            message.iDrive = 0;
            message.iFullPath = KNullDesC;
            }
            break;
            
        case TVideoPlayerActivationMessage::ELaunchServiceById:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: ELaunchServiceById");
            iSmClient->GetAnyServiceIdL( message.iServiceId );
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenServiceCatalogue:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenServiceCatalogue");
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenVideoShop:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenVideoShop");
            }
            break;
            
        case TVideoPlayerActivationMessage::ELiveTVMessage:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: ELiveTVMessage");
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenRecordingsCategory:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenRecordingsCategory");
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenVCSettings:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenVCSettings");
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenVCSettingsFeedSubscriptions:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenVCSettingsFeedSubscriptions");
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenVCSettingsAddFeed:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenVCSettingsAddFeed");
            }
            break;
            
        case TVideoPlayerActivationMessage::ELiveTVMessageLastWatched:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: ELiveTVMessageLastWatched");
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenVCSettingsLiveTVSubscriptions:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenVCSettingsLiveTVSubscriptions");
            }
            break;
            
        case TVideoPlayerActivationMessage::EOpenVCSettingsEditFeed:
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: msgType: EOpenVCSettingsEditFeed");
            }
            break;
        default:
            break;
        }
    
    TRAPD( err, SendActivationMessageL( message ) );
    
    VCXLOGLO1("<<<CVideoPlayerStartupApiTest::ActivationMessageL");
    return err;
    }

// -----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::KillFusionL
// -----------------------------------------------------------------------------
//
TInt CVideoPlayerStartupApiTest::KillFusionL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVideoPlayerStartupApiTest::KillFusionL");
    // Print to UI
    _LIT( KVideoPlayerStartupApiTest, "VideoPlayerStartupApiTest" );
    _LIT( KWhere, "In KillFusionL" );
    TestModuleIf().Printf( 0, KVideoPlayerStartupApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    RWsSession session;
    User::LeaveIfError( session.Connect() );
    TApaTaskList taskList( session );    
    TApaTask task = taskList.FindApp( KFusionUid );

    if( task.Exists() )
        {
        VCXLOGLO1("CVideoPlayerStartupApiTest:: Ending task.");  
        task.EndTask();
        User::After( 3000000 );
        if( task.Exists() )
            {
            VCXLOGLO1("CVideoPlayerStartupApiTest:: Killing task.");
            task.KillTask();
            }
        }

    session.Close();

    // Check the exit reasons for processes.
    TInt err = CheckProcessExits( iProcessHandles );
    
    iProcessHandles.ResetAndDestroy();
    
    VCXLOGLO1("<<<CVideoPlayerStartupApiTest::KillFusionL");
    return err;
    }

// ----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::SendActivationMessageL
// ----------------------------------------------------------------------------
//
void CVideoPlayerStartupApiTest::SendActivationMessageL( TVideoPlayerActivationMessage& aMessage )
    {
    VCXLOGLO1(">>>CVideoPlayerStartupApiTest::SendActivationMessageL");
    TPckg<TVideoPlayerActivationMessage> message( aMessage );
    
    RWsSession session;
    User::LeaveIfError( session.Connect() );
    TApaTaskList taskList( session );
    TApaTask task = taskList.FindApp( KFusionUid );

    RPointerArray<HBufC> processesBeforeMessage;

    if( processesBeforeMessage.Count() == 0 )
        {
        // Get list of processes before launch
        GetAllProcessNames( processesBeforeMessage );
        }
    
    TInt err( KErrNone );
    
    if( task.Exists() )
        {
        VCXLOGLO1( "CVideoPlayerStartupApiTest:: Task exists, bringing to fore." );
        iLog->Log( _L("CVideoPlayerStartupApiTest:: Task exists, bringing to fore.") );
        
        task.BringToForeground();

        TUid play = TUid::Uid( KVideoPlayerVodStartPlayer );
        TInt error = task.SendMessage( play, message );
        }
    else
        {                                
        VCXLOGLO1( "CVideoPlayerStartupApiTest:: Starting app." );
        iLog->Log( _L("CVideoPlayerStartupApiTest:: Starting app.") );
        
        RApaLsSession lsSession;
        User::LeaveIfError( lsSession.Connect() );

        VCXLOGLO1( "CVideoPlayerStartupApiTest:: Session ok." );
        iLog->Log( _L("CVideoPlayerStartupApiTest:: Session ok.") );

        TApaAppInfo appInfo;
        
        err = lsSession.GetAppInfo( appInfo, KFusionUid );

        if ( err == KErrNone )
            {
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo.iFullName );                            
            cmdLine->SetTailEndL( message );

            VCXLOGLO1( "CVideoPlayerStartupApiTest:: Start it." );
            iLog->Log( _L("CVideoPlayerStartupApiTest:: Start it.") );

            User::LeaveIfError( lsSession.StartApp( *cmdLine ) );
            CleanupStack::PopAndDestroy( cmdLine );
            }
        else
            {
            VCXLOGLO2( "CVideoPlayerStartupApiTest:: GetAppInfo ret: %d.", err );
            iLog->Log( _L("CVideoPlayerStartupApiTest:: GetAppInfo ret: %d."), err );
            }
        
        lsSession.Close();
        }
    
    session.Close();
    
    // Get process handles for the new processes
    GetHandlesForNewProcessesL( processesBeforeMessage, iProcessHandles );
    
    processesBeforeMessage.ResetAndDestroy();
    
    if( iProcessHandles.Count() < 1 )
        {
        VCXLOGLO1("CVideoPlayerStartupApiTest:: ERROR, No new processes detected.");
        iLog->Log( _L("CVideoPlayerStartupApiTest:: ERROR, No new processes detected.") );
        User::Leave( KErrNotFound );
        }
    
    User::LeaveIfError( err );
    VCXLOGLO1("<<<CVideoPlayerStartupApiTest::SendActivationMessageL");
    }

// ----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::GetAllProcessNames
// ----------------------------------------------------------------------------
//
void CVideoPlayerStartupApiTest::GetAllProcessNames( RPointerArray<HBufC>& aArray )
    {
    VCXLOGLO1(">>>CVideoPlayerStartupApiTest::GetAllProcessNames");
    
    TFindProcess findProcess;
    TFullName result;
    
    // Process count zero, populate array
    if( aArray.Count() <= 0 )
        {
        while( findProcess.Next(result) == KErrNone )
            {
            TRAP_IGNORE( 
                aArray.Append( result.AllocL() );
                );
            VCXLOGLO2("CVideoPlayerStartupApiTest: process: %S", &result);
            iLog->Log( _L("CVideoPlayerStartupApiTest: process: %S"), &result );
            }    
        }
    
    VCXLOGLO1("<<<CVideoPlayerStartupApiTest::GetAllProcessNames");
    }

// ----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::GetHandlesForNewProcessesL
// ----------------------------------------------------------------------------
//
void CVideoPlayerStartupApiTest::GetHandlesForNewProcessesL( RPointerArray<HBufC>& aExistingProcesses,
    RPointerArray<CTestProcess>& aHandles )
    {
    VCXLOGLO1(">>>CVideoPlayerStartupApiTest::GetHandlesForNewProcessesL");
    
    TFindProcess findProcess;
    TFullName processName;
       
    while( findProcess.Next( processName ) == KErrNone )
        {
        TInt found = EFalse;
        for( TInt i = 0; i < aExistingProcesses.Count(); i++ )
            {
            if( aExistingProcesses[i]->Des().CompareF( processName ) == KErrNone )
                {
                found = ETrue;
                }
            }
            
        if( !found )
            {
            CTestProcess* p = CTestProcess::NewL();
            TInt err = p->iProcess.Open( processName );
            if( err == KErrNone )
                {
                CleanupStack::PushL( p );
                p->iName = processName.AllocL();
                CleanupStack::Pop( p );
                aHandles.Append( p );
                }
            VCXLOGLO3("CVideoPlayerStartupApiTest:: new process: '%S, open result: %d", &processName, err );
            }
        }

    VCXLOGLO1("<<<CVideoPlayerStartupApiTest::GetHandlesForNewProcessesL");
    }

// ----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::CheckProcessExits
// ----------------------------------------------------------------------------
//
TInt CVideoPlayerStartupApiTest::CheckProcessExits( RPointerArray<CTestProcess>& aHandles )
    {
    VCXLOGLO1(">>>CVideoPlayerStartupApiTest::CheckProcessExits");
    
    TInt err( KErrNone );
   
    for( TInt i=0; i<aHandles.Count(); i++ )
        {
        CTestProcess* p = aHandles[i];
        if( p )
            {
            VCXLOGLO2("CVideoPlayerStartupApiTest:: Process: %S", p->iName );
            
            TInt exitType =  p->iProcess.ExitType();
            TInt exitReason = p->iProcess.ExitReason();
            TExitCategoryName exitCatName = p->iProcess.ExitCategory();
            
            VCXLOGLO2("CVideoPlayerStartupApiTest:: Exit type: %d", exitType);
            VCXLOGLO2("CVideoPlayerStartupApiTest:: Exit reason: %d", exitReason );
            VCXLOGLO2("CVideoPlayerStartupApiTest:: Exit category: %S", &exitCatName);
            
            iLog->Log(_L("CVideoPlayerStartupApiTest:: Exit type: %d"), exitType);
            iLog->Log(_L("CVideoPlayerStartupApiTest:: Exit reason: %d"), exitReason );
            iLog->Log(_L("CVideoPlayerStartupApiTest:: Exit category: %S"), &exitCatName);
            
            if( exitReason != KErrNone || (exitType != EExitPending && exitType != EExitKill) )
                {
                err = ( exitReason != KErrNone ) ? exitReason : exitType;   
                VCXLOGLO2("CVideoPlayerStartupApiTest:: Error with the process: %d.", err);
                iLog->Log( _L("CVideoPlayerStartupApiTest:: Error with the process: %d."), err );
                }
            }
        }
    
    VCXLOGLO1("<<<CVideoPlayerStartupApiTest::CheckProcessExits");
    return err;
    }

// -----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::VideoEventL
// -----------------------------------------------------------------------------
//
void CVideoPlayerStartupApiTest::VideoEventL( TVCXTestVideoEvent aEvent, 
        TUint32 aParam1, TUint32 aParam2, TInt aError )
    {
    VCXLOGLO1(">>>CVideoPlayerStartupApiTest::VideoEventL");
    
    VCXLOGLO5("CVideoPlayerStartupApiTest::VideoEventL: aEvent: %d, aParam1: %d, aParam2: %d, aError: %d", aEvent, aParam1, aParam2, aError);
    iMessageWait->ReceiveMessage( aEvent, aError );
    
    VCXLOGLO1("<<<CVideoPlayerStartupApiTest::VideoEventL");
    }


// -----------------------------------------------------------------------------
// CVideoPlayerStartupApiTest::MessageWaitComplete
// -----------------------------------------------------------------------------
//
void CVideoPlayerStartupApiTest::MessageWaitComplete( TInt aError )
    {
    if( aError != KErrNone )
        {
        VCXLOGLO1("CVideoPlayerStartupApiTest:: Message wait TIMEOUT!");
        iCollectionError = aError;
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
