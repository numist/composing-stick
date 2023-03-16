<?php
	assert_options(ASSERT_BAIL, true);
	assert('ini_get("short_open_tag")');
?>
<?

$page_start = microtime(true);

assert('php_sapi_name() ==="fpm-fcgi"', "this script requires variables set by an httpd to locate itself");

define("BLOGROOT", "./");

$webroot = ((isset($_SERVER["HTTPS"]) && $_SERVER["HTTPS"] == "on") ?
           "https" : "http")."://".$_SERVER["SERVER_NAME"].dirname($_SERVER["SCRIPT_NAME"]);
if(substr($webroot, -1) != "/") {
	$webroot .= "/";
}
define("WEBROOT", $webroot.BLOGROOT);

require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

function head() { ?>
	<script type="text/javascript">
		jsLoad("<?= WEBROOT ?>js/jquery.ba-dotimeout.min.js");
		jsLoad("<?= WEBROOT ?>js/infScr-1.0.2.min.js");
		jsLoad("<?= WEBROOT ?>js/jk-1.0.4.min.js");
	</script>
	<link href="<?= Link::feed() ?>" type="application/atom+xml" rel="alternate" title="posts" />
	<?
}

function nav() {
	return "<div class=\"nav\"><a href=\"".Link::about()."\">about</a></div>";
}

function title() {
	return "numist the fool";
}

require(BLOGROOT."templates/header.inc");

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

if(count($posts) && count(Post::getPosts(null, 10, $posts[count($posts) - 1]->timestamp(true)))) {
	?><article id="more"><p><a href="<?= $webroot ?>?/page/<?= ($start + $pages) ?>/">more…</a></p></article><?
}

require(BLOGROOT."templates/footer.inc");

?>
<!-- <?= number_format((microtime(true) - $page_start), 4); ?> -->