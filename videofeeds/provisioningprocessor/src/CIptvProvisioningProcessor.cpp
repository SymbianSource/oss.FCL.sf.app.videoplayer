/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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




#include <e32base.h>
#include "IptvDebug.h"
#include "CIptvUtil.h"
#include <bautils.h>
#include "CIptvProvisioningProcessor.h"
#include "CIptvPPContentHandler.h"

#include "CIptvServices.h"

_LIT8( KMimeType, "text/xml" );

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CIptvProvisioningProcessor::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningProcessor::ConstructL" );

    User::LeaveIfError( iFs.Connect() );
    iHandler = CIptvPPContentHandler::NewL( *this, iFs );
    iParser = CParser::NewL( KMimeType, *iHandler );
    }

// --------------------------------------------------------------------------
// Two-phased constructor.
// Create instance of concrete interface implementation.
// --------------------------------------------------------------------------
//
EXPORT_C CIptvProvisioningProcessor* CIptvProvisioningProcessor::NewL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningProcessor::NewL" );

    CIptvProvisioningProcessor* self =
        new( ELeave ) CIptvProvisioningProcessor();
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );

    return self;
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CIptvProvisioningProcessor::~CIptvProvisioningProcessor()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvProvisioningProcessor::~CIptvProvisioningProcessor" );

    delete iParser;
    delete iHandler;
    delete iXmlFileContent;
    iXmlFileContent = NULL;
    iFs.Close();
    }

// --------------------------------------------------------------------------
// C++ default constructor.
// --------------------------------------------------------------------------
//
CIptvProvisioningProcessor::CIptvProvisioningProcessor() :
    iHandler( NULL ),
    iParser( NULL ),
    iXmlFileContent( NULL ),
    iLocal( ETrue )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvProvisioningProcessor::CIptvProvisioningProcessor" );
    }

// --------------------------------------------------------------------------
// CIptvProvisioningProcessor::StopParsing
// --------------------------------------------------------------------------
//
void CIptvProvisioningProcessor::StopParsing()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningProcessor::StopParsing" );

    TRAPD( error, iParser->ParseEndL() );
    if ( KErrNone != error )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvProvisioningProcessor::StopParsing --- ParseEndL leaved %d", error );
        }
    }

// --------------------------------------------------------------------------
// CIptvProvisioningProcessor::ExternalProvisionL
// --------------------------------------------------------------------------
//
EXPORT_C CIptvServices* CIptvProvisioningProcessor::ExternalProvisionL(
    RFile& aFile,
    TInt& aError )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvProvisioningProcessor::ExternalProvisionL" );

    iLocal = EFalse;
    iHandler->Reset();
    iHandler->SetMode( iLocal );

    CIptvServices* services = CIptvServices::NewL();
    CleanupStack::PushL( services );
    iHandler->SetServicesArray( services );

    TInt size = 0;
    User::LeaveIfError( aFile.Size( size ) );
    delete iXmlFileContent;
    iXmlFileContent = NULL;
    iXmlFileContent = HBufC8::NewL( size );
    TPtr8 fileContentPtr = iXmlFileContent->Des();
    User::LeaveIfError( aFile.Read( fileContentPtr ) );
    TInt pos( 0 );
    aFile.Seek( ESeekStart, pos );

    if ( iXmlFileContent->Length() > 0 )
        {
        iParser->ParseBeginL();
        iParser->ParseL( iXmlFileContent->Des() );
        iParser->ParseEndL();
        }

    aError = iHandler->GetError();

    delete iXmlFileContent;
    iXmlFileContent = NULL;

    CleanupStack::Pop( services );
    return services;
    }

// --------------------------------------------------------------------------
// CIptvProvisioningProcessor::LocalProvisionL
// --------------------------------------------------------------------------
//
EXPORT_C CIptvServices* CIptvProvisioningProcessor::LocalProvisionL(
    const TDesC& aFileName,
    TInt& aError )
    {
    IPTVLOGSTRING2_LOW_LEVEL(
        "CIptvProvisioningProcessor::LocalProvisionL -- Provision file: %S",
        &aFileName );

    if ( !BaflUtils::FileExists( iFs, aFileName ) )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvProvisioningProcessor::File not found: %S", &aFileName );
        aError = KErrNotFound;
        return NULL;
        }

    iLocal = ETrue;
    iHandler->Reset();
    iHandler->SetMode( iLocal );

    CIptvServices* services = CIptvServices::NewL();
    CleanupStack::PushL( services );
    iHandler->SetServicesArray( services );

    RFile phile;
    TInt error( phile.Open( iFs, aFileName, EFileRead ) );

    if ( KErrNone != error )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvProvisioningProcessor::Provision file.Open error %d", error );
        aError = error;
        iHandler->Reset();
        iHandler->SetServicesArray( NULL );
        CleanupStack::PopAndDestroy( services );
        return NULL;
        }
    CleanupClosePushL( phile );
    TInt size = 0;
    User::LeaveIfError( phile.Size( size ) );
    delete iXmlFileContent;
    iXmlFileContent = NULL;
    iXmlFileContent = HBufC8::NewL( size );
    TPtr8 fileContentPtr = iXmlFileContent->Des();
    User::LeaveIfError( phile.Read( fileContentPtr ) );
    CleanupStack::PopAndDestroy( &phile ); // closes file

    if ( iXmlFileContent->Length() > 0 )
        {
        iParser->ParseBeginL();
        iParser->ParseL( iXmlFileContent->Des() );
        iParser->ParseEndL();
        }

    aError = iHandler->GetError();

    delete iXmlFileContent;
    iXmlFileContent = NULL;

    CleanupStack::Pop( services );

    return services;
    }

// --------------------------------------------------------------------------
// DLL entry point.
// --------------------------------------------------------------------------
//
#ifndef EKA2

GLDEF_C TInt E32Dll( TDllReason /* aReason */ )
    {
    return( KErrNone );
    }

#endif
