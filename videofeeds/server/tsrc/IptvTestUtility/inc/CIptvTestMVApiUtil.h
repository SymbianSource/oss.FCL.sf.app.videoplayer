/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Object to offer some common utility functions to Iptv engine testing*
*/




#ifndef CIPTVTESTMVAPIUTIL_H
#define CIPTVTESTMVAPIUTIL_H

//  INCLUDES
#include "MIptvMyVideosClientObserver.h"
#include "CIptvMyvideosClient.h"

// CONSTANTS
                                            
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
//class CIptvService;
class CIptvMyVideosVideoFullDetails;
class CIptvMediaContent;
// CLASS DECLARATION

/**
*  
*  
*  
*
*  @since
*/
class CIptvTestMVApiUtil : public CBase, public MIptvMyVideosClientObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIptvTestMVApiUtil* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CIptvTestMVApiUtil();


    public: // New functions

        /**
        * 
        * @since
        * @param 
        */
        
        /**
        * 
        */
		TInt GetMyVideosVideoDetailsL(CIptvMyVideosGlobalFileId& aFileId, CIptvMyVideosVideoFullDetails& aVideoDetails);


        /**
        * Clean up MyVideos
        */
        TInt ReallycleanL();
        
        //unused asynchcronous callbacks
        void ContentsUpdated(TIptvContentUpdatedEvent aEvent );
        
       	void CopyOrMoveResponse(TInt /* aServerStatus */, TInt /* aOperationStatus */, TInt /* aFailed */) {};
        void TotalVideoLenghtResponse(TIptvPlayTime /*aTotalPlayTime*/, TIptvFileSize /*aTotalFileSize*/) {};
        
        TBool GetVideoInfo(CIptvMyVideosGlobalFileId& aFileId, 
                            TUint32& aServiceId, 
                            TDes& aVideoName, 
                            TDes& aFileName);
                            
        TInt DeleteVideoL(CIptvMyVideosGlobalFileId& aFileId);
                            
        void DeleteResponse(TInt /*aStatus*/);  
        
    private:

        /**
        * C++ default constructor.
        */
        CIptvTestMVApiUtil();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        

    
    private:    // Data
    CIptvMyVideosClient*  iIptvMyVideosClient;   

    };


#endif      // CIPTVTESTMVAPIUTIL_H   
            
// End of File
