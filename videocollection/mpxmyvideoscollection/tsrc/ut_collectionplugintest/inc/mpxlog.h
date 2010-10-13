/*
* ============================================================================
*  Name        : mpxlog.h
*  Part of     : MPX / Common framework
*  Description : Debug print macros
*  Version     : %version: 1 % 
*
*  Copyright ?2006 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
* Template version: 4.1
*/

// !! README README README !! 

// This is a modified version for Video Center of the original mpxlog.h file.
// Use enable.cmd to take this version into use and disable.cmd to start
// using the original. Notice that you have to compile sw again to get trace
// changes active.

// !! README README README !! 

#ifndef MPXLOG_H
#define MPXLOG_H

// INCLUDES
#include <e32std.h>
#include <e32svr.h>

#ifdef _DEBUG
#define MPX_PERFORMANCE_LOG
#endif

// MACROS
#ifdef _DEBUG

// SELECT LOG TYPE FROM HERE
// 0 = RDebug
// 1 = file logging
// 2 = no logging
#define LOGTYPE 0


#if LOGTYPE == 0

#define MPX_DEBUG1(s)               { TBuf<250> buf; buf.Append(_L("20016B96: ")); buf.Append(_L(#s)); RDebug::Print( buf ); }
#define MPX_DEBUG2(s, a)            { TBuf<250> buf; buf.Append(_L("20016B96: ")); buf.Append(_L(#s)); RDebug::Print( buf, a ); }
#define MPX_DEBUG3(s, a, b)         { TBuf<250> buf; buf.Append(_L("20016B96: ")); buf.Append(_L(#s)); RDebug::Print( buf, a, b ); }
#define MPX_DEBUG4(s, a, b, c)      { TBuf<250> buf; buf.Append(_L("20016B96: ")); buf.Append(_L(#s)); RDebug::Print( buf, a, b, c ); }
#define MPX_DEBUG5(s, a, b, c, d)   { TBuf<250> buf; buf.Append(_L("20016B96: ")); buf.Append(_L(#s)); RDebug::Print( buf, a, b, c, d ); }

#define MPX_FUNC(fn)                TFunctionTrace _s(_L(fn));
#define MPX_FUNC_EX(fn)             TFunctionTrace _s(_L(fn), this);

#endif

#if LOGTYPE == 1

#include <flogger.h>

_LIT(KIPTVLogFolder2,"IPTV");
_LIT(KIPTVLogFile2,"IPTVLOG.TXT");

#define MPX_DEBUG1(AAA)                  { _LIT(tempIPTVLogDes,AAA); RFileLogger::Write(KIPTVLogFolder2(),KIPTVLogFile2(),EFileLoggingModeAppend,tempIPTVLogDes()); }
#define MPX_DEBUG2(AAA,BBB)              { _LIT(tempIPTVLogDes,AAA); RFileLogger::WriteFormat(KIPTVLogFolder2(),KIPTVLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempIPTVLogDes()),BBB); }
#define MPX_DEBUG3(AAA,BBB,CCC)          { _LIT(tempIPTVLogDes,AAA); RFileLogger::WriteFormat(KIPTVLogFolder2(),KIPTVLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempIPTVLogDes()),BBB,CCC); }
#define MPX_DEBUG4(AAA,BBB,CCC,DDD)      { _LIT(tempIPTVLogDes,AAA); RFileLogger::WriteFormat(KIPTVLogFolder2(),KIPTVLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempIPTVLogDes()),BBB,CCC,DDD); }
#define MPX_DEBUG5(AAA,BBB,CCC,DDD,EEE)  { _LIT(tempIPTVLogDes,AAA); RFileLogger::WriteFormat(KIPTVLogFolder2(),KIPTVLogFile2(),EFileLoggingModeAppend,TRefByValue<const TDesC>(tempIPTVLogDes()),BBB,CCC,DDD,EEE); }

#define MPX_FUNC(fn)                TFunctionTrace _s(_L(fn));
#define MPX_FUNC_EX(fn)             TFunctionTrace _s(_L(fn), this);

#endif

#if LOGTYPE == 2

#define MPX_DEBUG1(AAA)
#define MPX_DEBUG2(AAA,BBB)
#define MPX_DEBUG3(AAA,BBB,CCC)
#define MPX_DEBUG4(AAA,BBB,CCC,DDD)
#define MPX_DEBUG5(AAA,BBB,CCC,DDD,EEE)

#define MPX_FUNC(fn)
#define MPX_FUNC_EX(fn)

#endif


class TFunctionTrace
    {
    public:
        TFunctionTrace(TRefByValue<const TDesC> aName, TAny* aThis=NULL):
            iStr(aName), iThis(aThis)
            {
            if ( iThis )
                {
                MPX_DEBUG3("-->%S, 0x%08x",&iStr, iThis);
                }
            else
                {
                MPX_DEBUG2("-->%S",&iStr);
                }
            }
        ~TFunctionTrace()
            {
            if ( iThis )
                {
                MPX_DEBUG3("<--%S, 0x%08x",&iStr, iThis);
                }
            else
                {
                MPX_DEBUG2("<--%S",&iStr);
                }
            }
    private:
        TPtrC iStr;
        TAny* iThis;
    };

// print out (index, id) for each level and the first 15 ids at top level
#define MPX_DEBUG_PATH(aPath) { \
    TInt dlevels = (aPath).Levels();\
    TInt dcount = (aPath).Count();\
    MPX_DEBUG3("Collection Path debug: levels=  %d, count = %d", dlevels, dcount);\
    for (TInt i=0; i<dlevels;++i)\
        {\
        MPX_DEBUG4("Collection Path debug: Level[%d] = (index %d, id 0x%08x)", i, (aPath).Index(i), (aPath).Id(i).iId1);\
        }\
    if (dcount > 0)\
        {\
        TInt index = (aPath).Index();\
        MPX_DEBUG2("Collection Path debug current index %d", index);\
        if (index>=0 && index<dcount)\
            {\
            CMPXCollectionPath* dpath = const_cast<CMPXCollectionPath*>(&aPath);\
            dpath->SetToFirst();\
            TInt ditem(0);\
            do\
                {\
                MPX_DEBUG3("Collection Path debug top level: item %d = id 0x%08x", ditem, dpath->Id().iId1);\
                } while (++(*dpath) && ++ditem < 15 );\
            dpath->Set(index);\
            }\
        else\
            {\
            MPX_DEBUG1("Collection Path debug: Invalid current index");\
            }\
        }\
    }

#define MPX_DEBUG_THREAD(ss) { \
    RProcess process; \
    TFullName fullName; \
    process.FullName( fullName ); \
    RThread thisThread; \
    _LIT(_ss, ss); \
    MPX_DEBUG5("%S: process=%S, thread=%d, this=0x%08x", &_ss, &fullName, (TUint)thisThread.Id(), this); \
    }

#define MPX_S(a) _S(a)
#else
#define MPX_DEBUG1(s)
#define MPX_DEBUG2(s, a)
#define MPX_DEBUG3(s, a, b)
#define MPX_DEBUG4(s, a, b, c)
#define MPX_DEBUG5(s, a, b, c, d)
#define MPX_FUNC(s)
#define MPX_FUNC_EX(fn)
#define MPX_DEBUG_PATH(aPath)
#define MPX_DEBUG_THREAD(ss)
// Copy constant _KMPXErrorInfo and Macros MPX_S and MPX_ERROR_LOG from
// _DEBUG section if intend to log error in release build.
#define MPX_ERROR_LOG(aErr)
#endif

#ifdef MPX_PERFORMANCE_LOG
#define MPX_PERF_CHECKPT(s)     RDebug::Print(_L("--- PERFORMANCE CHECK POINT ---"));\
                                RDebug::Print(_L(#s));
#define MPX_PERF_START(fn) TPerfTrace perf__##fn (_L(#fn)); perf__##fn.Start();
#define MPX_PERF_END(fn) perf__##fn.End();
class TPerfTrace
    {
    public:
        TPerfTrace(TRefByValue<const TDesC> aName):
            iStr(aName)
            {
            }
        void TPerfTrace::Start()
            {
            iTime.UniversalTime();
            MPX_DEBUG2("-->%S",&iStr);
            }
        void TPerfTrace::End()
            {
            TTime now;
            now.UniversalTime();
            TInt delta = I64INT(now.MicroSecondsFrom(iTime).Int64());
            MPX_DEBUG3("<--%S, time taken %d us", &iStr, delta);
            }
    private:
        TPtrC iStr;
        TTime iTime;
    };
#else
#define MPX_PERF_CHECKPT(s)
#define MPX_PERF_START(fn)
#define MPX_PERF_END(fn)
#endif

#define MPX_TRAP(_r, _s) TRAP(_r,_s);MPX_ERROR_LOG(_r);
#define MPX_TRAPD(_r, _s) TRAPD(_r,_s);MPX_ERROR_LOG(_r);

#endif  // MPXLOG_H

// End of File
