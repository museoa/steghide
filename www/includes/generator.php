<?php
include 'definitions.php';

$ToRoot = "";
$SectionID = "";

function init ($secid)
{
	global $SectionID, $ToRoot;
	$SectionID = $secid;
	if (strpos ($secid, "/")) {
		$ToRoot = "../";
	}
}

/**
 * $section is a section id like "download" or "documentation/manpage"
 **/
function getSectionInfo ($secid)
{
	global $DefSections;
	$slashpos = strpos ($secid, "/");
	if ($slashpos) {
		// it is a subsection
		$mainsection = substr ($secid, 0, $slashpos);
		$subsection = substr ($secid, $slashpos + 1);
		return $DefSections[$mainsection]["subsections"][$subsection];
	}
	else {
		// it is a (main) section
		return $DefSections[$secid];
	}
}

function pseclinktr ($active, $css_td, $css_a, $imgsrc, $imgalt, $linkhref, $linkname, $spc)
{
	global $ToRoot;

	echo "$spc<tr><td class=\"$css_td\"";
	if ($active) {
		echo " colspan=\"2\"";
	}
	echo ">\n";
	echo "$spc <table cellspacing=\"0\" cellpadding=\"3\">\n";
	echo "$spc  <tr>\n";
	echo "$spc   <td><img src=\"$ToRoot$imgsrc\" alt=\"$imgalt\" border=\"0\"></td>\n";
	echo "$spc   <td><a class=\"$css_a\"";
	if ($linkhref != "") {
		echo " href=\"$ToRoot$linkhref\"";
	}
	echo ">$linkname</a></td>\n";
	echo "$spc  </tr>\n";
	echo "$spc </table>\n";
	echo "$spc</tr></tr>\n";
}

function psection ($sectioninfo, $sectionactive, $isfirst, $spc)
{
	global $ToRoot;

	if ($sectionactive && array_key_exists("subsections", $sectioninfo)) {
		/* this section is active and has subsections */
		if ($isfirst) {
			$csssection = "secopenfirst";
		}
		else {
			$csssection = "secopendefault";
		}

		$numsubsections = count($sectioninfo["subsections"]);
		echo "$spc<tr><td>\n"; // is part of hierachy table
		echo "$spc<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">\n"; // create table to contain section and open subsections

		// section name
		pseclinktr (TRUE, $csssection, "seclinkactivated", "$ToRoot\images/arrow_sec_down.png", "arrow down",
			"", $sectioninfo["linkname"], $spc);

		// dark area on the left
		echo "$spc<tr><td class=\"ssecleft\" rowspan=\"$numsubsections\" width=\"20%\"></td>\n";


		// print subsections
		$ssections = $sectioninfo["subsections"];
		reset($ssections);
		list($ssid, $ssinfo) = each($ssections); // first subsections (special treatment)
		echo "$spc<td class=\"ssecrightfirst\"><a class=\"sseclink\" href=\"";
		echo $ToRoot . $ssinfo["link"] . "\">" . $ssinfo["linkname"] . "</a></td>\n";
		while (list($ssid, $ssinfo) = each ($ssections)) { // other subsections
			echo "$spc<tr><td class=\"ssecrightdefault\"><a class=\"sseclink\" href=\"";
			echo $ToRoot . $ssinfo["link"] . "\">" . $ssinfo["linkname"] . "</a></td></tr>\n";
		}

		echo "$spc</table>\n";
		echo "$spc</td></tr>\n";
	}
	else {
		if ($isfirst) {
			$csssection = "secclosedfirst";
		}
		else {
			$csssection = "seccloseddefault";
		}

		if ($sectionactive) {
			/* this section is active but has no subsections */
			pseclinktr (TRUE, $csssection, "seclinkactivated", "images/arrow_sec_down.png", "arrow down",
				"", $sectioninfo["linkname"], $spc);
		}
		else {
			/* this section is not active */
			pseclinktr (FALSE, $csssection, "seclinkclickable", "images/arrow_sec_right.png", "arrow right",
				$sectioninfo["link"], $sectioninfo["linkname"], $spc);
		}
	}
}

/**
 * $activesec contains the name of the active section
 * $spc is the space to print at the beginning of every line
 **/
function phierachytable ($activesec, $spc)
{
	global $DefSections;

	echo $spc . "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\"> <!-- start of hierachy table -->\n";
	$isfirst = TRUE ;
	foreach ($DefSections as $sectionid => $sectioninfo) {
		psection ($sectioninfo, ($sectionid == $activesec), $isfirst, $spc . " ");
		$isfirst &= FALSE;
	}
	echo $spc . "</table> <!-- end of hierachy table -->\n";
}

/**
 * print everything that should go between the <head> and </head> tags for the section $SectionID
 **/
function pHead ()
{
	global $CSSFile, $ToRoot, $SectionID;

	$secinfo = getSectionInfo($SectionID);
	echo "<title>" . $secinfo["title"] . "</title>\n";
	echo "<link rel=\"stylesheet\" type=\"text/css\" href=\"$ToRoot$CSSFile\">\n";
}

/**
 * print everything that should be printed in the body before the real text starts (including the hierachy table)
 **/
function pBodyPrologue ()
{
	global $SectionID;
	$sectioninfo = getSectionInfo($SectionID); 

	echo " <table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\">\n"; // main table to get 3(left)+2(right)=5 areas
  	echo "  <tr>\n"; // upper areas (logo, title)
	echo "   <td id=\"topleft\" width=\"170\">LOGO</td>\n"; // top left area (logo)
	echo "   <td id=\"topright\"><h1>" . $sectioninfo["title"] . "</h1></td>\n"; // top right area (title)
	echo "  </tr>\n";
	echo "  <tr>\n"; // lower areas (hierachy, text)
	echo "   <td width=\"170\" height=\"100%\" valign=\"top\">\n"; // lower left area containing the hierachy table
	phierachytable ($SectionID, "    ");
	echo "   </td>\n";
	echo "   <td rowspan=\"2\">\n"; // lower right area containing the text
	echo "    <table width=\"100%\"><tr><td> <!-- main text position -->\n"; // 1x1 table used for spacing only
}

/**
 * print everything that should be printed in the body after the real text has ended (including the "last update"-block)
 **/
function pBodyEpilogue ($ts_lastupdate)
{
	echo "     </td></tr></table> <!-- main text position -->\n"; // end of 1x1 table
	echo "   </td>\n";
	echo "  </tr>\n"; // end of second row in 3,2 table
	echo "  <tr>\n"; // lower area (lastupdate)
	echo "   <td id=\"lastupdate\" valign=\"bottom\">last update:<br>";
	echo date("F j, Y", $ts_lastupdate) . "</td>\n"; // print last update
	echo "  </tr>\n";
	echo " </table>\n"; // end of main table
}
?>
