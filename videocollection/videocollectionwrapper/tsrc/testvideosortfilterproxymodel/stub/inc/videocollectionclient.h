
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
* Description:   Stub videocollectionclient class for videocollectionsortfilterproxy unit tests
*
*/

#ifndef __VIDEOCOLLECTIONCLIENT_H
#define __VIDEOCOLLECTIONCLIENT_H

//#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>
#include <qstring.h>
#include <qlist.h>

// FORWARD DECLARATIONS
class VideoCollectionUtils;


class  VideoCollectionClient
{

    public: // Constructors and destructor

        /**
         * constructor
         *
         */
    VideoCollectionClient();

        /**
         * destructor
         *
         */
    virtual ~VideoCollectionClient();

    public:

    /**
     *
     * if gFailInit is true returns -1
     * if gFailInit is false returns 0
     *
     * @return int
     */
    int initialize();

    /**
     *
     * if gFailStartOpen is true returns -1
     * if gFailStartOpen is false returns 0
     *
     * @return int
     */
    int startOpenCollection(int level);

    /**
     *
     * if gFailMediaPlayback is true returns -1
     * if gFailMediaPlayback is false returns 0
     *
     * @return int
     */
    int openVideo(int mpxId1);

    /**
     *
     * if gFailMediaDetails is true returns -1
     * if gFailMediaDetails is false returns 0
     *
     * @return int
     */
    int getVideoDetails(int mpxId1);

    /**
     * @return int
     */
    int addNewCollection(QString name, QString thumbnail, QList<TMPXItemId> mediaIds);

    /**
     * sets gFailInit
     *
     * @param bool
     */
    static void setInitFailure(bool fails);

    /**
     * sets gFailStartOpen
     *
     * @param bool
     */
    static void setStartopenFailure(bool fails);


    /**
     * sets gFailMediaPlayback
     *
     * @param bool
     */
    static void setOpenMediaplaybackFailure(bool fails);

    /**
     * sets gFailMediaDetails
     *
     * @param bool
     */
    static void setOpenMediaDetailsFailure(bool fails);

    /**
     * sets gFailSetSort
     *
     * @param bool
     */
    static void setSortMediasFailure(bool fails);

    /**
     * returns gSettedSortOrder
     *
     * @return TVcxMyVideosSortingOrder
     */
    static TVcxMyVideosSortingOrder getSettedSortOrder();
    
    /**
     * calls collection to go back to collection level
     * 
     */
    int back();

public:
    static bool mFailInit;
    static bool mFailStartOpen;
    static bool mFailMediaPlayback;
    static bool mFailMediaDetails;
    static bool mFailSetSort;
    static bool mFailAddNewCollection;
    static TVcxMyVideosSortingOrder mSettedSortOrder;
    static int mSortOrderSetCount;
    static QString mAddNewCollectionName;
    static QString mAddNewCollectionThumb;
    static QList<TMPXItemId> mAddNewCollectionIds;
    
private:

    /**
     * static instance from this class
     */
    static VideoCollectionClient *testInstance;
};

#endif // __VIDEOCOLLECTIONCLIENT_H
