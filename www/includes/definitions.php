<?php
$ArrowDown = "images/arrow_sec_down.png";
$ArrowRight = "images/arrow_sec_right.png";
$CSSFile = "includes/format.css";
$DefSections = array (
	"home" => array (
		"title" => "Steghide",
		"linkname" => "Home",
		"link" => "index.php"
	),
	"documentation" => array (
		"title" => "Steghide - Documentation",
		"linkname" => "Documentation",
		"link" => "documentation.php",
		"subsections" => array (
			"quickstart" => array (
				"title" => "Steghide - Quick Start",
				"linkname" => "quick start",
				"link" => "documentation/quickstart.php"
			),
			"manpage" => array (
				"title" => "Steghide - man page",
				"linkname" => "man page",
				"link" => "documentation/manpage.php"
			)
		)
	),
	"download" => array (
		"title" => "Steghide - Download",
		"linkname" => "Download",
		"link" => "download.php"
	),
	"development" => array (
		"title" => "Steghide - Development",
		"linkname" => "Development",
		"link" => "development.php"
	)
);
?>
