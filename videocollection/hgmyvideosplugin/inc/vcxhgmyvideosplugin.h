/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CVcxHgMyVideosPlugin class definition*
*/



#ifndef VCXHGMYVIDEOSPLUGIN_H
#define VCXHGMYVIDEOSPLUGIN_H

// INCLUDE FILES
#include <mpxaknviewplugin.h>

// CLASS DECLARATION

/**
 * 
 *
 *  @lib vcxhgmyvideosplugin.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosPlugin ) : public CMPXAknViewPlugin
    {
public:

    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
    static CVcxHgMyVideosPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CVcxHgMyVideosPlugin();

private:

    /**
     * From CMPXAknViewPlugin. Construct Avkon view.
     *
     * @return Pointer to a newly created Avkon view.
     */
    CAknView* ConstructViewLC();

private:

    /**
     * C++ default constructor.
     */
    CVcxHgMyVideosPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif // VCXHGMYVIDEOSPLUGIN_H
