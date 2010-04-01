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
* Description:
*
*/

#ifndef _VIDEOCOLLECTIONUILOADER_H_
#define _VIDEOCOLLECTIONUILOADER_H_

// System includes
#include <hbdocumentloader.h>

// Constants
static const char* DOCML_VIDEOCOLLECTIONVIEW_FILE     = ":/layout/collectionview.docml";
static const char* DOCML_NAME_VIEW                    = "view";

// Videocollection View
static const char* DOCML_NAME_VC_HEADINGBANNER        = "vc:mBanner";
static const char* DOCML_NAME_VC_VIDEOLISTWIDGET      = "vc:mListWidget";
static const char* DOCML_NAME_VC_VIDEOHINTWIDGET      = "vc:mHintWidget";

// Videocollection Options Menu
static const char* DOCML_NAME_OPTIONS_MENU            = "vc:mOptionsMenu";
static const char* DOCML_NAME_SORT_MENU               = "vc:mSortBy";

static const char* DOCML_NAME_SORT_BY_DATE            = "vc:mDate";
static const char* DOCML_NAME_SORT_BY_NAME            = "vc:mName";
static const char* DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS = "vc:mNumberOfItems";
static const char* DOCML_NAME_SORT_BY_TOTAL_LENGTH    = "vc:mTotalLength";
static const char* DOCML_NAME_SORT_BY_RATING          = "vc:mRating";
static const char* DOCML_NAME_SORT_BY_SIZE            = "vc:mSize";

static const char* DOCML_NAME_ADD_TO_COLLECTION       = "vc:mAddtoCollection";
static const char* DOCML_NAME_CREATE_COLLECTION       = "vc:mCreateNewCollection";
static const char* DOCML_NAME_DELETE_MULTIPLE         = "vc:mDeleteMultiple";

static const char* DOCML_NAME_PLAY_IN_QUEUE           = "vc:mPlayInQueue";
static const char* DOCML_NAME_PLAY_ALL_ITEMS          = "vc:mPlayAllItems";

// Videocollection hint widget
static const char* DOCML_NAME_NOVIDEOS_LABEL          = "vc:mNoVideosLabel";

// Class declaration
class VideoCollectionUiLoader : public HbDocumentLoader
{

public:

    VideoCollectionUiLoader();

    ~VideoCollectionUiLoader();

    /**
     * Returns the requested widget casted to correct type
     *
     * @param name Name of the widget
     * @return Pointer to the widget
     */
    template<class T>
    T* findWidget( QString name )
    {
        return qobject_cast<T*>( HbDocumentLoader::findWidget( name ) );
    }

    /**
     * Returns the requested object casted to correct type
     *
     * @param name Name of the object
     * @return Pointer to the object
     */
    template<class T>
    T* findObject( QString name )
    {
        return qobject_cast<T*>( HbDocumentLoader::findObject( name ) );
    }

    static bool mFindFailure;

private:

    QObject *createObject( const QString& type, const QString &name );
};

#endif // _VIDEOCOLLECTIONUILOADER_H_
