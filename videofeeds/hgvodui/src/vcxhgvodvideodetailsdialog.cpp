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
* Description:    HG VOD*
*/




// INCLUDE FILES

#include <aknmessagequerydialog.h>
#include <StringLoader.h>
#include <vcxhgvodui.rsg>
#include "vcxnscontent.h"
#include "vcxhgvodvideodetailsdialog.h"

// CONSTANTS
const TInt KVcxNsExtraWhiteSpace ( 2 );
const TInt KVcxNsSecondsInMinute ( 60 );
const TInt KVcxNsSecondsInHour (3600 );
const TInt KVcxNsVideoSizeGB ( 0x40000000 );
const TInt KVcxNsVideoSizeHalfGB ( 0x20000000 );
const TInt KVcxNsVideoSizeMB ( 0x100000 );
const TInt KVcxNsVideoSizeHalfMB ( 0x80000 );
const TInt KVcxNsVideoSizeKB ( 0x400 );
const TInt KVcxNsVideoSizeHalfKB ( 0x200 );
_LIT( KVcxNsEnterString, "\n" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TVcxHgVodVideoDetailsDialog::TVcxHgVodVideoDetailsDialog()
// ---------------------------------------------------------------------------
//
TVcxHgVodVideoDetailsDialog::TVcxHgVodVideoDetailsDialog()
    {
    }

// ---------------------------------------------------------------------------
// TVcxHgVodVideoDetailsDialog::ShowVideoDetailsDialogL()
// ---------------------------------------------------------------------------
//
void TVcxHgVodVideoDetailsDialog::ShowVideoDetailsDialogL(CVcxNsContent& aContent)
    {
    TUint32     totalLength         = 0;
    TUint       dispSize            = 0;
    HBufC*      videoName           = NULL;
    HBufC*      videoLanguage       = NULL;
    HBufC*      videoLength         = NULL;
    HBufC*      videoSize           = NULL;
    HBufC*      videoDescription    = NULL;
    HBufC*      videoAuthor         = NULL;
    HBufC*      videoCopyright      = NULL;    
    
    //Name
    if ( aContent.GetName().Length() > 0 )
        {
        videoName = StringLoader::LoadLC( R_VCXHGVOD_VIDEO_DETAILS_NAME, aContent.GetName() );
        totalLength += videoName->Length() + KVcxNsExtraWhiteSpace;
        }
    
    //Language
    if ( aContent.GetLanguage().Length() > 0 )
        {
        videoLanguage = StringLoader::LoadLC( R_VCXHGVOD_VIDEO_DETAILS_LANGUAGE, aContent.GetLanguage());
        totalLength += videoLanguage->Length() + KVcxNsExtraWhiteSpace;        
        }
    
    //Length
    if ( aContent.GetLength() > 0 )
        {
        TUint32 length = aContent.GetLength();
        TInt hours = length / KVcxNsSecondsInHour;
        TInt minutes = ( length - ( hours * KVcxNsSecondsInHour ) ) / KVcxNsSecondsInMinute;

        if ( hours == 0 && minutes == 0 )
            {
            minutes = 1;
            }
                
        CArrayFix<TInt>* numbers = new (ELeave) CArrayFixFlat<TInt>(2);
        CleanupStack::PushL( numbers );
        numbers->AppendL( hours );
        numbers->AppendL( minutes );
        videoLength = StringLoader::LoadL( R_VCXHGVOD_VIDEO_DETAILS_LENGTH, *numbers );
        CleanupStack::PopAndDestroy( numbers );
        CleanupStack::PushL( videoLength );
        totalLength += videoLength->Length() + KVcxNsExtraWhiteSpace;
        }
    
    //Size
    if ( aContent.GetSize() > 0 )
        {
        TUint32 size = aContent.GetSize();
        if ( size >= KVcxNsVideoSizeGB )
            {
            dispSize = ((TUint)size) + KVcxNsVideoSizeHalfGB;
            dispSize /= KVcxNsVideoSizeGB;
            videoSize = StringLoader::LoadLC( R_VCXHGVOD_VIDEO_DETAILS_SIZE_GB, dispSize );
            }
        else if ( size >= KVcxNsVideoSizeMB )
            {
            dispSize = ((TUint)size) + KVcxNsVideoSizeHalfMB;
            dispSize /= KVcxNsVideoSizeMB;
            videoSize = StringLoader::LoadLC( R_VCXHGVOD_VIDEO_DETAILS_SIZE_MB, dispSize );
            }
        else
            {
            dispSize = ((TUint)size) + KVcxNsVideoSizeHalfKB;
            dispSize /= KVcxNsVideoSizeKB;
            videoSize = StringLoader::LoadLC( R_VCXHGVOD_VIDEO_DETAILS_SIZE_KB, dispSize );
            }

        totalLength += videoSize->Length() + KVcxNsExtraWhiteSpace;
        }
    
    //Description
    if ( aContent.GetDescription().Length() > 0 )
        {
        videoDescription = StringLoader::LoadLC( R_VCXHGVOD_VIDEO_DETAILS_DESCRIPTION, aContent.GetDescription());
        totalLength += videoDescription->Length() + KVcxNsExtraWhiteSpace;        
        }
    
    //Author
    if ( aContent.GetAuthor().Length() > 0 )
        {
        videoAuthor = StringLoader::LoadLC( R_VCXHGVOD_VIDEO_DETAILS_AUTHOR, aContent.GetAuthor());
        totalLength += videoAuthor->Length() + KVcxNsExtraWhiteSpace;        
        }    
    
    //Copyright
    if ( aContent.GetCopyright().Length() > 0 )
        {
        videoCopyright = StringLoader::LoadLC( R_VCXHGVOD_VIDEO_DETAILS_COPYRIGHT, aContent.GetCopyright());
        totalLength += videoCopyright->Length() + KVcxNsExtraWhiteSpace;        
        }
    
    HBufC* context = HBufC::NewLC( totalLength );
    if ( videoName )
        {
        context->Des().Append( *videoName );
        context->Des().Append( KVcxNsEnterString );
        }
    if ( videoLength )
        {
        context->Des().Append( *videoLength );
        context->Des().Append( KVcxNsEnterString );
        }
    if ( videoLanguage )
        {
        context->Des().Append( *videoLanguage );
        context->Des().Append( KVcxNsEnterString );
        }
    if ( videoSize )
        {
        context->Des().Append( *videoSize );
        context->Des().Append( KVcxNsEnterString );
        }
    if ( videoDescription )
        {
        context->Des().Append( *videoDescription );
        context->Des().Append( KVcxNsEnterString );
        }
    if ( videoAuthor )
        {
        context->Des().Append( *videoAuthor );
        context->Des().Append( KVcxNsEnterString );
        }
    if ( videoCopyright )
        {
        context->Des().Append( *videoCopyright );
        context->Des().Append( KVcxNsEnterString );
        }
    
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *context );
    dlg->ExecuteLD(R_VCXHGVOD_VIDEO_DETAILS_MESSAGE_QUERY);
    
    CleanupStack::PopAndDestroy( context );
    
    if ( videoCopyright )
        {
        CleanupStack::PopAndDestroy( videoCopyright );
        }
    if ( videoAuthor )
        {
        CleanupStack::PopAndDestroy( videoAuthor );
        }
    if ( videoDescription )
        {
        CleanupStack::PopAndDestroy( videoDescription );
        }
    if ( videoSize )
        {
        CleanupStack::PopAndDestroy( videoSize );
        }    
    if ( videoLength )
        {
        CleanupStack::PopAndDestroy( videoLength );
        }
    if ( videoLanguage )
        {
        CleanupStack::PopAndDestroy( videoLanguage );
        }    
    if ( videoName )
        {
        CleanupStack::PopAndDestroy( videoName );
        }
    }
