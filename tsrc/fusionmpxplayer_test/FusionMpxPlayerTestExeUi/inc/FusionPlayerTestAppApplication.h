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
* Description: 
*
*/


#ifndef FUSIONPLAYERTESTAPPAPPLICATION_H
#define FUSIONPLAYERTESTAPPAPPLICATION_H

#include <aknapp.h>

/**
  *Class:		CFusionPlayerTestAppApplication
  */
class CFusionPlayerTestAppApplication : public CAknApplication
    {
public:  // from CApaApplication

/**
  *Function:	AppDllUid
  *
  *Discussion:	Returns the application DLL UID value
  *
  *Returns:		the UID of this Application/Dll
  *
  */
    TUid AppDllUid() const;

protected: // from CEikApplication
/**
  *Function:	CreateDocumentL
  *
  *Discussion:	Create a CApaDocument object and return a pointer to it
  *
  *Returns:		a pointer to the created document
  *
  */
    CApaDocument* CreateDocumentL();
    };

#endif // FUSIONPLAYERTESTAPPAPPLICATION_H
