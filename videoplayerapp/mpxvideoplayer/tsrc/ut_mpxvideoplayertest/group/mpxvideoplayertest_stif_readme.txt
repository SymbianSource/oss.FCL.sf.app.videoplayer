This document lists test scenarios you can run STIF tests and STIF with CTC++ (code coverage)

This project, mpxvideoplayertest.mmp, is built as part of PRJ_TESTMMP
so that it can be run as part of the MultiTest runner. The STIF .cfg and .ini
files are exported as part of the build for this. 

-------------------------------------
I. STIF Test on Emulator - No coverage
--------------------------------------
1) From Carbide, choose Carbide > MultiTest Runner
2) Choose the static analysis tests you want to run (or none)
3) Enable the "EUnit or STIF" check box and click Next
4) Make sure the build target platform is set to the emulator
5) Under "Select Tests" choose the mpxvideoplayer_test.mmp and check "STIF"
6) Select the .cfg and .ini files:
    \VideoApp_Domain\VideoApp_Domain\video\mpxvideoplayer\tsrc\ut_mpxvideoplayertest\conf\mpxvideoplayer_test.cfg
    \VideoApp_Domain\VideoApp_Domain\video\mpxvideoplayer\tsrc\ut_mpxvideoplayertest\init\TestFramework.ini
7) Click Finish

The test will run. If there are any errors check the log and enable "Debug" when running the tests from the
MultiTestrunner menu.


---------------------------------------------
II. CTC++ Code Coverage with STIF On Device
----------------------------------------------
CTC++ won't work with STIF so you need to run this on device.

Follow these steps:

1)  Create a CTC++ instrumented build with the following commands:
        abld test reallyclean armv5 udeb
        ctcwrap -i f abld test build armv5 udeb

2)  Create a sis for with the mpxvideoplayer.pkg and sign it with the R&D keys
        call makesis.exe mpxvideoplayertest.pkg mpxvideoplayertest.sis
        signsis.exe mpxvideoplayertest.sis mpxvideoplayertest.sisx rd.cer rd-key.pem

3)  Install mpxvideoplayertest.sisx on the device

4)  Launch STIF and execute test

5)  Launch CTC++ and write data to ctcdata.txt file (C:\data\ctc\)

5)  Copy ctcdata.txt to your PC and process the file with the following commands and 
    results will be written and function coverage displayed:
        ctcrecvr -i ctcdata.txt
        ctcpost -p profile.txt
        ctc2html -i profile.txt
