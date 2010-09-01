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
* Description:    Debug print macros*
*/



// Version : %version: 1 %




#ifndef __MPXVIDEO_DEBUG_H__
#define __MPXVIDEO_DEBUG_H__

// INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <e32des16.h>
#include <e32property.h>
#include <flogger.h>
#include <e32def.h>

#ifdef _DEBUG

class MPXDebug
{
    public:
        inline static void NullLog( TRefByValue<const TDesC16> /*aFmt*/, ... )
        {
        }

        inline static void FileLog( TRefByValue<const TDesC16> aFmt, ... )
        {
            VA_LIST list;
            VA_START(list,aFmt);
            RFileLogger::WriteFormat( _L("Fusion"), 
                                      _L("Fusion.txt"),
                                      EFileLoggingModeAppend,
                                      aFmt,
                                      list );
        }
};

#define FU_DEBUG MPXDebug::FileLog

// MACROS
#define MPX_ENTER_EXIT        TEnterExitLog _s

class TFusionLog : public TDes16Overflow
{
    public:
        
        inline static void FusionLog( TRefByValue<const TDesC16> aFmt, ... )
        {
            TBuf< 512 > buffer;
            
            VA_LIST list;
            VA_START( list, aFmt );
            buffer.AppendFormatList( aFmt, list );
            VA_END(list);
            
            FU_DEBUG(_L("#Fu# %S"), &buffer );
        }
};

class TMpxLog : public TDes16Overflow
{
    public:
        
        inline static void MpxLog( TRefByValue<const TDesC16> aFmt, ... )
        {
            TBuf< 512 > buffer;
            
            VA_LIST list;
            VA_START( list, aFmt );
            buffer.AppendFormatList( aFmt, list );
            VA_END(list);
            
            FU_DEBUG(_L("#Mpx# %S"), &buffer );
        }
};

#define MPX_DEBUG TMpxLog::MpxLog
#define MPX_DEBUG1(s)             MPX_DEBUG(_L(#s))
#define MPX_DEBUG2(s, a)          MPX_DEBUG(_L(#s), a)
#define MPX_DEBUG3(s, a, b)       MPX_DEBUG(_L(#s), a, b)
#define MPX_DEBUG4(s, a, b, c)    MPX_DEBUG(_L(#s), a, b, c)
#define MPX_DEBUG5(s, a, b, c, d) MPX_DEBUG(_L(#s), a, b, c, d)

class TFunctionTrace
    {
    public:
        TFunctionTrace(TRefByValue<const TDesC> aName, TAny* aThis=NULL):
            iStr(aName), iThis(aThis)
            {
            //iThis ? MPX_DEBUG3("-->%S, 0x%08x",&iStr, iThis) : MPX_DEBUG2("-->%S",&iStr);
            iThis ? MPX_DEBUG(_L("-->%S, 0x%08x"),&iStr, iThis) : MPX_DEBUG(_L("-->%S"),&iStr);
            }
        ~TFunctionTrace()
            {
            //iThis ? MPX_DEBUG3("<--%S, 0x%08x",&iStr, iThis) : MPX_DEBUG2("<--%S",&iStr);
            iThis ? MPX_DEBUG(_L("<--%S, 0x%08x"),&iStr, iThis) : MPX_DEBUG(_L("<--%S"),&iStr);
            }
    private:
        TPtrC iStr;
        TAny* iThis;
    };
    
class TEnterExitLog : public TDes16Overflow
{
    public:
        
        void Overflow(TDes16& /*aDes*/)
        {
            FU_DEBUG(_L("%S Logging Overflow"), &iFunctionName);
        }

        TEnterExitLog( TRefByValue<const TDesC> aFunctionName,
                       TRefByValue<const TDesC> aFmt, ... )
        {
            iFunctionName = HBufC::New( TDesC(aFunctionName).Length() );
            
            if ( iFunctionName )
            {
                iFunctionName->Des().Copy(aFunctionName);
            }
            
            TBuf< 512 > buffer;
            
            VA_LIST list;
            VA_START( list, aFmt );
            buffer.AppendFormatList( aFmt, list, this );
            VA_END(list);
            
            FU_DEBUG(_L("#Fu# --> %S %S"), iFunctionName, &buffer );
        }
        
        TEnterExitLog( TRefByValue<const TDesC> aFunctionName )
        {
            iFunctionName = HBufC::New( TDesC(aFunctionName).Length() );
            
            if ( iFunctionName )
            {
                iFunctionName->Des().Copy(aFunctionName);
            }
            
            FU_DEBUG(_L("#Fu# --> %S"), iFunctionName );
        }
        
        ~TEnterExitLog()
        {
            FU_DEBUG(_L("#Fu# <-- %S"), iFunctionName );
            delete iFunctionName;
        }
        
    private:
        HBufC*    iFunctionName;
};

_LIT(_KMPXErrorInfo, "#Fu# MPXVideo Error : error %d file %s line %d");

#define MPX_S(a) _S(a)

#define MPX_ERROR_LOG(aErr) \
    {\
        if (aErr) \
            FU_DEBUG(_KMPXErrorInfo, aErr, MPX_S(__FILE__), __LINE__);\
    }

#define MPX_TRAP(_r, _s) TRAP(_r,_s);MPX_ERROR_LOG(_r);
#define MPX_TRAPD(_r, _s) TRAPD(_r,_s);MPX_ERROR_LOG(_r);

///////////////////////////////////////////////////
#else // UREL VERSION STARTS HERE
///////////////////////////////////////////////////

#define FU_DEBUG 

// MACROS
#define MPX_ENTER_EXIT        

#define MPX_DEBUG 
#define MPX_DEBUG1(s)             
#define MPX_DEBUG2(s, a)          
#define MPX_DEBUG3(s, a, b)       
#define MPX_DEBUG4(s, a, b, c)    
#define MPX_DEBUG5(s, a, b, c, d) 

#define MPX_S(a) 

#define MPX_ERROR_LOG(aErr) 

#define MPX_TRAP(_r, _s) TRAP(_r,_s);
#define MPX_TRAPD(_r, _s) TRAPD(_r,_s); 

#endif // _DEBUG

#endif  // __MPXVIDEO_DEBUG_H__

// End of File
