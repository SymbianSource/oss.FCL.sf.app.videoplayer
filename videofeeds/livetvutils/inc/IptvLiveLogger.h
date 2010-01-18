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
* Description: 
*
*/


#ifndef __LIVETVLOGGER_H__
#define __LIVETVLOGGER_H__

//#include <e32std.h>
#include <e32debug.h>
#include <flogger.h>

_LIT( KDirectory, "livetv" );
_LIT( KFileName, "livetv2.log" );

//#define LIVE_TV_RDEBUG_TRACE
#ifdef _DEBUG
#define LIVE_TV_FILE_TRACE
#endif
	#ifdef LIVE_TV_RDEBUG_TRACE
	  #define LIVE_TV_TRACE1(a) RDebug::Print(a)
	  #define LIVE_TV_TRACE2(a,b) RDebug::Print(a,b)
	  #define LIVE_TV_TRACE3(a,b,c) RDebug::Print(a,b,c)
	  #define LIVE_TV_TRACE4(a,b,c,d) RDebug::Print(a,b,c,d)
	  #define LIVE_TV_TRACE5(a,b,c,d,e) RDebug::Print(a,b,c,d,e)
	#else
	  #ifdef LIVE_TV_FILE_TRACE
	    #define LIVE_TV_TRACE1(a) { RFileLogger::Write( KDirectory, KFileName, EFileLoggingModeAppend, a ); }
	    #define LIVE_TV_TRACE2(a,b) { RFileLogger::WriteFormat( KDirectory, KFileName, EFileLoggingModeAppend, a, b );}
	    #define LIVE_TV_TRACE3(a,b,c) { RFileLogger::WriteFormat( KDirectory, KFileName, EFileLoggingModeAppend, a, b, c ); }
	    #define LIVE_TV_TRACE4(a,b,c,d) { RFileLogger::WriteFormat( KDirectory, KFileName, EFileLoggingModeAppend, a, b, c, d ); }
	    #define LIVE_TV_TRACE5(a,b,c,d,e) { RFileLogger::WriteFormat( KDirectory, KFileName, EFileLoggingModeAppend, a, b, c, d, e ); }
	  #else
	    #define LIVE_TV_TRACE1(a)
	    #define LIVE_TV_TRACE2(a,b)
	    #define LIVE_TV_TRACE3(a,b,c)
	    #define LIVE_TV_TRACE4(a,b,c,d)
	    #define LIVE_TV_TRACE5(a,b,c,d,e)
	  #endif
	#endif
	
#endif// __LIVETVLOGGER_H__

// End of file
