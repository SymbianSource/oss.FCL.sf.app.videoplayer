use warnings;
use Cwd; # for cwd
use File::Copy;
use File::Path;

my $debug = 0;
my $allowRun = 1;
my $relType = "";
my $sisPath = "videoplayer\\tsrc\\testing\\sis";
my $certFile = "Nokia_RnDCert_02\.der";
my $keyFile = "Nokia_RnDCert_02\.key";
my $comment = ";TEMPCOMMENT ";


########################################
# PKG LISTS
#

my @engineTests = (
		"videoplayer\\tsrc\\testing\\sis\\fusion_engine_tests.pkg",
		"videoplayer\\mediasettings\\mediasettingsengine\\tsrc\\mediasettingsenginetest\\group\\mpxmediasettingsenginetest.pkg",
		"videoplayer\\mediasettings\\videoplayersettingsengine\\tsrc\\vpsettingsenginetest\\group\\vcxnssettingsenginetest.pkg",
		"videoplayer\\videocollection\\mpxmyvideoscollection\\tsrc\\mpxmvcolltest\\group\\VCXMyVideosCollectionPluginTest.pkg",
		"videoplayer\\videoplayer_plat\\videoplayer_startup_api\\tsrc\\group\\VideoPlayerStartupApiTest.pkg",
		"videoplayer\\videoplayer_plat\\video_player_file_details_dialog_api\\tsrc\\group\\ui_FileDetailsDialogtest.pkg",
		"videoutils\\videoutils_plat\\videoscheduler_api\\tsrc\\group\\videoscheduler_apitest.pkg",
		"videoutils\\videoutils_plat\\videoconnutility_api\\tsrc\\group\\videoconnutility_apitest.pkg"
	);

my @unitTests = (
		"videoplayer\\videoplayback\\videohelix\\tsrc\\ut_videohelixtest\\group\\videohelixtest.pkg",
		"videoplayer\\videoplayback\\videoplaybackcontrols\\tsrc\\videoplaybackcontrols_test\\group\\videoplaybackcontrolstest.pkg",
		"videoplayer\\videoplayback\\videoplaybackviews\\tsrc\\ut_userinputhandlertest\\group\\userinputhandlertest.pkg",
		"videoplayer\\videoplayerapp\\mpxvideoplayer\\tsrc\\ut_mpxvideoplayertest\\group\\mpxvideoplayertest.pkg",
		"videoplayer\\videocollection\\hgmyvideos\\tsrc\\ut_vcxhgmyvideosmainview\\group\\VcxHgMyVideosMainViewTest.pkg"
	);

########################################
# PARSE ARGUMENTS
#

while(scalar(@ARGV) >= 1)
{
	my $argument = shift(@ARGV);

	if($argument eq "-sispath")
	{
		$sisPath = shift();
	}

	elsif($argument eq "-type")
	{
		$relType = shift();
	}
	
	elsif($argument eq "-norun")
	{
		$allowRun = 0;
	}
	
	elsif($argument eq "-debug")
	{
		$debug = 1;
	}
	
	elsif($argument eq "-help")
	{
		ShowHelp();
	}
	
	else
	{
		ShowHelp();
	}
}

########################################
# MAIN FUNCTION
#

my $basepath = FindBasePath();
die("NO VIDEOAPP_DOMAIN FOLDER FOUND") if $basepath eq "";

print "CREATING ${basepath}\\${sisPath}\n" if $debug;
mkpath("${basepath}\\${sisPath}");

# if sispath doesn't contain files for signing, then search them under basepath
if ( !-e "${basepath}\\${sisPath}\\${certFile}" and !-e "${basepath}\\${sisPath}\\${keyFile}" )
{
	my @certfiles;
	FindFiles($basepath, $certFile, 1, \@certfiles);
	die("No $certFile found under ${basepath} !\n") if ( !$certfiles[0] );
	my @keyfiles;
	FindFiles($basepath, $keyFile, 1, \@keyfiles);
	die("No $keyFile found under ${basepath} !\n") if ( !$keyfiles[0] );

	print "COPYING $keyfiles[0] TO ${basepath}\\${sisPath}\\${keyFile}\n" if $debug;
	copy( $keyfiles[0], "${basepath}\\${sisPath}\\${keyFile}" ) if $allowRun;

	print "COPYING $keyfiles[0] TO ${basepath}\\${sisPath}\\${certFile}\n" if $debug;
	copy( $certfiles[0], "${basepath}\\${sisPath}\\${certFile}" ) if $allowRun;
}

die("No $certFile found in ${basepath}\\${sisPath}\n") if ( !-e "${basepath}\\${sisPath}\\${certFile}" );
die("No $keyFile found in ${basepath}\\${sisPath}\n") if ( !-e "${basepath}\\${sisPath}\\${keyFile}" );

foreach my $pkg ( @engineTests )
{
    CreateSis( $basepath, $pkg, $sisPath, $relType );
}

foreach my $pkg ( @unitTests )
{
    CreateSis( $basepath, $pkg, $sisPath, $relType );
}

########################################
# FindBasePath
#

sub FindBasePath
{
	print ">> FindBasePath\n" if $debug;
	$currpath = cwd;
	$basepath = "";
	my $domain = "VideoApp_Domain";
	
	my $pos = rindex( $currpath, $domain );
	
	if( $pos != -1 )
	{
		$basepath = "\\" . substr( $currpath, 3, $pos + length( $domain ) - 3 );
		$basepath =~ s/\//\\/g;
	  print "BASEPATH FOUND: $basepath\n" if $debug;
	}
	
	print "<< FindBasePath (" . $basepath . ")\n" if $debug;
	return $basepath;
}

########################################
# CreateSis
#

sub CreateSis
{
	my ( $baseDir, $pkgPath, $targetDir, $relType ) = @_;
	print ">>> CREATESIS\n" if $debug;
	my $cmd = "";
	print "baseDir: $baseDir\n" if $debug;
	print "pkgPath: $pkgPath\n" if $debug;
	print "targetDir: $targetDir\n" if $debug;
	
	my $pos = rindex( $pkgPath, '\\' );
	print "rindex: $pos\n" if $debug;
	
	my $temppath = substr( $pkgPath, 0, $pos );
	my $filename = substr( $pkgPath, $pos + 1, -4 );
	
	# add type to the end of the filename
	my $relfilename = $filename;
	if( $relType ne "" )
	{
		$relfilename .= "_" . $relType;
	}
	
	print "path: $temppath\n" if $debug;
	print "file: $filename\n" if $debug;
	print "relfile: $relfilename\n" if $debug;
	

	chdir("${basepath}\\${temppath}");
	
	AddComment( "${filename}.pkg" ) if $allowRun;
	if( $relType ne "" )
	{
		print "\nCREATE TEMPORARY PKG\n\n" if $debug;
		CreatePkg( $filename, $relType );
	}
	$cmd = "call makesis ${relfilename}\.pkg ${relfilename}\.sis";	
	print $cmd . "\n" if $debug;
	system($cmd) if $allowRun;
	RemComment( "${filename}.pkg" ) if $allowRun;
	copy( "${relfilename}\.sis", "${baseDir}\\${targetDir}\\${relfilename}\.sis" ) if $allowRun;
	chdir("${baseDir}\\${targetDir}");
	$cmd = "signsis ${relfilename}\.sis ${relfilename}\.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key";
	system($cmd) if $allowRun;
	unlink ( "${relfilename}\.sis" ) if $allowRun;
	print "<<< CREATESIS\n" if $debug;
}

	
########################################
# AddComment
#

sub AddComment
{
	my ($file) = @_;
	
	open(FILE_HANDLE, $file) or die ("Could not read file '$file'\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);
	    
  open(FILE_HANDLE, ">$file") or die ("Could not write file '$file'\n");
  foreach my $line ( @lines )
  {
	  if( $line =~ /\.\.\/init\/testframework\.ini/i or $line =~ /\/cccccc00\.cre/i or
	      $line =~ /\.\.\\init\\testframework\.ini/i or $line =~ /\\cccccc00\.cre/i )
	  {
	    if( $line !~ /$comment/ )
	    {
	    	print $line . "\n" if $debug;
				$line = $comment . $line;
				print $line . "\n" if $debug;
			}
	  }
	  print FILE_HANDLE $line;
  }	    
  close(FILE_HANDLE);
}

########################################
# RemComment
#

sub RemComment
{
	my ($file) = @_;
	
	open(FILE_HANDLE, $file) or die ("Could not read file '$file'\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);
	    
  open(FILE_HANDLE, ">$file") or die ("Could not write file '$file'\n");
  foreach my $line ( @lines )
  {
	  if( $line =~ /\.\.\/init\/testframework\.ini/i or $line =~ /\/cccccc00\.cre/i or
	      $line =~ /\.\.\\init\\testframework\.ini/i or $line =~ /\\cccccc00\.cre/i )
	  {
	    if( $line =~ /$comment/ )
	    {
	    	print $line . "\n" if $debug;
	    	$line = substr( $line, length( $comment ) );
	    	print $line . "\n" if $debug;
	    }
	  }
	  print FILE_HANDLE $line;
  }	    
  close(FILE_HANDLE);
}

########################################
# CreatePkg
#

sub CreatePkg
{
	my ($file, $type) = @_;
	
	open(FILE_HANDLE, "${file}\.pkg") or die ("Could not read file '${file}\.pkg'\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);

	my $currenttype = "";
  open(FILE_HANDLE, ">${file}_${type}\.pkg") or die ("Could not write file '${file}_${type}\.pkg'\n");
  foreach my $line ( @lines )
  {
	  if( $line =~ /\/armv5\/(\w+)\//i )
	  {
	  	$currenttype = $1;
	  	if( $currenttype ne $type )
	  	{
	  		print $line if $debug;
	  		$line =~ s/\/$currenttype\//\/$type\//;
	  		print $line if $debug;
	  	}
	  }
	  print FILE_HANDLE $line;
  }	    
  close(FILE_HANDLE);
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
				;
			}
		}
	}

	chdir ($startDir) or die("Could not change dir to $startDir");
}

#------------------------------------------------------------------------------------
# ShowHelp
#------------------------------------------------------------------------------------
sub ShowHelp {

print <<USAGE_EOF;
siscreator.pl

This script will search the VideoApp_Domain root folder from current path.
if the root folder is not found, the script will exit. In any other case,
it creates sis packages for components. 

Default path for sis-files is /videoplayer/tsrc/testing/sis.

Options:

-sispath    path under videoapp_domain to copy sis files
-type       wanted sisx type, urel or udeb
-debug      show debug message
-norun      do not run, for debugging purposes
-help       You're reading it.

USAGE_EOF

	exit();

};