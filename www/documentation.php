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
<a href="documentation/quickstart.php">quickstart page</a>. As a reference you will most likely want
to consult the <a href="documentation/manpage.php">man(ual) page</a> that comes with the distribution
(also available as <a href="documentation/manpage.pdf">pdf</a>, <a href="documentation/manpage.ps">postscript</a> and
<a href="documentation/manpage.ps.gz">gzipped postscript</a>).<br>
If you are interested in the details
of the implementation of steghide take a look at the source code documentation that comes with the
source code distribution (for more details see the <a href="development.php">development section</a>).

<!--
<h2>How steghide works:</h2>
<h3>Steganography</h3>
Steganography literally means covered writing. Its goal is to hide the fact
that some information does exist. This is often achieved by using a (rather
large) cover file and embedding the (rather short) secret message into this
file. The result is a innocuous looking file (the stego file) that contains
the secret message. Steganography is different from cryptography because in
cryptography everybody is allowed to know that a certain information exists,
but nobody with the key is allowed to know that information itself. In steganography
however you are not even allowed to know that a certain information exists if
you do not possess the correct key.

short terminology
explain basics of graph theoretic approach and why maintaining first order statistics is important
-->

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
