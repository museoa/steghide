<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<?php
ini_set("include_path", "./includes");
include "generator.php";
init("documentation");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>

<h2>What to Read:</h2>
If you are impatient and want to start using steghide as fast as possible take a look at the
Quick Start section below. As a reference you will most likely want
to consult the <a href="documentation/manpage.php">man(ual) page</a> that comes with the distribution
(also available as <a href="documentation/manpage.pdf">pdf</a>, <a href="documentation/manpage.ps">postscript</a> and
<a href="documentation/manpage.ps.gz">gzipped postscript</a>).
If you are interested in the details
of the implementation of steghide take a look at the source code documentation that comes with the
source code distribution (for more details see the <a href="development.php">development section</a>).

<h2>Quick Start:</h2>
Here are some examples of how steghide can be used. Take a look at these to get
a first impression. If you want more detailed information please read the
<a href="documentation/manpage.php">man(ual) page</a>.
<p>
The basic usage is as follows:
<pre>
  $ steghide embed -cf picture.jpg -ef secret.txt
  Enter passphrase:
  Re-Enter passphrase:
  embedding "secret.txt" in "picture.jpg"... done
</pre>
This command will embed the file secret.txt in the cover file picture.jpg.
<p>
After you have embedded your secret data as shown above you can send the file
picture.jpg to the person who should receive the secret message. The receiver
has to use steghide in the following way:
<pre>
  $ steghide extract -sf picture.jpg
  Enter passphrase:
  wrote extracted data to "secret.txt".
</pre>
If the supplied passphrase is correct, the contents of the original file
secret.txt will be extracted from the stego file picture.jpg and saved
in the current directory.
<p>
If you have received a file that contains embedded data and you want to get
some information about it before extracting it, use the info command:
<pre>
  $ steghide info received_file.wav
  "received_file.wav":
    format: wave audio, PCM encoding
    capacity: 3.5 KB
  Try to get information about embedded data ? (y/n) y
  Enter passphrase:
    embedded file "secret.txt":
      size: 1.6 KB
      encrypted: rijndael-128, cbc
      compressed: yes
</pre>
After printing some general information about the stego file (format, capacity) you will be
asked if steghide should try to get information about the embedded data. If you answer with
yes you have to supply a passphrase. Steghide will then try to extract the embedded data
with that passphrase and - if it succeeds - print some information about it.



<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
