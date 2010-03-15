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
* Description:   
*/


#ifndef FILEDETAILSPLUGIN_H
#define FILEDETAILSPLUGIN_H

// INCLUDES
#include <e32std.h>
#include <filedetailspluginif.h>


// CLASS DECLARATION    
/**
*  Implementation of the file details plugin
*
*/
NONSHARABLE_CLASS( CFileDetailsPlugin ) : public CFileDetailsPluginIF 
    {
    public:// Constructors and destructor

        /**
        * Two-phased constructor.        
        */
        static CFileDetailsPlugin* NewL();
        
        /**
        * C+ destructor
        */
        virtual ~CFileDetailsPlugin();        
   
    public: // From CFileDetailsPluginIF

        /**
           * Shows file details based on given mpx media
           * @param    aDetails  Already constructed details to be shown    
           * @return   None
           */  
        void ShowFileDetailsL( const CMPXMedia& aMedia );

    private:// Constructors and destructor

        /**
        * C++ constructor
        */
        CFileDetailsPlugin();
        
    };

#endif // FILEDETAILSPLUGIN_H
