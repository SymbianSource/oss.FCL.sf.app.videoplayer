#
# Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

#-----------------------------------------------
# Sequence
#-----------------------------------------------
# Reads parameters -> [if !ok params] ShowHelp
# ReadConfigFiles
#	-> ParseCfg
#	-> ReadCase
#
#    for all testreports:
#       ReadTestRunInfoFromStifReport
#           GetReportLogFileNames
#       ParseResultsAndLogs
#
#    ParseResultsAndLogs
#        WriteCaseLog
#			LogLineTimeMatchesForCase
#			FormatLogLine
#        UpdateCase
#        CheckCaseEntriesInLog
#
# OLD:
#	-> ParseReportFile
#		-> ReadReportLogs
#		-> WriteCaseLog
#			-> LogLineTimeMatchesForCase
#			-> FormatLogLine
#		-> CheckCaseEntriesInLog
#
# ReportNotDocumentedCases
# SortCases
# CalculateStats
# WriteOfficeXml -> XmlReadyText
# exit()

#-----------------------------------------------
# Other functions:
#-----------------------------------------------
# RemoveWhiteSpaces
# SpecialChars
# ReplaceChar
# GetCaseField
# GetCase (id or name)
# GetCaseDesc (id or name)
# PrintCases
# JustFormatLogFile

#
#
#
# STIF CFG:
#************************************************************************************
#[IptvETDescription]
# Testname: <insert text here>
#
# Purpose: <insert text here>
#
# Means: <insert text here>
#
# Required environment settings: <insert text here>
#
# Verification: <insert text here>
#
# Note: <insert text here>
#
# Related requirements: <reqid> <reqid> <reqid> ..
#
#[EndIptvETDescription]
#------------------------------------------------------------------------------------
#[Test]
# do stuff here
#[Endtest]

#------------------------------------------------------------------------------------
# Includes
#------------------------------------------------------------------------------------
#use strict;
use warnings;
use Cwd; # for cwd
use FindBin; # for FindBin:Bin
use File::Path; # for mkpath
use File::Copy;
use Date::Calc;

#------------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------------

my $lastCaseLogLine = 0;

my $ALT_RESULTCOUNT = 10; # maximum number of runs in alternative report

my @cases; #references to case arrays
my @gCfgFiles; # list of cfg files.
my @caseDescs; # case description and the case itself in text
my @notDocumentedCases; #array to hold information about the cases without documentation

$regexpCaseName = '[\x3C\x3E\(\)\w.,\/:_\-&\' ]+';

my $caseDefaultStatus = 			"NOT RUN";

#
# Case field numerations
#

my $CASE_NAME = 					0;
my $CASE_ID = 						1;
my $CASE_RUN_TIMES = 				2;
my $CASE_CRASHED = 					3;
my $CASE_FAILED = 					4;
my $CASE_PASSED = 					5;
my $CASE_STATUS = 					6;
my $CASE_PURPOSE = 					7;
my $CASE_MEANS = 					8;
my $CASE_REQUIRED_SETTINGS = 		9;
my $CASE_RELATED_REQUIREMENTS = 	10;
my $CASE_VERIFICATION = 			11;
my $CASE_NOTE = 					12;
my $CASE_REASON = 					13; # where crashed or what returned the error code
my $CASE_RESULT = 					14; # error code
my $CASE_LOG_FILE = 				15; # test case specific log file.
my $CASE_LOG_FILE_SCRIPTER = 	    16; # test case specific log from STIF testscripter logs.
my $CASE_CHECK_RESULT_LOG_FILE = 	17; # set if the case is in the log files, if next line is found: >>>Case start: ETxxxxx CaseName
									# or STIF own trace: Starting testcase [ET1000 Add multiple Iaps] (usually from fasttrace or emulator log)
my $CASE_CFG_FILE = 				18;
my $CASE_RUN_TIME_SECONDS = 		19; # how many seconds it took to run the case
my $CASE_REPORTLINE1 =              20;
my $CASE_REPORTLINE2 =              21;
my $CASE_REPORTLINE3 =              22;
my $CASE_ENTRY_LOG_START =          23;

my $TDTOOLRESULTFILE = ".\\text_file.txt";
my $XML_OUTPUT_FILE = ".\\test_cases.xml";
my $XML_BRIEF_OUTPUT_FILE = ".\\test_cases_brief.xml";
my $NOT_DOCUMENTED_CASES_FILE = ".\\test_cases_notdocumented.txt";

my @globalTempFiles;

#
# Tags inside the xml, will be replaced by case data when writing the xml file
#

my $CaseCountMul8 = "CASE_COUNT_MUL8";

my $STYLE_ID_STATUS_TAG = "STYLE_ID_STATUS"; # must be in the xml data

my $STYLE_ID_STATUS_FAILED = "s36";
my $STYLE_ID_STATUS_PASSED = "s37";
my $STYLE_ID_STATUS_CRASHED = "s38";
my $STYLE_ID_STATUS_UNKNOWN = "s401";
my $STYLE_ID_STATUS_NA = "s39";

my $STYLE_ID_STATUS_FAILED_CENTERED = "s36centered";
my $STYLE_ID_STATUS_PASSED_CENTERED = "s37centered";
my $STYLE_ID_STATUS_CRASHED_CENTERED = "s38centered";
my $STYLE_ID_STATUS_UNKNOWN_CENTERED = "s401centered";
my $STYLE_ID_STATUS_NA_CENTERED = "s39centered";

# these must be in the xml data to get the results inserted into xml
my $xmlDataCaseName = "XML_DATA_CASE_NAME";
my $xmlDataCaseRunTimes = "XML_DATA_CASE_RUN_TIMES";
my $xmlDataCaseFailed = "XML_DATA_CASE_FAILED";
my $xmlDataCaseCrashed = "XML_DATA_CASE_CRASHED";
my $xmlDataCasePassed = "XML_DATA_CASE_PASSED";
my $xmlDataCaseStatus = "XML_DATA_CASE_STATUS";
my $xmlDataCasePurpose = "XML_DATA_CASE_PURPOSE";
my $xmlDataCaseMeans = "XML_DATA_CASE_MEANS";
my $xmlDataCaseRequiredSettings = "XML_DATA_CASE_REQUIRED_SETTINGS";
my $xmlDataCaseRelatedRequirements = "XML_DATA_CASE_RELATED_REQUIREMENTS";
my $xmlDataCaseVerification = "XML_DATA_CASE_VERIFICATION";
my $xmlDataCaseNote = "XML_DATA_CASE_NOTE";
my $xmlDataCaseReason = "XML_DATA_CASE_REASON";
my $xmlDataCaseResult = "XML_DATA_CASE_RESULT";
my $xmlDataCaseLink1 = "XML_DATA_CASE_LINK1";
my $xmlDataCaseLink2 = "XML_DATA_CASE_LINK2";

my $xmlDataSummaryCaseCount = "XML_DATA_SUMMARY_TOTAL_CASES";
my $xmlDataSummaryPassed = "XML_DATA_SUMMARY_PASSED";
my $xmlDataSummaryFailed = "XML_DATA_SUMMARY_FAILED";
my $xmlDataSummaryCrashed = "XML_DATA_SUMMARY_CRASHED";
my $xmlDataSummaryTimeout = "XML_DATA_SUMMARY_TIMEOUT";
my $xmlDataSummaryPassRateTotal = "XML_DATA_SUMMARY_PASS_RATE_TOTAL";
my $xmlDataSummaryPassRateRun = "XML_DATA_SUMMARY_PASS_RATE_RUN";
my $xmlDataSummaryRunRate = "XML_DATA_SUMMARY_RUN_RATE";

my $xmlDataAltDateX = "XML_DATA_ALTDATE";

my $xmlDataAltResultX = "XML_DATA_ALTRESULT";

my $xmlDataCfgFile = "XML_DATA_CFG_FILE";

#
# Statistics
#

my $summaryCaseCount = 0;
my $summaryPassedCases = 0;
my $summaryFailedCases = 0;
my $summaryCrashedCases = 0;
my $summaryTimeoutCases = 0;
my $summaryPassRateTotal = 0;
my $summaryPassRateRun = 0;
my $summaryRunRate = 0;

#
# Xml data
#

my $xmlHeader;
my $xmlHeaderTableColumns;
my $xmlHeaderTableColumnsBrief;
my $xmlFooter;
my $xmlDataEmptyRow;
my $xmlSummaryData;
my $xmlData;
my $xmlDataCaseStatusDisabled;
my $xmlData2;
my $xmlDataBriefHeader;
my $xmlDataBrief;
my $xmlCfgFileBrief;
my $xmlHeaderTableColumnsAlt;
my $xmlCfgFileAltHeader;
my $xmlCfgFileAltData;

InitXmlData();

#------------------------------------------------------------------------------------
# GLOBAL CODE
#------------------------------------------------------------------------------------

my $argcount = scalar(@ARGV);

my $optionSortCases = 0;
my $optionShowMessages = 0;
my $optionHtml = 0;
my $optionLogAllCases = 1;
my $optionNoLogs = 0;
my $optionNoSecurityTests = 0;
my $optionNoLiveTvTests = 0;
my $optionCaseIncludedInLog = 0;
my $optionGoodIsBetter = 1;
my $optionPrintCfgSummaries = 0;
my $optionCaseRunTimes = 0;
my $optionFinalDoc = 1;
my $optionFixFilename = "";
my $optionDebug = 0;
my $optionCaseList = "";
my $optionAltResults = 0;
my $optionLogFileName = "fusion";

while(scalar(@ARGV) >= 1)
{
	my $argument = shift(@ARGV);

	if($argument eq "-h")
	{
		ShowHelp();
	}

	elsif($argument eq "-e")
	{
		$optionShowMessages = 1;
	}

	elsif($argument eq "-s")
	{
		$optionSortCases = 1;
	}

	elsif($argument eq "-html")
	{
		$optionHtml = 1;
	}

	elsif($argument eq "-logall")
	{
		$optionLogAllCases = 1;
	}

	elsif($argument eq "-nologs")
	{
		$optionNoLogs = 1;
	}

	elsif($argument eq "-nosecuritytests")
	{
		$optionNoSecurityTests = 1;
	}

	elsif($argument eq "-nolivetvtests")
	{
		$optionNoLiveTvTests = 1;
	}

	elsif($argument eq "-i")
	{
		$optionCaseIncludedInLog = 1;
	}

	elsif($argument eq "-file")
	{
		$file = shift(@ARGV);

		JustFormatLogFile($file);
		exit;
	}
	elsif($argument eq "-runtimes")
	{
		$optionCaseRunTimes = 1;
	}

	elsif($argument eq "-xxx")
	{
		$optionCaseIncludedInLog = 1;
		$optionNoLiveTvTests = 1;
		$optionNoSecurityTests = 1;
		$optionHtml = 1;
	}
	elsif($argument eq "-good")
	{
		$optionGoodIsBetter = 1;
	}
	elsif($argument eq "-cfgsum")
	{
		$optionPrintCfgSummaries = 1;
	}
	elsif($argument eq "-finalize")
	{
		$optionFinalDoc = 1;
	}
	elsif($argument eq "-fix") {
		$optionFixFilename = shift(@ARGV);
		FixExcelLogPaths();
        exit();
	}
	elsif($argument eq "-debug") {
	    $optionShowMessages = 1;
	    $optionDebug = 1;
	}
	elsif($argument eq "-caselist") {
	    $optionCaseList = shift(@ARGV);
	}
	elsif($argument eq "-alt") {
	    $optionAltResults = 1;
	}
	elsif( $argument eq "-logfile") {
		$optionLogFileName = shift(@ARGV);
	}
	else
	{
	    ShowHelp();
	}
}

UnzipAndCopyFiles();

my $totalExecutionTime = 0;

my $caseLogOutDir = "testcase_logs\\testcase_logs\\";

my @caseList;

if($optionCaseList ne "")
{
    ReadCaseList(\@caseList, $optionCaseList);
}

# Find cfg files.
FindCfgFiles( ".", \@gCfgFiles );

# None found from current dir, see one dir lower.
FindCfgFiles( "..", \@gCfgFiles ) if( scalar( @gCfgFiles ) < 1 );

# Parse results
if($optionPrintCfgSummaries)
{
	PrintCfgSummaries();
}
elsif( !$optionAltResults )
{
    my $res = ReadTestReports();

	if( $res == 0 )
	{
        foreach my $file ( @globalTempFiles )
        {
            unlink $file if( -e $file );
        }
		print("No reports to read.\n");
		exit;
	}

    #ReportNotDocumentedCases($NOT_DOCUMENTED_CASES_FILE);

    if($optionSortCases)
    {
	    @cases = sort SortCases @cases;
    }

    CalculateStats();

    #WriteOfficeXml($XML_OUTPUT_FILE, 0);
    WriteOfficeXml($XML_BRIEF_OUTPUT_FILE, 1);
}
else
{
    my @runs;
    ReadTestReportsAlt(\@runs);

    WriteOfficeXmlAlt($XML_BRIEF_OUTPUT_FILE, \@runs);
}

#print("Total run time: " . $totalExecutionTime . "\n") if( $optionCaseRunTimes );

foreach my $file ( @globalTempFiles )
{
    unlink $file if( defined( $file ) && -e $file );
}

exit();

#------------------------------------------------------------------------------------
# UnzipAndCopyFiles
#------------------------------------------------------------------------------------
sub UnzipAndCopyFiles
{
	my @files;
	system("rmdir ziptemp /s /q>NUL") if( -e "ziptemp" );
	
	# Unzip zips in current directory and one level of subdirectories. -> ATS results.
	GetAllFiles( ".", \@files, 1 );
	
	foreach my $file ( @files )
	{
		if( $file =~ m/\.zip/i )
		{
			my $fileName = GetPathFileName( $file );
			
			mkdir ("ziptemp");
			my $cmd = "unzip -o \"$file\" -d ziptemp\\$fileName >NUL";
			print("Unzipping $file\n") if( $optionShowMessages );
			system( $cmd );
		}
	}

	undef( @files );
	GetAllFiles( ".", \@files, -1 );

	# Copy files to current dir.

	my $currDir = cwd;
	
	foreach my $file ( @files )
	{
	    next if( -d $file );
	    
	    $file =~ s/\//\\/g;
	    
    	# Do nothing for files which are in current dir.
    	my $srcDir = GetPathDir( $file );
    	my $currpath = cwd;
    	$currpath =~ s/\//\\/g;
    	next if( $srcDir eq $currpath );
	    
	    my $filename = GetPathFileName( $file );
	    
	    # LOOP all testreport*.txt testscripter*.txt
	    # if file is testreport.txt
	    #   lastFolder = last folder from the path
	    #     LOOP all fusion*.txt
	    #       LOOP remove tailing text from lastFolder starting from last index of '.' until it matches with folder of fusion*.txt
	    #          pair fusion*.txt with testreport
	    
	    
		if( ( $filename =~ m/testreport/i || $filename =~ m/testscripter/i ) && $filename =~ m/\.txt/i )
		#if( ( $filename =~ m/$optionLogFileName/i || $filename =~ m/testreport/i || $filename =~ m/testscripter/i ) && $filename =~ m/\.txt/i )
		{
		    my $dst = "";
		    
		    if( $filename =~ m/testreport/i )
		    {
		        # Pair testreports and logs.
		        CopyTestReportWithLog( $file, \@files );
		    }
		    else
		    {
		        print("Copying file $file\n") if( $optionDebug );
		        # For other files we just need unique filename and copy them.
            	$dst = $filename;
            	
            	if( -e $filename )
            	{
                	# Separate file's name and extension.
                	my $name = "";
                	my $ext = "";
                	my $pos = index($filename, ".");
                	if( $pos != -1) 
                	{
                		$ext = substr( $filename, $pos );
                		$name = substr( $filename, 0, $pos );
                		
                		my $count = 0;
                		while( 1 )
                		{
                		    $dst = "$name" . "_" . $count . $ext;
                		    last if( ! -e "$dst" );
                		    $count++;
                		    if( $count > 1000 )
                		    {
                		        $dst = "";
                		        last;
                		    }
                		}
                	}
                }
    			push @globalTempFiles, $dst;
    			
    			print("Dest: $dst\n") if( $optionDebug );
    			
    			copy( $file, $dst ) or die "Could not copy '$file' to '$dst'\n";
		    }
		}
	}

	system("rmdir ziptemp /s /q>NUL") if( -e "ziptemp" );
}

sub CopyTestReportWithLog
{
    my ( $reportFileName, $refFiles ) = @_;
    
    print("\nTrying to pair: $reportFileName\n") if( $optionDebug );
    
    my $reportPath = GetPathDir( $reportFileName );
    
    my $logFileName = "";
    
#\ResultAndLogFiles_vod\22507.23325.1.4.25129-lab.net\TestReport.txt
#\ResultAndLogFiles_vod\22507.23325.1.4.25130-lab.net\Fusion.txt
#                       ^^^^^^^^^^^^^^^ matching part    
    # Find log file with about same path with the report.
    while( 1 )
    {    
        # Check the log files.
        foreach $file ( @$refFiles )
        {
            $file =~ s/\//\\/g;
            my $filename = GetPathFileName( $file );
            
            if( $filename =~ m/$optionLogFileName/i )
            {
                if( index( $file, $reportPath ) == 0 )
                {
                    $logFileName = $file;
                    
                    print("PAIRED: \n") if( $optionDebug );
                    print("reportPath: $reportPath\n") if( $optionDebug );
                    print("report: $reportFileName\n") if( $optionDebug );
                    print("log   : $logFileName\n") if( $optionDebug );
                    
                    print("Report log pair: $logFileName\n") if( $optionShowMessages && !$optionDebug );
                    last;
                }
            }
        }
        
        last if( $logFileName ne "" );
        
        # Remove all from last dot in the path.            
        my $pos = rindex( $reportPath, "." );
        last if( $pos == -1 );
        $reportPath = substr( $reportPath, 0, $pos );
        
        # The first 4 numbers might be same with multiple reports and logs. Ignore file if match is not found before.
        last if( rindex( $reportPath, "\\" ) == -1 );
        my $dotstr = substr( $reportPath, rindex( $reportPath, "\\" )+1 );
        last if( ! ( $dotstr =~ m/[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+/ ) );
        print( "DOT: $dotstr\n" ) if( $optionDebug );
    }

    if( $logFileName eq "" )
    {
        print("Could not find log file for: $reportFileName\n") if( $optionShowMessages );
    }
    
    my $dstReportFileName = "";
    my $dstReportLogName = "";

    # Find first available name for the report in current directory.
    my $fileNum = 0;

    my $filename = GetPathFileName( $reportFileName );
    
	# Separate file's name and extension.
	my $name = "";
	my $ext = "";

	my $pos = index($filename, ".");
	return if( $pos == -1);
	
	$ext = substr( $filename, $pos );
	$name = substr( $filename, 0, $pos );
	
	# Find free filename.
	while( 1 )
	{
	    $dstReportFileName = "$name" . "_" . $fileNum . $ext;
	    last if( ! -e "$dstReportFileName" );
	    $fileNum++;
	    if( $fileNum > 1000 )
	    {
	        die("Too many files to copy.");
	    }
	}
	
	if( $logFileName ne "" )
	{
    	# Get new name for the log with same numbering.
    	
    	$filename = GetPathFileName( $logFileName );
    	
        $pos = index($filename, ".");
    	return if( $pos == -1);
    	
    	$ext = substr( $filename, $pos );
    	$name = substr( $filename, 0, $pos );
    	
    	$dstLogFileName = "$name" . "_" . $fileNum . $ext;
    	die("Log filename already exists.") if( -e "$dstLogFileName" ); # Shouldn't happen.
    }
    
	# Copy the files.
	
    push @globalTempFiles, $dstReportFileName;
    push @globalTempFiles, $dstLogFileName;

	print("Copying $reportFileName\n") if( $optionShowMessages );
	print("Dest: $dstReportFileName\n") if( $optionShowMessages );
	
	copy( $reportFileName, $dstReportFileName ) or die "Could not copy '$reportFileName' to '$dstReportFileName'\n";	
	
	if( $logFileName ne "" )
	{	
	    print("Copying $logFileName\n") if( $optionShowMessages );
	    print("Dest: $dstLogFileName\n") if( $optionShowMessages );
	    copy( $logFileName, $dstLogFileName ) or die "Could not copy '$logFileName' to '$dstLogFileName'\n";	
    }
}

#------------------------------------------------------------------------------------
# ReadCaseList
#
#------------------------------------------------------------------------------------
sub ReadCaseList
{
    my ($refList, $filename) = @_;

	if( open(FILE_HANDLE, ".\\" . $filename) )
	{
		my @lines = <FILE_HANDLE>;
		close(FILE_HANDLE);
	    foreach $line (@lines)
	    {
	        my $caseId;

            if($line =~ m/^case /i)
            {
                if($line =~ m/^case ET([0-9]+) /i)
                {
                    $caseId = "ET" . $1;
                }
                else
                {
                    $caseId = substr($line, 4);
                }
                RemoveWhiteSpaces(\$caseId);
                push @$refList, $caseId;
            }
	    }
	}
}

#------------------------------------------------------------------------------------
# IsCaseInCaseList
#
#------------------------------------------------------------------------------------
sub IsCaseInCaseList
{
    my ($caseNameOrId) = @_;

     return 1 if(scalar(@caseList) <= 0);

    my $caseIdFromName = GetCaseIdFromName($caseNameOrId);

    foreach $caseId (@caseList)
    {
        if($caseId eq $caseNameOrId)
        {
            return 1;
        }
    }

    foreach $caseId (@caseList)
    {
        if($caseId eq $caseIdFromName)
        {
            return 1;
        }
    }

    return 0;
}

#------------------------------------------------------------------------------------
# FixExcelLogPaths
#
# Changes the paths on lines:
#<Cell ss:StyleID="s60" ss:HRef="C:\Documents and Settings\senbom\Desktop\testcase_logs\testcase_logs\ET17099_4371843802.txt"><Data>
# To:
#<Cell ss:StyleID="s60" ss:HRef="testcase_logs\ET17099_4371843802.txt"><Data>
#
# Because the absolute path does not work in Excel.
#
#------------------------------------------------------------------------------------
sub FixExcelLogPaths
{
	if( open(FILE_HANDLE, ".\\" . $optionFixFilename) )
	{
		my @lines = <FILE_HANDLE>;
		close(FILE_HANDLE);

		if(!open(FILE_HANDLE, ">" . $optionFixFilename) )
		{
			die("Could not write to file $file\n");
		}

		foreach my $line(@lines)
		{
			my $pos = index($line, "\\testcase_logs\\testcase_logs\\");
			if($pos != -1)
			{
				my $start = rindex($line, "\"", $pos);
				my $remstr = substr($line, $start+1, $pos-$start + length("\\testcase_logs") );

				$line = substr( $line, 0, index($line, $remstr) ) .
						   substr( $line, index($line, $remstr) + length($remstr) );

			}

			print FILE_HANDLE ($line);

		}
		close(FILE_HANDLE);

		print("Fixed paths in $optionFixFilename\n");
	}
	else
	{
	    die("Coult not open file $optionFixFilename\n");
	}
}

#------------------------------------------------------------------------------------
# CalculateStats
#------------------------------------------------------------------------------------
sub CalculateStats
{
	$summaryCaseCount = scalar( @cases );
	$summaryPassedCases = 0;
	$summaryFailedCases = 0;
	$summaryCrashedCases = 0;
	$summaryTimeoutCases = 0;
	$summaryPassRateTotal = 0;
	$summaryPassRateRun = 0;
	$summaryRunRate = 0;

	foreach $case(@cases)
	{
		if(defined(@{$case}[$CASE_STATUS]))
		{
			my $status = @{$case}[$CASE_STATUS];

			$summaryPassedCases++ if($status eq "PASSED");
			$summaryFailedCases++ if($status eq "FAILED");
			$summaryCrashedCases++ if($status eq "CRASHED");
		}
	}
	
	my $runCases = $summaryPassedCases + $summaryFailedCases + $summaryCrashedCases;
	
	$summaryPassRateTotal = $summaryPassedCases / $summaryCaseCount * 100;
	$summaryPassRateRun = $summaryPassedCases / $runCases * 100;
	$summaryRunRate = $runCases / $summaryCaseCount * 100;
}

#------------------------------------------------------------------------------------
# SortCases
#------------------------------------------------------------------------------------
sub SortCases()
{
	$aprio = 0;
	$bprio = 0;

	#print @{$a}[$CASE_STATUS] . " - " . $a . " vs " . $b . " - " . @{$b}[$CASE_STATUS] . "\n";

	if(@{$a}[$CASE_STATUS] eq "CRASHED") { $aprio = 5; }
	elsif(@{$a}[$CASE_STATUS] eq "FAILED") { $aprio = 4; }
	elsif(@{$a}[$CASE_STATUS] eq "PASSED") { $aprio = 3; }

	if(@{$b}[$CASE_STATUS] eq "CRASHED") { $bprio = 5; }
	elsif(@{$b}[$CASE_STATUS] eq "FAILED") { $bprio = 4; }
	elsif(@{$b}[$CASE_STATUS] eq "PASSED") { $bprio = 3; }

	if($aprio == $bprio)
	{
		return lc(@{$a}[$CASE_NAME]) cmp lc(@{$b}[$CASE_NAME]);
	}

	return $bprio cmp $aprio;
}


#------------------------------------------------------------------------------------
# ShowHelp
#------------------------------------------------------------------------------------
sub ShowHelp {

print <<USAGE_EOF;
testdoc.pl

Create folder to VaDo root folder for STIF results and copy this script and 
the results there. STIF CFG files are searched from . and .. from the script 
run folder. The STIF results can be either ATS results zips or multiple 
TestReport*.txt and fusion*.txt files as long as they have the same ending 
in the name. Script also parses the STIF testscripter logs it finds.

Options:
-i: STIF case will be included in the start of the case log
-s: cases will be sorted by status
-e: Shows extra information during the tool run
-html: Case logs are generated in html format instead of text
-logall: Logs are created for passed cases too.
-nologs: Logs are not processed.
-nosecuritytests: Security tests are not included in the reports.
-nolivetvttests: Live TV tests are not included in the reports.
-i: Test case from the CFG is included in the log file
-file <file>: Only formats defined log file into html
-runtimes: Result of the case will contain the case execution time in seconds.
-good: If case has passed once it will be marked as passed no matter how many 
 times it has failed.
-cfgsum: Prints summary info for cfgs: how many cases and how long the cfg did 
 take to execute.
-finalize
-fix if you save the file in Excel the log paths will come absolute and don't 
 work if moved to another directory. use this fix it.
-caselist <file>: Only reports the cases in caselist.txt, see genrerun.pl for 
 file format.
-logfile <file>: Sets filter for log filenames. Default is fusion.
-slogs: Reads STIF testscripter logs instead of custom logs.
These files will be generated:
	test_cases.xml
	test_cases_brief.xml
	test_cases_notdocumented.txt

Case specific log files are generated in testcase_logs directory and result 
file has links to them.

USAGE_EOF

	exit();

};

#------------------------------------------------------------------------------------
# ReadReportLogs
#
# Parameters:
#	$fileName
#	$refLogLines
#
# Reads log file of specific TestReport.
#
# The function has two different behaviours:
#
# Filenames for test report and log must match. For example
# if test report file is TestReport_XXX.txt then fusion_XXX.txt is read if found.
#
# If fileName is * then *fusion*.txt files will be read one by one and processed by function CheckCaseEntriesInLog()
#------------------------------------------------------------------------------------
sub ReadReportLogs
{
	my ($fileName, $refLogLines) = @_;

	return if($optionNoLogs || $optionPrintCfgSummaries);

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	#default mask for logs
	my $filemask = "^$optionLogFileName\.TXT";

	#create file mask for logs if testreport.txt is in format: testreport_*.txt
	if(index($fileName, "_") != -1)
	{
		my $spos = index($fileName, "_");
		my $epos = rindex($fileName, ".");

		$filemask = substr($fileName, $spos, $epos - $spos);
		#print("filemask: $filemask\n");

		#$filemask = "$optionLogFileName" . $filemask . "[a-z\-0-9_]*\.txt"; # this finds $optionLogFileName_xxx*.txt
		$filemask = "$optionLogFileName" . $filemask . "\.txt"; # this finds only $optionLogFileName_xxx.txt
		#print("filemask: $filemask\n");
		#die();
	}

	my $processLogsHere = 0;

	if($fileName eq "*")
	{
		$processLogsHere = 1;
		$filemask = $optionLogFileName . "[a-z\-0-9_]*\.txt"
	}

	my $i;

	#read the files into refLogLines array
	foreach $file(@filelist)
	{
		#if($file =~ m/^$optionLogFileName[a-z0-9_]+\.TXT$/i or $file =~ m/^$optionLogFileName\.TXT$/i )
		if($file =~ m/$filemask/i )
		{
			print("Reading log file: " . $file . ".");

			if( open(FILE_HANDLE, ".\\" . $file) )
			{
				my @tempArray = <FILE_HANDLE>;
				close(FILE_HANDLE);

				FormatFasttraceTraces(\@tempArray);

				if($processLogsHere)
				{
					FindAndMarkCaseEntriesInLog(\@tempArray, \@cases);

					CheckCaseEntriesInLog(\@tempArray);
				}
				else
				{
					while(scalar(@tempArray) > 0)
					{
						push @$refLogLines, (shift(@tempArray));
					}
				}
			}
		}
	}

	FindAndMarkCaseEntriesInLog($refLogLines, \@cases);
}

#------------------------------------------------------------------------------------
# GetReportLogFileNames
#
# Parameters:
#	$fileName
#	$refLogFiles
#
# Gets log filenames for specific testreport.
#
# The function has two different behaviours:
#
# Filenames for test report and log must match. For example
# if test report file is TestReport_XXX.txt then $optionLogFileName_XXX.txt is read if found.
#
# If fileName is * then *$optionLogFileName*.txt files will be read one by one and processed by function CheckCaseEntriesInLog()
#------------------------------------------------------------------------------------
sub GetReportLogFileNames
{
	my ($fileName, $refLogFiles) = @_;

	return if($optionNoLogs || $optionPrintCfgSummaries);

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	#default mask for logs
	my $filemask = "^$optionLogFileName\.TXT";

	#create file mask for logs if testreport.txt is in format: testreport_*.txt
	if(index($fileName, "_") != -1)
	{
		my $spos = index($fileName, "_");
		my $epos = rindex($fileName, ".");

		$filemask = substr($fileName, $spos, $epos - $spos);
		$filemask = "$optionLogFileName" . $filemask . "\.txt"; # this finds only $optionLogFileName_xxx.txt
	}

	if($fileName eq "*")
	{
		$filemask = $optionLogFileName . "[a-z0-9_]*\.txt"
	}

	foreach $file(@filelist)
	{
		if($file =~ m/$filemask/i )
		{
			push @$refLogFiles, $file;
		}
	}
}

#------------------------------------------------------------------------------------
# FindAndMarkCaseEntriesInLog
#
# Parameters:
#	$refLogLines
#
# Searches for case entries in the log and if found then marks the case in @cases array
#------------------------------------------------------------------------------------
sub FindAndMarkCaseEntriesInLog
{
	my ($refLogLines, $refCases) = @_;

	my $line;

	print(" - Searching case start entries from log\n") if($optionShowMessages);

    my $startLineIndex = 0;

	# scan the log thru and mark if there's log start entries
	for( my $i = 0; $i < scalar(@$refLogLines); $i++ )
	{
	    my $line = @$refLogLines[$i];

		my $caseStartInLog = 0;

		my $caseId = "";

		if($line =~ m/>>> Case start:/i)
		{
			$startLineIndex = $i;
			$caseStartInLog = 1;
			$caseId = substr($line, index($line, ">>> Case start:") + length(">>> Case start:"));
			print("Found case start: $caseId\n") if($optionShowMessages);
		}

		if($caseStartInLog)
		{
			RemoveWhiteSpaces(\$caseId);

			my $refCase = GetCase($caseId, $refCases);

			if(defined($refCase))
			{
				@$refCase[$CASE_CHECK_RESULT_LOG_FILE] = 1;
				@$refCase[$CASE_ENTRY_LOG_START] = $startLineIndex;
			}
			else
			{
				print("Case start in log found: $caseId\n") if($optionShowMessages);
				print("ERROR! The start in log found but case is not found from report or cfg.\n");
			}
		}
	}
}

#------------------------------------------------------------------------------------
# FormatFasttraceTraces
#
# Parameters:
#	$refLogArray
#------------------------------------------------------------------------------------
sub FormatFasttraceTraces
{
	my ($refLogArray) = @_;

	my $ftDebug = 0;

	# strip fasttrace logging text from the start of lines
	#[13:58:17.234] sti: MCU_ASCII_PRINTF; channel:0xE0; msg:$optionLogFileName.TXT 09/10/2006¿1:57:08¿C$optionLogFileName::CheckIsPluginRunning

	my $stripFasttrace = 0;
	# find first occurance of Musti message, remove anything before that
	my $i;

	for($i= 0; $i<100; $i++)
	{
		if(defined(@$refLogArray[$i]))
		{
			if(@$refLogArray[$i] =~ m/MCU_ASCII_PRINTF/i)
			{
				$stripFasttrace = 1;
				last;
			}
		}
	}

	print("fasttrace: end $i, strip: $stripFasttrace\n") if($ftDebug);

	#remove the fasttrace specific lines
	print("fasttrace: lines before: " . scalar(@$refLogArray) . "\n") if($ftDebug);

	splice(@$refLogArray, 0, $i) if($stripFasttrace);

	return if(!$stripFasttrace);

	print("fasttrace: lines " . scalar(@$refLogArray) . "\n") if($ftDebug);

	#remove the fasttrace line startings and change format of STIF messages

	for($i = 0; $i<scalar(@$refLogArray); $i++)
	{
		#before: [14:38:40.641] sti: MCU_ASCII_PRINTF; channel:0xE0; msg:TestCase [ET1024 Sort IapList when it has no IAPs] finished with verdict[0]
		#after:  [14:38:40.641] sti: MCU_ASCII_PRINTF; channel:0xE0; msg:$optionLogFileName.TXT 18/10/2006¿2:37:37¿TestCase [ET1024 Sort IapList when it has no IAPs] finished with verdict[0]
		if(@$refLogArray[$i] =~ m/TestCase \[([[a-z0-9_]+) ($regexpCaseName)\] finished with verdict\[[-]?([0-9]+)/i)
		{
			#die(@$refLogArray[$i]);

			my $startStr = substr(@$refLogArray[$i], 0, index(@$refLogArray[$i], "TestCase"));
			my $endStr = substr(@$refLogArray[$i], index(@$refLogArray[$i], "TestCase"));

			my $timeStr = FasttraceLogFindIptvTraceTime($refLogArray, $i);

			@$refLogArray[$i] = $startStr . "$optionLogFileName.TXT " . $timeStr . "\t" . $endStr;

			print("fasttrace: " . @$refLogArray[$i] . "\n") if($ftDebug);

			#print("\n\n" . $startStr . "'\n$optionLogFileName.TXT \n" . $timeStr . "'\n" . $endStr . "'\n" );
			#die();
		}

		#change the lines where is STIF message saying starting testcase to "$optionLogFileName.TXT <TIME> >>> Case start: <CASEID>" format, otherwise it will be not be recognized later
		if(@$refLogArray[$i] =~ m/Starting testcase \[([[a-z0-9_]+) ($regexpCaseName)\]/i)
		{
			print("debug: " . @$refLogArray[$i] . "\n");
			@$refLogArray[$i] = substr(@$refLogArray[$i], 0, index(@$refLogArray[$i], "Starting testcase"));

			my $timeStr = FasttraceLogFindIptvTraceTime($refLogArray, $i);

			@$refLogArray[$i] .= "$optionLogFileName.TXT $timeStr\t>>> Case start: $1" . "\r\n";

			print("fasttrace: '" . @$refLogArray[$i] . "'\n") if($ftDebug);

			#@$refLogArray[$i] = "[14:38:37.953] sti: MCU_ASCII_PRINTF; channel:0xE0; msg:$optionLogFileName.TXT 18/10/2006\t2:37:34\t>>> Case start: $1";
		}

		if(@$refLogArray[$i] =~ m/channel\:[0-9a-z]+; msg:$optionLogFileName.TXT [0-9]+\/[0-9]+\/[0-9]+/i)
		{
			@$refLogArray[$i] = substr(@$refLogArray[$i], index(@$refLogArray[$i], "msg:$optionLogFileName.TXT") + length("msg:$optionLogFileName.TXT") + 1);

			#replace fasttrace tab chars
			@$refLogArray[$i] =~ s/¿/\t/g;
			#print(@$refLogArray[$i]);

		}
		else
		{
			# if there's no match then this is not a trace for $optionLogFileName
			splice(@$refLogArray, $i, 1);
			$i--;
		}
	}
}


#------------------------------------------------------------------------------------
# FastTraceLogFindIptvTraceTime
#
# Parameters:
#	$refLines: reference to array of the log line
# 	$startLine: line where to start looking for
#
# Finds next time stamp of Iptvengine from Fasttrace log
#------------------------------------------------------------------------------------
sub FasttraceLogFindIptvTraceTime
{
	my ($refLines, $startLine) = @_;

	#find next trace of iptvengine and get the time and date
	my $timeStr = "";
	for(my $e = $startLine+1; $e < scalar(@$refLines); $e++)
	{
		if(@$refLines[$e] =~m/msg:$optionLogFileName.TXT/)
		{
			my $line = @$refLines[$e];
			#remove the fasttrace trace from line start, we don't want it's time
			$line = substr($line, index($line, "msg:$optionLogFileName.TXT"));

			if($line =~ m/([0-9]+\/[0-9]+\/[0-9]+)/i)
			{
				$timeStr .= $1 . "\t";
			}

			if($line =~ m/([0-9]+:[0-9]+:[0-9]+)/i)
			{
				$timeStr .= $1;
			}
			last if($timeStr ne "");
		}
	}

	return $timeStr;
}

#------------------------------------------------------------------------------------
# ReportNotDocumentedCases
#
# Parameters:
#	$file
#
# Writes the @notDocumentedCases array into a file
#------------------------------------------------------------------------------------
sub ReportNotDocumentedCases
{
	my ($file) = @_;

	if(scalar(@notDocumentedCases) > 0)
	{
		if(!open(FILE_HANDLE, ">" . $file) )
		{
			print("ERROR! Could not open file '" . $file . "'\n");
		} else {
			print FILE_HANDLE ( @notDocumentedCases );
			close(FILE_HANDLE);
			print("Report " . $NOT_DOCUMENTED_CASES_FILE . " written.\n");
		}
	} else
	{
		print("All cases are documented.");
	}
}

#------------------------------------------------------------------------------------
# ReadTestReports
#
# Reads TestReportXX.txt files from the current directory and updates the cases with the results.
# Reads also logs for the report and creates case specific logs.
#------------------------------------------------------------------------------------
sub ReadTestReports
{
	# Read filelist of current directory
	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	my $readSomething = 0;

    my @testScripterLogLines;
    ReadTestScripterLogs( \@testScripterLogLines );
    
    # Read reports and logs in the current directory
	foreach $file(@filelist)
	{
		next if( -d $file or $file eq "." or $file eq "..");

        # Read results STIF testreport
		if($file =~ m/^TestReport[a-z\-0-9_]+\.txt$/i or $file =~ m/^TestReport\.txt$/i )
		{
			if($optionShowMessages) { print("***************************\n"); };
			print("Report file: " . $file . "\n");

			my @logFiles;
			my @caseResults;

			ReadTestRunInfoFromStifReport( $file, \@caseResults, \@logFiles );
			ParseResultsAndLogs( \@caseResults, \@logFiles, \@testScripterLogLines );

			undef(@logFiles);
			undef(@caseResults);

			$readSomething++;
		}
	}

	return if($optionPrintCfgSummaries);

	# No testreports found. Read all logs and parse them.
	if($readSomething == 0)
	{
		print("\nNo test reports found in the current directory. Trying to read all the logs.\n");

		my @logFileLines;

		ReadReportLogs("*", \@logFileLines);
		
		if( scalar( @logFileLines ) > 0 )
		{
			$readSomething = 1;
		}

		undef(@logFileLines);
	}

	return $readSomething;
}

#------------------------------------------------------------------------------------
# ReadTestReportsAlt
#
# Every test report is one run and those should have as much same cases as possible.
#------------------------------------------------------------------------------------
sub ReadTestReportsAlt
{
    my ($refRuns) = @_;

	# Read filelist of current directory
	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

    my @runs;

    # Read reports and logs in the current directory
	foreach $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

        # Read results STIF testreport
		if($file =~ m/^TestReport[a-z0-9_]+\.txt$/i or $file =~ m/^TestReport\.txt$/i )
		{
			if($optionShowMessages) { print("***************************\n"); };
			print("Report file: " . $file . "\n");

            # Store report filename, run date and then cases into array

        	# Read date from report
        	if( !open(FILE_HANDLE, $file) ) {
        		print("ERROR! Could not open file '" . $file . "'\n");
        		return;
        	}
        	my @array = <FILE_HANDLE>;
        	close(FILE_HANDLE);

        	#Monday 25th August 2008
        	my @pieces = split(' ', $array[1]);

            $pieces[1] =~ s/st//;
            $pieces[1] =~ s/nd//;
            $pieces[1] =~ s/rd//;
            $pieces[1] =~ s/th//;

            undef(@array);

            my @runInfo;
			$runInfo[0] = $file;
			$runInfo[1] = substr( $pieces[0], 0, 3 ) . " " . $pieces[1] . " " . substr( $pieces[2], 0, 3 );

			my @runResults;
			my @logFiles;
			ReadTestRunInfoFromStifReport($file, \@runResults, \@logFiles );
            $runInfo[2] = \@runResults;

	        push @$refRuns, \@runInfo;
		}
	}
}

#------------------------------------------------------------------------------------
# ReadTestScripterLogs
#------------------------------------------------------------------------------------
sub ReadTestScripterLogs
{
    my ( $refLogLines ) = @_;
    
    return if($optionNoLogs || $optionPrintCfgSummaries);
    
	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);
	
	foreach $file(@filelist)
	{
		if($file =~ m/^testscripter[a-z0-9_]+.txt/i )
		{
		    print("Reading scripter log: $file\n") if( $optionDebug );
		    if( open(FILE_HANDLE, $file) )
    		{
    			my @tempArray = <FILE_HANDLE>;
    			close(FILE_HANDLE);
    
    			while(scalar(@tempArray) > 0)
    			{
    				push @$refLogLines, ( shift( @tempArray ) );
    			}
    			undef(@tempArray);
    		}
    		else
    		{
    		    print("ERROR: Could not open the log file: $file!\n");
    		}			
		}
		else
		{
		    print("Not a scripter log: $file\n") if( $optionDebug );
		}
	}
}


#------------------------------------------------------------------------------------
# ReadTestReportDates
#
# Reads the date from each testreport in current directory
#------------------------------------------------------------------------------------
sub ReadTestReportDates
{
    my ($refDates) = @_;

	# Read filelist of current directory
	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

    my @runs;

    # Read reports and logs in the current directory
	foreach $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

        # Read results STIF testreport
		if($file =~ m/^TestReport[a-z0-9_]+\.txt$/i or $file =~ m/^TestReport\.txt$/i )
		{
			my @fileDate;
			$fileDate[0] = $file;

        	if( !open(FILE_HANDLE, $file) ) {
        		print("ERROR! Could not open file '" . $file . "'\n");
        		return;
        	}
        	my @array = <FILE_HANDLE>;
        	close(FILE_HANDLE);

        	#Monday 25th August 2008
        	my @pieces = split(' ', $array[1]);

            $pieces[1] =~ s/st//;
            $pieces[1] =~ s/nd//;
            $pieces[1] =~ s/rd//;
            $pieces[1] =~ s/th//;

        	$fileDate[1] = substr( $pieces[0], 0, 3 ) . " " . $pieces[1] . " " . substr( $pieces[2], 0, 3 );

            undef(@array);

	        push @$refDates, \@fileDate;
		}
	}
}

#------------------------------------------------------------------------------------
# ParseTimeFromLine
#
#------------------------------------------------------------------------------------
sub ParseTimeFromLine
{
    my ($line) = @_;

    my $timestr = "";

    if($line =~ m/([0-9]+)(:[0-9]+:[0-9]+\,[0-9]*)/i || $line =~ m/([0-9]+)(:[0-9]+:[0-9]+\.[0-9]*)/i)
    {
        $timestr = "" . $1 . $2;
        if($1 > 12)
        {
            $timestr = "" . ($1-12) . $2;
        }
    }

    return $timestr;
}

#------------------------------------------------------------------------------------
# ReadTestRunInfoFromStifReport
#
# Parameters:
#	$file, the testreport
#   $refCaseResults, results for all cases of the testreport
#   $refLogFiles, filenames for the log files of the testreport
#------------------------------------------------------------------------------------
sub ReadTestRunInfoFromStifReport
{
	my ($file, $refCaseResults, $refLogFiles) = @_;

	if( !open(FILE_HANDLE, $file) ) {
		print("ERROR! Could not open file '" . $file . "'\n");
		return;
	}
	my @array = <FILE_HANDLE>;
	close(FILE_HANDLE);

    # Get log files for the report
    GetReportLogFileNames($file, $refLogFiles);

	my $index = 0;
	my $line = "";

	for(my $i = 0; $i < scalar(@array); $i++)
	{
		$line = $array[$i];
		RemoveWhiteSpaces(\$line);

#[testscripter_iptvservicetest][C:\TestFramework\IptvServiceTest.cfg][10] Title:[ET9009 Set id]
#[testscripter][c:\testframework\iptvvoddlapitest2.cfg][2] Title:[ET12100 Purchase]
#	StartTime: 8:54:23.5565 PM, EndTime: 8:54:24.3471 PM
#	Result: 0 [] ==> PASSED
#	Info: xxx

		# found a start of a result
		#[1] Title:[ET1000 Add multiple Iaps]
		my $cfgFile = "";

		if($line =~ m/([a-z_0-9.]+)\]\[[0-9]+\][ ]+Title:\[/i)
		{
			$cfgFile = $1;
			if($optionShowMessages) { print(" - Cfg: $cfgFile\n"); };
			
			for( my $e = 0; $e < scalar( @gCfgFiles ); $e++ )
			{
			    if( $gCfgFiles[$e] =~ m/$cfgFile/i )
			    {
			        my $caseCount = ParseCfg( $gCfgFiles[$e] );
			        $gCfgFiles[$e] = ""; # We don't want to read it again.
			    }
			}
		}

		if($line =~ m/\[[0-9]+\][ ]+Title:\[($regexpCaseName)\]$/i)
		{
			if($optionShowMessages) { print(" - Case: " . $1 . "\n"); };

			#get the case name
			my $caseName = $1;
			my $caseId = GetCaseIdFromName($caseName);
			my $caseRunTime = CountCaseRunTime($array[$i+1]);

            # Save case report lines for later use
            my $caseLine1 = $array[$i];
            my $caseLine2 = $array[$i+1];
            my $caseLine3 = $array[$i+2];

			# Read the case result

			#	StartTime: 3:54:10.1670 PM, EndTime: 3:54:23.8226 PM
			$line = $array[$i+2];
			RemoveWhiteSpaces(\$line);

			my $info = "";
			my $result = "N/A";
			my $status = "$caseDefaultStatus";
			my $reason = "";

			# Result: 0 [] ==> PASSED / something
			if($line =~ m/^Result:[ ]([-]*[0-9]+)[ ]\[[\w]*\][ ]==>[ ]([\w ]+)$/i)
			{
				$result = $1;
				$status = $2;
			}
            # Result: -2 [CTestRunner::RunError [CTestRunner::ExecuteCommandL returned error]] ==> FAILED
			# Result: -2 [CTestRunner::ExecuteCommandL returned error] ==> FAILED
			elsif($line =~ m/^Result:[ ]([-]*[0-9]+)[ ]\[([\w: ]+)\][ ]==>[ ]([\w]+)$/i or
			      $line =~ m/^Result:[ ]([-]*[0-9]+)[ ]\[[\w: ]+\[([\w: ]+)\]\][ ]==>[ ]([\w]+)$/i )
			{
				$result = $1;
				$status = $3;
				$reason = $2;
			}

			#CaseExecutionResult: Test case execution fails with -15
			elsif($line =~ m/^CaseExecutionResult:[ ]Test case execution fails with ([-]*[0-9]+)$/i)
			{
				$result = $1;
				$status = "FAILED";
				$reason = "Test case execution failed.";
			}

			# CaseExecutionResult: Leave during case: with -4
			elsif($line =~ m/^CaseExecutionResult:[ ]Leave during case: with ([-]*[0-9]+)$/i)
			{
				$result = $1;
				$status = "FAILED";
				$reason = "Leave during case";
			}
			#CaseExecutionResult: Crash reason:Error in test ca with 69
			#CaseExecutionResult: Crash reason:IptvEngineServer with 13
			elsif($line =~ m/^CaseExecutionResult:[ ]Crash reason:([\w_ \-]+)[ ]with[ ]([-]*[0-9]+)$/i)
			{
				$result = $2;
				$status = "CRASHED";
				$reason = $1;
			}

			# Result: -2 [Crash reason:KERN-EXEC] ==> FAILED
			elsif($line =~ m/^Result:[ ]([-]*[0-9]+)[ ]\[Crash reason:([\w\-:\. ]+)\] ==> ([\w]+)/i)
			{
				$result = $1;
				$status = "CRASHED";
				$reason = $2;
			}

			# CaseExecutionResult: TestModule loading fails, cannot connect to the TestModule with -2
			elsif($line =~ m/^CaseExecutionResult: TestModule loading fails, cannot connect to the TestModule with ([-]*[0-9]+)/i)
			{
				$result = $1;
				$status = "FAILED";
				$reason = "Test module loading failed.";
			}
			else
			{
				print("Error while parsing case result line: " . $line . "\n");
			}

			$line = $array[$i+3];
			RemoveWhiteSpaces(\$line);

			#	Info: xxx
			if($line =~ m/^Info:/i)
			{
				$info = substr($line, 5) . " / ";
				RemoveWhiteSpaces(\$info);
				$info .= " ";
			}

            my @caseResult;

            $caseResult[$CASE_CFG_FILE] = $cfgFile;
            $caseResult[$CASE_NAME] = $caseName;
            $caseResult[$CASE_ID] = $caseId;
            $caseResult[$CASE_RUN_TIME_SECONDS] = $caseRunTime;
        	$caseResult[$CASE_NOTE] = $info;
        	$caseResult[$CASE_RESULT] = $result;
        	$caseResult[$CASE_STATUS] = $status;
        	$caseResult[$CASE_REASON] = $reason;
        	$caseResult[$CASE_REPORTLINE1] = $caseLine1;
        	$caseResult[$CASE_REPORTLINE2] = $caseLine2;
        	$caseResult[$CASE_REPORTLINE3] = $caseLine3;

            push @$refCaseResults, \@caseResult;
		}
	}
}

#------------------------------------------------------------------------------------
# ParseResultsAndLogs
#
# Parameters:
#   $refCaseResults
#	$refLogFiles
#   $refTestScripterLogLines
#------------------------------------------------------------------------------------
sub ParseResultsAndLogs
{
	my ($refCaseResults, $refLogFiles, $refTestScripterLogLines) = @_;

    my @logLines;

    # Read log files
	foreach $file (@$refLogFiles)
	{
        $file =~ s/\//\\/g;

    	print("Reading log file: " . $file . " .. ");

		if( open(FILE_HANDLE, $file) )
		{
			my @tempArray = <FILE_HANDLE>;
			close(FILE_HANDLE);

			FormatFasttraceTraces(\@tempArray);
			print " .. ";

			while(scalar(@tempArray) > 0)
			{
				push @logLines, (shift(@tempArray));
			}
			print " done.\n";

			undef(@tempArray);
		}
		else
		{
		    print("ERROR: Could not open the log file: $file!\n");
		}
	}
	
	FindAndMarkCaseEntriesInLog( \@logLines, $refCaseResults );

    my @caseSpecificLogs;

    foreach $refCaseResult (@$refCaseResults)
    {
        my $cfgFile = @$refCaseResult[$CASE_CFG_FILE];
        my $caseName = @$refCaseResult[$CASE_NAME];
        my $caseId = @$refCaseResult[$CASE_ID];
        my $caseRunTime = @$refCaseResult[$CASE_RUN_TIME_SECONDS];
    	my $info = @$refCaseResult[$CASE_NOTE];
    	my $result = @$refCaseResult[$CASE_RESULT];
    	my $status = @$refCaseResult[$CASE_STATUS];
    	my $reason = @$refCaseResult[$CASE_REASON];
    	my $reportLine1 = @$refCaseResult[$CASE_REPORTLINE1];
    	my $reportLine2 = @$refCaseResult[$CASE_REPORTLINE2];
    	my $reportLine3 = @$refCaseResult[$CASE_REPORTLINE3];

        if($optionShowMessages) { print(" - Case: " . $caseName . "\n"); };

		# Create case specific log into the log directory
		my $caseLogFile = GetCaseField($caseId, $CASE_LOG_FILE);
		my $caseScripterLogFile = GetCaseField($caseId, $CASE_LOG_FILE_SCRIPTER);
        my $refCaseDesc = GetCaseDesc($caseId);

        $caseLogFile = GetFileNameForCaseLog(  $reportLine1, $reportLine2, $reportLine3 );
        $caseScripterLogFile = GetFileNameForCaseLog(  $reportLine1, $reportLine2, $reportLine3, "_scripter" );

        if( -e $caseLogFile )
        {
            unlink ( $caseLogFile );
        }

        if( -e $caseScripterLogFile )
        {
            unlink ( $caseScripterLogFile );
        }

        my @logInfo;
        $logInfo[0] = $caseLogFile;
        if( defined( @$refCaseResult[$CASE_CHECK_RESULT_LOG_FILE] ) )
        {
            # Case start entry trace was found from log.
            $logInfo[1] = @$refCaseResult[$CASE_ENTRY_LOG_START];
        }
        else
        {
            $logInfo[1] = -1;
        }
        $logInfo[2] = $reportLine1;
        $logInfo[3] = $reportLine2;
        $logInfo[4] = $reportLine3;
        $logInfo[5] = $refCaseDesc;
        $logInfo[6] = $caseScripterLogFile;
        push @caseSpecificLogs, \@logInfo;

		# Update case
		my $refCase = GetCase($caseId, \@cases);
		if(!defined($refCase))
		{
			$refCase = GetCaseByNameOnly($caseName);
		}

		if(defined($refCase))
		{
			UpdateCase($refCase, $caseLogFile, $caseScripterLogFile, $result, $status, $info, $reason, $caseRunTime);
		}
		else
		{
		    # Case doesn't exist yet, add it.		    
			my @case;
			$case[$CASE_NAME] = $caseName;
			$case[$CASE_ID] = $caseId;
			$case[$CASE_RUN_TIMES] = 0;
			$case[$CASE_CRASHED] = 0;
			$case[$CASE_FAILED] = 0;
			$case[$CASE_PASSED] = 0;
			$case[$CASE_STATUS] = "$caseDefaultStatus";
			$case[$CASE_PURPOSE] = "N/A";
			$case[$CASE_MEANS] = "N/A";
			$case[$CASE_REQUIRED_SETTINGS] = "N/A";
			$case[$CASE_RELATED_REQUIREMENTS] = "N/A";
			$case[$CASE_VERIFICATION] = "N/A";
			$case[$CASE_NOTE] = "N/A";
			$case[$CASE_REASON] = "";
			$case[$CASE_RESULT] = "";
			$case[$CASE_LOG_FILE] = "";
			$case[$CASE_LOG_FILE_SCRIPTER] = "";
			$case[$CASE_CHECK_RESULT_LOG_FILE] = 0;
			$case[$CASE_CFG_FILE] = $cfgFile;
			$case[$CASE_RUN_TIME_SECONDS] = 0;

			$refCase = GetCaseByNameOnly($caseName);

			UpdateCase(\@case, $caseLogFile, $caseScripterLogFile, $result, $status, $info, $reason, $caseRunTime);

            # Insert case after other cases from same CFG, if any.

            my $indexToInsert = -1;
            
            for( my $i = 0; $i < scalar(@cases); $i++ )
        	{
        		my $case = $cases[$i];
        		
        		if( defined(@$case[$CASE_CFG_FILE]) )
		        {
		            if( @$case[$CASE_CFG_FILE] eq $cfgFile )
		            {
		                $indexToInsert = $i;
		            }
		        }
		    }
		    
		    if( $indexToInsert == -1 )
		    {
		        push @cases, [ @case ];
		    }
		    else
		    {
		        splice @cases, $indexToInsert+1, 0, [ @case ];
		    }
		}
    }

    my $caseCount = scalar( @$refCaseResults );

    my @testScripterLogCaseLocations;
    PrepareTestScripterLogs( $refTestScripterLogLines, \@testScripterLogCaseLocations );
    WriteCaseSpecificScripterLogs( \@caseSpecificLogs, $refTestScripterLogLines, \@testScripterLogCaseLocations );
    WriteCaseSpecificLogs( \@caseSpecificLogs, $caseCount, \@logLines );    

	if($caseCount >= 10 && $optionShowMessages == 0)
	{
	     print("\n"); #line break for the progress bar
	}
	elsif($optionShowMessages == 0)
	{
	    print("##########\n"); # fake it
	}

	undef(@logLines);
}

#------------------------------------------------------------------------------------
# WriteCaseSpecificLogs
#
# Parameters:
#   $refCaseSpecificLogs 
#	$caseCount
#   $refLogLines
#------------------------------------------------------------------------------------
sub WriteCaseSpecificLogs
{
    my ($refCaseSpecificLogs, $caseCount, $refLogLines ) = @_;

    my $caseLogProgress = 0;

    foreach my $refLogInfo ( @$refCaseSpecificLogs )
    {
        #@logInfo[1] = @$refCaseResult[$CASE_ENTRY_LOG_START];

        my $caseLogFile = @$refLogInfo[0];
        my $reportLine1 = @$refLogInfo[2];
        my $reportLine2 = @$refLogInfo[3];
        my $reportLine3 = @$refLogInfo[4];
        my $refCaseDesc = @$refLogInfo[5];

        my $logWritten = 0;
        if( @$refLogInfo[1] != -1 )
        {
            # Case start entry trace found from log files.
            $logWritten = WriteCaseLogFromEntry($caseLogFile, $refLogLines, @$refLogInfo[1], $reportLine1, $reportLine2, $reportLine3);
        }

        if( !$logWritten )
        {
		    # Use case start and end times to write case specific log.
		    WriteCaseLog($caseLogFile, $refLogLines, $refCaseDesc, $reportLine1, $reportLine2, $reportLine3);
		}

		# Show simple progress bar
		if($caseCount >= 10 && $optionShowMessages == 0)
		{
			$caseLogProgress++;
			if($caseLogProgress > $caseCount/10)
			{
				$caseLogProgress = 0;
				print("#");
			}
		}
    }

	if($caseCount >= 10 && $optionShowMessages == 0)
	{
	     print("\n"); #line break for the progress bar
	}
	elsif($optionShowMessages == 0)
	{
	    print("##########\n"); # fake it
	}   
}

#------------------------------------------------------------------------------------
# PrepareTestScripterLogs
#
# Parameters:
#	$refTestScripterLogLines
#   $refTestScripterLogCaseLocations
#------------------------------------------------------------------------------------
sub PrepareTestScripterLogs
{
    my ($refTestScripterLogLines, $refTestScripterLogCaseLocations ) = @_;

    for( my $i = 0; $i < scalar(@$refTestScripterLogLines); $i++ )
    {
        my $line = @$refTestScripterLogLines[$i];
        
        if( $line =~ m/RunTest:/ )
        {
            push @$refTestScripterLogCaseLocations, ( $i );
        }
    }
}

#------------------------------------------------------------------------------------
# WriteCaseSpecificScripterLogs
#
# Parameters:
#	$refLogFiles
#   $refCaseResults
#   $refTestScripterLogCaseLocations
#------------------------------------------------------------------------------------
sub WriteCaseSpecificScripterLogs
{
    my ($refCaseSpecificLogs, $refScripterLogLines, $refTestScripterLogCaseLocations ) = @_;

    foreach my $refLogInfo ( @$refCaseSpecificLogs )
    {        
        my $caseLogFile = @$refLogInfo[6];
        my $reportLine1 = @$refLogInfo[2];
        my $reportLine2 = @$refLogInfo[3];
        my $reportLine3 = @$refLogInfo[4];
        my $refCaseDesc = @$refLogInfo[5];

        WriteCaseLogFromTestScripterLogs( $caseLogFile, $refScripterLogLines, $refTestScripterLogCaseLocations, $refCaseDesc, $reportLine1, $reportLine2, $reportLine3);
    }
}

#------------------------------------------------------------------------------------
# WriteCaseLogFromTestScripterLogs
#
#------------------------------------------------------------------------------------
sub WriteCaseLogFromTestScripterLogs
{ 
    my ( $caseLogFile, $refLogLines, $refTestScripterLogCaseLocations, $refCaseDesc, $line1, $line2, $line3 ) = @_;

    print("### WriteCaseLogFromTestScripterLogs") if( $optionDebug );

	# Don't generate log files if we only print summary
	if($optionPrintCfgSummaries)
	{
		return;
	}

	RemoveWhiteSpaces(\$line1);
	RemoveWhiteSpaces(\$line2);
	RemoveWhiteSpaces(\$line3);

	my $lineBreak = "\n";
	$lineBreak = "<br>\n" if($optionHtml);

	my $caseName = "";

	if($line1 =~ m/\[[0-9]+\][ ]+Title:\[([a-z0-9]+$regexpCaseName)\]$/i)
	{
		#"' ultraedit
		$caseName = $1;
		print(" Writing scripter log for: $caseName.\n") if($optionShowMessages);
	}
	else
	{
	    return;
	}

	# Write header for case

	my @writeArray;

	# split the first line
	push @writeArray, ( "<hr>" ) if($optionHtml);
	push @writeArray, ( substr($line1, 0, index($line1, "Title:")) . $lineBreak );
	push @writeArray, ( substr($line1, index($line1, "Title:")) . $lineBreak );
	my $title = substr($line1, index($line1, "Title:"));

	#($line1 . "\n");
	push @writeArray, ($line2 . $lineBreak);
	push @writeArray, ($line3 . $lineBreak);
	push @writeArray, ("\n");
	push @writeArray, ( "<hr>" ) if($optionHtml);

	# Include the script for the case in the file?

	if($optionCaseIncludedInLog)
	{
		foreach $linexxx (@{$refCaseDesc})
		{
			push @writeArray, ("$linexxx");
			push @writeArray, ($lineBreak) if($optionHtml);
		}
		push @writeArray, ( "<hr>" ) if($optionHtml);
		push @writeArray, ("\n");
	}
	
	# Create log directory

	if ( !(-e "testcase_logs") )
	{
  		mkdir("testcase_logs");
	}

	if ( !(-e "$caseLogOutDir") )
	{
		mkdir($caseLogOutDir, 0755);
	}

    # Find start and end lines for the case.
    my $startLine = -1;
    my $endLine = -1;

    my $caseNameRegexp = $caseName;
    $caseNameRegexp =~s/\)/\\\)/;
    $caseNameRegexp =~s/\(/\\\(/;

    # Try to find case start from cached locations.
    foreach my $lineNum ( @$refTestScripterLogCaseLocations )
    {
        if( @$refLogLines[$lineNum] =~ m/RunTest: $caseNameRegexp/ )
        {
            print("  - found cached start: $lineNum") if( $optionDebug );
            $startLine = $lineNum;
            last;
        }
    }    

    for( my $i = $startLine; $i < scalar(@$refLogLines); $i++ )
    {
        $i++ if( $i < 0 );
        
        my $line = @$refLogLines[$i];

        next if( !defined $line );
        
        # Case start was not found from cached locations, find it as we go thru the lines.
        if( $startLine == -1 )
        {   
            if( $line =~ m/RunTest: $caseNameRegexp/ )
            {
                print("  - found start: $line") if( $optionDebug );
                $startLine = $i;
            }
        }
        else
        {
            if( $line =~ m/\*\*\*Testcase [a-z]+\*\*\*/i )
            {
                print("  - found end: $line") if( $optionDebug );
                $endLine = $i + 1;
                last;
            }
        }
    }

    if( $startLine == -1 || $endLine == -1 )
    {
        return;
    }

	# Write the log file

	my $fileMode = ">>";

	print(" - Case log file: " . $caseLogFile ."\n") if($optionShowMessages);

	if(	open(FILE_HANDLE, $fileMode . $caseLogFile) )
	{
		if($optionHtml)
		{
			print FILE_HANDLE (
			"<html>\n" .
			"<head>\n" .
			"<title>$title</title>\n" .
			"</head>\n" .
			"<body>\n" .
			"<basefont color=\"black\" face=\"arial\" size=\"4\">\n"
			);
		}

		foreach $line(@writeArray)
		{
			print FILE_HANDLE ($line);
		}

		for( my $i = $startLine; $i < $endLine; $i++ )
		{
			my $line = @$refLogLines[$i];
			FormatLogLine(\$line) if($optionHtml);

			print FILE_HANDLE ($line);
		}

		if($optionHtml)
		{
		print FILE_HANDLE (
			"</body>\n" .
			"</html>"
			);
		}

        print FILE_HANDLE ( $lineBreak );
		print FILE_HANDLE ( $lineBreak );

		close FILE_HANDLE;

	} else
	{
		print("ERROR: Could not write to file: " . $caseLogFile . "\n");
	}
}

#------------------------------------------------------------------------------------
# CheckCaseEntriesInLog
#
# Parameters:
#	$refLogLines: reference to array of log lines
#
# Finds if there's log entries for cases that are not reported in the testreport files
# and create case specific logs for them
#------------------------------------------------------------------------------------
sub CheckCaseEntriesInLog
{
	my ($refLogLines) = @_;

	foreach $case(@cases)
	{
		if(@{$case}[$CASE_CHECK_RESULT_LOG_FILE] == 1)
		{
			@{$case}[$CASE_CHECK_RESULT_LOG_FILE] = 0;

#create the next lines for WriteCaseLog function

#line1: [testscripter_iptvservicemanagementapitestasync][c:\TestFramework\IptvServiceManagementApiTestAsync.cfg][2] Title:[ET08203 Add valid hardcoded service_0 ASYNC]
#line2:	StartTime: 3:06:14.2212 PM, EndTime: 3:06:18.1037 PM
#line3:	Result: 0 [] ==> PASSED

			#starttime = case startline
			#endttime = next case starttline | lastline

			my $line1 = "[unknown.cfg][0] Title:[" . @{$case}[$CASE_NAME] . "]";
			my $line2 = "";
			my $line3 = "Result: 0 [] ==> UNKNOWN";

			my $startTime = "";
			my $endTime = "";
			my $status = "UNKNOWN";
			my $caseLogFile = "";
			my $result = "0";
			my $info = "";
			my $reason = "";

			my $caseId = "";
			foreach $line(@$refLogLines)
			{
				# take the times from lines where is case start text

				if($startTime eq "" && $line =~ m/>>> Case start:/i)
				{
					$caseId = substr($line, index($line, ">>> Case start:") + length(">>> Case start:"));

					RemoveWhiteSpaces(\$caseId);

					# set start time if case matches
					#02/08/2006	12:15:42	>>> Case start: ET00000 caseId
					if($caseId eq @{$case}[$CASE_ID])
					{
						#print("*** Reading case name\n\n");
						if($line =~ m/([0-9]+):([0-9]+):([0-9]+)/)
						{
							#print("CASESTART: $line\n");
							$startTime = "$1:$2:$3.0000";
							#print("*** READ TIME $startTime\n\n");
							next; # skip the rest of loop otherwise endtime will be same
						}
					}
				}

				# find endtime for case if starttime is set
				if($startTime ne "")
				{
					#[14:38:40.641] sti: MCU_ASCII_PRINTF; channel:0xE0; msg:TestCase [ET1024 Sort IapList when it has no IAPs] finished with verdict[0]

					#die("$line") if($line =~m/$caseId/i);
					#die("line: $line\n") if($line =~ m/finished with verdict/);

					if($line =~ m/$caseId/i and $line =~ m/finished with verdict\[([-]*[0-9]+)\]/)
					{

						$result = $1;

						if($1 eq "0")
						{
							$status = "PASSED";
							$line3 = "Result: $1 [] ==> PASSED";
						}
						else
						{
							$status = "FAILED";
							$line3 = "Result: $1 [] ==> FAILED";
						}
					}

					#if line has time then grab it
					if($line =~ m/([0-9]+):([0-9]+):([0-9]+)/)
					{
						$endTime = "$1:$2:$3.9999";
						# stop searching when next case is starting
						if( ($line =~ m/>>> Case start:/i || $line =~ m/>>> Case end./i) and !($line =~ m/$caseId/) )
						{
							#print("CASEEND: $line\n");
							last;
						}
					}
				}

			}

			$line2 = "StartTime: $startTime PM, EndTime: $endTime PM";

			#print("l1: $line1\nl2: $line2\nl3: $line3\n\n");

			my $refCaseDesc = GetCaseDesc($caseId);

			$caseLogFile = GetFileNameForCaseLog( $line1, $line2, $line3 );

			WriteCaseLog($caseLogFile, $refLogLines, $refCaseDesc, $line1, $line2, $line3);

			my $refCase = GetCase($caseId, \@cases);

			UpdateCase($refCase, $caseLogFile, "", $result, $status, $info, $reason, 0);
		}
	}
}

sub UpdateCase
{
	my ($refCase, $caseLogFile, $caseScripterLogFile, $result, $status, $info, $reason, $caseRunTimeSeconds) = @_;

	if($optionShowMessages) { print(" - Case update: $result, $status, $reason, $caseRunTimeSeconds\n\n"); };

	die("Result not defined. Cannot update.\n") if(!defined($result)  );
	die("Status not defined. Cannot update.\n") if(!defined($status) );
	die("Info not defined. Cannot update.\n") if( !defined($info) );
	die("Reason not defined. Cannot update.\n") if( !defined($reason) );

	$caseRunTimeSeconds = 0 if($caseRunTimeSeconds < 0);

	# we have actual result, no need for this anymore
	@$refCase[$CASE_CHECK_RESULT_LOG_FILE] = 0;

	if(!$optionFinalDoc)
	{
		@$refCase[$CASE_RUN_TIMES]++;

		@$refCase[$CASE_RUN_TIME_SECONDS] = $caseRunTimeSeconds;

		if($status eq "CRASHED") {
			@$refCase[$CASE_CRASHED]++;
		}
		elsif($status eq "FAILED") {
			@$refCase[$CASE_FAILED]++;
		}
		elsif($status eq "PASSED") {
			@$refCase[$CASE_PASSED]++;
		}
	}
	else
	{
		if($status eq "PASSED") {
			@$refCase[$CASE_PASSED] = 1;
			@$refCase[$CASE_CRASHED] = 0;
			@$refCase[$CASE_FAILED] = 0;
		}
		elsif($status eq "CRASHED" && @$refCase[$CASE_STATUS] ne "PASSED") {
			@$refCase[$CASE_PASSED] = 0;
			@$refCase[$CASE_CRASHED] = 1;
			@$refCase[$CASE_FAILED] = 0;
		}
		elsif($status eq "FAILED" && @$refCase[$CASE_STATUS] ne "PASSED" && @$refCase[$CASE_STATUS] ne "CRASHED") {
			@$refCase[$CASE_PASSED] = 0;
			@$refCase[$CASE_CRASHED] = 0;
			@$refCase[$CASE_FAILED] = 1;
		}
	}

	my $updateCase = 0;

	if(!$optionGoodIsBetter and !$optionFinalDoc)
	{
		# Only update case status if it's more fatal than the current, CRASHED > FAILED > UNKNOWN > PASSED
		if($status ne "$caseDefaultStatus")
		{
			if (@$refCase[$CASE_STATUS] eq "$caseDefaultStatus") {
				$updateCase = 1;
			}
			elsif (@$refCase[$CASE_STATUS] eq "CRASHED" ) {

			}
			elsif (@$refCase[$CASE_STATUS] eq "FAILED") {
				$updateCase = 1 if($status eq "CRASHED");
			}
			elsif(@$refCase[$CASE_STATUS] eq "PASSED") {
				$updateCase = 1;
			}
		}
	}
	else
	{
		# Only update case status if it's less fatal than the current, PASSED > FAILED > CRASHED > UNKNOWN
		if($status ne "$caseDefaultStatus")
		{
			if (@$refCase[$CASE_STATUS] eq "$caseDefaultStatus") {
				$updateCase = 1;
			}
			elsif(@$refCase[$CASE_STATUS] eq "UNKNOWN") {
				$updateCase = 1;
			}
			elsif($status eq "PASSED")
			{
				$updateCase = 1;
			}
			elsif (@$refCase[$CASE_STATUS] eq "CRASHED" ) {
				$updateCase = 1 if($status eq "FAILED");
			}
			elsif (@$refCase[$CASE_STATUS] eq "FAILED") {

			}
		}
	}

	if($updateCase)
	{
		@$refCase[$CASE_REASON] = $info . $reason;
		@$refCase[$CASE_RESULT] = $result;
		@$refCase[$CASE_STATUS] = $status;
		@$refCase[$CASE_LOG_FILE] = $caseLogFile;
		@$refCase[$CASE_LOG_FILE_SCRIPTER] = $caseScripterLogFile;
	}
	else
	{
	    # -1112 overrides other error codes because the case succeeded but verifying has failed.
	    if(@$refCase[$CASE_RESULT] ne "-1112" and $result eq "-1112")
	    {
	        @$refCase[$CASE_RESULT] = $result;
	    }
	}
	#print "case: " . @{$case}[$CASE_NAME] . "\n" . "run: " . @{$case}[$CASE_RUN_TIMES] . ", crash: " . @{$case}[$CASE_CRASHED] . ", fail " . @{$case}[$CASE_FAILED] . ", passed: " . 	@{$case}[$CASE_PASSED] . "\n";
}

sub CountCaseRunTime
{
	my ($line) = @_;
	my $found = 0;
	RemoveWhiteSpaces(\$line);
	#StartTime: 16:12:10,2440, EndTime: 16:12:34,3880
	#StartTime: 12:02:49.9302 pm, EndTime: 12:03:04.5781 pm
	if($line =~ m/^StartTime: ([0-9]+):([0-9]+):([0-9]+)\,[0-9]*, Endtime: ([0-9]+):([0-9]+):([0-9]+)\,[0-9]*$/i)
	{
		$found = 1;
	}
	elsif($line =~ m/^StartTime: ([0-9]+):([0-9]+):([0-9]+)\.[0-9]*[ A-Z]*, Endtime: ([0-9]+):([0-9]+):([0-9]+)\.[0-9]*[ A-Z]*$/i)
	{
		$found = 1;
	}

    my $caseRunTime = 0;

	if($found)
	{
		my $startSeconds = ($1 * 60 * 60) + $2 * 60 + $3;
		my $endSeconds = ($4 * 60 * 60) + $5 * 60 + $6;

		$caseRunTime = $endSeconds - $startSeconds;
	}
	$caseRunTime = 0 if($caseRunTime < 0);

    $totalExecutionTime += $caseRunTime;
	return $caseRunTime;
}

sub GetFileNameForCaseLog
{
    my ($line1, $line2, $line3, $extra) = @_;

	RemoveWhiteSpaces(\$line1);
	RemoveWhiteSpaces(\$line2);
	RemoveWhiteSpaces(\$line3);

	my $caseId = "";

	if($line1 =~ m/\[[0-9]+\][ ]+Title:\[([a-z0-9]+)$regexpCaseName\]$/i)
	{
		#"' ultraedit
		$caseId = $1;
	}

	my $caseTimeStr = "";

	#StartTime: 16:12:10,2440, EndTime: 16:12:34,3880
	if($line2 =~ m/^StartTime: ([0-9]+):([0-9]+):([0-9]+)\,[0-9]*, Endtime: ([0-9]+):([0-9]+):([0-9]+)\,[0-9]*$/i)
	{
		$found = 1;
	}
	elsif($line2 =~ m/^StartTime: ([0-9]+):([0-9]+):([0-9]+)\.[0-9]*[ A-Z]*, Endtime: ([0-9]+):([0-9]+):([0-9]+)\.[0-9]*[ A-Z]*$/i)
	{
		$found = 1;
	}

	if( $found )
	{
		$caseTimeStr = $1 . $2 . $3 . $4 . $5 . $6;
	}

	my $fileext = ".txt";
	$fileext = ".html" if($optionHtml);

    my $caseLogFile;
    
     if( !defined( $extra ) )
     {
        $caseLogFile = $caseLogOutDir . $caseId . "_" . $caseTimeStr . $fileext;
    }
    else
    {
        $caseLogFile = $caseLogOutDir . $caseId . "_" . $caseTimeStr . $extra . $fileext;
    }

	print(" - Case log file: " . $caseLogFile ."\n") if($optionShowMessages);
	return $caseLogFile;
}

#------------------------------------------------------------------------------------
# WriteCaseLog
#
# Parameters:
# 	$caseLogFile: filename to write
# 	$refLogLines: input where log lines are stored
# 	$caseDesc: optional text description of case which will be written into the log file
# 	$line1: [testscripter_iptvservicemanagementapitestasync][c:\TestFramework\IptvServiceManagementApiTestAsync.cfg][2] Title:[ET08203 Add valid hardcoded service_0 ASYNC]
# 	$line2:	StartTime: 3:06:14.2212 PM, EndTime: 3:06:18.1037 PM
# 	$line3:	Result: 0 [] ==> PASSED
#
# Writes case specific log file into testcase_logs/testcase_logs directory. Reads the lines from @caseLogLines array
#------------------------------------------------------------------------------------
sub WriteCaseLog
{
#[testscripter_iptvservicemanagementapitestasync][c:\TestFramework\IptvServiceManagementApiTestAsync.cfg][2] Title:[ET08203 Add valid hardcoded service_0 ASYNC]
#	StartTime: 3:06:14.2212 PM, EndTime: 3:06:18.1037 PM
#	StartTime: 12:34:41.5407, EndTime: 12:34:41.9687
#	Result: 0 [] ==> PASSED

	my ($caseLogFile, $refLogLines, $refCaseDesc, $line1, $line2, $line3) = @_;

	# Don't generate log files if we only print summary
	if($optionPrintCfgSummaries)
	{
		return;
	}

	#print("1: $line1");	print("2: $line2");	print("3: $line3");

	RemoveWhiteSpaces(\$line1);
	RemoveWhiteSpaces(\$line2);
	RemoveWhiteSpaces(\$line3);

	my $lineBreak = "\n";
	$lineBreak = "<br>\n" if($optionHtml);

	my $caseId = "";

	if($line1 =~ m/\[[0-9]+\][ ]+Title:\[([a-z0-9]+)$regexpCaseName\]$/i)
	{
		#"' ultraedit
		$caseId = $1;
		print("Writing log for: $caseId.\n") if($optionShowMessages);
	}

	# Write header for case

	my @writeArray;

	# split the first line
	push @writeArray, ( "<hr>" ) if($optionHtml);
	push @writeArray, ( substr($line1, 0, index($line1, "Title:")) . $lineBreak );
	push @writeArray, ( substr($line1, index($line1, "Title:")) . $lineBreak );
	my $title = substr($line1, index($line1, "Title:"));

	#($line1 . "\n");
	push @writeArray, ($line2 . $lineBreak);
	push @writeArray, ($line3 . $lineBreak);
	push @writeArray, ("\n");
	push @writeArray, ( "<hr>" ) if($optionHtml);

	# Include the script for the case in the file?

	if($optionCaseIncludedInLog)
	{
		foreach $linexxx (@{$refCaseDesc})
		{
			push @writeArray, ("$linexxx");
			push @writeArray, ($lineBreak) if($optionHtml);
		}
		push @writeArray, ( "<hr>" ) if($optionHtml);
		push @writeArray, ("\n");
	}

	my $passed = 0;
	$passed = 1 if($line3 =~ m/==> PASSED/);

	# Get the log for the case

	my $startLine = 0;
	my $endLine = 0;

	my $logLineCount = scalar(@$refLogLines);

	if($logLineCount > 0 && ($optionLogAllCases || !$passed) )
	{
		if($lastCaseLogLine >= $logLineCount)
		{
			$lastCaseLogLine = 0;
		}

		# Get the start and end time for the case

		my $found = 0;
		#StartTime: 16:12:10,2440, EndTime: 16:12:34,3880
		if($line2 =~ m/^StartTime: ([0-9]+):([0-9]+):([0-9]+)\,[0-9]*, Endtime: ([0-9]+):([0-9]+):([0-9]+)\,[0-9]*$/i)
		{
			$found = 1;
		}
		elsif($line2 =~ m/^StartTime: ([0-9]+):([0-9]+):([0-9]+)\.[0-9]*[ A-Z]*, Endtime: ([0-9]+):([0-9]+):([0-9]+)\.[0-9]*[ A-Z]*$/i)
		{
			$found = 1;
		}
		else
		{
			print(" *ERROR* Could not parse case run time for case $caseId.  Can not write case log note. Returning.\n");
			return;
		}

		my $startSeconds = 0;
		my $endSeconds = 0;

		if($found)
		{
			$startSeconds = ($1 * 60 * 60) + $2 * 60 + $3;
			$endSeconds = ($4 * 60 * 60) + $5 * 60 + $6;

			my $caseRunTime = $endSeconds - $startSeconds;

			if($caseRunTime < 0)
			{
				# It's possible to code solution but it'd slow down the script.
				print(" *ERROR* Case $caseId has probably passed midnight/midday. Giving up writing case specific log.\n");
				return;
			}

			#print " - start:"  . $1 . ":" . $2 . ":" . $3 . " = " . $startSeconds . "\n";
			#print " - end: " . $4 . ":" . $5 . ":" . $6 . " = " . $endSeconds . "\n";
		}

		# Find indexes for start and end lines

		$startLine = -1;
		# If the line at $lastCaseLogLine matches then find backwards the first matching line,
		# otherwise some lines might be missed from the log note.
		if(LogLineTimeMatchesForCase(\@$refLogLines[$lastCaseLogLine], $startSeconds, $startSeconds))
		{
			#print("find back\n");
			for(my $i=$lastCaseLogLine; $i > 0; $i--)
			{
				if(!LogLineTimeMatchesForCase(\@$refLogLines[$i], $startSeconds, $startSeconds))
				{
					$startLine = $i+1;
					last;
				}
			}
		}
		# Otherwise try to find match forward.
		else
		{
			$startLine = FindLogLineMatch($refLogLines, $startSeconds, $endSeconds, $lastCaseLogLine, 1);
		}
		# If that doesn't succeed then lets go at start of the log. SLOW for huge logs.
		if($startLine == -1)
		{
			$startLine = FindLogLineMatch($refLogLines, $startSeconds, $endSeconds, 0, 1);
			$lastCaseLogLine = 0;
		}
		if($startLine == -1)
		{
			print(" *ERROR* startLine not found!\n");
			return;
		}

		print(" - StartLine: $startLine\n") if($optionShowMessages);

		# Find forward the last line which does not match.
		$endLine = FindLogLineMatch($refLogLines, $startSeconds, $endSeconds, $startLine+1, 0);

		if($endLine == -1 || $endLine < $startLine)
		{
		    print(" - End line not found, trying alternative method.\n") if($optionShowMessages);
			# Find first not matching line forward from the case start.
			$endLine = FindLogLineMatch($refLogLines, $startSeconds, $endSeconds, scalar(@$refLogLines)-1, 0);
			if( $endLine == -1 || $endLine < $startLine )
			{
				# Just take the rest of the log
				$endLine = scalar(@$refLogLines)-1;
			}
		}

		print(" - EndLine: $endLine\n") if($optionShowMessages);

		$lastCaseLogLine = $endLine-1;
	}

	# Create log directory

	if ( !(-e "testcase_logs") )
	{
  		mkdir("testcase_logs");
	}

	if ( !(-e "$caseLogOutDir") )
	{
		mkdir($caseLogOutDir, 0755);
	}

	# Write the log file

	my $fileMode = ">>";

	print(" - Case log file: " . $caseLogFile ."\n") if($optionShowMessages);

	if(	open(FILE_HANDLE, $fileMode . $caseLogFile) )
	{
		if($optionHtml)
		{
			print FILE_HANDLE (
			"<html>\n" .
			"<head>\n" .
			"<title>$title</title>\n" .
			"</head>\n" .
			"<body>\n" .
			"<basefont color=\"black\" face=\"arial\" size=\"4\">\n"
			);
		}

		foreach $line(@writeArray)
		{
			print FILE_HANDLE ($line);
		}

		for(my $i=$startLine;$i<$endLine;$i++)
		{
			my $line = @$refLogLines[$i];
			FormatLogLine(\$line) if($optionHtml);

			print FILE_HANDLE ($line);
		}

		if($optionHtml)
		{
		print FILE_HANDLE (
			"</body>\n" .
			"</html>"
			);
		}

        print FILE_HANDLE ( $lineBreak );
		print FILE_HANDLE ( $lineBreak );

		close FILE_HANDLE;

	} else
	{
		print("ERROR: Could not write to file: " . $caseLogFile . "\n");
	}
}

# There's case start entry in the log, write case log from there onwards.
sub WriteCaseLogFromEntry
{
 	my ($caseLogFile, $refLogLines, $caseEntryLogStartLine, $line1, $line2, $line3) = @_;

	# Don't generate log files if we only print summary
	if($optionPrintCfgSummaries)
	{
		return 0;
	}

	RemoveWhiteSpaces(\$line1);
	RemoveWhiteSpaces(\$line2);
	RemoveWhiteSpaces(\$line3);

	my $lineBreak = "\n";
	$lineBreak = "<br>\n" if($optionHtml);

	my $caseId = "";

	if($line1 =~ m/\[[0-9]+\][ ]+Title:\[([a-z0-9]+)$regexpCaseName\]$/i)
	{
		#"' ultraedit
		$caseId = $1;
	}

	# Write header for case

	my @writeArray;

	# split the first line
	push @writeArray, ( "<hr>" ) if($optionHtml);
	push @writeArray, ( substr($line1, 0, index($line1, "Title:")) . $lineBreak );
	push @writeArray, ( substr($line1, index($line1, "Title:")) . $lineBreak );
	my $title = substr($line1, index($line1, "Title:"));

	#($line1 . "\n");
	push @writeArray, ($line2 . $lineBreak);
	push @writeArray, ($line3 . $lineBreak);
	push @writeArray, ("\n");
	push @writeArray, ( "<hr>" ) if($optionHtml);

	my $passed = 0;
	$passed = 1 if($line3 =~ m/==> PASSED/);

	# Get the log for the case

	my $logLineCount = scalar(@$refLogLines);

	if($logLineCount > 0 && ($optionLogAllCases || !$passed) )
	{
		# Get the start and end time for the case

		my $found = 0;
		#StartTime: 16:12:10,2440, EndTime: 16:12:34,3880
		if($line2 =~ m/^StartTime: ([0-9]+):([0-9]+):([0-9]+)\,[0-9]*, Endtime: ([0-9]+):([0-9]+):([0-9]+)\,[0-9]*$/i)
		{
			$found = 1;
		}
		elsif($line2 =~ m/^StartTime: ([0-9]+):([0-9]+):([0-9]+)\.[0-9]*[ A-Z]*, Endtime: ([0-9]+):([0-9]+):([0-9]+)\.[0-9]*[ A-Z]*$/i)
		{
			$found = 1;
		}
		else
		{
			print(" *ERROR* Could not parse case run time for case $caseId.  Can not write case log note. Returning.\n");
			return 0;
		}

		my $startSeconds = 0;
		my $endSeconds = 0;

		if($found)
		{
			$startSeconds = ($1 * 60 * 60) + $2 * 60 + $3;
			$endSeconds = ($4 * 60 * 60) + $5 * 60 + $6;

			my $caseRunTime = $endSeconds - $startSeconds;

			if($caseRunTime < 0)
			{
				# It's possible to code solution but it'd slow down the script.
				#print(" *ERROR* Case has probably passed midnight/midday. Giving up writing case specific log.\n");
				#return;
			}
		}
	}

	# Create log directory

	if ( !(-e "testcase_logs") )
	{
  		mkdir("testcase_logs");
	}

	if ( !(-e "$caseLogOutDir") )
	{
		mkdir($caseLogOutDir, 0755);
	}

	# Write the log file

	my $fileMode = ">>";

	print(" - Case log file: " . $caseLogFile ."\n") if($optionShowMessages);

    my $caseEntryLogEndLine = -1;
    for(my $i=$caseEntryLogStartLine; $i<scalar(@$refLogLines); $i++ )
    {
        if(@$refLogLines[$i] =~ m/<<< Case end/i)
        {
            $caseEntryLogEndLine = $i;
            last;
        }
    }
    if( $caseEntryLogEndLine == -1 )
    {
        print("ERROR: $caseId is missing case end command in the script?\n");
        return 0;
    }

	if(	open(FILE_HANDLE, $fileMode . $caseLogFile) )
	{
		if($optionHtml)
		{
			print FILE_HANDLE (
			"<html>\n" .
			"<head>\n" .
			"<title>$title</title>\n" .
			"</head>\n" .
			"<body>\n" .
			"<basefont color=\"black\" face=\"arial\" size=\"4\">\n"
			);
		}

		for( my $i = $caseEntryLogStartLine; $i < $caseEntryLogEndLine; $i++ )
		{
            my $line = @$refLogLines[$i];
            FormatLogLine(\$line) if($optionHtml);
            print FILE_HANDLE ($line);
		}

		if($optionHtml)
		{
		    print FILE_HANDLE (
			    "</body>\n" .
			    "</html>"
			    );
		}
        print FILE_HANDLE ( $lineBreak );
		print FILE_HANDLE ( $lineBreak );

		close FILE_HANDLE;

	} else
	{
		print("ERROR: Could not write to file: " . $caseLogFile . "\n");
		return 0;
	}
	
	return 1;
}

#------------------------------------------------------------------------------------
# FindLogLineMatchRecurse
#
# Parameters:
# 	$startsSeconds		the min time the log line will match
# 	$endSeconds			the max time the log line will match
# 	$refStartLine		index of line to start searching
# 	$refLogLines		reference to array of the log
# 	$boolOp				1 = first line with match is returned, 0 = first line with no match is returned
# 	$inc				increment in the search loop
#	$recurse			can the function call itself, if yes this will be passed to the new call too
#	$maxLine			the last line to search
#
#------------------------------------------------------------------------------------
sub FindLogLineMatchRecurse
{
	my ($startSeconds, $endSeconds, $refStartLine, $refLogLines, $boolOp, $inc, $recurse, $maxLine) = @_;

    print("IN FindLogLineMatchRecurse\n") if($optionDebug);
	my $spacer = "";
	for(my $i = 0;$i<$recurse; $i++)
	{
		$spacer .= "-";
	}

	$maxLine = scalar(@$refLogLines) if(!defined($maxLine) or $maxLine > scalar(@$refLogLines));
	print("s: $startSeconds, e: $endSeconds, sl: $$refStartLine, b: $boolOp, i: $inc, r: $recurse, ml: $maxLine\n") if($optionDebug);
	print("$spacer ENTER: range: $$refStartLine - $maxLine, $inc\n") if($optionDebug);
	print("$spacer rec: $recurse\n") if($optionDebug);
	print("$spacer prev: $$refStartLine\n") if($optionDebug);
	print("$spacer inc: $inc\n") if($optionDebug);

	# First try to find the start line without recursion.
	for(my $i = $$refStartLine; $i < $maxLine; $i += $inc)
	{
		if(LogLineTimeMatchesForCase(\@$refLogLines[$i], $startSeconds, $endSeconds) == $boolOp)
		{
			# We have exact match.
			if($inc == 1)
			{
				$$refStartLine = $i;
			}
			# Not exatch match. Return start of previous block.
			elsif($i >= $$refStartLine + $inc)
			{
				$$refStartLine = $i - $inc;
			}

			print("MATCH LINE: $i: " . @$refLogLines[$i] . "\n") if($optionDebug);
			print("return line: " . $$refStartLine . "\n") if($optionDebug);
			print("EXIT FindLogLineMatchRecurse. return 1\n") if($optionDebug);
			return 1;
		}
	}

	if($recurse && $inc > 500)
	{
		# Split the current search block to smaller blocks and do search to them
		for(my $i = $$refStartLine; $i < $maxLine; $i += $inc)
		{
			print("$spacer $i\n") if($inc != 1 && $optionDebug);
			print("...RECURSING!\n") if($optionDebug);

			# Start search from the start of current block.
			my $prevPos = $i;

			# New last line to search
			my $end = $i + $inc;
			$end = scalar(@$refLogLines)-1 if($end > scalar(@$refLogLines)-1);

			# New increment, twice smaller than now
			my $newInc = int($inc / 2 + 0.5);
			$newInc = 200 if($newInc < 200);

			if(FindLogLineMatchRecurse($startSeconds, $endSeconds, \$prevPos, $refLogLines, $boolOp, $newInc, $recurse-1, $end) == 1)
			{
				$$refStartLine = $prevPos;
				print("EXIT FindLogLineMatchRecurse. return 1\n") if($optionDebug);
				return 1;
			}
		}
	}

	print("EXIT FindLogLineMatchRecurse. return 0\n") if($optionDebug);
	return 0;
}

#------------------------------------------------------------------------------------
# FindLogLineMatch
#
# Searches for log line with time between the startSeconds and endSeconds.
#
# Parameters:
# 	$refLogLines		reference to array of the log
# 	$startsSeconds		the min time the log line will match
# 	$endSeconds			the max time the log line will match
# 	$startLine			index of line to start searching
# 	$boolOp				1 = first line with match is returned, 0 = first line with no match is returned
#------------------------------------------------------------------------------------
sub FindLogLineMatch
{
	my ($refLogLines, $startSeconds, $endSeconds, $startLine, $boolOp) = @_;
    print("IN FindLogLineMatch\n") if($optionDebug);
	my $prevPos = $startLine;

	#print("\n\nSTART: $startLine, boolop: $boolOp \n");

	my $match = 0;
	$match = FindLogLineMatchRecurse($startSeconds, $endSeconds, \$prevPos, $refLogLines, $boolOp, 6000, 5);
	$prevPos = 0 if(!$match);
	# Refine the search to be exact line
	$match = FindLogLineMatchRecurse($startSeconds, $endSeconds, \$prevPos, $refLogLines, $boolOp, 1, 0);

    print("EXIT FindLogLineMatch\n") if($optionDebug);
	# Not found.
	return -1 if(!$match);

	return $prevPos;
}

#------------------------------------------------------------------------------------
# JustFormatLogFile
#
# Parameters:
#	$fileName
#------------------------------------------------------------------------------------
sub JustFormatLogFile
{
	my ($fileName) = @_;

	if( !open(FILE_HANDLE, $fileName) )
	{
		die("ERROR! Could not open file '" . $fileName . "'\n");
	}
	my @array = <FILE_HANDLE>;
	close(FILE_HANDLE);

	$fileName = substr($fileName, 0, index($fileName, ".txt")) . ".html";

	if(	open(FILE_HANDLE, ">" . $fileName) )
	{

		print FILE_HANDLE (
		"<html>\n" .
		"<head>\n" .
		"<title>$fileName</title>\n" .
		"</head>\n" .
		"<body>\n" .
		"<basefont color=\"black\" face=\"arial\" size=\"4\">\n"
		);


		my $i;
		for($i = 0;$i<scalar(@array); $i++)
		{
			my $line = $array[$i];
			$line =~ s/\t/ /; #remove tabs
			FormatLogLine(\$line);

			print FILE_HANDLE ($line);
		}

		print FILE_HANDLE (
		"</body>\n" .
		"</html>"
		);

		close FILE_HANDLE;
	} else
	{
		print("ERROR: Could not write to file: " . $fileName . "\n");
	}

}

#------------------------------------------------------------------------------------
# LogLineTimeMatchesForCase
#
# Parameters:
#	$refLine
#	$startSeconds
#	$endSeconds
#------------------------------------------------------------------------------------
sub LogLineTimeMatchesForCase
{
	my ($refLine, $startSeconds, $endSeconds) = @_;

	return 0 if(!defined($$refLine));

	$$refLine =~ s/\t/ /; #remove tabs

	#print($$refLine . "\n");
	#print(" - $startSeconds, $endSeconds\n");

	if(${$refLine} =~ m/[0-9\/]+ ([0-9]+):([0-9]+):([0-9]+)/i)
	{
#			print("YES " . $1 . "\n");

		my $currSeconds = $1 * 60 * 60 + $2 * 60 + $3;
		#print(" - curr: $currSeconds\n");

		#print("curr: $currSeconds, start: $startSeconds, end: $endSeconds\n");

		if($currSeconds >= $startSeconds and $currSeconds <= $endSeconds)
		{
			#print("MATCH curr: " . $1 . ":" . $2 . ":" . $3 . " = " . $currSeconds . "\n");
			return 1;
		}
	}
	#print("NO MATCH\n");
	return 0;
}

#------------------------------------------------------------------------------------
# FormatLogLine
# Parameters:
# 	$line
#------------------------------------------------------------------------------------
sub FormatLogLine
{
	my ($line) = @_;

#Color

# sisään
#27/06/2006	9:37:52	>>>*									royalblue #2B60DE
# pois
#27/06/2006	9:37:52	<<<*									lightblue #0000FF
#27/06/2006	9:23:57	* exit
# destructor
#27/06/2006	9:23:58	~*										#FF0000
#27/06/2006	9:23:58	>>>~*

#27/06/2006	9:37:52	CIptv*Test *							GREY #808080
#27/06/2006	9:37:52	*error/fail								RED #FF0000
#27/06/2006	9:23:41	CreateServerProcess() 					GREEN #00FF00

#!!	TTestObject::~TTestObject								#800080

my $color = "#000000";


if(${$line} =~ m/([0-9\/]+ [0-9]+:[0-9]+:[0-9]+)/i)
{
	$date = $1;
	${$line} = substr(${$line}, length($date));

	$color = "#808080" if(${$line} =~ m/CIptvTest[a-z0-9]+Test/i);

	$color = "#2B60DE" if(${$line} =~ m/^[\t ]*>>>/i);
	$color = "#2B60DE" if(${$line} =~ m/\(\)$/i);
	$color = "#2B60DE" if(${$line} =~ m/enter$/i);

	$color = "#0000FF" if(${$line} =~ m/^[\t ]*<<<*/i);
	$color = "#0000FF" if(${$line} =~ m/ exit$/i);

	$color = "#800080" if(${$line} =~ m/^[\t ]*~/i);
	$color = "#800080" if(${$line} =~ m/^[\t ]*>>>~/i);
	$color = "#800080" if(${$line} =~ m/::~/i);

	$color = "#FF0000" if(${$line} =~ m/error|fail/i);
	
	$color = "#00AA00" if(${$line} =~ m/passed/i);
	$color = "#FF0000" if(${$line} =~ m/\*\*\*Testcase[a-z ]+\*\*\*/i);
	$color = "#00AA00" if(${$line} =~ m/\*\*\*Testcase PASSED+\*\*\*/i);
	$color = "#0000FF" if(${$line} =~ m/Executing line/i);
	
	my $formatStart = "", $formatEnd = "";

	if(${$line} =~ m/Client created semaphore, init/i) { $formatStart = "<b>"; $formatEnd = "</b>"; }
	if(${$line} =~ m/CIptvEngineServer:: ->Starting server shutdown/i) { $formatStart = "<b>"; $formatEnd = "</b>"; }

	SpecialChars($line);

	$date = "<font color=\"black\" face=\"arial\" size=\"2\">" . $date . "</font>";
	${$line} = $date . "$formatStart<font color=\"$color\" face=\"arial\" size=\"2\">" . ${$line} . "</font>$formatEnd<br>";
}


#$line = "<font color=\"$color\" face=\"arial\" size=\"2\"> . $line

#$color = "#2B60DE" if($line =~ m/([0-9\/]+ [0-9]+:[0-9]+:[0-9]+) >>>/i)


}

sub PrintCfgSummaries
{
	my $caseCfgFileName = "";
	my %cfgCaseCount;
	my %cfgCaseTotalTime;

	foreach $case(@cases)
	{
		$caseCfgFileName = @{$case}[$CASE_CFG_FILE];

		if(!defined($cfgCaseCount{$caseCfgFileName}))
		{
			$cfgCaseCount{$caseCfgFileName} = 0;
			$cfgCaseTotalTime{$caseCfgFileName} = 0;
		}

		if(defined(@{$case}[$CASE_RUN_TIME_SECONDS]))
		{
			$cfgCaseTotalTime{$caseCfgFileName} +=  @{$case}[$CASE_RUN_TIME_SECONDS];
		}


		$cfgCaseCount{$caseCfgFileName}++ if(@{$case}[$CASE_STATUS] ne "$caseDefaultStatus") ;
	}

	my $totalTime = 0;
	printf("\n%-50s %10s %10s\n", "CFG", "Case count", "Case run time");
	foreach $caseCfgFileName (sort keys(%cfgCaseCount))
	{
		if($cfgCaseCount{$caseCfgFileName} > 0)
		{
			my $mins = ($cfgCaseTotalTime{$caseCfgFileName}/60);
			$totalTime += $mins;
			printf("%-50s %10d %10.1f\n", $caseCfgFileName, $cfgCaseCount{$caseCfgFileName}, $mins);
		}
	}
	printf("Total time: %.2f hours\n\n", $totalTime/60 );

	print("No cases run for:\n");
	foreach $caseCfgFileName (sort keys(%cfgCaseCount))
	{
		if($cfgCaseCount{$caseCfgFileName} == 0)
		{
			print("$caseCfgFileName\n");
		}
	}
}

#------------------------------------------------------------------------------------
# WriteOfficeXml
#
# Parameters:
#	$outputFile
#	$briefSummary
#
# Writes the cases into a word formatted xml file.
#------------------------------------------------------------------------------------
sub WriteOfficeXml
{
	my ($outputFile, $briefSummary) = @_;

	if(	!open(FILE_HANDLE, ">" . $outputFile) )
	{
		print("ERROR! Could not open file '" . $outputFile . "'\n");
		return;
	}

	my $writeData = $xmlHeader;
	my $replacement;

	if($briefSummary == 0) {
		$writeData .= $xmlHeaderTableColumns;
	}
	else {
		$writeData .= $xmlHeaderTableColumnsBrief;
	}

	$excelRowCount = scalar(@cases) * 9 + 100;
	$writeData =~ s/$CaseCountMul8/$excelRowCount/;

	print FILE_HANDLE ( $writeData );

	if($briefSummary) {
		$writeData = $xmlSummaryData;

		$writeData =~ s/$xmlDataSummaryCaseCount/$summaryCaseCount/;
		$writeData =~ s/$xmlDataSummaryPassed/$summaryPassedCases/;
		$writeData =~ s/$xmlDataSummaryFailed/$summaryFailedCases/;
		$writeData =~ s/$xmlDataSummaryCrashed/$summaryCrashedCases/;
		$writeData =~ s/$xmlDataSummaryTimeout/$summaryTimeoutCases/;
		$writeData =~ s/$xmlDataSummaryRunRate/$summaryRunRate/;
		$writeData =~ s/$xmlDataSummaryPassRateTotal/$summaryPassRateTotal/;
		$writeData =~ s/$xmlDataSummaryPassRateRun/$summaryPassRateRun/;

		print FILE_HANDLE ( $writeData );
	}

	my $caseCfgFileName = "";

	my %cfgCaseCount;

	# Sort the cases by the config files, cfg's which have no run cases come last

	if(!$optionSortCases)
	{
		#print("HASH CREATE\n");
		#create hash of cfg case counts
		foreach $case(@cases)
		{
			$caseCfgFileName = @{$case}[$CASE_CFG_FILE];

			if(!defined($cfgCaseCount{$caseCfgFileName}))
			{
				$cfgCaseCount{$caseCfgFileName} = 0;
			}

			$cfgCaseCount{$caseCfgFileName}++ if(@{$case}[$CASE_STATUS] ne "$caseDefaultStatus") ;
		}

		#print("THRU CASES\n");

		foreach $caseCfgFileName (keys(%cfgCaseCount))
		{
			# if cfg has zero cases run then it's cases are put back of the array
			if($cfgCaseCount{$caseCfgFileName} == 0)
			{
				#print("ZERO CASES RUN: $caseCfgFileName\n");
				my @moveCases;

				my $i;
				for($i=0; $i< scalar(@cases); $i++)
				{
					if($cases[$i][$CASE_CFG_FILE] eq $caseCfgFileName)
					{
						push @moveCases, splice(@cases, $i, 1);
						$i--;
					}
				}

				push @cases, @moveCases;
			}
		}
	}

	# Write the xml file

	$caseCfgFileName = "";

	foreach $case(@cases)
	{
	    if($optionShowMessages || $optionDebug) 
		{
			print(" - Case Xml : " . @{$case}[$CASE_NAME]  . ", status: " . @{$case}[$CASE_STATUS] . "\n") if( $optionDebug || @{$case}[$CASE_STATUS] ne "Not planned" );
		}
		
		if($briefSummary == 0)
		{
			$writeData = $xmlData;
			$writeData .= $xmlData2;
			$writeData .= $xmlDataEmptyRow;
		}
		else
		{
			$writeData = "";

			# Write cfg file to the brief xml if it has changed for the case
			if($optionSortCases == 0 && $caseCfgFileName ne @{$case}[$CASE_CFG_FILE])
			{
				$caseCfgFileName = @{$case}[$CASE_CFG_FILE];

				$writeData .= $xmlDataEmptyRow;
				$writeData .= $xmlCfgFileBrief;
				$writeData .= $xmlDataBriefHeader;

				$replacement = @{$case}[$CASE_CFG_FILE];
				XmlReadyText(\$replacement);
				$writeData =~ s/$xmlDataCfgFile/$replacement/;
			}
			
			# Write log file links to xml.

			my $caseHasLog = ( defined(@{$case}[$CASE_LOG_FILE]) and @{$case}[$CASE_LOG_FILE] ne "" );
			my $caseHasScripterLog = ( defined(@{$case}[$CASE_LOG_FILE_SCRIPTER]) and @{$case}[$CASE_LOG_FILE_SCRIPTER] ne "" );
			my $caseLogLink1 = "";
			my $caseLogLink2 = "";
			
			if( $caseHasLog && $caseHasScripterLog )
			{
			    print("   - Case has both log files\n") if ($optionDebug );
			    $writeData .= $xmlDataBrief2Links;
			    $caseLogLink1 = @{$case}[$CASE_LOG_FILE];
			    $caseLogLink2 = @{$case}[$CASE_LOG_FILE_SCRIPTER];
			}
			elsif( $caseHasLog )
			{
			    print("   - Case has log file\n") if ($optionDebug );
			    $writeData .= $xmlDataBrief1Link;
			    $caseLogLink1 = @{$case}[$CASE_LOG_FILE];
			}
			elsif( $caseHasScripterLog )
			{
			    print("   - Case has scripter log file\n") if ($optionDebug );
			    $writeData .= $xmlDataBrief1Link;
			    $caseLogLink1 = @{$case}[$CASE_LOG_FILE_SCRIPTER];
		    }
			else
			{
			    print("   - Case has no log files\n") if ($optionDebug );
			    $writeData .= $xmlDataBriefNoLinks;
			}
			
			if( $caseLogLink1 ne "" )
			{
                print("   - Log file 1: $caseLogLink1\n") if ($optionDebug );
                $replacement = $caseLogLink1;
				XmlReadyText(\$replacement);
				
				#print("wordxml log file 1: $replacement\n";

				#print("repl: " . $replacement . "\n");
				$replacement = substr($replacement, index($replacement, "\\") + 1);
				#$replacement = ".\\testcase_logs\\..\\" . $replacement;
				#print("repl: " . $replacement . "\n");

				$writeData =~ s/$xmlDataCaseLink1/$replacement/;
			}

			if( $caseLogLink2 ne "" )
			{
			    print("   - Log file 2: $caseLogLink2\n") if ($optionDebug );
                $replacement = $caseLogLink2;
				XmlReadyText(\$replacement);
				
				#print("wordxml log file 2: $replacement\n";

				#print("repl: " . $replacement . "\n");
				$replacement = substr($replacement, index($replacement, "\\") + 1);
				#$replacement = ".\\testcase_logs\\..\\" . $replacement;
				#print("repl: " . $replacement . "\n");

				$writeData =~ s/$xmlDataCaseLink2/$replacement/;
			}
			
			# Write case fail reason to xml.
			
			$replacement = @{$case}[$CASE_REASON];
			XmlReadyText(\$replacement);
			$writeData =~ s/$xmlDataCaseReason/$replacement/;

            # Write case run time or result to xml.

			if(!$optionCaseRunTimes)
			{
				$replacement = @{$case}[$CASE_RESULT];
			}
			else
			{
				$replacement = @{$case}[$CASE_RUN_TIME_SECONDS];
			}

			# Write result to xml.
			
			XmlReadyText(\$replacement);
			$writeData =~ s/$xmlDataCaseResult/$replacement/;
		}

		#$writeData = $xmlData;
		#if($option_brief_summary == 0) { $writeData .= $xmlData2; }
		#$writeData .= $xmlDataEmptyRow;

		$replacement = @{$case}[$CASE_NAME];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseName/$replacement/;

		$replacement = @{$case}[$CASE_RUN_TIMES];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseRunTimes/$replacement/;

		$replacement = @{$case}[$CASE_FAILED];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseFailed/$replacement/;

		$replacement = @{$case}[$CASE_CRASHED];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseCrashed/$replacement/;

		$replacement = @{$case}[$CASE_PASSED];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCasePassed/$replacement/;

		$replacement = @{$case}[$CASE_STATUS];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseStatus/$replacement/;

		$replacement = @{$case}[$CASE_PURPOSE];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCasePurpose/$replacement/;

		$replacement = @{$case}[$CASE_MEANS];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseMeans/$replacement/;

		$replacement = @{$case}[$CASE_REQUIRED_SETTINGS];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseRequiredSettings/$replacement/;

		$replacement = @{$case}[$CASE_RELATED_REQUIREMENTS];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseRelatedRequirements/$replacement/;

		$replacement = @{$case}[$CASE_VERIFICATION];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseVerification/$replacement/;

		$replacement = @{$case}[$CASE_NOTE];
		XmlReadyText(\$replacement);
		$writeData =~ s/$xmlDataCaseNote/$replacement/;

		if(@{$case}[$CASE_STATUS] eq "UNKNOWN") {
			$replacement = $STYLE_ID_STATUS_UNKNOWN;
		}
		elsif(@{$case}[$CASE_STATUS] eq "CRASHED") {
			$replacement = $STYLE_ID_STATUS_CRASHED;
		}
		elsif(@{$case}[$CASE_STATUS] eq "FAILED") {
			$replacement = $STYLE_ID_STATUS_FAILED;
		}
		elsif(@{$case}[$CASE_STATUS] eq "PASSED") {
			$replacement = $STYLE_ID_STATUS_PASSED;
		}
		else {
			$replacement = $STYLE_ID_STATUS_NA;
		}

		$writeData =~ s/$STYLE_ID_STATUS_TAG/$replacement/;

		print FILE_HANDLE ( $writeData );
	}

	print FILE_HANDLE ( $xmlFooter );

	close(FILE_HANDLE);

	print("Report " . $outputFile . " written.\n");
}

#------------------------------------------------------------------------------------
# WriteOfficeXmlAlt
#
# Parameters:
#	$outputFile
#
# Writes the cases into a word formatted xml file.
#------------------------------------------------------------------------------------
sub WriteOfficeXmlAlt
{
	my ($outputFile, $refRuns) = @_;

	CalculateStats();

	if(	!open(FILE_HANDLE, ">" . $outputFile) )
	{
		print("ERROR! Could not open file '" . $outputFile . "'\n");
		return;
	}

	my $writeData = $xmlHeader;
	my $replacement;

    $writeData .= $xmlHeaderTableColumnsAlt;

    my $xRefRunInfo = @$refRuns[0];
    my $xRefCases = @$xRefRunInfo[2];
    my $caseCount = scalar( @$xRefCases ) + 1000;

	$excelRowCount = $caseCount * 9 + 100;
	$writeData =~ s/$CaseCountMul8/$excelRowCount/;

	print FILE_HANDLE ( $writeData );

	my $caseCfgFileName = "";

	my %cfgCaseCount;

	# Sort the cases by the config files, cfg's which have no run cases come last

	if(!$optionSortCases)
	{
		#print("HASH CREATE\n");
		#create hash of cfg case counts
		foreach $case(@cases)
		{
			$caseCfgFileName = @{$case}[$CASE_CFG_FILE];

			if(!defined($cfgCaseCount{$caseCfgFileName}))
			{
				$cfgCaseCount{$caseCfgFileName} = 0;
			}

			$cfgCaseCount{$caseCfgFileName}++ if(@{$case}[$CASE_STATUS] ne "$caseDefaultStatus") ;
		}

		#print("THRU CASES\n");

		foreach $caseCfgFileName (keys(%cfgCaseCount))
		{
			# if cfg has zero cases run then it's cases are put back of the array
			if($cfgCaseCount{$caseCfgFileName} == 0)
			{
				#print("ZERO CASES RUN: $caseCfgFileName\n");
				my @moveCases;

				my $i;
				for($i=0; $i< scalar(@cases); $i++)
				{
					if($cases[$i][$CASE_CFG_FILE] eq $caseCfgFileName)
					{
						push @moveCases, splice(@cases, $i, 1);
						$i--;
					}
				}

				push @cases, @moveCases;
			}
		}
	}

    my @sortedRuns;

    #Monday 25th August 2008
    while( 1 )
    {
        my $maxTotal = 0;
        my $refWinner;
        for( my $i=0; $i<scalar(@$refRuns); $i++ )
        {
            my $refRunInfo = @$refRuns[$i];
            my $date = @$refRunInfo[1];
            my @parts = split(" ", $date);
            my $total = 0 + $parts[1];

            if( $parts[2] eq "Jan" )
            {
                $total += 1 * 30;
            }
            elsif( $parts[2] eq "Feb" )
            {
                $total += 2 * 30;
            }
            elsif( $parts[2] eq "Mar" )
            {
                $total += 3 * 30;
            }
            elsif( $parts[2] eq "Apr" )
            {
                $total += 4 * 30;
            }
            elsif( $parts[2] eq "May" )
            {
                $total += 5 * 30;
            }
            elsif( $parts[2] eq "Jun" )
            {
                $total += 6 * 30;
            }
            elsif( $parts[2] eq "Jul" )
            {
                $total += 7 * 30;
            }
            elsif( $parts[2] eq "Aug" )
            {
                $total += 8 * 30;
            }
            elsif( $parts[2] eq "Sep" )
            {
                $total += 9 * 30;
            }
            elsif( $parts[2] eq "Oct" )
            {
                $total += 10 * 30;
            }
            elsif( $parts[2] eq "Nov" )
            {
                $total += 11 * 30;
            }
            elsif( $parts[2] eq "Dec" )
            {
                $total += 12 * 30;
            }

            if( $maxTotal < $total )
            {
                my $foundAlready = 0;
                foreach my $xrun ( @sortedRuns )
                {
                    if( $xrun == @$refRuns[$i] )
                    {
                        $foundAlready = 1;
                    }

                }

                if( !$foundAlready )
                {
                    $maxTotal = $total;
                    $winner = @$refRuns[$i];
                }
            }
        }
        push @sortedRuns, $winner;
        if( scalar(@sortedRuns) == scalar(@$refRuns) )
        {
            last;
        }
    }

    foreach $run ( @sortedRuns )
    {
        #print("sorted: " . @$run[1] . "\n");
    }
    foreach $run ( @$refRuns )
    {
        #print("orig: " . @$run[1] . "\n");
    }

    my $refMainRunInfo = $sortedRuns[0];
    my $refMainCases = @$refMainRunInfo[2];

	$caseCfgFileName = "";
    $replacement = "";

    # This prints only cases which are in first run. Build list of cases in all runs and use that.
    foreach $mainCase ( @$refMainCases )
    {
        $writeData = "";

        # Status of the current case in different runs
        my @statuses;
        my @results;
        for( my $i = 0; $i<100; $i++ )
        {
            $statuses[$i] = "UNKNOWN";
            $results[$i] = "";
        }

        for( my $i = 0; $i<scalar( @sortedRuns ); $i++ )
        {
            my $refRunInfo = $sortedRuns[$i];

            my $refCases = @$refRunInfo[2];

            foreach $case ( @$refCases )
            {
                if( @$case[$CASE_ID] eq @$mainCase[$CASE_ID] )
                {
                    $statuses[$i] = @$case[$CASE_STATUS];
                    $results[$i] = @$case[$CASE_RESULT];
                }
            }
        }

		# If cfg changes, write xml header for it
		if($caseCfgFileName ne @$mainCase[$CASE_CFG_FILE])
		{
			$caseCfgFileName = @$mainCase[$CASE_CFG_FILE];

			$writeData .= $xmlDataEmptyRow;
			$writeData .= $xmlCfgFileBrief;
			$writeData .= $xmlCfgFileAltHeader;

			$replacement = @$mainCase[$CASE_CFG_FILE];
			XmlReadyText(\$replacement);
			$writeData =~ s/$xmlDataCfgFile/$replacement/;


            for( $i = 0; $i<$ALT_RESULTCOUNT; $i++ )
            {
                $replacement = "";
                if( defined( $sortedRuns[$i] ) )
                {
                    $refRunInfo = $sortedRuns[$i];
                    $replacement = @$refRunInfo[1];
                }
                my $field = $xmlDataAltDateX . ($i+1);
                $writeData =~ s/$field/$replacement/;
            }
		}

        # Write xml for X cases

        $writeData .= $xmlCfgFileAltData;

	    $replacement = @$mainCase[$CASE_NAME];
	    XmlReadyText(\$replacement);
	    $writeData =~ s/$xmlDataCaseName/$replacement/;

        # Replace the X fields for runs
        for( my $i = 0; $i<$ALT_RESULTCOUNT; $i++ )
        {
            $replacement = $statuses[$i];
            $replacement = $results[$i];
            my $field = $xmlDataAltResultX . ($i+1);
            $writeData =~ s/$field/$replacement/;

    		$replacement = $STYLE_ID_STATUS_NA_CENTERED;
    		if($statuses[$i] eq "CRASHED") {
    			$replacement = $STYLE_ID_STATUS_CRASHED_CENTERED;
    		}
    		elsif($statuses[$i] eq "FAILED") {
    			$replacement = $STYLE_ID_STATUS_FAILED_CENTERED;
    		}
    		elsif($statuses[$i] eq "PASSED") {
    			$replacement = $STYLE_ID_STATUS_PASSED_CENTERED;
    		}
    		else {
    			$replacement = $STYLE_ID_STATUS_NA_CENTERED;
    		}

    		$writeData =~ s/$STYLE_ID_STATUS_TAG/$replacement/;
        }
        print FILE_HANDLE ( $writeData );
    }

	print FILE_HANDLE ( $xmlFooter );
	close(FILE_HANDLE);
	print("Alternative report " . $outputFile . " written.\n");
	return;
}

#------------------------------------------------------------------------------------
# WriteTDToolResultFile
#
# Parameters:
#	$outputFile
#
# Writes the cases into TDTool formatted file which can be exported into Quality Center
#------------------------------------------------------------------------------------
sub WriteTDToolResultFile
{
	my ($outputFile) = @_;

	if(	!open(FILE_HANDLE, ">" . $outputFile) )
	{
		print("ERROR! Could not open file '" . $outputFile . "'\n");
		return;
	}

	my $writeData;
	my $replacement;

	my %cfgCaseCount;

	# sort the cases by the config files, cfg's which have no run cases come last
	if(!$optionSortCases)
	{
		foreach $case(@cases)
		{
			$caseCfgFileName = @{$case}[$CASE_CFG_FILE];

			if(!defined($cfgCaseCount{$caseCfgFileName}))
			{
				$cfgCaseCount{$caseCfgFileName} = 0;
			}

			$cfgCaseCount{$caseCfgFileName}++ if(@{$case}[$CASE_STATUS] ne "$caseDefaultStatus") ;
		}

		#print("THRU CASES\n");

		foreach $caseCfgFileName (keys(%cfgCaseCount))
		{
			# if cfg has zero cases run then it's cases are put back of the array
			if($cfgCaseCount{$caseCfgFileName} == 0)
			{
				#print("ZERO CASES RUN: $caseCfgFileName\n");
				my @moveCases;

				my $i;
				for($i=0; $i< scalar(@cases); $i++)
				{
					if($cases[$i][$CASE_CFG_FILE] eq $caseCfgFileName)
					{
						push @moveCases, splice(@cases, $i, 1);
						$i--;
					}
				}

				push @cases, @moveCases;
			}
		}
	}

#test_set_name_here
#[1]APES PTSW_WBA_TS -1.1. - Server Version - (01) - COMM_SERVER_VERSION_GET_REQ
#Passed&Actual result:  Info: 0

	print FILE_HANDLE ( "IPTV_Engine test set\n" );

	foreach $case(@cases)
	{
		$writeData = "[1]";

		my $caseName = @{$case}[$CASE_NAME];
		$writeData .= $caseName;

		$writeData .= "\n";

		my $result;

		if(@{$case}[$CASE_STATUS] eq "UNKNOWN") {
			$result = "N/A";
		}
		elsif(@{$case}[$CASE_STATUS] eq "CRASHED") {
			$result = "Failed";
		}
		elsif(@{$case}[$CASE_STATUS] eq "FAILED") {
			$result = "Failed";
		}
		elsif(@{$case}[$CASE_STATUS] eq "PASSED") {
			$result = "Passed";
		}
		else {
			$result = "No Run";
		}

		$writeData .= $result;



		my $comments = "&Info";

		#$comments = "CFG file: @{$case}[$CASE_CFG_FILE];

		#$comments .= "Actual result = " . @{$case}[$CASE_RESULT] . ", Info = " . @{$case}[$CASE_REASON];

		$writeData .= $comments;

		$writeData .= "\n";

		# Replace bad characters for QC
		$writeData =~ s/\\/-/g;
		$writeData =~ s/\//-/g;
		$writeData =~ s/:/-/g;
		$writeData =~ s/\"/-/g;
		$writeData =~ s/\?/-/g;
		$writeData =~ s/\'/-/g;
		$writeData =~ s/\|/-/g;
		$writeData =~ s/\*/-/g;
		$writeData =~ s/\%/-/g;
		$writeData =~ s/ä/a/g;
		$writeData =~ s/ö/o/g;
		$writeData =~ s/\å/a/g;
		$writeData =~ s/</(/g;
		$writeData =~ s/>/)/g;

		if($result eq "Passed" or $result eq "Failed")
		{
			print FILE_HANDLE ( $writeData );
		}
	}

	close(FILE_HANDLE);

	print("Report " . $outputFile . " written.\n");
}


#------------------------------------------------------------------------------------
# FindCfgFiles
# Parameters:
#	$godir, $refFiles
#
# Finds cfg files from the the specified directory and it's sub directories.
#------------------------------------------------------------------------------------
sub FindCfgFiles
{
	my ($godir, $refFiles) = @_;
	
    my $startDir = cwd;

	chdir($godir) or die("Could not change dir to $godir");
	
	opendir(DIR, ".");
	my @files = sort(readdir(DIR));
	closedir(DIR);
	
	foreach $file(@files)
	{
		if($file eq "." or $file eq "..") {next};

		if (-d $file)
		{
		 	FindCfgFiles( $file, $refFiles );
		} else {
			if($file =~ m/\.cfg$/i)
			{
				next if($file =~ m/livetv/i and $optionNoLiveTvTests);

				next if(
						($file =~ m/IptvSecurityServicesTest/i or
						 $file =~ m/IptvRssSecurityTest/i or
						 $file =~ m/IptvLiveTvSep/i or
						 $file =~ m/IptvRssDownloadSecurityTest/i)

						 and $optionNoSecurityTests
						);

				next if($file =~ m/example.cfg/i);

				push @$refFiles, ( cwd . "/" . $file );
			}
		}
	}

	chdir("$startDir") or die("Could not change dir to $startDir");
}

#------------------------------------------------------------------------------------
# ParseCfg
# Parameters:
#	$file
#------------------------------------------------------------------------------------
sub ParseCfg
{
	my ($file) = @_;

	my $fileCaseCount = 0;

	if( !open(FILE_HANDLE, $file) )
	{
		print("ERROR! Could not open file '" . $file . "'\n");
		return 0;
	}
	my @array = <FILE_HANDLE>;
	close(FILE_HANDLE);

	my $lineCount = scalar(@array);
	my $i;
	my $cfgFileWritten = 0;
	my $caseDocumentationStart = -1;
	my $line;

	my @caseDesc; # lines for case desc and case itself

	for($i=0; $i<$lineCount; $i++ ) {
		$line = $array[$i];

		RemoveWhiteSpaces(\$line);

		#print("           $line\n");

		if($line =~ m/^\#\[IptvETDescription\]/)
		{
			$caseDocumentationStart = $i;
		}

		if($line =~ m/\[Test\]/)
		{
			if($caseDocumentationStart >= 0)
			{
				ReadCaseAndDescText(\@array, \@caseDesc, $caseDocumentationStart-1)
			}
			else
			{
				ReadCaseAndDescText(\@array, \@caseDesc, $i);
			}

			#print("readcase desc: " . scalar(@caseDesc) . "\n");
		}
		else
		{
		    #print("NOT READ readcase desc: " . scalar(@caseDesc) . "\n");
		}

	    #test case starts here
	    if($line =~ m/^title /)
	    {
	    	#print("COUNT: " . scalar(@caseDesc) . "\n\n");

			if($caseDocumentationStart >= 0)
			{
				#print(">>> Case start: " . $line . "\n");
				#$i +=
				ReadCase(\@array, $caseDocumentationStart, $file);
				$caseDocumentationStart = -1;
			}

	    	my $caseName = substr($line, length("title "));
	    	my $caseId = GetCaseIdFromName($caseName);

	    	RemoveWhiteSpaces(\$caseName);
	    	#SpecialChars(\$caseName);

	    	my $documentationExists = GetCase($caseId, \@cases);

			$caseDesc[0] = $caseName;
			push @caseDescs, [ @caseDesc ];
			@caseDesc = (); #new case starts

	    	# create empty array for case
	    	if(!$documentationExists)
	    	{
	    		if($cfgFileWritten == 0)
	    		{
	    			$cfgFileWritten = 1;
	    			push @notDocumentedCases, ("\n\n" . $file .
"\n************************************************************************************************\n"
						);

	    		}
	    		push @notDocumentedCases, ($caseName . "\n");

				#create dummy case
				my @case;
				$case[$CASE_NAME] = $caseName;

				$case[$CASE_ID] = $caseId;

				$case[$CASE_RUN_TIMES] = 0;
				$case[$CASE_CRASHED] = 0;
				$case[$CASE_FAILED] = 0;
				$case[$CASE_PASSED] = 0;
				$case[$CASE_STATUS] = "$caseDefaultStatus";
				$case[$CASE_PURPOSE] = "N/A";
				$case[$CASE_MEANS] = "N/A";
				$case[$CASE_REQUIRED_SETTINGS] = "N/A";
				$case[$CASE_RELATED_REQUIREMENTS] = "N/A";
				$case[$CASE_VERIFICATION] = "N/A";
				$case[$CASE_NOTE] = "N/A";
				$case[$CASE_REASON] = "";
				$case[$CASE_RESULT] = "";
				$case[$CASE_LOG_FILE] = "";
				$case[$CASE_LOG_FILE_SCRIPTER] = "";
				$case[$CASE_CHECK_RESULT_LOG_FILE] = 0;
				$case[$CASE_CFG_FILE] = lc( GetPathFileName($file) );
				$case[$CASE_RUN_TIME_SECONDS] = 0;

                if( IsCaseInCaseList( $case[$CASE_ID] ) )
    			{
        			push @cases, [ @case ];
    		    }
	    	}

	    	$fileCaseCount++;
	    	if($optionShowMessages) { print(" - Case: " . $caseName . "\n"); };
	    }
	}

	return $fileCaseCount;
}


#------------------------------------------------------------------------------------
# ReadCaseAndDescText
#
# Parameters:
#	$lines
#	$caseInfo
#	$startIndex
#------------------------------------------------------------------------------------
sub ReadCaseAndDescText
{
	my ($lines, $caseInfo, $startIndex) = @_;
	my $lineCount = @{$lines};

	my $readLines = 0;
	my $i = 0;

	@{$caseInfo}[0] = "N/A";

	for($i=$startIndex; $i<$lineCount; $i++)
	{
		my $line = @{$lines}[$i];

		@{$caseInfo}[ scalar(@{$caseInfo}) ] = $line;

		#print($line . "\n");

		if($line =~ m/\[Endtest\]/i)
		{
#				push @cases, [ @case ];

			#foreach $xxx (@{$caseInfo})	{		print("XXX: " . $xxx . "");	}

			return $readLines;
		}

		$readLines++;
	}

	return $readLines;
}

#------------------------------------------------------------------------------------
# ReadCase
#
# Parameters:
#	$lines
#	$startIndex
#	$file
#------------------------------------------------------------------------------------
sub ReadCase
{
	my ($lines, $startIndex, $file) = @_;
	my $lineCount = @{$lines};

	my $readLines = 0;
	my $i = 0;

	my $caseName = "";
	my $casePurpose = "";
	my $caseMeans = "";
	my $caseRequiredSettings = "";
	my $caseRelatedRequirements = "";
	my $caseVerification = "";
	my $caseNote = "";

	#this is where text is appended if it's on a line which has no description tag
	# -> multiline descriptions can be specified
	my $refAppend = 0;

	for($i=$startIndex; $i<$lineCount; $i++)
	{
		my $line = @{$lines}[$i];

		#print($line . "\n");

		RemoveWhiteSpaces(\$line);
		#$line =~ s/\s+$//; #whitespaces in the end
		#$line =~ s/^\s+//; #whitespaces at the start

		if($line =~ m/^\#\[EndIptvETDescription\]/)
		{
			#print(" ***** End case\n");

			my @case;

			$case[$CASE_NAME] = $caseName;
			$case[$CASE_ID] = GetCaseIdFromName($caseName);

			$case[$CASE_RUN_TIMES] = 0;
			$case[$CASE_CRASHED] = 0;
			$case[$CASE_FAILED] = 0;
			$case[$CASE_PASSED] = 0;
			$case[$CASE_STATUS] = "$caseDefaultStatus";
			$case[$CASE_PURPOSE] = $casePurpose;
			$case[$CASE_MEANS] = $caseMeans;
			$case[$CASE_REQUIRED_SETTINGS] = $caseRequiredSettings;
			$case[$CASE_RELATED_REQUIREMENTS] = $caseRelatedRequirements;
			$case[$CASE_VERIFICATION] = $caseVerification;
			$case[$CASE_NOTE] = $caseNote;
			$case[$CASE_REASON] = "";
			$case[$CASE_RESULT] = "";
			$case[$CASE_LOG_FILE] = "";
			$case[$CASE_LOG_FILE_SCRIPTER] = "";
			$case[$CASE_CHECK_RESULT_LOG_FILE] = 0;
			$case[$CASE_CFG_FILE] = $file;
			$case[$CASE_RUN_TIME_SECONDS] = 0;

            if( IsCaseInCaseList( $case[$CASE_ID] ) )
			{
    			push @cases, [ @case ];
			}

			return $readLines;
		}

		elsif($line =~ m/^\# Testname:/)
		{
			$caseName = substr($line, length("# Testname:"));
			RemoveWhiteSpaces(\$caseName);
			#SpecialChars(\$caseName);
			$refAppend = \$caseName;
		}

		elsif($line =~ m/^\# Purpose:/)
		{
			$casePurpose = substr($line, length("# Purpose:"));
			RemoveWhiteSpaces(\$casePurpose);
			#SpecialChars(\$casePurpose);
			$refAppend = \$casePurpose;
		}

		elsif($line =~ m/^\# Means:/)
		{
			$caseMeans = substr($line, length("# Means:"));
			RemoveWhiteSpaces(\$caseMeans);
			#SpecialChars(\$caseMeans);
			$refAppend = \$caseMeans;
		}

		elsif($line =~ m/^\# Required environment settings:/)
		{
			$caseRequiredSettings = substr($line, length("# Required environment settings:"));
			RemoveWhiteSpaces(\$caseRequiredSettings);
			#SpecialChars(\$caseRequiredSettings);
			$refAppend = \$caseRequiredSettings;
		}

		elsif($line =~ m/^\# Related requirements:/)
		{
			$caseRelatedRequirements = substr($line, length("# Related requirements:"));
			RemoveWhiteSpaces(\$caseRelatedRequirements);
			$refAppend = \$caseRelatedRequirements;
		}

		elsif($line =~ m/^\# Verification:/)
		{
			$caseVerification = substr($line, length("# Verification:"));
			RemoveWhiteSpaces(\$caseVerification);
			$refAppend = \$caseVerification;
		}

		elsif($line =~ m/^\# Note:/)
		{
			$caseNote = substr($line, length("# Note:"));
			RemoveWhiteSpaces(\$caseNote);
			$refAppend = \$caseNote;
		}

		elsif($line =~ m/^\#/)
		{
			my $text = substr($line, length("#"));
			RemoveWhiteSpaces(\$text);
			#print(" ***** JOTTAI MUuTA: " . $text. "\n");
			${$refAppend} .= " " . $text;

			RemoveWhiteSpaces($refAppend);

		}

		$readLines++;
	}

	return $readLines;
}

#------------------------------------------------------------------------------------
# GetCaseIdFromName
#
# Parameters:
#	$caseName
#------------------------------------------------------------------------------------
sub GetCaseIdFromName
{
	my ($caseName) = @_;

	return "NOID" if(!defined($caseName) || $caseName eq "");

	RemoveWhiteSpaces(\$caseName);
	#SpecialChars(\$caseName);

	if($caseName =~ m/(ET[0-9]+)[ ]+/)
	{
		return $1;
	}

	return $caseName;
}

#------------------------------------------------------------------------------------
# GetCaseField
#
# Parameters:
#	$caseName
#	$caseField
#------------------------------------------------------------------------------------
sub GetCaseField
{
	my ($caseNameOrId, $caseField) = @_;

	my $ret = "";

	foreach my $case(@cases)
	{
		if(@{$case}[$CASE_NAME] eq $caseNameOrId or @{$case}[$CASE_ID] eq $caseNameOrId)
		{
			if( defined(@{$case}[$caseField]) )
			{
				$ret = @{$case}[$caseField];
			}
		}
	}

	return $ret;
}

#------------------------------------------------------------------------------------
# GetCaseByNameOnly
#
# Parameters:
#	$caseName
#
# Removes the case IDs from case names and tries to find case from @cases array
#------------------------------------------------------------------------------------
sub GetCaseByNameOnly
{
	my ($caseName) = @_;

	my $startPos = index($caseName, " ");
	$caseName  = substr($caseName, $startPos) if($startPos != -1);

	RemoveWhiteSpaces(\$caseName);

	for(my $i = 0; $i<scalar(@cases); $i++)
	{
		my $name = $cases[$i][$CASE_NAME];
		$startPos = index($name, " ");
		return if($startPos == -1);
		$name  = substr($name, $startPos);
		RemoveWhiteSpaces(\$name);

		return $cases[$i] if($name eq $caseName);
	}
	return;
}

#------------------------------------------------------------------------------------
# GetCase
#
# Parameters:
#	$caseNameOrID
#
# Returns reference to case
#------------------------------------------------------------------------------------
sub GetCase
{
	my ($caseNameOrID, $refCases) = @_;

	for(my $i = 0; $i<scalar(@$refCases); $i++)
	{
        $case = @$refCases[$i];
		if(@$case[$CASE_NAME] eq $caseNameOrID or @$case[$CASE_ID] eq $caseNameOrID)
		{
			return $case;
		}
	}
	return;
}

#------------------------------------------------------------------------------------
# GetCaseDesc
#
# Parameters:
#	$caseNameOrID
#
# Returns reference to casedesc
#------------------------------------------------------------------------------------
sub GetCaseDesc
{
	my ($caseNameOrID) = @_;

	for(my $i=0; $i<scalar(@caseDescs); $i++)
	{
		if($caseDescs[$i][0] eq $caseNameOrID || $caseDescs[$i][0] eq $caseNameOrID)
		{
			return $caseDescs[$i];
		}
	}
	return;
}

#------------------------------------------------------------------------------------
# RemoveWhiteSpaces
#
# Parameters:
# 	$text
#------------------------------------------------------------------------------------
sub RemoveWhiteSpaces()
{
	my ($text) = @_;
	${$text} =~ s/\s+$//; #whitespaces in the end
	${$text} =~ s/^\s+//; #whitespaces at the start
}

#------------------------------------------------------------------------------------
# XmlReadyText
#
# Parameters:
# 	$text
#------------------------------------------------------------------------------------
sub XmlReadyText
{
	my ($text) = @_;

	$txtlt = "&lt;";
	$txtgt = "&gt;";
	${$text} =~ s/</$txtlt/g;
	${$text} =~ s/>/$txtgt/g;

	${$text} =~ s/Ä/&Auml/g;
	${$text} =~ s/ä/&auml/g;
	${$text} =~ s/Ö/&Ouml/g;
	${$text} =~ s/ö/&ouml/g;

	#ä &auml;

#	Ö &Ouml;

#	ö &ouml;

}

#------------------------------------------------------------------------------------
# SpecialChars
#
# Parameters:
# 	$text
#------------------------------------------------------------------------------------
sub SpecialChars
{
	my ($text) = @_;


	ReplaceChar($text, "etmerkki#39;", "'" );
	ReplaceChar($text, "etmerkkiamp;", "&" );
	ReplaceChar($text, "etmerkkiacute;", "´" );

	ReplaceChar($text, "etmerkkilsquo;", "‘" );
	ReplaceChar($text, "etmerkkirsquo;", "’" );
	ReplaceChar($text, "etmerkkisbquo;", "‚" );
	ReplaceChar($text, "etmerkkildquo;", "“" );
	ReplaceChar($text, "etmerkkirdquo;", "”" );
	ReplaceChar($text, "etmerkkibdquo;", "„" );
	ReplaceChar($text, "etmerkki#34;", "\"" );

	ReplaceChar($text, "etmerkki#40;", "(" );
	ReplaceChar($text, "etmerkki#41;", ")" );

	ReplaceChar($text, "etmerkkifrasl;", "/" );
	ReplaceChar($text, "etmerkkilt;", "<" );
	ReplaceChar($text, "etmerkkigt;", ">" );
	ReplaceChar($text, "etmerkki#166;" , "¦" );

	ReplaceChar($text, "etmerkkipara;", "¶" );
	ReplaceChar($text, "etmerkkimiddot;", "·" );
	ReplaceChar($text, "etmerkkifrac14;", "¼" );
	ReplaceChar($text, "etmerkkifrac12;", "½" );
	ReplaceChar($text, "etmerkkifrac34;" , "¾" );


	${$text} =~ s/etmerkki/&/g;

	#	print("TEXT: " . ${$text} . "\n");
}

#------------------------------------------------------------------------------------
# ReplaceChar
#
# Parameters:
# 	$text
#	$replacement
#	$char
#------------------------------------------------------------------------------------
sub ReplaceChar
{
	my ($text, $replacement, $char) = @_;

#	print("txt: " . ${$text} . "char: " . $char . ", repl: " . $replacement . "\n");

	while(index(${$text}, $char)  != -1)
	{
		my $pos = index(${$text}, $char);
		if($pos < 0) { $pos = 0 };

		$str1 = substr(${$text}, 0, $pos);
		$str2 = substr(${$text}, $pos+1);

#		print "STR: '" .  $str1 . "' - '" . $str2 . "'\n";

#		print "XXX: " . $str1 . $replacement .  $str2 . "\n";

		${$text} = $str1 . $replacement . $str2;;
	}
}

#------------------------------------------------------------------------------------
# PrintCases
#------------------------------------------------------------------------------------
sub PrintCases
{

	my ($fileName) = @_;

	my @buff;

	push @buff, ("Name, Status, LogFile\n");

	my $i;

	push @buff, ("----------------------------------------\nCASES\n--------------------------------------\n");
	foreach $case(@cases)
	{

		#	my $case = $cases[$i];
		if(0 or @{$case}[$CASE_STATUS] ne "$caseDefaultStatus")
			{

			push @buff, (@{$case}[$CASE_NAME] . " ");
			#@{$case}[$CASE_RUN_TIMES]
			#@{$case}[$CASE_CRASHED]
			#@{$case}[$CASE_FAILED]
			#@{$case}[$CASE_PASSED]
			push @buff, (@{$case}[$CASE_STATUS] . " ");
			#@{$case}[$CASE_PURPOSE]
			#@{$case}[$CASE_MEANS]
			#@{$case}[$CASE_REQUIRED_SETTINGS]
			#@{$case}[$CASE_RELATED_REQUIREMENTS]
			#@{$case}[$CASE_VERIFICATION]
			#@{$case}[$CASE_NOTE]
			#@{$case}[$CASE_REASON]
			#@{$case}[$CASE_RESULT]
			push @buff, ("file: " . @{$case}[$CASE_LOG_FILE]  . " ");

			push @buff, ("check log: " . @{$case}[$CASE_CHECK_RESULT_LOG_FILE]  . " ");

			push @buff, ("\n");

			}
	}

	open(XXX, ">" . $fileName);
	print XXX ( @buff );
	close(XXX);

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

	chdir($godir) or die("Could not change dir to $godir");

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

	chdir ($startDir) or die("Could not change dir to $startDir");
}


#------------------------------------------------------------------------------------
# GetAllFiles
# Parameters:
#	$goDir, where to start finding
#	$refIncfiles, reference to array which will hold found files
#   $subDirLevel, how deep in the directories we go to search files. < 0 is no limit.
#------------------------------------------------------------------------------------
sub GetAllFiles
{
	my ($godir, $refIncfiles, $subDirLevel) = @_;

	my $startDir = cwd;

	chdir($godir) or die("Could not change dir to $godir");

	#print("Now in: " . cwd . "\n");

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};
		
		if (! (-d $file) )
		{
		 	my $path = $startDir . "/" . $file;
		 	push @$refIncfiles, $path;		 	
		} elsif( $subDirLevel > 0 || $subDirLevel < 0 )
		{
			my $backupDir = cwd;
			chdir( $file ) or die("Could not change dir to $file");
			GetAllFiles( $godir, $refIncfiles, $subDirLevel-1 );
			chdir( $backupDir ) or die("Could not change dir to $backupDir");
		}
	}

	chdir ($startDir) or die("Could not change dir to $startDir");
}

#------------------------------------------------------------------------------------
# FindDirs
# Parameters:
#	$goDir, where to start finding
#	$fileSearch, filename search
#	$refIncfiles, reference to array which will hold found files
#------------------------------------------------------------------------------------
sub FindDirs
{
	my ($godir, $fileSearch, $refIncfiles) = @_;

	my $startDir = cwd;

	chdir($godir) or die("Could not change dir to $godir");

	#print("Now in: " . cwd . "\n");

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

		if (-d $file)
		{
		    if( $file =~ m/$fileSearch/i )
		    {
		        push @$refIncfiles, (cwd . "/" . $file);
		    }
		 	FindDirs( $file, $fileSearch, $refIncfiles );
		}
	}

	chdir ($startDir) or die("Could not change dir to $startDir");
}

#------------------------------------------------------------------------------------
# GetPathFileName
#
# Parameters:
# 	$str
#------------------------------------------------------------------------------------
sub GetPathFileName
{
    my ($str) = @_;

    my $startPos = rindex($str, "\\");
    if($startPos == -1)
    {
        $startPos = rindex($str, "/");
        return $str if($startPos == -1);
    }

    my $filename = substr($str, $startPos+1);

    return $filename;
}

#------------------------------------------------------------------------------------
# GetPathDir
#
# Parameters:
# 	$str
#------------------------------------------------------------------------------------
sub GetPathDir
{
    my ($str) = @_;

    my $startPos = rindex($str, "\\");
    if($startPos == -1)
    {
        $startPos = rindex($str, "/");
        return $str if($startPos == -1);
    }

    my $filename = substr($str, 0, $startPos);

    return $filename;
}

sub InitXmlData
{

$xmlHeader =
"<?xml version='1.0'?>" .
"<?mso-application progid='Excel.Sheet'?>" .
"<Workbook xmlns='urn:schemas-microsoft-com:office:spreadsheet'" .
" xmlns:o='urn:schemas-microsoft-com:office:office'" .
" xmlns:x='urn:schemas-microsoft-com:office:excel'" .
" xmlns:ss='urn:schemas-microsoft-com:office:spreadsheet'" .
" xmlns:html='http://www.w3.org/TR/REC-html40'>" .
" <DocumentProperties xmlns='urn:schemas-microsoft-com:office:office'>" .
"  <Author>Senbom Petri</Author>" .
"  <LastAuthor>Senbom Petri</LastAuthor>" .
"  <Created>2006-06-09T11:08:05Z</Created>" .
"  <Company>Nokia Oyj</Company>" .
"  <Version>11.6568</Version>" .
" </DocumentProperties>" .
" <ExcelWorkbook xmlns='urn:schemas-microsoft-com:office:excel'>" .
"  <WindowHeight>12210</WindowHeight>" .
"  <WindowWidth>18780</WindowWidth>" .
"  <WindowTopX>360</WindowTopX>" .
"  <WindowTopY>255</WindowTopY>" .
"  <ProtectStructure>False</ProtectStructure>" .
"  <ProtectWindows>False</ProtectWindows>" .
" </ExcelWorkbook>" .
" <Styles>" .
"  <Style ss:ID='Default' ss:Name='Normal'>" .
"   <Alignment ss:Vertical='Bottom'/>" .
"   <Borders/>" .
"   <Font/>" .
"   <Interior/>" .
"   <NumberFormat/>" .
"   <Protection/>" .
"  </Style>" .
"  <Style ss:ID='m155260432'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='m155260442'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='m155266416'>" .
"   <Alignment ss:Vertical='Bottom'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Font x:Family='Swiss' ss:Bold='1'/>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='m155266426'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='m155266436'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='m155266446'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='s21'>" .
"   <Alignment ss:Horizontal='Right' ss:Vertical='Bottom'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#99CCFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='s25'>" .
"   <Alignment ss:Horizontal='Right' ss:Vertical='Bottom'/>" .
"   <Borders>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#99CCFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='s26'>" .
"   <Borders>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='s27'>" .
"   <Alignment ss:Horizontal='Center' ss:Vertical='Bottom'/>" .
"   <Borders>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#99CCFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='s28'>" .
"   <Borders>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='s29'>" .
"   <Borders>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='s30'>" .
"   <Alignment ss:Horizontal='Right' ss:Vertical='Top'/>" .
"   <Borders>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#99CCFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='s34'>" .
"   <Alignment ss:Horizontal='Right' ss:Vertical='Top'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#99CCFF' ss:Pattern='Solid'/>" .
"  </Style>" .
"  <Style ss:ID='s35'>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .

#failed

"  <Style ss:ID='s36'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFF00' ss:Pattern='Solid'/>" .
"  </Style>" .

#passed style

"  <Style ss:ID='s37'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#CCFFCC' ss:Pattern='Solid'/>" .
"  </Style>" .

#crashed style

"  <Style ss:ID='s38'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FF6600' ss:Pattern='Solid'/>" .
"  </Style>" .

# UNKNOWN style

"  <Style ss:ID='s401'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FF6600' ss:Pattern='Solid'/>" .
"  </Style>" .


#NA style
"  <Style ss:ID='s39'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"  </Style>" .


#failed centered

"  <Style ss:ID='s36centered'>" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFF00' ss:Pattern='Solid'/>" .
"  </Style>" .

#passed style centered

"  <Style ss:ID='s37centered'>" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#CCFFCC' ss:Pattern='Solid'/>" .
"  </Style>" .

#crashed style centered

"  <Style ss:ID='s38centered'>" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FF6600' ss:Pattern='Solid'/>" .
"  </Style>" .

# UNKNOWN style centered

"  <Style ss:ID='s401centered'>" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FF6600' ss:Pattern='Solid'/>" .
"  </Style>" .


#NA style centered
"  <Style ss:ID='s39centered'>" .
"   <Alignment ss:Horizontal=\"Center\" ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"  </Style>" .

#summary style
"  <Style ss:ID='summary'>" .
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .

#summary real number style
"  <Style ss:ID='summaryreal'>" .
"   <NumberFormat ss:Format='Fixed'/> ".
"   <Alignment ss:Vertical='Top' ss:WrapText='1'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .

#summary header
"  <Style ss:ID='summary_header'>" .
"   <Alignment ss:Horizontal='Left' ss:Vertical='Bottom'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Font x:Family='Swiss' ss:Bold='1'/>" .
"  </Style>" .

#brief header
"  <Style ss:ID='brief_header'>" .
"   <Alignment ss:Horizontal='Center' ss:Vertical='Bottom'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Interior ss:Color='#99CCFF' ss:Pattern='Solid'/>" .
"  </Style>" .

"  <Style ss:ID='brief_heading_cfgfilename'>" .
"   <Alignment ss:Vertical='Bottom'/>" .
"   <Borders>" .
"    <Border ss:Position='Bottom' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Left' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Right' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"    <Border ss:Position='Top' ss:LineStyle='Continuous' ss:Weight='1'/>" .
"   </Borders>" .
"   <Font x:Family='Swiss' ss:Bold='1'/>" .
"   <Interior ss:Color='#CCFFCC' ss:Pattern='Solid'/>" .
"  </Style>" .

#link
"  <Style ss:ID='urlparent' ss:Name='Hyperlink'>" .
"   <Font ss:Color='#0000FF' ss:Underline='Single'/>" .
"  </Style>" .

"  <Style ss:ID='url' ss:Parent='urlparent'>" .
"   <Interior ss:Color='#FFFFFF' ss:Pattern='Solid'/>" .
"  </Style>" .

" </Styles>" .
" <Worksheet ss:Name='Sheet1'>";

$xmlHeaderTableColumns =
"  <Table ss:ExpandedColumnCount='18' ss:ExpandedRowCount='CASE_COUNT_MUL8' x:FullColumns='1'" .
"   x:FullRows='1'>" .
"   <Column ss:AutoFitWidth='0' ss:Width='138.75'/>" .
"   <Column ss:Index='3' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='5' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='7' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='16' ss:AutoFitWidth='0' ss:Width='26.25'/>" .
"   <Column ss:AutoFitWidth='0' ss:Width='15'/>" .
"   <Column ss:AutoFitWidth='0' ss:Width='525.75'/>\n\n";

$xmlHeaderTableColumnsBrief =
"  <Table ss:ExpandedColumnCount='25' ss:ExpandedRowCount='CASE_COUNT_MUL8' x:FullColumns='1'" .
"   x:FullRows='1'>" .
"   <Column ss:AutoFitWidth='0' ss:Width='138.75'/>" .
"   <Column ss:Index='2' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='3' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='4' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='5' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='6' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='7' ss:AutoFitWidth='0' ss:Width='65'/>" . #status
"   <Column ss:Index='8' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='9' ss:AutoFitWidth='0' ss:Width='39'/>" .
"   <Column ss:Index='12' ss:AutoFitWidth='0' ss:Width='260'/>" . # info
"   <Column ss:Index='13' ss:AutoFitWidth='0' ss:Width='26.25'/>" . # link1
"   <Column ss:Index='14' ss:AutoFitWidth='0' ss:Width='26.25'/>" . # link2
"   <Column ss:Index='16' ss:AutoFitWidth='0' ss:Width='26.25'/>" .
"   <Column ss:AutoFitWidth='0' ss:Width='15'/>" .
"   <Column ss:AutoFitWidth='0' ss:Width='525.75'/>\n\n";

$xmlHeaderTableColumnsAlt =
"  <Table ss:ExpandedColumnCount='25' ss:ExpandedRowCount='CASE_COUNT_MUL8' x:FullColumns='1'" .
"   x:FullRows='1'>" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"138.75\"/>" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"30\" ss:Span=\"4\"/>" .
"   <Column ss:Index=\"7\" ss:AutoFitWidth=\"0\" ss:Width=\"60.00\" ss:Span=\"9\"/>" .
"   <Column ss:Index=\"17\" ss:AutoFitWidth=\"0\" ss:Width=\"15\"/>" .
"   <Column ss:AutoFitWidth=\"0\" ss:Width=\"525.75\"/>";

$xmlFooter =

"  </Table>" .
"  <WorksheetOptions xmlns='urn:schemas-microsoft-com:office:excel'>" .

#splitted panes
#"  <Selected/>" .
#"   <SplitHorizontal>3060</SplitHorizontal>" .
#"   <TopRowBottomPane>11</TopRowBottomPane>" .
#"   <ActivePane>2</ActivePane>" .
#"   <Panes>" .
#"    <Pane>" .
#"     <Number>3</Number>" .
#"    </Pane>" .
#"    <Pane>" .
#"     <Number>2</Number>" .
#"     <RangeSelection>R12C1:R12C8</RangeSelection>" .
#"    </Pane>" .
#"   </Panes>" .

#not splitted
"   <Selected/>" .
"   <Panes>" .
"    <Pane>" .
"     <Number>3</Number>" .
"     <ActiveRow>7</ActiveRow>" .
"     <ActiveCol>2</ActiveCol>" .
"    </Pane>" .
"   </Panes>" .

"   <ProtectObjects>False</ProtectObjects>" .
"   <ProtectScenarios>False</ProtectScenarios>" .
"  </WorksheetOptions>" .
" </Worksheet>" .
" <Worksheet ss:Name='Sheet2'>" .
"  <WorksheetOptions xmlns='urn:schemas-microsoft-com:office:excel'>" .
"   <ProtectObjects>False</ProtectObjects>" .
"   <ProtectScenarios>False</ProtectScenarios>" .
"  </WorksheetOptions>" .
" </Worksheet>" .
" <Worksheet ss:Name='Sheet3'>" .
"  <WorksheetOptions xmlns='urn:schemas-microsoft-com:office:excel'>" .
"   <ProtectObjects>False</ProtectObjects>" .
"   <ProtectScenarios>False</ProtectScenarios>" .
"  </WorksheetOptions>" .
" </Worksheet>" .
"</Workbook>\n\n";

$xmlDataEmptyRow =
"   <Row>" .
"    <Cell ss:StyleID='s35'><Data ss:Type='String'></Data></Cell>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"    <Cell ss:StyleID='s35'/>" .
"   </Row>\n\n";

$xmlSummaryData =

$xmlDataEmptyRow .

"   <Row>" .
"    <Cell ss:StyleID='s35' ss:MergeAcross='20'>" . "<Data ss:Type='String'>INSERT TEST SOFTWARE HERE</Data></Cell>" .
"   </Row>\n\n" .

"   <Row>" .
"    <Cell ss:StyleID='s35' ss:MergeAcross='20'>" . "<Data ss:Type='String'>INSERT TEST DEVICE HERE</Data></Cell>" .
"   </Row>\n\n" .

$xmlDataEmptyRow .

"   <Row>\n" .
"    <Cell ss:MergeAcross='4' ss:StyleID='summary_header'><Data ss:Type='String'>Test Execution Metrics</Data></Cell>" .
"    <Cell ss:MergeAcross='12' ss:StyleID='s35'/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:StyleID='s25'><Data ss:Type='String'>Total cases:</Data></Cell>" .
"    <Cell ss:MergeAcross='3' ss:StyleID='summary'><Data ss:Type='Number'>$xmlDataSummaryCaseCount</Data></Cell>" .
"    <Cell ss:MergeAcross='12' ss:StyleID='s35'/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:StyleID='s25'><Data ss:Type='String'>Passed:</Data></Cell>" .
"    <Cell ss:MergeAcross='3' ss:StyleID='summary'><Data ss:Type='Number'>$xmlDataSummaryPassed</Data></Cell>" .
"    <Cell ss:MergeAcross='12' ss:StyleID='s35'/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:StyleID='s25'><Data ss:Type='String'>Failed:</Data></Cell>" .
"    <Cell ss:MergeAcross='3' ss:StyleID='summary'><Data ss:Type='Number'>$xmlDataSummaryFailed</Data></Cell>" .
"    <Cell ss:MergeAcross='12' ss:StyleID='s35'/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:StyleID='s25'><Data ss:Type='String'>Crashed:</Data></Cell>" .
"    <Cell ss:MergeAcross='3' ss:StyleID='summary'><Data ss:Type='Number'>$xmlDataSummaryCrashed</Data></Cell>" .
"    <Cell ss:MergeAcross='12' ss:StyleID='s35'/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:StyleID='s25'><Data ss:Type='String'>Timeout:</Data></Cell>" .
"    <Cell ss:MergeAcross='3' ss:StyleID='summary'><Data ss:Type='Number'>$xmlDataSummaryTimeout</Data></Cell>" .
"    <Cell ss:MergeAcross='12' ss:StyleID='s35'/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:StyleID='s25'><Data ss:Type='String'>Pass rate % (total cases):</Data></Cell>" .
"    <Cell ss:MergeAcross='3' ss:StyleID='summaryreal'><Data ss:Type='Number'>$xmlDataSummaryPassRateTotal</Data></Cell>" .
"    <Cell ss:MergeAcross='12' ss:StyleID='s35'/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:StyleID='s25'><Data ss:Type='String'>Pass rate % (run cases):</Data></Cell>" .
"    <Cell ss:MergeAcross='3' ss:StyleID='summaryreal'><Data ss:Type='Number'>$xmlDataSummaryPassRateRun</Data></Cell>" .
"    <Cell ss:MergeAcross='12' ss:StyleID='s35'/>\n" .
"   </Row>\n" .
"   <Row>\n" .
"    <Cell ss:StyleID='s25'><Data ss:Type='String'>Run rate %:</Data></Cell>" .
"    <Cell ss:MergeAcross='3' ss:StyleID='summaryreal'><Data ss:Type='Number'>$xmlDataSummaryRunRate</Data></Cell>" .
"    <Cell ss:MergeAcross='12' ss:StyleID='s35'/>\n" .
"   </Row>\n" .
 $xmlDataEmptyRow;

$xmlData =
"   <Row>" .
"    <Cell ss:StyleID='s21'><Data ss:Type='String'>Name:</Data></Cell>" .
"    <Cell ss:MergeAcross='15' ss:StyleID='m155266416'><Data ss:Type='String'>XML_DATA_CASE_NAME</Data></Cell>" .
"    <Cell ss:StyleID='s35'/>" .
"   </Row>";

# not included in the case documentation report
$xmlDataCaseStatusDisabled =
"   <Row>" .
"    <Cell ss:StyleID='s25'><Data ss:Type='String'>Run times:</Data></Cell>" .
"    <Cell ss:StyleID='s26'><Data ss:Type='Number'>XML_DATA_CASE_RUN_TIMES</Data></Cell>" .
#
"    <Cell ss:StyleID='s27'><Data ss:Type='String'>Crashed</Data></Cell>" .
"    <Cell ss:StyleID='s28'><Data ss:Type='Number'>XML_DATA_CASE_CRASHED</Data></Cell>" .

"    <Cell ss:StyleID='s27'><Data ss:Type='String'>Failed</Data></Cell>" .
"    <Cell ss:StyleID='s28'><Data ss:Type='Number'>XML_DATA_CASE_FAILED</Data></Cell>" .
"    <Cell ss:StyleID='s27'><Data ss:Type='String'>Passed</Data></Cell>" .
"    <Cell ss:StyleID='s28'><Data ss:Type='Number'>XML_DATA_CASE_PASSED</Data></Cell>" .
"    <Cell ss:StyleID='s27'><Data ss:Type='String'>Status</Data></Cell>" .
#orig status style id s28
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='String'>XML_DATA_CASE_STATUS</Data></Cell>" .
"    <Cell ss:StyleID='s28'/>" .
"    <Cell ss:StyleID='s28'/>" .
"    <Cell ss:StyleID='s28'/>" .
"    <Cell ss:StyleID='s28'/>" .
"    <Cell ss:StyleID='s28'/>" .
"    <Cell ss:StyleID='s28'/>" .
"    <Cell ss:StyleID='s28'/>" .
#"    <Cell ss:StyleID='s28'/>" .
#"    <Cell ss:StyleID='s29'/>" .
"    <Cell ss:StyleID='s35'/>" .
"   </Row>\n\n";
$xmlDataCaseStatusDisabled .= "";

$xmlData2 =
"   <Row ss:AutoFitHeight='0' ss:Height='30'>" .
"    <Cell ss:StyleID='s30'><Data ss:Type='String'>Purpose:</Data></Cell>" .
"    <Cell ss:MergeAcross='15' ss:StyleID='m155266426'><Data ss:Type='String'>XML_DATA_CASE_PURPOSE</Data></Cell>" .
"    <Cell ss:StyleID='s35'/>" .
"   </Row>" .
"   <Row ss:AutoFitHeight='0' ss:Height='30'>" .
"    <Cell ss:StyleID='s30'><Data ss:Type='String'>Means:</Data></Cell>" .
"    <Cell ss:MergeAcross='15' ss:StyleID='m155266436'><Data ss:Type='String'>XML_DATA_CASE_MEANS</Data></Cell>" .
"    <Cell ss:StyleID='s35'/>" .
"   </Row>" .
"   <Row ss:AutoFitHeight='0' ss:Height='30'>" .
"    <Cell ss:StyleID='s30'><Data ss:Type='String'>Required environment settings:</Data></Cell>" .
"    <Cell ss:MergeAcross='15' ss:StyleID='m155266446'><Data ss:Type='String'>XML_DATA_CASE_REQUIRED_SETTINGS</Data></Cell>" .
"    <Cell ss:StyleID='s35'/>" .
"   </Row>" .
"   <Row ss:AutoFitHeight='0' ss:Height='30'>" .
"    <Cell ss:StyleID='s30'><Data ss:Type='String'>Related requirements:</Data></Cell>" .
"    <Cell ss:MergeAcross='15' ss:StyleID='m155266446'><Data ss:Type='String'>XML_DATA_CASE_RELATED_REQUIREMENTS</Data></Cell>" .
"    <Cell ss:StyleID='s35'/>" .
"   </Row>" .

"   <Row ss:AutoFitHeight='0' ss:Height='30'>" .
"    <Cell ss:StyleID='s30'><Data ss:Type='String'>Verification:</Data></Cell>" .
"    <Cell ss:MergeAcross='15' ss:StyleID='m155260432'><Data ss:Type='String'>XML_DATA_CASE_VERIFICATION</Data></Cell>" .
"    <Cell ss:StyleID='s35'/>" .
"   </Row>" .
"   <Row ss:AutoFitHeight='0' ss:Height='30'>" .
"    <Cell ss:StyleID='s34'><Data ss:Type='String'>Note:</Data></Cell>" .
"    <Cell ss:MergeAcross='15' ss:StyleID='m155260442'><Data ss:Type='String'>XML_DATA_CASE_NOTE</Data></Cell>" .
"    <Cell ss:StyleID='s35'/>" .
"   </Row>\n\n";


$xmlDataBriefHeader =
"   <Row>" .
"    <Cell ss:MergeAcross='5' ss:StyleID='brief_header'><Data ss:Type='String'>Name</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>Status</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>Passes</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>Fails</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>Crashes</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>Result</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>Info</Data></Cell>" .
"    <Cell ss:MergeAcross='6' ss:StyleID='s35'/>" .
"   </Row>\n\n";

$xmlDataBriefNoLinks =
"   <Row>\n" .
"    <Cell ss:MergeAcross='5' ss:StyleID='m155266416'><Data ss:Type='String'>XML_DATA_CASE_NAME</Data></Cell>\n" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='String'>XML_DATA_CASE_STATUS</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_PASSED</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_FAILED</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_CRASHED</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_RESULT</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='String'>XML_DATA_CASE_REASON</Data></Cell>\n" .
"    <Cell ss:MergeAcross='7' ss:StyleID='s35'/>\n" .
"   </Row>\n\n";

$xmlDataBrief1Link =
"   <Row>\n" .
"    <Cell ss:MergeAcross='5' ss:StyleID='m155266416'><Data ss:Type='String'>XML_DATA_CASE_NAME</Data></Cell>\n" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='String'>XML_DATA_CASE_STATUS</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_PASSED</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_FAILED</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_CRASHED</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_RESULT</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='String'>XML_DATA_CASE_REASON</Data></Cell>\n" .
"    <Cell ss:StyleID='url' ss:HRef='XML_DATA_CASE_LINK1'><Data ss:Type='String'>Link</Data></Cell>\n" .
"    <Cell ss:MergeAcross='6' ss:StyleID='s35'/>\n" .
"   </Row>\n\n";

$xmlDataBrief2Links =
"   <Row>\n" .
"    <Cell ss:MergeAcross='5' ss:StyleID='m155266416'><Data ss:Type='String'>XML_DATA_CASE_NAME</Data></Cell>\n" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='String'>XML_DATA_CASE_STATUS</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_PASSED</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_FAILED</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_CRASHED</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='Number'>XML_DATA_CASE_RESULT</Data></Cell>\n" .
"    <Cell ss:StyleID='s39'><Data ss:Type='String'>XML_DATA_CASE_REASON</Data></Cell>\n" .
"    <Cell ss:StyleID='url' ss:HRef='XML_DATA_CASE_LINK1'><Data ss:Type='String'>Link</Data></Cell>\n" .
"    <Cell ss:StyleID='url' ss:HRef='XML_DATA_CASE_LINK2'><Data ss:Type='String'>Link</Data></Cell>\n" .
"    <Cell ss:MergeAcross='5' ss:StyleID='s35'/>\n" .
"   </Row>\n\n";

$xmlCfgFileBrief =
"   <Row>" .
"    <Cell ss:MergeAcross='11' ss:StyleID='brief_heading_cfgfilename'><Data ss:Type='String'>XML_DATA_CFG_FILE</Data></Cell>" .
"    <Cell ss:MergeAcross='6' ss:StyleID='s35'/>" .
"   </Row>\n\n";


$xmlCfgFileAltHeader =
"   <Row>" .
"    <Cell ss:MergeAcross='5' ss:StyleID='brief_header'><Data ss:Type='String'>Name</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE1</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE2</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE3</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE4</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE5</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE6</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE7</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE8</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE9</Data></Cell>" .
"    <Cell ss:StyleID='brief_header'><Data ss:Type='String'>XML_DATA_ALTDATE10</Data></Cell>" .
"    <Cell ss:MergeAcross='6' ss:StyleID='s35'/>" .
"   </Row>\n\n";


$xmlCfgFileAltData =
"   <Row>" .
"    <Cell ss:MergeAcross='5' ss:StyleID='m155266416'><Data ss:Type='String'>XML_DATA_CASE_NAME</Data></Cell>\n" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT1</Data></Cell>" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT2</Data></Cell>" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT3</Data></Cell>" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT4</Data></Cell>" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT5</Data></Cell>" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT6</Data></Cell>" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT7</Data></Cell>" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT8</Data></Cell>" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT9</Data></Cell>" .
"    <Cell ss:StyleID='STYLE_ID_STATUS'><Data ss:Type='Number'>XML_DATA_ALTRESULT10</Data></Cell>" .
"    <Cell ss:MergeAcross='6' ss:StyleID='s35'/>" .
"   </Row>\n\n";

} # sub InitXml

