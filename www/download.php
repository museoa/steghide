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
<h2>Linux:</h2>
<h2>Source Code:</h2>
<h2>3rd Party Packages:</h2>
mandrake rpm, suse rpm, freebsd port, debian(?)
<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
