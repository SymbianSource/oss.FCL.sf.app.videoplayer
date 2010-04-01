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
* Description:  stub videocollection ui loader for testing video list selection dialog
* 
*/
#ifndef _VIDEOCOLLECTIONUILOADER_H_
#define _VIDEOCOLLECTIONUILOADER_H_

#include <QString>
#include "hblabel.h"
#include "hbcheckbox.h"
#include "hbstackedwidget.h"

static const char* DOCML_NAME_DLG_HEADINGLBL          = "mHeadingLabel";
static const char* DOCML_NAME_MARKALL                 = "mCheckMarkAll";
static const char* DOCML_NAME_LBL_SELECTION           = "mSelectionCount";
static const char* DOCML_NAME_LIST_CONTAINER          = "mListContainer";

class VideoCollectionUiLoader 
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
        T *object = 0;
        if(name == DOCML_NAME_DLG_HEADINGLBL)
        {
            if(!mHeadingLabel)
            {
                mHeadingLabel = new HbLabel();
            }
            object = qobject_cast<T*>(mHeadingLabel);
        }
        if(name == DOCML_NAME_MARKALL)
        {
            if(!mCheckMarkAll)
            {
                mCheckMarkAll = new HbCheckBox();
            }
            object = qobject_cast<T*>(mCheckMarkAll);
        }
        if(name == DOCML_NAME_LBL_SELECTION)
        {
            if(!mSelectionCount)
            {
                mSelectionCount = new HbLabel();
            }
            object = qobject_cast<T*>(mSelectionCount);
        }
        if(name == DOCML_NAME_LIST_CONTAINER)
        {
            if(!mListContainer)
            {
                mListContainer = new HbStackedWidget();
            }
            object = qobject_cast<T*>(mListContainer);
        }
        return object; 
    }
    
    /**
     * mHeadingLabel
     */
    HbLabel *mHeadingLabel;
       
    /**
     * mSelectionCount
     */
    HbLabel *mSelectionCount;
      
    /**
     * mCheckMarkAll
     */
    HbCheckBox *mCheckMarkAll;
    
    /**
     * mListContainer
     */
    HbStackedWidget *mListContainer;
  

};

#endif // _VIDEOCOLLECTIONUILOADER_H_
