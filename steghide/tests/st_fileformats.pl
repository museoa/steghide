#!/usr/bin/perl -w
use STSupport;

sub runAu {
	startSubCategory("au") ;
	while (glob(DATADIR . "*.au")) {
		addTestResult(runEmbExtCmp($_, DATADIR . RND100, {e => "none"}));
	}
	endSubCategory() ;
}

sub runBmp {
	startSubCategory("bmp") ;
	while (glob(DATADIR . "*.bmp")) {
		addTestResult(runEmbExtCmp($_, DATADIR . RND100, {e => "none"}));
	}
	endSubCategory() ;
}

sub runJpeg {
	startSubCategory("jpeg") ;
	while (glob(DATADIR . "*.jpg")) {
		addTestResult(runEmbExtCmp($_, DATADIR . RND100, {e => "none"}));
	}
	endSubCategory() ;
}

sub runWav {
	startSubCategory("wav") ;
	while (glob(DATADIR . "*.wav")) {
		addTestResult(runEmbExtCmp($_, DATADIR . RND100, {e => "none"}));
	}
	endSubCategory() ;
}

startCategory("File Formats") ;
runAu() ;
runBmp() ;
runJpeg() ;
runWav() ;
endCategory() ;
