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
			"manpage" => array (
				"title" => "Steghide - manual",
				"linkname" => "manual",
				"link" => "documentation/manpage.php"
			),
			"manpage_es" => array (
				"title" => "Steghide - spanish manual",
				"linkname" => "spanish manual",
				"link" => "documentation/manpage_es.php"
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
