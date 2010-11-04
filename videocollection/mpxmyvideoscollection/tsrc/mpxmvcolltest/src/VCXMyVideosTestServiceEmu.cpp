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



// INCLUDE FILES
#include "VCXMyVideosTestServiceEmu.h"
#include "VCXTestLog.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================ MEMBER FUNCTIONS ===============================

CVCXMyVideosTestContent* CVCXMyVideosTestContent::NewL( TInt aServiceId, TInt aContentId, const TPtrC& aUrl, const TPtrC& aTitle )
    {
    CVCXMyVideosTestContent* self = new (ELeave) CVCXMyVideosTestContent();
    CleanupStack::PushL(self);
    self->ConstructL( aServiceId, aContentId, aUrl, aTitle );
    CleanupStack::Pop(self);
    return self;
    }

CVCXMyVideosTestContent::CVCXMyVideosTestContent()
    {
    }

CVCXMyVideosTestContent::~CVCXMyVideosTestContent()
    {
    delete iUrl;
    iUrl = NULL;
    delete iTitle;
    iTitle = NULL;
    }

void CVCXMyVideosTestContent::ConstructL( TInt aServiceId, TInt aContentId, const TPtrC& aUrl, const TPtrC& aTitle )
    {
    iServiceId = aServiceId;
    iContentId = aContentId;
    iUrl = aUrl.AllocL();
    iTitle = aTitle.AllocL();
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestServiceEmu::NewL
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestServiceEmu* CVCXMyVideosTestServiceEmu::NewL()
    {
    VCXLOGLO1(">>>CVCXMyVideosTestServiceEmu::NewL");
    CVCXMyVideosTestServiceEmu* self = new (ELeave) CVCXMyVideosTestServiceEmu();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CVCXMyVideosTestServiceEmu::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestServiceEmu::CVCXMyVideosTestServiceEmu
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestServiceEmu::CVCXMyVideosTestServiceEmu( )
    {

    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestServiceEmu::~CVCXMyVideosTestServiceEmu
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestServiceEmu::~CVCXMyVideosTestServiceEmu( )
    {
    iContents.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestServiceEmu::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestServiceEmu::ConstructL()
    {
    VCXLOGLO1(">>>CVCXMyVideosTestServiceEmu::ConstructL");

    AddServiceContentL( KVcxTestServiceName1, 0, _L("rtsp://vishnu.wipsl.com/oops/winter_fun.mp4"), _L("rtsp1") );
	AddServiceContentL( KVcxTestServiceName1, 1, _L("rtsp://vishnu.wipsl.com/oops/oops1_1.mp4"), _L("rtsp2") );
	AddServiceContentL( KVcxTestServiceName1, 2, _L("rtsp://vishnu.wipsl.com/oops/oops1_2.mp4"), _L("rtsp3") );
	AddServiceContentL( KVcxTestServiceName1, 3, _L("rtsp://vishnu.wipsl.com/oops/oops1_3.mp4"), _L("rtsp4") );
	AddServiceContentL( KVcxTestServiceName1, 4, _L("rtsp://vishnu.wipsl.com/oops/oops2_1.mp4"), _L("rtsp5") );
	AddServiceContentL( KVcxTestServiceName1, 5, _L("rtsp://vishnu.wipsl.com/oops/oops2_2.mp4"), _L("rtsp6") );
	AddServiceContentL( KVcxTestServiceName1, 6, _L("rtsp://vishnu.wipsl.com/oops/oops2_3.mp4"), _L("rtsp7") );
	AddServiceContentL( KVcxTestServiceName1, 7, _L("rtsp://vishnu.wipsl.com/oops/speedway1.mp4"), _L("rtsp8") );
	AddServiceContentL( KVcxTestServiceName1, 8, _L("rtsp://vishnu.wipsl.com/oops/speedway1.mp4"), _L("rtsp9") );
	AddServiceContentL( KVcxTestServiceName1, 9, _L("rtsp://vishnu.wipsl.com/oops/speedway1.mp4"), _L("rtsp10") );

    AddServiceContentL( KVcxTestServiceName2, 0, _L("http://193.65.182.78/nokia_vod/bar.mp4"), _L("nokiavod1") );
    AddServiceContentL( KVcxTestServiceName2, 1, _L("http://193.65.182.78/nokia_vod/gallery.mp4"), _L("nokiavod2") );
    AddServiceContentL( KVcxTestServiceName2, 2, _L("http://193.65.182.78/nokia_vod/restaurant.mp4"), _L("nokiavod3") );
    AddServiceContentL( KVcxTestServiceName2, 3, _L("http://193.65.182.78/nokia_vod/waterski.mp4"), _L("nokiavod4") );
    AddServiceContentL( KVcxTestServiceName2, 4, _L("http://193.65.182.78/nokia_vod/wedding.mp4"), _L("nokiavod5") );
    AddServiceContentL( KVcxTestServiceName2, 5, _L("http://193.65.182.78/nokia_vod/videos/Nokia_N93_Sample_Video_Clip_3.mp4"), _L("nokiavod6") );
    AddServiceContentL( KVcxTestServiceName2, 6, _L("http://193.65.182.78/nokia_vod/videos/Nokia_N93_sample_nosound_1.mov"), _L("nokiavod7") );
    AddServiceContentL( KVcxTestServiceName2, 7, _L("http://193.65.182.78/nokia_vod/videos/Nokia_N93_sample_nosound_2.mov"), _L("nokiavod8") );
    AddServiceContentL( KVcxTestServiceName2, 8, _L("http://193.65.182.78/nokia_vod/videos/N80160AVC3gp15f.3gp"), _L("nokiavod9") );

    AddServiceContentL( KVcxTestServiceName3, 0, _L("http://193.65.182.78/videos/enginetesting.mp4"), _L("enginetest0_6megs") );
    AddServiceContentL( KVcxTestServiceName3, 1, _L("http://193.65.182.78/videos/enginetesting2.mp4"), _L("enginetest1_6megs") );
    AddServiceContentL( KVcxTestServiceName3, 2, _L("http://193.65.182.78/videos/enginetesting3.mp4"), _L("enginetest2_6megs") );
    AddServiceContentL( KVcxTestServiceName3, 3, _L("http://193.65.182.78/videos/enginetesting4.mp4"), _L("enginetest3_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 4, _L("http://193.65.182.78/videos/enginetesting5.mp4"), _L("enginetest4_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 5, _L("http://193.65.182.78/videos/enginetesting6.mp4"), _L("enginetest5_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 6, _L("http://193.65.182.78/videos/enginetesting7.mp4"), _L("enginetest6_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 7, _L("http://193.65.182.78/videos/enginetesting8.mp4"), _L("enginetest7_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 8, _L("http://193.65.182.78/videos/enginetesting9.mp4"), _L("enginetest8_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 9, _L("http://193.65.182.78/videos/enginetesting10.mp4"), _L("enginetest9_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 10, _L("http://193.65.182.78/Nseries/video1.3gp"), _L("enginetest10_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 11, _L("http://193.65.182.78/Nseries/video2.3gp"), _L("enginetest11_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 12, _L("http://193.65.182.78/Nseries/video3.3gp"), _L("enginetest12_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 13, _L("http://193.65.182.78/Nseries/video4.3gp"), _L("enginetest13_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 14, _L("http://193.65.182.78/Nseries/video5.3gp"), _L("enginetest14_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 15, _L("http://193.65.182.78/Nseries/video6.3gp"), _L("enginetest15_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 16, _L("http://193.65.182.78/Nseries/video7.3gp"), _L("enginetest16_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 17, _L("http://193.65.182.78/Nseries/video8.3gp"), _L("enginetest17_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 18, _L("http://193.65.182.78/Nseries/video9.3gp"), _L("enginetest18_3megs") );
    AddServiceContentL( KVcxTestServiceName3, 19, _L("http://193.65.182.78/videos/enginetesting.mp4"), _L("enginetest19_sameurlwith0") );
    AddServiceContentL( KVcxTestServiceName3, 20, _L("http://193.65.182.78/videos/Malchik_Gei.3gp"), _L("enginetest20_13megs") );

    AddServiceContentL( KVcxTestServiceName4, 0, _L("http://193.65.182.78/misc/smallvideos/small1.3gp"), _L("small0") );
    AddServiceContentL( KVcxTestServiceName4, 1, _L("http://193.65.182.78/misc/smallvideos/small2.3gp"), _L("small1") );
    AddServiceContentL( KVcxTestServiceName4, 2, _L("http://193.65.182.78/misc/smallvideos/small3.3gp"), _L("small2") );
    AddServiceContentL( KVcxTestServiceName4, 3, _L("http://193.65.182.78/misc/smallvideos/small4.3gp"), _L("small3") );
    AddServiceContentL( KVcxTestServiceName4, 4, _L("http://193.65.182.78/misc/smallvideos/small5.3gp"), _L("small4") );
    AddServiceContentL( KVcxTestServiceName4, 5, _L("http://193.65.182.78/misc/smallvideos/small6.3gp"), _L("small5") );
    AddServiceContentL( KVcxTestServiceName4, 6, _L("http://193.65.182.78/misc/smallvideos/small7.3gp"), _L("small6") );
    AddServiceContentL( KVcxTestServiceName4, 7, _L("http://193.65.182.78/misc/smallvideos/small8.3gp"), _L("small7") );
    AddServiceContentL( KVcxTestServiceName4, 8, _L("http://193.65.182.78/misc/smallvideos/small9.3gp"), _L("small8") );
    AddServiceContentL( KVcxTestServiceName4, 9, _L("http://193.65.182.78/misc/smallvideos/small10.3gp"), _L("smal9l") );
    AddServiceContentL( KVcxTestServiceName4, 10, _L("http://193.65.182.78/misc/smallvideos/small11.3gp"), _L("small10") );
    AddServiceContentL( KVcxTestServiceName4, 11, _L("http://193.65.182.78/misc/smallvideos/small12.3gp"), _L("small11") );
    AddServiceContentL( KVcxTestServiceName4, 12, _L("http://193.65.182.78/misc/smallvideos/small13.3gp"), _L("small12") );
    AddServiceContentL( KVcxTestServiceName4, 13, _L("http://193.65.182.78/misc/smallvideos/small14.3gp"), _L("small13") );
    AddServiceContentL( KVcxTestServiceName4, 14, _L("http://193.65.182.78/misc/smallvideos/small15.3gp"), _L("small14") );
    AddServiceContentL( KVcxTestServiceName4, 15, _L("http://193.65.182.78/misc/smallvideos/small16.3gp"), _L("small15") );
    AddServiceContentL( KVcxTestServiceName4, 16, _L("http://193.65.182.78/misc/smallvideos/small17.3gp"), _L("small16") );
    AddServiceContentL( KVcxTestServiceName4, 17, _L("http://193.65.182.78/misc/smallvideos/small18.3gp"), _L("small17") );
    AddServiceContentL( KVcxTestServiceName4, 18, _L("http://193.65.182.78/misc/smallvideos/small19.3gp"), _L("small18") );
    AddServiceContentL( KVcxTestServiceName4, 19, _L("http://193.65.182.78/misc/smallvideos/small20.3gp"), _L("small19") );
    AddServiceContentL( KVcxTestServiceName4, 19, _L("http://193.65.182.78/misc/smallvideos/small20.3gp"), _L("small19") );

    AddServiceContentL( KVcxTestServiceName5, 0, _L("http://193.65.182.78/videos/misc/TuntematonSotilas_mpeg4_QVGA_768.mp4"), _L("GIGAHUGE") );
    AddServiceContentL( KVcxTestServiceName5, 1, _L("http://193.65.182.78/videos/misc/elephantsdream-480-ipod.mp4"), _L("elephant_83MB") );
    AddServiceContentL( KVcxTestServiceName5, 2, _L("http://193.65.182.78/videos/misc/Tatu-Live_at_Muz-TV_Awards_2005-XviD-TATU.RU.mp4"), _L("tatulive_53MB") );
    AddServiceContentL( KVcxTestServiceName5, 3, _L("http://193.65.182.78/videos/misc/Bob%20Marley%20-%20No%20woman%20no%20cry.mp4"), _L("bob_33MB") );
    AddServiceContentL( KVcxTestServiceName5, 4, _L("http://193.65.182.78/videos/misc/PrettyFU.mp4"), _L("FU_39MB") );
    AddServiceContentL( KVcxTestServiceName5, 5, _L("http://193.65.182.78/videos/misc/Starz.mp4"), _L("starz_29MB") );
    AddServiceContentL( KVcxTestServiceName5, 6, _L("http://193.65.182.78/videos/misc/Prostye%20Dvizinya.mp4"), _L("tatuprosty_26MB") );
    AddServiceContentL( KVcxTestServiceName5, 7, _L("http://193.65.182.78/videos/misc/Taty-Nas_ne_dogonyat.mp4"), _L("tatu_nas_ne_25MB") );
    AddServiceContentL( KVcxTestServiceName5, 8, _L("http://193.65.182.78/videos/misc/All_the_Things_She_Said_Carson_Daily_Live_tatumedia.net.mp4"), _L("tatu_allthings_25MB") );
    AddServiceContentL( KVcxTestServiceName5, 9, _L("http://193.65.182.78/videos/misc/Ya%20Soshla%20S%20Uma.mp4"), _L("tatu_soshla_24MB") );
    AddServiceContentL( KVcxTestServiceName5, 10, _L("http://193.65.182.78/videos/misc/Loves%20Me%20Not%20(Otro%20Rollo).mp4"), _L("loves_me_not_23MB") );
    AddServiceContentL( KVcxTestServiceName5, 11, _L("http://193.65.182.78/videos/misc/Malchik_Gei_(MTV_Poland).mp4"), _L("tatu_malchik_21MB") );
    AddServiceContentL( KVcxTestServiceName5, 12, _L("http://193.65.182.78/videos/misc/Depeche%20Mode%20-%20Suffer%20Well.mp4"), _L("SUFFER_WELL_21MB") );
    AddServiceContentL( KVcxTestServiceName5, 13, _L("http://193.65.182.78/videos/misc/how_soon_is_now.mp4"), _L("how_soon_is_now_WAT_21MB") );
    AddServiceContentL( KVcxTestServiceName5, 14, _L("http://193.65.182.78/videos/misc/Depeche%20Mode%20-%20John%20The%20Revelator.mp4"), _L("John_revelator_19MB") );
    AddServiceContentL( KVcxTestServiceName5, 15, _L("http://193.65.182.78/videos/misc/Depeche%20Mode%20-%20Enjoy%20the%20silence.mp4"), _L("depeche_enjoy_silence_18MB") );
    AddServiceContentL( KVcxTestServiceName5, 16, _L("http://193.65.182.78/videos/misc/PCD_Musica2.mp4"), _L("PCD_18MB") );
    AddServiceContentL( KVcxTestServiceName5, 17, _L("http://193.65.182.78/videos/short/2seconds.mp4"), _L("2seconds") );
    AddServiceContentL( KVcxTestServiceName5, 18, _L("http://193.65.182.78/videos/short/6seconds.mp4"), _L("6seconds") );
    AddServiceContentL( KVcxTestServiceName5, 19, _L("http://193.65.182.78/videos/short/11seconds.mp4"), _L("11seconds") );
    AddServiceContentL( KVcxTestServiceName5, 20, _L("http://193.65.182.78/videos/short/16seconds.mp4"), _L("16seconds") );
    AddServiceContentL( KVcxTestServiceName5, 21, _L("http://193.65.182.78/videos/misc/video_18kb.3gp"), _L("tiny_18KB") );

    VCXLOGLO1("<<<CVCXMyVideosTestServiceEmu::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestServiceEmu::GetServiceIdL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosTestServiceEmu::GetServiceIdL( const TDesC& aServiceName, TInt& aServiceId )
    {

    aServiceId = -1;

    if( aServiceName == KVcxTestServiceName1 )
        {
        aServiceId = 1;
        }

    if( aServiceName == KVcxTestServiceName2 )
        {
        aServiceId = 2;
        }

    if( aServiceName == KVcxTestServiceName3 )
        {
        aServiceId = 3;
        }

    if( aServiceName == KVcxTestServiceName4 )
        {
        aServiceId = 4;
        }

    if( aServiceName == KVcxTestServiceName5 )
        {
        aServiceId = 5;
        }

    TInt ret = KErrNone;

    if( aServiceId == -1 )
        {
        ret = KErrNotFound;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestServiceEmu::GetContentUrlL
// -----------------------------------------------------------------------------
//
const TPtrC CVCXMyVideosTestServiceEmu::GetContentUrlL( TInt aServiceId, TInt aContentId )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestServiceEmu::GetContentUrlL");

    for( TInt i=0; i<iContents.Count(); i++ )
        {
        if( iContents[i]->iServiceId == aServiceId && iContents[i]->iContentId == aContentId )
            {
            return iContents[i]->iUrl->Des();
            }
        }

    User::Leave( KErrNotFound );

    VCXLOGLO1("<<<CVCXMyVideosTestServiceEmu::GetContentUrlL");
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestServiceEmu::GetContentTitleL
// -----------------------------------------------------------------------------
//
const TPtrC CVCXMyVideosTestServiceEmu::GetContentTitleL( TInt aServiceId, TInt aContentId )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestServiceEmu::GetContentTitleL");

    for( TInt i=0; i<iContents.Count(); i++ )
        {
        if( iContents[i]->iServiceId == aServiceId && iContents[i]->iContentId == aContentId )
            {
            return iContents[i]->iTitle->Des();
            }
        }

    User::Leave( KErrNotFound );

    VCXLOGLO1("<<<CVCXMyVideosTestServiceEmu::GetContentTitleL");
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestServiceEmu::AddServiceContentL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestServiceEmu::AddServiceContentL( const TDesC& aServiceName, TInt aContentId, const TDesC& aUrl, const TDesC& aTitle )
    {
    TInt serviceId(0);
    GetServiceIdL( aServiceName, serviceId );
    CVCXMyVideosTestContent* content = CVCXMyVideosTestContent::NewL( serviceId, aContentId, aUrl, aTitle );
    iContents.Append( content );
    }

//  End of File
