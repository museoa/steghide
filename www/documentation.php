<?php
include "includes/generator.php";
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
<a href="documentation/quickstart.php">quickstart page</a>. For a more in-depth explanation, please
continue to read this page. As a reference you will most likely want
to consult the <a href="documentation/manpage.php">man(ual) page</a> that comes with the distribution
(also available as <a href="documentation/manpage.pdf">pdf</a>, <a href="documentation/manpage.ps">postscript</a> and
<a href="documentation/manpage.ps.gz">gzipped postscript</a>). If you are interested in the details
of the implementation of steghide take a look at the source code documentation that comes with the
source code distribution (for more details see the <a href="development.php">development section</a>).

<h2>How steghide works:</h2>
<h3>Steganography</h3>
Steganography literally means covered writing. Its goal is to hide the fact
that communication is taking place. This is often achieved by using a (rather
large) cover file and embedding the (rather short) secret message into this
file. The result is a innocuous looking file (the stego file) that contains
the secret message.

short terminology
explain basics of graph theoretic approach and why maintaining first order statistics is important

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
