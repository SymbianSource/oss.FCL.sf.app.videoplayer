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

echo ----------------------------------------
echo CLEAN VIDEOUTILS
echo ----------------------------------------

pushd \VideoApp_Domain\videoutils\group
call bldmake bldfiles
call abld reallyclean
popd

echo ----------------------------------------
echo CLEAN VIDEOPLAYER
echo ----------------------------------------

pushd \VideoApp_Domain\videoplayer\group
call bldmake bldfiles
call abld reallyclean
popd

echo ----------------------------------------
echo CLEAN VIDEOUTILS/TSRC
echo ----------------------------------------

pushd \VideoApp_Domain\videoutils\tsrc\group
call bldmake bldfiles
call abld test reallyclean
popd

echo ----------------------------------------
echo CLEAN VIDEOPLAYER/TSRC
echo ----------------------------------------

pushd \VideoApp_Domain\videoplayer\tsrc\group
call bldmake bldfiles
call abld test reallyclean
popd

echo ----------------------------------------
echo CLEAN VIDEOPLAYER UNIT TESTS
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\videoplayback\videohelix\tsrc\ut_videohelixtest\group
call bldmake bldfiles
call abld test reallyclean
call popd

call pushd \VideoApp_Domain\videoplayer\videoplayback\videoplaybackcontrols\tsrc\videoplaybackcontrols_test\group
call bldmake bldfiles
call abld test reallyclean
call popd

call pushd \VideoApp_Domain\videoplayer\videoplayback\videoplaybackviews\tsrc\ut_userinputhandlertest\group
call bldmake bldfiles
call abld test reallyclean
call popd

call pushd \VideoApp_Domain\videoplayer\videoplayerapp\mpxvideoplayer\tsrc\ut_mpxvideoplayertest\group
call bldmake bldfiles
call abld test reallyclean
call popd