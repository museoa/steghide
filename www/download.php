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

<?php pBoxHead ("Download the latest version of steghide (0.5.1):"); ?>
<b>Windows:</b>
<br>
<a href="download/steghide-0.4.6b.win32.zip">steghide.exe with libraries</a> (1.4 MB)
<p>
After downloading unzip the contents of the zip file into a directory of your choice. You can use steghide from the DOS command prompt.
<p>
NOTE: If you are using Windows NT or Windows 2000 and you get an error message
saying that a libmcrypt module could not be opened, try copying the contents of
the steghide\modules folder into the main steghide folder.
<p>

<b>Linux:</b>
<br>
<a href="download/steghide-0.4.6b-1.i386.rpm">binary rpm package</a> (453 KB)
<br>
<a href="download/steghide-0.4.6b-1.src.rpm">source rpm package</a> (225 KB)
<p>

<b>Source Code:</b>
<br>
<a href="download/steghide-0.4.6b.tar">as .tar</a> (1.1 MB)
<br>
<a href="download/steghide-0.4.6b.tar.gz">compressed as .tar.gz</a> (222 KB)
<br>
<a href="download/steghide-0.4.6b.tar.bz2">compressed as .tar.bz2</a> (186 KB)
<br>
<a href="download/steghide-0.4.6b.zip">compressed as .zip</a> (312 KB)
<br>
a <a href="download/buildwithgcc3.patch">patch</a> to compile version 0.4.6b on gcc3 (46 KB)<br>
<?php pBoxEnd(); ?>

<?php pBoxHead("3rd Party Packages:"); ?>
mandrake rpm, suse rpm, freebsd port, debian(?)
<?php pBoxEnd(); ?>

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
