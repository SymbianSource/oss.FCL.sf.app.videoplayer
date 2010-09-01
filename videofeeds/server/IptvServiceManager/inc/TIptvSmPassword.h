/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/





#ifndef TIPTVSMPASSWORD_H
#define TIPTVSMPASSWORD_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Container class for user name and password.
 */
class TIptvSmPassword
    {

    public:     // Data
    
        /**
        * User name.
        */
        TBuf<KIptvSmPasswordsDbUserNameMaxLength> iUserName;
        
        /**
        * Password.
        */
        TBuf<KIptvSmPasswordsDbPasswordMaxLength> iPassword;
    };

#endif      // TIPTVSMPASSWORD_H   
            
// End of File
