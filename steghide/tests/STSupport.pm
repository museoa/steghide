package STSupport;
require Exporter;

our @ISA = qw(Exporter);
our @EXPORT = qw(
	DATADIR RND100
	getArgString
	startCategory endCategory startSubCategory endSubCategory addTestResult
	runEmbExtCmp
);

$curcategory = "";
$cursubcategory = "";
$subcatstatus = 0 ;

use constant DATADIR => "./data/" ;
use constant RND100 => "rnd100" ;

use constant TAB => "    " ;
use constant STEGHIDE => "../src/steghide" ;
use constant DEFAULTSTGFN => "stgfile" ;
use constant DEFAULTEXTFN => "extfile" ;
use constant DEFEMBARGBASE => (command => "embed", f => "", q => "", p => "\"a passphrase\"", sf => DEFAULTSTGFN) ;
use constant DEFEXTARGBASE => (command => "extract", f => "", q => "", p => "\"a passphrase\"", sf => DEFAULTSTGFN, xf => DEFAULTEXTFN) ;

#
# create a string of arguments ready to be fed to steghide
# expects a hash containing a "command" => "something" and argument/argument value pairs
# 
sub getArgString {
	my %args = @_ ;

	$retval = $args{command} ;
	delete $args{command} ;

	foreach $arg (keys %args) {
		$retval .= " -$arg $args{$arg}" ;
	}

	return $retval ;
}

sub startCategory {
	$curcategory = $_[0] ;
	print "$curcategory:\n" ;
}

sub endCategory {
	$curcategory = "" ;
	print "\n" ;
}

sub startSubCategory {
	$cursubcategory = $_[0] ;
	print TAB . $cursubcategory ;
	$subcatstatus = 1 ;
}

sub endSubCategory {
	$cursubcategory = "" ;
	if ($subcatstatus) {
		print "ok\n" ;
	}
	else {
		print " !! FAILED !!\n" ;
	}
}

sub addTestResult {
	if ($_[0]) {
		print ".";
	}
	else {
		print "F";
		$subcatstatus = 0 ;
	}
}

#
# compare two files
# returns true iff files are equal
sub compare {
	system "cmp -s $_[0] $_[1]" ;
	return not $? ;
}

#
# remove all files in the list passed to it
sub remove {
	while (@_) {
		$f = shift @_ ;
		system "rm -f $f" ;
	}
}

#
# run an embed-extract-compare test
# expects 1-4 arguments: cvrfn, embfn, callerembargs, callerextargs
sub runEmbExtCmp {
	# evaluate arguments
	$cvrfn = shift @_ ;
	$embfn = DATADIR . RND100 ;	# default
	if (@_) { $embfn = shift @_ ; }
	%callerembargs = () ;
	if (@_) {
		$ref = shift @_ ;
		%callerembargs = %$ref ;
	}
	%callerextargs = () ;
	if (@_) {
		$ref = shift @_ ;
		%callerextargs = %$ref ;
	}

	# prepare embedding
	%embargs = (
		DEFEMBARGBASE,
		cf => $cvrfn,
		ef => $embfn,
		%callerembargs
	) ;
	$embcommand = STEGHIDE . " " . getArgString(%embargs) ;

	# prepare extracting
	%extargs = (
		DEFEXTARGBASE,
		%callerextargs
	) ;
	$extcommand = STEGHIDE . " " . getArgString(%extargs) ;

	system $embcommand ;
	system $extcommand ;

	$retval = compare ($embargs{ef}, $extargs{xf}) ;
	remove ($embargs{sf}, $extargs{xf}) ;

	return $retval ;
}

1;
