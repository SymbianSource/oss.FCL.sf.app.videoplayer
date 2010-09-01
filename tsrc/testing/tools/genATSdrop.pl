#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

#------------------------------------------------------------------------------------
# Includes
#------------------------------------------------------------------------------------
use strict;
use warnings;
use Cwd; # for cwd
use FindBin; # for FindBin:Bin
use File::Path; # for mkpath
use Date::Calc;
use File::Copy;
use lib "$FindBin::Bin/../lib";

{ # No globals.
    my $cgidir = GetCgiDir();
    
    if ( ! -e( $cgidir . "rerunsubs.pl" ) )
    {
        $cgidir = cwd;
        my $domain = "VideoApp_Domain";
        my $pos = index( $cgidir, $domain );
        if( $pos != -1 )
        {
            $cgidir = substr( $cgidir, 0, $pos + length( $domain ) ) . "\\videoplayer\\tsrc\\testing\\tools\\";
            
        }
    }
    require( $cgidir . "utils.pl" );
    require( $cgidir . "rerunsubs.pl" );
}

#------------------------------------------------------------------------------------
# GLOBAL CODE
#------------------------------------------------------------------------------------

my $basePath = "testdrop"; # ATS XML
my $dropFilePath = "ATS3Drop\\"; # All other files
my $dropImagePath = "ATS3Drop\\images\\";
my $dropBinaryPath = "ATS3Drop\\armv5_urel\\"; # Binaries

my $currentPlanId = 0;
my $currentSessionId = 0;
my $currentSetId = 0;
my $currentCaseId = 0;
my $currentStepId = 0;

# Attributes for files which are copied to device. Or SIS installations.
my $ATSFILE_NAME = 0;
my $ATSFILE_HARNESS = 1;
my $ATSFILE_ENABLED = 2;
my $ATSFILE_PASSRATE = 3;
my $ATSFILE_SIGNIFICANT = 4;
my $ATSFILE_TYPE = 5;
my $ATSFILE_SRC = 6;
my $ATSFILE_DST = 7;
my $ATSFILE_COMMAND = 9;
my $ATSFILE_DROPFULL_PATH = 10; # Contains the path in drop to the file
my $ATSFILE_STEP_XML = 11; # XML defining the step
my $ATSFILE_STEP_TIMEOUT = 12; # only for SIS file

# These must be replaced when writing a step into XML file
my $ATSXML_DROPPRIORITY = "ATSXML_DROPPRIORITY";
my $ATSXML_TESTNAME = "ATSXML_TESTNAME";
my $ATSXML_FLASH_IMAGE_NAME = "ATSXML_FLASH_IMAGE_NAME";
my $ATSXML_TESTDEVICE = "ATSXML_TESTDEVICE";
my $ATSXML_TESTPLAN_NAME = "ATSXML_TESTPLAN_NAME";
my $ATSXML_TESTSESSION_NAME = "ATSXML_TESTSESSION_NAME";
my $ATSXML_TESTSET_NAME = "ATSXML_TESTSET_NAME";
my $ATSXML_STEP_NAME = "ATSXML_STEP_NAME";
my $ATSXML_TESTCASE_NAME = "ATSXML_TESTCASE_NAME";
my $ATSXML_STEP_HARNESS = "ATSXML_STEP_HARNESS";
my $ATSXML_STEP_ENABLED = "ATSXML_STEP_ENABLED";
my $ATSXML_STEP_PASSRATE = "ATSXML_STEP_PASSRATE";
my $ATSXML_STEP_SIGNIFICANT = "ATSXML_STEP_SIGNIFICANT";
my $ATSXML_STEP_COMMAND = "ATSXML_STEP_COMMAND";
my $ATSXML_STEP_PARAM_TYPE = "ATSXML_STEP_PARAM_TYPE";
my $ATSXML_STEP_PARAM_SRC = "ATSXML_STEP_PARAM_SRC";
my $ATSXML_STEP_PARAM_DST = "ATSXML_STEP_PARAM_DST";
my $ATSXML_STEP_RUNCASES_FILENAME = "ATSXML_STEP_RUNCASES_FILENAME";
my $ATSXML_STEP_RUNCASES_TIMEOUT = "ATSXML_STEP_RUNCASES_TIMEOUT";
#my $ATSXML_STEP_PARAM_COMPONENT_PATH = "ATSXML_STEP_PARAM_COMPONENT_PATH";
my $ATSXML_STEP_RUNCASE_CASENAME = "ATSXML_STEP_RUNCASE_CASENAME";
my $ATSXML_STEP_RUNCASE_FILENAME = "ATSXML_STEP_RUNCASE_FILENAME";
my $ATSXML_EMAIL = "ATSXML_EMAIL";
my $ATSXML_SIS_FILENAME = "ATSXML_SIS_FILENAME";
my $ATSXML_STEP_TIMEOUT = "ATSXML_STEP_TIMEOUT";
my $ATSXML_FETCH_PATH = "ATSXML_FETCH_PATH";
my $ATSXML_CTC_PATH = "ATSXML_CTC_PATH";

# These are initialized in the InitXml()
my $ATSXML_HEADER_START;
my $ATSXML_HEADER_END;
my $ATSXML_FOOTER;
my $ATSXML_PLANSTART;
my $ATSXML_CREATEDIRS;
my $ATSXML_FLASH;
my $ATSXML_FORMAT_DRIVES;
my $ATSXML_PLANEND;
my $ATSXML_STEP;
my $ATSXML_FETCH_STEPS;
my $ATSXML_FETCH_STEP;
my $ATSXML_CTCWRITE_STEP;
my $ATSXML_STEP_DEFAULT;
my $ATSXML_STEP_BINARY;
my $ATSXML_STEP_RUNCASES;
my $ATSXML_STEP_RUNCASES_WITH_BOOT;
my $ATSXML_STEP_RUNCASES_INI;
my $ATSXML_STEP_RUNCASES_INI_WITH_BOOT;
my $ATSXML_STEP_RUNCASE;
my $ATSXML_STEP_INSTALLSIS;
my $ATSXML_POSTACTION;

my $atsCfgFileName = "";
my $atsUsername = "";
my $atsPassword = "";

my $paramStartDir = "..";
my $paramVerbose = 1;
my $paramDebug = 0;
my $paramFindPkgFilePath = "";
my $paramExecuteDrop = "";
my $paramDontDeleteGeneratedFiles = 0;
my $paramCTC = 0;
my $paramNameFilter = "";
my $paramSkipFlashCopy = 0;
my $paramForcedBuild = "";
my $paramRerunXml = 0;
my $paramUseSecondaryIni = 0;
my $paramCaseClass = "";

my $globalAtsHost = "10.11.2.4";
my $globalCtcExePath = "xxx";
my @globalFlashFiles;
my @globalSisFiles;
my @globalPkgFiles;
my $globalDeviceName = "";
my $globalDeviceVersion = "";
my $globalDeviceHWID = "";
my $globalEmail = "";
my $globalIgnoreSis = 0;
my @globalFailedCfgs;

# The code

InitXml();

ReadArgs();

if($paramExecuteDrop ne "")
{
    ExecuteDrop($paramExecuteDrop, 1);
}
else
{
    if($atsCfgFileName eq "")
    {
        ShowHelp();
    }

	ParseFile( $atsCfgFileName );
}

exit();


#------------------------------------------------------------------------------------
# ReadArgs
#------------------------------------------------------------------------------------
sub ReadArgs
    {
    ShowHelp() if(scalar(@ARGV) == 0);

    while(scalar(@ARGV) >= 1)
    {
        my $argument = shift(@ARGV);
        
        if($argument eq "-f")
        {
            $atsCfgFileName = shift(@ARGV)
        }
        elsif($argument eq "-u")
        {
            $atsUsername = shift(@ARGV)
        }
        elsif($argument eq "-p")
        {
            $atsPassword = shift(@ARGV)
        }
        elsif($argument eq "-h")
    	{
    		ShowHelp();
    	}
    	elsif($argument eq "-q")
    	{
    		$paramVerbose = 0;
    	}
    	elsif($argument eq "-d")
    	{
    		$paramVerbose = 1;
    		$paramDebug = 1;
    	}
    	elsif($argument eq -"filepath")
    	{
    	    $paramFindPkgFilePath = shift(@ARGV);
    	}
    	elsif($argument eq "-run")
    	{
    	    $paramExecuteDrop = shift(@ARGV);
    	    $paramVerbose = 1;
    	}
    	elsif($argument eq "-nodelete")
    	{
    	    $paramDontDeleteGeneratedFiles = 1;
    	}
    	elsif($argument eq "-ctcrom")
    	{
    	    $paramCTC = 1;
    	    $globalCtcExePath = "z:\\sys\\bin\\ctcman.exe";
    	}
    	elsif($argument eq "-ctcsis")
    	{
    	    $globalCtcExePath = "c:\\sys\\bin\\ctcman.exe";
    	    $paramCTC = 1;
    	}
    	elsif($argument eq "-dropname")
    	{
    	    $paramNameFilter = shift(@ARGV);
    	}
    	elsif($argument eq "-noflash")
    	{
    	    $paramSkipFlashCopy = 1;
    	}
    	elsif( $argument eq "-atshost" )
    	{
        	$globalAtsHost = shift(@ARGV);
        }
    	elsif( $argument eq "-forcebuild" )
    	{
        	$paramForcedBuild = shift(@ARGV);
        	if( !defined($paramForcedBuild) || ( $paramForcedBuild ne "udeb" && $paramForcedBuild ne "urel" ) )
        	{
        	    die("\n-forcebuild parameter must be 'udeb' or 'urel'");
        	}
        }
    	elsif( $argument eq "-rerun" )
    	{
        	$paramRerunXml = 1;
        	my $file = shift(@ARGV);
        	die("Could not read failed cases!") if ( Rerun_ReadFailedCases( $file, \@globalFailedCfgs ) );
            if( scalar( @globalFailedCfgs ) <= 0 )
            {
            	print("No fails to rerun!\n");
            	exit();
            }
        }
    	elsif( $argument eq "-allini" )
    	{
            $paramUseSecondaryIni = 1;
        }
        elsif( $argument eq "-caseclass" )
    	{
            $paramCaseClass = shift(@ARGV);
        }
    	else
    	{
    	    die("Unknown command line option: '$argument'.\n");
    	}

    }
    
    die("Parameter -f is required.\n") if( $atsCfgFileName eq "" );
}

#------------------------------------------------------------------------------------
# ShowHelp
#------------------------------------------------------------------------------------
sub ShowHelp
{
print <<USAGE_EOF;
genATSdrop.pl -f <drop or master cfg.txt> -u <ats username> -p <ats password>

Params:
-f <filename> Filename of ATS cfg file.
-u <username> ATS username. not required.
-p <password> ATS password. not required.
-q            Quiet mode
-d            Debug information
-run xxx      Run xxx drop in the ATS farm. xxx can be a folder or a zip file (w/o .zip)
-filepath xxx Where files from PKG are searched for, default is the path in PKG file.
-h            help
-nodelete     For default the script deletes generated files.
-noflash      Skip copying flash files to drop.
-ctcrom       CTC++ is installed in the ROM. Fetches the CTC result files from the device.
-ctcsis       CTC++ is installed from SIS. Fetches the CTC result files from the device.
-atshost      Address of ATS host to run the tests.
-dropname xxx Runs only drops which match with xxx string.
-forcebuild   udeb or urel -> forces to use the defined build for all executables read from PKGs
-rerun xxx    Reads results from xml and writes only failed cases into ATS3 drop CFG files.
-allini       Uses secondary testframework.ini file to run the cases, if it's found from init/all/ folder.

Below is example for the ATS master cfg file. The commands override the 
ATS drop cfg commands, except PKG command.

[MASTERCFG]
DEVICE Ivalo
FLASH \\\\network\\flashimages\\core.fpsx
FLASH \\\\network\\flashimages\\variant.fpsx
FLASH \\\\network\\flashimages\\eraseuserdisk.fpsx
SIS \\\\app\\sis\\app.sisx
SIS \\\\app\\tsrc\\sistests.sisx
IGNORESIS 
PKG \\\\app\\tsrc\\data\\cenrepchanges.pkg
EMAIL xxx
; Drop configs must be in tsrc folders and they must be defined after other commands.
INC /ido/api/module1/tsrc/conf/ats_config.txt
INC /ido/api2/module/tsrc/conf/ats_config.txt
[EMDMASTERCFG]

Available commands for the ATS drop cfg file. List order is also the recommended order in the cfg file.

 NAME XXX      Test drop name.
 PRIORITY xxx  Test drop execution priority, 0 is the highest
 DEVICE x1 x2 x3  x1 = Device name in the ATS system, x2 = device version (optional), x3 = device HWID (optional)
 FLASH         File to be flashed. Can be defined with path.
 PKG xxx       Text file listing the files which are copied into the device by ATS.
               Uses same format as Symbian PKG files.
 SIS x1        x1 = SIS/SISX file which needs to be installed.
 IGNORESIS     Master cfg only, ignores SIS commands afterwards.
 BOOT          Reboots the device. Testframework.ini must have cfg with cases Dummy1 and Dummy2.
 INI xxx       Command to execute ini file in ATS system.
 CFG xxx       Command to execute cfg in ATS system. Cfg files must be listed
               in the PKG files. No path support.
 CASELIST xxx  Command to execute invidual STIF cases from CFG files.
 FETCH xxx     Fetches file(s) from a directory. ( FETCH e:\\analyzetool\\* )
 EMAIL xxx     Email address(es) where ATS sends the report.

 USERNAME xxx  Username for ATS login
 PASSWORD xxx  Password for ATS login

Example:

[DROP]
NAME STIF_TestDrop
DEVICE Tube B1 1001
FLASH \\\\network\\flashimages\\core.fpsx
FLASH \\\\network\\flashimages\\variant.fpsx
FLASH \\\\network\\flashimages\\eraseuserdisk.fpsx
PKG \\\\app\\tsrc\\data\cenrepchanges.pkg
SIS \\\\app\\sis\\app.sisx
SIS \\\\app\\tsrc\\sis\\tests.sisx
BOOT
INI atsconfig\\drop1\\TestFramework.ini 1200
EMAIL dude\@nokia.com
[ENDDROP]

USAGE_EOF

	exit();
}

#------------------------------------------------------------------------------------
# ParseFile
# Parameters:
#   $filename
#------------------------------------------------------------------------------------
sub ParseFile
{
	my($filename) = @_;
	
	# Read the file and check if it's drop conf or master conf file.
	open(FILE_HANDLE, $filename) or die("ERROR! Could not open config file '" . $filename . "'\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);

	my $isDropFile = 0;
	
	foreach my $line (@lines)
	{
		if( $line =~ m/\[DROP\]/i || $line =~ m/\[ENDDROP\]/i )
		{
			$isDropFile++;
		}
	}
	
	if( $isDropFile >= 2 )
	{
    	ParseAtsCfg( $filename );
    }
    else
    {
    	ParseMasterCfg( $filename );
    }
}

#------------------------------------------------------------------------------------
# ParseMasterCfg
# Parameters:
#   $filename
#------------------------------------------------------------------------------------
sub ParseMasterCfg
{
	my($filename) = @_;
		
	my $index = 0;
	
	open(FILE_HANDLE, $filename) or die("ERROR! Could not open master config file '" . $filename . "'\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);
	
    while($index < scalar(@lines))
    {
        my $line = $lines[$index];

        my $cmd;
        my $params;
		ReadCmdFromLine( \$line, \$cmd, \$params );

        #
        # CMD FLASH
        #
        if($cmd eq "FLASH")
        {
            push @globalFlashFiles, $params;
        }
		
		#
		# CMD DEVICE
		#
		if( $cmd eq "DEVICE" )
		{
			my @paramArray = split( / /, $params );
			$globalDeviceName = $paramArray[0] if( defined( $paramArray[0] ) );
			$globalDeviceVersion = $paramArray[1] if( defined( $paramArray[1] ) );
			$globalDeviceHWID = $paramArray[2] if( defined( $paramArray[2] ) );
			print("Global device: $globalDeviceName\n");
			print("Global device version: $globalDeviceVersion\n");
			print("Global device HWID: $globalDeviceHWID\n");
		}
		
		#
		# CMD EMAIL
		#
		if( $cmd eq "EMAIL" )
		{
			$globalEmail = $params;
		}

        #
        # CMD INC
        #
        if($cmd eq "INC")
        {
			# Forward slashes to backward.
			$params =~ s/\//\\/g;
			
			my $pos = index( $params, "tsrc" );
			if( $pos != -1 )
			{
				print("\nReading config file $params\n");
				my $path = substr( $params, 0, $pos+4 );
				my $file = substr( $params, $pos+5 );
				
				$pos = index( $file, "conf" );
				
				die("INC file must be in conf directory! Incorrect params: $params") if( $pos == -1);

                if( $pos != 0 )
                {
                    $path .= "\\" . substr( $file, 0, $pos-1 );
                    $file = substr( $file, $pos );
                }
				
				my $startDir = cwd;
				
				# Change where the master config file is.
				if( rindex($filename, "\\") != -1 )
				{
					chdir( substr($filename, 0, rindex($filename, "\\") ) );
				}
				
				# Go where the drop config file is.
				chdir( $path );
				
				ParseAtsCfg( $file );
				
				chdir( $startDir );			
			}
			else
			{
				die("ERROR! The path $params is invalid, config must be under tsrc folder.\n");
			}
		}

        #
        # CMD SIS
        #
        if( $cmd eq "SIS" )
        {
            print("Master CFG has SIS: $params\n"); 
            push @globalSisFiles, $params;
        }
        
        #
        # CMD SIS
        #
        if( $cmd eq "IGNORESIS" )
        {
            print("Master CFG ignore sis command.\n"); 
            $globalIgnoreSis = 1;
        }
        
        #
        # CMD PKG
        #
        if( $cmd eq "PKG" )
        {
            print("Master CFG has PKG: $params\n"); 
            push @globalPkgFiles, $params;
        }				
		
		$index++;
	}
}	

#------------------------------------------------------------------------------------
# ParseAtsCfg
# Parameters:
#   $filename
#------------------------------------------------------------------------------------
sub ParseAtsCfg
{
    my($filename) = @_;
    
	open(FILE_HANDLE, $filename) or die("ERROR! Could not open drop config file '" . $filename . "'\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);
	
    my $index = 0;

    my $testname = "";
    my $testpriority = 0;
    my $deviceName = "";
    my $deviceVersion = "";
    my $deviceHWID = "";
    my $testClosed = 1; # is file handle closed?
    my $planStartWritten = 0;
    my $installationStepsWritten = 0;
    my $additionalFilesCopiedToDrop = 0; # have ini and flash files copied to drop already.
    my @atsFiles;
    my @flashFiles;
    my @sisFiles;
    my @pkgFiles;
    my @globalPkgFilesForConf;
    my $startOfDropIndex = 0;
    my $email = "";

    my $cmdDropStartRead = 0;
    my $cmdNameRead = 0;
    my $cmdDeviceRead = 0;
    my $cmdPkgRead = 0;
    my $cmdSisRead = 0;
    my $skipCurrentDrop = 0;

    my $bootDefined = 0;
    
    my @preRunCustomXmlData;
    my @postRunCustomXmlData;
    
    if( -e "custom\\prerun_custom.xml" )
    {
    	open( FILE_HANDLE, "custom\\prerun_custom.xml" ) or die("ERROR! Could not read conf/prerun_custom.xml.\n");
    	@preRunCustomXmlData = <FILE_HANDLE>;
    	close(FILE_HANDLE);
    }
    if( -e "custom\\postrun_custom.xml" )
    {
    	open( FILE_HANDLE, "custom\\postrun_custom.xml" ) or die("ERROR! Could not read conf/postrun_custom.xml.\n");
    	@postRunCustomXmlData = <FILE_HANDLE>;
    	close(FILE_HANDLE);
    }

    if( $paramCaseClass ne "" )
    {
        my $cgiDir = GetCgiDir();
        system("perl $cgiDir" . "genCfgs.pl -c $paramCaseClass");

        my @newCfgs;
        FindFiles(".\\conf_temp", "cfg", 1, \@newCfgs);
        if( scalar(@newCfgs) < 1 )
        {
            print("No class in cases. Aborting drop.\n");
            return;
        }
        
        push @globalPkgFilesForConf, "conf_temp\\cfgs.pkg";
    }
    
    while($index < scalar(@lines))
    {
        my $line = $lines[$index];
        
        my $cmd;
        my $params;		
		ReadCmdFromLine( \$line, \$cmd, \$params );

        #
        # CMD DROP
        #
        if($cmd eq "[DROP]")
        {
            $startOfDropIndex = $index;
            # Reset variables for this drop
            $testpriority = 10;
            $testname = "";
            $deviceName = "";
            $deviceVersion = "";
            $deviceHWID = "";
            $planStartWritten = 0;
            $installationStepsWritten = 0;
            $additionalFilesCopiedToDrop = 0;
            undef( @atsFiles );
            undef( @flashFiles );
            undef( @sisFiles );
            undef( @pkgFiles );
            $email = "";

            $basePath = "";

            $currentPlanId = 0;
            $currentSessionId = 0;
            $currentSetId = 0;
            $currentCaseId = 0;
            $currentStepId = 0;

            $cmdDropStartRead = 1;
            $cmdNameRead = 0;
            $cmdDeviceRead = 0;
            $cmdPkgRead = 0;
            $cmdSisRead = 0;
        }

        #
        # CMD NAME
        #
        if($cmd eq "NAME" && !$skipCurrentDrop)
        {
            die ("Command [DROP] missing or NAME in wrong place. Line $index.") if(!$cmdDropStartRead);
            $testname = $params;
            
            $basePath = $params;
            $basePath =~ s/ /_/g;
            
            if( -e "$basePath.zip" ) 
            {
            	print("\ndel $basePath.zip\n");
            	system("del $basePath.zip");
            }

			if( -e $basePath )
			{
            	print("rmdir /s /q $basePath\n");
            	system("rmdir /s /q $basePath");
            }

            $skipCurrentDrop = 0;
            if( $paramNameFilter ne "" )
            {
                if( !($testname =~ m/$paramNameFilter/i) )
                {
                    $skipCurrentDrop = 1;
                    print("Skipping drop: $testname\n");
                }
            }

            if( !$skipCurrentDrop )
            {
                print("\nCreating testdrop: $testname\n");

                # Create needed directories
                mkdir($basePath);
                mkdir($basePath . "\\" . $dropFilePath);
                mkdir($basePath . "\\" . $dropImagePath);
                mkdir($basePath . "\\" . $dropBinaryPath);

                # Open XML file for writing
                my $xmlfilename = "$basePath\\test.xml";
                if(!open(FILE_HANDLE_XML, ">$xmlfilename") )
                {
                    print("ERROR! Could not open file for writing '" . $xmlfilename . "'\n");
                    return 0;
                }
                $cmdNameRead = 1;
            }
            $testClosed = 0;
        }

        #
        # CMD PRIORITY
        #
        if($cmd eq "PRIORITY" && !$skipCurrentDrop)
        {
            die ("Command [DROP] missing or PRIORITY in wrong place. Line $index.") if(!$cmdDropStartRead);
            $testpriority = $params;
        }

        #
        # CMD DEVICE
        #
        if($cmd eq "DEVICE" && !$skipCurrentDrop)
        {
            die ("Command DEVICE in wrong place. Line $index.") if(!$cmdDropStartRead || $cmdPkgRead || $cmdSisRead );
            # Write XML header with test name and device
            die("Device must be set in the CFG!") if($params eq "");
            die("NAME must be set in the cfg!") if($testname eq "");

            my @paramArray = split( / /, $params );
            $deviceName = $paramArray[0] if( defined( $paramArray[0] ) );
            $deviceVersion = $paramArray[1] if( defined( $paramArray[1] ) );
            $deviceHWID = $paramArray[2] if( defined( $paramArray[2] ) );
            
            $deviceName = $globalDeviceName if( $globalDeviceName ne "" );
            $deviceVersion = $globalDeviceVersion if( $globalDeviceVersion ne "" );
            $deviceHWID = $globalDeviceHWID if( $globalDeviceHWID ne "" );

            my $data = $ATSXML_HEADER_START;
            $data =~ s/$ATSXML_TESTNAME/$testname/;
            $data =~ s/$ATSXML_TESTDEVICE/$deviceName/;
            $data =~ s/$ATSXML_DROPPRIORITY/$testpriority/;

            if( $deviceVersion ne "" )
            {
                $data .= "            <property name=\"PROTO_VERSION\" value=\"$deviceVersion\" />\n";
            }
            if( $deviceHWID ne "" )
            {
                $data .= "            <property name=\"HWID\" value=\"$deviceHWID\" />\n";
            }

            $data .= $ATSXML_HEADER_END;

            print FILE_HANDLE_XML ($data);

            if($paramVerbose)
            {
                print("Device name: '$deviceName'\n");
                print("Device version: '$deviceVersion'\n");
                print("Device HWID: '$deviceHWID'\n");
            }
            $cmdDeviceRead = 1;
        }

        #
        # CMD FLASH
        #
        if($cmd eq "FLASH" && !$skipCurrentDrop)
        {
            die ("Command FLASH in wrong place. Line $index.") if(!$cmdDropStartRead || !$cmdNameRead || !$cmdDeviceRead || $cmdPkgRead || $cmdSisRead );

            push @flashFiles, $params;
        }

        #
        # Write the XML start before test run commands.
        #
        if( ( $cmd eq "INI" || $cmd eq "CFG" || $cmd eq "CASELIST" ) && !$skipCurrentDrop )
        {
            # Write plan start and flashing commands to XML.
            if($planStartWritten == 0)
            {
                WriteXmlPlanStart(\*FILE_HANDLE_XML, $deviceName);
                
                my $refFlashFiles = \@flashFiles;
                $refFlashFiles = \@globalFlashFiles if( scalar(@globalFlashFiles) > 0 );

                foreach my $flashFile ( @$refFlashFiles )
              	{
	            	my $file = GetPathFileName( $flashFile );
	            	my $data = $ATSXML_FLASH;
	            	$data =~ s/$ATSXML_FLASH_IMAGE_NAME/$file/;
	            	print FILE_HANDLE_XML ($data);
	            	print("Flash file: $file\n") if($paramVerbose);
                }
	          
                # Steps to format drives.
                #WriteXmlFormatDrives(\*FILE_HANDLE_XML);
                
                # Tsrc prerun custom steps.
                if( scalar(@preRunCustomXmlData) > 0 ) 
                {
                    print("Steps from prerun xml.\n");
                    print FILE_HANDLE_XML (@preRunCustomXmlData) 
                }
                
                $planStartWritten = 1;
            }

            # Copy files to drop.
            if( $additionalFilesCopiedToDrop == 0 )
            {
                # Copy flash files to drop.
                my $refFlashFiles = \@flashFiles;
                $refFlashFiles = \@globalFlashFiles if( scalar(@globalFlashFiles) > 0 );
            	CopyFlashFilesToDrop( $refFlashFiles, \@atsFiles );
            	
            	# Copy SIS files to drop and add to files to install.
            	my $refSisFiles = \@sisFiles;
            	$refSisFiles = \@globalSisFiles if( scalar( @globalSisFiles ) > 0 );
         	    foreach my $sisFile ( @$refSisFiles )
            	{
            	    CopySisFileToDrop( $sisFile, \@atsFiles );
            	}

                my @allPkgFiles = ( @pkgFiles, @globalPkgFiles, @globalPkgFilesForConf );

            	foreach my $pkgFile ( @allPkgFiles )
            	{
            	    CopyPkgFilesToDrop( $pkgFile, \@atsFiles );
            	}
            	
            	# Copy testframework.ini of drop to drop. 
            	CopyTestframeworkIniOfDropToDrop( \@lines, $startOfDropIndex, \@atsFiles ) if( $paramCaseClass eq "" );

                $additionalFilesCopiedToDrop = 1;
            }

            # XML file installation steps before these commands.
            if( $cmd eq "INI" || $cmd eq "CFG" || $cmd eq "CASELIST" )
            {
                if($installationStepsWritten == 0)
                {
                    print("File installation steps.\n") if($paramVerbose);
                    $installationStepsWritten = 1;
                    WriteXmlInstallationSteps(\@atsFiles, \*FILE_HANDLE_XML);
                }
            }
        }

        #
        # CMD PKG
        #
        if( $cmd eq "PKG" && !$skipCurrentDrop )
        {
            die ("Command PKG in wrong place. Line $index.") if(!$cmdDropStartRead || !$cmdNameRead || !$cmdDeviceRead );
            push @pkgFiles, $params;
            $cmdPkgRead = 1;
        }

        #
        # CMD BOOT
        #
        if( $cmd eq "BOOT" )
        {
            $bootDefined = 1;
        }

        #
        # CMD INI & CFG
        #
        if( ($cmd eq "INI" || $cmd eq "CFG") && !$skipCurrentDrop)
        {
            die ("Command $cmd in wrong place. Line $index.") if(!$cmdDropStartRead || !$cmdNameRead || !$cmdDeviceRead );
            my $refData;
            if($cmd eq "CFG")
            {
                if( !$bootDefined )
                {
                    $refData = \$ATSXML_STEP_RUNCASES;
                }
                else
                {
                    $refData = \$ATSXML_STEP_RUNCASES_WITH_BOOT;
                }

            }
            else
            {
                if( !$bootDefined )
                {
                    $refData = \$ATSXML_STEP_RUNCASES_INI;
                }
                else
                {
                    $refData = \$ATSXML_STEP_RUNCASES_INI_WITH_BOOT;
                }
            }

            my $pos = index($params, " ");
            my $testfilename = substr($params, 0, $pos);
            my $timeout = substr($params, $pos);

            RemoveWhiteSpaces(\$testfilename);
            RemoveWhiteSpaces(\$timeout);

            $testfilename = GetPathFileName($testfilename);

            die("Commands INI and CFG must be have params file and timeout!") if($testfilename eq "" or $timeout eq "");

            WriteXmlRunStepsForFile($testfilename, $timeout, $refData, \*FILE_HANDLE_XML);
        }

        #
        # CMD SIS
        #
        if( $cmd eq "SIS" && !$skipCurrentDrop && !$globalIgnoreSis )
        {
            die ("Command $cmd in wrong place. Line $index.") if(!$cmdDropStartRead || !$cmdNameRead || !$cmdDeviceRead );
            push @sisFiles, $params;
            $cmdSisRead = 1;
        }

        #
        # CMD CASELIST
        #
        if($cmd eq "CASELIST" && !$skipCurrentDrop)
        {
            die ("Command CASELIST in wrong place. Line $index.") if(!$cmdDropStartRead || !$cmdNameRead || !$cmdDeviceRead );
            my @caseListLines;
            my $caseCount = 0;

        	if( !open(FILE_HANDLE_CASELIST, $params) )
        	{
        		die("ERROR! Could not open file '" . $params . "'\n");
        	}
        	@caseListLines = <FILE_HANDLE_CASELIST>;
        	close(FILE_HANDLE_CASELIST);

        	WriteXmlRunStepsForCases(\@caseListLines, \*FILE_HANDLE_XML);
        }

        #
        # CMD FETCH
        #
        if($cmd eq "FETCH" && !$skipCurrentDrop)
        {
            die ("Command FETCH in wrong place. Line $index.") if(!$cmdDropStartRead || !$cmdNameRead || !$cmdDeviceRead );
            WriteXmlFetchStep(\*FILE_HANDLE_XML, $params);
        }

        #
        # CMD EMAIL
        #
        if($cmd eq "EMAIL" && !$skipCurrentDrop)
        {
			$email = $params;
			$email = $globalEmail if( $globalEmail ne "" );
        }

 		#
        # CMD ENDDROP
        #
        if($cmd eq "[ENDDROP]" && !$skipCurrentDrop)
        {
            $skipCurrentDrop = 0;
            
            die ("[ENDDROP] in wrong place. Line $index.") if(!$cmdDropStartRead || !$cmdNameRead || !$cmdDeviceRead );
            
            # Tsrc prerun custom steps.
            if( scalar(@postRunCustomXmlData) > 0 ) 
            {
                print("Steps from postrun xml.\n");
                print FILE_HANDLE_XML (@postRunCustomXmlData) 
            }
            else
            {
                WriteXmlFetchSteps(\*FILE_HANDLE_XML);
            }

            # End case, set, plan, session
            print FILE_HANDLE_XML ($ATSXML_PLANEND);

            ## Post action, fetches reports to network drive
            #print FILE_HANDLE_XML ($ATSXML_POSTACTION);

            WriteFileList(\@atsFiles, \*FILE_HANDLE_XML);

            # Write footer with email address
            my $data = $ATSXML_FOOTER;
            $data =~ s/$ATSXML_EMAIL/$email/;
            $data =~ s/$ATSXML_TESTNAME/$testname/;
            print FILE_HANDLE_XML ($data);

            if($testClosed == 0)
            {
                $testClosed = 1;
                close(FILE_HANDLE_XML);
            }
        	
        	if( $paramRerunXml )
        	{
            	my $failedCaseCount = 0;
            	
            	foreach my $pkgFile ( @pkgFiles )
            	{
            	    $failedCaseCount += GetFailedCasesCountForPkg( $pkgFile, \@globalFailedCfgs );
            	}
            	foreach my $pkgFile ( @globalPkgFiles )
            	{
            	    $failedCaseCount += GetFailedCasesCountForPkg( $pkgFile, \@globalFailedCfgs );
            	}
            	
            	if( $failedCaseCount > 0 ) 
            	{
            	    ExecuteDrop($basePath, 0);
            	}
            	else
            	{
            	    print("No fails. Removing the drop.\n");
                    if( -e "$basePath.zip" ) 
                    {
                    	print("\ndel $basePath.zip\n");
                    	system("del $basePath.zip");
                    }
                
                	if( -e $basePath )
                	{
                    	print("rmdir /s /q $basePath\n");
                    	system("rmdir /s /q $basePath");
                    }
            	}
            }
            else
            {
                ExecuteDrop($basePath, 0);
            }
        }

        #
        # CMD USERNAME
        #
        if($cmd eq "USERNAME" && !$skipCurrentDrop)
        {
            $atsUsername = $params if( $atsUsername ) eq "";
        }

        #
        # CMD PASSWORD
        #
        if($cmd eq "PASSWORD" && !$skipCurrentDrop)
        {
            $atsPassword = $params if( $atsPassword ) eq "";
        }

        $index++;
    }

    if($testClosed == 0)
    {
        close(FILE_HANDLE_XML);
        $testClosed = 1;
    }

    print("Done.\n");
}

#------------------------------------------------------------------------------------
# ReadCmdFromLine
# Parameters:
#   $refLine, $refCmd, $refParams
# Return 
#  1 if it's a command, otherwise 0.
#------------------------------------------------------------------------------------
sub ReadCmdFromLine
{
	my ($refLine, $refCmd, $refParams) = @_;
	
    RemoveWhiteSpaces($refLine);

	$$refCmd = $$refLine;
	$$refParams = "";
    
    if(index($$refLine, ";") == 0 || index($$refLine, "#") == 0)
    {
		return 0;
    }

    my $pos = index($$refLine, " ");
    if($pos != -1)
    {
        $$refCmd = substr($$refLine, 0, $pos);
        $$refParams = substr($$refLine, $pos);
    }
    RemoveWhiteSpaces($refCmd);
    RemoveWhiteSpaces($refParams);

    print("CMD: '$$refCmd', Params: '$$refParams'\n") if($paramDebug);
    return 1;
}

#------------------------------------------------------------------------------------
# CopySisFileToDrop
# Parameters:
#   $sisFile, $refAtsFiles
#------------------------------------------------------------------------------------
sub CopySisFileToDrop
{
    my( $sisFile, $refAtsFiles ) = @_;
    
    # Copy file to drop.
    my $pkgline = "\"$sisFile\" - \"c:\\testframework\\$sisFile\"";
    print("SIS file to drop: $sisFile.\n") if($paramVerbose);
    CopyFileToDrop($pkgline, $refAtsFiles, 1);
    
    $sisFile = GetPathFileName($sisFile);
    
    # Installation step.
    my @atsFile;
    $atsFile[$ATSFILE_SRC] = "c:\\testframework\\$sisFile";
    $atsFile[$ATSFILE_STEP_XML] = $ATSXML_STEP_INSTALLSIS;
    $atsFile[$ATSFILE_STEP_TIMEOUT] = 5000;    
    push @$refAtsFiles, [ @atsFile ];
}

#------------------------------------------------------------------------------------
# CopyTestframeworkIniOfDropToDrop
# Parameters:
#   $refAtsConfLines, $refAtsFiles
#------------------------------------------------------------------------------------
sub CopyTestframeworkIniOfDropToDrop
{
    my( $refAtsConfLines, $startOfDropIndex, $refAtsFiles ) = @_;

    my $index2 = $startOfDropIndex+1;
    while($index2 < scalar(@$refAtsConfLines))
    {
        my $line = @$refAtsConfLines[$index2];
        RemoveWhiteSpaces(\$line);
        if($line eq "[ENDDROP]" or $line eq "[DROP]")
        {
            last;
        }

        $index2++;
        my $cmd = $line;
        my $filename = "";

        my $pos = index($line, " ");
        if($pos != -1)
        {
            $cmd = substr($line, 0, $pos);
            $filename = substr($line, $pos);
            RemoveWhiteSpaces(\$filename);
        }
        RemoveWhiteSpaces(\$cmd);
        RemoveWhiteSpaces(\$filename);

        if( $cmd eq "INI" )
        {
            if(index($filename, " ") != -1)
            {
                $filename = substr($filename, 0, index($filename, " "));
            }
            print("INI file to drop: $filename.\n") if($paramVerbose);

            my $pkgline = "\"$filename\" - \"c:\\testframework\\$filename\"";
            CopyFileToDrop($pkgline, $refAtsFiles, 1);
        }
    }    
}

#------------------------------------------------------------------------------------
# CopyFlashFilesToDrop
# Parameters:
#   $refFlashFiles, $refAtsFiles
#------------------------------------------------------------------------------------
sub CopyFlashFilesToDrop
{
	my( $refFlashFiles, $refAtsFiles ) = @_;
	
	my $refCopyFlashFiles = $refFlashFiles;
	
 	$refCopyFlashFiles = \@globalFlashFiles if( scalar(@globalFlashFiles) > 0 );
    foreach my $flashFile ( @$refCopyFlashFiles )
  	{
	    # Strip the path from the flash file
	    my $flashfilename = GetPathFileName($flashFile);
	    my $dstpath = $basePath . "\\" . $dropImagePath . $flashfilename;
	
	    if( !$paramSkipFlashCopy )
	    {
	        print("copy $flashFile -> $dstpath\n") if($paramVerbose);;
	        if(! -e $flashFile)
	        {
	            print("File $flashFile does not exist! Must be copied manually to '$dstpath'!\n");
	        }
	        else
	        {
	            copy($flashFile, $dstpath);
	            die("Failed to copy '$flashFile' -> '$dstpath'") if(! -e $dstpath);
	        }
	    }
	    my @atsFile;
	    $atsFile[$ATSFILE_SRC] = $flashfilename;
	    $atsFile[$ATSFILE_DROPFULL_PATH] = $dropImagePath;
	    push @$refAtsFiles, [ @atsFile ];
	}
}

#------------------------------------------------------------------------------------
# CopyPkgFilesToDrop
# Parameters:
#	$filename, pkg file to be read
#   $refAtsFiles
# returns 0 if something fails, otherwise 1
#------------------------------------------------------------------------------------
sub CopyPkgFilesToDrop
{
    my($filename, $refAtsFiles) = @_;

    print("Reading PKG $filename\n") if($paramDebug);

	if( !open(FILE_HANDLE, $filename) )
	{
		print("ERROR! Could not open file '" . $filename . "'\n");
		#die( cwd );
		return 0;
	}
	my @array = <FILE_HANDLE>;
	close(FILE_HANDLE);

    mkdir($basePath);
    mkdir($basePath . "\\" . $dropFilePath);
    mkdir($basePath . "\\" . $dropImagePath);
    mkdir($basePath . "\\" . $dropBinaryPath);

    foreach my $line (@array)
    {
        my $fileCopied = CopyFileToDrop($line, $refAtsFiles);

        # Error, file was not copied and it was under epoc32 directory
        if(!$fileCopied and index(lc($line), "\\epoc32\\") != -1)
        {

        }
    }

	return 1;
}

#------------------------------------------------------------------------------------
# CopyFileToDrop
# Parameters:
#	$line from pkg
#   $refAtsFiles
#   $force, file is added to drop even if it does not exist
# returns 0 if something fails, otherwise 1
#------------------------------------------------------------------------------------
sub CopyFileToDrop
{
    my ($line, $refAtsFiles, $force) = @_;

    my $src; # File location on the workstation
    my $dst; # Where file would be installed from SIS

	# Change forward slahes to backward.
	$line =~ s/\//\\/g;
	
	# Lets hope there's only one level change.
	#$line =~ s/\.\./\./g; 

    # SRC                                                                              DST
    #"\epoc32\data\z\system\data\IptvServiceManagementApiTestIapSelection.cfg"      - "c:\TestFramework\IptvServiceManagementApiTestIapSelection.cfg"

    print("\n") if($paramDebug);

    RemoveWhiteSpaces(\$line);
    if( index($line, ";") == 0 && !( $line =~ m/testframework.ini/i ) )
    {
        print("Skipping, comment: $line\n") if($paramDebug);
        return 0;
    }

    if( index($line, "\\") == -1 )
    {
        print("Skipping, not a file: $line\n") if($paramDebug);
        return 0;
    }

    if( index($line, "http:\\") != -1 )
    {
        print("Skipping, url: $line\n") if($paramDebug);
        return 0;
    }

    if(substr($line, 0, 9) eq "IF exists")
    {
        print("Skipping, not a file: $line\n") if($paramDebug);
        return 0;
    }

	# Solve src

    my $startPos = index($line, "\"")+1;
    my $endPos = index($line, "\"", $startPos+1);
    
    return 0 if($startPos == -1 || $endPos == -1);
    
    $src = substr($line, $startPos, $endPos - $startPos);

    # We are not in group folder, like the pkg file is written.
    if( index( $src, "..\\" ) == 0 )
    {
        $src = substr( $src, 3 );
    }
    
    if($paramFindPkgFilePath ne "")
    {
        my $newsrc = $paramFindPkgFilePath . "\\" . GetPathFileName($src);
        if(-e $newsrc)
        {
            $src = $newsrc;
        }
    }
   
    # Check for forced builds.
    if( $paramForcedBuild eq "udeb" && index($src, "\\urel\\") != -1 )
    {
        $src =~ s/\\urel\\/\\udeb\\/g;
    }
    if( $paramForcedBuild eq "urel" && index($src, "\\udeb\\") != -1 )
    {
        $src =~ s/\\udeb\\/\\urel\\/g;
    }

    print("src $src\n") if($paramDebug);

	# Solve dst

    $startPos = index($line, "\"", $endPos+1)+1;
    $endPos = index($line, "\"", $startPos);

    return 0 if($startPos == -1 || $endPos == -1);

    $dst = substr($line, $startPos, $endPos - $startPos);
    print("dst $dst\n") if($paramDebug);

    # Resolve copy directory and construct info for the ATS XML

    my @atsFile;
    $atsFile[$ATSFILE_NAME] = "";
    $atsFile[$ATSFILE_HARNESS] = "STIF";
    $atsFile[$ATSFILE_ENABLED] = "true";
    $atsFile[$ATSFILE_PASSRATE] = "100";
    $atsFile[$ATSFILE_SIGNIFICANT] = "false";
    $atsFile[$ATSFILE_TYPE] = "";
    $atsFile[$ATSFILE_SRC] = "";
    $atsFile[$ATSFILE_DST] = "";
    $atsFile[$ATSFILE_DROPFULL_PATH] = "";
    $atsFile[$ATSFILE_COMMAND] = "install";
    $atsFile[$ATSFILE_STEP_XML] = $ATSXML_STEP_DEFAULT;
    $atsFile[$ATSFILE_STEP_TIMEOUT] = 5000;

    my $copyDest = "";

#\Sys\Bin\
# -> dropBinaryPath

    my $found = 0;

    if(!$found and index(lc($dst), "\\sys\\bin\\") != -1)
    {
        print(" - Binary\n") if($paramDebug);
        
        $atsFile[$ATSFILE_NAME] = "Install binary file";
        $atsFile[$ATSFILE_TYPE] = "binary";
        $atsFile[$ATSFILE_DST] = "c:\\sys\\bin\\" . GetPathFileName($dst);
        $atsFile[$ATSFILE_STEP_XML] = $ATSXML_STEP_BINARY;
        $atsFile[$ATSFILE_DROPFULL_PATH] = $dropBinaryPath;

        $found = 1;
        $copyDest = $basePath . "\\" . $dropBinaryPath;
    }

#c:\testing\verifyzip\
# -> dropFilePath

    if(!$found and index(lc($dst), "c:\\testing\\data\\verifyzip\\") != -1)
    {
    	print(" - Verify\n") if($paramDebug);
        $found = 1;
        $copyDest = $basePath . "\\" . $dropFilePath;

        $atsFile[$ATSFILE_NAME] = "Install verify file";
        $atsFile[$ATSFILE_DST] = "c:\\testing\\data\\verifyzip\\" . GetPathFileName($dst);
        $atsFile[$ATSFILE_DROPFULL_PATH] = $dropFilePath;
    }

#c:\TestFramework\
# -> dropFilePath

    if(!$found and index(lc($dst), "c:\\testframework\\") != -1)
    {
    	print(" - STIF file\n") if($paramDebug);
        if( $src =~ m/init\\TestFramework.ini/i && $paramUseSecondaryIni )
        {
            my $secIni = "init\\all\\testframework.ini";
            if( -e $secIni )
            {
                $src = $secIni;
            }
        }
        $found = 1;
        $copyDest = $basePath . "\\" . $dropFilePath;

        $atsFile[$ATSFILE_NAME] = "Install testscript file";
        $atsFile[$ATSFILE_DST] = "c:\\testframework\\" . GetPathFileName($dst);
        $atsFile[$ATSFILE_DROPFULL_PATH] = $dropFilePath;
    }

#c:\
# -> dropFilePath

    if(!$found and index(lc($dst), "c:\\") != -1 && index(lc($dst), "\\") == rindex(lc($dst), "\\") )
    {
        print(" - Other test file\n") if($paramDebug);
        $found = 1;
        $copyDest = $basePath . "\\" . $dropFilePath;

        $atsFile[$ATSFILE_NAME] = "Install testscript file";
        $atsFile[$ATSFILE_DST] = "c:\\" . GetPathFileName($dst);
        $atsFile[$ATSFILE_DROPFULL_PATH] = $dropFilePath;
    }

# All other files

    if(!$found)
    {
    	print(" - Some other file\n") if($paramDebug);
        $copyDest = $basePath . "\\" . $dropFilePath;

        $atsFile[$ATSFILE_NAME] = "Install other file";
        $atsFile[$ATSFILE_DROPFULL_PATH] = $dropFilePath;

        $dst =~ s/!:\\/c:\\/g;
        $atsFile[$ATSFILE_DST] = $dst;
    }

    $copyDest .= GetPathFileName($dst);
    print("-> $copyDest\n")  if($paramDebug);

    my $error = 0;
    if(! -e $src)
    {
        if(!$force)
        {
            print("File does not exist: '$src'. File not added.\n");
            $error = 1;
        }
        else
        {
            print("File does not exist: '$src'. Must be copied manually!\n");
        }
    }

    if( $src =~ m/\.cfg/i && $paramRerunXml )
    {
        Rerun_GenerateCfg( $src, $copyDest, \@globalFailedCfgs );
    }
    else
    {
        copy($src, $copyDest);
    }

    if(! -e $copyDest and !$error and !$force)
    {
        print("Copy failed for some reason. '$src' -> '$copyDest'. File not added.\n");
        $error = 1;
    }

    return 1 if($error);

    $atsFile[$ATSFILE_SRC] = GetPathFileName($dst);

    if( $atsFile[$ATSFILE_NAME] ne "" and $atsFile[$ATSFILE_SRC] ne "")
    {
        print("ADDING to ATS files\n\n") if($paramDebug);
        push @$refAtsFiles, [ @atsFile ];
    }

    return 1;
}

#------------------------------------------------------------------------------------
# GetFailedCasesCountForPkg
# Parameters:
#	$filename, pkg file to be read
#   $refFailedCfgs
# returns count of failed case for a PKGs cfg files.
#------------------------------------------------------------------------------------
sub GetFailedCasesCountForPkg
{
    my( $filename, $refFailedCfgs ) = @_;

	if( !open(FILE_HANDLE, $filename) )
	{
		print("ERROR! Could not open file '" . $filename . "'\n");
		die( cwd );
	}
	my @array = <FILE_HANDLE>;
	close(FILE_HANDLE);

    my $failedCaseCount = 0;

    foreach my $line (@array)
    {
        if( $line =~ m/\.cfg/i )
        {
            foreach my $cfg( @$refFailedCfgs )
            {
                my $cfgFile = @$cfg[0];
                
                if( $line =~ m/$cfgFile/i )
                {
                    my $refCases = @$cfg[1];
                    $failedCaseCount += scalar(@$refCases);
                }
            }
        }
    }

	return $failedCaseCount;
}

#------------------------------------------------------------------------------------
# ExecuteDrop
# Parameters:
#   $dropName, $forceNoDelete
#------------------------------------------------------------------------------------
sub ExecuteDrop
{
    my($dropName, $forceNoDelete) = @_;

    if($atsUsername eq "" or $atsPassword eq "")
    {
        print("User credentials not given as params, drop $basePath must be run manually!\n");
        return;
    }
    
    my $cgidir = GetCgiDir();

	# Create zip if it doesn't exist.
    if(!-e "$dropName.zip")
    {
        if(!-e "$dropName")
        {
            die("$dropName.zip or $dropName directory does not exist! Cannot execute.\n");
        }
        # Make zip
        chdir $dropName;
        my $runcmd = "zip -r ..\\$dropName.zip ATS3Drop test.xml";
        print($runcmd . "\n") if($paramVerbose);
        $runcmd .= ">NUL" if($paramDebug == 0);
        system($runcmd);
        chdir "..";
    }
    else
    {
        print("$dropName.zip exists. Executing the drop.\n");
    }

	my $dropDir = cwd;
	
	# Copy file to network.
	
	my $dropNetworkPath = "\\\\147.243.252.83\\Groups2\\cats\\";
	my $dropPath = $dropNetworkPath . "$dropName.zip";
	
	my $notifyCmdFile = "";
	
	print("Copying drop to network...\n");	
	if( copy( "$dropName.zip", $dropPath ) )
	{
	    $notifyCmdFile = "notifydrop_network.cmd";
	}
	else
	{
	    print("Could not copy drop to network: $dropPath, sending via web.\n");
	    $notifyCmdFile = "notifydrop_local.cmd";
	    $dropPath = "$dropDir/$dropName.zip";
	}
#	    $notifyCmdFile = "notifydrop_local.cmd";
#	    $dropPath = "$dropDir/$dropName.zip";
	    	
    # Run the drop
    my $runcmd = "";
    my $printcmd = "";

    $runcmd = "$notifyCmdFile $globalAtsHost $dropPath $atsUsername $atsPassword";
    $printcmd = "$notifyCmdFile $globalAtsHost $dropPath $atsUsername <password>";
    print($printcmd . "\n") if($paramVerbose);
    
    if(!-e "$cgidir$notifyCmdFile")
    {
      	print("ATS $cgidir$notifyCmdFile is missing!\n");
	}
	else
	{
		chdir( $cgidir );
		system( $runcmd );
		sleep(5);
    	chdir( $dropDir );
    	
		# Delete generated files.
	    if( $paramDontDeleteGeneratedFiles == 0 && $forceNoDelete == 0 )
	    {
	        print("\ndel $dropName.zip\n");
	        system("del $dropName.zip");
	
	        print("rmdir /s /q $dropName\n");
	        system("rmdir /s /q $dropName");
	    }    	
    }
}

#------------------------------------------------------------------------------------
# WriteFileList
# Parameters:
#   $refAtsFiles
#   $fh
#------------------------------------------------------------------------------------
sub WriteFileList
{
    my ($refAtsFiles, $fh) = @_;

    # Write list of the files
    print $fh ("<files>\n");
    foreach my $file (@$refAtsFiles)
    {
    	if( defined( @$file[$ATSFILE_DROPFULL_PATH] ) and defined( @$file[$ATSFILE_SRC] ) ) 
    	{
        	my $file = @$file[$ATSFILE_DROPFULL_PATH] . @$file[$ATSFILE_SRC];
	        print $fh ("<file>$file</file>\n");
	    }
    }
    print $fh ("</files>\n");
}

#------------------------------------------------------------------------------------
# WriteXmlPlanStart
# Parameters:
#   $fh
#------------------------------------------------------------------------------------
sub WriteXmlPlanStart
{
    my ($fh, $deviceName) = @_;

    my $data = $ATSXML_PLANSTART;

	my $replacement = "$deviceName - ";

	$replacement .= "$paramCaseClass - " if( $paramCaseClass );	
	
	if( -e "\\currentRelease.xml" )
	{
		if( open(FILE_HANDLE, "\\currentRelease.xml") )
		{
			my @lines = <FILE_HANDLE>;
			close(FILE_HANDLE);
			
			foreach my $line ( @lines )
			{
				if( $line =~ m/\<release name=\"([[a-z0-9_]+)\"/i )
				{
					$replacement .= "$1 - ";
					last;
				}
			}		
		}
	}
	
	$replacement .= Timestamp();
	
    $data =~ s/$ATSXML_TESTPLAN_NAME/$replacement/;

    $replacement = "Session";
    $data =~ s/$ATSXML_TESTSESSION_NAME/$replacement/;

    $replacement = "Set";
    $data =~ s/$ATSXML_TESTSET_NAME/$replacement/;

    $replacement = "Case";
    $data =~ s/$ATSXML_TESTCASE_NAME/$replacement/;

    print $fh ($data);
}

#------------------------------------------------------------------------------------
# WriteXmlFormatDrives
# Parameters:
#   $fh
#------------------------------------------------------------------------------------
sub WriteXmlFormatDrives
{
    my ($fh) = @_;

    my $data = $ATSXML_FORMAT_DRIVES;

    print $fh ($data);
}

#------------------------------------------------------------------------------------
# WriteXmlCreateDirsSteps
# Parameters:
#   $fh
#------------------------------------------------------------------------------------
sub WriteXmlCreateDirsSteps
{
    my ($fh) = @_;

    my $data = $ATSXML_CREATEDIRS;

    print $fh ($data);
}

#------------------------------------------------------------------------------------
# WriteXmlRunStepsForFile
# Parameters:
#	$filename
#   $timeout
#   $refData
#   $fh
#------------------------------------------------------------------------------------
sub WriteXmlRunStepsForFile
{
    my ($filename, $timeout, $refData, $fh) = @_;

    my $data = $$refData;

    my $replacement = $filename;
    XmlReadyText(\$replacement);
    $data =~ s/$ATSXML_STEP_NAME/$replacement/;

    print("Run step for '"  . $filename . "'\n") if($paramVerbose);

    $replacement = "C:\\testframework\\" . $filename;
    XmlReadyText(\$replacement);
    $data =~ s/$ATSXML_STEP_RUNCASES_FILENAME/$replacement/;

    $replacement = $timeout;
    XmlReadyText(\$replacement);
    $data =~ s/$ATSXML_STEP_RUNCASES_TIMEOUT/$replacement/;

    print $fh ($data);
}

#------------------------------------------------------------------------------------
# WriteXmlRunStepsForCases
# Parameters:
#	$refCaseList
#   $fh
#------------------------------------------------------------------------------------
sub WriteXmlRunStepsForCases
{
    my ($refCaseList, $fh) = @_;

    my $caseCount = 0;
    my $currCfgFileName = "";

	foreach my $line (@$refCaseList)
	{
        my $cmd;
        my $param;
        my $pos = index($line, " ");
        die("Case list file should have format: [cfg/case] [filename/casename]!\n") if($pos == -1);
        $cmd = substr($line, 0, $pos);
        $pos += 1;
        $param = substr($line, $pos, length($line) - $pos - 1);

        if($cmd eq "cfg")
        {
            $currCfgFileName = $param;
        }
        else
        {
            die("Case list file first line must be 'cfg [filename]'!") if($currCfgFileName eq "");

    	    my $data = $ATSXML_STEP_RUNCASE;

            #$replacement = "Case $caseCount";
            my $replacement = $param;
            XmlReadyText(\$replacement);
            $data =~ s/$ATSXML_STEP_NAME/$replacement/;
            $caseCount++;

            $replacement = "c:\\testframework\\$currCfgFileName";
            XmlReadyText(\$replacement);
            $data =~ s/$ATSXML_STEP_RUNCASE_FILENAME/$replacement/;

            $replacement = "$param";
            XmlReadyText(\$replacement);
            $data =~ s/$ATSXML_STEP_RUNCASE_CASENAME/$replacement/;

            print $fh ($data);
        }

    }
}

#------------------------------------------------------------------------------------
# WriteXmlFetchSteps
# Parameters:
#   $refAtsFiles
#   $fh
#------------------------------------------------------------------------------------
sub WriteXmlFetchSteps
{
    my ($fh) = @_;

    #
    # Fetch logs etc. 5 steps
    #

    my $data = $ATSXML_FETCH_STEPS;
    my $replacement = "";

    print $fh ($data);

    if($paramCTC)
    {
        $data = $ATSXML_CTCWRITE_STEP;
        $data =~ s/$ATSXML_CTC_PATH/$globalCtcExePath/;
        print $fh ($data);

        WriteXmlFetchStep($fh, "c:\\data\\ctc\\ctcdata.txt");
    }
}


#------------------------------------------------------------------------------------
# WriteXmlFetchStep
# Parameters:
#   $refAtsFiles
#   $fh
#------------------------------------------------------------------------------------
sub WriteXmlFetchStep
{
    my ($fh, $path) = @_;

    my $data = $ATSXML_FETCH_STEP;

    $data =~ s/$ATSXML_FETCH_PATH/$path/;

    print $fh ($data);
}

#------------------------------------------------------------------------------------
# WriteXmlInstallationSteps
# Parameters:
#   $refAtsFiles
#   $fh
#------------------------------------------------------------------------------------
sub WriteXmlInstallationSteps
{
    my ($refAtsFiles, $fh) = @_;

	if($paramDebug)
	{
		print("ATS install file count: " . scalar( @$refAtsFiles ) . "\n") if($paramDebug);
	}

    my $replacement = "";

    foreach my $file( @$refAtsFiles )
    {
        next if( !defined( @$file[$ATSFILE_STEP_XML] ) );
        
        my $data = @$file[$ATSFILE_STEP_XML];

		if( $data =~ m/$ATSXML_STEP_NAME/ )
		{
			die("ATSFILE_NAME is not defined!") if( !defined( @$file[$ATSFILE_NAME] ) );
            $replacement = @$file[$ATSFILE_NAME];
            XmlReadyText(\$replacement);
            $data =~ s/$ATSXML_STEP_NAME/$replacement/;
		}
			
		if( $data =~ m/$ATSXML_STEP_HARNESS/ )
		{
			die("ATSFILE_HARNESS is not defined!") if( !defined( @$file[$ATSFILE_HARNESS] ) );
	        $replacement = @$file[$ATSFILE_HARNESS];
	        XmlReadyText(\$replacement);
	        $data =~ s/$ATSXML_STEP_HARNESS/$replacement/;
		}

		if( $data =~ m/$ATSXML_STEP_ENABLED/ )
		{
			die("ATSFILE_ENABLED is not defined!") if( !defined( @$file[$ATSFILE_ENABLED] ) );
	        $replacement = @$file[$ATSFILE_ENABLED];
	        XmlReadyText(\$replacement);
	        $data =~ s/$ATSXML_STEP_ENABLED/$replacement/;
		}

		if( $data =~ m/$ATSXML_STEP_PASSRATE/ )
		{
			die("ATSFILE_PASSRATE is not defined!") if( !defined( @$file[$ATSFILE_PASSRATE] ) );
	        $replacement = @$file[$ATSFILE_PASSRATE];
	        XmlReadyText(\$replacement);
	        $data =~ s/$ATSXML_STEP_PASSRATE/$replacement/;
		}
	
		if( $data =~ m/$ATSXML_STEP_SIGNIFICANT/ )
		{
			die("ATSFILE_SIGNIFICANT is not defined!") if( !defined( @$file[$ATSFILE_SIGNIFICANT] ) );
	        $replacement = @$file[$ATSFILE_SIGNIFICANT];
	        XmlReadyText(\$replacement);
	        $data =~ s/$ATSXML_STEP_SIGNIFICANT/$replacement/;
		}
	
		if( $data =~ m/$ATSXML_STEP_PARAM_TYPE/ )
		{
			die("ATSFILE_TYPE is not defined!") if( !defined( @$file[$ATSFILE_TYPE] ) );
	        $replacement = @$file[$ATSFILE_TYPE];
	        XmlReadyText(\$replacement);
	        $data =~ s/$ATSXML_STEP_PARAM_TYPE/$replacement/;
		}
	
		if( $data =~ m/$ATSXML_STEP_PARAM_SRC/ )
		{
			die("ATSFILE_NAME is not defined!") if( !defined( @$file[$ATSFILE_SRC] ) );
	        $replacement = @$file[$ATSFILE_SRC];
	        XmlReadyText(\$replacement);
	        $data =~ s/$ATSXML_STEP_PARAM_SRC/$replacement/;
		}
	
		if( $data =~ m/$ATSXML_STEP_PARAM_DST/ )
		{
			die("ATSFILE_DST is not defined!") if( !defined( @$file[$ATSFILE_DST] ) );
	        $replacement = @$file[$ATSFILE_DST];
	        XmlReadyText(\$replacement);
	        $data =~ s/$ATSXML_STEP_PARAM_DST/$replacement/;
		}
	
		if( $data =~ m/$ATSXML_STEP_COMMAND/ )
		{
			die("ATSFILE_COMMAND is not defined!") if( !defined( @$file[$ATSFILE_COMMAND] ) );
	        $replacement = @$file[$ATSFILE_COMMAND];
	        XmlReadyText(\$replacement);
	        $data =~ s/$ATSXML_STEP_COMMAND/$replacement/;
		}
	
		if( $data =~ m/$ATSXML_STEP_TIMEOUT/ )
		{
			die("ATSFILE_STEP_TIMEOUT is not defined!") if( !defined( @$file[$ATSFILE_STEP_TIMEOUT] ) );
	        $replacement = @$file[$ATSFILE_STEP_TIMEOUT];
	        XmlReadyText(\$replacement);
	        $data =~ s/$ATSXML_STEP_TIMEOUT/$replacement/;
	    }

        print $fh ($data);
    }
}

#------------------------------------------------------------------------------------
# GetNextPlanId
#------------------------------------------------------------------------------------
sub GetNextPlanId
{
    $currentPlanId++;
    my $id = "1.$currentPlanId";

    $currentSessionId = 0;
    $currentSetId = 0;
    $currentCaseId = 0;
    $currentStepId = 0;

    return $id;
}

#------------------------------------------------------------------------------------
# GetNextSessionId
#------------------------------------------------------------------------------------
sub GetNextSessionId
{
    $currentSessionId++;
    my $id = "1.$currentPlanId.$currentSessionId";
    return $id;
}

#------------------------------------------------------------------------------------
# GetNextSetId
#------------------------------------------------------------------------------------
sub GetNextSetId
{
    $currentSetId++;
    my $id = "1.$currentPlanId.$currentSessionId.$currentSetId";
    return $id;
}

#------------------------------------------------------------------------------------
# GetNextCaseId
#------------------------------------------------------------------------------------
sub GetNextCaseId
{
    $currentCaseId++;
    my $id = "1.$currentPlanId.$currentSessionId.$currentSetId.$currentCaseId";
    return $id;
}

#------------------------------------------------------------------------------------
# GetNextStepId
#------------------------------------------------------------------------------------
sub GetNextStepId
{
    $currentStepId++;
    my $id = "1.$currentPlanId.$currentSessionId.$currentSetId.$currentCaseId.$currentStepId";
    return $id;
}

#------------------------------------------------------------------------------------
# Timestamp
#
# Parameters:
#------------------------------------------------------------------------------------
sub Timestamp
{
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);

	$mday = "0$mday" if( length($mday) < 2 );
	$hour = "0$hour" if( length($hour) < 2 );
	$min = "0$min" if( length($min) < 2 );
	
    return "" . ($year+1900) . "-" . ($mon+1) . "-" . $mday . " " . $hour . ":" . $min;
}

#------------------------------------------------------------------------------------
# InitXml
#
# Parameters:
#------------------------------------------------------------------------------------
sub InitXml
{

$ATSXML_HEADER_START =
#"<?xml version=\"1.0\"?>\n" .
"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" .
"<test>\n" .
"<priority>ATSXML_DROPPRIORITY</priority>" .
"    <id>1</id>\n" .
"   <name>ATSXML_TESTNAME</name>\n" .
"    <target>\n" .
"        <device rank=\"none\" alias=\"DEFAULT\">\n" .
"            <property name=\"HARNESS\" value=\"STIF\" />\n" .
"            <property name=\"TYPE\" value=\"ATSXML_TESTDEVICE\" />\n";

$ATSXML_HEADER_END =
"        </device>\n" .
"    </target>\n";

$ATSXML_PLANSTART =
"    <plan name=\"ATSXML_TESTPLAN_NAME\" harness=\"STIF\" enabled=\"true\" passrate=\"100\">\n" .
"        <session name=\"ATSXML_TESTSESSION_NAME\" harness=\"STIF\" enabled=\"true\" passrate=\"100\">\n" .
"            <set name=\"ATSXML_TESTSET_NAME\" harness=\"STIF\" enabled=\"true\" passrate=\"100\">\n" .
"                <target>\n" .
"                    <device rank=\"master\" alias=\"DEFAULT\"/>\n" .
"                </target>\n" .
"                <case name=\"ATSXML_TESTCASE_NAME\" harness=\"STIF\" enabled=\"true\" passrate=\"100\">\n";


$ATSXML_FLASH = "   <flash target-alias=\"DEFAULT\" images=\"ATS3Drop/images/ATSXML_FLASH_IMAGE_NAME\" />\n";

$ATSXML_FORMAT_DRIVES = 
"                   <step name=\"Format drive E\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"                       <command>format</command>\n" . 
"                           <params>\n" .
"                               <param drive=\"e\"/>" .
"                               <param mode=\"quick\"/>\n" .
"                           </params>\n" .
"                   </step>\n" .
"                   <step name=\"Format drive F\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"                       <command>format</command>\n" . 
"                           <params>\n" .
"                               <param drive=\"f\"/>" .
"                               <param mode=\"quick\"/>\n" .
"                           </params>\n" .
"                   </step>\n" .
"                   <step name=\"Format drive G\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"                       <command>format</command>\n" . 
"                           <params>\n" .
"                               <param drive=\"g\"/>" .
"                               <param mode=\"quick\"/>\n" .
"                           </params>\n" .
"                   </step>\n";

$ATSXML_CREATEDIRS =
"                    <step name=\"Create log directory for testmodule\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"	                    	<command>makedir</command>\n" .
"	                    	<params>\n" .
"	                        <param dir=\"c:\\logs\\testframework\"/>\n" .
"	                    	</params>\n" .
"	                </step>\n" .
"	                <step name=\"Create log directory for iptvlogs\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"	                    	<command>makedir</command>\n" .
"	                    	<params>\n" .
"	                        <param dir=\"c:\\logs\\Fusion\"/>\n" .
"	                    	</params>\n" .
"	                </step>\n" .
"	                <step name=\"Create log directory for livetvlogs\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"	                    	<command>makedir</command>\n" .
"	                    	<params>\n" .
"	                        <param dir=\"c:\\logs\\Livetv\"/>\n" .
"	                    	</params>\n" .
"	                </step>\n" .
"	                <step name=\"Create directory for verify files\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"	                    	<command>makedir</command>\n" .
"	                    	<params>\n" .
"	                        <param dir=\"c:\\testing\\data\\verifyzip\"/>\n" .
"	                    	</params>\n" .
"	                </step>\n" .
"	                <step name=\"Create directory for verify files\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"	                    	<command>makedir</command>\n" .
"	                    	<params>\n" .
"	                        <param dir=\"c:\\testing\\data\\verifynew\"/>\n" .
"	                    	</params>\n" .
"	                </step>\n" .
"	                <step name=\"Create directory for TestScripter logs\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"	                    	<command>makedir</command>\n" .
"	                    	<params>\n" .
"	                        <param dir=\"c:\\logs\\testframework\\TestScripter\"/>\n" .
"	                    	</params>\n" .
"	                </step>\n";

$ATSXML_PLANEND =
"                </case>\n" .
"            </set>\n" .
"        </session>\n" .
"    </plan>\n";


$ATSXML_FOOTER =
"<postAction>\n" .
"    	<type>SendEmailAction</type>\n" .
"    	<params>\n" .
"    		<param name='type' value='ATS3_REPORT'/>\n" .
"    		<param name='to' value='ATSXML_EMAIL'/>\n" .
"    		<param name='subject' value='ATS3 Report - ATSXML_TESTNAME'/>\n" .
"    		<param name='send-files' value='true'/>\n" .
"    	</params>\n" .
"  </postAction>\n" .
"</test>\n";

$ATSXML_STEP_DEFAULT =
"                    <step name=\"ATSXML_STEP_NAME\" harness=\"ATSXML_STEP_HARNESS\" enabled=\"ATSXML_STEP_ENABLED\" passrate=\"ATSXML_STEP_PASSRATE\" significant=\"ATSXML_STEP_SIGNIFICANT\">\n" .
"                           <command>ATSXML_STEP_COMMAND</command>\n" .
"                           <params>\n" .
"                           <param src=\"ATSXML_STEP_PARAM_SRC\"/>\n" .
"                           <param dst=\"ATSXML_STEP_PARAM_DST\"/>\n" .
#"                          <param component-path=\"ATSXML_STEP_PARAM_COMPONENT_PATH\"/>\n" .
"                           </params>\n" .
"                    </step>\n";

$ATSXML_STEP_BINARY =
"                    <step name=\"ATSXML_STEP_NAME\" harness=\"ATSXML_STEP_HARNESS\" enabled=\"ATSXML_STEP_ENABLED\" passrate=\"ATSXML_STEP_PASSRATE\" significant=\"ATSXML_STEP_SIGNIFICANT\">\n" .
"                           <command>ATSXML_STEP_COMMAND</command>\n" .
"                           <params>\n" .
"    	                    <param type=\"ATSXML_STEP_PARAM_TYPE\"/>\n" .
"                           <param src=\"ATSXML_STEP_PARAM_SRC\"/>\n" .
"                           <param dst=\"ATSXML_STEP_PARAM_DST\"/>\n" .
#"                          <param component-path=\"ATSXML_STEP_PARAM_COMPONENT_PATH\"/>\n" .
"                           </params>\n" .
"                    </step>\n";

$ATSXML_STEP_RUNCASES =
"                    <step name=\"ATSXML_STEP_NAME\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"true\">\n" .
"                           <command>run-cases</command>\n" .
"                           <params>\n" .
"	                        <param module=\"TestScripter\"/>\n" .
"	                        <param testcase-file=\"ATSXML_STEP_RUNCASES_FILENAME\"/>\n" .
"	                        <param filter=\"*\"/>\n" .
"	                        <param timeout=\"ATSXML_STEP_RUNCASES_TIMEOUT\"/>\n" .
"	                        <param component-path=\"ATS3Drop\"/>\n" .
"                           </params>\n" .
"                    </step>\n";

$ATSXML_STEP_RUNCASES_WITH_BOOT =
"                    <step name=\"ATSXML_STEP_NAME\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"true\">\n" .
"                           <command>run-cases</command>\n" .
"                           <reboot target-alias=\"*\" when=\"before\"/>\n" .
"                           <params>\n" .
"	                        <param module=\"TestScripter\"/>\n" .
"	                        <param testcase-file=\"ATSXML_STEP_RUNCASES_FILENAME\"/>\n" .
"	                        <param filter=\"*\"/>\n" .
"	                        <param timeout=\"ATSXML_STEP_RUNCASES_TIMEOUT\"/>\n" .
"	                        <param component-path=\"ATS3Drop\"/>\n" .
"                           </params>\n" .
"                    </step>\n";

$ATSXML_STEP_RUNCASES_INI =
"                    <step name=\"ATSXML_STEP_NAME\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"true\">\n" .
"                           <command>run-cases</command>\n" .
"                           <params>\n" .
"                           <param engineini=\"ATSXML_STEP_RUNCASES_FILENAME\"/>\n" .
"                           <param filter=\"*\"/>\n" .
"                           <param timeout=\"ATSXML_STEP_RUNCASES_TIMEOUT\"/>\n" .
"                           <param repeat=\"1\"/>\n" .
"                           </params>\n" .
"                    </step>\n";

$ATSXML_STEP_RUNCASES_INI_WITH_BOOT =
"                    <step name=\"ATSXML_STEP_NAME\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"true\">\n" .
"                           <command>run-cases</command>\n" .
"                           <reboot target-alias=\"*\" when=\"before\" />\n" .
"                           <params>\n" .
"                           <param engineini=\"ATSXML_STEP_RUNCASES_FILENAME\"/>\n" .
"                           <param filter=\"*\"/>\n" .
"                           <param timeout=\"ATSXML_STEP_RUNCASES_TIMEOUT\"/>\n" .
"                           <param repeat=\"1\"/>\n" .
"                           </params>\n" .
"                    </step>\n";


$ATSXML_STEP_RUNCASE =
"                    <step name=\"ATSXML_STEP_NAME\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"true\">\n" .
"                    <command>run-case</command>\n" .
"                           <params>\n" .
"                           <param module=\"TestScripter\"/>\n" .
"                           <param testcase-file=\"ATSXML_STEP_RUNCASE_FILENAME\"/>\n" .
"                           <param id=\"ATSXML_STEP_RUNCASE_CASENAME\"/>\n" .
"                           <param timeout=\"600\"/>\n" .
"                    </params>\n" .
"                    </step>\n";

$ATSXML_STEP_INSTALLSIS =
"                    <step name=\"Install SIS\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"                    <command>install-software</command>\n" .
"                    <params>\n" .
"                    <param sisPackageName=\"ATSXML_STEP_PARAM_SRC\"/>\n" .
"                    <param upgradeAllowed=\"true\"/>\n" .
"                    <param optionalItemsAllowed=\"true\"/>\n" .
"                    <param OCSP_Done=\"true\"/>\n" .
"                    <param ignoreOCSPWarnings=\"true\"/>\n" .
"                    <param untrustedAllowed=\"true\"/>\n" .
"                    <param packageInfoAllowed=\"true\"/>\n" .
"                    <param userCapGranted=\"true\"/>\n" .
"                    <param killApp=\"true\"/>\n" .
"                    <param overWriteAllowed=\"true\"/>\n" .
"                    <param downloadAllowed=\"false\"/>\n" .
"<!--                    <param downloadUsername=\"user\"/>-->\n" .
"<!--                    <param downloadPassword=\"passwd\"/>-->\n" .
"                    <param installDrive=\"C\"/>\n" .
"                    <param upgradeData=\"true\"/>\n" .
"                    <param timeout=\"ATSXML_STEP_TIMEOUT\"/>\n" .
"                    </params>\n" .
"                    </step>\n";

$ATSXML_FETCH_STEPS =
"                    <step name=\"Fetch Test module Report\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"                           <command>fetch-log</command>\n" .
"                           <params>\n" .
"                    	    <param type=\"text\"/>\n" .
"                    	    <param delete=\"true\"/>\n" .
"                    	    <param path=\"C:\\Logs\\TestFramework\\*\"/>\n" .
"                           </params>\n" .
"                    </step>\n" .
"                    <step name=\"Fetch TestScripter logs\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"                           <command>fetch-log</command>\n" .
"                           <params>\n" .
"                    	    <param type=\"text\"/>\n" .
"                    	    <param delete=\"true\"/>\n" .
"                    	    <param path=\"C:\\Logs\\TestFramework\\TestScripter\\*\"/>\n" .
"                           </params>\n" .
"                    </step>\n";

$ATSXML_FETCH_STEP =
"                    <step name=\"Fetch Test module Report\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"                           <command>fetch-log</command>\n" .
"                           <params>\n" .
"                    	    <param type=\"text\"/>\n" .
"                    	    <param delete=\"true\"/>\n" .
"                    	    <param path=\"ATSXML_FETCH_PATH\"/>\n" .
"                           </params>\n" .
"                    </step>\n";

$ATSXML_CTCWRITE_STEP =
"                   <step name=\"Save CTC data\" harness=\"STIF\" enabled=\"true\" passrate=\"100\" significant=\"false\">\n" .
"                           <command>execute</command>\n" .
"                           <params>\n" .
"                           <param parameters=\"writefile\"/>\n" .
"                           <param file=\"ATSXML_CTC_PATH\"/>\n" .
"                           </params>\n" .
"                   </step>\n";

$ATSXML_POSTACTION =
"                   <postAction>" .
"                       <type>FileStoreAction</type>" .
"                       <params>" .
#"                       <param name=\"to-folder\" value=\"\\someserver\somefolder\§RUN_NAME§\"/>" .
"                       <param name=\"to-folder\" value=\"\\\\oucifs1\\cp_sw\\programs\\video_applications_stream\\iptv\\testing\\CI_reports\\§RUN_START_DATE§_§RUN_START_TIME§_§RUN_NAME§\"/>" .
"                       <param name=\"report type\" value=\"STIF_COMPONENT_REPORT_ALL_CASES\"/>" .
"                       </params>" .
"                   </postAction>";

}



sub GetCgiDir
  {
    # Solve where the script is located.
    $0=~/^(.+[\\\/])[^\\\/]+[\\\/]*$/;
    my $cgidir= $1 || "./";
    
    # And try to solve relative path.
    if( index( $cgidir, "..\\" ) != -1 )
    {
    	my $p = cwd;
    	$p =~ s/\//\\/g;
    	$cgidir =~ s/\//\\/g;
    	while(1) 
    	{
    		my $pos = index( $cgidir, "..\\" );
    		last if( $pos == -1 );
    		$cgidir = substr( $cgidir, $pos+3 );
    		
    		$pos = rindex( $p, "\\" );
    		last if( $pos == -1 );
    		$p = substr( $p, 0, $pos);
    	}
    	$cgidir = $p . "\\" . $cgidir;
    }
    return $cgidir;
}



#------------------------------------------------------------------------------------
# FindFiles
# Parameters:
#	$goDir, where to start finding
#	$fileSearch, filename search
#	$searchType, 0 = fullname search, 1 = filetype search
#	$refIncfiles, reference to array which will hold found files
#------------------------------------------------------------------------------------
sub FindFiles
{
	my ($godir, $fileSearch, $searchType, $refIncfiles) = @_;

	my $startDir = cwd;

	chdir($godir);

	#print("Now in: " . cwd . "\n");

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

		if (-d $file)
		{
		 	FindFiles( $file, $fileSearch, $searchType, $refIncfiles);
		} else
		{
			if( ($file =~ m/$fileSearch/i and $searchType == 0 ) or ($file =~ m/$fileSearch$/i and $searchType == 1 ) )
			{
                $file = cwd . "/" . $file;
				push @$refIncfiles, $file;
				#print("$file\n");
			}
		}
	}

	chdir ($startDir);
}
