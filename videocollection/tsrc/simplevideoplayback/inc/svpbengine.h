#ifndef SVPBENGINE_H
#define SVPBENGINE_H

#include <qobject>
#include <mpxplaybackobserver.h>
#include "svpbsurfacecontainer.h"

class MMPXPlaybackUtility;

enum SvpbEngineState
{
    Disconnected = 0,
    Connected, // == Stopped
    Initializing,
    MediaRequested,
    PlayRequested,
    Playing,
    PauseRequested,
    Paused,
    StopRequested
};

class SvpbEngine : public QObject,
    public MMPXPlaybackObserver,
    public MMPXPlaybackCallback
{
    Q_OBJECT

public:
    SvpbEngine(QObject *parent=0);
    virtual ~SvpbEngine();

    void connectMPX();
    void disconnectMPX();

    void setSurfaceContainer(SvpbSurfaceContainer *surfaceContainer);

signals:
    void finished();

public slots:
    void togglePause();
    void stop();

private: // From MMPXPlaybackObserver
    void HandlePlaybackMessage(CMPXMessage* aMessage, TInt aErr);

private: // from MMPXPlaybackCallback
    void HandlePropertyL(TMPXPlaybackProperty aProperty, TInt aValue, TInt aError);
    void HandleSubPlayerNamesL(TUid aPlayer, const MDesCArray* aSubPlayers,
                               TBool aComplete, TInt aError);
    void HandleMediaL(const CMPXMedia& aProperties, TInt aError);

private:
    void mpxPbStateChanged(TMPXPlaybackState type, int err);
    void requestMedia();

private: // data
    SvpbSurfaceContainer *mSurfaceContainer;
    SvpbEngineState mState;
    MMPXPlaybackUtility *mPlaybackUtility;
    TMPXPlaybackState mMpxPbState;
};

#endif // SVPBENGINE_H
