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

function isssec ($secid)
{
	return strpos($secid, "/");
}

/**
 * get section name of secid  
 **/
function getsec ($secid)
{
	$pos = strpos($secid, "/");
	if ($pos) {
		return substr($secid, 0, strpos($secid, "/"));
	}
	else {
		return $secid;
	}
}

/**
 * get subsection name of secid ("" if secid is a section)
 **/
function getssec ($secid)
{
	$pos = strpos($secid, "/");
	if ($pos) {
		return substr($secid, strpos($secid, "/") + 1);
	}
	else {
		return "";
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

/**
 * print a td that contains a subsection link
 * \param link what to put into href
 * \param linkname name(text) of link
 * \param pos is "first", "default" or "last" to indicate veritcal position of this subsection
 * \param isactive is this subsection currently active?
 **/
function psseclinktd ($link, $linkname, $pos, $isactive, $spc)
{
	global $ToRoot;

	$css_td = "ssecright$pos";
	if ($isactive) {
		$css_a = "sseclinkactive";
		$css_td .= "open";
	}
	else {
		$css_a = "sseclinkclickable";
		$css_td .= "closed";
	}

	echo $spc;
	if ($pos != "first") {
		echo "<tr>";
	}
	echo "<td class=\"$css_td\"><a class=\"$css_a\"";
	if (!$isactive) {
		echo " href=\"$ToRoot$link\"";
	}
	echo ">$linkname</a></td></tr>\n";
}

/**
 * print a section in the hierachy table
 * \param sec the section id of the section to print
 * \param sectioninfo the sectioninfo for the section to print
 * \param activesecid id of active section or subsection
 * \param isfirst TRUE iff this is the first section in the hierachy table
 * \param spc space to be left in front of every line of html output
 **/
function psection ($sec, $sectioninfo, $activesecid, $isfirst, $spc)
{
	global $ToRoot;

	if ($sec == getsec($activesecid)) {
		/* this section is active */
		if (array_key_exists("subsections", $sectioninfo)) {
			/* this section is active and has subsections (one of them may be active) */
			if (isssec($activesecid)) {
				$activesec = getsec($activesecid);
				$activessec = getssec($activesecid);
				echo "<!-- activessec: $activessec -->";
			}
			else {
				$activesec = $activesecid;
			}

			$numsubsections = count($sectioninfo["subsections"]);
			echo "$spc<tr><td>\n"; // is part of hierachy table
			echo "$spc<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">\n"; // create table to contain section and open subsections

			// section name
			if ($sec == $activesecid) {
				if ($isfirst) {
					$csssection = "secopenfirst";
				}
				else {
					$csssection = "secopendefault";
				}

				pseclinktr (TRUE, $csssection, "seclinkactive", "images/arrow_sec_down.png", "arrow down",
					"", $sectioninfo["linkname"], $spc);
			}
			else {
				if ($isfirst) {
					$csssection = "secclosedfirst";
				}
				else {
					$csssection = "seccloseddefault";
				}

				pseclinktr (TRUE, $csssection, "seclinkactive", "images/arrow_sec_down.png", "arrow down",
					$sectioninfo["link"], $sectioninfo["linkname"], $spc);
			}

			// dark area on the left - &nbsp; to make mozilla draw the left border
			echo "$spc<tr><td class=\"ssecleft\" rowspan=\"$numsubsections\" width=\"20%\">&nbsp;</td>\n";

			// print subsections
			$ssections = $sectioninfo["subsections"];

			$firstssec = array_splice ($ssections, 0, 1);
			$lastssec = array_splice ($ssections, count($ssections) - 1, 1);
			$otherssec = $ssections ;

			// first subsection (special treatment)
			$ssinfo = current($firstssec);
			psseclinktd ($ssinfo["link"], $ssinfo["linkname"], "first", (key($firstssec) == $activessec), $spc);

			// other subsections
			while (list($ssid, $ssinfo) = each ($otherssec)) { // other subsections
				psseclinktd ($ssinfo["link"], $ssinfo["linkname"], "default", ($ssid == $activessec), $spc);
			}

			// last subsection (special treatment)
			$ssinfo = current($lastssec);
			psseclinktd ($ssinfo["link"], $ssinfo["linkname"], "last", (key($lastssec) == $activessec), $spc);

			echo "$spc</table>\n";
			echo "$spc</td></tr>\n";
		}
		else {
			/* this section is active but has no subsections */
			if ($isfirst) {
				$csssection = "secclosedfirst";
			}
			else {
				$csssection = "seccloseddefault";
			}
			pseclinktr (TRUE, $csssection, "seclinkactive", "images/arrow_sec_down.png", "arrow down",
				"", $sectioninfo["linkname"], $spc);
		}
	}
	else {
		/* this section is not active */
		if ($isfirst) {
			$csssection = "secclosedfirst";
		}
		else {
			$csssection = "seccloseddefault";
		}
		pseclinktr (FALSE, $csssection, "seclinkclickable", "images/arrow_sec_right.png", "arrow right",
			$sectioninfo["link"], $sectioninfo["linkname"], $spc);
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
		psection ($sectionid, $sectioninfo, $activesec, $isfirst, $spc . " ");
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
	global $SectionID, $ToRoot;
	$sectioninfo = getSectionInfo($SectionID); 

	echo " <table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\">\n"; // main table to get 3(left)+2(right)=5 areas
  	echo "  <tr>\n"; // upper areas (logo, title)
	echo "   <td id=\"topleft\" width=\"170\" align=\"center\" valgin=\"middle\"><img src=\"" . $ToRoot . "images/logo.png" . "\"></td>\n"; // top left area (logo)
	echo "   <td id=\"topright\"><h1>" . $sectioninfo["title"] . "</h1></td>\n"; // top right area (title)
	echo "  </tr>\n";
	echo "  <tr>\n"; // lower areas (hierachy, text)
	echo "   <td width=\"170\" height=\"100%\" valign=\"top\">\n"; // lower left area containing the hierachy table
	phierachytable ($SectionID, "    ");
	echo "   </td>\n";
	echo "   <td rowspan=\"2\">\n"; // lower right area containing the text
	echo "    <table width=\"100%\" cellpadding=\"10\"><tr><td> <!-- main text position -->\n"; // 1x1 table used for spacing only
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

function pBoxHead ($title)
{
	echo "<table width=\"100%\" bgcolor=\"#000000\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">\n";
	echo "<tr><td valign=\"top\" bgcolor=\"#DED6C6\"><b>$title</b></td></tr>\n";
	echo "<tr><td valign=\"top\" bgcolor=\"#FFFFFF\">\n";
}

function pBoxEnd ()
{
	echo "</td></tr>\n";
	echo "</table>\n";
	echo "<br>\n";
}
?>
