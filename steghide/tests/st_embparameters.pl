#!/usr/bin/perl -w
use STSupport;

use constant EMPTY => {e => "none", Z => "", K => ""} ;

sub runEncryption {
	startSubCategory("Encryption") ;
	endSubCategory() ;
}

sub runCompression {
	startSubCategory("Compression") ;
	endSubCategory() ;
}

sub runChecksum {
	startSubCategory("Checksum") ;
	$ref = EMPTY ;
	%args = %$ref ;
	delete $args{K} ;
	addTestResult(runEmbExtCmp(DATADIR . "mulaw_std.au", DATADIR . RND100, \%args));
	addTestResult(runEmbExtCmp(DATADIR . "pcm16_std.au", DATADIR . RND100, \%args));
	addTestResult(runEmbExtCmp(DATADIR . "os21x1_std.bmp", DATADIR . RND100, \%args));
	addTestResult(runEmbExtCmp(DATADIR . "win3x24_std.bmp", DATADIR . RND100, \%args));
	addTestResult(runEmbExtCmp(DATADIR . "pcm8_std.wav", DATADIR . RND100, \%args));
	endSubCategory() ;
}

startCategory("Embedding Parameters", @ARGV) ;
runEncryption() ;
runCompression() ;
runChecksum() ;
exit endCategory() ;
