#!/usr/bin/perl -w
use STSupport;

sub runAu {
	startSubCategory("au") ;
	addTestResult(runEmbExtCmp(DATADIR . "mulaw_std.au", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "pcm8_std.au", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "pcm16_std.au", DATADIR . RND100, {e => "none"}));
	endSubCategory() ;
}

sub runBmp {
	startSubCategory("bmp") ;
	addTestResult(runEmbExtCmp(DATADIR . "os21x1_std.bmp", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "os21x4_std.bmp", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "os21x8_std.bmp", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "os21x24_std.bmp", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "win3x1_std.bmp", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "win3x4_std.bmp", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "win3x8_std.bmp", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "win3x24_std.bmp", DATADIR . RND100, {e => "none"}));
	endSubCategory() ;
}

sub runJpeg {
	startSubCategory("jpeg") ;
	endSubCategory() ;
}

sub runWav {
	startSubCategory("wav") ;
	addTestResult(runEmbExtCmp(DATADIR . "pcm8_std.wav", DATADIR . RND100, {e => "none"}));
	addTestResult(runEmbExtCmp(DATADIR . "pcm16_std.wav", DATADIR . RND100, {e => "none"}));
	endSubCategory() ;
}

startCategory("File Formats") ;
runAu() ;
runBmp() ;
runJpeg() ;
runWav() ;
endCategory() ;
