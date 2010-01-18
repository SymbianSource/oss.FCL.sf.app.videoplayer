// Services which can be created with addhardcodedservice

// http://193.65.182.78/oops.xml
SERVICE_0               "OOPS"

// http://193.65.182.78/nokia_vod.xml
SERVICE_1               "NokiaVideos"

// http://193.65.182.78/testingfeed_iptvengine.xml
SERVICE_2               "Rocketboom"

// http://193.65.182.78/yle.xml
SERVICE_3		"YLE Mediasali"

// http://ws27.oyy.fi/rss/kanava.xml
SERVICE_4		"Kärppäkanava"

// These have same addresses than services above.
SERVICE_6		"OOPS6"
SERVICE_9		"OOPS9"
SERVICE_50		"OOPS50"
SERVICE_7		"NokiaVideos7"
SERVICE_5		"Rocketboom5"
SERVICE_8		"Rocketboom8"
SERVICE_10		"NSeries"
SERVICE_15		"InvalidService3"
SERVICE_16		"InvalidService4"

// This is created by IptvSetValidServices.cfg
// http://193.65.182.78/misc/smallvideos/smallvideos.xml
SERVICE_SMALLVIDEOS     "SmallVideos"

// This is created by IptvSetValidServices.cfg
// http://193.65.182.78/misc/smallvideos/smallvideos.xml
SERVICE_BIGVIDEOS     "BIGFAKEVIDEOS"

SERVICE_XMLTV            "XmlTvTestService"
SERVICE_THOMSON          "ThomsonTestService"

SERVICE_15IAPS		Service15Iaps
SERVICE_NOIAPS		ServiceNoIaps
// all data is uninitialized
SERVICE_UNINITIALIZED 	UNINITIALIZED
INVALID_SERVICE_1	InvalidService3
INVALID_SERVICE_2	InvalidService4

// Service names to be used when creating new services from the ones above
EXTRA_SERVICE_1		"ExtraService1"
EXTRA_SERVICE_2		"ExtraService2"
EXTRA_SERVICE_3		"ExtraService3"
EXTRA_SERVICE_4		"ExtraService4"
EXTRA_SERVICE_5		"ExtraService5"
EXTRA_SERVICE_6		"ExtraService6"
EXTRA_SERVICE_7		"ExtraService7"
EXTRA_SERVICE_8		"ExtraService8"
EXTRA_SERVICE_9		"ExtraService9"

VODSERVICE 		"Vod service"
INVALIDSERVICE 		"Invalid service"

SERVICE1 		"TestService1"
SERVICE2 		"TestService2"
SERVICE3 		"TestService3"
SERVICE4 		"TestService4"
SERVICE5 		"TestService5"
SERVICE5 		"TestService5"
SERVICE6 		"TestService6"

LIVETVTHOMSONSERVICE	"LiveTvThomsonService"
LIVETVXMLTVSERVICE	"LiveTvXmlTvService"
LIVETVSERVICESIEMENS1	"LiveTvSiemensService1"
LIVETVSERVICESIEMENS2	"LiveTvSiemensService2"
LIVETVSERVICESIEMENS3	"LiveTvSiemensService3"


// this is used only in the UpdateService test method
// this is needed to reference the uninitialized service, it has no name
LAST_SERVICE_IN_DB 60000