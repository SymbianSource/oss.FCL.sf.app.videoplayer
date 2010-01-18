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
* Description:  interface class definition for action handler plugin*
*/


// Version : %version: 19 %

#include <e32def.h>
#include <e32uid.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <apgtask.h>
#include <eikenv.h>
#include <vwsdef.h>
#include <coeaui.h>
#include <avkon.rsg>
#include <AknGlobalNote.h>
#include <MediatorEventProvider.h>
#include <centralrepository.h>
#include <LiwVariant.h>
#include <videoplayeractivationmessage.h>
#include <vcxmyvideosdefs.h>
#include <ErrorUI.h>

#include "videoahpluginconstants.h"
#include "videoactionhandler.h"
#include "videoplayerpskeys.h"
#include "iptvlastwatcheddata.h"
#include "iptvlastwatchedapi.h"
#include "vcxhgvodtelephonyclient.h"

// CONSTANTS
const TUid KUidMPXVideoPlayerApp = { 0x200159B2 };
const TUid KUidMediaSettingsApp  = { 0x10005A3F };

_LIT8( KKeyMessage,     "message" );
_LIT8( KKeyMessageData, "message_data" );

//  Constants for wg name. Used to identify if wg is actually ours.
_LIT( KVideoPlayerWgName, "*200159b2*");
const TUint KVideoPlayerWgNameSize = 50;

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CVideoActionHandler::ConstructL
// ----------------------------------------------------------------------------
//
void CVideoActionHandler::ConstructL()
    {   
    iMediatorEventProvider = CMediatorEventProvider::NewL();
    }

// ----------------------------------------------------------------------------
// CVideoActionHandler::NewL
// ----------------------------------------------------------------------------
//
CVideoActionHandler* CVideoActionHandler::NewL()
    {
    CVideoActionHandler* self = CVideoActionHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CVideoActionHandler::NewLC
// ----------------------------------------------------------------------------
//
CVideoActionHandler* CVideoActionHandler::NewLC()
    {
    CVideoActionHandler* self = new( ELeave ) CVideoActionHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CVideoActionHandler::~CVideoActionHandler
// ---------------------------------------------------------------------------
CVideoActionHandler::~CVideoActionHandler()
	{  
	delete iMediatorEventProvider;
	}

// ---------------------------------------------------------------------------
// CVideoActionHandler::ExecuteActionL
// Executes provided action
// ---------------------------------------------------------------------------
//
TInt CVideoActionHandler::ExecuteActionL( const CLiwMap* aMap )
    {    
    TInt errCode( KErrNotFound );   
    
    TVideoPlayerActivationMessage*  activationMsg = new( ELeave ) TVideoPlayerActivationMessage;
    CleanupStack::PushL( activationMsg );
    
    errCode = PrepareMessageL( aMap, *activationMsg );
    if( KErrNone == errCode )
        {
        errCode = StartAppL( *activationMsg );
        }
    CleanupStack::PopAndDestroy( activationMsg );
    
    return errCode;
    }

// ---------------------------------------------------------------------------
// CVideoActionHandler::PrepareMessageL
// ---------------------------------------------------------------------------
//
TInt CVideoActionHandler::PrepareMessageL( const CLiwMap* aMap, 
                                           TVideoPlayerActivationMessage& aActivationMsg )
    {
    TInt err( KErrNone );

    TLiwVariant variant; 
    variant.PushL();
    err = ExtractVariant( aMap, variant, KKeyMessage );
    
    if( err != KErrNone )    
	    {
	    CleanupStack::PopAndDestroy( &variant );     
	    return err;
	    }
	    
    TPtrC message = variant.AsDes();
       

    aActivationMsg.iServiceId = 0;
    aActivationMsg.iFileId    = 0;
    aActivationMsg.iDrive     = 0;            	
    aActivationMsg.iMsgSender = TVideoPlayerActivationMessage::EMatrixMenu;         
       
    if ( !message.CompareF( KVcxNsExtLaunchVideoCmd ) )
        {
        aActivationMsg.iMsgType = TVideoPlayerActivationMessage::ELaunchVideoToPlayer;
        TLiwVariant variantData;
        variantData.PushL();
        err = ExtractVariant( aMap, variantData, KKeyMessageData );

        if( err == KErrNone )
            {
            TPtrC data = variantData.AsDes();
            aActivationMsg.iFullPath = data;
            }
        else if( err == KErrNotFound )
            {
            err = KErrNone;
            }
        CleanupStack::PopAndDestroy( &variantData );
        }
    else if ( !message.CompareF( KVcxNsExtLaunchLastWatchedCmd ) )
        {
        aActivationMsg.iMsgType = TVideoPlayerActivationMessage::ELaunchVideoToPlayer;
        LoadLwDataL( aActivationMsg );
        }
    else if ( !message.CompareF( KVcxNsExtOpenStorageCmd ) )
        {
        aActivationMsg.iMsgType = TVideoPlayerActivationMessage::EOpenVideoStorage;    
        }
    else if ( !message.CompareF( KVcxNsExtInternetVideoCmd ) )
        {
        aActivationMsg.iMsgType = TVideoPlayerActivationMessage::EOpenInternetVideos;
        }
    else if ( !message.CompareF( KVcxNsExtServiceCatalCmd ) )
        {
        aActivationMsg.iMsgType = TVideoPlayerActivationMessage::EOpenServiceCatalogue;
        }
    else if ( !message.CompareF( KVcxNsExtVideoShopCmd ) )
        {
        aActivationMsg.iMsgType = TVideoPlayerActivationMessage::EOpenVideoShop;
        }
    else if ( !message.CompareF( KVcxNsExtLaunchVCSettingsCmd ) )
        {
        aActivationMsg.iMsgType = TVideoPlayerActivationMessage::EOpenVCSettings;
        }
    else if ( !message.CompareF( KVcxNsExtOpenLiveTVCmd ) )
		{
		aActivationMsg.iMsgType = TVideoPlayerActivationMessage::ELiveTVMessage;
		}
    else if( !message.CompareF( KVcxNsExtLaunchLiveTVLastWatch ) )
        {
        aActivationMsg.iMsgType = TVideoPlayerActivationMessage::ELiveTVMessageLastWatched;
        }
    else if( !message.CompareF( KVcxNsExtLoadTvVideoSuite ) )
        {
        // If the player app is running 
        // in background, switch to that
        TryBringVideoPlayerToForegroundL();

        SendMediatorEvent( KVcxNsAhEventSuiteLoaded ); 
        // This needs cleaner way to not to execute the cmd.
        err = KErrNotSupported;
        }
    else if ( !message.CompareF( KVcxNsExtUnloadTvVideoSuite ) )
        {
        SendMediatorEvent( KVcxNsAhEventSuiteUnloaded ); 
        // This needs cleaner way to not to execute the cmd.
        err = KErrNotSupported;
        }
    else
        {
        err = KErrNotSupported;
        }
    CleanupStack::PopAndDestroy( &variant );
    
    return err;
	}

// ---------------------------------------------------------------------------
// CVideoActionHandler::StartAppL
// ---------------------------------------------------------------------------
//
TInt CVideoActionHandler::StartAppL( TVideoPlayerActivationMessage& aMessage )
    {
    TInt err(KErrNone);    
    
    // If settings are requested launch them, otherwise launch my videos
    if (aMessage.iMsgType == TVideoPlayerActivationMessage::EOpenVCSettings)
        {
        err = LaunchMediaSettingsL( aMessage );
        }
    else
        {
        err = LaunchMyVideosL( aMessage );
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// CVideoActionHandler::ExtractVariant
// ---------------------------------------------------------------------------
//
TInt CVideoActionHandler::ExtractVariant( const CLiwMap* aMap,
								          TLiwVariant& aVariant,
								          const TDesC8& aKeyName )
   
    {
    TInt errCode( KErrNone );
	TBool found( EFalse );
    TRAP( errCode, found = aMap->FindL( aKeyName, aVariant ) );
    
    if ( errCode == KErrNone && !found )
       	{
       	errCode = KErrNotFound;
    	}
	
	return errCode;
    }

// ---------------------------------------------------------------------------
// CVideoActionHandler::SendMediatorEvent
// ---------------------------------------------------------------------------
//
void CVideoActionHandler::SendMediatorEvent( TInt aEvent )
    {
    iMediatorEventProvider->RaiseEvent( 
                                    TUid::Uid( KVcxNsAhMediatorDomain ),
                                    TUid::Uid( KVcxNsAhMediatorCategory ), 
                                    aEvent,
                                    TVersion( KVcxNsAhEventVersion, 0, 0 ),
                                    KNullDesC8() );
    }

// ---------------------------------------------------------------------------
// CVideoActionHandler::LaunchMediaSettingsL
// ---------------------------------------------------------------------------
//
TInt CVideoActionHandler::LaunchMediaSettingsL( TVideoPlayerActivationMessage& aMessage )
    {
    // KIptvVodStartPlayer is not currently used. Any case we need it?    
    return LaunchMatrixTaskL( aMessage, KUidMediaSettingsApp, KVideoPlayerVodStartPlayer );
    }


// ---------------------------------------------------------------------------
// CVideoActionHandler::LaunchMyVideosL
// ---------------------------------------------------------------------------
//
TInt CVideoActionHandler::LaunchMyVideosL( TVideoPlayerActivationMessage& aMessage )
    {
    // KIptvVodStartPlayer is not currently used. Any case we need it?    
    return LaunchMatrixTaskL( aMessage, KUidMPXVideoPlayerApp, KVideoPlayerVodStartPlayer );    
    }


// ---------------------------------------------------------------------------
// CVideoActionHandler::LaunchMatrixTaskL
// ---------------------------------------------------------------------------
//
TInt CVideoActionHandler::LaunchMatrixTaskL( TVideoPlayerActivationMessage& aMessage, TUid appUID, TInt aActivationMessage )
    {

    TInt err(KErrNone);

    TPckg<TVideoPlayerActivationMessage> message(aMessage);

    RWsSession session;
    User::LeaveIfError(session.Connect());

    // check if there is existing stand alone running
    TInt ourWgId( 0 );
    TInt ret( 0 );

    // No need to check wgId if we are lauching Media Settings
    if ( appUID != KUidMediaSettingsApp )
        {
        ret = RProperty::Get( KVideoPlayerRPropertyCategory, KVideoPlayerRPropertyWGIdKey, ourWgId );
        }
        
    if( !ret )
        {
        // check if the wgId is really one of our own
        TBuf<KVideoPlayerWgNameSize> wgName;
        session.GetWindowGroupNameFromIdentifier(ourWgId, wgName );        
        if( wgName.MatchF( KVideoPlayerWgName ) == KErrNotFound )
            {
            // Clear wg id, it is not ours anymore
            ourWgId = 0;
            }
        }
        
    if( ourWgId != 0 )
        {
        // Send message to our wg id            
        TUid play = TUid::Uid(aActivationMessage);        
        session.SetWindowGroupOrdinalPosition( ourWgId, 0 );
        err = session.SendMessageToWindowGroup( ourWgId, play, message );             
        }        
    else
        {
        // No stand alone available, create new one
        RApaLsSession lsSession;
        err = lsSession.Connect();
        TApaAppInfo appInfo;

        if (lsSession.GetAppInfo(appInfo, appUID) == KErrNone)
            {
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL(appInfo.iFullName);
            cmdLine->SetTailEndL(message);
            User::LeaveIfError(lsSession.StartApp(*cmdLine));
            CleanupStack::PopAndDestroy(cmdLine);
            }
        else
            {
            err = KErrNotFound;
            }

        lsSession.Close();   
        }    
    
    session.Close();
    return err;
    }

// ---------------------------------------------------------------------------
// CVideoActionHandler::LoadLwDataL
// ---------------------------------------------------------------------------
//
void CVideoActionHandler::LoadLwDataL( TVideoPlayerActivationMessage& aMessage )
    {
    CIptvLastWatchedData* data = CIptvLastWatchedData::NewL();
    CleanupStack::PushL( data );
    
    CIptvLastWatchedApi* lwApi = CIptvLastWatchedApi::NewL();
    CleanupStack::PushL( lwApi );
    
    lwApi->GetLastWatchedDataL( *data );
    
    if ( data->ParentalControl() != KVcxMyvideosCenRepParentControlKeyOff )
        {
        TInt parentControlSetting( KVcxMyvideosCenRepParentControlKeyDefault );
        CRepository* rep = NULL;
        TInt error;
        TRAP( error, rep = CRepository::NewL( TUid::Uid( KVcxMyVideosCenRepUid ) ) )

        if ( error == KErrNone )
            {
            CleanupStack::PushL( rep );
            
            error = rep->Get( KVcxMyvideosCenRepParentControlKey, parentControlSetting );
            
            if ( error == KErrNone && parentControlSetting != KVcxMyvideosCenRepParentControlKeyOff )
                {
                if ( static_cast<TInt>( data->ParentalControl() ) >= parentControlSetting )
                    {
                    CVcxHgVodTelephonyClient* tc = new( ELeave ) CVcxHgVodTelephonyClient();
                    CleanupStack::PushL(tc);
                    
                    if ( !tc->CheckLockCodeL() )
                        {
                        CErrorUI* eu = CErrorUI::NewLC();
                        eu->ShowGlobalErrorNoteL( KErrPermissionDenied );
                        CleanupStack::PopAndDestroy( eu );
                        User::Leave( KErrPermissionDenied );
                        }
                    
                    CleanupStack::PopAndDestroy( tc );
                    }
                }
            CleanupStack::PopAndDestroy( rep );
            }
        }

    if( data->MpxId() != KIdUndefined )
        {
        aMessage.iServiceId = data->MpxId();
		aMessage.iFullPath  = data->Uri();
        }
    else
        {
		aMessage.iServiceId = 0;
		aMessage.iFullPath = data->Uri();
        }
    
    CleanupStack::PopAndDestroy( lwApi );
    CleanupStack::PopAndDestroy( data );
    }

// ---------------------------------------------------------------------------
// CVideoActionHandler::TryBringVideoPlayerToForegroundL
// ---------------------------------------------------------------------------
//
TInt CVideoActionHandler::TryBringVideoPlayerToForegroundL()
    {
    TInt ret ( KErrNotFound );
    RWsSession session;
    User::LeaveIfError( session.Connect() );
	CleanupClosePushL( session );

    TInt ourWgId( 0 );
    TInt error = RProperty::Get( KVideoPlayerRPropertyCategory, KVideoPlayerRPropertyWGIdKey, ourWgId );
        
    if( error == KErrNone && ourWgId != 0 )
        {
        TBuf<KVideoPlayerWgNameSize> wgName;
        session.GetWindowGroupNameFromIdentifier(ourWgId, wgName );
        
        if( wgName.MatchF( KVideoPlayerWgName ) != KErrNotFound )
            {
            ret = session.SetWindowGroupOrdinalPosition( ourWgId, 0 );
            }
        }

	CleanupStack::PopAndDestroy( &session );
    return ret;
    }
