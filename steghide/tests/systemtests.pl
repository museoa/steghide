#!/usr/bin/perl -w
use FileHandle ;
STDOUT->autoflush(1) ;

print "-------------------------\n\n" ;
print "Running System Tests.....\n\n" ;
print "-------------------------\n\n" ;

while (<st*.pl>) {
	system "./$_" ;
}
