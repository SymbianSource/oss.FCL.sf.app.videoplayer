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
* Description:   Videolist sort filter proxy implementation
* 
*/

#ifndef VIDEOSORTFILTERPROXYMODEL_H
#define VIDEOSORTFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>

class VideoSortFilterProxyModel : public QSortFilterProxyModel 
{
    Q_OBJECT
    
public: 
    
    /**
     * Contructor.
     *
     * @param parent parent of this widget
     */
    VideoSortFilterProxyModel(QObject *parent=0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoSortFilterProxyModel();
    
public:
	
	/**
     * Opens a collection into all videos category, if not yet opened, and
     * returns pointer to model
     */    
    int open(int level);
    
    /**
     * Starts sorting using idle timer: starts timer with zero 
     * interval. When system has time to run timer sort() -method
     * is called.
     *  
     */
    void doSorting(int role, Qt::SortOrder order, bool async = true);
    
    /**
     * Method to delete items at provided indeces
     */
    int deleteItems(const QModelIndexList &indexList);

    void reset();
    
    /**
     * Overridden, so that we can return the mSortRole parameter.
     */
    int sortRole() const;
    
    /**
     * Overridden, so that we can return the mSortOrder parameter.
     */
    Qt::SortOrder sortOrder() const;
    
signals:

    /**
     * Emitted after sorting is done
     *  
     */
    void sortingReady();
    
    /**
     * emitted to indicate model readiness
     */
    void modelReady();
    
protected: // from QSortFilterProxyModel
    
    /**
     * Compares items based in the role setted before sorting.
     *  
     */
    bool lessThan ( const QModelIndex & left, const QModelIndex & right ) const;

    /**
     * Filtering functionality. Called by the fw.
     *  
	 * @param source_row row to chexk
	 * @param source_parent row's parent
	 * 
	 * @return bool row accepted(true) or not accepted(false)
     */
    bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;
    
    
private slots:

    /**
     * Timeout slot for zero timer. Stops timer and calls sort(). 
     * After call is finished, emits sortingReady.
     *  
     */
    void processSorting();
    
private:
    QMap<int, QVariant> mData;
    int mRowCount;
    
public:
    int mLevel;
    
	static bool mOpenFails;
	static int mSortRole;
	static Qt::SortOrder mSortOrder;
	static bool mSortAsync;
    static bool mDeleteItemsFails;
    static int mDoSortingCallCount;
    
};

#endif  //VIDEOSORTFILTERPROXYMODEL_H
