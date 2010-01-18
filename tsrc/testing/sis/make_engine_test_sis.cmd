@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

@echo off

call pushd \VideoApp_Domain
call \VideoApp_Domain\videoplayer\tsrc\testing\tools\tsrpkgtool.pl -a
call popd

echo ----------------------------------------
echo VIDEOPLAYER - MEDIASETTINGSENGINETEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\mediasettings\mediasettingsengine\tsrc\mediasettingsenginetest\group
call makesis mpxmediasettingsenginetest.pkg mediasettingsenginetest.sis
call copy mediasettingsenginetest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe mediasettingsenginetest.sis mediasettingsenginetest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del mediasettingsenginetest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VIDEOPLAYERSETTINGSENGINETEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\mediasettings\videoplayersettingsengine\tsrc\vpsettingsenginetest\group
call makesis vcxnssettingsenginetest.pkg vcxnssettingsenginetest.sis
call copy vcxnssettingsenginetest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe vcxnssettingsenginetest.sis vcxnssettingsenginetest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del vcxnssettingsenginetest.sis

echo ----------------------------------------
echo VIDEOPLAYER - MPXMYVIDEOSCOLLECTIONTEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videocollection\mpxmyvideoscollection\tsrc\mpxmvcolltest\group
call makesis VCXMyVideosCollectionPluginTest.pkg VCXMyVideosCollectionPluginTest.sis
call copy VCXMyVideosCollectionPluginTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VCXMyVideosCollectionPluginTest.sis VCXMyVideosCollectionPluginTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VCXMyVideosCollectionPluginTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VCXPROVISIONINGTEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videofeeds\server\tsrc\VCXProvisioningTest\group
call makesis VCXProvisioningTest.pkg VCXProvisioningTest.sis
call copy VCXProvisioningTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VCXProvisioningTest.sis VCXProvisioningTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VCXProvisioningTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VCXSCHEDULEDREMINDERTEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videofeeds\server\tsrc\VCXScheduledReminderTest\group
call makesis VCXScheduledReminderTest.pkg VCXScheduledReminderTest.sis
call copy VCXScheduledReminderTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VCXScheduledReminderTest.sis VCXScheduledReminderTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VCXScheduledReminderTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VCXSCHEDULEDDOWNLOADTEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videofeeds\server\tsrc\VCXScheduledDownloadTest\group
call makesis VCXScheduledDownloadTest.pkg VCXScheduledDownloadTest.sis
call copy VCXScheduledDownloadTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VCXScheduledDownloadTest.sis VCXScheduledDownloadTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VCXScheduledDownloadTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VCXSERVICEMANAGEMENTAPITEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videofeeds\server\tsrc\VCXServiceManagementApiTest\group
call makesis VCXServiceManagementApiTest.pkg VCXServiceManagementApiTest.sis
call copy VCXServiceManagementApiTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VCXServiceManagementApiTest.sis VCXServiceManagementApiTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VCXServiceManagementApiTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VCXVODCONTENTAPITEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videofeeds\server\tsrc\VCXVodContentApiTest\group
call makesis VCXVodContentApiTest.pkg VCXVodContentApiTest.sis
call copy VCXVodContentApiTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VCXVodContentApiTest.sis VCXVodContentApiTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VCXVodContentApiTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VCXSETTINGSDATABASETEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videofeeds\server\tsrc\VCXSettingsDatabaseTest\group
call makesis VCXSettingsDatabaseTest.pkg VCXSettingsDatabaseTest.sis
call copy VCXSettingsDatabaseTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VCXSettingsDatabaseTest.sis VCXSettingsDatabaseTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VCXSettingsDatabaseTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VCXIAPLISTTEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videofeeds\server\tsrc\VCXIapListTest\group
call makesis VCXIapListTest.pkg VCXIapListTest.sis
call copy VCXIapListTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VCXIapListTest.sis VCXIapListTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VCXIapListTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VCXSERVICETEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videofeeds\server\tsrc\VCXServiceTest\group
call makesis VCXServiceTest.pkg VCXServiceTest.sis
call copy VCXServiceTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VCXServiceTest.sis VCXServiceTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VCXServiceTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - VIDEOPLAYER_STARTUP_API
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videoplayer_plat\videoplayer_startup_api\tsrc\group
call makesis VideoPlayerStartupApiTest.pkg VideoPlayerStartupApiTest.sis
call copy VideoPlayerStartupApiTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe VideoPlayerStartupApiTest.sis VideoPlayerStartupApiTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VideoPlayerStartupApiTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - MPX_FUSION_API
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\tsrc\fusionmpxplayer_test\group
call makesis FusionMpxPlayer_ApiTest.pkg FusionMpxPlayer_ApiTest.sis
call copy FusionMpxPlayer_ApiTest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe FusionMpxPlayer_ApiTest.sis FusionMpxPlayer_ApiTest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del FusionMpxPlayer_ApiTest.sis

echo ----------------------------------------
echo VIDEOPLAYER - UT_MPXVIDEOPLAYER
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videoplayerapp\mpxvideoplayer\tsrc\ut_mpxvideoplayertest\group
call makesis mpxvideoplayertest.pkg mpxvideoplayertest.sis
call copy mpxvideoplayertest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe mpxvideoplayertest.sis mpxvideoplayertest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del mpxvideoplayertest.sis

echo ----------------------------------------
echo VIDEOPLAYER_PLAT - FILE_DETAILS_DIALOG_API
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videoplayer_plat\video_player_file_details_dialog_api\tsrc\group
call makesis ui_FileDetailsDialogtest.pkg ui_FileDetailsDialogtest.sis
call copy ui_FileDetailsDialogtest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe ui_FileDetailsDialogtest.sis ui_FileDetailsDialogtest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del ui_FileDetailsDialogtest.sis

echo ----------------------------------------
echo VIDEOUTILS - VIDEOSCHEDULER_APITEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoutils\videoutils_plat\videoscheduler_api\tsrc\group
call makesis videoscheduler_apitest.pkg videoscheduler_apitest.sis
call copy videoscheduler_apitest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe videoscheduler_apitest.sis videoscheduler_apitest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del videoscheduler_apitest.sis

echo ----------------------------------------
echo VIDEOUTILS - VIDEOCONNUTILITY_APITEST
echo ----------------------------------------

call pushd \VideoApp_Domain\videoutils\videoutils_plat\videoconnutility_api\tsrc\group
call makesis videoconnutility_apitest.pkg videoconnutility_apitest.sis
call copy videoconnutility_apitest.sis \VideoApp_Domain\videoplayer\tsrc\testing\sis
call popd
call signsis.exe videoconnutility_apitest.sis videoconnutility_apitest.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del videoconnutility_apitest.sis

echo ----------------------------------------
echo WHOLE SET
echo ----------------------------------------

call makesis fusion_engine_tests.pkg fusion_engine_tests.sis
call signsis.exe fusion_engine_tests.sis fusion_engine_tests.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del fusion_engine_tests.sis

call pushd \VideoApp_Domain
call \VideoApp_Domain\videoplayer\tsrc\testing\tools\tsrpkgtool.pl -a
call popd