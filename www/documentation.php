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
for the impantient: quick start, reference: man page, etc... (download in other formats)<p>
doxygen ?
<h2>How steghide works:</h2>
explain basics of graph theoretic approach and why maintaining first order statistics is important
<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
