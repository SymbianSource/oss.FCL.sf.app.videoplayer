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
* Description:    NSeries settings plugin definition*
*/




#ifndef VCXNSSETTINGSPLUGIN_H
#define VCXNSSETTINGSPLUGIN_H

// INCLUDES
#include <mpxaknviewplugin.h>

// CLASS DECLARATION

/**
 *  Settings view plugin definition.
 *
 *  @lib vcxnssettingsplugin.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CVcxNsSettingsPlugin ) : public CMPXAknViewPlugin
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CVcxNsSettingsPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CVcxNsSettingsPlugin();

private:

    /**
     * From CMPXAknViewPlugin. Construct Avkon view.
     *
     * @return Pointer to a created Avkon view.
     */
    CAknView* ConstructViewLC();

private:

    /**
     * C++ default constructor.
     */
    CVcxNsSettingsPlugin();

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL();
    };

#endif // VCXNSSETTINGSPLUGIN_H
