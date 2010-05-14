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
* Description:   tester for methods in VideoListSelectionDialog
* 
*/

#include <qdebug.h>
#include <qapplication.h>
#include "hbview.h"
#include "hbabstractitemview.h"
#include "hbdocumentloader.h"
#include "hbstackedwidget.h"
#include "hbcheckbox.h"
#include "hbinputdialog.h"
#include "hbdialog.h"
#include "hbwidget.h"
#include "hblistview.h"
#include "hbmessagebox.h"

#include "testvideolistselectiondialog.h"

#include "videocollectionuiloader.h"
#include "videolistdatamodel.h"
#include "videosortfilterproxymodel.h"
#include "videolistwidget.h"
#include "videocollectionwrapper.h"
#include "videocollectioncommon.h"
#include <vcxmyvideosdefs.h>

#include "videolistdatamodeldata.h"
#include "videocollectionwrapperdata.h"
#include "videocollectionuiloaderdata.h"
#include "videolistwidgetdata.h"
#include "videosortfilterproxymodeldata.h"
#include "videocollectionviewutilsdata.h"

#define private public
#include "videolistselectiondialogtester.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    TestVideoListSelectionDialog tv;

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
        pass[2] = "c:\\data\\testvideolistselectiondialog.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

// ---------------------------------------------------------------------------
// global qHash function required fo creating hash values for TMPXItemId -keys
// ---------------------------------------------------------------------------
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// ---------------------------------------------------------------------------
// setRowCount
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::setRowCount(int count, VideoCollectionCommon::TModelType type, VideoListDataModel *model)
{
    if (!model)
    {
        VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
        VideoSortFilterProxyModel *model = wrapper.getModel(type);
        QVERIFY(model);
        
        VideoListDataModel *sourceModel = qobject_cast<VideoListDataModel*>(model->sourceModel());
        QVERIFY(sourceModel);
        
        VideoListDataModelData::setRowCount(*sourceModel, count);
    }
    else
    {
        VideoListDataModelData::setRowCount(*model, count);
    }
}

// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::initTestCase()
{
    mTestHelper = new VideoListSelectionDialogTesterHelper();
}

// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::cleanupTestCase()
{
    delete mTestHelper;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::init()
{
    mTestObject = 0;
    mModel = 0;
    mTestUiLoader = 0;
    mTestWidget = 0;
    
    mInitOk = false;
    
    mTestUiLoader = new VideoCollectionUiLoader();
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    mModel = VideoCollectionWrapper::instance().getModel(VideoCollectionCommon::EModelTypeGeneric);
    
    mTestHelper->mTestable = mTestObject;
    QVERIFY(mTestHelper->connectSignals());
    
    mInitOk = true;
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::cleanup()
{
    mTestHelper->disconnectSignals();
    
    delete mTestObject; 
    mTestObject = 0;
    
    delete mTestUiLoader;
    mTestUiLoader = 0;  
    
    mSourceModel = 0;
    
    mModel = 0;
    
    
}

// ---------------------------------------------------------------------------
// testConstructDestruct
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testConstructDestruct()
{
    QVERIFY(mInitOk == true);
    
    QVERIFY(mTestObject != 0);
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mModel != 0);

    delete mTestObject;
    mTestObject = 0;
    
    // Getting model fails 
    VideoCollectionWrapperData::mGetModelFails = true;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    QVERIFY(mTestObject->mModel == 0);
    QVERIFY(mTestObject->mListWidget == 0);
    
    delete mTestObject;
    mTestObject = 0;

    // List widget initialize fails
    VideoCollectionWrapperData::mGetModelFails = false;
    VideoListWidgetData::mInitializeReturnValue = -1;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget == 0);
}

// ---------------------------------------------------------------------------
// testSetupContent
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testSetupContent()
{
    VideoCollectionWrapperData::reset();
    VideoListWidgetData::reset();
    
    QVERIFY(mInitOk == true);
    QVERIFY(mModel != 0);
    
    TMPXItemId mpxId;
    
    // invalid type provided, object's internal data will not change
    mTestObject->mTypeOfSelection = VideoListSelectionDialog::EDeleteVideos;
    mTestObject->setupContent(-1, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EDeleteVideos);
    mTestObject->setupContent(600, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EDeleteVideos);
    
    // No data. type EDeleteVideos. Default mpx item 
    setRowCount(0);
    mpxId = TMPXItemId();
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EDeleteVideos);
    QCOMPARE(mTestObject->mSelection.count(), 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 1);
    QVERIFY(mTestObject->mSelectedAlbumId == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mListContainer->mCurrentWidget == mTestObject->mListWidget);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mCheckboxContainer->isVisible() == true);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mPrimaryAction != 0);
    QVERIFY(mTestObject->mSecondaryAction != 0);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterValue);
    
    VideoSortFilterProxyModelData::reset();
    
    // second setup (for coverity)
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EDeleteVideos);
    QCOMPARE(mTestObject->mSelection.count(), 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 1);
    QVERIFY(mTestObject->mSelectedAlbumId == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mListContainer->mCurrentWidget == mTestObject->mListWidget);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mCheckboxContainer->isVisible() == true);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mPrimaryAction != 0);
    QVERIFY(mTestObject->mSecondaryAction != 0);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterValue);
        
    delete mTestObject;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    VideoSortFilterProxyModelData::reset();
    
    // No data. type ESelectCollection. Default mpx item 
    setRowCount(0);
    mpxId = TMPXItemId();
    mTestObject->setupContent(VideoListSelectionDialog::ESelectCollection, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::ESelectCollection);
    QCOMPARE(mTestObject->mSelection.count(), 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 1);
    QVERIFY(mTestObject->mSelectedAlbumId == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mListContainer->mCurrentWidget == mTestObject->mListWidget);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mCheckboxContainer->isVisible() == false);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mPrimaryAction != 0);
    QVERIFY(mTestObject->mSecondaryAction != 0);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterId == mpxId);
    QVERIFY(!VideoSortFilterProxyModelData::mGenericFilterValue);
    
    delete mTestObject;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    VideoSortFilterProxyModelData::reset();
    
    // No data. type EAddToCollection:. Default mpx item 
    setRowCount(0);
    mpxId = TMPXItemId();
    mTestObject->setupContent(VideoListSelectionDialog::EAddToCollection, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EAddToCollection);
    QCOMPARE(mTestObject->mSelection.count(), 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 1);
    QVERIFY(mTestObject->mSelectedAlbumId == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mListContainer->mCurrentWidget == mTestObject->mListWidget);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mCheckboxContainer->isVisible() == true);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mPrimaryAction != 0);
    QVERIFY(mTestObject->mSecondaryAction != 0);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterId == mpxId);
    QVERIFY(!VideoSortFilterProxyModelData::mGenericFilterValue);
    
    delete mTestObject;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    VideoSortFilterProxyModelData::reset();
    
    // No data. type ERemoveFromCollection:. Default mpx item 
    setRowCount(0);
    mpxId = TMPXItemId();
    mTestObject->setupContent(VideoListSelectionDialog::ERemoveFromCollection, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::ERemoveFromCollection);
    QCOMPARE(mTestObject->mSelection.count(), 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 1);
    QVERIFY(mTestObject->mSelectedAlbumId == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mListContainer->mCurrentWidget == mTestObject->mListWidget);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mCheckboxContainer->isVisible() == true);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mPrimaryAction != 0);
    QVERIFY(mTestObject->mSecondaryAction != 0);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterValue);
    
    delete mTestObject;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);    
    VideoSortFilterProxyModelData::reset();
    
    // Some data, mpx item type is album 
    setRowCount(10);
    mpxId = TMPXItemId(0, KVcxMvcMediaTypeAlbum);
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EDeleteVideos);
    QCOMPARE(mTestObject->mSelection.count(), 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 0);
    QVERIFY(mTestObject->mSelectedAlbumId == mpxId);
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mListContainer->mCurrentWidget == mTestObject->mListWidget);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mCheckboxContainer->isVisible() == true);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mPrimaryAction != 0);
    QVERIFY(mTestObject->mSecondaryAction != 0);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterValue);
    
    delete mTestObject;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    VideoSortFilterProxyModelData::reset();
    
    // Some data, mpx item type is invalid: category
    mpxId = TMPXItemId(0, KVcxMvcMediaTypeCategory);
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EDeleteVideos);
    QCOMPARE(mTestObject->mSelection.count(), 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 0);
    QVERIFY(mTestObject->mSelectedAlbumId == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mListContainer->mCurrentWidget == mTestObject->mListWidget);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mCheckboxContainer->isVisible() == true);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mPrimaryAction != 0);
    QVERIFY(mTestObject->mSecondaryAction != 0);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterValue);
    
    delete mTestObject;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    VideoSortFilterProxyModelData::reset();
    
    // Some data, mpx item type is invalid id
    mpxId = TMPXItemId::InvalidId();
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EDeleteVideos);
    QCOMPARE(mTestObject->mSelection.count(), 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 0);
    QVERIFY(mTestObject->mSelectedAlbumId == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mListContainer->mCurrentWidget == mTestObject->mListWidget);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mCheckboxContainer->isVisible() == true);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mPrimaryAction != 0);
    QVERIFY(mTestObject->mSecondaryAction != 0);  
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterValue);
    
    VideoSortFilterProxyModelData::reset();
    // some data, second initialization without widget and model, for coverity
    delete mTestObject->mListWidget;
    mTestObject->mListWidget = 0;
    mTestObject->mModel = 0;
    mTestObject->mListContainer = 0;
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EDeleteVideos);
    QCOMPARE(mTestObject->mSelection.count(), 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 0);
    QVERIFY(mTestObject->mSelectedAlbumId == TMPXItemId::InvalidId());
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mListContainer->mCurrentWidget == mTestObject->mListWidget);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mCheckboxContainer->isVisible() == true);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mPrimaryAction != 0);
    QVERIFY(mTestObject->mSecondaryAction != 0);  
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mGenericFilterValue);
}

// ---------------------------------------------------------------------------
// testSetupInitFailures
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testSetupInitFailures()
{
    TMPXItemId mpxId;
    
    // initDialog fails because model get fails. 
    QVERIFY(mTestObject->mModel != 0);
    VideoSortFilterProxyModel *backup = mTestObject->mModel;
    VideoListWidget *backupWidget = mTestObject->mListWidget;
    mTestObject->mModel = 0;
    mTestObject->mListWidget = 0;
    VideoCollectionWrapperData::mGetModelFails = true;
    mpxId = TMPXItemId(0, KVcxMvcMediaTypeVideo);
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    VideoCollectionWrapperData::mGetModelFails = false;
    QVERIFY(mTestObject->mModel == 0);
    QVERIFY(mTestObject->mListWidget == 0);
    QVERIFY(mTestObject->mListContainer == 0);
    QVERIFY(mTestObject->mHeading == 0);
    QVERIFY(mTestObject->mCheckboxContainer == 0);
    QVERIFY(mTestObject->mItemCount == 0);
    QVERIFY(mTestObject->mCheckBox == 0);
    mTestObject->mModel = backup; backup = 0;
    mTestObject->mListWidget = backupWidget; backupWidget = 0;
    
    delete mTestObject;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    
    // initDialog fails because mListWidget::initialize
    QVERIFY(mTestObject->mModel != 0);
    VideoListWidgetData::mInitializeReturnValue = -1;
    backupWidget = mTestObject->mListWidget;
    mTestObject->mListWidget = 0;
    mpxId = TMPXItemId(0, KVcxMvcMediaTypeVideo);
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget == 0);    
    QVERIFY(mTestObject->mListContainer == 0);
    QVERIFY(mTestObject->mHeading == 0);
    QVERIFY(mTestObject->mCheckboxContainer == 0);
    QVERIFY(mTestObject->mItemCount == 0);
    QVERIFY(mTestObject->mCheckBox == 0);  
    VideoListWidgetData::mInitializeReturnValue = 0;
    
    delete mTestObject;
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    
    // initdialog fails because DOCML_NAME_LIST_CONTAINER finding fails
    VideoCollectionUiLoaderData::mFindFailure = false;
    VideoCollectionUiLoaderData::mFindFailureNameList.clear();
    VideoCollectionUiLoaderData::mFindFailureNameList.append(DOCML_NAME_LIST_CONTAINER);
    mpxId = TMPXItemId(0, KVcxMvcMediaTypeVideo);
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mListWidget != 0);    
    QVERIFY(mTestObject->mListContainer == 0);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mCheckboxContainer != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);     
    VideoCollectionUiLoaderData::mFindFailureNameList.clear();
}

// ---------------------------------------------------------------------------
// testExec
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testExec()
{   
    // there's not much to test in exec -method, basically these
    // are for coberity only
    // we're using EDeleteVideos type and make sure correct
    // status exists in view utils after exec 
    TMPXItemId mpxId = TMPXItemId(0, KVcxMvcMediaTypeVideo);
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    
    HbDialog::execReturnPrimary = true;
    
    VideoCollectionViewUtilsData::mLastError = -1;
    
    // empty model row count
    setRowCount(0);
    mTestObject->exec();
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusDeleteInProgress);
    VideoCollectionViewUtilsData::mLastError = -1;
    
    // model contains data
    setRowCount(10);
    mTestObject->exec();
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusDeleteInProgress);
    
    // just to make sure finishedSlot is called with correct params
    VideoCollectionViewUtilsData::mLastError = -1;
    HbDialog::execReturnPrimary = false;
    mTestObject->exec();
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
}

// ---------------------------------------------------------------------------
// testFinishedSlot
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testFinishedSlot()
{
    // finished slot tested throught exec -method
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    
    TMPXItemId mpxId = TMPXItemId(1, KVcxMvcMediaTypeAlbum);
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    // finished with secondary action
    VideoCollectionViewUtilsData::mLastError = -1;
    HbDialog::execReturnPrimary = false;
    VideoSortFilterProxyModelData::reset();
    mTestObject->exec();
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    QVERIFY(!VideoSortFilterProxyModelData::mItemIds.count());

    HbDialog::execReturnPrimary = true;    
    
    ////////////
    // mTypeOfSelection == ESelectCollection     
    //  mSelectedAlbumId != TMPXItemId::InvalidId())    
    VideoCollectionViewUtilsData::mLastError = -1;    
    mTestObject->setupContent(VideoListSelectionDialog::ESelectCollection, mpxId);
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    VideoSortFilterProxyModelData::mAddItemsInAlbumReturnValue = 0;
    mTestObject->exec();
    // type of selection has changed
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EAddToCollection);
    // videos added into collection
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusVideosAddedToCollection);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.count() == 2);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(0) == TMPXItemId(1,0));
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(1) == TMPXItemId(2,0));
    
    HbInputDialog *dialog = new HbInputDialog();
    
    VideoCollectionViewUtilsData::mLastError = -1;
    //  mSelectedAlbumId == TMPXItemId::InvalidId())
    mpxId = TMPXItemId::InvalidId();
    // query for new album name does not set selected
    mTestObject->setupContent(VideoListSelectionDialog::ESelectCollection, mpxId);
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    VideoSortFilterProxyModelData::mNewAlbumId = TMPXItemId::InvalidId();
    mTestObject->exec();
    dialog->emitDialogFinished(mTestObject, SLOT(newAlbumNameDialogFinished(HbAction *)), 1); // No selected.
    // type of selection does not change
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EAddToCollection);
    // since there's no selected videos, status code does not change
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    QVERIFY(!VideoSortFilterProxyModelData::mItemIds.count());
 
    // query for new album name sets selected
    mTestObject->setupContent(VideoListSelectionDialog::ESelectCollection, mpxId);
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    VideoSortFilterProxyModelData::mNewAlbumId = TMPXItemId(1,2);
    HbInputDialog::mValueReturnValue = QVariant(QString("testname"));
    mTestObject->exec();  
    dialog->emitDialogFinished(mTestObject, SLOT(newAlbumNameDialogFinished(HbAction *)), 0); // Yes selected.
    // type of selection has changed
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EAddToCollection);
    // videos added into collection
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusVideosAddedToCollection);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == VideoSortFilterProxyModelData::mNewAlbumId);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.count() == 2);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(0) == TMPXItemId(1,0));
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(1) == TMPXItemId(2,0));
    
    ////////////
    // mTypeOfSelection == EAddToCollection
    VideoCollectionViewUtilsData::mLastError = -1;
    // selected album id == invalid
    mTestObject->setupContent(VideoListSelectionDialog::EAddToCollection, TMPXItemId::InvalidId());
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    QVERIFY(!VideoSortFilterProxyModelData::mItemIds.count());
      
    // no selected videos
    mpxId = TMPXItemId(1, KVcxMvcMediaTypeAlbum);
    mTestObject->setupContent(VideoListSelectionDialog::EAddToCollection, mpxId);
    VideoSortFilterProxyModelData::reset();
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    QVERIFY(!VideoSortFilterProxyModelData::mItemIds.count());
    
    // selected album exist, selected videos exists, add items fails    
    mTestObject->setupContent(VideoListSelectionDialog::EAddToCollection, mpxId);
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    VideoSortFilterProxyModelData::mAddItemsInAlbumReturnValue = -1;
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    QVERIFY(!VideoSortFilterProxyModelData::mItemIds.count());
    
    // selected album exist, selected videos exists, add items succeeds   
    mTestObject->setupContent(VideoListSelectionDialog::EAddToCollection, mpxId);
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    VideoSortFilterProxyModelData::mAddItemsInAlbumReturnValue = 0;
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusVideosAddedToCollection);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.count() == 2);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(0) == TMPXItemId(1,0));
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(1) == TMPXItemId(2,0));
    
    VideoCollectionViewUtilsData::mLastError = -1;
    // selected album exist, selected videos exists, name for selected album exist
    // (using ESelectCollection type to fetch albumname)
    mTestObject->setupContent(VideoListSelectionDialog::ESelectCollection, mpxId);
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    VideoSortFilterProxyModelData::mNewAlbumId = TMPXItemId(1,2);
    mTestObject->exec();
    dialog->emitDialogFinished(mTestObject, SLOT(newAlbumNameDialogFinished(HbAction *)), 0); // Yes selected.
    // type of selection has changed
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EAddToCollection);
    // videos added into collection
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusVideosAddedToCollection);
    
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.count() == 2);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(0) == TMPXItemId(1,0));
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(1) == TMPXItemId(2,0));
    
    /////
    // mTypeOfSelection == ERemoveFromCollection
    VideoCollectionViewUtilsData::mLastError = -1;
    // selected album id == invalid
    mTestObject->setupContent(VideoListSelectionDialog::ERemoveFromCollection, TMPXItemId::InvalidId());
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::ERemoveFromCollection);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    QVERIFY(!VideoSortFilterProxyModelData::mItemIds.count());
      
    // no selected videos
    mpxId = TMPXItemId(1, KVcxMvcMediaTypeAlbum);
    mTestObject->setupContent(VideoListSelectionDialog::ERemoveFromCollection, mpxId);
    VideoSortFilterProxyModelData::reset();
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::ERemoveFromCollection);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    QVERIFY(!VideoSortFilterProxyModelData::mItemIds.count());
    
    // selected videos exists, removeitemsFromAlbum fails   
    mTestObject->setupContent(VideoListSelectionDialog::ERemoveFromCollection, mpxId);
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    VideoSortFilterProxyModelData::mRemoveItemsFromAlbumReturnValue = -1;
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::ERemoveFromCollection);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == TMPXItemId::InvalidId());
    QVERIFY(!VideoSortFilterProxyModelData::mItemIds.count());
       
    mTestObject->setupContent(VideoListSelectionDialog::ERemoveFromCollection, mpxId);
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    VideoSortFilterProxyModelData::mRemoveItemsFromAlbumReturnValue = 0;
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::ERemoveFromCollection);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusVideosRemovedFromCollection);
    QVERIFY(VideoSortFilterProxyModelData::mLastItemId == mpxId);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.count() == 2);
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(0) == TMPXItemId(1,0));
    QVERIFY(VideoSortFilterProxyModelData::mItemIds.at(1) == TMPXItemId(2,0));
    
    /////
    // mTypeOfSelection == EDeleteVideos
    VideoCollectionViewUtilsData::mLastError = -1;
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == VideoListSelectionDialog::EDeleteVideos);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusDeleteInProgress);
    
    // invalid type
    mTestObject->mTypeOfSelection = 500;
    VideoCollectionViewUtilsData::mLastError = -1;
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    VideoSortFilterProxyModelData::reset();
    mTestObject->exec();  
    QVERIFY(mTestObject->mTypeOfSelection == 500);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == -1);
    QVERIFY(!VideoSortFilterProxyModelData::mItemIds.count());
   
}

void TestVideoListSelectionDialog::testMarkAllStateChangedSlot()
{
    VideoCollectionWrapperData::reset();
    VideoListWidgetData::reset();
    VideoSortFilterProxyModelData::reset();

    mTestHelper->mTestable = mTestObject;
    mTestHelper->connectSignals();
    
    HbListView::mAllSelectedStatus = -1;
    
    // forced check
    mTestObject->mForcedCheck = true;
    mTestHelper->emitMarkAllStateChanged(Qt::Checked);
    mTestObject->mForcedCheck = false;
    QVERIFY(HbListView::mAllSelectedStatus == -1);
    
    // checked
    mTestHelper->emitMarkAllStateChanged(Qt::Checked);
    QVERIFY(HbListView::mAllSelectedStatus == 1);
    HbListView::mAllSelectedStatus = -1;
    
    // unchecked
    mTestHelper->emitMarkAllStateChanged(Qt::Unchecked);
    QVERIFY(HbListView::mAllSelectedStatus == 0);
    
    HbListView::mAllSelectedStatus = -1;
    
    mTestHelper->disconnectSignals();

}

void TestVideoListSelectionDialog::testSelectionChangedSlot()
{
    QItemSelection selected;
    QItemSelection deselected;
    
    mTestHelper->mTestable = mTestObject;
    mTestHelper->connectSignals();
    
    mTestObject->mSelection.clear();
    
    // no primary action
    mTestHelper->emitSelectionChangedSlot(selected, deselected);
    QVERIFY(!mTestObject->mSelection.count());
    
    TMPXItemId mpxId = TMPXItemId::InvalidId();
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, TMPXItemId(1,1));

    setRowCount(10);
    
    // select item
    mTestObject->mPrimaryAction->setEnabled(false);
    QModelIndex index = mModel->index(0, 0);
    QItemSelectionRange range(mModel->index(0, 0));
    selected.append(range);
    mTestHelper->emitSelectionChangedSlot(selected, deselected);
    QVERIFY(mTestObject->mSelection.count() == 1);
    QVERIFY(mTestObject->mPrimaryAction->isEnabled() == true);
    
    // deselect item
    selected.clear();
    deselected.append(range);
    mTestHelper->emitSelectionChangedSlot(selected, deselected);
    QVERIFY(mTestObject->mSelection.count() == 0);
    QVERIFY(mTestObject->mPrimaryAction->isEnabled() == false);
    
    mTestHelper->disconnectSignals();

}

void TestVideoListSelectionDialog::testSingleItemSelectedSlot()
{
    mTestHelper->mTestable = mTestObject;
    mTestHelper->connectSignals();
    
    setRowCount(10);
    TMPXItemId mpxId = TMPXItemId::InvalidId();
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    
    mTestObject->mTypeOfSelection = VideoListSelectionDialog::ESelectCollection;
    mTestHelper->emitSingleItemSelectedSlot(mModel->index(0, 0));
    
    mTestObject->mTypeOfSelection = VideoListSelectionDialog::ESelectCollection;
    QModelIndex invalidIndex;
    mTestHelper->emitSingleItemSelectedSlot(invalidIndex);
    
    mTestObject->mTypeOfSelection = VideoListSelectionDialog::EDeleteVideos;
    mTestHelper->emitSingleItemSelectedSlot(mModel->index(0, 0));
    
    mTestHelper->disconnectSignals();
}

void TestVideoListSelectionDialog::testModelReadySlot()
{

    mTestHelper->mTestable = mTestObject;
    mTestHelper->connectSignals();
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, TMPXItemId(1,2));
    
    mTestObject->mPrimaryAction->mTriggeredCount = 0;
    
    // type of selection != ESelectCollection    
    mTestHelper->emitModelReadySlot();
    QVERIFY(mTestObject->mPrimaryAction->mTriggeredCount == 0);    
    
    mTestObject->setupContent(VideoListSelectionDialog::ESelectCollection, TMPXItemId(1,2));
    // no items in model
    setRowCount(0);
    mTestHelper->emitModelReadySlot();
    QVERIFY(mTestObject->mPrimaryAction->mTriggeredCount == 1);    
    
    mTestObject->mPrimaryAction->mTriggeredCount = 0;
    setRowCount(10);
    // type of selection is ESelectCollection and there are items in model
    mTestHelper->emitModelReadySlot();
    QVERIFY(mTestObject->mPrimaryAction->mTriggeredCount == 0);   
    
    mTestHelper->disconnectSignals();
}

void TestVideoListSelectionDialog::testUpdateCounterSlot()
{
    mTestHelper->mTestable = mTestObject;
    mTestHelper->connectSignals();
    
    TMPXItemId mpxId = TMPXItemId::InvalidId();
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    
    // itemCount is null
    HbLabel *labelBackup = mTestObject->mItemCount;
    mTestObject->mCheckBox->setChecked(true);
    mTestObject->mItemCount = 0;
    mTestHelper->emitUpdateCounterSlot();
    mTestObject->mItemCount = labelBackup;
    QVERIFY(mTestObject->mCheckBox->mChecked == true);
    
    // no items in model
    setRowCount(0);
    mTestObject->mCheckBox->setChecked(true);
    mTestHelper->emitUpdateCounterSlot();
    QVERIFY(mTestObject->mCheckBox->mChecked == false);

    // all selected.
    setRowCount(1);
    mTestObject->mCheckBox->setChecked(false);
    QItemSelection selected;
    QItemSelectionRange range(mTestObject->mModel->index(0, 0));
    selected.append(range);
    mTestObject->mSelection.merge(selected, QItemSelectionModel::Select);
    mTestHelper->emitUpdateCounterSlot();
    QVERIFY(mTestObject->mCheckBox->mChecked == true);
    
    // not all selected
    setRowCount(10);
    mTestObject->mCheckBox->setChecked(true);
    selected.clear();
    range = QItemSelectionRange(mModel->index(0, 0));
    selected.append(range);
    mTestObject->mSelection.clear();
    mTestObject->mSelection.merge(selected, QItemSelectionModel::Select);
    mTestHelper->emitUpdateCounterSlot();
    QVERIFY(mTestObject->mCheckBox->mChecked == false);
    
    mTestHelper->disconnectSignals();
}

void TestVideoListSelectionDialog::testPrimaryActionTriggeredSlot()
{
    mTestHelper->mTestable = mTestObject;
    mTestHelper->connectSignals();
    
    TMPXItemId mpxId = TMPXItemId::InvalidId();
    mTestObject->setupContent(VideoListSelectionDialog::EDeleteVideos, mpxId);
    
    // nothing selected, album id invalid, selection type != EAddToCollection
    mTestObject->mPrimaryAction->mTriggeredCount = 0;
    mTestObject->mSecondaryAction->mTriggeredCount = 0;
    mTestObject->mTypeOfSelection = VideoListSelectionDialog::EDeleteVideos;
    mTestHelper->emitPrimaryActionTriggeredSlot();
    QCOMPARE(mTestObject->mPrimaryAction->mTriggeredCount, 0);
    QCOMPARE(mTestObject->mSecondaryAction->mTriggeredCount, 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 0);
    
    // nothing selected, album id invalid, selection type EAddToCollection
    mTestObject->mSelectedAlbumId = TMPXItemId::InvalidId();
    mTestObject->mPrimaryAction->mTriggeredCount = 0;
    mTestObject->mSecondaryAction->mTriggeredCount = 0;
    mTestObject->mTypeOfSelection = VideoListSelectionDialog::EAddToCollection;
    mTestHelper->emitPrimaryActionTriggeredSlot();
    QCOMPARE(mTestObject->mPrimaryAction->mTriggeredCount, 0);
    QCOMPARE(mTestObject->mSecondaryAction->mTriggeredCount, 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 0);
    
    // nothing selected, album id invalid, selection type ESelectCollection
    mTestObject->mSelectedAlbumId = TMPXItemId::InvalidId();
    mTestObject->mPrimaryAction->mTriggeredCount = 0;
    mTestObject->mSecondaryAction->mTriggeredCount = 0;
    mTestObject->mTypeOfSelection = VideoListSelectionDialog::ESelectCollection;
    mTestHelper->emitPrimaryActionTriggeredSlot();
    QCOMPARE(mTestObject->mPrimaryAction->mTriggeredCount, 0);
    QCOMPARE(mTestObject->mSecondaryAction->mTriggeredCount, 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 0);
    
    // nothing selected, album id invalid, selection type invalid
    mTestObject->mSelectedAlbumId = TMPXItemId::InvalidId();
    mTestObject->mPrimaryAction->mTriggeredCount = 0;
    mTestObject->mSecondaryAction->mTriggeredCount = 0;
    mTestObject->mTypeOfSelection = 555;
    mTestHelper->emitPrimaryActionTriggeredSlot();
    QCOMPARE(mTestObject->mPrimaryAction->mTriggeredCount, 0);
    QCOMPARE(mTestObject->mSecondaryAction->mTriggeredCount, 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 0);

    // nothing selected, album id ok
    mTestObject->mSelectedAlbumId = TMPXItemId(0, KVcxMvcMediaTypeAlbum);
    mTestObject->mPrimaryAction->mTriggeredCount = 0;
    mTestObject->mSecondaryAction->mTriggeredCount = 0;
    mTestObject->mTypeOfSelection = VideoListSelectionDialog::ESelectCollection;
    mTestHelper->emitPrimaryActionTriggeredSlot();
    QCOMPARE(mTestObject->mPrimaryAction->mTriggeredCount, 1);
    QCOMPARE(mTestObject->mSecondaryAction->mTriggeredCount, 0);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 0);

    setRowCount(10);
    VideoSortFilterProxyModelData::mItemIds.append(TMPXItemId(0,0));
    VideoSortFilterProxyModelData::mItemIds.append(TMPXItemId(1,2));
    
    // videos selected, other one's type is not video
    QItemSelection selected;
    QItemSelectionRange range(mTestObject->mModel->index(0, 0), mTestObject->mModel->index(1, 0));
    selected.append(range);
    mTestObject->mSelection.merge(selected, QItemSelectionModel::Select);
    mTestObject->mPrimaryAction->mTriggeredCount = 0;
    mTestObject->mSecondaryAction->mTriggeredCount = 0;
    mTestObject->mTypeOfSelection = VideoListSelectionDialog::EAddToCollection;
    mTestObject->mSelectedVideos.clear();
    mTestHelper->emitPrimaryActionTriggeredSlot();
    QCOMPARE(mTestObject->mPrimaryAction->mTriggeredCount, 1);
    QCOMPARE(mTestObject->mSelectedVideos.count(), 1);
  
    mTestHelper->disconnectSignals(); 
}

void TestVideoListSelectionDialog::testGetSelectedName()
{
    mTestHelper->mTestable = mTestObject;
    mTestHelper->connectSignals();
    
    mTestObject->setupContent(VideoListSelectionDialog::ERemoveFromCollection, TMPXItemId::InvalidId());
    
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,0));
    mTestObject->mSelectedVideos.insert(TMPXItemId(2,0));
    
    VideoCollectionViewUtilsData::mLastStatusAdditional = QVariant();
    // mSelectedAlbumId is video
    mTestObject->mSelectedAlbumId = TMPXItemId(1,0);
    mTestHelper->emitFinishedSlot(mTestObject->mPrimaryAction);
    QVERIFY(VideoCollectionViewUtilsData::mLastStatusAdditional.isValid());
    QVERIFY(!VideoCollectionViewUtilsData::mLastStatusAdditional.toString().length());
    
    mTestObject->mSelectedAlbumId = TMPXItemId(1,2);
    // fetching collections model unsucceeds
    VideoCollectionWrapperData::mGetModelFails = true;
    mTestHelper->emitFinishedSlot(mTestObject->mPrimaryAction);
    QVERIFY(VideoCollectionViewUtilsData::mLastStatusAdditional.isValid());
    QVERIFY(!VideoCollectionViewUtilsData::mLastStatusAdditional.toString().length());
    VideoCollectionWrapperData::mGetModelFails = false;
    
    // invalid index for selected album
    mTestHelper->emitFinishedSlot(mTestObject->mPrimaryAction);
    QVERIFY(VideoCollectionViewUtilsData::mLastStatusAdditional.isValid());
    QVERIFY(!VideoCollectionViewUtilsData::mLastStatusAdditional.toString().length());
    
    // succeed case:
    // need to add album ib into selected list, because stub proxy overwrites
    // VideoSortFilterProxyModelData::mItemIds at removeitems removeItemsFromAlbum -method
    // and we need valid index for id
    QString testname("testname");
    VideoListDataModelData::mData[VideoCollectionCommon::KeyTitle] = testname;
    mTestObject->mSelectedVideos.insert(TMPXItemId(1,2));   
    mTestHelper->emitFinishedSlot(mTestObject->mPrimaryAction);
    QVERIFY(VideoCollectionViewUtilsData::mLastStatusAdditional.isValid());
    QVERIFY(VideoCollectionViewUtilsData::mLastStatusAdditional.toString().length() > 0);
    QVERIFY(VideoCollectionViewUtilsData::mLastStatusAdditional.toString() == "testname");
    
    mTestHelper->disconnectSignals(); 
}

// end of file
