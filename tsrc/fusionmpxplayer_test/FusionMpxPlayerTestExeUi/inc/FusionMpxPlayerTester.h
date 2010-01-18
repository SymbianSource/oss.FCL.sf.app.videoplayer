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
* Description: 
*
*/


#ifndef FUSIONMPXPLAYERTESTER_H
#define FUSIONMPXPLAYERTESTER_H

//  INCLUDES
#include <e32base.h>

#include <mpxvideoplaybackdefs.h>
#include <mpxplaybackobserver.h>
#include <mpxviewactivationobserver.h>

// FORWARD DECLARATIONS
class CVcxConnectionUtility;
class MMPXPlaybackUtility;
class MMPXViewUtility;
class CIptvTestActiveWait;

// DATA TYPES
const TInt KTestUndefined = 10001;
const TInt KFusionTestMsgNone = -66666;

// CLASS DECLARATION
class TExpectedGeneralMessage
    {
    public:
        TExpectedGeneralMessage( TInt aEvent, TInt aType, TInt aData )
            {
            iEvent = aEvent;
            iType = aType;
            iData = aData;
            }
    public:
        TInt iEvent;
        TInt iType;
        TInt iData;
    };

class MFusionMpxPlayerTesterObserver
    {
    public:
        /**
         * Sent when expected message list is empty. 
         */
        virtual void TesterCommandComplete() = 0;
    };

/**
* 
*/
class CFusionMpxPlayerTester : public CBase,
                               public MMPXPlaybackObserver,
                               public MMPXViewActivationObserver,
                               public MMPXPlaybackCallback
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CFusionMpxPlayerTester* NewL( MFusionMpxPlayerTesterObserver* aObserver );

        /**
        * C++ default constructor.
        */
        CFusionMpxPlayerTester();

        /**
        * Destructor.
        */
        ~CFusionMpxPlayerTester();

    public: // New functions

        TInt OpenLocalFile( const TDesC& aFileName );
        
        void OpenUrlL( const TDesC& aUrl, TInt aIapID );

        void CreateGeneralPlaybackCommandL( TMPXPlaybackCommand aCmd, TInt aData = KTestUndefined );
        
        void CreateVideoPlaybackCommandL( TMPXVideoPlaybackCommand aCmd );
        
        /**
         * Checks that all expected messages have been gotten.
         */
        TInt CheckExpectedMessages();
        
        void AddExpectedGeneralPlaybackCommandMessages( TMPXPlaybackCommand aCmd, TInt &aData );
        void AddExpectedVideoPlaybackCommandMessages( TMPXVideoPlaybackCommand aCmd );
        TInt ExpectedMessageCount() { return iExpectedGeneralMessages.Count(); };
        void PrepareCloseL();
    
    private: // New functions

        void GetGeneralPlaybackCommandDes( TMPXPlaybackCommand aCmd, TDes& aDes );
        void GetVideoPlaybackCommandDes( TMPXVideoPlaybackCommand aCmd, TDes& aDes );
        void GetGeneralPlaybackEventDes( TInt aEvent, TDes& aDes );
        void GetStateDes( TMPXPlaybackState aState, TDes& aDes );
        void GetPropertyDes( TMPXPlaybackProperty aProperty, TDes& aDes );        
        
    private: // Constructors and destructor

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MFusionMpxPlayerTesterObserver* aObserver );

    private: // Functions from base classes

        /**
        *  Handle playback message.
        *  Note: only one of HandlePlaybackMessage callback can be implemented.
        *  Note: change into pure virtual when all clients updated.
        *
        *  @since S60 3.2.3
        *  @param aMsg playback message, ownership not transferred.
        *         Please check aMsg is not NULL before using it. If aErr is not
        *         KErrNone, plugin might still call back with more info in the aMsg.
        *  @param aErr system error code.
        */
        virtual void HandlePlaybackMessage( CMPXMessage* aMsg, TInt aErr );

        void DoHandleGeneralPlaybackMessageL( const CMPXMessage& aMessage );
        
        void DoHandleVideoPlaybackMessageL( const CMPXMessage& aMessage );

        void DoHandleStateChangeL( TMPXPlaybackState aState );
        
        void DoHandlePropertyChangeL( TInt aProperty, TInt aValue );
        
        /**
         * Handle view activation.
         *
         * @since S60 3.2.3
         * @param aCurrentViewType Current view type Uid.
         * @param aPreviousViewType Previous view type Uid.
         */
        virtual void HandleViewActivation( const TUid& aCurrentViewType, const TUid& aPreviousViewType );

        /**
         * Called in case a view plugin is added/updated/removed through a SIS install.
         *
         * @since S60 3.2.3
         * @param aViewUid view UID
         * @param aUpdateEvent Defines the update type (addition/update/removal).
         * @param aLoaded ETrue if the view plugin is loaded, EFalse otherwise. It is recommended
         *          that client applications exit when loaded plugins are updated or removed.
         *          This would normally be handled in the AppUi.
         * @param aData Its meaning depends on the update event:
         *            EViewPluginAdd - new view plugin version
         *            EViewPluginUpdate - new view plugin version
         *            EViewPluginRemove - old view plugin version
         */
        virtual void HandleViewUpdate( TUid aViewUid, TViewUpdateEvent aUpdateEvent, 
                TBool aLoaded, TInt aData );

        /**
        *  Handle playback property.
        *
        *  @since S60 3.2.3
        *  @param aProperty the property
        *  @param aValue the value of the property
        *  @param aError error code
        */
        virtual void HandlePropertyL(TMPXPlaybackProperty aProperty,
                                     TInt aValue, 
                                     TInt aError);
        
        /**
        *  Method is called continously until aComplete=ETrue, signifying that 
        *  it is done and there will be no more callbacks.
        *  Only new items are passed each time.
        *
        *  @since S60 3.2.3
        *  @param aPlayer UID of the subplayer
        *  @param aSubPlayers a list of sub players
        *  @param aComplete ETrue no more sub players. EFalse more subplayer
        *                   expected
        *  @param aError error code
        */
        virtual void HandleSubPlayerNamesL(TUid aPlayer, 
                                           const MDesCArray* aSubPlayers,
                                           TBool aComplete,
                                           TInt aError);
        
        /**
        *  Call back of media request.
        *
        *  @since S60 3.2.3
        *  @param aMedia media 
        *  @param aError error code    
        */
        virtual void HandleMediaL(const CMPXMedia& aProperties,
                                  TInt aError);
        
        /**
         *  Handle completion of a asynchronous command.
         *  Note: All clients should implement this callback.
         *
         *  @since S60 3.2.3
         *  @param aCommandResult result of the command, NULL if error
         *  @param aError error code    
         */
        virtual void HandlePlaybackCommandComplete( CMPXCommand* /*aCommandResult*/, 
                                                   TInt /*aError*/ );
        
    private: // Data
        MFusionMpxPlayerTesterObserver* iObserver;
        
        MMPXPlaybackUtility*      iPlaybackUtility;

        MMPXViewUtility*          iViewUtility;
        
        RArray<TExpectedGeneralMessage> iExpectedGeneralMessages;
        
        TMPXPlaybackState iState;
        
        TInt iVolume;
        TInt iVolumeBeforeMute;
        
        TBool iInitComplete;
        
        CIptvTestActiveWait* iWait;
        };

#endif // FUSIONMPXPLAYERTESTER_H
