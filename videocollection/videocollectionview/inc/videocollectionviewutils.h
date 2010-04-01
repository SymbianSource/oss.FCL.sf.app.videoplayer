/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: video collection view plugin's ui utils class
* 
*/

#ifndef __VIDEOCOLLECTIONVIEWUTILS_H__
#define __VIDEOCOLLECTIONVIEWUTILS_H__

#include <qobject.h>

class VideoCollectionViewUtils : public QObject
{
    Q_OBJECT
    
public:
    
    /**
     * Returns singleton instance for this class.
     * 
     * WARNING! Not safe to call this from destructor of another function scope static object!
     * 
     * @return The singleton instance.
     */
    static VideoCollectionViewUtils& instance();
    
    /**
     * Saves the sorting role and order to cenrep.
     * 
     * @param role The sorting role.
     * @param order The sorting order (e.g. ascending or descending).
     * @return Zero if operation succeeded, less than zero in error cases.
     */
    int saveSortingValues(int role, Qt::SortOrder order);
    
    /**
     * Loads the sorting role and order from cenrep.
     * 
     * @param role On return contains the sorting role.
     * @param order On return contains the sorting order
     * @return Zero if operation succeeded, less than zero in error cases.
     */
    int loadSortingValues(int& role, Qt::SortOrder& order);
    
    /**
     * Get service icon resource strings from cenrep.
     * 
     * @param icon On return contains the resource string for icon image.
     * @param iconPressed On return contains the resource string for pressed icon image.
     * @return Zero if operation succeeded, less than zero in error cases. 
     */
    int getServiceIconStrings(QString& icon, QString& iconPressed);
    
    /**
     * Get service URI string.
     * 
     * @return Service URI string. Invalid string in error cases.
     */
    QString getServiceUriString();
      
public slots:
    
    /**
     * Shows status msgs
     *
     * @param statusCode status code
     * @param additional additional data gotten from the status 
     */
    void showStatusMsgSlot(int statusCode, QVariant &additional);
   
private:
    
    /**
     * disables copy-constructor and assingment operator
     */
    Q_DISABLE_COPY(VideoCollectionViewUtils)
    
    /**
     * constructor
     */
    VideoCollectionViewUtils();
     
    /**
     * destructor
     */
    virtual ~VideoCollectionViewUtils();

    
};

#endif //__VIDEOCOLLECTIONUIUTILS_H__

