<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<?php
include "includes/generator.php";
init("steganography");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>

<?php pBoxHead("What is steganography ?"); ?>
general introduction
examples of images/audio files with data embedded by steghide
<!--
Steganography literally means covered writing. Its goal is to hide the fact
that some information (and not only communication - cryptography is also about hiding
some information, not some communication) does exist. This is often achieved by using a (rather
large) cover file and embedding the (rather short) secret message into this
file. The result is a innocuous looking file (the stego file) that contains
the secret message. Steganography is different from cryptography because in
cryptography everybody is allowed to know that a certain information exists,
but nobody with the key is allowed to know that information itself. In steganography
however you are not even allowed to know that a certain information exists if
you do not possess the correct key.

short terminology
explain why maintaining first order statistics is important
-->

<?php pBoxEnd(); ?>

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
