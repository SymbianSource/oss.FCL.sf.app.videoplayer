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
#include <qhash.h>
#include <qmap.h>

// Constants
static const char* DOCML_VIDEOCOLLECTIONVIEW_FILE     = ":/layout/collectionview.docml";
static const char* DOCML_NAME_VIEW                    = "view";

// Videocollection View
static const char* DOCML_NAME_VC_HEADINGBANNER        = "vc:mBanner";
static const char* DOCML_NAME_VC_COLLECTIONWIDGET     = "vc:mCollectionWidget";
static const char* DOCML_NAME_VC_COLLECTIONCONTENTWIDGET = "vc:mCollectionContentWidget";
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
static const char* DOCML_NAME_HINT_BUTTON             = "vc:mHintButton";
static const char* DOCML_NAME_HINT_LABEL              = "vc:mHintTextLabel";
static const char* DOCML_NAME_NO_VIDEOS_LABEL         = "vc:mNoVideosLabel";

// video multiselection dialog
static const char* DOCML_VIDEOSELECTIONDIALOG_FILE    = ":/layout/videolistselectiondialog.docml";
static const char* DOCML_NAME_DIALOG                  = "mMultiSelectionDialog";
static const char* DOCML_NAME_DLG_HEADINGLBL          = "mHeadingLabel";
static const char* DOCML_NAME_CHECK_CONTAINER         = "mCheckBoxContainer";
static const char* DOCML_NAME_MARKALL                 = "mCheckMarkAll";
static const char* DOCML_NAME_LBL_SELECTION           = "mSelectionCount";
static const char* DOCML_NAME_LIST_CONTAINER          = "mListContainer";

// async loading timeout
static const int ASYNC_FIND_TIMEOUT                   = 50; // ms

class QActionGroup;
class HbAction;

// Class declaration
class VideoCollectionUiLoader:
    public QObject,
    public HbDocumentLoader
{
    Q_OBJECT
    
private:
    /** actions used in menus and toolbars */
    enum ActionIds
    {
        EActionSortBy          =  1,
        EActionSortByDate,
        EActionSortByName,
        EACtionSortByItemCount,
        EActionSortByLength,
        EActionSortBySize,
        EActionNewCollection,
        EActionAddToCollection,
        EActionDelete,
        ETBActionAllVideos,
        ETBActionCollections,
        ETBActionServices,
        ETBActionAddVideos,
        ETBActionRemoveVideos,
        ETBActionSortVideos
    };
    
    /** VideoCollectionUiLoader parameter class */ 
    class Params
    {
    public:
        Params(const QString& name,
            bool isWidget = false,
            QObject *receiver = 0,
            const char *docml = 0,
            const char *member = 0):
            mName(name),
            mIsWidget(isWidget),
            mReceiver(receiver),
            mDocml(docml),
            mMember(member)
            {
            // nothing to do 
            }
        
        bool isDuplicate(const Params& params) const
            {
            bool isSame(false);
            
            if (mName == params.mName &&
                mReceiver == params.mReceiver &&
                mDocml == params.mDocml &&
                mMember == params.mMember)
            {
                isSame = true;
            }
            
            return isSame;
            }
        
    public:
        QString mName;
        bool mIsWidget;
        QObject *mReceiver;
        const char *mDocml;
        const char *mMember;
    };

public:
    /**
     * C++ constructor.
     */
    VideoCollectionUiLoader();
    
    /**
     * C++ destructor.
     */
    virtual ~VideoCollectionUiLoader();
    
    /**
     * Starts to load a specified UI section.
     * 
     * @param uiSection, UI section to load.
     * @param receiver, Receiver of a ready signal.
     * @param widgetSlot, Slot which is called when a widget is ready.
     * @param objectSlot, Slot which is called when an object is ready.
     * @return None.
     */
    void startLoading(QSet<QString> uiSections,
        QObject *receiver,
        const char *widgetSlot,
        const char *objectSlot);

    /**
     * Returns the requested widget casted to correct type
     *
     * @param name Name of the widget
     * @return Pointer to the widget
     */
    template<class T>
    T* findWidget(const QString &name)
    {
        return qobject_cast<T*>(doFindWidget(name));
    }

    /**
     * Returns the requested object casted to correct type
     *
     * @param name Name of the object
     * @return Pointer to the object
     */
    template<class T>
    T* findObject(const QString &name)
    {
        return qobject_cast<T*>(doFindObject(name));
    }
    
    /**
     * Set video services in use.
     */
    void setIsService(bool isService);

signals:
    /**
     * Signals that widget has been loaded asynchonously.
     * 
     * @param widget, Widget which was loaded.
     * @param name, Name of the widget in document.
     * @return None.
     */
    void widgetReady(QGraphicsWidget *widget, const QString &name);

    /**
     * Signals that object has been loaded asynchonously.
     * 
     * @param object, Object which was loaded.
     * @param name, Name of the object in document.
     * @return None.
     */
    void objectReady(QObject *object, const QString &name);

public:
    /**
     * Loads widget from document.
     * 
     * @param name, Widget name.
     * @return QGraphicsWidget*.
     */
    QGraphicsWidget* doFindWidget(const QString &name);
    
    /**
     * Loads object from document.
     * 
     * @param name, Widget name.
     * @return QGraphicsWidget*.
     */
    QObject* doFindObject(const QString &name);
    
private:
    /**
     * Adds a ui section to async loading queue.
     */
    void addToQueue(Params &params);
    
    /**
     * Init a specific widget.
     */
    void initWidget(QGraphicsWidget *widget,
        const QString &name);
    
    /**
     * Init a specific object.
     */
    void initObject(QObject *object,
        const QString& name);
    
private:
    /** from QObject */
    void timerEvent(QTimerEvent *event); 
    
    /** from HbDocumentLoader */
    QObject *createObject(const QString& type, const QString &name);
    
private:
    /**
     * Run next async find request from queue.
     */
    void runNext();
    
    /**
     * Check that set params are valid.
     */
    bool isValid(const Params &params);
    
private:
    /** async queue */
    QList<Params> mQueue;
    
    /** timer id */
    int mTimerId;
    
    /** list of loaded widgets */
    QHash<QString, QGraphicsWidget*> mWidgets;

    /** list of loaded objects */
    QHash<QString, QObject*> mObjects;
    
    /** menu actions */
    QMap<ActionIds, HbAction*> mMenuActions;
    
    /** toolbar actions */
    QMap<ActionIds, HbAction*> mToolbarActions;
    
    /** action group for "sort by" actions */
    QActionGroup* mSortGroup;
    
    /** is service */
    bool mIsService;
};

#endif // _VIDEOCOLLECTIONUILOADER_H_
