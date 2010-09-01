/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Resource file loader for components that cannot use the*
*/




#include <e32base.h>
#include <bautils.h>
#include <coemain.h>
#include "IptvDebug.h"
#include "CIptvResourceLoader.h"

_LIT( KIptvEDrive, "E:" );
_LIT( KIptvFDrive, "F:" );
_LIT( KIptvZDrive, "Z:" );
_LIT( KIptvRsc, ".RSC" );
_LIT( KIptvRxx, ".R" );
const TInt KIptvLangCodeWidth2( 2 );
const TInt KIptvLangExtLength( 10 );

// ---------------------------------------------------------
// CIptvResourceLoader::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CIptvResourceLoader* CIptvResourceLoader::NewL( CCoeEnv& aCoeEnv )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvResourceLoader::NewL" );

    CIptvResourceLoader* self = new( ELeave ) CIptvResourceLoader( aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CIptvResourceLoader::CIptvResourceLoader
// C++ default constructor
// ---------------------------------------------------------
//
CIptvResourceLoader::CIptvResourceLoader( CCoeEnv& aCoeEnv )
  : iCoeEnv( aCoeEnv )
    {
    // None
    }

// ---------------------------------------------------------
// CIptvResourceLoader::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvResourceLoader::ConstructL()
    {    
    // None
    }

// ---------------------------------------------------------
// CIptvResourceLoader::~CIptvResourceLoader
// Destructor
// -----------------------------------------------------------------------------
//
CIptvResourceLoader::~CIptvResourceLoader()
    {        
    IPTVLOGSTRING_LOW_LEVEL( "CIptvResourceLoader::~CIptvResourceLoader" );
    
    if ( iAdded )
        {
        iCoeEnv.DeleteResourceFile( iOffset );    
        }
    }

// ---------------------------------------------------------
// CIptvResourceLoader::AddResourceL
// 
// ---------------------------------------------------------
//
void CIptvResourceLoader::AddResourceL(
    const TDesC& aResourceFile )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvResourceLoader::AddResourceL()" );

    // Language
    TLanguage language = User::Language();
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvResourceLoader::AddResourceL(), language: %d", language );
    TBuf<KIptvLangExtLength> langExt( KNullDesC );
    if ( language <= ELangOther ) // Langs 0 - 99
        {
        langExt.NumFixedWidth( language, EDecimal, KIptvLangCodeWidth2 );
        }
    else // Languages 100 - ELangMaximum
        {
        langExt.Num( language, EDecimal );
        }

    // File parser 
    TParse* fp = new( ELeave ) TParse();
    CleanupStack::PushL( fp );
    User::LeaveIfError( fp->Set( aResourceFile, NULL, NULL ) );

    // Search with language code
    TFileName file( fp->Path() );
    file.Append( fp->Name() );
    file.Append( KIptvRxx );
    file.Append( langExt );
    TInt error( SearchResourceFile( file ) );

    // If no language file found, change ext to .RSC
    if ( error )
        {
        file.Copy( fp->Path() );
        file.Append( fp->Name() );
        file.Append( KIptvRsc );
        file.ZeroTerminate();
        error = SearchResourceFile( file );
        }

    // See result
    if ( error )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvResourceLoader::AddResourceL(), NO RESOURCE FILE ADDED!" );
        iAdded = EFalse;
        }                   
    else
        {
        iAdded = ETrue;
        }
    
    CleanupStack::PopAndDestroy( fp );
    User::LeaveIfError( error );
    }

// ---------------------------------------------------------
// CIptvResourceLoader::SearchResourceFile
// 
// ---------------------------------------------------------
//
TInt CIptvResourceLoader::SearchResourceFile( const TDesC& aFile )
    {
    // Try to load resource file without drive letter
    TInt error( AddResourceFile( aFile ) );

    if ( error )
        {
        // Try to load resource file from E-drive
        TFileName eDrive( KIptvEDrive );
        eDrive.Append( aFile );
        error = AddResourceFile( eDrive );
        }

    if ( error )
        {
        // Try to load resource file from F-drive
        TFileName fDrive( KIptvFDrive);
        fDrive.Append( aFile );
        error = AddResourceFile( fDrive );
        }

    if ( error )
        {
        // Try to load resource file from Z-drive
        TFileName zDrive( KIptvZDrive);
        zDrive.Append( aFile );
        error = AddResourceFile( zDrive );
        }

    // If language variant was not found, next try to find nearest one
    if ( error )
        {
        TFileName tempFile( aFile );
        BaflUtils::NearestLanguageFile( iCoeEnv.FsSession(), tempFile );
        // Did BaflUtil change the file
        if ( tempFile.Compare( aFile ) != 0 )
            {
            error = AddResourceFile( tempFile );
            }
        }
    
    return error;
    }

// ---------------------------------------------------------
// CIptvResourceLoader::AddResourceFile
// 
// ---------------------------------------------------------
//
TInt CIptvResourceLoader::AddResourceFile( const TDesC& aFile )
    {
    TInt error( KErrNotFound );
    if ( BaflUtils::FileExists( iCoeEnv.FsSession(), aFile ) )
        {
        TRAP( error, iOffset = iCoeEnv.AddResourceFileL( aFile ) );
        if ( error == KErrNone )
            {
            IPTVLOGSTRING2_LOW_LEVEL( "CIptvResourceLoader::AddResourceFile(), Added: %S", &aFile );    
            }
        }    

    return error;
    }
            
