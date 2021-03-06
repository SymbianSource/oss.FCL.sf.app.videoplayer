
VCXCollectionMyVideos "20016B97" old
// 0 is mapped to the VCXCollectionMyVideos constant
//VCXCollectionMyVideos "0"

VCXCollectionMusic "101FFC3A"
VCXCollectionPodcast "101FFC3C"

// Index, not id of the item. always same?
// From vcxmyvideoscollection.hrh
EVcxMyVideosAllVideos      0
//EVcxMyVideosPlaylists
EVcxMyVideosDownloads      1
EVcxMyVideosTvRecordings   2
EVcxMyVideosCapturedVideos 3 // NO SUPPORT ON UI
EVcxMyVideosOther          4
//EVcxMyVideosDownloadList   5 // NOT SUPPORTED

// Download state
EVcxMyVideosDlStateNone 0
EVcxMyVideosDlStateDownloading 1
EVcxMyVideosDlStateFailed 2
EVcxMyVideosDlStatePaused 3
EVcxMyVideosDlStateDownloaded 4

// Origin
EVcxMyVideosOriginOther 0
EVcxMyVideosOriginCapturedWithCamera 1
EVcxMyVideosOriginDownloaded 2
EVcxMyVideosOriginSideLoaded 3 // sideloaded videos to actually to the first cat
EVcxMyVideosOriginTvRecording 4

// Sorting order
EVcxMyVideosSortingNone 0
EVcxMyVideosSortingId 1
EVcxMyVideosSortingCreationDate 2
EVcxMyVideosSortingModified 3
EVcxMyVideosSortingName 4
EVcxMyVideosSortingSize 5

SIDELOADER_WAIT 25000
PAUSE_MV_CASE_BEGIN 500
MV_LOOP_COUNT 100

// From CIptvTestVideoCreator.h
IptvTestVideo3Gp            0       // Extension .3GP
IptvTestVideo3Gp2           1       // Extension .3G2
IptvTestVideoMp2            2       // Extension .MP2
IptvTestVideoMpeg1          3       // Extension .MPG
IptvTestVideoMpeg2          4       // Extension .MPG
IptvTestVideoSuperVideoCd   5       // Extension .MPG
IptvTestVideoMp3            6       // Extension .MP3
IptvTestVideoAppleMpeg      7       // Extension .MP4
IptvTestVideoMpeg4          8       // Extension .MP4
IptvTestVideoMpegAvc        9       // Extension .MP4
IptvTestVideoQuicktime      10      // Extension .MOV
IptvTestVideoRealVideo      11      // Extension .RM
IptvTestVideoAvi            12      // Extension .AVI
IptvTestVideoWmv            13      // Extension .WMV
IptvTestVideoAviAC3         14      // Extension .AVI

PAUSE_SIDELOAD              30000   // 30 seconds
PAUSE_SIDELOAD_LONG         60000   // a minute
PAUSE_SIDELOAD_VERY_LONG    300000  // 10 minutes

KVCXMYVideosTestMessageNotAccepted                    0

KVCXMYVideosTestMessageDlSucceeded                    1000
KVCXMYVideosTestMessageDlFailed                       1001
KVCXMYVideosTestMessageDlPaused                       1002
KVCXMYVideosTestMessageDlGeneral                      1003
KVCXMYVideosTestMessageDlRunning                      1020
KVCXMYVideosTestMessageDlCanceled                     1021
KVCXMYVideosTestMessageDlProgressed                   1022

KVCXMYVideosTestMessageCollectionMessageError         2000
KVCXMYVideosTestMessageCollectionOpened               2001
KVCXMYVideosTestMessagePlaylistOpened                 2002
KVCXMYVideosTestMessageCollectionMedia                2003
KVCXMYVideosTestMessageCommandComplete                2004
KVCXMYVideosTestMessageGotMediasByKMpxId              2005
KVCXMYVideosTestMessageMoveResp                       2006
KVCXMYVideosTestMessageCopyResp                       2007
KVCXMYVideosTestMessageListComplete                   2008
KVCXMYVideosTestMessageCopyOrMoveStarted              2009
KVCXMYVideosTestMessageDeleteStarted                  2010
KVCXMYVideosTestMessageDeleteResp                     2011
KVCXMYVideosTestMessageCommandCanceled                2012

KVCXMYVideosTestMessageCollectionGeneral              3000
KVCXMYVideosTestMessageCollectionItemChanged          3001
KVCXMYVideosTestMessageCollectionCategoryChanged      3002

KVCXMYVideosTestMessageMpxItemInserted                4000
KVCXMYVideosTestMessageMpxItemDeleted                 4001
KVCXMYVideosTestMessageMpxItemModified                4002

KVCXMyVideosTestMessageVideoListOrderChanged          5000

KVCXMYVideosTestErrorDownloadNotFound                 10001
KVCXMYVideosTestErrorFileNotFound                     10002

10SECONDS 10
30SECONDS 30
1MINUTE 60
2MINUTES 120
5MINUTES 300
10MINUTES 600
15MINUTES 900
30MINUTES 2300

VCXMYVideosTest2kURL                                 "XXX2kURLXXX"
VCXMYVideosTest1200charsURL                          "1kcharsurl"
VCXMYVideosTest1kURL                                 "XXX1kURLXXX"
VCXTestZeroUrl                                       "VcxTestZeroUrl"

// Iap ID is 0
VCXTestZeroIap                                       "VcxTestZeroIap"
// Iap ID is 10001
VCXTestInvalidIap                                    "VcxTestInvalidIap"


// Attribute mapping IDs
// MPX ID (BRIEF), TMPXItemId, iId2  - Ready only.
EVcxTestMapping_KMPXMediaGeneralId 1
// Descriptor, contains video name (BRIEF)
EVcxTestMapping_KMPXMediaGeneralTitle 2
// Descriptor, description of the video
EVcxTestMapping_KMPXMediaGeneralComment 3
//  Descriptor. File path. (BRIEF)
EVcxTestMapping_KMPXMediaGeneralUri 4
//  TInt, size in bytes. (BRIEF, OBSOLETE)
EVcxTestMapping_KMPXMediaGeneralSize 5
//	TInt64, size in bytes. (BRIEF)
EVcxTestMapping_KMPXMediaGeneralExtSizeInt64 6
// TInt64 - Ready only.
EVcxTestMapping_KMPXMediaGeneralDate 7
//  TUint32
EVcxTestMapping_KMPXMediaGeneralFlags 8
//  Descriptor
EVcxTestMapping_KMPXMediaGeneralCopyright 9
// Descriptor - Ready only.
EVcxTestMapping_KMPXMediaGeneralMimeType 10
// TInt64
EVcxTestMapping_KVcxMediaMyVideosModifiedDate 11
// TInt32, Age profile / rating age. (BRIEF)
EVcxTestMapping_KVcxMediaMyVideosAgeProfile 12
// Descriptor
EVcxTestMapping_KVcxMediaMyVideosAudioLanguage 13
// Descriptor
EVcxTestMapping_KVcxMediaMyVideosAuthor 14
// TUint8, The origin of the object, see values from TVcxMyVideosOrigin.
EVcxTestMapping_KVcxMediaMyVideosOrigin 15
// TReal32, duration of the video (play time).
EVcxTestMapping_KVcxMediaMyVideosDuration 16
// TReal32, last playback position in seconds.
EVcxTestMapping_KMPXMediaGeneralLastPlaybackPosition 17
// TUint32. Download ID in Download Manager, 0 means none.
EVcxTestMapping_KVcxMediaMyVideosDownloadId 18

// Rest are not saved into MDS

//Descriptor. remote url. (BRIEF)
EVcxTestMapping_KVcxMediaMyVideosRemoteUrl 19
// TUint8, download state, see values from TVcxMyVideosDownloadState. (BRIEF)
EVcxTestMapping_KVcxMediaMyVideosDownloadState 20
// TUint32, IAP ID in CommsDb.
EVcxTestMapping_KVcxMediaMyVideosIapId 21
// Descriptor. Username. This is used in KVcxCommandMyVideosStartDownload only.
EVcxTestMapping_KVcxMediaMyVideosUsername 22
// Descriptor. Password. This is used in KVcxCommandMyVideosStartDownload only.
EVcxTestMapping_KVcxMediaMyVideosPassword 23
// TUint. My Videos command/message ID. Used when sending commands to my videos collection
EVcxTestMapping_KVcxMediaMyVideosCommandId 24
// TInt8. Download progress. My Videos collection updates this value when download is
EVcxTestMapping_KVcxMediaMyVideosDownloadProgress 25
// TUint32, This attribute is only used in category items. Indicates how many items are under it.
EVcxTestMapping_KVcxMediaMyVideosCategoryItemCount 26
//  TUint32. This attribute is only used in category items. Indicates how many new items are under it.
EVcxTestMapping_KVcxMediaMyVideosCategoryNewItemCount 27

// Same as before modified date but when used will set the time to current
EVcxTestMapping_KVcxMediaMyVideosModifiedDateCurrent 28
EVcxTestMapping_KVcxMediaMyVideosCategoryNewItemName 29
EVcxTestMapping_KVcxMediaMyVideosAudioFourCc 30

// Flags
// New video flag.
EVcxMyVideosVideoNew                1
// Scheduled flag, not downloaded yet.
EVcxMyVideosVideoScheduledDownload  2
// Deletion warning flag.
EVcxMyVideosVideoDeleteWarning      4
// Protection flag.
EVcxMyVideosVideoProtected          8
// DRM protection flag.
EVcxMyVideosVideoDrmProtected       16
// Video located at the memorycard flag.
EVcxMyVideosVideoMemoryCard         32
// Preview, not full clip.
EVcxMyVideosVideoPreview            64
// Recording partially failed  // DUPLICATE in harvesterrtpmetadatarader.h
EVcxMyVideosVideoRecPartiallyFailed 128
// Recording failed  // DUPLICATE in harvesterrtpmetadatarader.h
EVcxMyVideosVideoRecFailed          256
EVcxMyVideosServiceHasReadOnlyIap   512
// Set to indicate that there is no UI present, used in KVcxCommandMyVideosStartDownload command only
EVcxMyVideosSilent                  1024


FREESPACE_100KB                     102400
FREESPACE_500KB                     512000
FREESPACE_4MBPLUS100KB              4296704
FREESPACE_1MB                       1024000
FREESPACE_2MB                       2048000
FREESPACE_5MB                       5120000
FREESPACE_10MB                      10240000

// http://193.65.182.78/nokia_vod.xml
SERVICE_1                          "NokiaVideos"
// http://193.65.182.78/testingfeed_iptvengine.xml
SERVICE_2                          "Enginetestingfeed"
// http://193.65.182.78/miscvideos.xml
SERVICE_MISCVIDEOS                          "Miscvideos"
// http://193.65.182.78/misc/smallvideos/smallvideos.xml
SERVICE_SMALLVIDEOS                "SmallVideos"


KVcxNsMpxEventDownloadStarted            1
KVcxNsMpxEventAllDownloadsEnded          2
KVcxNsMpxEventDownloadCompleted          4 
KVcxNsMpxEventNewVideosCountDecreased    5
