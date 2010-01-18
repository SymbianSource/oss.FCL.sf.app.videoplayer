#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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

$pkgfile = $ARGV[0];


#presumable comment mark

print "file:$filu\n";

# Read file template
open(IN, "$pkgfile") or die "Cannot open file $pkgfile $_";
@template = <IN>;
close (IN);

foreach $line (@template) {
	#print $line;
	if ($line =~ /\"(\S+?dll)\"/gi) {
			$dll = $1;
			print "Elftrania: $dll\n";
			system("Elftran -version 10.1 $dll");
	}
	else{
		
	}
}