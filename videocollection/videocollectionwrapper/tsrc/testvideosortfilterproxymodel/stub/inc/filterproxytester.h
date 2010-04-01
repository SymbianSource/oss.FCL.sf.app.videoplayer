
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
* Description:  Helper class to test protected members from videocollectionsortfilterproxy 
* 
*/

#ifndef __FILTERLISTPROXY_TESTER_H
#define __FILTERLISTPROXY_TESTER_H

#include <QObject>

#include "videosortfilterproxymodel.h"

class VideoListDataModel;

class  FilterProxyTester : public VideoSortFilterProxyModel
{

    Q_OBJECT
    
public: // Constructors and destructor

    /**
     * Constructor
     *
     */
    FilterProxyTester(int type, QObject *parent=0);

    /**
     * destructor
     *
     */
    virtual ~FilterProxyTester();

public: // helper methods for test
    
    /**
     * Helper method to call protected lessThan
     *
     */
    bool callLessThan( const QModelIndex & left, const QModelIndex & right ) const;
    
    /**
     * Helper methor to call protected filterAcceptsRow.
     * 
     */
    bool callFilterAcceptsRow( int source_row, const QModelIndex & source_parent ) const;
    
    /**
     * For setting CVideoCollectionClient pointer
     *
     */
    int setClient(VideoCollectionClient *collectionClient);

    /**
     * For getting CVideoCollectionClient pointer
     *
     */
    VideoCollectionClient* getClient();

};

#endif // __FILTERLISTPROXY_TESTER_H


