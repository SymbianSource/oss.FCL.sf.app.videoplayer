/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Data class for external folder information.*
*/




#ifndef IPTVEXTERNALFOLDERINFO_H
#define IPTVEXTERNALFOLDERINFO_H

// CLASS DECLARATION

/**
* Data class for storing information about Video Center external folder.
*/
class TIptvExternalFolderInfo
    {
    public: // Data type declarations.

        /**
         * Enumeration to define how external folders should be treated
         * when following them for added video and thumbnail files.
         */
        enum TIptvExternalFolderType
            {
            EIptvUndefined         = 0, // Not defined.
            EIptvRootLevelOnly     = 1, // Only root level is scanned, no sub directories.
            EIptvSubFoldersOnly    = 2, // Only subdirectories are scanned.
            EIptvRootAndSubFolders = 3  // Both root level and subdirectories are scanned.
            };

    public: // Member variables.

        /**
         *
         */
        TBuf<KMaxPath> iPath;

        /**
         *
         */        
        TIptvExternalFolderType iType;

        /**
         *
         */        
        TBool iPermanent;

        /**
         *
         */        
        TBool iGalleryFolder;
        
        /**
         *
         */        
        TBool iDragFoldersScanned;
    };

#endif // IPTVEXTERNALFOLDERINFO_H

// End of file.
