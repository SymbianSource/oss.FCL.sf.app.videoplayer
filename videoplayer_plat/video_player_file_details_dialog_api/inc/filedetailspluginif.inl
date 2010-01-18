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
* Description:  File details plugin inlines.
*
*/



#ifndef FILEDETAILSPLUGINIF_INL
#define FILEDETAILSPLUGINIF_INL

// -----------------------------------------------------------------------------
// Instantiates appropriate plugin implementation.
// -----------------------------------------------------------------------------
//
inline CFileDetailsPluginIF* CFileDetailsPluginIF::NewL( )
    {        
    TEComResolverParams resolverParams;    
    TAny* interface = REComSession::CreateImplementationL( 
        TUid::Uid( KFileDetailsPluginInterfaceUid ),
        _FOFF ( CFileDetailsPluginIF, iDtorIDKey ), 
	resolverParams );
    return static_cast<CFileDetailsPluginIF*> ( interface );          
    }

// -----------------------------------------------------------------------------
// Interface's destructor
// -----------------------------------------------------------------------------
//
inline CFileDetailsPluginIF::~CFileDetailsPluginIF()
    {
    // Inform the ECOM framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation( iDtorIDKey );
    }

#endif //FILEDETAILSPLUGINIF_INL
