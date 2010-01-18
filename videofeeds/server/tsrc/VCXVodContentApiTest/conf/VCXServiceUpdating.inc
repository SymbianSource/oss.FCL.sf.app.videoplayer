
EId 0 
EName 1
EAddress 2
EIconPath 3
EEpgPluginUid 4
EVodPluginUid 5
EIptvPluginUid 6
EServiceType 7
ESetIapList 8
EUserName 9
EPassword 10
EDesc 11
EFlags 12
EApplicationUid 13
EDisplayOrder 14
EProviderId 15
EAccountManagementUrl 16
EScheduleDlTime 17 // parameters: scheduled times of the day
EScheduleDlNetwork 18 // parameter: AP name
EScheduleDlType 19 // parameters: schedule type and the number of the possible downloads
EScheduleLastDlTime 20 // time used is the current current
ESearchUrl 21 // parameters: schedule type and the number of the possible downloads

// few extra parameters for the fields above
EAddress_2kLongUrl              "Address_2kLongUrl"
EAddress_256charsLongSearchUrl  "Address_256charsLongSearchUrl"
EAddress_256LongFileName        "Address_256CharsLongFileName"

// Schedule times
ENoSchedule  0
ENight       1 
EMorning     2
ENoon        4
EAfternoon   8
EEvening     16

// Schedule network
EManual       0
EWLAN         1
EHomeCellular 2
EAlways       3

// Service flags
ESelected 1
EReadOnly 2
EMainService 4
EConnectionApproved 8
EReadOnlyIaps 16
EGroupedService 32
EGroupReceived 64

// Service types
EVod 0
ELiveTv 1
EVodCast 2
EBrowser 3
EVideoRemote 4
EApplication 5
EMobileTv 6
ESecureProvisioningServer 7
EVideoStore 8
EServiceGroup 9
ESearch 10
EUpload 11
EOther 12



// Schedule download types
EIptvTestNoDownload          0
EIptvTestDownloadLatest      1
EIptvTestDownloadOldest      2
EIptvTestDownloadAddedAfterLastScheduledDownload 3
EIptvTestDownloadAll         4
