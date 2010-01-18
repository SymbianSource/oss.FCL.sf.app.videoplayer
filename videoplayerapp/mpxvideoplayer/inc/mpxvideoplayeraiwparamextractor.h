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
* Description:  Application class
*
*/

// Version : %version: da1mmcf#3 %

#ifndef MPXVIDEOPLAYERAIWPARAMEXTRACTOR_H_
#define MPXVIDEOPLAYERAIWPARAMEXTRACTOR_H_

class CAiwGenericParamList;

/**
*  CMpxVideoPlayerAiwParamExtractor
*  Extracts parameters from generic paramater list
*
*  @lib mpxvideoplayer.exe 
*  @since MpxVideoPlayer 0.1
*/
class CMpxVideoPlayerAiwParamExtractor : public CBase
    {
    public:        // Constructors and destructor
        static CMpxVideoPlayerAiwParamExtractor* NewL( );        
           
        /**
        * Destructor.
        */
        virtual ~CMpxVideoPlayerAiwParamExtractor();
           
        /**
        * Extracts parameters from given generic parameter list.
        */
        void  ExtractParameters( const CAiwGenericParamList& aParamList );
           
        /**
        * Returns download id extracted previously from generic parameter list
        */
        TInt32  DlId( );
        
    private:
        /**
         * Constructor
         */
        CMpxVideoPlayerAiwParamExtractor( );
         
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
        
        /**
         * DlId found from paramaters
         */
        TInt32 iDlId;
    };

#endif /*MPXVIDEOPLAYERAIWPARAMEXTRACTOR_H_*/
