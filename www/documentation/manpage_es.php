<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<?php
ini_set("include_path", "../includes");
include "generator.php";
init("documentation/manpage_es");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue();
include "manpage_es.html"; ?>
<hr>
You can also download this manual as <a href="manual_es.pdf">pdf</a>, <a href="manual_es.ps">postscript</a> and
<a href="manual_es.ps.gz">gzipped postscript</a>.
<? pBodyEpilogue(getlastmod()); ?>
</body>
</html>
