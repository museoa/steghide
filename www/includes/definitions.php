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
				"title" => "Steghide - Quick Start Examples",
				"linkname" => "quick start",
				"link" => "documentation/quickstart.php"
			),
			"manpage" => array (
				"title" => "Steghide - man page",
				"linkname" => "man page",
				"link" => "documentation/manpage.php"
			),
			"faq" => array (
				"title" => "Steghide - Frequently Asked Questions",
				"linkname" => "FAQ",
				"link" => "documentation/faq.php"
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
		"link" => "development.php",
		"subsections" => array (
			"testing" => array (
				"title" => "Steghide - Testing",
				"linkname" => "testing",
				"link" => "development/testing.php"
			),
			"doxygencvs" => array (
				"title" => "Steghide - Doxygen Documentation (current CVS)",	/* never used */
				"linkname" => "doxygen (cvs)",
				"link" => "documentation/doxygencvs/html/index.html" /* FIXME - in doc directory ? */
			),
			"doxygenlatest" => array (
				"title" => "Steghide - Doxygen Documentation (version 0.5.1)", /* never used */
				"linkname" => "doxygen (0.5.1)",
				"link" => "documentation/doxygenlatest/html/index.html" /* FIXME - in doc directory ? */
			)
		)
	),
	"steganography" => array (
		"title" => "Steghide - Steganography",
		"linkname" => "Steganography",
		"link" => "steganography.php",
		"subsections" => array (
			"links" => array (
				"title" => "Steghide - Steganography-Links",
				"linkname" => "links",
				"link" => "steganography/links.php"
			),
			"literature" => array (
				"title" => "Steghide - Steganography-Literature",
				"linkname" => "literature",
				"link" => "steganography/literature.php"
			)
		)
	)
);
?>
