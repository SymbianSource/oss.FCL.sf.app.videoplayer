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
* Description:   Unit test for methods in VideoThumbnailData
*
*/

#ifndef __TESTVIDEOTHUMBNAILDATA_H__
#define __TESTVIDEOTHUMBNAILDATA_H__

// INCLUDES
#include <QtTest/QtTest>

// FORWARD DECLARATIONS

class TestVideoThumbnailData : public QObject
{
    Q_OBJECT

    void init();

    void cleanup();
    
    bool checkThumbnailReadyCount(QSignalSpy *spy, int count);

    // test functions for the test framework
private slots:
    void testInstance();
    void testStartFetchingThumbnail();
    void testRemoveThumbnail();
    void testGetThumbnail();
    void testEnableBackgroundFetching();
    void testFreeThumbnailData();
    void testStartBackgroundFetching();

signals:

private:

};

#endif  // __TESTVIDEOTHUMBNAILDATA_H__

// End of file
