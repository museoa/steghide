<?php
ini_set("include_path", "./includes");
include "generator.php";
init("download");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>

You can download the current version of steghide (0.5.1) on this page.
The download links point directly into the sourceforge.net download area.
As an alternative to this page you can select the files directly in the
<a href="http://sourceforge.net/project/showfiles.php?group_id=15895">sourceforge.net download area</a> for
steghide (also go there if you look for an older version).<br>

<h2>Windows:</h2>
<a href="http://prdownloads.sourceforge.net/steghide/steghide-0.5.1.win32.zip?download">Windows package</a>
<p>
After downloading unzip the contents of the zip file into a directory of your choice.
You can use steghide from the DOS command prompt.

<h2>Linux:</h2>
<a href="http://prdownloads.sourceforge.net/steghide/steghide-0.5.1-1.i386.rpm?download">binary rpm package</a>
<br>
<a href="http://prdownloads.sourceforge.net/steghide/steghide-0.5.1-1.src.rpm?download">source rpm package</a>
<p>
These rpm packages have been built on a SuSE 8.2 system but they should work on other systems too.

<h2>Source Code:</h2>
<a href="http://prdownloads.sourceforge.net/steghide/steghide-0.5.1.tar.gz?download">compressed as .tar.gz</a>
<br>
<a href="http://prdownloads.sourceforge.net/steghide/steghide-0.5.1.tar.bz2?download">compressed as .tar.bz2</a>
<br>
<a href="http://prdownloads.sourceforge.net/steghide/steghide-0.5.1.zip?download">compressed as .zip</a>

<h2>3rd Party Packages:</h2>
Some Linux distributions provide steghide packages: SuSE, Mandrake,
<a href="http://packages.debian.org/stable/non-us/steghide.html">debian</a>.
To find other rpm packages you might want to try <a href="http://www.rpmfind.net/">rpmfind.net</a>.
There also exists a FreeBSD port of steghide.

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
