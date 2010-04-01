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
* Description: dummy videosortfilterproxymodel definition for 
*              VideoListSelectionDialog unit tests
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
            
};



#endif  //VIDEOSORTFILTERPROXYMODEL_H
