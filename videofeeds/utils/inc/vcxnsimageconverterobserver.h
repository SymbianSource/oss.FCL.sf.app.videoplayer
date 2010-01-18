/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Interface MVcxNsImageConverterObserver. Defines callback*
*/




#ifndef M_VCXNSIMAGECONVERTEROBSERVER_H
#define M_VCXNSIMAGECONVERTEROBSERVER_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CGulIcon;

// CLASS DECLARATION

/**
*  MVcxNsImageConverterObserver class definition.
*  Provides an interface for image converter observer.
*
*  @lib IptvUtil.lib
*/
class MVcxNsImageConverterObserver
    {
public:        

    /**
    * Notify utilizing object about completed image conversion
    *
    * @param aError Error code.
    * @param aIcon  Pointer to the created icon, ownership is transferred.
    *               NULL if conversion failed with error.
    */  
    virtual void ImageConversionCompletedL( TInt aError, CGulIcon* aIcon ) = 0;        
    };

#endif // M_VCXNSIMAGECONVERTEROBSERVER_H   
