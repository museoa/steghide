<?php
include "includes/generator.php";
init("development");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>
<h2>? Contributing ?</h2>
written in C++, look at TODO, non-programmers: translations, testing (especially useful when on uncommon system)
<h2>Access to current development source code:</h2>
anonymous cvs, webcvs, cvs tarballs (from sf.net) ?
<h2>Development mailing list:</h2>
subscribe box, archive
<h2>Source Code Documentation (doxygen):</h2>
What is doxygen ?<p>
doxygen for current cvs (+ in other formats), doxygen for last version (+ in other formats)
<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html
