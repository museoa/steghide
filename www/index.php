<?php
include "includes/generator.php";
init("home");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>
<br>
<center><h2>Welcome to the steghide website!</h2></center>
<br>
<?php pBoxHead("Introduction:"); ?>
Steghide is a steganography program that is able to hide data in various kinds of image- and audio-files. The color- respectivly
sample-frequencies are not changed thus making the embedding resistant against first-order statistical tests.
<p>
The current version is 0.5.1.
<p>
Features:
<ul>
<li>compression of embedded data</li>
<li>encryption of embedded data</li>
<li>embedding of a checksum to verify the integrity of the extraced data</li>
<li>support for JPEG, BMP, WAV and AU files</li>
</ul>
<p>
Steghide is licensed under the GNU General Public License (GPL) which permits
modification and distribution of the program as long as these modifications are
made available to the public under the GPL. For more information, see the
<a href="http://www.gnu.org/copyleft/gpl.html">full text of the GPL</a>.
<?php pBoxEnd(); ?>

<?php pBoxHead("News:"); ?>
<!-- take news from http://sourceforge.net/export/projnews.php?group_id=15895&limit=3&flat=1&show_summaries=1 -->
<b>Sept. 30, 2003: steghide 0.5.1beta released!</b><br>
I have released a beta version for the upcoming 0.5.1 release. It is intended for testing
purposes. If you find a bug, please <a href="mailto:<shetzl@chello.at>">tell me</a>. You
can download steghide 0.5.1beta from the
<a href="http://www.sourceforge.net/projects/steghide">sourceforge project page</a>.
The package for MS Windows is not yet released. I will create and upload it in the next few days.<br>
If you download the source code and compile steghide yourself, please do not forget to
run "make check" after running "make" to execute a bunch of unit and system tests.
<p>
<?php pBoxEnd(); ?>

<?php pBoxHead("Dependencies:"); ?>
You should have the following libraries installed to use steghide.
<p>
<dl>
<dt><a href="http://mhash.sourceforge.net/">libmhash</a></dt>
 <dd>A library that provides various hash algorithms and cryptographic key generation algorithms. Steghide needs this library
 to convert a passphrase into a form that can be used as input for cryptographic and steganographic algorithms.</dd>
<dt><a href="http://mcrypt.sourceforge.net/">libmcrypt</a></dt>
 <dd>A library that provides a lot of symmetric encryption algorithms. If you compile steghide without libmcrypt you will not
 be able to use steghide to encrypt data before embedding nor to extract encrypted data (even if you know the correct passphrase).</dd>
<dt><a href="http://www.ijg.org/">libjpeg</a></dt>
 <dd>A library implementing jpeg image compression. Without this library you will not be able to embed data in jpeg files nor
 to extract data from jpeg files.</dd>
<dt><a href="http://www.gzip.org/zlib/">zlib</a></dt>
 <dd>A lossless data compression library. If you compile steghide without having this library installed you will not be able
 to use steghide to compress data before embedding nor to extract compressed data from a stego-file.</dd>
</dl>
<p>
Libmhash is absolutely required to compile steghide. While you
can compile it without the other libraries they are highly recommended as major functionality will not be available without them. If
you download the Windows version you do not have to worry about the libraries because they come with the package. If you use a major
Linux Distribution you probably do not have to worry about the libraries as well, most of them will likely be installed already.
<?php pBoxEnd(); ?>

<?php pBoxHead("Contact:"); ?>
If a new version of steghide is released, the announcement will be posted to the
<b>steghide-announce</b> mailing list. If you want to be informed of new versions, please
<a href="http://lists.sourceforge.net/lists/listinfo/steghide-announce">subscribe
to this list</a>.
<p>
The <b>steghide-devel</b> mailing list serves as a discussion forum on steghide
development. Bug reports, comments, suggestions, etc. are welcome here. If a new
version of steghide is released, the announcement will also be posted to this list,
so you do not have to subscribe to both lists. If you want to participate in the discussion
on the development of steghide, please <a href="http://lists.sourceforge.net/lists/listinfo/steghide-devel">
subscribe to this list</a>.
<p>
The author of steghide is <a href="mailto:Stefan Hetzl <shetzl@chello.at>">Stefan Hetzl</a>.
<?php pBoxEnd(); ?>

<?php pBoxHead("Hosting:"); ?>
Thanks for hosting the website, the cvs, the mailing lists and all the other stuff goes
to <a href="http://sourceforge.net">sourceforge.net</a> !<br>
The SourceForge project information website for steghide can
be found <a href="http://sourceforge.net/projects/steghide/">here</a>.
<?php pBoxEnd(); ?>

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
