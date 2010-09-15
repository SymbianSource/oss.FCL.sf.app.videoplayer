/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Helper class for serial or file tracing of Video Center.*
*/




#ifndef IPTVDEBUG_H
#define IPTVDEBUG_H

#include <flogger.h>

// Define logging level here

// Minimum log output
#define IPTV_HIGH_LEVEL_TRACES

// Medium log output
#define IPTV_MIDDLE_LEVEL_TRACES

// Maximum log output
#define IPTV_LOW_LEVEL_TRACES


/*
-----------------------------------------------------------------------------

	LOGGING MACROs

	USE THESE MACROS IN YOUR CODE

	Usage:
	
	Example: IPTVLOGSTRING_HIGH_LEVEL ("Test");
	Example: IPTVLOGSTRING2_HIGH_LEVEL ("Test %d", aValue);
	Example: IPTVLOGSTRING3_HIGH_LEVEL ("Test %d %d", aValue1, aValue2);
	Example: IPTVLOGSTRING4_HIGH_LEVEL ("Test %d %d %d", aValue1, aValue2, aValue3);

	Example: IPTV_LOG_STR_DESC_HIGH_LEVEL (own_desc);
	Example: IPTV_LOG_STR_DESC2_HIGH_LEVEL (own_desc, aValue));
	Example: IPTV_LOG_STR_DESC3_HIGH_LEVEL (own_desc, aValue, aValue2);
	Example: IPTV_LOG_STR_DESC4_HIGH_LEVEL (own_desc, aValue, aValue2, aValue3);

-----------------------------------------------------------------------------
*/


#ifdef IPTV_HIGH_LEVEL_TRACES

#define IPTVLOGTEXT_HIGH_LEVEL(AAAA)   IPTV_LOGTEXT(AAAA)
#define IPTVLOGSTRING_HIGH_LEVEL(AAAA) 	IPTV_LOGSTRING(AAAA) 
#define IPTVLOGSTRING2_HIGH_LEVEL(AAAA,BBBB) 	IPTV_LOGSTRING2(AAAA,BBBB) 
#define IPTVLOGSTRING3_HIGH_LEVEL(AAAA,BBBB,CCCC)   IPTV_LOGSTRING3(AAAA,BBBB,CCCC) 
#define IPTVLOGSTRING4_HIGH_LEVEL(AAAA,BBBB,CCCC,DDDD)   IPTV_LOGSTRING4(AAAA,BBBB,CCCC,DDDD) 

#define IPTV_LOG_STR_DESC_HIGH_LEVEL(AAAA) 	IPTV_LOG_STR_DESC(AAAA) 
#define IPTV_LOG_STR_DESC2_HIGH_LEVEL(AAAA,BBBB) 	IPTV_LOG_STR_DESC2(AAAA,BBBB) 
#define IPTV_LOG_STR_DESC3_HIGH_LEVEL(AAAA,BBBB,CCCC)   IPTV_LOG_STR_DESC3(AAAA,BBBB,CCCC) 
#define IPTV_LOG_STR_DESC4_HIGH_LEVEL(AAAA,BBBB,CCCC,DDDD)   IPTV_LOG_STR_DESC4(AAAA,BBBB,CCCC,DDDD) 

#else

#define IPTVLOGTEXT_HIGH_LEVEL(AAAA)
#define IPTVLOGSTRING_HIGH_LEVEL(AAAA)
#define IPTVLOGSTRING2_HIGH_LEVEL(AAAA,BBBB)
#define IPTVLOGSTRING3_HIGH_LEVEL(AAAA,BBBB,CCCC)
#define IPTVLOGSTRING4_HIGH_LEVEL(AAAA,BBBB,CCCC,DDDD)

#define IPTV_LOG_STR_DESC_HIGH_LEVEL(AAAA)
#define IPTV_LOG_STR_DESC2_HIGH_LEVEL(AAAA,BBBB)
#define IPTV_LOG_STR_DESC3_HIGH_LEVEL(AAAA,BBBB,CCCC)
#define IPTV_LOG_STR_DESC4_HIGH_LEVEL(AAAA,BBBB,CCCC,DDDD)

#endif


#ifdef IPTV_MIDDLE_LEVEL_TRACES
#define TFLOGTEXT_MIDDLE_LEVEL(AAAA) IPTV_LOGTEXT(AAAA)
#define IPTVLOGSTRING_MIDDLE_LEVEL(AAAA) IPTV_LOGSTRING(AAAA)
#define IPTVLOGSTRING2_MIDDLE_LEVEL(AAAA,BBBB) IPTV_LOGSTRING2(AAAA,BBBB)
#define IPTVLOGSTRING3_MIDDLE_LEVEL(AAAA,BBBB,CCCC) IPTV_LOGSTRING3(AAAA,BBBB,CCCC)
#define IPTVLOGSTRING4_MIDDLE_LEVEL(AAAA,BBBB,CCCC,DDDD) IPTV_LOGSTRING4(AAAA,BBBB,CCCC,DDDD)

#define IPTV_LOG_STR_DESC_MIDDLE_LEVEL(AAAA) 	IPTV_LOG_STR_DESC(AAAA) 
#define IPTV_LOG_STR_DESC2_MIDDLE_LEVEL(AAAA,BBBB) 	IPTV_LOG_STR_DESC2(AAAA,BBBB) 
#define IPTV_LOG_STR_DESC3_MIDDLE_LEVEL(AAAA,BBBB,CCCC)   IPTV_LOG_STR_DESC3(AAAA,BBBB,CCCC) 
#define IPTV_LOG_STR_DESC4_MIDDLE_LEVEL(AAAA,BBBB,CCCC,DDDD)   IPTV_LOG_STR_DESC4(AAAA,BBBB,CCCC,DDDD) 

#else

#define TFLOGTEXT_MIDDLE_LEVEL(AAAA)
#define IPTVLOGSTRING_MIDDLE_LEVEL(AAAA)
#define IPTVLOGSTRING2_MIDDLE_LEVEL(AAAA,BBBB)
#define IPTVLOGSTRING3_MIDDLE_LEVEL(AAAA,BBBB,CCCC)
#define IPTVLOGSTRING4_MIDDLE_LEVEL(AAAA,BBBB,CCCC,DDDD)

#define IPTV_LOG_STR_DESC_MIDDLE_LEVEL(AAAA)
#define IPTV_LOG_STR_DESC2_MIDDLE_LEVEL(AAAA,BBBB)
#define IPTV_LOG_STR_DESC3_MIDDLE_LEVEL(AAAA,BBBB,CCCC)
#define IPTV_LOG_STR_DESC4_MIDDLE_LEVEL(AAAA,BBBB,CCCC,DDDD)

#endif
	    

#ifdef IPTV_LOW_LEVEL_TRACES

#define IPTVLOGTEXT_LOW_LEVEL(AAAA)    IPTV_LOGTEXT(AAAA)
#define IPTVLOGSTRING_LOW_LEVEL(AAAA)    IPTV_LOGSTRING(AAAA)
#define IPTVLOGSTRING2_LOW_LEVEL(AAAA,BBBB)    IPTV_LOGSTRING2(AAAA,BBBB)
#define IPTVLOGSTRING3_LOW_LEVEL(AAAA,BBBB,CCCC)    IPTV_LOGSTRING3(AAAA,BBBB,CCCC)
#define IPTVLOGSTRING4_LOW_LEVEL(AAAA,BBBB,CCCC,DDDD)    IPTV_LOGSTRING4(AAAA,BBBB,CCCC,DDDD)

#define IPTV_LOG_STR_DESC_LOW_LEVEL(AAAA) 	IPTV_LOG_STR_DESC(AAAA) 
#define IPTV_LOG_STR_DESC2_LOW_LEVEL(AAAA,BBBB) 	IPTV_LOG_STR_DESC2(AAAA,BBBB) 
#define IPTV_LOG_STR_DESC3_LOW_LEVEL(AAAA,BBBB,CCCC)   IPTV_LOG_STR_DESC3(AAAA,BBBB,CCCC) 
#define IPTV_LOG_STR_DESC4_LOW_LEVEL(AAAA,BBBB,CCCC,DDDD)   IPTV_LOG_STR_DESC4(AAAA,BBBB,CCCC,DDDD) 

#else

#define IPTVLOGTEXT_LOW_LEVEL(AAAA)
#define IPTVLOGSTRING_LOW_LEVEL(AAAA)
#define IPTVLOGSTRING2_LOW_LEVEL(AAAA,BBBB)
#define IPTVLOGSTRING3_LOW_LEVEL(AAAA,BBBB,CCCC)
#define IPTVLOGSTRING4_LOW_LEVEL(AAAA,BBBB,CCCC,DDDD)

#define IPTV_LOG_STR_DESC_LOW_LEVEL(AAAA)
#define IPTV_LOG_STR_DESC2_LOW_LEVEL(AAAA,BBBB)
#define IPTV_LOG_STR_DESC3_LOW_LEVEL(AAAA,BBBB,CCCC)
#define IPTV_LOG_STR_DESC4_LOW_LEVEL(AAAA,BBBB,CCCC,DDDD)

#endif


/* 
-----------------------------------------------------------------------------

	LOG SELECTION

-----------------------------------------------------------------------------
*/


// 0 = No logging, 
// 1 = Flogger, 
// 2 = RDebug


#ifndef _DEBUG

// UREL BUILD:
#define IPTV_LOGGING_METHOD  0   // No logging in UREL builds

#else // urel


#ifdef __WINSCW__

// WINSCW BUILD:
#define IPTV_LOGGING_METHOD  2  // RDebug is default with emulator

#else

// ARMV5 BUILD:
#define IPTV_LOGGING_METHOD  2  // RDebug is default with target device

#endif


#endif // _DEBUG


/* 
-----------------------------------------------------------------------------

	LOG SETTINGS

-----------------------------------------------------------------------------
*/

#if IPTV_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KIPTVLogFolder,"IPTV");
_LIT(KIPTVLogFile,"IPTVLOG.TXT");

#elif IPTV_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#endif





#if IPTV_LOGGING_METHOD == 1      // Flogger


#define IPTV_LOGTEXT(AAA)                RFileLogger::Write(KIPTVLogFolder(),KIPTVLogFile(),EFileLoggingModeAppend, AAA)

#define IPTV_LOG_STR_DESC(AAA)                RFileLogger::Write(KIPTVLogFolder(),KIPTVLogFile(),EFileLoggingModeAppend, AAA)
#define IPTV_LOG_STR_DESC2(AAA,BBB)         /*lint -e{717}*/ do { RFileLogger::WriteFormat(KIPTVLogFolder(),KIPTVLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA()),BBB); } while ( EFalse )
#define IPTV_LOG_STR_DESC3(AAA,BBB,CCC)     /*lint -e{717}*/ do { RFileLogger::WriteFormat(KIPTVLogFolder(),KIPTVLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA()),BBB,CCC); } while ( EFalse )
#define IPTV_LOG_STR_DESC4(AAA,BBB,CCC,DDD) /*lint -e{717}*/ do { RFileLogger::WriteFormat(KIPTVLogFolder(),KIPTVLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA()),BBB,CCC,DDD); } while ( EFalse )

#define IPTV_LOGSTRING(AAA)                 /*lint -e{717}*/ do { _LIT(tempIPTVLogDes,AAA); RFileLogger::Write(KIPTVLogFolder(),KIPTVLogFile(),EFileLoggingModeAppend,tempIPTVLogDes()); } while ( EFalse )
#define IPTV_LOGSTRING2(AAA,BBB)            /*lint -e{717}*/ do { _LIT(tempIPTVLogDes,AAA); RFileLogger::WriteFormat(KIPTVLogFolder(),KIPTVLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempIPTVLogDes()),BBB); } while ( EFalse )
#define IPTV_LOGSTRING3(AAA,BBB,CCC)        /*lint -e{717}*/ do { _LIT(tempIPTVLogDes,AAA); RFileLogger::WriteFormat(KIPTVLogFolder(),KIPTVLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempIPTVLogDes()),BBB,CCC); } while ( EFalse )
#define IPTV_LOGSTRING4(AAA,BBB,CCC,DDD)    /*lint -e{717}*/ do { _LIT(tempIPTVLogDes,AAA); RFileLogger::WriteFormat(KIPTVLogFolder(),KIPTVLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempIPTVLogDes()),BBB,CCC,DDD); } while ( EFalse )

// Time stamp
_LIT(KIptvLogTimeFormatString, "IPTV TIMESTAMP: %H:%T:%S:%*C3");
#define IPTV_LOGSTRING_TIMESTAMP { \
                                 TTime logTime; \
                                 logTime.HomeTime(); \
                                 TBuf<256> logBuffer; \
                                 logTime.FormatL(logBuffer, KIptvLogTimeFormatString); \
                                 RFileLogger::Write(KIPTVLogFolder(), KIPTVLogFile(), EFileLoggingModeAppend, logBuffer); \
                                 }

// Memory stamp
_LIT(KIptvLogMemoryStampString, "IPTV MEMORYSTAMP: %d KB");
#define IPTV_LOGSTRING_MEMORYSTAMP { \
                                   User::CompressAllHeaps(); \
                                   TMemoryInfoV1Buf logMemory; \
                                   UserHal::MemoryInfo(logMemory); \
                                   TInt logMemoryInt = (TInt)(logMemory().iFreeRamInBytes); \
                                   TBuf<256> logMemoryStr; \
                                   logMemoryStr.Format(KIptvLogMemoryStampString, (logMemoryInt / 1024) ); \
                                   RFileLogger::Write(KIPTVLogFolder(), KIPTVLogFile(), EFileLoggingModeAppend, logMemoryStr); \
                                   }


#elif IPTV_LOGGING_METHOD == 2    // RDebug


#define IPTV_LOGTEXT(AAA)                    RDebug::Print(AAA)

#define IPTV_LOG_STR_DESC(AAA)               RDebug::Print(AAA)
#define IPTV_LOG_STR_DESC2(AAA,BBB)       	 /*lint -e{717}*/ do {  RDebug::Print(AAA, BBB); } while ( EFalse )
#define IPTV_LOG_STR_DESC3(AAA,BBB,CCC)      /*lint -e{717}*/ do {  RDebug::Print(AAA, BBB, CCC); } while ( EFalse )
#define IPTV_LOG_STR_DESC4(AAA,BBB,CCC,DDD)  /*lint -e{717}*/ do {  RDebug::Print(AAA, BBB, CCC, DDD); } while ( EFalse )

#define IPTV_LOGSTRING(AAA)                  /*lint -e{717}*/ do { _LIT(tempIPTVLogDes,AAA); RDebug::Print(tempIPTVLogDes); } while ( EFalse )
#define IPTV_LOGSTRING2(AAA,BBB)             /*lint -e{717}*/ do { _LIT(tempIPTVLogDes,AAA); RDebug::Print(tempIPTVLogDes, BBB); } while ( EFalse )
#define IPTV_LOGSTRING3(AAA,BBB,CCC)         /*lint -e{717}*/ do { _LIT(tempIPTVLogDes,AAA); RDebug::Print(tempIPTVLogDes, BBB, CCC); } while ( EFalse )
#define IPTV_LOGSTRING4(AAA,BBB,CCC,DDD)     /*lint -e{717}*/ do { _LIT(tempIPTVLogDes,AAA); RDebug::Print(tempIPTVLogDes, BBB, CCC, DDD); } while ( EFalse )

// Time stamp
_LIT(KIptvLogTimeFormatString, "IPTV TIMESTAMP: %H:%T:%S:%*C3");
#define IPTV_LOGSTRING_TIMESTAMP { \
                                 TTime logTime; \
                                 logTime.HomeTime(); \
                                 TBuf<256> logBuffer; \
                                 logTime.FormatL(logBuffer, KIptvLogTimeFormatString); \
                                 RDebug::Print(logBuffer); \
                                 }

// Memory stamp
_LIT(KIptvLogMemoryStampString, "IPTV MEMORYSTAMP: %d KB");
#define IPTV_LOGSTRING_MEMORYSTAMP { \
                                   User::CompressAllHeaps(); \
                                   TMemoryInfoV1Buf logMemory; \
                                   UserHal::MemoryInfo(logMemory); \
                                   TInt logMemoryInt = (TInt)(logMemory().iFreeRamInBytes); \
                                   TBuf<256> logMemoryStr; \
                                   logMemoryStr.Format(KIptvLogMemoryStampString, (logMemoryInt / 1024) ); \
                                   RDebug::Print(logMemoryStr); \
                                   }


#else	// TF_LOGGING_METHOD == 0 or invalid


#define IPTV_LOGSTRING(AAA)              
#define IPTV_LOGSTRING2(AAA,BBB)         
#define IPTV_LOGSTRING3(AAA,BBB,CCC)     
#define IPTV_LOGSTRING4(AAA,BBB,CCC,DDD) 

#define IPTV_LOGTEXT(AAA)                

#define IPTV_LOG_STR_DESC(AAA)              
#define IPTV_LOG_STR_DESC2(AAA,BBB)         
#define IPTV_LOG_STR_DESC3(AAA,BBB,CCC)     
#define IPTV_LOG_STR_DESC4(AAA,BBB,CCC,DDD) 

#define IPTV_LOGSTRING_TIMESTAMP
#define IPTV_LOGSTRING_MEMORYSTAMP


#endif // TF_LOGGING_METHOD


#endif // IPTVDEBUG_H
