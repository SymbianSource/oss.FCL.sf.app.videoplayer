'
' WScript that will invoke the test run execute functionality at ATS3 web server without installing any ATS3
' specific programs on the local PC.
'
' Password must be given in encrypted format and the path to test drop must be in URLEncoded
'
' Usage cscript wshRunX.vbs <username> <password> <server hostname> <path to testDrop.zip>
'

' Get the command line arguments
set args = WScript.Arguments

' Check that all arguments have been specified
Set objShell = WScript.CreateObject("WScript.Shell")
Set env = objShell.Environment("Process")
checkEnvVars(env)

' Invoke the web application and write the result to stdOut
WScript.StdOut.Write doTestRunX(env("ats3.username"), env("ats3.password"), env("ats3.host"), URLEncode( env("ats3.pathToDrop") ))

' Quit the script
Wscript.Quit

Function checkEnvVars(env)
	if env("ats3.username") = "" then
		WScript.Echo "Environment variable ats3.username not specified"
		WScript.Quit 1
	elseif env("ats3.password") = "" then
		WScript.Echo "Environment variable ats3.password not specified"
		WScript.Quit 1
	elseif env("ats3.host") = "" then
		WScript.Echo "Environment variable ats3.host not specified"
		WScript.Quit 1		
	elseif env("ats3.pathToDrop") = "" then
		WScript.Echo "Environment variable ats3.pathToDrop not specified"
		WScript.Quit 1	
	end if
End Function

' Invoke the ATS3 web application in given host with the specified username, password and file path
Function doTestRunX(uname, password, hostName, pathToDrop)
	Set objxmlHTTP = CreateObject("Microsoft.XMLHTTP")
	
	Call objxmlHTTP.open("GET", "http://" & hostName & "/ats3/XTestRunExecute.do?username=" & uname & "&password=" & password & "&testrunpath=" & pathToDrop, False)
	objxmlHTTP.Send()

	doTestRunX = objxmlHTTP.ResponseText
End Function

Function URLEncode(data)
	data = replace(data,"\","/")
	data = replace(data,"$","%24")
	data = replace(data,"&","%26")
	data = replace(data,"+","%2B")
	data = replace(data,",","%2C")
	data = replace(data,"/","%2F")
	data = replace(data,":","%3A")
	data = replace(data,";","%3B")
	data = replace(data,"=","%3D")
	data = replace(data,"?","%3F")
	data = replace(data,"@","%40")
	URLEncode = data
End Function