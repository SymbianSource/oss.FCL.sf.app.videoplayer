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
#include "testvideolistselectiondialog.h"
#include <qapplication.h>
#include "hbaction.h"
#include "videocollectionuiloader.h"
#include "videolistwidget.h"
#include "dummydatamodel.h"
#include "videosortfilterproxymodel.h"

#define private public
#include "videolistselectiondialog.h"
#undef private

const int DEFAULT_ROW_COUNT = 10;

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    TestVideoListSelectionDialog tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideolistselectiondialog.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::init()
{
    mTestUiLoader = new VideoCollectionUiLoader();
    mTestObject = new VideoListSelectionDialog(mTestUiLoader);
    
    mSourceModel = new DummyDataModel();
    mSourceModel->setRowCount(DEFAULT_ROW_COUNT);
    
    mModel = new VideoSortFilterProxyModel();
    mModel->setSourceModel(mSourceModel);
    
    mTestWidget = new VideoListWidget(*mModel);
    
}
 
// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::cleanup()
{
    delete mTestObject; 
    mTestObject = 0;
    
    delete mTestUiLoader;
    mTestUiLoader = 0;  
    
    delete mTestWidget;
    mTestWidget = 0;
    
    delete mSourceModel;
    mSourceModel = 0;
    
    delete mModel;
    mModel = 0;
}

// ---------------------------------------------------------------------------
// testSetContentEmptyTitle
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testSetContentEmptyTitle()
{
    QVERIFY(mTestObject != 0);
    
    QString title = "";
    mTestObject->setContent(title, mTestWidget);
    // at first invalid setContent, all 
    // members remain null
    QVERIFY(mTestObject->mVideoList == 0);
    QVERIFY(mTestObject->mHeading == 0);
    QVERIFY(mTestObject->mItemCount == 0);
    QVERIFY(mTestObject->mCheckBox == 0);
    QVERIFY(mTestObject->mListContainer == 0);
    
    title = "test"; 
    mTestObject->setContent(title, mTestWidget);
    // succeed setContent. All members are initialized
    QVERIFY(mTestObject->mVideoList != 0);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    // list container gets initialized at exec
    QVERIFY(mTestObject->mListContainer == 0);
    
    title = ""; 
    mTestObject->setContent(title, mTestWidget);
    // invalid setContent after one or more succeed one:
    // videolist is nul indicating invalid state, whereas
    // UI components have been fetched from ui loader
    QVERIFY(mTestObject->mVideoList == 0);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    // list container gets initialized at exec
    QVERIFY(mTestObject->mListContainer == 0);
}
    
// ---------------------------------------------------------------------------
// testSetContentNullcontent
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testSetContentNullcontent()
{
    QVERIFY(mTestObject != 0);
        
    QString title = "test";
    mTestObject->setContent(title, 0);
    // at first invalid setContent, all 
    // members remain null
    QVERIFY(mTestObject->mVideoList == 0);
    QVERIFY(mTestObject->mHeading == 0);
    QVERIFY(mTestObject->mItemCount == 0);
    QVERIFY(mTestObject->mCheckBox == 0);
    QVERIFY(mTestObject->mListContainer == 0);
    
    mTestObject->setContent(title, mTestWidget);
    // succeed setContent. All members are initialized
    QVERIFY(mTestObject->mVideoList != 0);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    // list container gets initialized at exec
    QVERIFY(mTestObject->mListContainer == 0);
    
    title = ""; 
    mTestObject->setContent(title, 0);
    // invalid setContent after one or more succeed one:
    // videolist is nul indicating invalid state, whereas
    // UI components have been fetched from ui loader
    QVERIFY(mTestObject->mVideoList == 0);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    // list container gets initialized at exec
    QVERIFY(mTestObject->mListContainer == 0);
}
    
// ---------------------------------------------------------------------------
// tesSetContentItemFinding
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::tesSetContentItemFinding()
{
    // all items are not yet created -> they are to be created
    QString title = "test";
    mTestObject->setContent(title, mTestWidget);
    QVERIFY(mTestObject->mVideoList != 0);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    // list container gets initialized at exec
    QVERIFY(mTestObject->mListContainer == 0);
    QVERIFY(mTestObject->primaryAction());
    QVERIFY(mTestObject->secondaryAction());
    QVERIFY(mTestObject->mHeading->mPlainTxt == title); 
    QVERIFY(mTestObject->mItemCount->mAlignment == Qt::AlignRight);
    QVERIFY(mTestObject->mItemCount->mPlainTxt == "0/" + QString::number(DEFAULT_ROW_COUNT));
    QVERIFY(mTestObject->mCheckBox->mChecked == false);  
    QVERIFY(mTestObject->primaryAction()->mTxt == "OK");
    QVERIFY(mTestObject->secondaryAction()->mTxt == "Cancel");
        
    // all items exists beforehand 
    title = "test2";
    mTestObject->setContent(title, mTestWidget);
    QVERIFY(mTestObject->mVideoList != 0);
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    // list container gets initialized at exec
    QVERIFY(mTestObject->mListContainer == 0);
    QVERIFY(mTestObject->primaryAction());
    QVERIFY(mTestObject->secondaryAction());
    QVERIFY(mTestObject->mHeading->mPlainTxt == title); 
    QVERIFY(mTestObject->mItemCount->mAlignment == Qt::AlignRight);
    QVERIFY(mTestObject->mItemCount->mPlainTxt == "0/" + QString::number(DEFAULT_ROW_COUNT));
    QVERIFY(mTestObject->mCheckBox->mChecked == false);  
    QVERIFY(mTestObject->primaryAction()->mTxt == "OK");
    QVERIFY(mTestObject->secondaryAction()->mTxt == "Cancel");
    
}

// ---------------------------------------------------------------------------
// testExec
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testExec()
{
    // no videolist
    mTestObject->exec();
    QVERIFY(mTestObject->mVideoList == 0);
    QVERIFY(mTestObject->mHeading == 0);
    QVERIFY(mTestObject->mItemCount == 0);
    QVERIFY(mTestObject->mCheckBox == 0);
    QVERIFY(mTestObject->mListContainer == 0);
    QVERIFY(mTestObject->mSelection.count() ==  0);
    
    QString title = "test";
    mTestObject->setContent(title, mTestWidget);
    
    // mListContainer not yet loaded
    HbDialog::execReturnPrimary = false;
    mTestObject->exec();
    QVERIFY(mTestObject->mVideoList != 0);    
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mSelection.count() ==  0);
    QVERIFY(mTestObject->mVideoList->mVisibility = true);
    QVERIFY(mTestObject->mVideoList->mEndSelModelStartedCount == 1);
    QVERIFY(mTestObject->mVideoList->mStartSelModeStartedCount == 1);
    QVERIFY(mTestObject->mVideoList->mSelectionMode == HbAbstractItemView::NoSelection);
    
    // mListContainer loaded
    mTestObject->mVideoList->mVisibility = false;
    mTestObject->mVideoList->mEndSelModelStartedCount = 0;
    mTestObject->mVideoList->mStartSelModeStartedCount = 0;
    
    HbDialog::execReturnPrimary = false;
    mTestObject->exec();
    QVERIFY(mTestObject->mVideoList != 0);    
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mSelection.count() ==  0);
    
    QVERIFY(mTestObject->mVideoList->mVisibility = true);
    QVERIFY(mTestObject->mVideoList->mEndSelModelStartedCount == 1);
    QVERIFY(mTestObject->mVideoList->mStartSelModeStartedCount == 1);
    QVERIFY(mTestObject->mVideoList->mSelectionMode == HbAbstractItemView::NoSelection);
    
    mTestObject->mVideoList->mVisibility = false;
    mTestObject->mVideoList->mEndSelModelStartedCount = 0;
    mTestObject->mVideoList->mStartSelModeStartedCount = 0;
    // not accepted
    HbDialog::execReturnPrimary = true;
    mTestObject->exec();
    QVERIFY(mTestObject->mVideoList != 0);    
    QVERIFY(mTestObject->mHeading != 0);
    QVERIFY(mTestObject->mItemCount != 0);
    QVERIFY(mTestObject->mCheckBox != 0);
    QVERIFY(mTestObject->mListContainer != 0);
    QVERIFY(mTestObject->mSelection.count() ==  0);
    
    QVERIFY(mTestObject->mVideoList->mVisibility = true);
    QVERIFY(mTestObject->mVideoList->mEndSelModelStartedCount == 1);
    QVERIFY(mTestObject->mVideoList->mStartSelModeStartedCount == 1);
    QVERIFY(mTestObject->mVideoList->mSelectionMode == HbAbstractItemView::NoSelection);
}

// ---------------------------------------------------------------------------
// testmMarkAllStateChangedSlot
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testmMarkAllStateChangedSlot()
{   
    connect(this, SIGNAL(signalTestMarkAll(int)), mTestObject, SLOT(markAllStateChangedSlot(int)));
    // without videolist
    emit signalTestMarkAll(Qt::Checked);
    QVERIFY(mTestWidget->mInitiallySelected.indexes().count() == 0);

    QString title = "test";
    mTestObject->setContent(title, mTestWidget);   
    mTestObject->mForcedCheck = true;
    // with forcedcheck
    emit signalTestMarkAll(Qt::Checked);
    QVERIFY(mTestWidget->mInitiallySelected.indexes().count() == 0);
    
    mTestObject->mForcedCheck = false;
    // checked
    emit signalTestMarkAll(Qt::Checked);
    QVERIFY(mTestWidget->mInitiallySelected.indexes().count() == DEFAULT_ROW_COUNT);
    
    // not checked (== everything else than Qt::Checked
    emit signalTestMarkAll(Qt::Checked + 1);
    QVERIFY(mTestWidget->mInitiallySelected.indexes().count() == 0);
    disconnect(this, SIGNAL(signalTestMarkAll(int)), mTestObject, SLOT(markAllStateChangedSlot(int)));
}

// ---------------------------------------------------------------------------
// testSelectionChangedSlot
// ---------------------------------------------------------------------------
//
void TestVideoListSelectionDialog::testSelectionChangedSlot()
{
    connect(this, SIGNAL(signalTestSelectionChanged(const QItemSelection&, const QItemSelection&)), 
            mTestObject, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection &)));
    ////////////////
    // no primary
    ////////////////
    emit signalTestSelectionChanged(QItemSelection (), QItemSelection ());
    QVERIFY(mTestObject->mSelection.count() == 0);
    
    ////////////////
    // selected == 0 and deselected == 0
    ////////////////
    QString title = "test";
    mTestObject->setContent(title, mTestWidget);           
    emit signalTestSelectionChanged(QItemSelection (), QItemSelection ());
    QVERIFY(mTestObject->getSelection().indexes().count() == 0);
    QVERIFY(mTestObject->primaryAction()->mDisable == true);
    QVERIFY(mTestObject->mCheckBox->mChecked == false);

    ////////////////
    // deselected > 0
    ////////////////
    // "deselect" items from 0 to DEFAULT_ROW_COUNT / 2
    QItemSelection deselection(mSourceModel->index(0, 0, QModelIndex()), mSourceModel->index(DEFAULT_ROW_COUNT / 2, 0, QModelIndex())); 
    
    emit signalTestSelectionChanged(QItemSelection (), deselection);
    QVERIFY(mTestObject->getSelection().indexes().count() == 0);
    QVERIFY(mTestObject->primaryAction()->mDisable == true);
    QVERIFY(mTestObject->mItemCount->mPlainTxt == "0/" + QString::number(DEFAULT_ROW_COUNT));
    QVERIFY(mTestObject->mCheckBox->mChecked == false);
    
    ////////////////
    // selected > 0
    ////////////////
    mTestObject->mSelection.clear();
    // "select" items from (DEFAULT_ROW_COUNT / 2) + 1 to DEFAULT_ROW_COUNT - 1
    QItemSelection selection(mSourceModel->index((DEFAULT_ROW_COUNT / 2) + 1, 0, QModelIndex()), 
                                mSourceModel->index(DEFAULT_ROW_COUNT - 1, 0, QModelIndex()));
   
    int count = 1 + ((DEFAULT_ROW_COUNT - 1) - ((DEFAULT_ROW_COUNT / 2) + 1));
    
    ///////////////
    // selected > 0
    ///////////////
    emit signalTestSelectionChanged(selection, deselection);
    QVERIFY(mTestObject->getSelection().indexes().count() == count);
    QVERIFY(mTestObject->primaryAction()->mDisable == false);
    QVERIFY(mTestObject->mItemCount->mPlainTxt == QString::number(count) + "/" + QString::number(DEFAULT_ROW_COUNT));
    QVERIFY(mTestObject->mCheckBox->mChecked == false);
    
    //////////////////
    // without videolist
    ///////////////
    mTestObject->mVideoList = 0;
    selection.clear();
    deselection.clear();
    mTestObject->mSelection.clear();
    
    // "select" 2 items
    selection.select(mSourceModel->index(DEFAULT_ROW_COUNT - 2, 0, QModelIndex()),
                        mSourceModel->index(DEFAULT_ROW_COUNT - 1, 0, QModelIndex()));
    deselection.select( mSourceModel->index(0, 0, QModelIndex()),
                        mSourceModel->index(DEFAULT_ROW_COUNT - 3, 0, QModelIndex()));
   
    emit signalTestSelectionChanged(selection, deselection);
    QVERIFY(mTestObject->getSelection().indexes().count() == 2);
    QVERIFY(mTestObject->primaryAction()->mDisable == false);
    QVERIFY(mTestObject->mCheckBox->mChecked == false);
    // count should have not changed
    QVERIFY(mTestObject->mItemCount->mPlainTxt == QString::number(count) + "/" + QString::number(DEFAULT_ROW_COUNT));
    
    ///////////////
    // wihtout mItemCount
    ///////////////
    mTestObject->mVideoList = mTestWidget;
    HbLabel *tmpLabel = mTestObject->mItemCount;
    mTestObject->mItemCount = 0;
    selection.clear();
    deselection.clear();
    mTestObject->mSelection.clear();
    
    // "select" 2 items
    selection.select(mSourceModel->index(DEFAULT_ROW_COUNT - 2, 0, QModelIndex()),
                        mSourceModel->index(DEFAULT_ROW_COUNT - 1, 0, QModelIndex()));   
    deselection.select( mSourceModel->index(0, 0, QModelIndex()),
                           mSourceModel->index(DEFAULT_ROW_COUNT - 3, 0, QModelIndex()));
    
    emit signalTestSelectionChanged(selection, deselection);
    QVERIFY(mTestObject->getSelection().indexes().count() == 2);
    QVERIFY(mTestObject->primaryAction()->mDisable == false);
    QVERIFY(mTestObject->mCheckBox->mChecked == false);
    mTestObject->mItemCount = tmpLabel;
    
    ///////////////
    // selectionCount == rowCount and rowcount == 0
    ///////////////
    mSourceModel->setRowCount(0);
    selection.clear();
    deselection.clear();
    mTestObject->mSelection.clear();

    emit signalTestSelectionChanged(selection, deselection);
    QVERIFY(mTestObject->getSelection().indexes().count() == 0);
    QVERIFY(mTestObject->primaryAction()->mDisable == true);
    QVERIFY(mTestObject->mItemCount->mPlainTxt == "0/0");
    QVERIFY(mTestObject->mCheckBox->mChecked == false);
    
    mSourceModel->setRowCount(DEFAULT_ROW_COUNT);
    
    ///////////////
   // selectionCount == rowCount and rowcount > 0
   ///////////////
    selection.clear();
    deselection.clear();
    selection.select(mSourceModel->index(0, 0, QModelIndex()),
                            mSourceModel->index(DEFAULT_ROW_COUNT - 1, 0, QModelIndex()));
    
    emit signalTestSelectionChanged(selection, deselection);
    QVERIFY(mTestObject->getSelection().indexes().count() == DEFAULT_ROW_COUNT);
    QVERIFY(mTestObject->primaryAction()->mDisable == false);
    QVERIFY(mTestObject->mCheckBox->mChecked == true);
    QVERIFY(mTestObject->mItemCount->mPlainTxt == QString::number(DEFAULT_ROW_COUNT) + "/" + QString::number(DEFAULT_ROW_COUNT));
    
    disconnect(this, SIGNAL(signalTestSelectionChanged(const QItemSelection&, const QItemSelection&)), 
                mTestObject, SLOT(selectionChangedSlot(const QItemSelection&, const QItemSelection &)));
}
// End of file
    


