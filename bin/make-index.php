<?

define("BLOGROOT", dirname($argv[0])."/../");
define("WEBROOT", "./");
require_once(BLOGROOT."lib/intro.inc");
require_once(BLOGROOT."lib/post.class.inc");

assert('php_sapi_name() == "cli"');

function head() {
	return "<link href=\"".Link::feed()."\" type=\"application/atom+xml\" rel=\"alternate\" title=\"posts\" />";
}

function nav() {
	return "<div class=\"nav\"><a href=\"".Link::about()."\">about</a></div>
			<div class=\"nav\" style=\"margin-left: 65px\"><a href=\"".Link::browse()."\">browse</a></div>";
}

function title() {
	return "numist the fool";
}

require(BLOGROOT."templates/header.inc");

$posts = Post::getPosts();


foreach($posts as $post) {
	echo poast($post);
}

require(BLOGROOT."templates/footer.inc");

?>