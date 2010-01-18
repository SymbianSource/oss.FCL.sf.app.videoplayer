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
* Description:    NSeries schedule plugin definition*
*/




#ifndef VCXNSSCHEDULEPLUGIN_H
#define VCXNSSCHEDULEPLUGIN_H

// INCLUDES
#include <mpxaknviewplugin.h>

// CLASS DECLARATION

/**
 *  Schedule view plugin definition.
 *
 *  @lib vcxnsscheduleplugin.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CVcxNsSchedulePlugin ) : public CMPXAknViewPlugin
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CVcxNsSchedulePlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CVcxNsSchedulePlugin();

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
    CVcxNsSchedulePlugin();

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL();
    };

#endif // VCXNSSCHEDULEPLUGIN_H
