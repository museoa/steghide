<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<?php
ini_set("include_path", "./includes");
include "generator.php";
init("development");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>

You can contribute to the development of steghide in various ways, for example by contributing code, translating the program's messages
or by running the test suite and sending bug reports.
Take a look at the TODO file that comes with the distribution for some ideas for future features.

<h2>Access to current development source code:</h2>
You can access the most recent development source code via anonymous <a href="http://www.cvshome.org/">cvs</a>. Just
type the following lines:
<pre><code>
$ cvs -d:pserver:anonymous@cvs.steghide.sourceforge.net:/cvsroot/steghide login
CVS password:  [ Hit RETURN here ]

$ cvs -z3 -d:pserver:anonymous@cvs.steghide.sourceforge.net:/cvsroot/steghide co steghide
</code></pre>
You can also <a href="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/steghide/">browse the cvs repository</a> on the web.

<h2>Development mailing list:</h2>
The <b>steghide-devel</b> mailing list serves as a discussion forum on steghide
development. If you want to report a bug, or have comments or suggestions, please send a mail
to this list. If a new version of steghide is released, the announcement will also be
posted to this list.<p>
If you want to subscribe to this list, please go to the
<a href="http://lists.sourceforge.net/lists/listinfo/steghide-devel">listinfo page</a>.

<h2>Source Code Documentation:</h2>
Steghide uses <a href="http://www.doxygen.org/">doxygen</a> as documentation system.
Doxygen makes it easy for a programmer to provide up-to-date source code documentation because the documentation
is part of the source code. This documentation can be a great help for you if you want to hunt down a bug or add a feature.
It is generated automatically in the <code>doc/doxygen</code> directory when you build steghide and <code>./configure</code>
has detected that you have doxygen installed. If you install doxygen later, do not forget to re-run <code>./configure</code>.

<h2>Test Suite:</h2>
The test suite can be found in the <code>tests</code> directory of the source code distribution and consists of
two parts: unit tests and system tests.

To run the test suite type <code>make check</code> after building steghide. If any test fails, please
<a href="mailto:steghide-devel@lists.sourceforge.net">report it</a>.

<h2>Internationalization:</h2>
The output of steghide is fully internationalized using <a href="http://www.gnu.org/software/gettext/">GNU gettext</a>. Translations
into french, spanish, romanian and german exist. If you want to translate steghide into another language you need to download the source code
distribution and fill in the empty lines in the file <code>steghide.pot</code> that can be found in the <code>po</code> subdirectory
(see <code>de.po</code> for an example).

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
