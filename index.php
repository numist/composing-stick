<?

$page_start = microtime(true);

if(php_sapi_name() ==="cli") {
	trigger_error("this script requires variables set by an httpd to locate itself", E_USER_ERROR);
}

define("BLOGROOT", "./");

$webroot = ((isset($_SERVER["HTTPS"]) && $_SERVER["HTTPS"] == "on") ?
           "https" : "http")."://".$_SERVER["SERVER_NAME"].dirname($_SERVER["SCRIPT_NAME"]);
if(substr($webroot, -1) != "/") {
	$webroot .= "/";
}
define("WEBROOT", $webroot);

require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

function head() { ?>
	<script src="<?= WEBROOT ?>js/jquery.ba-dotimeout.min.js" type="text/javascript" charset="utf-8"></script>
	<script type="text/javascript">
		// http://code.google.com/speed/page-speed/docs/payload.html#DeferLoadingJS
		// Add a script element as a child of the body
		function downloadJSAtOnload() {
		  var element = document.createElement("script");
		  element.src = "<?= WEBROOT ?>js/pax_scroll.js?v=7";
		  document.body.appendChild(element);
		}
		// Check for browser support of event handling capability
		if (window.addEventListener)
		  window.addEventListener("load", downloadJSAtOnload, false);
		else if (window.attachEvent)
		  window.attachEvent("onload", downloadJSAtOnload);
		else 
		  window.onload = downloadJSAtOnload;
	</script>
	<link href="<?= Link::feed() ?>" type="application/atom+xml" rel="alternate" title="posts" />
	<?
}

function nav() {
	return "<div class=\"nav\"><a href=\"".Link::about()."\">about</a></div>
			<div class=\"nav\" style=\"margin-left: 65px\"><a href=\"".Link::browse()."\">browse</a></div>";
}

function title() {
	return "numist the fool";
}

require(BLOGROOT."templates/header.inc");

// array(4) { [0]=> string(8) "/page/1/" [1]=> string(4) "page" [2]=> string(1) "1" [3]=> string(0) "" }
// array(4) { [0]=> string(11) "/pages/1-2/" [1]=> string(5) "pages" [2]=> string(1) "1" [3]=> string(1) "2" }
preg_match("/\/(pages?)\/([0-9]+)-?([0-9]*)\/?$/", $_SERVER["REQUEST_URI"], $matches);
if(count($matches) < 4) {
	$start = $pages = 1;
} else {
	if(strlen($matches[3]) == 0) {
		if ($matches[1] == "page") {
			$start = $matches[2];
			$pages = 1;
		} else {
			assert('$matches[1] == "pages"');
			$start = 1;
			$pages = $matches[2];
		}
	} else {
		$start = $matches[2];
		$pages = $matches[3] - $matches[2] + 1;
	}
}

$posts = Post::getPages($start, $pages);


foreach($posts as $post) {
	echo poast($post);
}

if(count($posts) == $pages * 10) {
	?><article id="more"><p><a href="<?= $webroot ?>?/page/<?= ($start + $pages) ?>/">more…</a></p></article><?
}

require(BLOGROOT."templates/footer.inc");

?>
<!-- <?= number_format((microtime(true) - $page_start), 4); ?> -->