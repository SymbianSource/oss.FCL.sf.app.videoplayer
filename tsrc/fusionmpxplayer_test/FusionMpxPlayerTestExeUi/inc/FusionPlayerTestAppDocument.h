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


#ifndef FUSIONPLAYERTESTAPPDOCUMENT_H__
#define FUSIONPLAYERTESTAPPDOCUMENT_H__


#include <akndoc.h>

// Forward references
class CEikApplication;
class MMPXViewUtility;

/**
 * Class:		CFusionPlayerTestAppDocument
 */
class CFusionPlayerTestAppDocument : public CAknDocument
    {
public:

 /**
  *Function:	NewL
  *
  *Discussion:  Construct a CFusionPlayerTestAppDocument for the AVKON application
  *				aApp using two phase construction, and return a pointer to
  *				the created object
  *
  *Param:		aApp -  application creating this document
  *
  *Returns:		a pointer to the created instance of CFusionPlayerTestAppDocument
  *
  */
    static CFusionPlayerTestAppDocument* NewL(CEikApplication& aApp);

 /**
  *Function:	NewLC
  *
  *Discussion:	Construct a CFusionPlayerTestAppDocument for the AVKON application
  *				aApp using two phase construction, and return a pointer to the
  *				created object. Pointer to object is left on the cleanup stack
  *
  *Param:		aApp application creating this document
  *
  *Returns:		a pointer to the created instance of CFusionPlayerTestAppDocument
  *
  */
    static CFusionPlayerTestAppDocument* NewLC(CEikApplication& aApp);

 /**
  *Function:	~CFusionPlayerTestAppDocument
  *
  *Discussion:	 Destroy the object and release all memory objects
  *
  */
    ~CFusionPlayerTestAppDocument();

 /**
  *Function:	CreateAppUiL
  *
  *Discussion:	Create a CFusionPlayerTestAppAppUi object and return a pointer to it
  *Returns :	a pointer to the created instance of the AppUi created
  */
    CEikAppUi* CreateAppUiL();
private:

 /**
  *Function:	ConstructL
  *
  *Discussion:	Perform the second phase construction of a CFusionPlayerTestAppDocument object
  */
    void ConstructL();

 /**
  *Function:	CFusionPlayerTestAppDocument
  *
  *Discussion:	Perform the first phase of two phase construction
  *
  *Param:		aApp application creating this document
  */
    CFusionPlayerTestAppDocument(CEikApplication& aApp);

    MMPXViewUtility* iViewUtility;
    
    };


#endif // FUSIONPLAYERTESTAPPDOCUMENT_H__
