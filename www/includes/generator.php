<?php
include 'definitions.php';

$Debug = false ;
$ToRoot = "";
$ActiveSectionID = "";
$FirstSectionID = "";
$NavRows = 0;	// size of the navigation part of the main table in rows

function init ($secid)
{
	global $ActiveSectionID, $ToRoot, $NavRows, $DefSections, $FirstSectionID, $Debug;
	$ActiveSectionID = $secid;
	if (strpos ($secid, "/")) {
		$ToRoot = "../";
	}
	$FirstSectionID = "home";
	$NavRows = count($DefSections) + 1 ; // also count the "last change"-area
	if ($Debug) {
		echo "\n<!-- DEBUG: after init: FirstSectionID: $FirstSectionID, NavRows: $NavRows -->\n" ;
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

/**
 * print a <td>...</td> that contains a section link in the MAIN or in one of the SECTION_* tables
 * \param tabletype MAIN or SECTION
 **/ 
function pseclinktd ($tabletype, $active, $css_td, $css_a, $imgsrc, $imgalt, $linkhref, $linkname, $spc)
{
	global $ToRoot;

	echo "$spc<td class=\"$css_td\"";
	if ($active && $tabletype == "SECTION") {
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
	echo "$spc</td>\n";
}

/**
 * print a td that contains a subsection link
 * \param link what to put into href
 * \param linkname name(text) of link
 * \param pos is "first", "default" or "last" or "only" to indicate veritcal position of this subsection
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
	if ($pos == "default" || $pos == "last") {
		echo "<tr>";
	}
	echo " <td class=\"$css_td\">\n";
	echo "$spc  <a class=\"$css_a\"";
	if (!$isactive) {
		echo " href=\"$ToRoot$link\"";
	}
	echo ">$linkname</a>\n";
	echo "$spc </td>\n";
	echo "$spc</tr>\n";
}

/**
 * print a <td>...</td> in the MAIN table that contains a section
 * \param $secid the section id of the section to print
 * \param $spc the amount of space to be kept before the opening <td>
 **/
function psectiontd ($sec, $spc)
{
	global $Debug, $ToRoot, $DefSections, $FirstSectionID, $ActiveSectionID ;
	if ($Debug) {
		echo "\n<!-- DEBUG: psectiontd called with sec: $sec, space: >$spc< -->\n";
	}
	$isfirst = ($sec == $FirstSectionID);
	$sectioninfo = $DefSections[$sec] ;

	if ($sec == getsec($ActiveSectionID)) { // is this section active ?
		if (array_key_exists("subsections", $sectioninfo)) {
			/* this section is active and has subsections (one of them may be active) */
			if (isssec($ActiveSectionID)) {
				$activesec = getsec($ActiveSectionID);
				$activessec = getssec($ActiveSectionID);
			}
			else {
				$activesec = $ActiveSectionID;
			}

			$numsubsections = count($sectioninfo["subsections"]);
			echo "$spc<td height=\"100%\"> <!-- section: $sec, ACTIVE -->\n";
			echo "$spc <table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\"";
			if ($Debug) {
				echo " style=\"border: solid 1px lime\"";
			}
			echo "> <!-- table: SECTION_$sec -->\n";
			echo "$spc  <tr>\n";

			// section name
			if ($sec == $ActiveSectionID) {
				pseclinktd ("SECTION", TRUE, "secopen", "seclinkactive", "images/arrow_sec_down.png", "arrow down",
					"", $sectioninfo["linkname"], $spc . "   ");
			}
			else {
				pseclinktd ("SECTION", TRUE, "secopen", "seclinkactive", "images/arrow_sec_down.png", "arrow down",
					$sectioninfo["link"], $sectioninfo["linkname"], $spc . "   ");
			}
			echo "$spc  </tr>\n";

			// dark area on the left - &nbsp; to make mozilla draw the left border
			echo "$spc  <tr>\n";
			echo "$spc   <td class=\"ssecleft\" rowspan=\"$numsubsections\" width=\"30\">&nbsp;</td>\n";

			// print subsections
			$ssections = $sectioninfo["subsections"];

			if (count($ssections) == 1) {
				// the only subsection
				$ssinfo = current($ssections);
				psseclinktd ($ssinfo["link"], $ssinfo["linkname"], "only", (key($ssections) == $activessec), $spc . "   ");
			}
			else {
				$firstssec = array_splice ($ssections, 0, 1);
				$lastssec = array_splice ($ssections, count($ssections) - 1, 1);
				$otherssec = $ssections ;

				// first subsection (special treatment)
				$ssinfo = current($firstssec);
				psseclinktd ($ssinfo["link"], $ssinfo["linkname"], "first", (key($firstssec) == $activessec), $spc . "   ");

				// other subsections
				while (list($ssid, $ssinfo) = each ($otherssec)) { // other subsections
					psseclinktd ($ssinfo["link"], $ssinfo["linkname"], "default", ($ssid == $activessec), $spc . "   ");
				}

				// last subsection (special treatment)
				$ssinfo = current($lastssec);
				psseclinktd ($ssinfo["link"], $ssinfo["linkname"], "last", (key($lastssec) == $activessec), $spc . "   ");
			}

			echo "$spc  </table> <!-- end of table: SECTION_$sec -->\n";
			echo "$spc </td>\n";
		}
		else {
			/* this section is active but has no subsections */
			pseclinktd ("MAIN", TRUE, "secclosed", "seclinkactive", "images/arrow_sec_down.png", "arrow down",
				"", $sectioninfo["linkname"], $spc);
		}
	}
	else {
		pseclinktd ("MAIN", FALSE, "secclosed", "seclinkclickable", "images/arrow_sec_right.png", "arrow right",
			$sectioninfo["link"], $sectioninfo["linkname"], $spc);
	}
}

function phierachytable ($spc)
{
	global $DefSections, $Debug;

	echo $spc . "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\"";
	if ($Debug) {
		echo " style=\"border: solid 1px red\"";
	}
	echo "> <!-- table: HIERACHY -->\n";
	$isfirst = TRUE ;
	foreach ($DefSections as $sectionid => $sectioninfo) {
		echo "$spc <tr>\n";
		psectiontd ($sectionid, $spc . "  ");
		echo "$spc </tr>\n";
		$isfirst &= FALSE;
	}
	echo $spc . "</table> <!-- end of table: HIERACHY -->\n";
}

/**
 * print everything that should go between the <head> and </head> tags for the section $ActiveSectionID
 **/
function pHead ()
{
	global $CSSFile, $ToRoot, $ActiveSectionID;
	$secinfo = getSectionInfo($ActiveSectionID);
	echo "<title>" . $secinfo["title"] . "</title>\n";
	echo "<meta name=\"author\" content=\"Stefan Hetzl <shetzl@chello.at>\">\n";
	echo "<meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-1\">\n";
	echo "<link rel=\"stylesheet\" type=\"text/css\" href=\"$ToRoot$CSSFile\">\n";
}

/**
 * print everything that should be printed in the body before the real text starts
 **/
function pBodyPrologue ()
{
	global $DefSections, $ActiveSectionID, $ToRoot, $FirstSectionID, $NavRows, $Debug;
	$sectioninfo = getSectionInfo($ActiveSectionID); 

	echo " <table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\"";
	if ($Debug) {
		echo " border=\"2\"";
	}
	echo "> <!-- table: MAIN -->\n";
  	echo "  <tr>\n"; // upper areas (logo, title)
	echo "   <td id=\"topleft\"><img src=\"" . $ToRoot . "images/logo.png" . "\" alt=\"steghide logo\"></td>\n"; // top left area (logo)
	echo "   <td id=\"topright\">" . $sectioninfo["title"] . "</td>\n"; // top right area (title)
	echo "  </tr>\n";
	echo "  <tr>\n";
	echo "   <td id=\"conthierachytable\" valign=\"top\">\n";
	phierachytable("    ");
	echo "   </td>\n";
	echo "   <td style=\"padding: 10px\" rowspan=\"2\"> <!-- main text -->\n";
}

/**
 * print everything that should be printed in the body after the real text has ended
 **/
function pBodyEpilogue ($ts_lastchange)
{
	global $ActiveSectionID, $DefSections, $ToRoot;

	echo "   </td> <!-- end of main text -->\n";
	echo "  </tr>\n";

	// print "last change"-area
	echo "  <tr>\n";
	echo "   <td id=\"contlastchange\" height=\"100%\" align=\"center\" valign=\"bottom\">\n" ;
	echo "    <a href=\"http://validator.w3.org/check/referer\"><img border=\"0\" src=\"" . $ToRoot . "images/valid-html401.png\"\n";
	echo "    alt=\"Valid HTML 4.01!\" height=\"31\" width=\"88\"></a>\n";
	echo "    <p>\n";
	echo "    hosted by:<br>\n";
	include "sflogo.html";
	echo "    <p>\n";
	echo "    last change:<br>";
	echo date("F j, Y", $ts_lastchange) . "\n";
	echo "   </td>\n";
	echo "  </tr>\n";

	echo " </table> <!-- table: MAIN -->\n";
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
