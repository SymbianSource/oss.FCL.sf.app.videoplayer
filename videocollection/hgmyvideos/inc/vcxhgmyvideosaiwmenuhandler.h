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
* Description:  A class to manage AIW provided menu items*
*/


#ifndef VCXHGMYVIDEOSAIWMENUHANDLER_H
#define VCXHGMYVIDEOSAIWMENUHANDLER_H

#include <e32std.h>

class CAiwServiceHandler;
class CEikMenuPane;
class CMediaRecognizer;
class CAiwGenericParamList;

/**
 *  A class to manage AIW provided menu items
 *
 */
NONSHARABLE_CLASS(CHgMyVideosAiwMenuHandler) : public CBase
    {
    public:

        CHgMyVideosAiwMenuHandler();

        virtual ~CHgMyVideosAiwMenuHandler();

        /**
         * Attach AIW menu items to a menu pane.
         *
         * @param aMenuPaneId    Menu pane ID where to add AIW menu items
         */
        void AttachMenuL( TInt aMenuPaneId );

        /**
         * Detach AIW menu items from a menu pane.
         *
         * @param aMenuPaneId    Menu pane ID where to add AIW menu items
         */
        void DetachMenu( TInt aMenuPaneId );

        /**
         * Pass a submenu to AIW for dynamic initialization.
         *
         * @param aMenuPane      Menu pane where to add AIW menu items
         * @return               ETrue if this menu was handled by AIW
         */
        TBool TryHandleSubmenuL( CEikMenuPane* aMenuPane );

        /**
         * Initialize an AIW provided menu pane.
         *
         * @param aResourceId    Resource ID for this menu
         * @param aMenuPane      Menu pane ID where to add AIW menu items
         * @param aPath          Full file path
         * @param aMimeType      MIME type or KNullDesC for autodetect
         */
        void InitializeMenuPaneL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane,
            const TDesC& aPath,
            const TDesC& aMimeType );

        /**
         * Check if a given command ID is for an AIW command
         *
         * @param aCommand       Command ID
         * @return               ETrue if this is an AIW command
         */
        TBool IsAiwCommand( TInt aCommand );

        /**
         * Execute an AIW command
         *
         * @param aCommand       Command ID
         * @param aPath          Full file path
         * @param aMimeType      MIME type for track or KNullDesC for autodetect
         */
        void HandleAiwCommandL(
            TInt aCommand,
            const TDesC& aPath,
            const TDesC& aMimeType );

        /**
        *  Closes any potential open AIW menus
        *
        */
        void CloseOpenAiwMenuL();
        
        /**
         *  Verify if AIW menu is active
         *
         * @return ETrue if AIW menu is active or EFalse
         */
        TBool IsMenuActive();        

    private:

        /**
        * Fill file path and MIME type generic parameters for AIW.
        */
        void FillAiwParamsL(
            CAiwGenericParamList& aAiwParamsList,
            const TDesC& aPath,
            const TDesC& aMimeType );

    private: // data

        /**
         * AIW service handler implementation.
         * Own.
         */
        CAiwServiceHandler* iAiwServiceHandler;

        /**
         * Media recognizer for autodetecting MIME types.
         * Own.
         */
        CMediaRecognizer* iMediaRecognizer;

        /**
         * Flag for active AIW menu, ETrue if active or EFalse if not
         * 
         */
        TBool     iMenuActive;
        
        /**
         * Active AIW command ID when executing AIW command
         */
        TInt      iCommand;
        
        /**
         * AIW parameter filepath string
         * 
         */
        HBufC*    iPath;
        
        /**
         * AIW parameter mimetype string
         * 
         */
        HBufC*    iMimeType;
    };

#endif // VCXHGMYVIDEOSAIWMENUHANDLER_H
