<?php
include "../includes/generator.php";
init("documentation/manpage");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>
man page
<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
