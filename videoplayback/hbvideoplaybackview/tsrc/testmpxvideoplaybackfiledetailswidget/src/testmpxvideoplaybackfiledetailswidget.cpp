/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   tester for methods in TestMPXVideoPlaybackFileDetailsWidget
* 
*/

// Version : %version:  1 %


#include <qdebug>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>

#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "testmpxvideoplaybackfiledetailswidget.h"

#define private public
#include "mpxvideoplaybackfiledetailswidget.h"
#undef private

const QString KFILEPATH = "C:\\data\\qvptestconf.txt";


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMPXVideoPlaybackFileDetailsWidget tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmpxvideoplaybackfiledetailswidget.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::init()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::init()"));

    mController = new QMPXVideoPlaybackControlsController();   
    mWidget = new QMPXVideoPlaybackFileDetailsWidget( mController );
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::cleanup()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mWidget )
    {
        delete mWidget;
        mWidget = NULL;
    }
}

// ---------------------------------------------------------------------------
// testTitle
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testTitle()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testTitle()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // For Audio only view
    //
    mController->mViewMode = EAudioOnlyView;

    // If title information is available
    details->mTitle = "Title for Test clip";
    mWidget->updateWithFileDetails( details );
    verifyResult( "Title", true, details->mTitle );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    // If title informatio is not available
    details->mTitle = "";
    details->mClipName = KFILEPATH;

    mWidget->updateWithFileDetails( details );
    verifyResult( "Title", true, "qvptestconf" );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // For Details view
    //
    mController->mViewMode = EFullScreenView;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Title", false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDescription
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testDescription()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testDescription()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mDescription = "This is for unit test";
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Description", true, details->mDescription );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDate
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testDuration()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testDuration()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // If it's local mode, need to show the duration
    //
    details->mPlaybackMode = EMPXVideoLiveStreaming;
    details->mDuration = 3700000;
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Duration", "" );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // If it's local mode, need to show the duration
    //
    details->mPlaybackMode = EMPXVideoLocal;
    details->mDuration = 3700000;
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Duration", true, "1 hr 1 min 40 sec " );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDate
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testDate()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testDate()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // If it is streaming case, we don't show size information
    //
    details->mClipName = KFILEPATH;
    details->mPlaybackMode = EMPXVideoStreaming;
    mWidget->updateWithFileDetails( details );

    verifyResult( "Date", false );
    verifyResult( "Modified", false );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // If it is local, we  show size information
    //
    details->mClipName = KFILEPATH;
    details->mPlaybackMode = EMPXVideoLocal;
    mWidget->updateWithFileDetails( details );

    verifyResult( "Date", true, "", false );
    verifyResult( "Modified", true, "", false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testLocation
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testLocation()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testLocation()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mLocation = "C:\\data\\Videos\\";
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Location", true, details->mLocation );

    cleanup();
}

// ---------------------------------------------------------------------------
// testAuthor
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testAuthor()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testAuthor()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mArtist = "Fusion";
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Author", true, details->mArtist );

    cleanup();
}

// ---------------------------------------------------------------------------
// testCopyright
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testCopyright()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testCopyright()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mCopyright = "Fusion team";
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Copyright", true, details->mCopyright );

    cleanup();
}

// ---------------------------------------------------------------------------
// testLanguage
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testLanguage()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testLanguage()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mLanguage = "English";
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Language", true, details->mLanguage );

    cleanup();
}

// ---------------------------------------------------------------------------
// testKeywords
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testKeywords()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testKeywords()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mKeywords = "QTest";
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Keywords", true, details->mKeywords );

    cleanup();
}

// ---------------------------------------------------------------------------
// testSize
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testSize()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testSize()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // If it is streaming case, we don't show size information
    //
    details->mPlaybackMode = EMPXVideoStreaming;
    details->mClipName = KFILEPATH;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Size", false );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // If it is local, we show size information
    //
    details->mPlaybackMode = EMPXVideoLocal;
    mWidget->updateWithFileDetails( details );
    
    verifyResult( "Size", true, "", false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testResolution
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testResolution()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testResolution()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // If it is audio only clip, don't need to show resolution
    //
    details->mVideoEnabled = false;
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Resolution", false );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // If it has video track
    //
    details->mVideoEnabled = true;
    details->mVideoWidth = 640;
    details->mVideoHeight = 360;

    mWidget->updateWithFileDetails( details );

    QString resolution = QString("%1 x %2")
            .arg( details->mVideoWidth ).arg( details->mVideoHeight );

    verifyResult( "Resolution", true, resolution );

    cleanup();
}

// ---------------------------------------------------------------------------
// testFormat
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testFormat()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testFormat()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mMimeType = "Nothing";
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Format", true, details->mMimeType );

    cleanup();
}

// ---------------------------------------------------------------------------
// testBitrate
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testBitrate()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testBitrate()"));


    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mBitRate = 1024;
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Bitrate", true, "1024 Kbps" );

    cleanup();
}

// ---------------------------------------------------------------------------
// testFolder
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::testFolder()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackFileDetailsWidget::testFolder()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // If it is streaming case, we don't show folder information
    //
    details->mPlaybackMode = EMPXVideoStreaming;
    details->mClipName = KFILEPATH;
    
    mWidget->updateWithFileDetails( details );

    verifyResult( "Collection", false );

    mWidget->mListWidget->clear();
    mWidget->mFileDetailsUpdated = false;

    //
    // If it is local case, we show folder information
    //
    details->mPlaybackMode = EMPXVideoLocal;

    mWidget->updateWithFileDetails( details );

    verifyResult( "Collection", true, "", false );

    cleanup();

}

// ---------------------------------------------------------------------------
// verifyResult
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackFileDetailsWidget::verifyResult( 
        QString primaryText, bool exist, QString expectedSecondaryText, bool needToBeCompared )
{
    QString secondaryText = "";
    int i = 0;

    for( ; i < mWidget->mListWidget->count() ; i++ )
    {
        HbListWidgetItem *item = mWidget->mListWidget->item( i );

        if ( item->text() == primaryText )
        {
            secondaryText = item->secondaryText();
            break;
        }
    }

    //
    // If the item needs to be existed in HbListWidget, check the result
    //
    if ( exist )
    {
        if ( needToBeCompared )
        {
            QVERIFY( expectedSecondaryText == secondaryText );
        }
        else
        {
            //
            // Doens't need to compare the secondary text. Just make sure it's in the list
            //
            QVERIFY( i < mWidget->mListWidget->count() );
        }

    }
    //
    // If not, just make sure that item is not in the list
    //
    else
    {
        QVERIFY( i == mWidget->mListWidget->count() );
    }
}

// End of file
