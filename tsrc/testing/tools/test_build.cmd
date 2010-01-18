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

call pushd \VideoApp_Domain\videoplayer\tsrc\testing\tools
call test_clean.cmd
call popd

call pushd \VideoApp_Domain

echo ----------------------------------------
echo COPY LOG HEADERS
echo ----------------------------------------
call \VideoApp_Domain\videoplayer\tsrc\testing\tools\copylogheaders.cmd

echo ----------------------------------------
echo BUILD VIDEOUTILS
echo ----------------------------------------

call pushd videoutils\group
call bldmake bldfiles
call abld build armv5 udeb -k
call abld build winscw udeb -k
call popd

echo ----------------------------------------
echo BUILD VIDEO
echo ----------------------------------------

call pushd video\group
call bldmake bldfiles
call abld build armv5 udeb -k
call abld build winscw udeb -k
call popd

echo ----------------------------------------
echo BUILD VIDEOUTILS/TSRC
echo ----------------------------------------

call pushd videoutils\tsrc\group
call bldmake bldfiles
call abld test build armv5 udeb -k
call abld test build winscw udeb -k
call popd

echo ----------------------------------------
echo BUILD VIDEO/TSRC
echo ----------------------------------------

call pushd video\tsrc\group
call bldmake bldfiles
call abld test build armv5 udeb -k
call abld test build winscw udeb -k
call popd

echo ----------------------------------------
echo CREATE VIDEOPLAYER SIS
echo ----------------------------------------
call pushd \VideoApp_Domain\videoplayer\videoplayerapp\mpxvideoplayer\sis
call pushd \VideoApp_Domain\videoplayer\mediasettings\sis
call attrib -r -a -s -h *.pkg
call perl -pi.bak -e "s/urel/udeb/g" mediasettings.pkg
call popd
call attrib -r -a -s -h *.pkg
call perl -pi.bak -e "s/urel/udeb/g" VideoPlayer.pkg
call perl -pi.bak -e "s/urel/udeb/g" disable_mediaplayer.pkg
call make_videoplayer_sisx.bat
call popd

call popd