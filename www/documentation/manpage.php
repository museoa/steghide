<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<?php
ini_set("include_path", "../includes");
include "generator.php";
init("documentation/manpage");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue();
include "manpage.html";
pBodyEpilogue(getlastmod()); ?>
</body>
</html>
