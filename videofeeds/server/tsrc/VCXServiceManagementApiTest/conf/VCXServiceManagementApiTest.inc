WLAN_AP_HIDDEN "HiddenWlan"
FILE c:\testframework\services.txt
NOT_SELECTED 0
SELECTED 1
TEST_IF_IAP_EXISTS 1

SYNC 0
ASYNC 1

PERMISSION_ALLOWED                      0
PERMISSION_NOT_ALLOWED                  1
PERMISSION_CONFIRM                      2

NEXT_VALID_HC_SERVICE                   40    // This will always add next service in the list, starting from TestService

ESearchNoFlags                          0
ESearchSelectedServices			1
ESearchMainServices			2
ESearchSubServices			4
ESearchVod				8
ESearchLiveTv				16
ESearchVodCast				32
ESearchBrowser				64
ESearchVideoRemote			128
ESearchApplication			256
ESearchMobileTv				512
ESearchVodServices			1024
ESearchServiceGroups			2048
ESearchGroupedServices			4096
ESearchOther				8192

// GetServicesFiltered sort flags
EDateAscending                          0
EDateDescending	                        1
EDisplayOrderAscending		        2
EDisplayOrderDescending                 3

EIptvSmNoSpecialAction                  0
EIptvSmCancelNextAsyncRequest           1
EIptvSmCorruptServicesDb                2
EIptvSmCorruptPasswordsDb               3
EIptvSmLockServicesDb                   4
EIptvSmLockPasswordsDb                  5
EIptvSmDropServicesTable                6
EIptvSmDropPasswordsTable               7
EIptvSmCreateDummyServicesTable	        8
EIptvSmCreateDummyPasswordsTable        9

// Network selection
EProcessing                             0
ESucceeded                              1
EGeneralError                           2
ENoSuitableIapFound                     3
EDoingWlanScan                          4
EServiceNotFound                        5
ECancelled                              6

// Responses from SM
// MIptvServiceManagementClientObserver::TRespStatus
SMESucceeded                            0
SMEServiceNotFound                      1
SMEGeneralError                         2
SMECancelled                            3

STRESSCOUNT1 20

TRUE 1
FALSE 0

PAUSE 5000

// Provision zip file
PROVISION_ZIP	"c:\testing\data\provisionzip\provision.zip"
UNZIPPED_SERVICE	"c:\testing\data\unzipped_service.xml"