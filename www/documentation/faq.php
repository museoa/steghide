<?php
include "../includes/generator.php";
init("documentation/faq")
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>
faq
<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
