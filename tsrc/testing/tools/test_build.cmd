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
echo LOGGING HEADERS
echo ----------------------------------------
call \VideoApp_Domain\videoplayer\tsrc\testing\tools\copylogheaders.cmd

echo ----------------------------------------
echo ADD COMMENTS TO .PKG AND .INF
echo ----------------------------------------
call pushd \VideoApp_Domain\
call \VideoApp_Domain\videoplayer\tsrc\testing\tools\tsrpkgtool.pl -a
call popd

echo ----------------------------------------
echo CLEANUP
echo ----------------------------------------
call pushd \VideoApp_Domain\videoplayer\tsrc\testing\tools
call test_clean.cmd
call popd

echo ----------------------------------------
echo BUILD VIDEOUTILS
echo ----------------------------------------

call pushd \VideoApp_Domain\videoutils\group
call bldmake bldfiles
call abld build armv5 udeb -k
call abld build winscw udeb -k
call popd

echo ----------------------------------------
echo BUILD VIDEO
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\group
call bldmake bldfiles
call abld build armv5 udeb -k
call abld build winscw udeb -k
call popd

echo ----------------------------------------
echo BUILD VIDEOUTILS/TSRC
echo ----------------------------------------

call pushd \VideoApp_Domain\videoutils\tsrc\group
call bldmake bldfiles
call abld test build armv5 udeb -k
call abld test build winscw udeb -k
call popd

echo ----------------------------------------
echo BUILD VIDEO/TSRC
echo ----------------------------------------

call pushd \VideoApp_Domain\videoplayer\tsrc\group
call bldmake bldfiles
call abld test build armv5 udeb -k
call abld test build winscw udeb -k
call popd

echo ----------------------------------------
echo CREATE VIDEOPLAYER SIS
echo ----------------------------------------
call pushd \VideoApp_Domain\videoplayer\videoplayerapp\mpxvideoplayer\sis
call \VideoApp_Domain\videoplayer\tsrc\testing\tools\setversion.pl -f VideoPlayer.pkg
call make_videoplayer_sisx.bat
call popd

echo ----------------------------------------
echo BUILD VIDEOPLAYER UNIT TESTS
echo ----------------------------------------

call pushd videoplayer\videoplayback\videohelix\tsrc\ut_videohelixtest\group
call bldmake bldfiles
call abld test build
call popd

call pushd videoplayer\videoplayback\videoplaybackcontrols\tsrc\videoplaybackcontrols_test\group
call bldmake bldfiles
call abld test build
call popd

call pushd videoplayer\videoplayback\videoplaybackviews\tsrc\ut_userinputhandlertest\group
call bldmake bldfiles
call abld test build
call popd

call pushd videoplayer\videoplayerapp\mpxvideoplayer\tsrc\ut_mpxvideoplayertest\group
call bldmake bldfiles
call abld test build
call popd

call popd

echo ----------------------------------------
echo REMOVE COMMENTS FROM .PKG & .INF
echo ----------------------------------------
call pushd \VideoApp_Domain\
call \VideoApp_Domain\videoplayer\tsrc\testing\tools\tsrpkgtool.pl -r
call popd