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
* Description:   TestVideoSettingsGroup class implementation
* 
*/

// INCLUDE FILES
#include <QtTest/QtTest>
#include <qdebug.h>
#include <hbapplication.h>
#include <hblineedit.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>
#include <MPSettEngPluginImplementationUIDs.hrh>

#include "testvideosettingsgroup.h"
#include "videosettingsaccesspointentry.h"
#include "mpsettingsmodel.h"
#include "hbdataformmodel.h"
#include "cpitemdatahelper.h"
#include "hbvalidator.h"

// trick to get access to protected/private members.
#define private public
#include "videosettingsgroup.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    TestVideoSettingsGroup tc;
    
    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideosettingsgroup.txt";

    return QTest::qExec(&tc, 3, pass);
}

// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::initTestCase()
{
    mItemHelper = new CpItemDataHelper;
}

// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::cleanupTestCase()
{
    delete mItemHelper;
    mItemHelper = 0;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::init()
{
    CMPSettingsModel::mImplUid = TUid::Null();
    CMPSettingsModel::mConfigMode = -1;
    CMPSettingsModel::mProxyMode = 0;
    CMPSettingsModel::mHostName = QString("testhostname");
    CMPSettingsModel::mProxyPort = 4096;
    CMPSettingsModel::mMinUdpPort = 2048;
    CMPSettingsModel::mMaxUdpPort = 8080;
    CMPSettingsModel::mStoreSettingsCount = 0;
    CMPSettingsModel::mDestructorCount = 0;
    CMPSettingsModel::mApId = 5;
    HbEditorInterface::mFilter = 0;
    
    mTestObject = new VideoSettingsGroup(*mItemHelper);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::cleanup()
{
    disconnect();
    delete mTestObject;
    mTestObject = 0;
}

// ---------------------------------------------------------------------------
// testSettingsConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testSettingsConstruction()
{
    QVERIFY( mTestObject->mSettingsModel != 0 );
    QCOMPARE( mTestObject->mSettingsModel->mConfigMode, (int)EConfigUser);
    QVERIFY( CMPSettingsModel::mImplUid == TUid::Uid(KMPSettEngImplUidROP) );
    
    // verify also the itemShown connection to form here.
    QCOMPARE( mItemHelper->mFormConnection.signal, SIGNAL(itemShown(const QModelIndex)) );
    QVERIFY( mItemHelper->mFormConnection.receiver == mTestObject );
    QCOMPARE( mItemHelper->mFormConnection.method, SLOT(itemShown(const QModelIndex)) );
}

// ---------------------------------------------------------------------------
// testAccessPointConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testAccessPointConstruction()
{
    QVERIFY( mTestObject->mAccessPointItem != 0 );
    QVERIFY( mTestObject->indexOf(static_cast<HbDataFormModelItem*>(mTestObject->mAccessPointItem)) >= 0 );
    QCOMPARE( mTestObject->mAccessPointItem->mText, hbTrId("txt_videos_dblist_access_point") );
    QCOMPARE( mTestObject->mAccessPointItem->mApId, (uint)CMPSettingsModel::mApId );
}

// ---------------------------------------------------------------------------
// testLowestUdpPortConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testLowestUdpPortConstruction()
{
    QVERIFY( mTestObject->mLowestUDPPortItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mLowestUDPPortItem) >= 0 );
    QCOMPARE( mTestObject->mLowestUDPPortItem->contentWidgetData(QString("maxRows")).toInt(), 1);
    QCOMPARE( mTestObject->mLowestUDPPortItem->contentWidgetData(QString("text")).toString(), 
        QString::number(CMPSettingsModel::mMinUdpPort) );
    
    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mLowestUDPPortItem];
    QCOMPARE( connection.signal, SIGNAL(editingFinished()) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(lowestUdpPortEditingFinished()) );
}

// ---------------------------------------------------------------------------
// testHighestUdpPortConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testHighestUdpPortConstruction()
{
    QVERIFY( mTestObject->mHighestUDPPortItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mHighestUDPPortItem) >= 0 );
    QCOMPARE( mTestObject->mHighestUDPPortItem->contentWidgetData(QString("maxRows")).toInt(), 1);
    QCOMPARE( mTestObject->mHighestUDPPortItem->contentWidgetData(QString("text")).toString(), 
        QString::number(CMPSettingsModel::mMaxUdpPort) );
    
    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mHighestUDPPortItem];
    QCOMPARE( connection.signal, SIGNAL(editingFinished()) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(highestUdpPortEditingFinished()) );
}

// ---------------------------------------------------------------------------
// testUseProxyConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testUseProxyConstruction()
{
    QVERIFY( mTestObject->mUseProxyItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mUseProxyItem) >= 0 );
    QCOMPARE( mTestObject->mUseProxyItem->contentWidgetData(QString("checked")).toBool(), 
        CMPSettingsModel::mProxyMode == 1 );
    
    // connection checking in the testItemShown.
}

// ---------------------------------------------------------------------------
// testProxyServerConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testProxyServerConstruction()
{
    QVERIFY( mTestObject->mProxyServerItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mProxyServerItem) >= 0 );
    QCOMPARE( mTestObject->mProxyServerItem->contentWidgetData(QString("maxRows")).toInt(), 1);
    QCOMPARE( mTestObject->mProxyServerItem->contentWidgetData(QString("text")).toString(), 
        CMPSettingsModel::mHostName );
    QCOMPARE( mTestObject->mProxyServerItem->contentWidgetData(QString("placeholderText")).toString(),
        hbTrId("txt_videos_dblist_none") );

    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mProxyServerItem];
    QCOMPARE( connection.signal, SIGNAL(editingFinished()) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(proxyServerEditingFinished()) );
}

// ---------------------------------------------------------------------------
// testProxyPortConstruction
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testProxyPortConstruction()
{
    QVERIFY( mTestObject->mProxyPortItem != 0 );
    QVERIFY( mTestObject->indexOf(mTestObject->mProxyPortItem) >= 0 );
    QCOMPARE( mTestObject->mProxyPortItem->contentWidgetData(QString("maxRows")).toInt(), 1);
    QCOMPARE( mTestObject->mProxyPortItem->contentWidgetData(QString("text")).toString(), 
        QString::number(CMPSettingsModel::mProxyPort) );

    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mProxyPortItem];
    QCOMPARE( connection.signal, SIGNAL(editingFinished()) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(proxyPortEditingFinished()) );
}

// ---------------------------------------------------------------------------
// testDestructor
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testDestructor()
{
    delete mTestObject;
    mTestObject = 0;
    QCOMPARE( CMPSettingsModel::mStoreSettingsCount, 1 );
    QCOMPARE( CMPSettingsModel::mDestructorCount, 1 );
}

// ---------------------------------------------------------------------------
// testDestructor
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testGetAccessPointId()
{
    CMPSettingsModel::mApId = 4;
    
    QCOMPARE( mTestObject->getAccessPointId(), (uint)CMPSettingsModel::mApId );
}

// ---------------------------------------------------------------------------
// testDestructor
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testSetAccessPointId()
{
    CMPSettingsModel::mApId = -1;
    
    uint ap = 3;
    
    mTestObject->setAccessPointId(ap);
    
    QCOMPARE( CMPSettingsModel::mApId, (int)ap );
}

// ---------------------------------------------------------------------------
// testLowestUdpPortEditingFinished
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testLowestUdpPortEditingFinished()
{
    connect(this, SIGNAL(testTextSignal()),
        mTestObject, SLOT(lowestUdpPortEditingFinished()));
    
    QString highText("12345");
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), highText);
    
    QString testString("1234");
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMinUdpPort, testString.toInt() );
    
    CMPSettingsModel::mMinUdpPort = 0;
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), QString("12346"));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMinUdpPort, highText.toInt() );
    
    CMPSettingsModel::mMinUdpPort = 0;
    testString = "12345";
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMinUdpPort, testString.toInt() );    
}

// ---------------------------------------------------------------------------
// testHighestUdpPortEditingFinished
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testHighestUdpPortEditingFinished()
{
    connect(this, SIGNAL(testTextSignal()),
        mTestObject, SLOT(highestUdpPortEditingFinished()));
    
    QString lowText("1234");
    mTestObject->mLowestUDPPortItem->setContentWidgetData(QString("text"), lowText);
    
    QString testString("12345");
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMaxUdpPort, testString.toInt() );
    
    CMPSettingsModel::mMaxUdpPort = 0;
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), QString("1233"));
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMaxUdpPort, lowText.toInt() );
    
    CMPSettingsModel::mMaxUdpPort = 0;
    testString = "1234";
    mTestObject->mHighestUDPPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    
    QCOMPARE( CMPSettingsModel::mMaxUdpPort, testString.toInt() );
}

// ---------------------------------------------------------------------------
// testUseProxyToggled
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testUseProxyToggled()
{
    connect(this, SIGNAL(testIntSignal(int)),
        mTestObject, SLOT(useProxyToggled(int)));
    
    emit testIntSignal(Qt::Unchecked);
    
    QVERIFY( mTestObject->mProxyServerItem->isEnabled() == false );
    QVERIFY( mTestObject->mProxyPortItem->isEnabled() == false );
    QCOMPARE( CMPSettingsModel::mProxyMode, 0);
    
    emit testIntSignal(Qt::Checked);
    
    QVERIFY( mTestObject->mProxyServerItem->isEnabled() );
    QVERIFY( mTestObject->mProxyPortItem->isEnabled() );
    QCOMPARE( CMPSettingsModel::mProxyMode, 1);
}

// ---------------------------------------------------------------------------
// testProxyServerEditingFinished
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testProxyServerEditingFinished()
{
    connect(this, SIGNAL(testTextSignal()),
        mTestObject, SLOT(proxyServerEditingFinished()));
    
    QString testString("testtesttest");
    mTestObject->mProxyServerItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    QCOMPARE( CMPSettingsModel::mHostName, testString );
}

// ---------------------------------------------------------------------------
// testProxyPortEditingFinished
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testProxyPortEditingFinished()
{
    connect(this, SIGNAL(testTextSignal()),
        mTestObject, SLOT(proxyPortEditingFinished()));
    
    QString testString("1234");
    mTestObject->mProxyPortItem->setContentWidgetData(QString("text"), testString);
    
    emit testTextSignal();
    QCOMPARE( CMPSettingsModel::mProxyPort, testString.toInt() );
}

// ---------------------------------------------------------------------------
// testItemShown
// ---------------------------------------------------------------------------
//
void TestVideoSettingsGroup::testItemShown()
{
    connect(this, SIGNAL(testShownSignal(const QModelIndex&)), 
        mTestObject, SLOT(itemShown(const QModelIndex&)));
    
    QModelIndex index;
    
    HbLineEdit* editor = new HbLineEdit();
    mItemHelper->mWidgetReturnValue = editor;
    
    QIntValidator* val;
    
    mItemHelper->mModelItemReturnValue = mTestObject->mLowestUDPPortItem;
    HbEditorInterface::mFilter = 0;
    emit testShownSignal(index);
    QVERIFY( HbEditorInterface::mFilter == HbDigitsOnlyFilter::instance() );
    QCOMPARE( editor->mValidator->mValidators.count(), 1 );
    val = static_cast<QIntValidator*>(editor->mValidator->mValidators.at(0));
    QCOMPARE( val->bottom(), 1024 );
    QCOMPARE( val->top(), 65535 );

    mItemHelper->mModelItemReturnValue = mTestObject->mProxyPortItem;
    HbEditorInterface::mFilter = 0;
    emit testShownSignal(index);
    QVERIFY( HbEditorInterface::mFilter == HbDigitsOnlyFilter::instance() );
    QCOMPARE( editor->mValidator->mValidators.count(), 1 );
    val = static_cast<QIntValidator*>(editor->mValidator->mValidators.at(0));
    QCOMPARE( val->bottom(), 1 );
    QCOMPARE( val->top(), 65535 );
    
    mItemHelper->mModelItemReturnValue = mTestObject->mHighestUDPPortItem;
    HbEditorInterface::mFilter = 0;
    emit testShownSignal(index);
    QVERIFY( HbEditorInterface::mFilter == HbDigitsOnlyFilter::instance() );
    QCOMPARE( editor->mValidator->mValidators.count(), 1 );
    val = static_cast<QIntValidator*>(editor->mValidator->mValidators.at(0));
    QCOMPARE( val->bottom(), 1024 );
    QCOMPARE( val->top(), 65535 );
    
    mItemHelper->mModelItemReturnValue = mTestObject->mProxyServerItem;
    HbEditorInterface::mFilter = 0;
    emit testShownSignal(index);
    QVERIFY( HbEditorInterface::mFilter == HbUrlFilter::instance() );
    
    // verify the use proxy connection:
    ConnectionHolder connection = mItemHelper->mConnections[mTestObject->mUseProxyItem];
    QCOMPARE( connection.signal, SIGNAL(stateChanged(int)) ); 
    QVERIFY( connection.receiver == mTestObject );
    QCOMPARE( connection.method, SLOT(useProxyToggled(int)) );
    
    delete editor;
}

// end of file
