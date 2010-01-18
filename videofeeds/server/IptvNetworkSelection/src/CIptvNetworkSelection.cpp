/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Handles IAP selection and connection permissions.*
*/




/*

IAP selection procedure:
------------------------

Return iCurrentIap if it's feasible:
  
  1. If Service has EReadOnlyIaps flag set and iCurrentIap is not found from iService
     IAPs, set iCurrentIapIsFeasible to EFalse and jump to section 4.
  2. If iCiAiMatch exists, return it. [EXIT]
  3. If iCurrentIap is EGprs or ECsd type, return it. [EXIT]
  4. Run WLAN scan.
  5. If iCurrentIapIsFeasible is ETrue
    5.1 Calculate iCiSwiMatch
    5.2 If iCiSwiMatch exists, return it. [EXIT]

iCurrentIap wasn't feasible, try to find from service IAPs list:

  6. Put service IAPs to iAvailableIaps list.
  7. Check if WLAN is available, (iActiveIaps has WLAN IAP which is not on service IAPs).
    7.1 If WLAN is not available remove all WLAN IAPS from iAvailableIaps list.
    7.2 Choose best from iAvailableIaps. [EXIT]
  8. Remove all WLAN IAPs from iAvailableIaps list.
  9. Add iSiSwiMatch to iAvailableIaps list.
 10. Choose highest priority from iAvailableIaps. [EXIT]   

Note1: In offline mode hidden WLAN IAPs from service IAPS and current iap
       are added to Scanned WLAN IAPs even if they are not found from the scan.

Note2: If phone is in offline mode, then all EGprs and ECsd IAPs are filtered out
       from iService IAPs and iCurrentIap.

Note3: iCurrentIap is not feasible if service has EReadOnlyIaps flag set and iCurrentIap
       is not found from service IAPs, ie iCiSiMatch does not contain value. 
*/

// INCLUDE FILES
#include "CIptvNetworkSelection.h"
#include "CIptvServer.h"
#include "CIptvServiceManager.h"
#include <cdbcols.h>
#include <WlanCdbCols.h>
#include "IptvDebug.h"
#include <commdb.h>
#include <bldvariant.hrh>

#include <es_enum.h> //TConnectionInfo
#include <in_sock.h>
#include <rconnmon.h>
#include <wlanmgmtcommon.h>

#include <CProfileChangeNotifyHandler.h>
#include <Profile.hrh>

#include <MProfileEngine.h>

#include <cmdestinationext.h>
#include <cmmanagerext.h>
#include <centralrepository.h>
#include <cmpluginwlandef.h> 

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

#ifdef __WINSCW__
const TInt KWlanScanSimulationTimerTime = 5000000; // 5 seconds
#endif

// MACROS

// LOCAL CONSTANTS AND MACROS

#if IPTV_LOGGING_METHOD != 0

_LIT(KIptvNsWlan, "EWlan");
_LIT(KIptvNsGprs, "EGprs");
_LIT(KIptvNsCsd, "ECsd");
_LIT(KIptvNsUnknown, "EUnknown");

#endif

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::CIptvNetworkSelection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvNetworkSelection::CIptvNetworkSelection( CIptvServer& aServer )
:CActive(EPriorityStandard), iServer(aServer)
    {
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::ConstructL()
    {

#ifdef __WINSCW__
    iWlanScanSimulationTimer = CIptvTimer::NewL( EPriorityStandard, *this );
#endif

    CActiveScheduler::Add( this );
    iGetUsedIapReqs.Reset();
    
		//Initialize iOffline
		MProfileEngine* engine = CreateProfileEngineL();
		CleanupReleasePushL( *engine );
		
    if ( engine )
        {
        if ( engine->ActiveProfileId() == EProfileOffLineId )
            {
            iOffline = ETrue;	
            }
        else
            {
            iOffline = EFalse;
            }
        }
			
    CleanupStack::PopAndDestroy( engine );  
        
    iProfChangeHandler = CProfileChangeNotifyHandler::NewL( this ); 
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvNetworkSelection* CIptvNetworkSelection::NewL( CIptvServer& aServer )
    {
    CIptvNetworkSelection* self = new( ELeave ) CIptvNetworkSelection( aServer );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CIptvNetworkSelection::~CIptvNetworkSelection()
    {
    Cancel();
    delete iActiveIaps;
    delete iScannedWlanIaps;
    delete iAvailableIaps;
    iGetUsedIapReqs.Close();
    iConnMon.Close();
#ifdef __WINSCW__
    delete iWlanScanSimulationTimer;
#endif
    delete iProfChangeHandler;
    delete iService;
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::SetUsedIap
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::SetUsedIapL( TUint32 aIapId,
                                         TUint32 aServiceId,
                                         TBool aSetToDefaultForService,
                                         TRespStatus& aRespStatus )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::SetUsedIapL() start");

    IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: setting %d as a current IAP in use.", aIapId);
    iCurrentIap.iId          = aIapId;
    iCurrentIap.iValueExists = ETrue;
     
    if ( aSetToDefaultForService )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: adding to services' (%d) iap list too", aServiceId);
        CIptvService* service = iServer.iServiceManager->GetServiceL( aServiceId, ETrue /* do open/close */ );
        if ( !service )
            {
            IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: service not found: %d", aServiceId);
            aRespStatus = EServiceNotFound;
            return;
            }
            
        CleanupStack::PushL( service );         // 1->
        
        TInt i;
        TBool zeroPrioExists   = EFalse;

        //try to delete iap if its there already
        TIptvIap iap;
        iap.iId       = aIapId;
        iap.iPriority = 0; //0 = the highest priority (king), prio has no effect on delete
        TInt err = service->iIapList->DeleteIap( iap );

        if ( err == KErrNone )
            {
            IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: There was already iap with id %d, deleted it", aIapId);
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: iap did not exist, didn't delete");
            }
                    
        //move up priorities if there is priority 0 iap already
        for ( i = 0; i < service->iIapList->Count(); i++ )
            {
            if ( service->iIapList->IapL(i).iPriority == 0 )
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: There was already prio 0");
                zeroPrioExists = ETrue;
                break;
                }
            }
        if ( zeroPrioExists )
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: adding +1 to all priorities");
            for ( i = 0; i < service->iIapList->Count(); i++ )
                {
                service->iIapList->IapL(i).iPriority += 1; 
                }            
            }
        
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: adding new IAP with prio 0");
        
        service->iIapList->AddIap( iap );
        iServer.iServiceManager->UpdateServiceL( *service );
        CleanupStack::PopAndDestroy( service ); // <-1
        }
            
    aRespStatus = ESucceeded;

    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::SetUsedIapL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::GetUsedIapReqL
// Called by Network Selection client
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::GetUsedIapReqL( TIptvGetUsedIapReq* aGetUsedIapReq )
    {
    UpdateServiceL( aGetUsedIapReq->iServiceId );
    if ( !iService )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: * No such service, aborting *");
        aGetUsedIapReq->iRespStatus = EServiceNotFound;
        return;
        }

    TInt destId( 0 );
    TBool invalidDest( EFalse );

    CRepository* cenRep = CRepository::NewLC( KIptvAlrCenRepUid );
    if ( cenRep->Get( KIptvCenRepUsedSnapIdKey, destId ) != KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: * KIptvCenRepUsedSnapIdKey not found *");
        aGetUsedIapReq->iRespStatus = static_cast<TUint8>( ENoSuitableIapFound );
        invalidDest = ETrue;
        }
    else if ( destId == KIptvCenRepAPModeAlwaysAsk )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: * KIptvCenRepUsedSnapIdKey always ask");
        aGetUsedIapReq->iRespStatus = static_cast<TUint8>( EFailedAlwaysAskSelected );
        invalidDest = ETrue;
        }

    CleanupStack::PopAndDestroy( cenRep );

    // Scan is useless, when always ask is selected.
    if ( invalidDest  && !( (iService->GetFlags() & CIptvService::EReadOnlyIaps ) ) )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: ALR//Always ask mode on or key not found");
        return;
        }

    if ( (iService->GetFlags() & CIptvService::EReadOnlyIaps) )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: * service has read only iaps *");
        }

    aGetUsedIapReq->iRespStatus = EProcessing;
    UpdateAllButWlanListsL( *aGetUsedIapReq );
    
    //informs client that response will arrive asyncrounously to callback function
    aGetUsedIapReq->iRespStatus = EDoingWlanScan;

    //add TIptvGetUsedIapReq to list
    iGetUsedIapReqs.AppendL( aGetUsedIapReq );
    
#ifdef __WINSCW__

    //use timer to simulate event receiving
    iWlanScanSimulationTimer->Cancel();
    iWlanScanSimulationTimer->After( KWlanScanSimulationTimerTime );

#else

    if ( !IsActive() )
        {
        UpdateWlanScannedIapsL(); //async
        }
#endif

    //Now we just wait call to RunL() (or TimerExpired() on WINSCW),
    //RunL() converts SSIDS to IAPS and calls HandleGetUsedIapRequests()

    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetUsedIapReqL() exit");
    return;
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::HandleGetUsedIapRequests
// Called by RunL() when wlan scan has finished
// -----------------------------------------------------------------------------
// 
void CIptvNetworkSelection::HandleGetUsedIapRequests()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::HandleGetUsedIapRequests() start");
    TInt i;
    for ( i = 0; i < iGetUsedIapReqs.Count(); i++ )
        {
        TRAPD( err, GetUsedIapAfterWlanScanL( *(iGetUsedIapReqs[i]) ) );
        if ( err == KErrNone )
            {
            //inform the request owner
            iGetUsedIapReqs[i]->iNsObserver->GetUsedIapResp( iGetUsedIapReqs[i] );
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: * failed to get used iap after wlan scan! *");
            iGetUsedIapReqs[i]->iRespStatus = static_cast<TUint8>(EGeneralError);
            iGetUsedIapReqs[i]->iNsObserver->GetUsedIapResp( iGetUsedIapReqs[i] );
            }
        }
    iGetUsedIapReqs.Close(); //delete request items
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::HandleGetUsedIapRequests() exit");
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::CancelGetUsedIapReq
// -----------------------------------------------------------------------------
// 
TInt CIptvNetworkSelection::CancelGetUsedIapReq( MIptvNetworkSelectionObserver* aObserver )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::CancelGetUsedIapReq() start");
    TInt i;
    
    if ( iGetUsedIapReqs.Count() == 0 )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: no requests pending.");
        return KErrNotFound;
        }
    
    TBool found = EFalse;    
    for ( i = 0; i < iGetUsedIapReqs.Count() && !found; i++ )
        {
        if ( iGetUsedIapReqs[i]->iNsObserver == aObserver )
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: request removed, cancel resp sent.");
            iGetUsedIapReqs[i]->iRespStatus = static_cast<TUint8>(ECancelled);
            iGetUsedIapReqs[i]->iNsObserver->GetUsedIapResp( iGetUsedIapReqs[i] );
            iGetUsedIapReqs.Remove( i );
            found = ETrue;
            }
        }

    if ( iGetUsedIapReqs.Count() == 0 )
        {
#ifdef __WINSCW__
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: this was the last request, cancel wlan scan simulation timer (WINSCW only).");
        iWlanScanSimulationTimer->Cancel();
#else
        // Cancel wlan scan
        Cancel();
        iGetUsedIapReqs.Close();
#endif        
        }

    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::CancelGetUsedIapReq() exit");

    if ( !found )
        {
        return KErrNotFound;
        }
    else
        {
        return KErrNone;
        }
    }
            
// -----------------------------------------------------------------------------
// CIptvNetworkSelection::GetUsedIapAfterWlanScanL
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::GetUsedIapAfterWlanScanL( TIptvGetUsedIapReq& aGetUsedIapReq )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetUsedIapAfterWlanScanL() start");

    //we update these again, AI might have changed
    UpdateAllButWlanListsL( aGetUsedIapReq );
       
#ifndef __WINSCW__  // wlan stuff is not needed in winscw
    // Note: This iOffLine check was added to fix the error TSIN-6YTDSJ 
    // "Video Center tries to connect via unavailable WLAN when refreshing
    // video feed". In online mode the RConnectionMonitor does return the
    // correct status of hidden WLAN APs so there is no need to add them
    // manually. Manual addition is needed only in offline mode where platform
    // cannot sniff for hidden APs (user authorisation would be required).
    if ( iOffline )
        {
        AddHiddenWlanSiIapsToSwiL();
        }
#endif
    // ALR defined
    // Used iap selected from SNAP with highest priority.
    RCmManagerExt manager;
	manager.OpenL();
	CleanupClosePushL( manager );
	
	TInt destId( 0 );
	CRepository* cenRep = CRepository::NewLC( KIptvAlrCenRepUid );
	if( cenRep->Get( KIptvCenRepUsedSnapIdKey, destId ) != KErrNone )
		{
		IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: * KIptvCenRepUsedSnapIdKey not found *");
		CleanupStack::PopAndDestroy( cenRep );
		CleanupStack::PopAndDestroy( &manager );
		aGetUsedIapReq.iRespStatus = static_cast<TUint8>(ENoSuitableIapFound);
		return;
		}
	
	CleanupStack::PopAndDestroy( cenRep );
	
	// Check if read only iaps
	if ( ( iService->GetFlags() & CIptvService::EReadOnlyIaps ) )
	    {
	    delete iAvailableIaps;
        iAvailableIaps = NULL;
        iAvailableIaps = CIptvIapList::NewL();

        TInt i;
        
#ifndef __WINSCW__
        for ( i = 0; i < iScannedWlanIaps->Count(); i++ )
            {
            iAvailableIaps->AddIap( iScannedWlanIaps->IapL( i ) );
            }
#endif      //__WINSCW__

        for( i = 0; i < iActiveIaps->Count(); i++ )
            {
            iAvailableIaps->AddIap( iActiveIaps->IapL( i ) );
            }
        
        TInt t=0;
		while( t < iService->iIapList->Count() )
			{
			TInt temp = iService->iIapList->IapL( t ).iId;
			
			RCmConnectionMethodExt conn;
			CleanupClosePushL( conn );
			TRAPD( err, conn = 
			    manager.ConnectionMethodL( iService->iIapList->IapL( t ).iId ) );
			    if( err == KErrNone )
			        {
			        if( KUidWlanBearerType != conn.GetIntAttributeL( CMManager::ECmBearerType ) )
			            {
			            CleanupStack::PopAndDestroy( &conn );
                    	CleanupStack::PopAndDestroy( &manager );
			            // Found read only iap other than wlan
			            SelectL( aGetUsedIapReq, iService->iIapList->IapL( t ).iId );
			            return;
			            }
			        }
			TInt s=0;
			while( s < iAvailableIaps->Count() )
		        {
				if( iService->iIapList->IapL( t ).iId == iAvailableIaps->IapL( s ).iId )
			        {
			        CleanupStack::PopAndDestroy( &conn );
			        CleanupStack::PopAndDestroy( &manager );
                    // Found suitable read only iap, select
                    SelectL( aGetUsedIapReq, iService->iIapList->IapL( t ).iId );
                    return;
					}
				s++;
				}
		    CleanupStack::PopAndDestroy( &conn );
		    t++;
			}
	    }
    
    //Check destination key
    if( destId == KIptvCenRepAPModeAlwaysAsk )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: * KIptvCenRepUsedSnapIdKey always ask *");
        CleanupStack::PopAndDestroy( &manager );
        aGetUsedIapReq.iRespStatus = static_cast<TUint8>(EFailedAlwaysAskSelected);
        return;
        }
	
	RCmDestinationExt cmDest = manager.DestinationL( destId );
	CleanupClosePushL( cmDest );
	
	TInt cmCount = cmDest.ConnectionMethodCount();
	TInt bestIap( 0 );
	TInt alrIap( 0 );
	TUint bearerType( 0 );
	
#ifdef __WINSCW__
	if( cmCount )
		{
		// In WINSW supposedly first priority can be used
		RCmConnectionMethodExt connMethod = cmDest.ConnectionMethodL( 0 );
		CleanupClosePushL( connMethod );
		bestIap = connMethod.GetIntAttributeL( CMManager::ECmIapId );
		CleanupStack::PopAndDestroy( &connMethod );
		}	
#else	// __WINSCW__

    delete iAvailableIaps;
    iAvailableIaps = NULL;
    iAvailableIaps = CIptvIapList::NewL();

    TInt i;
    for ( i = 0; i < iScannedWlanIaps->Count(); i++ )
        {
        iAvailableIaps->AddIap( iScannedWlanIaps->IapL( i ) );
        }
    for( i = 0; i < iActiveIaps->Count(); i++ )
        {
        iAvailableIaps->AddIap( iActiveIaps->IapL( i ) );
        }
  
    TInt iapCount = iAvailableIaps->Count();
    TBool gprsSelected( EFalse );

	if( cmCount )
		{
		IPTVLOGSTRING3_LOW_LEVEL("CIptvNetworkSelection:: %d %d", cmCount, iapCount );
		TInt t=0;
		while( t<cmCount )
			{
			RCmConnectionMethodExt connMethod = cmDest.ConnectionMethodL( t );
			CleanupClosePushL( connMethod );
			alrIap = connMethod.GetIntAttributeL( CMManager::ECmIapId );
			bearerType = connMethod.GetIntAttributeL( CMManager::ECmBearerType );
			CleanupStack::PopAndDestroy( &connMethod );
			if( !iOffline && ( KUidWlanBearerType != bearerType ) )
			    {
			    // Found gprs, select
			    bestIap = alrIap;
			    gprsSelected = ETrue;
			    t = cmCount;
			    }
			else
			    {
			    TInt s=0;
			    while( s<iapCount )
				    {
				    if( alrIap == iAvailableIaps->IapL( s ).iId )
					    {
					    // Found usable wlan, select
					    bestIap = alrIap;
                        s = iapCount;
                        t = cmCount;
					    }
					s++;
				    }
				}
		    t++;
			}
		}

    // Check if wlan available when gprs already selected
    if( !iOffline )
        {

		if( gprsSelected )
		    {
		    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: check if WLAN available" );
		    // GPRS selected, check if WLAN available in other SNAPs
	        
	        // Get list of connection methods that do not belong to any destination
	        RArray<TUint32> iapArray( 3 );
	        manager.ConnectionMethodL( iapArray );
	        IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: not any dest: %d", iapArray.Count() );
	        
	        TInt checkedId( 0 );
	        for( TInt t=0; t<iapArray.Count(); t++ )
	            {	            
	            // Check if WLAN iap found
	            RCmConnectionMethodExt cm = manager.ConnectionMethodL( iapArray[t] );
                CleanupClosePushL( cm );
	            if( cm.GetIntAttributeL( CMManager::ECmBearerType ) == KUidWlanBearerType )
	                {
	                checkedId = cm.GetIntAttributeL( CMManager::ECmIapId );
	                TInt s=0;
			        while( s < iapCount )
				        {
				        if( checkedId == iAvailableIaps->IapL( s ).iId )
					        {
	                        IPTVLOGSTRING_LOW_LEVEL( "CIptvNetworkSelection:: iWlanWhenGPRS = ETrue" );
	                        iWlanWhenGPRS = ETrue;
                            s = iapCount;
					        }
					    s++;
				        }   

	                }
	            CleanupStack::PopAndDestroy( &cm );
	            }
	            
	        if( !iWlanWhenGPRS )
	            {
	            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: Still not found WLAN" );
	            // Still not found WLAN, check other snaps
	            RArray<TUint32> destArray(3);
	            manager.AllDestinationsL( destArray );
	            
	            TInt selectedDest( 0 );
            	CRepository* cenRep = CRepository::NewLC( KIptvAlrCenRepUid );
	            cenRep->Get( KIptvCenRepUsedSnapIdKey, selectedDest );
	            CleanupStack::PopAndDestroy( cenRep );
	            IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: dest count:%d", destArray.Count() );
	            
	            for( TInt s=0; s<destArray.Count(); s++ )
	                {
	                // Check all other destinations all iaps
	                if( selectedDest != destArray[s] )
	                    {
	                    RCmDestinationExt cmDest = manager.DestinationL( destArray[s] );
	                    CleanupClosePushL( cmDest );
	                    TInt methodCount = cmDest.ConnectionMethodCount();
	                    
	                    IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: methodCoun:%d", methodCount );
	                    
	                    for( TInt t=0; t<methodCount; t++ )
	                        {
	                        RCmConnectionMethodExt cm = cmDest.ConnectionMethodL( t );
	                        if(  cm.GetIntAttributeL( CMManager::ECmBearerType ) == KUidWlanBearerType )
	                            {
	                            checkedId = cm.GetIntAttributeL( CMManager::ECmIapId );
	                            TInt s=0;
			                    while( s < iapCount )
				                    {
				                    if( checkedId == iAvailableIaps->IapL( s ).iId )
					                    {
	                                    IPTVLOGSTRING_LOW_LEVEL(" CIptvNetworkSelection:: iWlanWhenGPRS = ETrue" );
	                                    iWlanWhenGPRS = ETrue;
                                        s = iapCount;
					                    }
					                s++;
				                    }
	                            }
	                        }
	                    CleanupStack::PopAndDestroy( &cmDest );
	                    }
	                }
	            
	            }		    
		    }
        }
    
#endif	// __WINSCW__
	
	CleanupStack::PopAndDestroy( &cmDest );
	CleanupStack::PopAndDestroy( &manager );
	
    if( bestIap )
    	{
    	SelectL( aGetUsedIapReq, bestIap );
    	}
    else
    	{
    	aGetUsedIapReq.iRespStatus = static_cast<TUint8>(ENoSuitableIapFound);
    	}
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetUsedIapAfterWlanScanL() exit");        
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::SetConnectionAllowed
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::SetConnectionAllowedL(TBool aConnectionAllowed,
                                                  TUint32 aIapId,
                                                  TRespStatus& aRespStatus)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::SetConnectionAllowedL() start");

    if ( aConnectionAllowed )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: setting connection allowed for: %S type",
                                 &(ConnectionTypeDes( CIptvUtil::ConnectionTypeL( aIapId ) ) ));

        iConnectionAllowedMask |= CIptvUtil::ConnectionTypeL( aIapId );

        }
    else
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: clearing connection allowed for: %S type",
                                 &(ConnectionTypeDes( CIptvUtil::ConnectionTypeL( aIapId ) ) ));

        iConnectionAllowedMask &= ~CIptvUtil::ConnectionTypeL(aIapId);
        }

    IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: connection allowed mask is now: %x", iConnectionAllowedMask);
        
    aRespStatus = ESucceeded;             
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::SetConnectionAllowedL() exit"); 
    }
    
// -----------------------------------------------------------------------------
// CIptvNetworkSelection::GetIapNameL
// Gets IAP name, if empty, tries to find SSID
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::GetIapNameL( TUint32 aIapId, TDes& aIapName )
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetIapNameL() start");
    
    TInt leaveValue = KErrNotFound;
    
    aIapName.Zero();
    
    CCommsDatabase* commsDb = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    CleanupStack::PushL( commsDb ); //1->
    
    CCommsDbTableView* iapTableView;

    //get iap table view
    iapTableView = commsDb->OpenTableLC( TPtrC(IAP) ); //2->
    
    //Search correct iap and pick connection name
    TInt err = iapTableView->GotoFirstRecord();

    while ( err == KErrNone )
        {
        TUint32 iapId;
        iapTableView->ReadUintL( TPtrC(COMMDB_ID), iapId );

        if ( aIapId == iapId )
            {
            //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: IAP found.");

            TUint32 iapService;

            iapTableView->ReadTextL( TPtrC(COMMDB_NAME), aIapName );
            //IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: COMMDB_NAME: %S", &aIapName);

            iapTableView->ReadUintL( TPtrC(IAP_SERVICE), iapService );
            //IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: IAP_SERVICE = %d", iapService);
            
            //Try to get SSID if IAP had no name
            if ( aIapName.Length() == 0 )
                {
                //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: IAP name was empty, trying to get SSID");

                TBuf<KIptvUtilMaxTextFieldLengthInDb> colText;
                iapTableView->ReadTextL( TPtrC(IAP_SERVICE_TYPE), colText ); 
                if ( colText == KIptvUtilLanService )
                    {
                    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: was lan service, proceeding");
                    
                    TInt err = GetSsidL( iapService, aIapName );
                    if ( err == KErrNone )
                        {
                        //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: SSID id found, returning it");
                        leaveValue = KErrNone;    
                        }
                    else
                        {
                        //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: Could not get SSID, wasn't WLAN service");                    
                        }
                    }
                else
                    {
                    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: was not lan service, can't be wlan service.");
                    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: giving up.");
                    }
                }
            else
                {
                //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: IAP name found, returning it");
                leaveValue = KErrNone;
                }
                            
            break;
            }
        err = iapTableView->GotoNextRecord();
        }
    CleanupStack::PopAndDestroy( iapTableView );                //<-2
    CleanupStack::PopAndDestroy( commsDb );                     //<-1

    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetIapNameL() exit");    
    User::LeaveIfError( leaveValue );
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::GetIapIdL
// Gets IAP ID
// -----------------------------------------------------------------------------
//
TInt CIptvNetworkSelection::GetIapIdL(TUint32& aIapId, const TDesC& aIapName)
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetIapIdL() start");
    
    TInt returnValue = KErrNotFound;
        
    CCommsDatabase* commsDb = CCommsDatabase::NewL(EDatabaseTypeUnspecified);
    CleanupStack::PushL(commsDb); //1->
    
    CCommsDbTableView* iapTableView;

    //get iap table view
    iapTableView = commsDb->OpenTableLC(TPtrC(IAP)); //2->
    
    //Search correct iap name and pick iap id
    TInt err = iapTableView->GotoFirstRecord();

    while ( err == KErrNone )
        {
        TBuf<KIptvUtilMaxTextFieldLengthInDb> iapName;
        
        iapTableView->ReadTextL(TPtrC(COMMDB_NAME), iapName);

        if ( aIapName == iapName )
            {
            //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: IAP found.");

            iapTableView->ReadUintL(TPtrC(COMMDB_ID), aIapId);
            //IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: COMMDB_ID: %d", aIapId);

            returnValue = KErrNone;                                    
            break;
            }
        err = iapTableView->GotoNextRecord();
        }
    CleanupStack::PopAndDestroy(iapTableView);                //<-2
    CleanupStack::PopAndDestroy(commsDb);                     //<-1

    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetIapIdL() exit");    
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::GetSsidL
// -----------------------------------------------------------------------------
//
TInt CIptvNetworkSelection::GetSsidL( TUint32 aWlanServiceId, TDes& aSsid )
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetSsidL() start");
    
    TInt returnValue = KErrNotFound;
    
    CCommsDatabase* commsDb = CCommsDatabase::NewL(EDatabaseTypeUnspecified);
    CleanupStack::PushL(commsDb);                             //1->
    
    CCommsDbTableView* wlanTableView;

    //get wlan table view
    wlanTableView = commsDb->OpenTableLC(TPtrC(WLAN_SERVICE));//2->
    
    TUint32 wlanServiceId;
    TInt err = wlanTableView->GotoFirstRecord();
    TBuf<KWlanMaxSsidLength> ssid;
    
    while(err == KErrNone)
        {
        wlanTableView->ReadUintL(TPtrC(WLAN_SERVICE_ID), wlanServiceId);


#if 1 // def __SERIES60_32__
        wlanTableView->ReadTextL(TPtrC(NU_WLAN_SSID), ssid);
#else
        wlanTableView->ReadTextL(TPtrC(WLAN_SSID), ssid); 
#endif // __SERIES60_32__

        if(wlanServiceId == aWlanServiceId)
            {
            //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: WLAN service ID found, returning SSID");
            aSsid = ssid;
            returnValue = KErrNone;
            break;
            }
        err = wlanTableView->GotoNextRecord();
        }
    CleanupStack::PopAndDestroy(wlanTableView); // <-2
    CleanupStack::PopAndDestroy(commsDb);       // <-1

    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetSsidL() exit");    
    return returnValue;
    }
    
// -----------------------------------------------------------------------------
// CIptvNetworkSelection::GetActiveIapsL
// -----------------------------------------------------------------------------
//
CIptvIapList* CIptvNetworkSelection::GetActiveIapsL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetActiveIaps() start");

    CIptvIapList* iapList;
    iapList = CIptvIapList::NewL();
    CleanupStack::PushL(iapList); // 1->
    
    TUint count;
    RSocketServ serv;
    RConnection connection;

    
    if ( serv.Connect() == KErrNone )
        {
        //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: socket server open");
        if ( connection.Open(serv, KAfInet ) == KErrNone)
            {
            //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: connection open");
            if ( connection.EnumerateConnections( count ) == KErrNone )
                {
                //IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: connection count: %d", count);
                for (; count; --count )
                    {
                    TPckgBuf<TConnectionInfo> connInfo;

                    if ( connection.GetConnectionInfo( count, connInfo ) == KErrNone )
                        {
                        IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: connection: %d", count);
                        IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: IapId: %d", connInfo().iIapId);
                        TIptvIap iap;
                        iap.iId       = connInfo().iIapId;
                        iap.iPriority = 0;
                        if ( iapList->AddIap( iap, ETrue ) != KErrNone )
                            {
                            IPTVLOGSTRING_HIGH_LEVEL("CIptvNetworkSelection:: IAP list overflow, could not add all active IAPS!");
                            }
                        }    
                    }
                }
            connection.Close();
            }
        serv.Close();
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetActiveIaps() exit");

    CleanupStack::Pop(iapList); // <-1
    return iapList;
    }
    
// -----------------------------------------------------------------------------
// CIptvNetworkSelection::UpdateActiveIapsL
// -----------------------------------------------------------------------------
//    
void CIptvNetworkSelection::UpdateActiveIapsL()
    {
    if ( iActiveIaps )
        {
        delete iActiveIaps;
        iActiveIaps = NULL;
        }
        
    iActiveIaps = GetActiveIapsL();
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::UpdateWlanScannedIapsL
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::UpdateWlanScannedIapsL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateWlanScannedIapsL() start");
                
    User::LeaveIfError( iConnMon.ConnectL() );

    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: iConnMon connected");
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: starting async wlan scan");

    iConnMon.GetPckgAttribute( EBearerIdWLAN, 
                               0, 
                               KIapAvailability, 
                               iIapInfoBuf, 
                               iStatus );

    SetActive();

    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateWlanScannedIapsL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::RunL
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::RunL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::RunL() start");    

    delete iScannedWlanIaps;
    iScannedWlanIaps = NULL;
    iScannedWlanIaps = CIptvIapList::NewL();

    if ( iStatus.Int() != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: wlan scan failed. iStatus: %d", iStatus.Int() );
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: wlan scan successful.");

        TConnMonIapInfo iapInfo = iIapInfoBuf();
        TInt i;
        for ( i = 0; i < iapInfo.iCount; i++ )
            {
            IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: wlan iap: %d", iapInfo.iIap[i].iIapId);
            TIptvIap iap;
            iap.iId          = iapInfo.iIap[i].iIapId;
            iap.iPriority    = 0;
            iap.iValueExists = ETrue;
            iScannedWlanIaps->AddIap( iap, ETrue /* ignore max limit */ );
            }
        }

    iConnMon.Close();

    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: Scanned WLAN IAPs:");
#if IPTV_LOGGING_METHOD != 0
    PrintIapList( *iScannedWlanIaps );
#endif

    //wlan scan has been performed (successfully or not)
    //Now handle all GetUsedIapReq:s we have pending
    HandleGetUsedIapRequests();

    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::RunL() exit");
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::RunError
// -----------------------------------------------------------------------------
//   
TInt CIptvNetworkSelection::RunError( TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::RunError() leave in RunL!");

    iConnMon.Close();
    return KErrNone;
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::FindCommonIapFromListsL
// -----------------------------------------------------------------------------
//    
TInt CIptvNetworkSelection::FindCommonIapFromListsL( CIptvIapList& aServiceIaps,
                                                     CIptvIapList& aCompareIaps,
                                                     TIptvIap& aIap )
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::FindCommonIapFromListsL() start");

    aServiceIaps.SortByPriorityL();
    
    TInt i, j;
    for ( j = 0; j < aServiceIaps.Count(); j++ )
        {
        //IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: service iap = %d", aServiceIaps.IapL(j).iId);    
        for ( i = 0; i < aCompareIaps.Count(); i++ )
            {
            //IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: compare iap = %d", aCompareIaps.IapL(i).iId);    

            if ( aServiceIaps.IapL(j).iId == aCompareIaps.IapL(i).iId )
                {
                //IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: common IAP found from service and compare lists: %d", aServiceIaps.IapL(j).iId);    
                aIap.iValueExists = ETrue;
                aIap.iId          = aServiceIaps.IapL(j).iId;
                aIap.iPriority    = aServiceIaps.IapL(j).iPriority;
                //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::FindCommonIapFromListsL() exit");
                return KErrNone;
                } 
            }
        
        }
        
    aIap.iValueExists = EFalse;
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: common IAP not found from service and compare lists");    
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::FindCommonIapFromListsL() exit");
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::IsConnectionAllowedL
// -----------------------------------------------------------------------------
//
CIptvNetworkSelection::TConnectionPermission CIptvNetworkSelection::IsConnectionAllowedL(
        TUint32 aIapId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::IsConnectionAllowed() start");
    
    TConnectionPermission connectionPermission;
    CIptvUtil::TConnectionType connectionType;
    
    connectionType = CIptvUtil::ConnectionTypeL( aIapId );
    
#if IPTV_LOGGING_METHOD != 0
        const TInt KMaxIapNameLength = 255;
        TBuf<KMaxIapNameLength> iapName;
        TRAP_IGNORE( GetIapNameL( aIapId, iapName ) ); //dont leave from debug code

        IPTVLOGSTRING3_LOW_LEVEL("CIptvNetworkSelection:: IAP Name = %S, connection type = %S",
                                 &iapName,
                                 &ConnectionTypeDes( connectionType ));
#endif
    
    if ( connectionType & iConnectionAllowedMask )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: Connection is allowed");
        connectionPermission = EAllowed;
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: Connection is not allowed");
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: ->EMustAskConfirmation");
        connectionPermission = EMustAskConfirmation;
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::IsConnectionAllowed() exit");
    return connectionPermission;
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::CreateFilteredIapListL
// -----------------------------------------------------------------------------
//
CIptvIapList* CIptvNetworkSelection::CreateFilteredIapListL(CIptvIapList& aIapList,
                                                           TUint32 aConnectionTypeMask)
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::CreateFilteredIapListL() start");
    
    CIptvIapList* filteredIapList = CIptvIapList::NewL();
    CleanupStack::PushL( filteredIapList ); // 1->
    
    TInt i;
    for ( i = 0; i < aIapList.Count(); i++ )
        {
        if((CIptvUtil::ConnectionTypeL( aIapList.IapL(i).iId ) & aConnectionTypeMask))
            {
            //IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: Adding %d to filtered iap list.", aIapList.IapL(i).iId);
            filteredIapList->AddIap( aIapList.IapL( i ), ETrue );
            }
        }
    CleanupStack::Pop( filteredIapList ); // <-1

    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::CreateFilteredIapListL() exit");
    return filteredIapList;
    }

#ifdef ISIAIMATCH_USED
// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::UpdateSiAiMatchL
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::UpdateSiAiMatchL()
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateSiAiMatchL() start");

    if ( iService && iActiveIaps )
        {
        TInt err =  FindCommonIapFromListsL(*(iService->iIapList),
                                        *iActiveIaps,
                                        iSiAiMatch);    
        }
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateSiAiMatchL() exit");
    }
#endif

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::UpdateSiSwiMatchL
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::UpdateSiSwiMatchL()
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateSiSwiMatchL() start");

    if ( iService && iScannedWlanIaps )
        {
        TInt err =  FindCommonIapFromListsL(*(iService->iIapList),
                                        *iScannedWlanIaps,
                                        iSiSwiMatch);    
        }
    
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateSiSwiMatchL() exit");
    }

#ifdef ICISIMATCH_USED
// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::UpdateCiSiMatch
// -----------------------------------------------------------------------------
//    
void CIptvNetworkSelection::UpdateCiSiMatch()
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateCiSiMatch() start");

    TUint8 iapIndex;

    if ( !iCurrentIap.iValueExists )
        {
        iCiSiMatch.iValueExists = EFalse;
        //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateCiSiMatch() exit");
        return;
        }
    TInt err( KErrNotReady );        
    if ( iService && iService->iIapList )
        {
        err = iService->iIapList->FindIap(iCurrentIap.iId, iapIndex);
        }

    if ( err == KErrNone )
        {
        iCiSiMatch.iValueExists = ETrue;
        TRAP_IGNORE( //iapIndex is valid since FindIap didnt return error, we can ignore leave
        iCiSiMatch.iId          = iService->iIapList->IapL( iapIndex ).iId;
        iCiSiMatch.iPriority    = iService->iIapList->IapL( iapIndex ).iPriority;
        );
        }
    else
        {
        iCiSiMatch.iValueExists = EFalse;
        }    
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateCiSiMatch() exit");
    }
#endif

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::UpdateCiAiMatch
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::UpdateCiAiMatch()
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateCiAiMatch() start");

    TUint8 iapIndex;
    
    if ( !iCurrentIap.iValueExists )
        {
        iCiAiMatch.iValueExists = EFalse;
        //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateCiAiMatch() exit");
        return;
        }
                
    TInt err = iActiveIaps->FindIap(iCurrentIap.iId, iapIndex);

    if ( err == KErrNone )
        {
        iCiAiMatch.iValueExists = ETrue;
        TRAP_IGNORE( //iapIndex is valid, we can ignore leave
        iCiAiMatch.iId          = iActiveIaps->IapL(iapIndex).iId;
        iCiAiMatch.iPriority    = iActiveIaps->IapL(iapIndex).iPriority; //this is not valid
        );
        }
    else
        {
        iCiAiMatch.iValueExists = EFalse;
        }    
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateCiAiMatch() exit");
    } 

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::UpdateCiSwiMatchL
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::UpdateCiSwiMatchL()
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateCiSwiMatchL() start");

    TUint8 iapIndex;

    if ( !iCurrentIap.iValueExists )
        {
        iCiSwiMatch.iValueExists = EFalse;
        return;
        }
    TInt err( KErrNotReady );
    if ( iScannedWlanIaps )
        {
        err = iScannedWlanIaps->FindIap( iCurrentIap.iId, iapIndex );    
        }

    if ( err == KErrNone )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: CiSwiMatch found.");
        iCiSwiMatch.iValueExists = ETrue;
        iCiSwiMatch.iId          = iCurrentIap.iId;
        iCiSwiMatch.iPriority    = iCurrentIap.iPriority;
        }
    else
        {
        TBool isHiddenWlanIap = EFalse;
        CIptvUtil::TConnectionType connType;
        
        connType = CIptvUtil::ConnectionTypeL( iCurrentIap.iId, isHiddenWlanIap );
        
        if ( connType == CIptvUtil::EWlan && isHiddenWlanIap && iOffline )
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: Current IAP was hidden wlan iap and we were in offline mode, setting to CiSwiMatch");
            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: even if it is not on scanned wlan iaps list.");
            iCiSwiMatch.iValueExists = ETrue;
            iCiSwiMatch.iId          = iCurrentIap.iId;
            iCiSwiMatch.iPriority    = iCurrentIap.iPriority;
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: CiSwiMatch not found.");
            iCiSwiMatch.iValueExists = EFalse;
            }
        }    
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateCiSwiMatchL() exit");
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::DoCancel
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::DoCancel()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::DoCancel() active object cancelled!");
    
    iConnMon.CancelAsyncRequest( EConnMonGetPckgAttribute ); // From TConnMonAsyncRequest
    iConnMon.Close();
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::TimerExpired
// -----------------------------------------------------------------------------
//
#ifdef __WINSCW__    
void CIptvNetworkSelection::TimerExpired(CIptvTimer* aTimer)
#else
void CIptvNetworkSelection::TimerExpired(CIptvTimer* /*aTimer*/)
#endif
    {
#ifdef __WINSCW__        

    if ( aTimer == iWlanScanSimulationTimer )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: iWlanScanSimulationTimer expired");
        HandleGetUsedIapRequests();
        return;
        }

#endif

    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::GetWlanIapFromAiL
// -----------------------------------------------------------------------------
//    
void CIptvNetworkSelection::GetWlanIapFromAiL(TIptvIap& aIap)
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetWlanIapFromAiL() start");

    aIap.iValueExists = EFalse;
    
    TInt i;
    for ( i = 0; i < iActiveIaps->Count(); i++ )
        {
        if ( CIptvUtil::ConnectionTypeL( iActiveIaps->IapL( i ).iId ) == CIptvUtil::EWlan )
            {
            //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: WLAN IAP found from Ai");
            aIap.iValueExists = ETrue;
            aIap.iId          = iActiveIaps->IapL( i ).iId;
            aIap.iPriority    = iActiveIaps->IapL( i ).iPriority;
            return; //there can be only one wlan iap in Ai, no need to go further
            }
        }
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::GetWlanIapFromAiL() exit");
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::SelectCiL
// -----------------------------------------------------------------------------
//    
void CIptvNetworkSelection::SelectCiL(TIptvGetUsedIapReq& aGetUsedIapReq)
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: Selected iCurrentIap");
    SelectL(aGetUsedIapReq, iCurrentIap.iId);
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::SelectL
// -----------------------------------------------------------------------------
//    
void CIptvNetworkSelection::SelectL(TIptvGetUsedIapReq& aGetUsedIapReq, TUint32 aIapId)
    {
    aGetUsedIapReq.iIapId                = aIapId;
    aGetUsedIapReq.iWLANWhenGPRS		= iWlanWhenGPRS;
    
    TRAPD( err, GetIapNameL(aGetUsedIapReq.iIapId, aGetUsedIapReq.iIapName));
    
    // If other than KErrNoMemory error occurs, we just give empty iap name and ignore leave
    if ( err != KErrNone )
        {
        if (err == KErrNoMemory )
            {
            User::Leave( KErrNoMemory );
            }
        else
            {
            aGetUsedIapReq.iIapName.Zero();
            }
        }
        
    aGetUsedIapReq.iConnectionPermission = static_cast<TUint8>( IsConnectionAllowedL(
                                                                    aGetUsedIapReq.iIapId ) );
    aGetUsedIapReq.iRespStatus = static_cast<TUint8>( ESucceeded );

    IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: Selected IAP ID: %d", aIapId);
    IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: Selected IAP name: %S", &(aGetUsedIapReq.iIapName));
    IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: Selected conn perm: %d", aGetUsedIapReq.iConnectionPermission);
    IPTVLOGSTRING2_LOW_LEVEL("CIptvNetworkSelection:: Selected resp status: %d", aGetUsedIapReq.iRespStatus);
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::UpdateAllButWlanListsL
// -----------------------------------------------------------------------------
//    
void CIptvNetworkSelection::UpdateAllButWlanListsL(TIptvGetUsedIapReq& /*aGetUsedIapReq*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateAllButWlanListsL() start");

    if ( iOffline )
        {
        ClearGprsAndCsdIapsL();
        }
    
    UpdateActiveIapsL();
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateAllButWlanListsL() exit");
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::UpdateServiceL
// -----------------------------------------------------------------------------
//    
void CIptvNetworkSelection::UpdateServiceL(TUint32 aServiceId)
    {
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateServiceL() start");

    delete iService;
    iService = NULL;
        
    iService = iServer.iServiceManager->GetServiceL( aServiceId, ETrue /* do open/close */ );
    if ( iService )
        {
        iService->iIapList->SortByPriorityL();
        
        //convert iap name (if exists) to iap id
        if ( iService->GetIapNameL().Length() > 0 )
            {
            TUint32 iapId;
            TInt err = GetIapIdL(iapId, iService->GetIapNameL());
            TIptvIap iap;
            iap.iId          = iapId;
            iap.iPriority    = 0;
            iap.iValueExists = ETrue;
            iService->GetIapList().AddIap(iap);
            IPTVLOGSTRING3_LOW_LEVEL("CIptvNetworkSelection:: Converted %S iap name to iap id: %d", &(iService->GetIapNameL()), iapId);
            iService->SetIapNameL(KIptvEmptyDes);
            iServer.iServiceManager->UpdateServiceL(*iService);
            }
        }
        
    //IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::UpdateServiceL() exit");
    }

#if IPTV_LOGGING_METHOD != 0
// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::PrintIapList
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::PrintIapList( CIptvIapList& aIapList )
    {
    TUint8 i;
    
    if ( aIapList.Count() == 0 )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvNetworkSelection:: * Iaplist empty. *");
        return;
        }
        
    const TInt KMaxIapNameLength = 255;
    TBuf<KMaxIapNameLength> iapName;
    TUint32 iapId;
    TUint16 iapPriority;
    for ( i = 0; i < aIapList.Count(); i++ )
        {
        TRAPD( err, GetIapNameL( aIapList.IapL(i).iId, iapName ));
        if ( err != KErrNone )
            {
            iapName.Zero();
            }

        TRAP( err, iapId = aIapList.IapL(i).iId );
        if ( err != KErrNone )
            {
            iapId = KMaxTUint32;
            }

        TRAP( err, iapPriority = aIapList.IapL(i).iPriority );
        if ( err != KErrNone )
            {
            iapPriority = KMaxTUint16;
            }

        IPTVLOGSTRING4_HIGH_LEVEL(
            "CIptvNetworkSelection:: * IAP Name: %S, ID %d PRIO: %d *",
            &iapName, iapId, iapPriority );
        }
    }
#endif

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::ClearUsedIap
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::ClearUsedIap()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvNetworkSelection:: current iap cleared.");
    iCurrentIap.iValueExists = EFalse;
    iCurrentIap.iId          = 0;
    }

// -----------------------------------------------------------------------------    
// CIptvNetworkSelection::AddHiddenWlanSiIapsToSwiL
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::AddHiddenWlanSiIapsToSwiL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::AddHiddenWlanSiIapsToSwiL() start");

    TInt i;
    for ( i = 0; i < iService->iIapList->Count(); i++ )
        {
        TIptvIap& iap = iService->iIapList->IapL( i );
        TBool isHiddenWlanIap;
        CIptvUtil::TConnectionType connType;
        
        connType = CIptvUtil::ConnectionTypeL( iap.iId, isHiddenWlanIap );
        
        if ( connType == CIptvUtil::EWlan && isHiddenWlanIap )
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvNetworkSelection:: adding hidden service IAP (%d) to scanned wlan iaps list", iap.iId);
            if ( iScannedWlanIaps )
                {
                TInt err = iScannedWlanIaps->AddIap(iap, ETrue /* ignore max limit */);
                if ( err != KErrNone )
                    {
                    IPTVLOGSTRING2_LOW_LEVEL( "CIptvNetworkSelection::AddHiddenWlanSiIapsToSwiL: iScannedWlanIaps->AddIap failed: %d", err );
                    }
                }
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::AddHiddenWlanSiIapsToSwiL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::HandleActiveProfileEventL()
// -----------------------------------------------------------------------------
//
void CIptvNetworkSelection::HandleActiveProfileEventL( TProfileEvent aProfileEvent, 
                                                       TInt aProfileId ) 
    {
    if ( aProfileEvent == EProfileNewActiveProfile ) //active profile has been changed
        {
        if ( aProfileId == EProfileOffLineId )
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: phone changed to offline mode");
            iOffline = ETrue;
            }
        else
            {
            if (iOffline)
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: phone changed back to online mode.");
                iOffline = EFalse;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CIptvNetworkSelection::ClearGprsAndCsdIaps()
// -----------------------------------------------------------------------------
//
void  CIptvNetworkSelection::ClearGprsAndCsdIapsL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::ClearGprsAndCsdIaps() start");

    //Clear iCurrentIap if it is EGprs or ECsd    
    if ( iCurrentIap.iValueExists )
        {
        if ( CIptvUtil::ConnectionTypeL( iCurrentIap.iId ) == CIptvUtil::EGprs || 
             CIptvUtil::ConnectionTypeL( iCurrentIap.iId ) == CIptvUtil::ECsd )
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: iCurrentIap (GPRS/CSD type) cleared");
            iCurrentIap.iValueExists = EFalse;
            }
        }

    //Clear EGprs and ECsd iaps from iService iaps
    CIptvIapList* serviceIaps = CreateFilteredIapListL( iService->GetIapList(), CIptvUtil::EWlan );
    CleanupStack::PushL( serviceIaps ); // 1->
    iService->SetIapListL( *serviceIaps );
    CleanupStack::PopAndDestroy( serviceIaps ); // <-1
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection:: iService GPRS and CSD IAPS cleared");
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvNetworkSelection::ClearGprsAndCsdIaps() exit");
    }

#if IPTV_LOGGING_METHOD != 0
// -----------------------------------------------------------------------------
// CIptvNetworkSelection::ConnectionTypeDes()
// -----------------------------------------------------------------------------
//
const TDesC& CIptvNetworkSelection::ConnectionTypeDes( CIptvUtil::TConnectionType aConnectionType )
    {
    switch ( aConnectionType )
        {
        case CIptvUtil::EWlan:
            return KIptvNsWlan;
            
        case CIptvUtil::EGprs:
            return KIptvNsGprs;
            
        case CIptvUtil::ECsd:
            return KIptvNsCsd;
            
        default:
            return KIptvNsUnknown;
        }
    }
#endif
