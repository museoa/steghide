<?php
include "includes/generator.php";
init("development");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>

<?php pBoxHead("? Contributing ?"); ?>
written in C++, look at TODO, non-programmers: translations, testing (especially useful when on uncommon system)
<?php pBoxEnd(); ?>

<?php pBoxHead("Access to current development source code:"); ?>
You can access the most recent development source code via anonymous <a href="http://www.cvshome.org/">cvs</a>. Just
type the following lines:
<p>
$ cvs -d:pserver:anonymous@cvs.steghide.sourceforge.net:/cvsroot/steghide login
<br>CVS password:  [ Hit RETURN here ]
<p>
$ cvs -z3 -d:pserver:anonymous@cvs.steghide.sourceforge.net:/cvsroot/steghide co steghide
<p>
You can also <a href="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/steghide/">browse the cvs repository</a> on the web.

<!-- tarballs ? -->

<?php pBoxEnd(); ?>

<?php pBoxHead("Development mailing list:"); ?>
<!--
The <b>steghide-devel</b> mailing list serves as a discussion forum on steghide
development. If you want to report a bug, or have comments or suggestions, mail to this list.

version of steghide is released, the announcement will also be posted to this list,
so you do not have to subscribe to both lists. If you want to participate in the discussion
on the development of steghide, <a href="http://lists.sourceforge.net/lists/listinfo/steghide-devel">
subscribe to this list</a>.
<p>
-->
<?php pBoxEnd(); ?>

<?php pBoxHead("Source Code Documentation (doxygen):"); ?>
What is doxygen ?<p>
doxygen for current cvs (+ in other formats), doxygen for last version (+ in other formats)
<?php pBoxEnd(); ?>

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html
