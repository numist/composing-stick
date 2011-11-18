<?

$preview = $argc > 2;

define("BLOGROOT", dirname($argv[0])."/../");
define("WEBROOT", "../../".($preview ? "../" : ""));

require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

assert('$argc == 2 || $argc == 3');
assert('is_dir(BLOGROOT."data/".$argv[1])');

$post = new Post($argv[1]);

function title() {
	global $post;
	return $post->title();
}

function nav() {
	return "<div class=\"nav\"><a href=\"".Link::index()."\">home</a></div>
			<div class=\"nav\" style=\"margin-left: 65px\"><a href=\"".Link::about()."\">about</a></div>
			<div class=\"nav\" style=\"margin-left: 130px\"><a href=\"".Link::browse()."\">browse</a></div>";
}

function postwidth() {
	global $post;
	return $post->width();
}

require(BLOGROOT."templates/header.inc");

echo poast($post, true);

require(BLOGROOT."templates/footer.inc");


?>
