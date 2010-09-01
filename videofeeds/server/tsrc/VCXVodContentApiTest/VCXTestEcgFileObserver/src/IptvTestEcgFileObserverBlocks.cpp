/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "IptvTestEcgFileObserver.h"

// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "VCXTestLog.h"
#include "TestUtilConnection.h"
#include "TestUtilConnectionWaiter.h"
#include "IptvTestDownloadManager.h"
#include "VCXTestCommon.h"
#include "CIptvTestMobilecrashWatcher.h"
#include <BADESCA.H>
#include <COMMDB.H>
#include "VCXTestCommon.h"
#include "CIptvTestUtilities.h"

class CIptvTestUtilities;

// CONSTANTS

_LIT( KDateTimeString, "%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%.%*C2%:3%-B" );
_LIT( KServicePathFormat, "C:\\data\\videocenter\\ecg\\%d\\tbn\\" );

/* 5 videos; thumbnails: all; touch TN file: all */ 
_LIT( KIptvTestUrlUpdateAll, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op30=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op31=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op32=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op33=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op24=on&op14=on&op34=on&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text" );

/* 5 videos; thumbnails: all; touch TN file: none */ 
_LIT( KIptvTestUrl1, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op24=on&op14=on&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text" );

/* 5 videos; thumbnails: all; touch TN file: 0, 2, 4 */
_LIT( KIptvTestUrl2, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op30=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op32=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op24=on&op14=on&op34=on&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text" );

/* 5 videos; touch feed TN; thumbnails: all; touch TN file: none */
_LIT( KIptvTestUrl3, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&opx=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op24=on&op14=on&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text" );

/* 4 videos; touch feed TN; thumbnails: all */
_LIT( KIptvTestUrl4, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text" );

/* Videos 0-3, same thumbnails in 0,1 and 2,3 */
_LIT( KIptvTestUrl5, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op30=on&op71=n95_1.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op31=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op32=on&op73=n95_3.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op33=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text" );

/* 3 videos; thumbnails: 1, 2; touch TN file: 1, 2 */ 
_LIT( KIptvTestUrl6, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op30=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op31=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text&XIIT=1" );

/* 3 videos; thumbnails: 1, 3; touch TN file: 1, 2 */
_LIT( KIptvTestUrl7, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op30=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op31=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text&XIIT=1" );

/* Videos 1,2,4,5, thumbnails in all, not touched */
_LIT( KIptvTestUrl8, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op12=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op24=on&op14=on&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text&XIIT=1");

/* Videos 1, 2, thumbnails, 2nd touched */
_LIT( KIptvTestUrl9, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op31=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text&XIIT=1");

/* All videos, thumbnails, last one touched */
_LIT( KIptvTestUrl10, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op24=on&op14=on&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op25=on&op15=on&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op26=on&op16=on&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op27=on&op17=on&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&op28=on&op18=on&op38=on&hiddenfield=Secret+Text");

/* All videos, thumbnails, first one touched */
_LIT( KIptvTestUrl11, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op30=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op24=on&op14=on&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op25=on&op15=on&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op26=on&op16=on&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op27=on&op17=on&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&op28=on&op18=on&hiddenfield=Secret+Text");

/* Videos 0-3, thumbnails */
_LIT( KIptvTestUrl12, "http://193.65.182.78/cgi-bin/nokia_tntest_engine.cgi?op0=on&op70=n95_1.png&op80=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op90=N95&op20=on&op10=on&op71=n95_2.png&op81=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op91=N95&op21=on&op11=on&op72=n95_3.png&op82=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op92=N95&op22=on&op12=on&op73=n93.png&op83=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op93=N9x+and+N80&op23=on&op13=on&op74=n80.png&op84=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op94=N9x+and+N80&op75=n91.png&op85=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op95=N9x+and+N80&op76=n76.png&op86=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op96=N7x&op77=n70.png&op87=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op97=N7x&op78=n72.png&op88=Fri%2C+26+May+2015+14%3A35%3A26+GMT&op98=N7x&hiddenfield=Secret+Text");

// MODULE DATA STRUCTURES

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CIptvTestEcgFileObserver::Delete()
    {
	if(iDownloadManager)
		{
		delete iDownloadManager;
		iDownloadManager = NULL;
		}
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CIptvTestEcgFileObserver::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "SetObservedServicePath", CIptvTestEcgFileObserver::SetObservedServicePathL ),
		ENTRY( "Download", CIptvTestEcgFileObserver::DownloadL ),
		ENTRY( "AddObservedFilename", CIptvTestEcgFileObserver::AddObservedFilenameL ),
		ENTRY( "GetFileDate", CIptvTestEcgFileObserver::GetFileDateL ),
		ENTRY( "CheckFileDate", CIptvTestEcgFileObserver::CheckFileDateL ),
		ENTRY( "CheckFileState", CIptvTestEcgFileObserver::CheckFileState ),
		ENTRY( "SyncTimeWithServer", CIptvTestEcgFileObserver::SyncTimeWithServerL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::DownloadL
// -----------------------------------------------------------------------------
//
TInt CIptvTestEcgFileObserver::DownloadL( CStifItemParser& aItem  )
	{
    VCXLOGLO1(">>>CIptvTestEcgFileObserver::DownloadL");
    // Print to UI
    _LIT( KIptvTestUtilModule, "CIptvTestEcgFileObserver" );
    _LIT( KWhere, "In DownloadL" );
    TestModuleIf().Printf( 0, KIptvTestUtilModule, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt err = KErrNone;

 	aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

 	TPtrC pAddress, pOutFile, pUserName, pPassword, pIapName;

    if( aItem.GetNextString ( pAddress ) != KErrNone )
        {
		VCXLOGLO1("CIptvTestEcgFileObserver::DownloadL: Could not read parameter address!");
		return KErrGeneral;
        }

    HBufC* heapBuffer = HBufC::NewL(4000);
    TPtr ptr( heapBuffer->Des() );

    if( ParseAddress( pAddress ) )
        {
        VCXLOGLO1("CIptvTestEcgFileObserver::DownloadL: Using hard coded address.");
        }
    ptr.Copy( pAddress );

    if( aItem.GetNextString ( pOutFile ) != KErrNone )
        {
		VCXLOGLO1("CIptvTestEcgFileObserver::DownloadL: Could not read parameter outputfile!");
		return KErrGeneral;
        }

    if( aItem.GetNextString ( pUserName ) != KErrNone )
        {
		VCXLOGLO1("CIptvTestEcgFileObserver::DownloadL: no user name specified!");
        }

    if( aItem.GetNextString ( pPassword ) != KErrNone )
        {
		VCXLOGLO1("CIptvTestEcgFileObserver::DownloadL: no password specified!");
        }

    if( aItem.GetNextString ( pIapName ) != KErrNone )
        {
		VCXLOGLO1("CIptvTestEcgFileObserver::DownloadL: no iap specified!");
        }

	VCXLOGLO2("Address: %S", &pAddress);
	VCXLOGLO2("OutFile: %S", &pOutFile);
	VCXLOGLO2("name: %S", &pUserName);
	VCXLOGLO2("Password: %S", &pPassword);
	VCXLOGLO2("Iap: %S", &pIapName);

   if(iDownloadManager)
        {
        delete iDownloadManager;
        iDownloadManager = NULL;
        }
	
	if(!iDownloadManager)
		{
		iDownloadManager = CIptvTestDownloadManager::NewL(this);
		}

    TUint32 iapId(0);

    CVCXTestCommon* vcxCommon;
    vcxCommon = CVCXTestCommon::NewL();
    vcxCommon->GetIapIdL( pIapName, iapId );
    delete vcxCommon;

	err = iDownloadManager->DownloadL(ptr, pOutFile, pUserName, pPassword, iapId);
	if(err != KErrNone)
		{
		VCXLOGLO2("iDownloadManager returned: %d", err);
		}

	VCXLOGLO1("<<<CIptvTestEcgFileObserver::DownloadL");
    return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::ParseAddress
// -----------------------------------------------------------------------------
//
TBool CIptvTestEcgFileObserver::ParseAddress( TPtrC& aAddress )
    {
    TBool found( 0 );

    if( aAddress == _L("KIptvTestUrlUpdateAll") )
        {
        aAddress.Set( KIptvTestUrlUpdateAll );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl1") )
        {
        aAddress.Set( KIptvTestUrl1 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl2") )
        {
        aAddress.Set( KIptvTestUrl2 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl3") )
        {
        aAddress.Set( KIptvTestUrl3 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl4") )
        {
        aAddress.Set( KIptvTestUrl4 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl5") )
        {
        aAddress.Set( KIptvTestUrl5 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl6") )
        {
        aAddress.Set( KIptvTestUrl6 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl7") )
        {
        aAddress.Set( KIptvTestUrl7 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl8") )
        {
        aAddress.Set( KIptvTestUrl8 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl9") )
        {
        aAddress.Set( KIptvTestUrl9 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl10") )
        {
        aAddress.Set( KIptvTestUrl10 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl11") )
        {
        aAddress.Set( KIptvTestUrl11 );
        found = ETrue;
        }
    else if( aAddress == _L("KIptvTestUrl12") )
        {
        aAddress.Set( KIptvTestUrl12 );
        found = ETrue;
        }
    return found;
    }


// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::SetObservedServicePathL
// -----------------------------------------------------------------------------
//
TInt CIptvTestEcgFileObserver::SetObservedServicePathL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvTestEcgFileObserver::SetObservedFilenameL");
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt error( 0 );
    TPtrC serviceName;
    error = aItem.GetNextString( serviceName );
    if( error != KErrNone )
        {
        VCXLOGLO1("CIptvTestEcgFileObserver::SetObservedFilenameL -- No service given!");
        VCXLOGLO1("<<<CIptvTestEcgFileObserver::SetObservedFilenameL");
        return error;
        }

    TInt serviceId( 0 );

    CIptvTestUtilities* util;
    util = CIptvTestUtilities::NewL();
    util->GetServiceIdByNameL( serviceName, serviceId );
    delete util;

    iObservedServicePath = HBufC::NewL(1024);
    iObservedServicePath->Des().Format( KServicePathFormat(), serviceId );
    VCXLOGLO2("CIptvTestEcgFileObserver::SetObservedServicePathL -- path:   %S", iObservedServicePath );
    
    iObservedFiles.ResetAndDestroy();

    VCXLOGLO1("<<<CIptvTestEcgFileObserver::SetObservedFilenameL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::AddObservedFilenameL
// -----------------------------------------------------------------------------
//
TInt CIptvTestEcgFileObserver::AddObservedFilenameL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvTestUtilModule::AddObservedFilenameL");
		/*
    // Print to UI
    _LIT( KIptvTestEcgFileObserver, "IptvTestEcgFileObserver" );
    _LIT( KWhere, "In AddObservedFilenameL" );
    TestModuleIf().Printf( 0, KIptvTestEcgFileObserver, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    */
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    
    TPtrC tempFileName;
    while( aItem.GetNextString( tempFileName ) == KErrNone )
        {
        HBufC* temp = HBufC::NewL( tempFileName.Length() + 1 + iObservedServicePath->Length() + 1);
        CleanupStack::PushL(temp);
        temp->Des().Append( *iObservedServicePath );
        temp->Des().Append( tempFileName );
        VCXLOGLO2("CIptvTestEcgFileObserver::SetObservedFilenameL -- adding: %S", temp );
        iObservedFiles.AppendL( temp );
        CleanupStack::Pop(temp);
        }
    VCXLOGLO1("<<<CIptvTestEcgFileObserver::SetObservedFilenameL");
    return 0;
    }

// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::GetFileDate
// -----------------------------------------------------------------------------
//
TInt CIptvTestEcgFileObserver::GetFileDateL( CStifItemParser& /* aItem */ )
	{
	VCXLOGLO1(">>>CIptvTestEcgFileObserver::GetFileDateL");

    iFileModifiedDates.Reset();

    TInt err( 0 );
    TInt index( 0 );
	for( index = 0; index < iObservedFiles.Count() ; index++ )
	    {
	    HBufC16* fileName = iObservedFiles[index];
	    VCXLOGLO2("CIptvTestEcgFileObserver::GetFileDate -- fetching: %S", fileName);

	    TBuf<256> timeString;
	    RFile file;
	    CleanupClosePushL(file);
	    TInt err = file.Open(iFs, *fileName, EFileRead);
	    TTime temp( 0 );
	    if( err == KErrNone )
	        {
            file.Modified( temp );
            temp.FormatL(timeString, KDateTimeString);
            file.Close();
	        }
	    VCXLOGLO2("CIptvTestEcgFileObserver::GetFileDate -- date: %S", &timeString );
	    CleanupStack::PopAndDestroy();
        iFileModifiedDates.Append( temp );
	    }
	VCXLOGLO2("<<<CIptvTestEcgFileObserver:: returning: %d", err);
	VCXLOGLO1("<<<CIptvTestEcgFileObserver::GetFileDateL");
	return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::CheckFileDateL
// -----------------------------------------------------------------------------
//
TInt CIptvTestEcgFileObserver::CheckFileDateL( CStifItemParser& aItem )
	{
	VCXLOGLO1(">>>CIptvTestEcgFileObserver::CheckFileDateL");

	TInt err( 0 );

	/* Check every observed file for modified date */
    TInt index( 0 );
    TBuf<256> timeSaved;
    TBuf<256> timeCheck;

	for( index = 0; index < iObservedFiles.Count() ; index++ )
	    {
        TBool expectedValue( EFalse ); // Default is that dates are not modified 
        TInt temp;
        if( aItem.GetNextInt( temp ) == KErrNone ) 
            {
            expectedValue = static_cast<TBool>( temp );   
            }
	    
	    HBufC16* fileName = iObservedFiles[index];
        RFile file;
        CleanupClosePushL(file);
        err = file.Open( iFs, *fileName, EFileRead );
        
        if( err == KErrNone )
            {
            VCXLOGLO2("CIptvTestEcgFileObserver::CheckFileDate -- Getting modified date from %S", fileName );
            
            TTime checkModified( 0 );
            User::LeaveIfError( file.Modified( checkModified ) );
            checkModified.FormatL(timeCheck, KDateTimeString);
            
            TTime savedModified( 0 );
            if( iFileModifiedDates.Count() > index )
                {
                savedModified = iFileModifiedDates[index];
                savedModified.FormatL(timeSaved, KDateTimeString);
                }
            VCXLOGLO3("CIptvTestEcgFileObserver::CheckFileDate -- Saved date: %S,  New date:   %S", &timeSaved, &timeCheck );
                 
            // Set the modified value to true if the modified dates don't match
            TBool modifiedValue = (checkModified != savedModified);
            
            VCXLOGLO3("CIptvTestEcgFileObserver::CheckFileDate -- expect: %d, modified: %d", expectedValue, modifiedValue );
            
            // If the expected value doesn't match with the modified value, return error
            if( expectedValue != modifiedValue )
                {
                VCXLOGLO1("CIptvTestEcgFileObserver::CheckFileDate -- value not expected!");
                err = KErrCorrupt; 
                }            
            }
        else
            {
            if( err == KErrNotFound )
                {
                err = KErrNone; // We don't care if the file doesn't exist, check is done by CheckFileState
                }
            else
                {
                VCXLOGLO2("IptvTestEcgFileObserver::CheckFileDate -- error opening file: %d", err );
                }
            }

        CleanupStack::PopAndDestroy(&file);
	    }
	
	VCXLOGLO2("<<<CIptvTestEcgFileObserver:: returning: %d", err);
	VCXLOGLO1("<<<CIptvTestEcgFileObserver::CheckFileDateL");
	return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::CheckFileState
// -----------------------------------------------------------------------------
//
TInt CIptvTestEcgFileObserver::CheckFileState( CStifItemParser& aItem )
	{
    VCXLOGLO1(">>>CIptvTestEcgFileObserver::CheckFileState");

    TInt err( 0 );

    TInt index( 0 );
    for( index = 0; index < iObservedFiles.Count() ; index++ )
        {
        TInt temp = ETrue;
        TBool checkStatus( temp ); // Default: checkStatus is ETrue 
        aItem.GetNextInt( temp );
        checkStatus = static_cast<TBool>( temp );

        HBufC16* fileName = iObservedFiles[index];

    	/* Get the state of the file (exists / not ) */
        TBool exists = BaflUtils::FileExists( iFs, *fileName );
        VCXLOGLO4("CIptvTestEcgFileObserver::CheckFileState -- Checking state of %S, real: %d, expected: %d", fileName, exists, checkStatus );

        if( checkStatus != exists )
        	{
    		VCXLOGLO1("CIptvTestEcgFileObserver::CheckFileState -- File state doesn't match the expected status!");
    		err = KErrCorrupt;
    		break;
        	}
        }
    VCXLOGLO2("<<<CIptvTestEcgFileObserver:: returning: %d", err);
    VCXLOGLO1("<<<CIptvTestEcgFileObserver::CheckFileState");
    return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::SyncTimeWithServerL
// -----------------------------------------------------------------------------
//
TInt CIptvTestEcgFileObserver::SyncTimeWithServerL( CStifItemParser& aItem )
	{
    VCXLOGLO1(">>>CIptvTestUtilModule::SyncTimeWithServerL");
		/*
    // Print to UI
    _LIT( KIptvTestEcgFileObserver, "IptvTestEcgFileObserver" );
    _LIT( KWhere, "In SyncTimeWithServerL" );
    TestModuleIf().Printf( 0, KIptvTestEcgFileObserver, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    */

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    TInt error;
    RFile file;

    TPtrC tempFileName;
    if( aItem.GetNextString( tempFileName ) )
        {
        VCXLOGLO1( "CIptvTestEcgFileObserver::SyncTimeWithServerL -- No filename given!" );
        VCXLOGLO1( "<<<CIptvTestEcgFileObserver::SyncTimeWithServerL" );
        return KErrArgument;
        }
	VCXLOGLO2("CIptvTestEcgFileObserver::SyncTimeWithServerL -- Reading from: %S", &tempFileName );	

	TBuf8<22> textFromFile;
	error = file.Open( iFs, tempFileName, EFileRead );
	VCXLOGLO2( "CIptvTestEcgFileObserver:: file open %d", error );
	
	if( error == KErrNone )
	    {
	    error = file.Read( textFromFile );
	    VCXLOGLO2( "CIptvTestEcgFileObserver:: file read %d", error );
	    file.Close();
	    
	
	    HBufC* monni = HBufC::NewL( textFromFile.Length() );
	    monni->Des().Copy(textFromFile);
	
	    VCXLOGLO2("CIptvTestEcgFileObserver::SyncTimeWithServerL -- String read: %S", monni );	
	
	    TTime newTime( *monni );
	    delete monni;
	
	    TInt err = CVCXTestCommon::SetSystemTime( newTime );
	
	    VCXLOGLO2( "CIptvTestEcgFileObserver:: returning: %d", error );
	    }
	
	VCXLOGLO1( "<<<CIptvTestEcgFileObserver::SyncTimeWithServerL" );
	return error;
	}


// -----------------------------------------------------------------------------
// CIptvTestEcgFileObserver::DownloadFinished
// -----------------------------------------------------------------------------
//
void CIptvTestEcgFileObserver::DownloadFinished(TInt aError)
	{
	VCXLOGLO2("CIptvTestEcgFileObserver:: Download finished: %d", aError);
	Signal(aError);
	}

//  [End of File] - Do not remove
