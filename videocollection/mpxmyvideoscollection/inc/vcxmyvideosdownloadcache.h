/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Contains cached MDS media items which are being downloaded.*
*/




#ifndef VCXMYVIDEOSDOWNLOADCACHE_H
#define VCXMYVIDEOSDOWNLOADCACHE_H

// INCLUDES

// FORWARD DECLARATIONS
class CMPXMedia;

// CONSTANTS

// CLASS DECLARATION

/**
* Used for storing download ids and media object pointers to circular buffers for fast access.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosDownloadCache) : public CBase
    {    
    public: // Constructors and destructor

        /**
        * Two-phased constructor
        * @return   Object constructed
        */
        static CVcxMyVideosDownloadCache* NewL();

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosDownloadCache();
 
    private:
        /**
        * Constructor
        */
        CVcxMyVideosDownloadCache();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL ();

    public:                        
        /**
        * Tries to find item with aDownloadId from Download Cache.
        *
        * @param aDownloadId   Download ID in Download Manager.
        * @return              Returns pointer to media object, NULL if not found.
        *                      Ownership does not move.
        */
        CMPXMedia* Get( TUint32 aDownloadId );

        /**
        * Adds download ID and index pair to Download Cache.
        *
        * @param aDownloadId  Download ID in Download Manager
        * @param aVideo       Pointer to video, ownership does not move.
        */
        void Add( TUint32 aDownloadId, CMPXMedia* aVideo );
        
        /**
        * Deletes item from Download Cache
        *
        * @param aVideo  Video to remove.
        */
        void Delete( CMPXMedia* aVideo );
                                          
    private:
   
        /**
        * Array used as circular buffer, contains download ids.
        */
        RArray<TUint32> iDownloadId;
        
        /**
        * Array used as circular buffer, contains pointers to medias.
        */
        RPointerArray<CMPXMedia> iMedia;
        
        /**
        * Current position of the circular buffers (iDownloadId and iMedia).
        * Next item added is written to this position.
        */
        TInt iPos;

    };

#endif   // VCXMYVIDEOSVIDEOCACHE_H


