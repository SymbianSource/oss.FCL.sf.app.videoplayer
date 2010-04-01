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
* Description:   VideoCollectionWrapper dummy class definition
* 
*/

#ifndef __VIDEOCOLLECTIONWRAPPER_H__
#define __VIDEOCOLLECTIONWRAPPER_H__


// INCLUDES
#include <QObject>


// FORWARD DECLARATIONS
class QAbstractItemModel;

class VideoSortFilterProxyModel;


class VideoCollectionWrapper : public QObject         
{    
    Q_OBJECT
    
public: // Constructor


    enum TModelType
    {
        EAllVideos,
        ECollections,
        ECollectionContent,
        EGeneric
    };

    /**
     * Returns singleton instance for this class.
     * 
     * WARNING! Not safe to call this from destructor of another function scope static object!
     * 
     * @return The singleton instance.
     */
    static VideoCollectionWrapper &instance();

    /**
     * Decreases the reference count, when count reaches zero cleanup is done.
     * 
     */
    void decreaseReferenceCount();    

   /**
    * return mModel
    */
    VideoSortFilterProxyModel* getModel(int type);

   /**
    * NOP at the moment
    */
   void reset();
	
   /**
    * sete mModel
    */
   void setModel(VideoSortFilterProxyModel* model);
   
signals:
   
    void error(int errorCode, QVariant &additional);

private:
   
   /**
    * Private contructor.
    */    
   VideoCollectionWrapper(); 
   
   /**
    * Private destructor.
    *
    */
   virtual ~VideoCollectionWrapper();    
   
    
private:
    /**
     * model to be used when testing videofiledetailsviewplugin
     */
    VideoSortFilterProxyModel* mModel;

public:
    
    static VideoCollectionWrapper *mInstance;
    
    static int mReferenceCount;
    
};

#endif  // __VIDEOCOLLECTIONWRAPPER_H__
// End of file
    


