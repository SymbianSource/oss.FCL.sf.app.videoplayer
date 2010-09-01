/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Helper class for serial or file tracing
 *
*/

// Version : %version: da1mmcf#3 %

#ifndef MPXVPDEBUG_H
#define MPXVPDEBUG_H

#include <flogger.h>

// Define logging level here

// Minimum log output
#define MPXVP_HIGH_LEVEL_TRACES

// Medium log output
#define MPXVP_MIDDLE_LEVEL_TRACES

// Maximum log output
#define MPXVP_LOW_LEVEL_TRACES


/*
-----------------------------------------------------------------------------

	LOGGING MACROs

	USE THESE MACROS IN YOUR CODE

	Usage:
	
	Example: MPXVPLOGSTRING_HIGH_LEVEL ("Test");
	Example: MPXVPLOGSTRING2_HIGH_LEVEL ("Test %d", aValue);
	Example: MPXVPLOGSTRING3_HIGH_LEVEL ("Test %d %d", aValue1, aValue2);
	Example: MPXVPLOGSTRING4_HIGH_LEVEL ("Test %d %d %d", aValue1, aValue2, aValue3);

	Example: MPXVP_LOG_STR_DESC_HIGH_LEVEL (own_desc);
	Example: MPXVP_LOG_STR_DESC2_HIGH_LEVEL (own_desc, aValue));
	Example: MPXVP_LOG_STR_DESC3_HIGH_LEVEL (own_desc, aValue, aValue2);
	Example: MPXVP_LOG_STR_DESC4_HIGH_LEVEL (own_desc, aValue, aValue2, aValue3);

-----------------------------------------------------------------------------
*/


#ifdef MPXVP_HIGH_LEVEL_TRACES

#define MPXVPLOGTEXT_HIGH_LEVEL(AAAA)   MPXVP_LOGTEXT(AAAA)
#define MPXVPLOGSTRING_HIGH_LEVEL(AAAA) 	MPXVP_LOGSTRING(AAAA) 
#define MPXVPLOGSTRING2_HIGH_LEVEL(AAAA,BBBB) 	MPXVP_LOGSTRING2(AAAA,BBBB) 
#define MPXVPLOGSTRING3_HIGH_LEVEL(AAAA,BBBB,CCCC)   MPXVP_LOGSTRING3(AAAA,BBBB,CCCC) 
#define MPXVPLOGSTRING4_HIGH_LEVEL(AAAA,BBBB,CCCC,DDDD)   MPXVP_LOGSTRING4(AAAA,BBBB,CCCC,DDDD) 

#define MPXVP_LOG_STR_DESC_HIGH_LEVEL(AAAA) 	MPXVP_LOG_STR_DESC(AAAA) 
#define MPXVP_LOG_STR_DESC2_HIGH_LEVEL(AAAA,BBBB) 	MPXVP_LOG_STR_DESC2(AAAA,BBBB) 
#define MPXVP_LOG_STR_DESC3_HIGH_LEVEL(AAAA,BBBB,CCCC)   MPXVP_LOG_STR_DESC3(AAAA,BBBB,CCCC) 
#define MPXVP_LOG_STR_DESC4_HIGH_LEVEL(AAAA,BBBB,CCCC,DDDD)   MPXVP_LOG_STR_DESC4(AAAA,BBBB,CCCC,DDDD) 

#else

#define MPXVPLOGTEXT_HIGH_LEVEL(AAAA)
#define MPXVPLOGSTRING_HIGH_LEVEL(AAAA)
#define MPXVPLOGSTRING2_HIGH_LEVEL(AAAA,BBBB)
#define MPXVPLOGSTRING3_HIGH_LEVEL(AAAA,BBBB,CCCC)
#define MPXVPLOGSTRING4_HIGH_LEVEL(AAAA,BBBB,CCCC,DDDD)

#define MPXVP_LOG_STR_DESC_HIGH_LEVEL(AAAA)
#define MPXVP_LOG_STR_DESC2_HIGH_LEVEL(AAAA,BBBB)
#define MPXVP_LOG_STR_DESC3_HIGH_LEVEL(AAAA,BBBB,CCCC)
#define MPXVP_LOG_STR_DESC4_HIGH_LEVEL(AAAA,BBBB,CCCC,DDDD)

#endif


#ifdef MPXVP_MIDDLE_LEVEL_TRACES
#define TFLOGTEXT_MIDDLE_LEVEL(AAAA) MPXVP_LOGTEXT(AAAA)
#define MPXVPLOGSTRING_MIDDLE_LEVEL(AAAA) MPXVP_LOGSTRING(AAAA)
#define MPXVPLOGSTRING2_MIDDLE_LEVEL(AAAA,BBBB) MPXVP_LOGSTRING2(AAAA,BBBB)
#define MPXVPLOGSTRING3_MIDDLE_LEVEL(AAAA,BBBB,CCCC) MPXVP_LOGSTRING3(AAAA,BBBB,CCCC)
#define MPXVPLOGSTRING4_MIDDLE_LEVEL(AAAA,BBBB,CCCC,DDDD) MPXVP_LOGSTRING4(AAAA,BBBB,CCCC,DDDD)

#define MPXVP_LOG_STR_DESC_MIDDLE_LEVEL(AAAA) 	MPXVP_LOG_STR_DESC(AAAA) 
#define MPXVP_LOG_STR_DESC2_MIDDLE_LEVEL(AAAA,BBBB) 	MPXVP_LOG_STR_DESC2(AAAA,BBBB) 
#define MPXVP_LOG_STR_DESC3_MIDDLE_LEVEL(AAAA,BBBB,CCCC)   MPXVP_LOG_STR_DESC3(AAAA,BBBB,CCCC) 
#define MPXVP_LOG_STR_DESC4_MIDDLE_LEVEL(AAAA,BBBB,CCCC,DDDD)   MPXVP_LOG_STR_DESC4(AAAA,BBBB,CCCC,DDDD) 

#else

#define TFLOGTEXT_MIDDLE_LEVEL(AAAA)
#define MPXVPLOGSTRING_MIDDLE_LEVEL(AAAA)
#define MPXVPLOGSTRING2_MIDDLE_LEVEL(AAAA,BBBB)
#define MPXVPLOGSTRING3_MIDDLE_LEVEL(AAAA,BBBB,CCCC)
#define MPXVPLOGSTRING4_MIDDLE_LEVEL(AAAA,BBBB,CCCC,DDDD)

#define MPXVP_LOG_STR_DESC_MIDDLE_LEVEL(AAAA)
#define MPXVP_LOG_STR_DESC2_MIDDLE_LEVEL(AAAA,BBBB)
#define MPXVP_LOG_STR_DESC3_MIDDLE_LEVEL(AAAA,BBBB,CCCC)
#define MPXVP_LOG_STR_DESC4_MIDDLE_LEVEL(AAAA,BBBB,CCCC,DDDD)

#endif
	    

#ifdef MPXVP_LOW_LEVEL_TRACES

#define MPXVPLOGTEXT_LOW_LEVEL(AAAA)    MPXVP_LOGTEXT(AAAA)
#define MPXVPLOGSTRING_LOW_LEVEL(AAAA)    MPXVP_LOGSTRING(AAAA)
#define MPXVPLOGSTRING2_LOW_LEVEL(AAAA,BBBB)    MPXVP_LOGSTRING2(AAAA,BBBB)
#define MPXVPLOGSTRING3_LOW_LEVEL(AAAA,BBBB,CCCC)    MPXVP_LOGSTRING3(AAAA,BBBB,CCCC)
#define MPXVPLOGSTRING4_LOW_LEVEL(AAAA,BBBB,CCCC,DDDD)    MPXVP_LOGSTRING4(AAAA,BBBB,CCCC,DDDD)

#define MPXVP_LOG_STR_DESC_LOW_LEVEL(AAAA) 	MPXVP_LOG_STR_DESC(AAAA) 
#define MPXVP_LOG_STR_DESC2_LOW_LEVEL(AAAA,BBBB) 	MPXVP_LOG_STR_DESC2(AAAA,BBBB) 
#define MPXVP_LOG_STR_DESC3_LOW_LEVEL(AAAA,BBBB,CCCC)   MPXVP_LOG_STR_DESC3(AAAA,BBBB,CCCC) 
#define MPXVP_LOG_STR_DESC4_LOW_LEVEL(AAAA,BBBB,CCCC,DDDD)   MPXVP_LOG_STR_DESC4(AAAA,BBBB,CCCC,DDDD) 

#else

#define MPXVPLOGTEXT_LOW_LEVEL(AAAA)
#define MPXVPLOGSTRING_LOW_LEVEL(AAAA)
#define MPXVPLOGSTRING2_LOW_LEVEL(AAAA,BBBB)
#define MPXVPLOGSTRING3_LOW_LEVEL(AAAA,BBBB,CCCC)
#define MPXVPLOGSTRING4_LOW_LEVEL(AAAA,BBBB,CCCC,DDDD)

#define MPXVP_LOG_STR_DESC_LOW_LEVEL(AAAA)
#define MPXVP_LOG_STR_DESC2_LOW_LEVEL(AAAA,BBBB)
#define MPXVP_LOG_STR_DESC3_LOW_LEVEL(AAAA,BBBB,CCCC)
#define MPXVP_LOG_STR_DESC4_LOW_LEVEL(AAAA,BBBB,CCCC,DDDD)

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
#define MPXVP_LOGGING_METHOD  2   // No logging in UREL builds

#else // urel


#ifdef __WINSCW__

// WINSCW BUILD:
#define MPXVP_LOGGING_METHOD  2  // RDebug is default with emulator

#else

// ARMV5 BUILD:
#define MPXVP_LOGGING_METHOD  2  // Flogger is default with target device

#endif


#endif // _DEBUG


/* 
-----------------------------------------------------------------------------

	LOG SETTINGS

-----------------------------------------------------------------------------
*/

#if MPXVP_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KMPXVPLogFolder,"MPXVP");
_LIT(KMPXVPLogFile,"MPXVPLOG.TXT");

#elif MPXVP_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#endif





#if MPXVP_LOGGING_METHOD == 1      // Flogger


#define MPXVP_LOGTEXT(AAA)                RFileLogger::Write(KMPXVPLogFolder(),KMPXVPLogFile(),EFileLoggingModeAppend, AAA)

#define MPXVP_LOG_STR_DESC(AAA)                RFileLogger::Write(KMPXVPLogFolder(),KMPXVPLogFile(),EFileLoggingModeAppend, AAA)
#define MPXVP_LOG_STR_DESC2(AAA,BBB)         /*lint -e{717}*/ do { RFileLogger::WriteFormat(KMPXVPLogFolder(),KMPXVPLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA()),BBB); } while ( EFalse )
#define MPXVP_LOG_STR_DESC3(AAA,BBB,CCC)     /*lint -e{717}*/ do { RFileLogger::WriteFormat(KMPXVPLogFolder(),KMPXVPLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA()),BBB,CCC); } while ( EFalse )
#define MPXVP_LOG_STR_DESC4(AAA,BBB,CCC,DDD) /*lint -e{717}*/ do { RFileLogger::WriteFormat(KMPXVPLogFolder(),KMPXVPLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA()),BBB,CCC,DDD); } while ( EFalse )

#define MPXVP_LOGSTRING(AAA)                 /*lint -e{717}*/ do { _LIT(tempMPXVPLogDes,AAA); RFileLogger::Write(KMPXVPLogFolder(),KMPXVPLogFile(),EFileLoggingModeAppend,tempMPXVPLogDes()); } while ( EFalse )
#define MPXVP_LOGSTRING2(AAA,BBB)            /*lint -e{717}*/ do { _LIT(tempMPXVPLogDes,AAA); RFileLogger::WriteFormat(KMPXVPLogFolder(),KMPXVPLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempMPXVPLogDes()),BBB); } while ( EFalse )
#define MPXVP_LOGSTRING3(AAA,BBB,CCC)        /*lint -e{717}*/ do { _LIT(tempMPXVPLogDes,AAA); RFileLogger::WriteFormat(KMPXVPLogFolder(),KMPXVPLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempMPXVPLogDes()),BBB,CCC); } while ( EFalse )
#define MPXVP_LOGSTRING4(AAA,BBB,CCC,DDD)    /*lint -e{717}*/ do { _LIT(tempMPXVPLogDes,AAA); RFileLogger::WriteFormat(KMPXVPLogFolder(),KMPXVPLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempMPXVPLogDes()),BBB,CCC,DDD); } while ( EFalse )

// Time stamp
_LIT(KMpxvpLogTimeFormatString, "MPXVP TIMESTAMP: %H:%T:%S:%*C3");
#define MPXVP_LOGSTRING_TIMESTAMP { \
                                 TTime logTime; \
                                 logTime.HomeTime(); \
                                 TBuf<256> logBuffer; \
                                 logTime.FormatL(logBuffer, KMpxvpLogTimeFormatString); \
                                 RFileLogger::Write(KMPXVPLogFolder(), KMPXVPLogFile(), EFileLoggingModeAppend, logBuffer); \
                                 }

// Memory stamp
_LIT(KMpxvpLogMemoryStampString, "MPXVP MEMORYSTAMP: %d KB");
#define MPXVP_LOGSTRING_MEMORYSTAMP { \
                                   User::CompressAllHeaps(); \
                                   TMemoryInfoV1Buf logMemory; \
                                   UserHal::MemoryInfo(logMemory); \
                                   TInt logMemoryInt = (TInt)(logMemory().iFreeRamInBytes); \
                                   TBuf<256> logMemoryStr; \
                                   logMemoryStr.Format(KMpxvpLogMemoryStampString, (logMemoryInt / 1024) ); \
                                   RFileLogger::Write(KMPXVPLogFolder(), KMPXVPLogFile(), EFileLoggingModeAppend, logMemoryStr); \
                                   }


#elif MPXVP_LOGGING_METHOD == 2    // RDebug


#define MPXVP_LOGTEXT(AAA)                    RDebug::Print(AAA)

#define MPXVP_LOG_STR_DESC(AAA)               RDebug::Print(AAA)
#define MPXVP_LOG_STR_DESC2(AAA,BBB)       	 /*lint -e{717}*/ do {  RDebug::Print(AAA, BBB); } while ( EFalse )
#define MPXVP_LOG_STR_DESC3(AAA,BBB,CCC)      /*lint -e{717}*/ do {  RDebug::Print(AAA, BBB, CCC); } while ( EFalse )
#define MPXVP_LOG_STR_DESC4(AAA,BBB,CCC,DDD)  /*lint -e{717}*/ do {  RDebug::Print(AAA, BBB, CCC, DDD); } while ( EFalse )

#define MPXVP_LOGSTRING(AAA)                  /*lint -e{717}*/ do { _LIT(tempMPXVPLogDes,AAA); RDebug::Print(tempMPXVPLogDes); } while ( EFalse )
#define MPXVP_LOGSTRING2(AAA,BBB)             /*lint -e{717}*/ do { _LIT(tempMPXVPLogDes,AAA); RDebug::Print(tempMPXVPLogDes, BBB); } while ( EFalse )
#define MPXVP_LOGSTRING3(AAA,BBB,CCC)         /*lint -e{717}*/ do { _LIT(tempMPXVPLogDes,AAA); RDebug::Print(tempMPXVPLogDes, BBB, CCC); } while ( EFalse )
#define MPXVP_LOGSTRING4(AAA,BBB,CCC,DDD)     /*lint -e{717}*/ do { _LIT(tempMPXVPLogDes,AAA); RDebug::Print(tempMPXVPLogDes, BBB, CCC, DDD); } while ( EFalse )

// Time stamp
_LIT(KMpxvpLogTimeFormatString, "MPXVP TIMESTAMP: %H:%T:%S:%*C3");
#define MPXVP_LOGSTRING_TIMESTAMP { \
                                 TTime logTime; \
                                 logTime.HomeTime(); \
                                 TBuf<256> logBuffer; \
                                 logTime.FormatL(logBuffer, KMpxvpLogTimeFormatString); \
                                 RDebug::Print(logBuffer); \
                                 }

// Memory stamp
_LIT(KMpxvpLogMemoryStampString, "MPXVP MEMORYSTAMP: %d KB");
#define MPXVP_LOGSTRING_MEMORYSTAMP { \
                                   User::CompressAllHeaps(); \
                                   TMemoryInfoV1Buf logMemory; \
                                   UserHal::MemoryInfo(logMemory); \
                                   TInt logMemoryInt = (TInt)(logMemory().iFreeRamInBytes); \
                                   TBuf<256> logMemoryStr; \
                                   logMemoryStr.Format(KMpxvpLogMemoryStampString, (logMemoryInt / 1024) ); \
                                   RDebug::Print(logMemoryStr); \
                                   }


#else	// TF_LOGGING_METHOD == 0 or invalid


#define MPXVP_LOGSTRING(AAA)              
#define MPXVP_LOGSTRING2(AAA,BBB)         
#define MPXVP_LOGSTRING3(AAA,BBB,CCC)     
#define MPXVP_LOGSTRING4(AAA,BBB,CCC,DDD) 

#define MPXVP_LOGTEXT(AAA)                

#define MPXVP_LOG_STR_DESC(AAA)              
#define MPXVP_LOG_STR_DESC2(AAA,BBB)         
#define MPXVP_LOG_STR_DESC3(AAA,BBB,CCC)     
#define MPXVP_LOG_STR_DESC4(AAA,BBB,CCC,DDD) 

#define MPXVP_LOGSTRING_TIMESTAMP
#define MPXVP_LOGSTRING_MEMORYSTAMP


#endif // TF_LOGGING_METHOD


#endif // MPXVPDEBUG_H
