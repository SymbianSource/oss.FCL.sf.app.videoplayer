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
* Description: tests for VideoOperatorService
*
*/

#ifndef TESTVIDEOOPERATORSERVICE_H
#define TESTVIDEOCOLLECTIONUILOADER_H

// INCLUDES
#include <qttest/qttest>

// FORWARD DECLARATIONS
class VideoOperatorService;

class TestVideoOperatorService : public QObject
{
    Q_OBJECT

public:
    TestVideoOperatorService();
    virtual ~TestVideoOperatorService();
    
private slots: // test functions for the test framework
    void init();
        
    void cleanup();
    
private slots:
    
    void testLoad();
    void testTitle();
    void testIconResource();
    void testLaunchService();
    
signals:
    
private:
    /** class under test */
    VideoOperatorService *mTestObject;
    

};

#endif//TESTVIDEOCOLLECTIONUILOADER_H
