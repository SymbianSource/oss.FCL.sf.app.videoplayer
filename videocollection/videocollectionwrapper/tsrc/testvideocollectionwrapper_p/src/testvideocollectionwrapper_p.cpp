
/**
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
* Description:   tester for methods in VideoCollectionWrapperPrivate
* 
*/


// INCLUDES
#include <qabstractitemmodel.h>
#include <hbapplication.h>
#include "testvideocollectionwrapper_p.h"
#include "videocollectionwrapper.h"
#include "videocollectioncommon.h"
#include "videolistdatamodel.h"
#include "videosortfilterproxymodel.h"
#include "videocollectionclient.h"
#include "vcxmyvideosdefs.h"

#define private public
#include "videocollectionwrapper_p.h"
#undef private

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    TestVideCollectionWrapper_p tv;
    HbApplication app(argc, argv);
    int res;
    if(argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\testvideocollectionwrapper_p.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}


// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::init()
{
    mTestObject = new VideoCollectionWrapperPrivate();
    QVERIFY(mTestObject);
}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::cleanup()
{
    delete mTestObject;  
    mTestObject = 0;
}

// -----------------------------------------------------------------------------
// testGetModelInitFail
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::testGetModelInitFail()
{
    QVERIFY(mTestObject);        
    
    VideoSortFilterProxyModel *proxyGotten = 0;
    int type = -1;
           
    // source model init fails
    VideoListDataModel::mInitFails = true;
    type = VideoCollectionWrapper::EAllVideos;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(!proxyGotten);
    
    VideoListDataModel::mInitFails = false;
    VideoSortFilterProxyModel::setInitFailure(true);
    
    // VideoCollectionWrapper::EAllvideos -> init fails
    type = VideoCollectionWrapper::EAllVideos;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(!proxyGotten);
    
    // VideoCollectionWrapper::ECollections -> init fails
    type = VideoCollectionWrapper::ECollections;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(!proxyGotten);
    
    // VideoCollectionWrapper::EGeneric -> init fails
    type = VideoCollectionWrapper::EGeneric;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(!proxyGotten);
    
    type = VideoCollectionWrapper::ECollectionContent;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(!proxyGotten);
    
    // called with invalid type param
    type = -1;
    VideoListDataModel::mInitFails = false;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(!proxyGotten);
    
}

 
// -----------------------------------------------------------------------------
// testGetModelSucceed
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::testGetModelSucceed()
{
    QVERIFY(mTestObject);        
       
    VideoSortFilterProxyModel *proxyGotten = 0;
    int type = -1;    
    VideoSortFilterProxyModel::setInitFailure(false);
    // VideoCollectionWrapper::EAllvideos
    type = VideoCollectionWrapper::EAllVideos;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(proxyGotten);
    QVERIFY(proxyGotten->mType == type);
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(proxyGotten);
    QVERIFY(proxyGotten->mType == type);
    delete proxyGotten;
    proxyGotten = 0;
   
    // VideoCollectionWrapper::ECollections
    type = VideoCollectionWrapper::ECollections;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(proxyGotten);
    QVERIFY(proxyGotten->mType == type);
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(proxyGotten);
    QVERIFY(proxyGotten->mType == type);
    delete proxyGotten;
    proxyGotten = 0;
   
   // VideoCollectionWrapper::EGeneric
    type = VideoCollectionWrapper::EGeneric;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(proxyGotten);
    QVERIFY(proxyGotten->mType == type);
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(proxyGotten);
    QVERIFY(proxyGotten->mType == type);
    delete proxyGotten;
    proxyGotten = 0;
    
    // VideoCollectionWrapper::ECollectionContent
    type = VideoCollectionWrapper::ECollectionContent;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(proxyGotten);
    QVERIFY(proxyGotten->mType == type);
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(proxyGotten);
    QVERIFY(proxyGotten->mType == type);
    delete proxyGotten;
    proxyGotten = 0;
   
    // called with invalid type param
    type = -1;
    VideoListDataModel::mInitFails = false;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(!proxyGotten);
    
}

// -----------------------------------------------------------------------------
// testAboutToQuitSlot
// -----------------------------------------------------------------------------
//
void TestVideCollectionWrapper_p::testAboutToQuitSlot()
{
    QVERIFY(mTestObject);        
    connect(this, SIGNAL(testSignal()), mTestObject, SLOT(aboutToQuitSlot()));
    
    VideoSortFilterProxyModel *proxyGotten = 0;
    int type = -1;    
    
    VideoSortFilterProxyModel::setInitFailure(false);
    type = VideoCollectionWrapper::EAllVideos;
    proxyGotten = mTestObject->getModel(type);
    QVERIFY(proxyGotten);
    QVERIFY(!mTestObject->mSourceModel.isNull());
    emit testSignal();
    QVERIFY(mTestObject->mSourceModel.isNull());
    
    emit testSignal();
    QVERIFY(mTestObject->mSourceModel.isNull());
    
    
    disconnect(this, SIGNAL(testSignal()), mTestObject, SLOT(aboutToQuitSlot()));
}


// End of file
    


