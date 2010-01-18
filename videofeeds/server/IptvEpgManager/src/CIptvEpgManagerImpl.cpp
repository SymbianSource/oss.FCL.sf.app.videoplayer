/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include <e32base.h>
#include <bautils.h>
#include "IptvDebug.h"
#include "CIptvEpgDatabase.h"
#include "CIptvEpgLiveTvCallbackImpl.h"
#include "IptvLiveEpgConstants.h"
#include "CIptvEpgDb.h"
#include "CIptvEpgManagerImpl.h"
#include "CIptvServer.h"
#include "CIptvEpgSession.h"
#include "CIptvServiceManager.h"
#include "CIptvEpgVodMsqQueue.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvEpgManagerImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvEpgManagerImpl::ConstructL()
    {       
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerImpl::ConstructL");

    User::LeaveIfError( iServer.iServiceManager->RegisterObserver( this ) );
    User::LeaveIfError( iFs.Connect() );
    }

// ---------------------------------------------------------
// CIptvEpgManagerImpl::NewL
// Two-phased constructor.
// Create instance of concrete interface implementation
// ---------------------------------------------------------
//
CIptvEpgManagerImpl* CIptvEpgManagerImpl::NewL(CIptvServer& aServer)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerImpl::NewL");  

    CIptvEpgManagerImpl* self = new(ELeave) CIptvEpgManagerImpl(aServer);
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvEpgManagerImpl::~CIptvEpgManagerImpl
// Destructor
// ---------------------------------------------------------
//
CIptvEpgManagerImpl::~CIptvEpgManagerImpl()
    {	
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerImpl::~CIptvEpgManagerImpl");  

    iServer.iServiceManager->DeRegisterObserver(this);
    iEpgDbArray.ResetAndDestroy();
    iFs.Close();
    }

// ---------------------------------------------------------
// CIptvEpgManagerImpl::CIptvEpgManagerImpl
// C++ default constructor
// ---------------------------------------------------------
//
CIptvEpgManagerImpl::CIptvEpgManagerImpl(CIptvServer& aServer) : 
    iServer( aServer )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerImpl::CIptvEpgManagerImpl");
    }

// ---------------------------------------------------------
// CIptvEpgManagerImpl::CreateSession
// 
// ---------------------------------------------------------
// 
CIptvEpgSession* CIptvEpgManagerImpl::CreateSessionL(TUint32 aServiceId, CIptvEpgVodMsqQueue* aMsgQueue, TInt& aError, TBool aPluginCanRun)
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerImpl::CreateSession");  
  
    CIptvEpgSession* session = NULL;
    CIptvService*    service = NULL;

    service = iServer.iServiceManager->GetServiceL( aServiceId, ETrue /* do open/close */ );

    if (service)
        {
        CIptvService::TServiceType serviceType = service->GetType();
        delete service;

        CDbHolder* dbHolder = NULL;

        switch (serviceType)
            {
            case CIptvService::EVod:
            case CIptvService::EVodCast:
            case CIptvService::ESearch:
            case CIptvService::EServiceGroup:
                {
                dbHolder = GetDatabaseHolderL( aServiceId, ETrue );
                session = CIptvEpgSession::NewL(dbHolder->iEpgDb, aServiceId, aMsgQueue, iServer, aPluginCanRun, aError);
                }
                break;
            case CIptvService::ELiveTv:
                {
                dbHolder = GetDatabaseHolderL( aServiceId, EFalse );
                session = CIptvEpgSession::NewL(dbHolder->iLiveTvDb, aServiceId, aMsgQueue, iServer, aPluginCanRun, aError);
                }
                break;                        
            default:
                User::Leave( KErrNotSupported );
                break;
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    return session;
    }

// --------------------------------------------------------------------------
// CIptvEpgManagerImpl::GetDatabaseHolderL
// 
// --------------------------------------------------------------------------
// 
CIptvEpgManagerImpl::CDbHolder* CIptvEpgManagerImpl::GetDatabaseHolderL(
    TUint32 aServiceId,
    TBool aEpgDb )
    {
    //  Try to find first
    for (TInt i = 0; i < iEpgDbArray.Count(); i++)
        {
        CIptvEpgManagerImpl::CDbHolder* dbHolder = iEpgDbArray[i];
        if (dbHolder->iServiceId == aServiceId)
            {
            dbHolder->iReferenceCount++;
            return dbHolder;
            }
        }

    //  Create holder for the db
    CIptvEpgManagerImpl::CDbHolder* dbHolder = new (ELeave) CDbHolder();
    CleanupStack::PushL( dbHolder );
    iEpgDbArray.AppendL( dbHolder );
    CleanupStack::Pop( dbHolder );
    dbHolder->iServiceId = aServiceId;
    dbHolder->iReferenceCount = 1;

    //  Create db
    if (aEpgDb)
        {
        TBuf<KIptvMaxPath> path;
        CIptvUtil::GetPathL( iFs, EIptvPathEcg, path );
        path.Append(KIptvEpgDbName);
        TBuf<KIptvServiceIdMaxLength> service;
        service.Num( aServiceId );
        path.Append( service );

        dbHolder->iEpgDb = CIptvEpgDb::NewL( path, aServiceId, iServer );
        }
    else
        {
        TBuf<KIptvMaxPath> path;
        CIptvUtil::GetPathL( iFs, EIptvPathTmpLive, aServiceId, path );
        path.Append( KLiveTvEPGDatabaseName );
        
        // if path exists delete the file. This is done to make sure
        // update is done to a clear database.
        if ( BaflUtils::FileExists( iFs, path ) )
        	{
        	TInt error = BaflUtils::DeleteFile( iFs, path );
			if ( error != KErrNone )
				{
				IPTVLOGSTRING2_LOW_LEVEL("Failed to delete live database file: %d", error);  	
				}
		    
        	}
        dbHolder->iLiveTvDb = CIptvEpgDatabase::NewL( path );
        }

    return dbHolder;
    }

// ---------------------------------------------------------
// CIptvEpgManagerImpl::CloseSession
// 
// ---------------------------------------------------------
// 
void CIptvEpgManagerImpl::CloseSession(TUint32 aServiceId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerImpl::CloseSession");  

    //  Find the database
    for (TInt i = 0; i < iEpgDbArray.Count(); i++)
        {
        CIptvEpgManagerImpl::CDbHolder* dbHolder = iEpgDbArray[i];
        if (dbHolder->iServiceId == aServiceId)
            {
            //  Decrease ref count and if it goes to zero, delete it
            dbHolder->iReferenceCount--;
            if (dbHolder->iReferenceCount < 1)
                {
                iEpgDbArray.Remove( i );
                delete dbHolder;
                }
            }
        }
    }

// ---------------------------------------------------------
// CIptvEpgManagerImpl::HandleSmEvent
// 
// ---------------------------------------------------------
// 
void CIptvEpgManagerImpl::HandleSmEvent(CIptvSmEvent& aEvent)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerImpl::ServiceModified");

    // Delete epg-database. There is no session with aServiceId when this mehtod is called.
    // -> We can delete database directly.

    if (aEvent.iEvent == CIptvSmEvent::EServiceModified)
        {
        RFs fs;
        TInt error = fs.Connect();
        if (error == KErrNone)
            {        
            TBuf<KIptvMaxPath> path;
            TRAP_IGNORE(CIptvUtil::GetPathL(fs, EIptvPathEcg, path, EDriveC, EFalse));
        
            path.Append(KIptvEpgDbName);
        
            TBuf<KIptvServiceIdMaxLength> service;
            service.Num(aEvent.iServiceId);
            path.Append(service);
        
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgManagerImpl::ServiceModified --> deleting: %S", &path);
        
            // If exist, delete epg database
            if (BaflUtils::FileExists(fs, path))
                {    
                BaflUtils::DeleteFile(fs, path);
                }      
          
            fs.Close();
            }
        }
    }

CIptvEpgManagerImpl::CDbHolder::~CDbHolder()
    {
    delete iEpgDb;
    delete iLiveTvDb;
    }
