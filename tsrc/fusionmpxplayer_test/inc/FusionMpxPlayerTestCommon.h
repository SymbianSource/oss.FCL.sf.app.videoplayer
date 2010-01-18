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


#ifndef FUSIONMPXPLAYERTESTCOMMON_H
#define FUSIONMPXPLAYERTESTCOMMON_H

// Category for PS keys used by tests. Test module and exe must both use this.
const TUid KFusionMpxPlayerTestPScategory = { 0x101FB3E3 };

const TUint KFusionMpxPlayerTestExeGlobalTesterCount = 1;

// Set this to 1 to inform all the tester exe's for shutdown.
const TInt KFusionMpxPlayerTestExeGlobalShutdownKey = 2;

/**
 *  P&S key for commands, created by tester exe which adds it's own process id to this.
 */
const TUint KFusionMpxPlayerTestExePsKeyCmd = 10; // Observed for changes by tester exe.

/**
 *  Parameters for the commands, created by tester exe which adds it's own process id to these.
 */
const TUint KFusionMpxPlayerTestExePsKeyCmdIntParam1 = 11;
const TUint KFusionMpxPlayerTestExePsKeyCmdIntParam2 = 12;
const TUint KFusionMpxPlayerTestExePsKeyCmdDescParam1 = 13;

/**
 *  P&S keys for responses, created by tester exe which adds it's own process id to these.
 */
// This is sent after tester exe has received the cmd.
// Contains symbian error code.
const TUint KFusionMpxPlayerTestExePsKeyResponse = 20;
const TUint KFusionMpxPlayerTestExePsKeyResponseError = 21;

/**
 * List of commands to tester exe.
 */
enum TVcxFusionMpxPlayerCommand
    {
    // Params: none
    EVcxFusionMpxPlayerCommandShutdown, 

    // Descparam1: filename and path
    // Intparam1: start playback
    EVcxFusionMpxPlayerCommandOpenLocalFile,

    // Descparam1: url
    // Intparam1: iapId
    EVcxFusionMpxPlayerCommandOpenUrl,
    
    // Issues general playback command to player
    // Params: intparam1: command from mpxplaybackcommanddefs.h
    //         others: as needed
    EVcxFusionMpxPlayerCommandGeneralPlaybackCommand,  
    
    // Issues video playback command to player
    // Params: intparam1: command from  mpxvideoplaybackdefs.h
    //         others: as needed
    EVcxFusionMpxPlayerCommandVideoPlaybackCommand, 
    };

/**
 * List of responses from tester exe.
 */
enum TVcxFusionMpxPlayerResponse
    {
    // Sent after receiving command.
    EVcxFusionMpxPlayerResponseGeneralError = 1,
    // Sent after completing the command.
    EVcxFusionMpxPlayerResponseCommandComplete,
    // Sent after tester class is destroyed. 
    EVcxFusionMpxPlayerResponseShutdown,
    };

#endif // FUSIONMPXPLAYERTESTCOMMON_H
