<?php
include "includes/generator.php";
init("download");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>

<h2>Windows:</h2>
<a href="download/steghide-0.5.1.win32.zip">steghide.exe with libraries</a> (1.4 MB)
After downloading unzip the contents of the zip file into a directory of your choice. You can use steghide from the DOS command prompt.
<p>
NOTE: If you are using Windows NT or Windows 2000 and you get an error message
saying that a libmcrypt module could not be opened, try copying the contents of
the steghide\modules folder into the main steghide folder.

<h2>Linux:</h2>
<a href="download/steghide-0.5.1-1.i386.rpm">binary rpm package</a> (453 KB)
<br>
<a href="download/steghide-0.5.1-1.src.rpm">source rpm package</a> (225 KB)

<h2>Source Code:</h2>
<a href="download/steghide-0.5.1.tar">as .tar</a> (1.1 MB)
<br>
<a href="download/steghide-0.5.1.tar.gz">compressed as .tar.gz</a> (222 KB)
<br>
<a href="download/steghide-0.5.1.tar.bz2">compressed as .tar.bz2</a> (186 KB)
<br>
<a href="download/steghide-0.5.1.zip">compressed as .zip</a> (312 KB)

<h2>3rd Party Packages:</h2>
Some Linux distributions provide steghide packages: SuSE, Mandrake,
<a href="http://packages.debian.org/stable/non-us/steghide.html">debian</a>.
To find rpm packages you might want to try <a href="http://www.rpmfind.net/">rpmfind.net</a>.
There also exists a FreeBSD port of steghide.

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
