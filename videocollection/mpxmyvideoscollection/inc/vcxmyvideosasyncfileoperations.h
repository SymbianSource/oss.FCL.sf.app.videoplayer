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
* Description:    Implementation for MPX My Videos Collection Move, Copy and Delete operations.*
*/




#ifndef VCXMYVIDEOSASYNCFILEOPERATIONS_H
#define VCXMYVIDEOSASYNCFILEOPERATIONS_H

// INCLUDES

#include <mpxcollectionplugin.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>

// FORWARD DECLARATIONS
class CVcxMyVideosCollectionPlugin;

// CONSTANTS

// CLASS DECLARATION

/**
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosAsyncFileOperations) :
    public CBase
    {    
    public: // Constructors and destructor

        /**
        * Two-phased constructor
        * @return object constructed
        */
        static CVcxMyVideosAsyncFileOperations* NewL( CVcxMyVideosCollectionPlugin& aCollection );

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosAsyncFileOperations();

    public: // from base classes
                 
    private:
        /**
        * Constructor
        */
        CVcxMyVideosAsyncFileOperations( CVcxMyVideosCollectionPlugin& aCollection );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL ();

    public:
    
        /**
        * Deletes the file and removes media item from the collection.
        *
        * @param aMdsId        MDS id of the item to be deleted.
        * @param aForce        If ETrue, the video will be deleted even if download is ongoing.
        */
        void DeleteVideoL( TUint32 aMdsId, TBool aForce = EFalse );
                
        /**
        * Performs one Move or Copy operation. Called by HandleStep.
        *
        * @return MVcxMyVideosActiveTaskObserver::EDone if was last operation,
        *         MVcxMyVideosActiveTaskObserver::EMoreToCome otherwise.
        */
        MVcxMyVideosActiveTaskObserver::TStepResult HandleMoveOrCopyStepL();

        /**
        * Moves or copies video to another drive.
        *
        * @param aMdsId        MDS id of the item to be moved.
        * @param aTargetDrive  Target drive.
        * @param aMove         If ETrue, the source file is deleted.
        */
        void MoveOrCopyVideoL( TUint32 aMdsId, TInt aTargetDrive, TBool aMove );
                       
        /**
        * Generates and sends resp message for Move,Copy and Delete operations.
        * Does nothing if there is some other operation going on.
        * This function is called when operation is interrupted from a leave or
        * cancel request.
        * Leaves with KErrNotFound if no operation was in progress.
        *
        * @param aErr  Error code which is set for non handled items in the resp array.
        */
        void CancelOperationL( TInt aErr );
 
        /**
        * Performs one Delete operation. Called by HandleStep.
        *
        * @return MVcxMyVideosActiveTaskObserver::EDone if was last operation,
        *         MVcxMyVideosActiveTaskObserver::EMoreToCome otherwise.
        */
        MVcxMyVideosActiveTaskObserver::TStepResult HandleDeleteStepL();

    private:
        
        /**
        * Called when file is going to be moved or copied to aDrive.
        * Checks if target drive has enough free space.
        *
        * @param aPath  File path of the file which is going to be moved or copied.
        * @param aDrive Target drive.
        * @return ETrue If target drive has enough space, EFalse otherwise.
        */
        TBool DriveHasEnoughFreeSpaceL( const TDesC& aPath, TInt aDrive );

        /**
        * Sends response array for Move, Copy or Delete operation.
        *
        * @param aMessageId  Response message ID.
        */
        void SendOperationRespL( TInt aCmdId );

        /**
        * Generates target path for move and copy operations.
        *
        * @param aSourcePath  Full source path.
        * @param aTargetPath  Target path is written here.
        * @param aTargetDrive Target drive.
        */
        void GenerateTargetPathForMoveOrCopyL(
                const TDesC& aSourcePath, TDes& aTargetPath, TInt aTargetDrive );

    public:

        /**
        * Array containing item ids which are being moved/copied or deleted.
        */
        RArray<TUint32> iOperationIdArray;
    
        /**
        * Array containing results for move/copy/delete operations. Array positions
        * are in sync with iOperationIdArray.
        */
        RArray<TInt> iOperationResult;
                
    private:

        /**
        * File server session.
        */
        RFs iFs;
            
        /**
        * The item currently being moved/copied or deleted.
        */
        TInt iCurrentOperationIndex;
                        
        /**
        * Target drive for Move or Copy operation.
        */
        TInt iTargetDrive;

        /**
        * Owner of this class.
        */
        CVcxMyVideosCollectionPlugin& iCollection;
                                
    };

#endif   // VCXMYVIDEOSASYNCFILEOPERATIONS_H


