<?php
include "../includes/generator.php";
init("development/testing");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>
explain testsuite (perl need for system tests)...
provide reference tarball for difftest (? php script for other way round?)
<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
