
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

// INCLUDES

#define private public
#include "videosortfilterproxymodel.h"
#undef private

#include "filterproxytester.h"
#include "videolistdatamodel.h"

// ---------------------------------------------------------------------------
// FilterProxyTester
// ---------------------------------------------------------------------------
//
FilterProxyTester::FilterProxyTester(VideoCollectionCommon::TModelType type, QObject *parent) :
VideoSortFilterProxyModel(type, parent)
{
    // NOP
}

// ---------------------------------------------------------------------------
// ~FilterProxyTester
// ---------------------------------------------------------------------------
//
FilterProxyTester::~FilterProxyTester()
{
    // NOP
}

// ---------------------------------------------------------------------------
// callLessThan
// ---------------------------------------------------------------------------
//   
bool FilterProxyTester::callLessThan( const QModelIndex & left, const QModelIndex & right ) const
{
    return VideoSortFilterProxyModel::lessThan(left, right);
}

// ---------------------------------------------------------------------------
// callLessThan
// ---------------------------------------------------------------------------
//   
bool FilterProxyTester::callFilterAcceptsRow( int source_row, const QModelIndex & source_parent ) const
{
    return VideoSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

// ---------------------------------------------------------------------------
// setClient
// ---------------------------------------------------------------------------
//
int FilterProxyTester::setClient(VideoCollectionClient *collectionClient)
{
	VideoSortFilterProxyModel::mCollectionClient = collectionClient;
	return 0;
}

// ---------------------------------------------------------------------------
// getClient
// ---------------------------------------------------------------------------
//
VideoCollectionClient* FilterProxyTester::getClient()
{
    return VideoSortFilterProxyModel::mCollectionClient;
}

// End of file
    


