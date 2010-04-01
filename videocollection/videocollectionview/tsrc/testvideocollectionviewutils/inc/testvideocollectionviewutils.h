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
* Description:   tester for methods in VideoCollectionViewUtils
* 
*/

#ifndef __TESTVIDEOCOLLECTIONVIEWUTILS_H__
#define __TESTVIDEOCOLLECTIONVIEWUTILS_H__


// INCLUDES
#include <QtTest/QtTest>
#include "hbmessagebox.h"


class VideoCollectionWrapper;

class TestVideoVideoCollectionViewUtils : public QObject
{
    Q_OBJECT

    // test functions for the test framework
private slots:
   
    /**
     * will be called before each testfunction is executed.
     *
     */
    void init(); 
   
    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();

    /**
     * verifies showErrorMsgSlot
     */
    void testShowErrorMsgSlot();
    
    /**
     * verifies saveSortingValues
     */
    void testSaveSortingValues();

    /**
     * verifies loadSortingValues
     */
    void testLoadSortingValues();

};


#endif  // __TESTVIDEOCOLLECTIONVIEWUTILS_H__

// End of file
    


