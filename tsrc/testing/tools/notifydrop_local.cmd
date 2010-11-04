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

set ats3.username=admin
set ats3.password=5A3CBBF36BEDD7E99D3606C3F45DD57C
set ats3.host=%1
set ats3.pathToDrop=%2

rem If no attributes have been set, show usage.
if "%1" == "" (goto Usage) else (goto SetCredentials)

rem Set the credentials. If third attribute is set, use given username and password instead of default
:SetCredentials
if "%3" == "" (goto Execute)
set ats3.username=%3
set ats3.password=%4
set ats3.schedule=%5
goto Execute

rem Execute the vbs script that will invoke the web app which will start the test run
:Execute
cscript wshTestRun_local.vbs
goto End

:Usage
echo.
echo.
echo USAGE: notifyimport.cmd ^<ATS3 web server hostname^> ^<path to zip containing ATS3 testrun^> ^[^<username^>^ ^<password^>]
echo.
echo NOTE: username is 'admin' and password is default admin password is either plain text or in encrypted form. Both username and password are defined in notifyimport.cmd
echo.
echo EXAMPLES:
echo  notifyimport.cmd someserver.somehost.com c:\the_test_drop.zip
echo  notifyimport.cmd someserver.somehost.com c:\the_test_drop.zip myUserName myPassword

:End