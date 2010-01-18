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
echo CLEAN VIDEOUTILS/TSRC
echo ----------------------------------------

pushd \VideoApp_Domain\videoutils\tsrc\group
call bldmake bldfiles
call abld test reallyclean
popd

echo ----------------------------------------
echo CLEAN VIDEO
echo ----------------------------------------

pushd \VideoApp_Domain\videoplayer\group
call bldmake bldfiles
call abld reallyclean
popd

echo ----------------------------------------
echo CLEAN VIDEO/TSRC
echo ----------------------------------------

pushd \VideoApp_Domain\videoplayer\tsrc\group
call bldmake bldfiles
call abld test reallyclean
popd
