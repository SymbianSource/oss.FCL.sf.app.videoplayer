/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines callback method which is used to indicate*
*/




#ifndef VCXHGVODTHUMBNAILOBSERVER_H
#define VCXHGVODTHUMBNAILOBSERVER_H

class CGulIcon;

/**
*  MVcxNsImageConverterObserver class definition.
*  Provides an interface for image converter observer.
*
*/
class MVcxHgVodThumbnailObserver
    {
    public:        

        /**
        * Notify utilizing object about completed thumbnail fetching.
        *
        * @param aIcon Pointer to the created icon, ownership is transferred.
        * @param aClientId Client specified id for the thumbnail.
        */
        virtual void MvtoThumbnailReady( CGulIcon* aIcon, TInt64 aClientId ) = 0;        
    };

#endif // VCXHGVODTHUMBNAILOBSERVER_H   
