<?php
include "includes/generator.php";
init("home");
?>
<html>
<head>
<?php pHead(); ?>
</head>
<body>
<?php pBodyPrologue(); ?>
<center>Welcome to the steghide website!</center>
<p>
<h2>Introduction:</h2>
<!-- FIXME: for layout of paragraphs following the <h2>s see http://sox.sourceforge.net/ -->
see old website
<h2>Dependencies:</h2>
required: libmhash, highly recommended: libmcrypt, libjpeg, optional: perl (tests) (layout: see debian package pages)
<p>
<h2>Contact:</h2>
announce, devel, author, subscribe-box for announce list
<p>
<h2>Hosting:</h2>
<table width=\"100%\">
<tr>
<td>
Thanks for hosting the website, the cvs, the mailing lists and all the other stuff goes
to <a href="http://sourceforge.net">sourceforge.net</a> !
<p>
The SourceForge project information website for steghide can be found <a href="http://sourceforge.net/projects/steghide/">here</a>.
</td>
<td>
sf.net logo
<!-- FIXME not included to avoid influencing page visit stats too much
<a href="http://sourceforge.net"><img src="http://sourceforge.net/sflogo.php?group_id=15895" width="88"
height="31" border="0" alt="SourceForge Logo"></a>
-->
</td>
</tr>
</table>
<?php pBodyEpilogue(getlastmod()); ?>
</body>
</html>
