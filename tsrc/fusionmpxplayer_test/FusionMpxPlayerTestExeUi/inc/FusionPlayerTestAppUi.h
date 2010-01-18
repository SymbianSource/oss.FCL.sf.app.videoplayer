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


#ifndef FUSIONPLAYERTESTAPPAPPUI_H
#define FUSIONPLAYERTESTAPPAPPUI_H

#include <aknviewappui.h>
#include <aknnavi.h>

#include "FusionMpxPlayerTestCmdListener.h"

/**
 * Class:		CFusionPlayerTestAppAppUi
 */
class CFusionPlayerTestAppAppUi : public CAknViewAppUi,
                                  public MFusionMpxPlayerTestShutdownObserver
    {
public:
	/**
	 * Function : ConstructL
	 *
	 * Discussion : Performs the second phase construction of a
	 *				CFusionPlayerTestAppAppUi object	this needs to be public due to
	 *				the way the framework constructs the AppUi
	 */
    void ConstructL();

	/**
	 * Function :	CFusionPlayerTestAppAppUi
	 *
	 * Discussion : Perform the first phase of two phase construction.
	 *				This needs to be public due to the way the framework
	 *				constructs the AppUi
	 */
    CFusionPlayerTestAppAppUi();

	/**
	 * Function :	~CFusionPlayerTestAppAppUi
	 *
	 * Discussion :	Hidden virtual destructor. Destroys object and releases
	 *				all associated memory.
	 *
	 */
    ~CFusionPlayerTestAppAppUi();


public:

    // from CEikAppUi
	/**
	 * Function :
	 *
	 * Discussion :	Handle user menu selections
	 *
	 * Params :		aCommand - the enumerated code for the option selected
	 *
	 * Returns :
	 *
	 */
	void HandleCommandL(TInt aCommand);


	// From MFusionMpxPlayerTestShutdownObserver
	void ShutdownCommandReceived();
	
private:

    CFusionMpxPlayerTestCmdListener* iCmdListener;

    TInt iPSKeyBase;
	};

#endif // FUSIONPLAYERTESTAPPAPPUI_H

