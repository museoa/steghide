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

<?php pBoxHead("What to Read:"); ?>
for the impantient: quick start, reference: man page, etc... (download in other formats)<p>
doxygen ?
<?php pBoxEnd(); ?>

<?php pBoxHead("How steghide works:"); ?>
explain basics of graph theoretic approach and why maintaining first order statistics is important
<?php pBoxEnd(); ?>

<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
