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
<?php pBoxEnd(); ?>

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
